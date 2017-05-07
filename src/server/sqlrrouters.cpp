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

class sqlrrouterplugin {
	public:
		sqlrrouter	*r;
		dynamiclib	*dl;
};

class sqlrroutersprivate {
	friend class sqlrrouters;
	private:
		const char		*_conid;
		sqlrservercontroller	*_cont;

		singlylinkedlist< sqlrrouterplugin * >	_llist;
};

sqlrrouters::sqlrrouters(sqlrservercontroller *cont) {
	debugFunction();
	pvt=new sqlrroutersprivate;
	pvt->_conid=NULL;
	pvt->_cont=cont;
}

sqlrrouters::~sqlrrouters() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrrouters::load(xmldomnode *parameters,
				const char **connectionids,
				sqlrconnection **connections,
				uint16_t connectioncount) {
	debugFunction();

	unload();

	// run through the router list
	for (xmldomnode *router=parameters->getFirstTagChild();
			!router->isNullNode();
			router=router->getNextTagSibling()) {

		// load router
		loadRouter(router,connectionids,connections,connectioncount);
	}
	return true;
}

void sqlrrouters::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouterplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		sqlrrouterplugin	*sqlrsp=node->getValue();
		delete sqlrsp->r;
		delete sqlrsp->dl;
		delete sqlrsp;
	}
	pvt->_llist.clear();
}

void sqlrrouters::loadRouter(xmldomnode *router,
				const char **connectionids,
				sqlrconnection **connections,
				uint16_t connectioncount) {

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

	if (pvt->_cont->getConfig()->getDebugRouters()) {
		stdoutput.printf("loading router: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the router module
	stringbuffer	modulename;
	modulename.append(pvt->_cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("router_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf(
			"failed to load router module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the router itself
	stringbuffer	functionname;
	functionname.append("new_sqlrrouter_")->append(module);
	sqlrrouter *(*newRouter)(sqlrservercontroller *,
					sqlrrouters *,
					xmldomnode *,
					const char **,
					sqlrconnection **,
					uint16_t)=
			(sqlrrouter *(*)(sqlrservercontroller *,
						sqlrrouters *,
						xmldomnode *,
						const char **,
						sqlrconnection **,
						uint16_t))
				dl->getSymbol(functionname.getString());
	if (!newRouter) {
		stdoutput.printf("failed to load router: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrrouter	*r=(*newRouter)(pvt->_cont,this,router,
							connectionids,
							connections,
							connectioncount);

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
	pvt->_llist.append(sqlrsp);
}

const char *sqlrrouters::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur) {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouterplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		const char	*connectionid=
				node->getValue()->r->route(sqlrcon,sqlrcur);
		if (connectionid) {
			return connectionid;
		}
	}
	return NULL;
}

bool sqlrrouters::routeEntireSession() {
	debugFunction();
	for (singlylinkedlistnode< sqlrrouterplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		if (!node->getValue()->r->routeEntireSession()) {
			return false;
		}
	}
	return true;
}

void sqlrrouters::endSession() {
	// nothing for now, maybe in the future
}

void sqlrrouters::setCurrentConnectionId(const char *conid) {
	pvt->_conid=conid;
}

const char *sqlrrouters::getCurrentConnectionId() {
	return pvt->_conid;
}
