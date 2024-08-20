// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrauthdeclarations.cpp"
	}
#endif

sqlrauths::sqlrauths(sqlrservercontroller *cont) : sqlrservermodules(cont) {
	pvt=NULL;
	setDebug(cont->getConfig()->getDebugAuths());
}

sqlrauths::~sqlrauths() {
}

bool sqlrauths::load(domnode *parameters) {

	unload();

	// run through the module tags
	for (domnode *moduledata=parameters->getFirstTagChild();
				!moduledata->isNullNode();
				moduledata=moduledata->getNextTagSibling()) {

		// skip disabled modules
		if (isModuleDisabled(moduledata)) {
			continue;
		}

		// load the module
		loadModule(moduledata);
	}
	return true;
}

void sqlrauths::loadModule(domnode *parameters) {

	// ignore non-auths
	if (charstring::compare(parameters->getName(),"auth")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading auth module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the password encryption module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("auth_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load auth module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the password encryption itself
	stringbuffer	functionname;
	functionname.append("new_sqlrauth_")->append(module);
	sqlrauth *(*newAuth)(sqlrservercontroller *,
					domnode *)=
			(sqlrauth *(*)(sqlrservercontroller *,
					domnode *))
				dl->getSymbol(functionname.getString());
	if (!newAuth) {
		stdoutput.printf("failed to load auth: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrauth	*au=(*newAuth)(cont,parameters);

#else

	dynamiclib	*dl=NULL;
	sqlrauth	*au;
	#include "sqlrauthassignments.cpp"
	{
		au=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=au;
	sqlrmp->dl=dl;
	sqlrmp->module=module;
	blist.append(sqlrmp);
}

const char *sqlrauths::auth(sqlrcredentials *cred) {
	if (!cred) {
		return NULL;
	}
	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("running auth: %s...",node->getValue()->module);
		
		sqlrauth	*m=(sqlrauth *)node->getValue()->m;
		const char	*autheduser=m->auth(cred);
		if (autheduser) {
			return autheduser;
		}
	}
	return NULL;
}
