// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <debugprint.h>

#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrprotocoldeclarations.cpp"
	}
#endif

sqlrprotocols::sqlrprotocols(sqlrservercontroller *cont, sqlrpaths *sqlrpth) {
	debugFunction();
	this->cont=cont;
	libexecdir=sqlrpth->getLibExecDir();
}

sqlrprotocols::~sqlrprotocols() {
	debugFunction();
	unloadProtocols();
}

bool sqlrprotocols::loadProtocols() {
	debugFunction();

	unloadProtocols();

	// run through the protocols
	for (linkedlistnode< listenercontainer * >
			*node=cont->cfgfl->getListenerList()->getFirst();
			node; node=node->getNext()) {

		const char	*protocol=node->getValue()->getProtocol();

		// don't load this protocol if it was loaded previously
		sqlrprotocolplugin	*pp=NULL;
		if (protos.getValue(protocol,&pp)) {
			debugPrintf("skipping protocol %s "
					" (already loaded)...\n",protocol);
			continue;
		}

		debugPrintf("loading protocol ...\n");

		// load password encryption
		loadProtocol(protocol);
	}
	return true;
}

void sqlrprotocols::unloadProtocols() {
	debugFunction();
	for (linkedlistnode< dictionarynode< const char *,
						sqlrprotocolplugin * > *>
			*node=protos.getList()->getFirst();
			node; node=node->getNext()) {
		sqlrprotocolplugin	*sqlrpp=node->getValue()->getValue();
		delete sqlrpp->pr;
		delete sqlrpp->dl;
		delete sqlrpp;
	}
	protos.clear();
}

void sqlrprotocols::loadProtocol(const char *module) {

	debugFunction();

	debugPrintf("loading protocol: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the password encryption module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append("sqlrprotocol_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load protocol module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the password encryption itself
	stringbuffer	functionname;
	functionname.append("new_sqlrprotocol_")->append(module);
	sqlrprotocol *(*newProtocol)(sqlrservercontroller *)=
			(sqlrprotocol *(*)(sqlrservercontroller *))
				dl->getSymbol(functionname.getString());
	if (!newProtocol) {
		stdoutput.printf("failed to create protocol: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrprotocol	*pr=(*newProtocol)(cont);

#else

	dynamiclib	*dl=NULL;
	sqlrprotocol	*pr;
	#include "sqlrprotocolassignments.cpp"
	{
		pr=NULL;
	}
#endif

	// add the plugin to the list
	sqlrprotocolplugin	*sqlrpp=new sqlrprotocolplugin;
	sqlrpp->pr=pr;
	sqlrpp->dl=dl;
	protos.setValue(module,sqlrpp);
}

sqlrprotocol *sqlrprotocols::getProtocol(const char *module) {
	debugFunction();
	sqlrprotocolplugin	*pp=NULL;
	if (!protos.getValue(module,&pp)) {
		return NULL;
	}
	return pp->pr;
}
