// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrrouterdeclarations.cpp"
	}
#endif

class sqlrroutersprivate {
	friend class sqlrrouters;
	private:
		sqlrconnection		**_conns;
		const char		**_connids;
		uint16_t		_conncount;
		const char		*_connid;
};

sqlrrouters::sqlrrouters(sqlrservercontroller *cont,
				sqlrconnection **connections,
				const char **connectionids,
				uint16_t connectioncount) :
				sqlrservermodules(cont) {
	pvt=new sqlrroutersprivate;
	setDebug(cont->getConfig()->getDebugRouters());
	pvt->_conns=connections;
	pvt->_connids=connectionids;
	pvt->_conncount=connectioncount;
	pvt->_connid=NULL;
}

sqlrrouters::~sqlrrouters() {
	delete pvt;
}

void sqlrrouters::loadModule(domnode *parameters) {

	// ignore non-routers
	if (charstring::compare(parameters->getName(),"router")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading router module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the router module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
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
					domnode *)=
			(sqlrrouter *(*)(sqlrservercontroller *,
						sqlrrouters *,
						domnode *))
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
	sqlrrouter	*r=(*newRouter)(cont,this,parameters);

#else

	dynamiclib	*dl=NULL;
	sqlrrouter	*r;
	#include "sqlrrouterassignments.cpp"
	{
		r=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=r;
	sqlrmp->dl=dl;
	sqlrmp->module=module;
	blist.append(sqlrmp);
}

const char *sqlrrouters::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char **err,
					int64_t *errn) {

	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("checking route: %s...",node->getValue()->module);

		sqlrrouter	*r=(sqlrrouter *)node->getValue()->m;
		const char	*connid=r->route(sqlrcon,sqlrcur,err,errn);
		if (connid) {
			return connid;
		}
	}
	return NULL;
}

bool sqlrrouters::routeEntireSession() {
	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {
		sqlrrouter	*r=(sqlrrouter *)node->getValue()->m;
		if (!r->routeEntireSession()) {
			return false;
		}
	}
	return true;
}

void sqlrrouters::setCurrentConnectionId(const char *connid) {
	pvt->_connid=connid;
}

const char *sqlrrouters::getCurrentConnectionId() {
	return pvt->_connid;
}

const char **sqlrrouters::getConnectionIds() {
	return pvt->_connids;
}

sqlrconnection **sqlrrouters::getConnections() {
	return pvt->_conns;
}

uint16_t sqlrrouters::getConnectionCount() {
	return pvt->_conncount;
}
