// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <debugprint.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

sqlrresultsettranslations::sqlrresultsettranslations() {
	debugFunction();
	xmld=NULL;
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

	debugPrintf("loading result set translation: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(LIBEXECDIR);
	modulename.append("/sqlrresultsettranslation_");
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
	functionname.append("new_")->append(module);
	sqlrresultsettranslation *(*newResultSetTranslation)
		(sqlrresultsettranslations *, xmldomnode *)=
		(sqlrresultsettranslation *(*)
		(sqlrresultsettranslations *, xmldomnode *))
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
		(*newResultSetTranslation)(this,resultsettranslation);

#else

	dynamiclib			*dl=NULL;
	sqlrresultsettranslation	*rstr=NULL;
#endif

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
						uint16_t fieldindex,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength) {
	debugFunction();

	for (singlylinkedlistnode< sqlrresultsettranslationplugin * > *node=
						tlist.getFirst();
						node; node=node->getNext()) {
		if (!node->getValue()->rstr->run(sqlrcon,sqlrcur,
						fieldindex,
						field,fieldlength,
						newfield,newfieldlength)) {
			return false;
		}
	}
	return true;
}
