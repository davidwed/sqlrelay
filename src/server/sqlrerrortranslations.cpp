// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrerrortranslationdeclarations.cpp"
	}
#endif

class sqlrerrortranslationplugin {
	public:
		sqlrerrortranslation	*etr;
		dynamiclib		*dl;
		const char		*module;
};

class sqlrerrortranslationsprivate {
	friend class sqlrerrortranslations;
	private:
		sqlrservercontroller	*_cont;
		
		bool		_debug;

		singlylinkedlist< sqlrerrortranslationplugin * >	_tlist;

		const char	*_error;
};

sqlrerrortranslations::sqlrerrortranslations(sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrerrortranslationsprivate;
	pvt->_cont=cont;
	pvt->_debug=cont->getConfig()->getDebugTranslations();
	pvt->_error=NULL;
}

sqlrerrortranslations::~sqlrerrortranslations() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrerrortranslations::load(domnode *parameters) {
	debugFunction();

	unload();

	// run through the error translation list
	for (domnode *errortranslation=parameters->getFirstTagChild();
		!errortranslation->isNullNode();
		errortranslation=errortranslation->getNextTagSibling()) {

		// load error translation
		loadErrorTranslation(errortranslation);
	}

	return true;
}

void sqlrerrortranslations::unload() {
	debugFunction();
	for (listnode< sqlrerrortranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		sqlrerrortranslationplugin	*sqlt=node->getValue();
		delete sqlt->etr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_tlist.clear();
}

void sqlrerrortranslations::loadErrorTranslation(domnode *errortranslation) {
	debugFunction();

	// ignore non-errortranslations
	if (charstring::compare(errortranslation->getName()
					,"errortranslation")) {
		return;
	}

	// get the error translation name
	const char	*module=errortranslation->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=errortranslation->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	if (pvt->_debug) {
		stdoutput.printf("loading error "
					"translation module: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the error translation module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("errortranslation_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load error "
				"translation module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the error translation itself
	stringbuffer	functionname;
	functionname.append("new_sqlrerrortranslation_")->append(module);
	sqlrerrortranslation *(*newErrorTranslation)(sqlrservercontroller *,
						sqlrerrortranslations *,
						domnode *)=
		(sqlrerrortranslation *(*)(sqlrservercontroller *,
						sqlrerrortranslations *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newErrorTranslation) {
		stdoutput.printf("failed to load error "
					"translation: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrerrortranslation	*tr=
		(*newErrorTranslation)
			(pvt->_cont,this,errortranslation);

#else
	dynamiclib		*dl=NULL;
	sqlrerrortranslation	*tr;
	#include "sqlrerrortranslationassignments.cpp"
	{
		tr=NULL;
	}
#endif

	if (pvt->_debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrerrortranslationplugin	*sqltp=new sqlrerrortranslationplugin;
	sqltp->etr=tr;
	sqltp->dl=dl;
	sqltp->module=module;
	pvt->_tlist.append(sqltp);
}

bool sqlrerrortranslations::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrparser *sqlrp,
					int64_t errornumber,
					const char *error,
					uint32_t errorlength,
					int64_t *translatederrornumber,
					stringbuffer *translatederror) {
	debugFunction();

	pvt->_error=NULL;

	for (listnode< sqlrerrortranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {

		if (pvt->_debug) {
			stdoutput.printf("\nrunning translation:  %s...\n\n",
						node->getValue()->module);
		}

		if (!node->getValue()->etr->run(sqlrcon,sqlrcur,
							errornumber,
							error,
							errorlength,
							translatederrornumber,
							translatederror)) {
			pvt->_error=node->getValue()->etr->getError();
			return false;
		}
	}
	return true;
}

const char *sqlrerrortranslations::getError() {
	return pvt->_error;
}

void sqlrerrortranslations::endTransaction(bool commit) {
	for (listnode< sqlrerrortranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->etr->endTransaction(commit);
	}
}

void sqlrerrortranslations::endSession() {
	for (listnode< sqlrerrortranslationplugin * > *node=
						pvt->_tlist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->etr->endSession();
	}
}
