// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrresultsetrowtranslationdeclarations.cpp"
	}
#endif

class sqlrresultsetrowtranslationsprivate {
	friend class sqlrresultsetrowtranslations;
	private:
		const char	*_error;
};

sqlrresultsetrowtranslations::sqlrresultsetrowtranslations(
						sqlrservercontroller *cont) :
						sqlrservermodules(cont) {
	pvt=new sqlrresultsetrowtranslationsprivate;
	setDebug(cont->getConfig()->getDebugResultSetRowTranslations());
	pvt->_error=NULL;
}

sqlrresultsetrowtranslations::~sqlrresultsetrowtranslations() {
	delete pvt;
}

void sqlrresultsetrowtranslations::loadModule(domnode *parameters) {

	// ignore non-resultsetrowtranslations
	if (charstring::compare(parameters->getName(),
					"resultsetrowtranslation")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading result set row translation: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the result set translation module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
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
					domnode *)=
		(sqlrresultsetrowtranslation *(*)
					(sqlrservercontroller *,
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
		(*newResultSetTranslation)(cont,parameters);

#else
	dynamiclib			*dl=NULL;
	sqlrresultsetrowtranslation	*rstr;
	#include "sqlrresultsetrowtranslationassignments.cpp"
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

bool sqlrresultsetrowtranslations::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char ***fields,
						uint64_t **fieldsizes) {

	pvt->_error=NULL;

	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("running result set row translation:  %s...",
						node->getValue()->module);

		sqlrresultsetrowtranslation	*rstr=
			(sqlrresultsetrowtranslation *)node->getValue()->m;
		if (!rstr->run(sqlrcon,sqlrcur,
					colcount,fieldnames,
					fields,fieldsizes)) {
			pvt->_error=rstr->getError();
			return false;
		}
	}
	return true;
}

const char *sqlrresultsetrowtranslations::getError() {
	return pvt->_error;
}
