// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrmoduledatadeclarations.cpp"
	}
#endif

class sqlrdatabaseobject {
	public:
		const char	*database;
		const char	*schema;
		const char	*object;
		const char	*dependency;
};

class sqlrmoduledatasprivate {
	friend class sqlrmoduledatas;
	private:
		dictionary< const char *, sqlrmoduledata * >	_mdict;
};

sqlrmoduledatas::sqlrmoduledatas(sqlrservercontroller *cont) :
						sqlrservermodules(cont) {
	pvt=new sqlrmoduledatasprivate;
	setDebug(cont->getConfig()->getDebugModuleDatas());
}

sqlrmoduledatas::~sqlrmoduledatas() {
	delete pvt;
}

void sqlrmoduledatas::loadModule(domnode *parameters) {

	// ignore non-moduledatas
	if (charstring::compare(parameters->getName(),"moduledata")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading moduledata module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the moduledata module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("moduledata_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load "
				"moduledata module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the moduledata itself
	stringbuffer	functionname;
	functionname.append("new_sqlrmoduledata_")->append(module);
	sqlrmoduledata *(*newModuleData)(domnode *)=
		(sqlrmoduledata *(*)(domnode *))
				dl->getSymbol(functionname.getString());
	if (!newModuleData) {
		stdoutput.printf("failed to load moduledata: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrmoduledata	*md=(*newModuleData)(parameters);

#else
	dynamiclib	*dl=NULL;
	sqlrmoduledata	*md;
	#include "sqlrmoduledataassignments.cpp"
	{
		md=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlmp=new sqlrmoduleplugin;
	sqlmp->m=md;
	sqlmp->dl=dl;
	sqlmp->module=module;
	blist.append(sqlmp);
	pvt->_mdict.setValue(md->getId(),md);
}

sqlrmoduledata *sqlrmoduledatas::getModuleData(const char *id) {
	return pvt->_mdict.getValue(id);
}

void sqlrmoduledatas::closeResultSet(sqlrservercursor *sqlrcur) {
	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {
		sqlrmoduledata	*md=(sqlrmoduledata *)node->getValue()->m;
		md->closeResultSet(sqlrcur);
	}
}
