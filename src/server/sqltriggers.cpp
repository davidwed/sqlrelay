// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltriggers.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqltriggerdeclarations.cpp"
	}
#endif

sqltriggers::sqltriggers() {
	debugFunction();
	xmld=NULL;
}

sqltriggers::~sqltriggers() {
	debugFunction();
	unloadTriggers();
	delete xmld;
}

bool sqltriggers::loadTriggers(const char *triggers) {
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
		linkedlist< sqltriggerplugin * >	*list=
				(before)?&beforetriggers:&aftertriggers;

		// load trigger
		loadTrigger(trigger,list);
	}
	return true;
}

void sqltriggers::unloadTriggers() {
	debugFunction();
	for (linkedlistnode< sqltriggerplugin * > *bnode=
				beforetriggers.getFirstNode();
					bnode; bnode=bnode->getNext()) {
		sqltriggerplugin	*sqlt=bnode->getData();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	beforetriggers.clear();
	for (linkedlistnode< sqltriggerplugin * > *anode=
				aftertriggers.getFirstNode();
					anode; anode=anode->getNext()) {
		sqltriggerplugin	*sqlt=anode->getData();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	aftertriggers.clear();
}

void sqltriggers::loadTrigger(xmldomnode *trigger,
				linkedlist< sqltriggerplugin * > *list) {

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
	modulename.append("/sqltrigger_");
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
	sqltrigger *(*newTrigger)(xmldomnode *)=
			(sqltrigger *(*)(xmldomnode *))
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
	sqltrigger	*tr=(*newTrigger)(trigger);

#else

	dynamiclib	*dl=NULL;
	sqltrigger	*tr;
	#include "sqltriggerassignments.cpp"
	{
		tr=NULL;
	}
#endif

	// add the plugin to the list
	sqltriggerplugin	*sqltp=new sqltriggerplugin;
	sqltp->tr=tr;
	sqltp->dl=dl;
	list->append(sqltp);
}

void sqltriggers::runBeforeTriggers(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	runTriggers(sqlrcon,sqlrcur,querytree,&beforetriggers,true,true);
}

void sqltriggers::runAfterTriggers(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree,
						bool success) {
	debugFunction();
	runTriggers(sqlrcon,sqlrcur,querytree,&aftertriggers,false,success);
}

void sqltriggers::runTriggers(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					linkedlist< sqltriggerplugin * > *list,
					bool before,
					bool success) {
	debugFunction();
	if (!querytree) {
		return;
	}
	for (linkedlistnode< sqltriggerplugin * > *node=list->getFirstNode();
						node; node=node->getNext()) {
		node->getData()->tr->run(sqlrcon,sqlrcur,
						querytree,before,success);
	}
}
