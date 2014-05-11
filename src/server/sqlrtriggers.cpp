// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrtriggers.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrtriggerdeclarations.cpp"
	}
#endif

sqlrtriggers::sqlrtriggers() {
	debugFunction();
	xmld=NULL;
}

sqlrtriggers::~sqlrtriggers() {
	debugFunction();
	unloadTriggers();
	delete xmld;
}

bool sqlrtriggers::loadTriggers(const char *triggers) {
	debugFunction();

	unloadTriggers();

	// create the parser
	delete xmld;
	xmld=new xmldom();

	// parse the triggers
	if (!xmld->parseString(triggers)) {
		return false;
	}

	// get the triggers tag
	xmldomnode	*triggersnode=
			xmld->getRootNode()->getFirstTagChild("triggers");
	if (triggersnode->isNullNode()) {
		return false;
	}

	// run through the trigger list
	for (xmldomnode *trigger=triggersnode->getFirstTagChild();
		!trigger->isNullNode(); trigger=trigger->getNextTagSibling()) {

		// get whether to run before or after a query
		bool	before=!charstring::compare(
					trigger->getAttributeValue("when"),
					"before");

		debugPrintf("loading trigger %s ...\n",
				(before)?"before":"after");

		// determine which list to put the trigger in
		linkedlist< sqlrtriggerplugin * >	*list=
				(before)?&beforetriggers:&aftertriggers;

		// load trigger
		loadTrigger(trigger,list);
	}
	return true;
}

void sqlrtriggers::unloadTriggers() {
	debugFunction();
	for (linkedlistnode< sqlrtriggerplugin * > *bnode=
				beforetriggers.getFirstNode();
					bnode; bnode=bnode->getNext()) {
		sqlrtriggerplugin	*sqlt=bnode->getValue();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	beforetriggers.clear();
	for (linkedlistnode< sqlrtriggerplugin * > *anode=
				aftertriggers.getFirstNode();
					anode; anode=anode->getNext()) {
		sqlrtriggerplugin	*sqlt=anode->getValue();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	aftertriggers.clear();
}

void sqlrtriggers::loadTrigger(xmldomnode *trigger,
				linkedlist< sqlrtriggerplugin * > *list) {

	debugFunction();

	// ignore non-triggers
	if (charstring::compare(trigger->getName(),"trigger")) {
		return;
	}

	// get the trigger name
	const char	*module=trigger->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=trigger->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	debugPrintf("loading trigger: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the trigger module
	stringbuffer	modulename;
	modulename.append(LIBEXECDIR);
	modulename.append("/sqlrtrigger_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load trigger module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the trigger itself
	stringbuffer	functionname;
	functionname.append("new_")->append(module);
	sqlrtrigger *(*newTrigger)(xmldomnode *)=
			(sqlrtrigger *(*)(xmldomnode *))
				dl->getSymbol(functionname.getString());
	if (!newTrigger) {
		stdoutput.printf("failed to create trigger: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrtrigger	*tr=(*newTrigger)(trigger);

#else

	dynamiclib	*dl=NULL;
	sqlrtrigger	*tr;
	#include "sqlrtriggerassignments.cpp"
	{
		tr=NULL;
	}
#endif

	// add the plugin to the list
	sqlrtriggerplugin	*sqltp=new sqlrtriggerplugin;
	sqltp->tr=tr;
	sqltp->dl=dl;
	list->append(sqltp);
}

void sqlrtriggers::runBeforeTriggers(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	runTriggers(sqlrcon,sqlrcur,querytree,&beforetriggers,true,true);
}

void sqlrtriggers::runAfterTriggers(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree,
						bool success) {
	debugFunction();
	runTriggers(sqlrcon,sqlrcur,querytree,&aftertriggers,false,success);
}

void sqlrtriggers::runTriggers(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					linkedlist< sqlrtriggerplugin * > *list,
					bool before,
					bool success) {
	debugFunction();
	if (!querytree) {
		return;
	}
	for (linkedlistnode< sqlrtriggerplugin * > *node=list->getFirstNode();
						node; node=node->getNext()) {
		node->getValue()->tr->run(sqlrcon,sqlrcur,
						querytree,before,success);
	}
}
