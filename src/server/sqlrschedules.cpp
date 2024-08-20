// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrscheduledeclarations.cpp"
	}
#endif

sqlrschedules::sqlrschedules(sqlrservercontroller *cont) :
					sqlrservermodules(cont) {
	pvt=NULL;
}

sqlrschedules::~sqlrschedules() {
}

void sqlrschedules::loadModule(domnode *parameters) {

	// ignore non-schedules
	if (charstring::compare(parameters->getName(),"schedule")) {
		return;
	}

	// get the module name
	const char	*module=getModuleName(parameters);
	if (charstring::isNullOrEmpty(module)) {
		return;
	}

	debugWrite("loading schedule module: %s",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the schedule module
	stringbuffer	modulename;
	modulename.append(cont->getPaths()->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("schedule_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf(
			"failed to load schedule module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the schedule itself
	stringbuffer	functionname;
	functionname.append("new_sqlrschedule_")->append(module);
	sqlrschedule *(*newSchedule)(sqlrservercontroller *,
						domnode *)=
			(sqlrschedule *(*)(sqlrservercontroller *,
							domnode *))
				dl->getSymbol(functionname.getString());
	if (!newSchedule) {
		stdoutput.printf("failed to load schedule: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrschedule	*s=(*newSchedule)(cont,parameters);

#else

	dynamiclib	*dl=NULL;
	sqlrschedule	*s;
	#include "sqlrscheduleassignments.cpp"
	{
		s=NULL;
	}
#endif

	debugWrite("success");

	// add the plugin to the list
	sqlrmoduleplugin	*sqlrmp=new sqlrmoduleplugin;
	sqlrmp->m=s;
	sqlrmp->dl=dl;
	sqlrmp->module=module;
	blist.append(sqlrmp);
}

bool sqlrschedules::allowed(sqlrserverconnection *sqlrcon, const char *user) {

	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {

		debugWrite("checking schedule: %s...",node->getValue()->module);

		sqlrschedule	*s=(sqlrschedule *)node->getValue()->m;
		if (!s->allowed(sqlrcon,user)) {
			return false;
		}
	}
	return true;
}
