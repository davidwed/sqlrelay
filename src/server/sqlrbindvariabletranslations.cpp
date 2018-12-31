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
		#include "sqlrbindvariabletranslationdeclarations.cpp"
	}
#endif

class sqlrbindvariabletranslationplugin {
	public:
		sqlrbindvariabletranslation	*bvtr;
		dynamiclib			*dl;
		const char			*module;
};

class sqlrbindvariabletranslationsprivate {
	friend class sqlrbindvariabletranslations;
	private:
		sqlrservercontroller	*_cont;

		bool		_debug;

		singlylinkedlist< sqlrbindvariabletranslationplugin * >	_tlist;

		const char	*_error;
};

sqlrbindvariabletranslations::sqlrbindvariabletranslations(
					sqlrservercontroller *cont) {

	debugFunction();
	pvt=new sqlrbindvariabletranslationsprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugBindVariableTranslations();
	pvt->_error=NULL;
}

sqlrbindvariabletranslations::~sqlrbindvariabletranslations() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrbindvariabletranslations::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the bind variable translation list
	for (domnode *bindvariabletranslation=parameters->getFirstTagChild();
			!bindvariabletranslation->isNullNode();
			bindvariabletranslation=
				bindvariabletranslation->getNextTagSibling()) {

		// load bind variable translation
		loadBindVariableTranslation(bindvariabletranslation);
	}

	return true;
}

void sqlrbindvariabletranslations::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrbindvariabletranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		sqlrbindvariabletranslationplugin	*sqlt=node->getValue();
		delete sqlt->bvtr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_tlist.clear();
}

void sqlrbindvariabletranslations::loadBindVariableTranslation(
					domnode *bindvariabletranslation) {
	debugFunction();

	// ignore non-bindvariabletranslations
	if (charstring::compare(bindvariabletranslation->getName(),
						"bindvariabletranslation")) {
		return;
	}

	// get the bind variable translation name
	const char	*module=
			bindvariabletranslation->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=bindvariabletranslation->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("loading bind variable translation: %s\n",
									module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the bind variable translation module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("bindvariabletranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load bind variable "
					"translation module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the bind variable translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrbindvariabletranslation_")->append(module);
	sqlrbindvariabletranslation *(*newBindVariableTranslation)
					(sqlrservercontroller *,
					sqlrbindvariabletranslations *,
					domnode *)=
		(sqlrbindvariabletranslation *(*)
					(sqlrservercontroller *,
					sqlrbindvariabletranslations *,
					domnode *))
				dl->getSymbol(functionname.getString());
	if (!newBindVariableTranslation) {
		stdoutput.printf("failed to load "
				"bind variable translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrbindvariabletranslation	*bvtr=
		(*newBindVariableTranslation)
			(pvt->_cont,this,bindvariabletranslation);

#else
	dynamiclib			*dl=NULL;
	sqlrbindvariabletranslation	*bvtr;
	#include "sqlrbindvariabletranslationassignments.cpp"
	{
		bvtr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrbindvariabletranslationplugin	*sqlrrstp=
				new sqlrbindvariabletranslationplugin;
	sqlrrstp->bvtr=bvtr;
	sqlrrstp->dl=dl;
	sqlrrstp->module=module;
	pvt->_tlist.append(sqlrrstp);
}

bool sqlrbindvariabletranslations::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {
	debugFunction();

	pvt->_error=NULL;

	for (singlylinkedlistnode< sqlrbindvariabletranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		if (pvt->_debug) {
			stdoutput.printf("\nrunning translation:  %s...\n\n",
						node->getValue()->module);
		}

		if (!node->getValue()->bvtr->run(sqlrcon,sqlrcur)) {
			pvt->_error=node->getValue()->bvtr->getError();
			return false;
		}
	}
	return true;
}

const char *sqlrbindvariabletranslations::getError() {
	return pvt->_error;
}

void sqlrbindvariabletranslations::endTransaction(bool commit) {
	for (singlylinkedlistnode< sqlrbindvariabletranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->bvtr->endTransaction(commit);
	}
}

void sqlrbindvariabletranslations::endSession() {
	for (singlylinkedlistnode< sqlrbindvariabletranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->bvtr->endSession();
	}
}
