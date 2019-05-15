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
		#include "sqlrresultsetrowtranslationdeclarations.cpp"
	}
#endif

class sqlrresultsetrowtranslationplugin {
	public:
		sqlrresultsetrowtranslation	*rstr;
		dynamiclib			*dl;
		const char			*module;
};

class sqlrresultsetrowtranslationsprivate {
	friend class sqlrresultsetrowtranslations;
	private:
		sqlrservercontroller	*_cont;

		bool		_debug;

		singlylinkedlist< sqlrresultsetrowtranslationplugin * >	_tlist;

		const char	*_error;
};

sqlrresultsetrowtranslations::sqlrresultsetrowtranslations(
						sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrresultsetrowtranslationsprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugResultSetRowTranslations();
	pvt->_error=NULL;
}

sqlrresultsetrowtranslations::~sqlrresultsetrowtranslations() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrresultsetrowtranslations::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the result set translation list
	for (domnode *resultsetrowtranslation=parameters->getFirstTagChild();
			!resultsetrowtranslation->isNullNode();
			resultsetrowtranslation=
				resultsetrowtranslation->getNextTagSibling()) {

		// load result set translation
		loadResultSetRowTranslation(resultsetrowtranslation);
	}

	return true;
}

void sqlrresultsetrowtranslations::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrresultsetrowtranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		sqlrresultsetrowtranslationplugin	*sqlt=node->getValue();
		delete sqlt->rstr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_tlist.clear();
}

void sqlrresultsetrowtranslations::loadResultSetRowTranslation(
				domnode *resultsetrowtranslation) {
	debugFunction();

	// ignore non-resultsetrowtranslations
	if (charstring::compare(resultsetrowtranslation->getName(),
						"resultsetrowtranslation")) {
		return;
	}

	// get the result set translation name
	const char	*module=
			resultsetrowtranslation->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=resultsetrowtranslation->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("loading result set row translation: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("resultsetrowtranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"result set row translation module: %s\n",
				module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the result set translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrresultsetrowtranslation_")->append(module);
	sqlrresultsetrowtranslation *(*newResultSetTranslation)
					(sqlrservercontroller *,
					sqlrresultsetrowtranslations *,
					domnode *)=
		(sqlrresultsetrowtranslation *(*)
					(sqlrservercontroller *,
					sqlrresultsetrowtranslations *,
					domnode *))
				dl->getSymbol(functionname.getString());
	if (!newResultSetTranslation) {
		stdoutput.printf("failed to load "
				"result set row translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrresultsetrowtranslation	*rstr=
		(*newResultSetTranslation)
			(pvt->_cont,this,resultsetrowtranslation);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsetrowtranslation	*rstr;
	#include "sqlrresultsetrowtranslationassignments.cpp"
	{
		rstr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrresultsetrowtranslationplugin	*sqlrrstp=
				new sqlrresultsetrowtranslationplugin;
	sqlrrstp->rstr=rstr;
	sqlrrstp->dl=dl;
	sqlrrstp->module=module;
	pvt->_tlist.append(sqlrrstp);
}

bool sqlrresultsetrowtranslations::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char ***fields,
						uint64_t **fieldlengths) {
	debugFunction();

	pvt->_error=NULL;

	for (singlylinkedlistnode< sqlrresultsetrowtranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		if (pvt->_debug) {
			stdoutput.printf("\nrunning translation:  %s...\n\n",
						node->getValue()->module);
		}

		if (!node->getValue()->rstr->run(sqlrcon,sqlrcur,
						colcount,fieldnames,
						fields,fieldlengths)) {
			pvt->_error=node->getValue()->rstr->getError();
			return false;
		}
	}
	return true;
}

const char *sqlrresultsetrowtranslations::getError() {
	return pvt->_error;
}

void sqlrresultsetrowtranslations::endTransaction(bool commit) {
	for (singlylinkedlistnode< sqlrresultsetrowtranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->rstr->endTransaction(commit);
	}
}

void sqlrresultsetrowtranslations::endSession() {
	for (singlylinkedlistnode< sqlrresultsetrowtranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->rstr->endSession();
	}
}
