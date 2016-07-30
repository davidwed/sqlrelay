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
		#include "sqlrroutedeclarations.cpp"
	}
#endif

sqlrroutes::sqlrroutes(sqlrpaths *sqlrpth) {
	debugFunction();
	libexecdir=sqlrpth->getLibExecDir();
}

sqlrroutes::~sqlrroutes() {
	debugFunction();
	unloadRoutes();
}

bool sqlrroutes::loadRoutes(xmldomnode *parameters) {
	debugFunction();

	unloadRoutes();

	// run through the route list
	for (xmldomnode *route=parameters->getFirstTagChild();
			!route->isNullNode();
			route=route->getNextTagSibling()) {

		debugPrintf("loading route ...\n");

		// load route
		loadRoute(route);
	}
	return true;
}

void sqlrroutes::unloadRoutes() {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouteplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		sqlrrouteplugin	*sqlrsp=node->getValue();
		delete sqlrsp->r;
		delete sqlrsp->dl;
		delete sqlrsp;
	}
	llist.clear();
}

void sqlrroutes::loadRoute(xmldomnode *route) {

	debugFunction();

	// ignore non-routes
	if (charstring::compare(route->getName(),"route")) {
		return;
	}

	// get the route name
	const char	*module=route->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=route->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	debugPrintf("loading route: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the route module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append(SQLR);
	modulename.append("route_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf(
			"failed to load route module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the route itself
	stringbuffer	functionname;
	functionname.append("new_sqlrroute_")->append(module);
	sqlrroute *(*newRoute)(xmldomnode *)=
			(sqlrroute *(*)(xmldomnode *))
				dl->getSymbol(functionname.getString());
	if (!newRoute) {
		stdoutput.printf("failed to create route: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrroute	*r=(*newRoute)(route);

#else

	dynamiclib	*dl=NULL;
	sqlrroute	*s;
	#include "sqlrrouteassignments.cpp"
	{
		r=NULL;
	}
#endif

	// add the plugin to the list
	sqlrrouteplugin	*sqlrsp=new sqlrrouteplugin;
	sqlrsp->r=r;
	sqlrsp->dl=dl;
	llist.append(sqlrsp);
}

void sqlrroutes::initRoutes(sqlrserverconnection *sqlrcon) {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouteplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->r->init(sqlrcon);
	}
}

bool sqlrroutes::route(sqlrserverconnection *sqlrcon) {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouteplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		if (!node->getValue()->r->route(sqlrcon)) {
			return false;
		}
	}
	return true;
}
