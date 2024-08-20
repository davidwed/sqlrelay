// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrresultsettranslationdeclarations.cpp"
	}
#endif

class sqlrresultsettranslationsprivate {
	friend class sqlrresultsettranslations;
	private:
		const char	*_error;
};

sqlrresultsettranslations::sqlrresultsettranslations(
					sqlrservercontroller *cont) :
					sqlrservermodules(cont) {

	pvt=new sqlrresultsettranslationsprivate;
	setDebug(cont->getConfig()->getDebugResultSetTranslations());
	pvt->_error=NULL;
}

sqlrresultsettranslations::~sqlrresultsettranslations() {
	delete pvt;
}

void sqlrresultsettranslations::loadModule(domnode *parameters) {

	// ignore non-resultsettranslations
	if (charstring::compare(parameters->getName(),"resultsettranslation")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading result set translation: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
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
					domnode *)=
		(sqlrresultsettranslation *(*)
					(sqlrservercontroller *,
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
		(*newResultSetTranslation)(cont,parameters);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsettranslation	*rstr;
	#include "sqlrresultsettranslationassignments.cpp"
	{
		rstr=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=rstr;
	sqlrmp->dl=dl;
	sqlrmp->module=module;
	blist.append(sqlrmp);
}

bool sqlrresultsettranslations::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *fieldname,
						uint32_t fieldindex,
						const char **field,
						uint64_t *fieldsize) {

	pvt->_error=NULL;

	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("running result set translation:  %s...",
						node->getValue()->module);

		sqlrresultsettranslation	*rstr=
			(sqlrresultsettranslation *)node->getValue()->m;
		if (!rstr->run(sqlrcon,sqlrcur,
					fieldname,fieldindex,
					field,fieldsize)) {
			pvt->_error=rstr->getError();
			return false;
		}
	}
	return true;
}

const char *sqlrresultsettranslations::getError() {
	return pvt->_error;
}
