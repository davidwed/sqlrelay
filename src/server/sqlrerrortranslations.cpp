// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrerrortranslationdeclarations.cpp"
	}
#endif

class sqlrerrortranslationsprivate {
	friend class sqlrerrortranslations;
	private:
		const char	*_error;
};

sqlrerrortranslations::sqlrerrortranslations(sqlrservercontroller *cont) :
						sqlrservermodules(cont) {
	pvt=new sqlrerrortranslationsprivate;
	setDebug(cont->getConfig()->getDebugErrorTranslations());
	pvt->_error=NULL;
}

sqlrerrortranslations::~sqlrerrortranslations() {
	delete pvt;
}

void sqlrerrortranslations::loadModule(domnode *parameters) {

	// ignore non-errortranslations
	if (charstring::compare(parameters->getName(),"errortranslation")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading error translation module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the error translation module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
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
						domnode *)=
		(sqlrerrortranslation *(*)(sqlrservercontroller *,
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
	sqlrerrortranslation	*tr=(*newErrorTranslation)(cont,parameters);

#else
	dynamiclib		*dl=NULL;
	sqlrerrortranslation	*tr;
	#include "sqlrerrortranslationassignments.cpp"
	{
		tr=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=tr;
	sqlrmp->dl=dl;
	sqlrmp->module=module;
	blist.append(sqlrmp);
}

bool sqlrerrortranslations::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					int64_t errornumber,
					const char *error,
					uint32_t errorsize,
					int64_t *translatederrornumber,
					stringbuffer *translatederror) {

	pvt->_error=NULL;

	int64_t		temperrornumber1;
	int64_t		temperrornumber2;
	stringbuffer	temperrorstr1;
	stringbuffer	temperrorstr2;
	int64_t		*temperrornumber=&temperrornumber1;
	stringbuffer	*temperrorstr=&temperrorstr1;
	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("running error translation: %s...",
						node->getValue()->module);

		temperrorstr->clear();

		sqlrerrortranslation	*etr=
			(sqlrerrortranslation *)node->getValue()->m;
		if (!etr->run(sqlrcon,sqlrcur,
					errornumber,
					error,
					errorsize,
					temperrornumber,
					temperrorstr)) {
			pvt->_error=etr->getError();
			return false;
		}

		error=temperrorstr->getString();
		errorsize=temperrorstr->getSize();
		errornumber=*temperrornumber;

		temperrorstr=(temperrorstr==&temperrorstr1)?
					&temperrorstr2:&temperrorstr1;
		temperrornumber=(temperrornumber==&temperrornumber1)?
					&temperrornumber2:&temperrornumber1;
	}

	translatederror->append(error,errorsize);
	*translatederrornumber=errornumber;

	return true;
}

const char *sqlrerrortranslations::getError() {
	return pvt->_error;
}
