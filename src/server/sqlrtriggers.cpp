// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

sqlrtriggers::sqlrtriggers(sqlrpaths *sqlrpth, bool debug) {
	debugFunction();
	libexecdir=sqlrpth->getLibExecDir();
	this->debug=debug;
}

sqlrtriggers::~sqlrtriggers() {
	debugFunction();
	unloadTriggers();
}

bool sqlrtriggers::loadTriggers(xmldomnode *parameters) {
	debugFunction();

	unloadTriggers();

	// run through the trigger list
	for (xmldomnode *trigger=parameters->getFirstTagChild();
		!trigger->isNullNode(); trigger=trigger->getNextTagSibling()) {

		// add trigger to before list
		if (charstring::contains(
				trigger->getAttributeValue("when"),
				"before")) {
			if (debug) {
				stdoutput.printf("loading trigger "
							"before ...\n");
			}
			loadTrigger(trigger,&beforetriggers);
		}

		// add trigger to after list
		if (charstring::contains(
				trigger->getAttributeValue("when"),
				"after")) {
			if (debug) {
				stdoutput.printf("loading trigger "
							"after ...\n");
			}
			loadTrigger(trigger,&aftertriggers);
		}
	}
	return true;
}

void sqlrtriggers::unloadTriggers() {
	debugFunction();
	for (singlylinkedlistnode< sqlrtriggerplugin * > *bnode=
				beforetriggers.getFirst();
					bnode; bnode=bnode->getNext()) {
		sqlrtriggerplugin	*sqlt=bnode->getValue();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	beforetriggers.clear();
	for (singlylinkedlistnode< sqlrtriggerplugin * > *anode=
				aftertriggers.getFirst();
					anode; anode=anode->getNext()) {
		sqlrtriggerplugin	*sqlt=anode->getValue();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	aftertriggers.clear();
}

void sqlrtriggers::loadTrigger(xmldomnode *trigger,
				singlylinkedlist< sqlrtriggerplugin * > *list) {

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

	if (debug) {
		stdoutput.printf("loading trigger: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the trigger module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append(SQLR);
	modulename.append("trigger_");
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
	functionname.append("new_sqlrtrigger_")->append(module);
	sqlrtrigger *(*newTrigger)(xmldomnode *, bool)=
			(sqlrtrigger *(*)(xmldomnode *, bool))
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
	sqlrtrigger	*tr=(*newTrigger)(trigger,debug);

#else

	dynamiclib	*dl=NULL;
	sqlrtrigger	*tr=NULL;
#endif

	if (debug) {
		stdoutput.printf("success\n");
	}

	// add the plugin to the list
	sqlrtriggerplugin	*sqltp=new sqlrtriggerplugin;
	sqltp->tr=tr;
	sqltp->dl=dl;
	list->append(sqltp);
}

void sqlrtriggers::runBeforeTriggers(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	runTriggers(sqlrcon,sqlrcur,querytree,&beforetriggers,true,true);
}

void sqlrtriggers::runAfterTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree,
						bool success) {
	debugFunction();
	runTriggers(sqlrcon,sqlrcur,querytree,&aftertriggers,false,success);
}

void sqlrtriggers::runTriggers(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				xmldom *querytree,
				singlylinkedlist< sqlrtriggerplugin * > *list,
				bool before,
				bool success) {
	debugFunction();
	if (!querytree) {
		return;
	}
	for (singlylinkedlistnode< sqlrtriggerplugin * > *node=list->getFirst();
						node; node=node->getNext()) {
		if (debug) {
			stdoutput.printf("\nrunning %s trigger...\n\n",
						(before)?"before":"after");
		}
		node->getValue()->tr->run(sqlrcon,sqlrcur,
						querytree,before,success);
	}
}
