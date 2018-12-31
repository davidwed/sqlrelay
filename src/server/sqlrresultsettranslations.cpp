// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrresultsettranslationdeclarations.cpp"
	}
#endif

class sqlrresultsettranslationplugin {
	public:
		sqlrresultsettranslation	*rstr;
		dynamiclib			*dl;
		const char			*module;
};

class sqlrresultsettranslationsprivate {
	friend class sqlrresultsettranslations;
	private:
		sqlrservercontroller	*_cont;

		bool		_debug;

		singlylinkedlist< sqlrresultsettranslationplugin * >	_tlist;

		const char	*_error;
};

sqlrresultsettranslations::sqlrresultsettranslations(
					sqlrservercontroller *cont) {

	debugFunction();
	pvt=new sqlrresultsettranslationsprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugResultSetTranslations();
	pvt->_error=NULL;
}

sqlrresultsettranslations::~sqlrresultsettranslations() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrresultsettranslations::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the result set translation list
	for (domnode *resultsettranslation=parameters->getFirstTagChild();
			!resultsettranslation->isNullNode();
			resultsettranslation=
				resultsettranslation->getNextTagSibling()) {

		// load result set translation
		loadResultSetTranslation(resultsettranslation);
	}

	return true;
}

void sqlrresultsettranslations::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrresultsettranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		sqlrresultsettranslationplugin	*sqlt=node->getValue();
		delete sqlt->rstr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_tlist.clear();
}

void sqlrresultsettranslations::loadResultSetTranslation(
				domnode *resultsettranslation) {
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

	if (pvt->_debug) {
		stdoutput.printf("loading result set translation: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("resultsettranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"result set translation module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the result set translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrresultsettranslation_")->append(module);
	sqlrresultsettranslation *(*newResultSetTranslation)
					(sqlrservercontroller *,
					sqlrresultsettranslations *,
					domnode *)=
		(sqlrresultsettranslation *(*)
					(sqlrservercontroller *,
					sqlrresultsettranslations *,
					domnode *))
				dl->getSymbol(functionname.getString());
	if (!newResultSetTranslation) {
		stdoutput.printf("failed to load "
				"result set translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrresultsettranslation	*rstr=
		(*newResultSetTranslation)
			(pvt->_cont,this,resultsettranslation);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsettranslation	*rstr;
	#include "sqlrresultsettranslationassignments.cpp"
	{
		rstr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrresultsettranslationplugin	*sqlrrstp=
				new sqlrresultsettranslationplugin;
	sqlrrstp->rstr=rstr;
	sqlrrstp->dl=dl;
	sqlrrstp->module=module;
	pvt->_tlist.append(sqlrrstp);
}

bool sqlrresultsettranslations::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *fieldname,
						uint32_t fieldindex,
						const char **field,
						uint64_t *fieldlength) {
	debugFunction();

	pvt->_error=NULL;

	for (singlylinkedlistnode< sqlrresultsettranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		if (pvt->_debug) {
			stdoutput.printf("\nrunning translation:  %s...\n\n",
						node->getValue()->module);
		}

		if (!node->getValue()->rstr->run(sqlrcon,sqlrcur,
						fieldname,fieldindex,
						field,fieldlength)) {
			pvt->_error=node->getValue()->rstr->getError();
			return false;
		}
	}
	return true;
}

const char *sqlrresultsettranslations::getError() {
	return pvt->_error;
}

void sqlrresultsettranslations::endTransaction(bool commit) {
	for (singlylinkedlistnode< sqlrresultsettranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->rstr->endTransaction(commit);
	}
}

void sqlrresultsettranslations::endSession() {
	for (singlylinkedlistnode< sqlrresultsettranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->rstr->endSession();
	}
}
