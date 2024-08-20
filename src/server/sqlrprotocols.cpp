// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrprotocoldeclarations.cpp"
	}
#endif

class sqlrprotocolsprivate {
	friend class sqlrprotocols;
	private:
		dictionary< uint16_t , sqlrmoduleplugin * >	_protos;
};

sqlrprotocols::sqlrprotocols(sqlrservercontroller *cont) :
						sqlrservermodules(cont) {
	pvt=new sqlrprotocolsprivate;
}

sqlrprotocols::~sqlrprotocols() {
	delete pvt;
}

bool sqlrprotocols::load(domnode *parameters) {

	unload();

	// run through the module tags
	uint16_t	i=0;
	for (domnode *moduledata=parameters->getFirstTagChild();
				!moduledata->isNullNode();
				moduledata=moduledata->getNextTagSibling()) {

		// skip disabled moudles
		if (isModuleDisabled(moduledata)) {
			continue;
		}

		// load the module
		loadModule(moduledata,i);

		// next...
		i++;
	}
	return true;
}

void sqlrprotocols::loadModule(domnode *parameters, uint16_t index) {

	// ignore any non-listener entries
	if (charstring::compare(parameters->getName(),"listener")) {
		return;
	}

	// get the protocol name
	const char	*module=parameters->getAttributeValue("protocol");

	debugWrite("loading protocol module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the protocol module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("protocol_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load protocol module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the protocol itself
	stringbuffer	functionname;
	functionname.append("new_sqlrprotocol_")->append(module);
	sqlrprotocol *(*newProtocol)(sqlrservercontroller *,
					domnode *)=
			(sqlrprotocol *(*)(sqlrservercontroller *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newProtocol) {
		stdoutput.printf("failed to load protocol: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrprotocol	*pr=(*newProtocol)(cont,parameters);

#else

	dynamiclib	*dl=NULL;
	sqlrprotocol	*pr;
	#include "sqlrprotocolassignments.cpp"
	{
		pr=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=pr;
	sqlrmp->dl=dl;
	blist.append(sqlrmp);
	pvt->_protos.setValue(index,sqlrmp);
}

sqlrprotocol *sqlrprotocols::getProtocol(uint16_t index) {
	sqlrmoduleplugin	*pp=NULL;
	if (!pvt->_protos.getValue(index,&pp)) {
		return NULL;
	}
	return (sqlrprotocol *)pp->m;
}
