// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrtriggerdeclarations.cpp"
	}
#endif

sqlrtriggers::sqlrtriggers(sqlrservercontroller *cont) :
					sqlrservermodules(cont) {
	pvt=NULL;
	setDebug(cont->getConfig()->getDebugTriggers());
}

sqlrtriggers::~sqlrtriggers() {
}

bool sqlrtriggers::load(domnode *parameters) {

	unload();

	// run through the module list
	for (domnode *moduledata=parameters->getFirstTagChild();
				!moduledata->isNullNode();
				moduledata=moduledata->getNextTagSibling()) {

		// skip disabled modules
		if (isModuleDisabled(moduledata)) {
			continue;
		}

		// is this a before, after, or both trigger
		bool	before=(charstring::contains(
					moduledata->getAttributeValue("when"),
					"before") ||
				charstring::contains(
					moduledata->getAttributeValue("when"),
					"both"));
		bool	after=(charstring::contains(
					moduledata->getAttributeValue("when"),
					"after") ||
				charstring::contains(
					moduledata->getAttributeValue("when"),
					"both"));

		// load the trigger
		sqlrmoduleplugin	*p;
		loadModule(moduledata,&p);
		if (!p) {
			continue;
		}

		// add trigger to before list
		if (before) {
			debugWrite("before trigger");
			blist.append(p);
		}

		// add trigger to after list
		if (after) {
			debugWrite("after trigger");
			alist.append(p);
		}
	}
	return true;
}

void sqlrtriggers::loadModule(domnode *parameters, sqlrmoduleplugin **plugin) {

	*plugin=NULL;

	// ignore non-triggers
	if (charstring::compare(parameters->getName(),"trigger")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading trigger: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the trigger module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("trigger_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load trigger module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the trigger itself
	stringbuffer	functionname;
	functionname.append("new_sqlrtrigger_")->append(module);
	sqlrtrigger *(*newTrigger)(sqlrservercontroller *,
						domnode *)=
			(sqlrtrigger *(*)(sqlrservercontroller *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newTrigger) {
		stdoutput.printf("failed to load trigger: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrtrigger	*tr=(*newTrigger)(cont,parameters);

#else

	dynamiclib	*dl=NULL;
	sqlrtrigger	*tr;
	#include "sqlrtriggerassignments.cpp"
	{
		tr=NULL;
	}
#endif

	debugWrite("success");

	// build and return the plugin
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=tr;
	sqlrmp->dl=dl;
	sqlrmp->module=module;
	*plugin=sqlrmp;
	return;
}

bool sqlrtriggers::runBeforeTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {
	return runBefore(sqlrcon,sqlrcur,&blist);
}

bool sqlrtriggers::runAfterTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {
	return runAfter(sqlrcon,sqlrcur,&alist);
}

bool sqlrtriggers::runBefore(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				singlylinkedlist< sqlrmoduleplugin * > *list) {
	for (listnode< sqlrmoduleplugin * > *node=list->getFirst();
						node; node=node->getNext()) {

		debugWrite("running before trigger: %s...",
					node->getValue()->module);

		sqlrtrigger	*tr=(sqlrtrigger *)node->getValue()->m;
		if (!tr->runBefore(sqlrcon,sqlrcur)) {
			return false;
		}
	}
	return true;
}

bool sqlrtriggers::runAfter(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				singlylinkedlist< sqlrmoduleplugin * > *list) {
	for (listnode< sqlrmoduleplugin * > *node=list->getFirst();
						node; node=node->getNext()) {

		debugWrite("running after trigger: %s...",
					node->getValue()->module);

		sqlrtrigger	*tr=(sqlrtrigger *)node->getValue()->m;
		if (!tr->runAfter(sqlrcon,sqlrcur)) {
			return false;
		}
	}
	return true;
}
