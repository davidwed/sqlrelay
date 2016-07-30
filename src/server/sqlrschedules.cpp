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
		#include "sqlrscheduledeclarations.cpp"
	}
#endif

sqlrschedules::sqlrschedules(sqlrpaths *sqlrpth) {
	debugFunction();
	libexecdir=sqlrpth->getLibExecDir();
}

sqlrschedules::~sqlrschedules() {
	debugFunction();
	unloadSchedules();
}

bool sqlrschedules::loadSchedules(xmldomnode *parameters) {
	debugFunction();

	unloadSchedules();

	// run through the schedule list
	for (xmldomnode *schedule=parameters->getFirstTagChild();
			!schedule->isNullNode();
			schedule=schedule->getNextTagSibling()) {

		debugPrintf("loading schedule ...\n");

		// load schedule
		loadSchedule(schedule);
	}
	return true;
}

void sqlrschedules::unloadSchedules() {
	debugFunction();
	for (singlylinkedlistnode< sqlrscheduleplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		sqlrscheduleplugin	*sqlrsp=node->getValue();
		delete sqlrsp->s;
		delete sqlrsp->dl;
		delete sqlrsp;
	}
	llist.clear();
}

void sqlrschedules::loadSchedule(xmldomnode *schedule) {

	debugFunction();

	// ignore non-schedules
	if (charstring::compare(schedule->getName(),"schedule")) {
		return;
	}

	// get the schedule name
	const char	*module=schedule->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=schedule->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	debugPrintf("loading schedule: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the schedule module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append(SQLR);
	modulename.append("schedule_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf(
			"failed to load schedule module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the schedule itself
	stringbuffer	functionname;
	functionname.append("new_sqlrschedule_")->append(module);
	sqlrschedule *(*newSchedule)(xmldomnode *)=
			(sqlrschedule *(*)(xmldomnode *))
				dl->getSymbol(functionname.getString());
	if (!newSchedule) {
		stdoutput.printf("failed to create schedule: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrschedule	*s=(*newSchedule)(schedule);

#else

	dynamiclib	*dl=NULL;
	sqlrschedule	*s;
	#include "sqlrscheduleassignments.cpp"
	{
		s=NULL;
	}
#endif

	// add the plugin to the list
	sqlrscheduleplugin	*sqlrsp=new sqlrscheduleplugin;
	sqlrsp->s=s;
	sqlrsp->dl=dl;
	llist.append(sqlrsp);
}

void sqlrschedules::initSchedules(sqlrserverconnection *sqlrcon) {
	debugFunction();
	for (singlylinkedlistnode< sqlrscheduleplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->s->init(sqlrcon);
	}
}

bool sqlrschedules::allowed(sqlrserverconnection *sqlrcon) {
	debugFunction();
	for (singlylinkedlistnode< sqlrscheduleplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		if (!node->getValue()->s->allowed(sqlrcon)) {
			return false;
		}
	}
	return true;
}
