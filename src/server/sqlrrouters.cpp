// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrrouterdeclarations.cpp"
	}
#endif

sqlrrouters::sqlrrouters(sqlrpaths *sqlrpth) {
	debugFunction();
	libexecdir=sqlrpth->getLibExecDir();
}

sqlrrouters::~sqlrrouters() {
	debugFunction();
	unloadRouters();
}

bool sqlrrouters::loadRouters(xmldomnode *parameters) {
	debugFunction();

	unloadRouters();

	// run through the router list
	for (xmldomnode *router=parameters->getFirstTagChild();
			!router->isNullNode();
			router=router->getNextTagSibling()) {

		debugPrintf("loading router ...\n");

		// load router
		loadRouter(router);
	}
	return true;
}

void sqlrrouters::unloadRouters() {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouterplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		sqlrrouterplugin	*sqlrsp=node->getValue();
		delete sqlrsp->r;
		delete sqlrsp->dl;
		delete sqlrsp;
	}
	llist.clear();
}

void sqlrrouters::loadRouter(xmldomnode *router) {

	debugFunction();

	// ignore non-routers
	if (charstring::compare(router->getName(),"router")) {
		return;
	}

	// get the router name
	const char	*module=router->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=router->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	debugPrintf("loading router: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the router module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append(SQLR);
	modulename.append("router_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf(
			"failed to load router module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the router itself
	stringbuffer	functionname;
	functionname.append("new_sqlrrouter_")->append(module);
	sqlrrouter *(*newRouter)(xmldomnode *)=
			(sqlrrouter *(*)(xmldomnode *))
				dl->getSymbol(functionname.getString());
	if (!newRouter) {
		stdoutput.printf("failed to create router: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrrouter	*r=(*newRouter)(router);

#else

	dynamiclib	*dl=NULL;
	sqlrrouter	*r;
	#include "sqlrrouterassignments.cpp"
	{
		r=NULL;
	}
#endif

	// add the plugin to the list
	sqlrrouterplugin	*sqlrsp=new sqlrrouterplugin;
	sqlrsp->r=r;
	sqlrsp->dl=dl;
	llist.append(sqlrsp);
}

void sqlrrouters::initRouters(sqlrserverconnection *sqlrcon) {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouterplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->r->init(sqlrcon);
	}
}

bool sqlrrouters::route(sqlrserverconnection *sqlrcon) {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouterplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		if (!node->getValue()->r->route(sqlrcon)) {
			return false;
		}
	}
	return true;
}
