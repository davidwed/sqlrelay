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

sqlrrouters::sqlrrouters(sqlrpaths *sqlrpth, bool debug) {
	debugFunction();
	libexecdir=sqlrpth->getLibExecDir();
	this->debug=debug;
}

sqlrrouters::~sqlrrouters() {
	debugFunction();
	unload();
}

bool sqlrrouters::load(xmldomnode *parameters) {
	debugFunction();

	unload();

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

void sqlrrouters::unload() {
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
	sqlrrouter *(*newRouter)(xmldomnode *,bool)=
			(sqlrrouter *(*)(xmldomnode *,bool))
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
	sqlrrouter	*r=(*newRouter)(router,debug);

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

const char *sqlrrouters::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur) {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouterplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		const char	*connectionid=
				node->getValue()->r->route(sqlrcon,sqlrcur);
		if (connectionid) {
			return connectionid;
		}
	}
	return NULL;
}
