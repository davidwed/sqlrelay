// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

class sqlrtriggerplugin {
	public:
		sqlrtrigger	*tr;
		dynamiclib	*dl;
};

class sqlrtriggersprivate {
	friend class sqlrtriggers;
	private:
		const char	*_libexecdir;
		bool		_debug;

		singlylinkedlist< sqlrtriggerplugin * >	_beforetriggers;
		singlylinkedlist< sqlrtriggerplugin * >	_aftertriggers;
};

sqlrtriggers::sqlrtriggers(sqlrpaths *sqlrpth, bool debug) {
	debugFunction();
	pvt=new sqlrtriggersprivate;
	pvt->_libexecdir=sqlrpth->getLibExecDir();
	pvt->_debug=debug;
}

sqlrtriggers::~sqlrtriggers() {
	debugFunction();
	unload();
	delete pvt;
}

bool sqlrtriggers::load(xmldomnode *parameters) {
	debugFunction();

	unload();

	// run through the trigger list
	for (xmldomnode *trigger=parameters->getFirstTagChild();
		!trigger->isNullNode(); trigger=trigger->getNextTagSibling()) {

		// add trigger to before list
		if (charstring::contains(
				trigger->getAttributeValue("when"),
				"before")) {
			if (pvt->_debug) {
				stdoutput.printf("loading trigger "
							"before ...\n");
			}
			loadTrigger(trigger,&pvt->_beforetriggers);
		}

		// add trigger to after list
		if (charstring::contains(
				trigger->getAttributeValue("when"),
				"after")) {
			if (pvt->_debug) {
				stdoutput.printf("loading trigger "
							"after ...\n");
			}
			loadTrigger(trigger,&pvt->_aftertriggers);
		}
	}
	return true;
}

void sqlrtriggers::unload() {
	debugFunction();
	for (singlylinkedlistnode< sqlrtriggerplugin * > *bnode=
				pvt->_beforetriggers.getFirst();
					bnode; bnode=bnode->getNext()) {
		sqlrtriggerplugin	*sqlt=bnode->getValue();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_beforetriggers.clear();
	for (singlylinkedlistnode< sqlrtriggerplugin * > *anode=
				pvt->_aftertriggers.getFirst();
					anode; anode=anode->getNext()) {
		sqlrtriggerplugin	*sqlt=anode->getValue();
		delete sqlt->tr;
		delete sqlt->dl;
		delete sqlt;
	}
	pvt->_aftertriggers.clear();
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

	if (pvt->_debug) {
		stdoutput.printf("loading trigger: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the trigger module
	stringbuffer	modulename;
	modulename.append(pvt->_libexecdir);
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
	sqlrtrigger	*tr=(*newTrigger)(trigger,pvt->_debug);

#else

	dynamiclib	*dl=NULL;
	sqlrtrigger	*tr=NULL;
#endif

	if (pvt->_debug) {
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
	run(sqlrcon,sqlrcur,querytree,&pvt->_beforetriggers,true,true);
}

void sqlrtriggers::runAfterTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree,
						bool success) {
	debugFunction();
	run(sqlrcon,sqlrcur,querytree,&pvt->_aftertriggers,false,success);
}

void sqlrtriggers::run(sqlrserverconnection *sqlrcon,
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
		if (pvt->_debug) {
			stdoutput.printf("\nrunning %s trigger...\n\n",
						(before)?"before":"after");
		}
		node->getValue()->tr->run(sqlrcon,sqlrcur,
						querytree,before,success);
	}
}
