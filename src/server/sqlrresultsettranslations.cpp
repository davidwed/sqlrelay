// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <debugprint.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrresultsettranslationdeclarations.cpp"
	}
#endif

sqlrresultsettranslations::sqlrresultsettranslations(sqlrpaths *sqlrpth,
								bool debug) {
	debugFunction();
	xmld=NULL;
	libexecdir=sqlrpth->getLibExecDir();
	this->debug=debug;
}

sqlrresultsettranslations::~sqlrresultsettranslations() {
	debugFunction();
	unloadResultSetTranslations();
	delete xmld;
}

bool sqlrresultsettranslations::loadResultSetTranslations(
					const char *resultsettranslations) {
	debugFunction();

	unloadResultSetTranslations();

	// create the parser
	delete xmld;
	xmld=new xmldom();

	// parse the result set translations
	if (!xmld->parseString(resultsettranslations)) {
		return false;
	}

	// get the result set translations tag
	xmldomnode	*resultsettranslationsnode=
			xmld->getRootNode()->getFirstTagChild(
						"resultsettranslations");
	if (resultsettranslationsnode->isNullNode()) {
		return false;
	}

	// run through the result set translation list
	for (xmldomnode *resultsettranslation=
				resultsettranslationsnode->getFirstTagChild();
			!resultsettranslation->isNullNode();
			resultsettranslation=
				resultsettranslation->getNextTagSibling()) {

		// load result set translation
		loadResultSetTranslation(resultsettranslation);
	}

	return true;
}

void sqlrresultsettranslations::unloadResultSetTranslations() {
	debugFunction();
	for (singlylinkedlistnode< sqlrresultsettranslationplugin * > *node=
						tlist.getFirst();
						node; node=node->getNext()) {
		sqlrresultsettranslationplugin	*sqlt=node->getValue();
		delete sqlt->rstr;
		delete sqlt->dl;
		delete sqlt;
	}
	tlist.clear();
}

void sqlrresultsettranslations::loadResultSetTranslation(
				xmldomnode *resultsettranslation) {
	debugFunction();

	// ignore non-resultsettranslations
	if (charstring::compare(resultsettranslation->getName(),
						"resultsettranslation")) {
		return;
	}

	// get the result set translation name
	const char	*module=
			resultsettranslation->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=resultsettranslation->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	if (debug) {
		stdoutput.printf("loading result set translation: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append(SQLR);
	modulename.append("resultsettranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"result set translation module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the result set translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrresultsettranslation_")->append(module);
	sqlrresultsettranslation *(*newResultSetTranslation)
		(sqlrresultsettranslations *, xmldomnode *, bool)=
		(sqlrresultsettranslation *(*)
		(sqlrresultsettranslations *, xmldomnode *, bool))
				dl->getSymbol(functionname.getString());
	if (!newResultSetTranslation) {
		stdoutput.printf("failed to create "
				"result set translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrresultsettranslation	*rstr=
		(*newResultSetTranslation)(this,resultsettranslation,debug);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsettranslation	*rstr;
	#include "sqlrresultsettranslationassignments.cpp"
	{
		rstr=NULL;
	}
#endif

	if (debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrresultsettranslationplugin	*sqlrrstp=
				new sqlrresultsettranslationplugin;
	sqlrrstp->rstr=rstr;
	sqlrrstp->dl=dl;
	tlist.append(sqlrrstp);
}

bool sqlrresultsettranslations::runResultSetTranslations(
						sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *fieldname,
						uint16_t fieldindex,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength) {
	debugFunction();

	*newfield=field;
	*newfieldlength=fieldlength;

	for (singlylinkedlistnode< sqlrresultsettranslationplugin * > *node=
						tlist.getFirst();
						node; node=node->getNext()) {
		if (debug) {
			stdoutput.printf(
				"\nrunning result set translation...\n\n");
		}

		if (!node->getValue()->rstr->run(sqlrcon,sqlrcur,
						fieldname,fieldindex,
						*newfield,*newfieldlength,
						newfield,newfieldlength)) {
			return false;
		}
	}
	return true;
}
