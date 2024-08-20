// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrquerydeclarations.cpp"
	}
#endif

sqlrqueries::sqlrqueries(sqlrservercontroller *cont) : sqlrservermodules(cont) {
	pvt=NULL;
}

sqlrqueries::~sqlrqueries() {
}

void sqlrqueries::loadModule(domnode *parameters) {

	// ignore non-queries
	if (charstring::compare(parameters->getName(),"query")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading query module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the query module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("query_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load query module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the query itself
	stringbuffer	functionname;
	functionname.append("new_sqlrquery_")->append(module);
	sqlrquery *(*newQuery)(sqlrservercontroller *,
					domnode *)=
			(sqlrquery *(*)(sqlrservercontroller *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newQuery) {
		stdoutput.printf("failed to load query: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrquery	*qr=(*newQuery)(cont,parameters);

#else

	dynamiclib	*dl=NULL;
	sqlrquery	*qr;
	#include "sqlrqueryassignments.cpp"
	{
		qr=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=qr;
	sqlrmp->dl=dl;
	sqlrmp->module=module;
	blist.append(sqlrmp);
}

sqlrquerycursor *sqlrqueries::match(sqlrserverconnection *sqlrcon,
					const char *querystring,
					uint32_t querysize,
					uint16_t id) {

	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("matching against query: %s...",
					node->getValue()->module);

		sqlrquery	*qr=(sqlrquery *)node->getValue()->m;
		if (qr->match(querystring,querysize)) {
			return qr->newCursor(sqlrcon,id);
		}
	}
	return NULL;
}
