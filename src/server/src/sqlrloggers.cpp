// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrloggers.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <debugprint.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/stdio.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrloggerdeclarations.cpp"
	}
#endif

sqlrloggers::sqlrloggers() {
	debugFunction();
	xmld=NULL;
}

sqlrloggers::~sqlrloggers() {
	debugFunction();
	unloadLoggers();
	delete xmld;
}

bool sqlrloggers::loadLoggers(const char *loggers) {
	debugFunction();

	unloadLoggers();

	// create the parser
	delete xmld;
	xmld=new xmldom();

	// parse the loggers
	if (!xmld->parseString(loggers)) {
		return false;
	}

	// get the loggers tag
	xmldomnode	*loggersnode=
			xmld->getRootNode()->getFirstTagChild("loggers");
	if (loggersnode->isNullNode()) {
		return false;
	}

	// run through the logger list
	for (xmldomnode *logger=loggersnode->getFirstTagChild();
		!logger->isNullNode(); logger=logger->getNextTagSibling()) {

		debugPrintf("loading logger ...\n");

		// load logger
		loadLogger(logger);
	}
	return true;
}

void sqlrloggers::unloadLoggers() {
	debugFunction();
	for (linkedlistnode< sqlrloggerplugin * > *node=
				llist.getFirstNode();
					node; node=node->getNext()) {
		sqlrloggerplugin	*sqlrlp=node->getValue();
		delete sqlrlp->lg;
		delete sqlrlp->dl;
		delete sqlrlp;
	}
	llist.clear();
}

void sqlrloggers::loadLogger(xmldomnode *logger) {

	debugFunction();

	// ignore non-loggers
	if (charstring::compare(logger->getName(),"logger")) {
		return;
	}

	// get the logger name
	const char	*module=logger->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=logger->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	debugPrintf("loading logger: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the logger module
	stringbuffer	modulename;
	modulename.append(LIBEXECDIR);
	modulename.append("/sqlrlogger_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf("failed to load logger module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the logger itself
	stringbuffer	functionname;
	functionname.append("new_")->append(module);
	sqlrlogger *(*newLogger)(xmldomnode *)=
			(sqlrlogger *(*)(xmldomnode *))
				dl->getSymbol(functionname.getString());
	if (!newLogger) {
		stdoutput.printf("failed to create logger: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrlogger	*lg=(*newLogger)(logger);

#else

	dynamiclib	*dl=NULL;
	sqlrlogger	*lg;
	#include "sqlrloggerassignments.cpp"
	{
		lg=NULL;
	}
#endif

	// add the plugin to the list
	sqlrloggerplugin	*sqlrlp=new sqlrloggerplugin;
	sqlrlp->lg=lg;
	sqlrlp->dl=dl;
	llist.append(sqlrlp);
}

void sqlrloggers::initLoggers(sqlrlistener *sqlrl,
				sqlrconnection_svr *sqlrcon) {
	debugFunction();
	for (linkedlistnode< sqlrloggerplugin * > *node=llist.getFirstNode();
						node; node=node->getNext()) {
		node->getValue()->lg->init(sqlrl,sqlrcon);
	}
}

void sqlrloggers::runLoggers(sqlrlistener *sqlrl,
					sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrlogger_eventtype_t event,
					const char *info) {
	debugFunction();
	for (linkedlistnode< sqlrloggerplugin * > *node=llist.getFirstNode();
						node; node=node->getNext()) {
		node->getValue()->lg->run(sqlrl,sqlrcon,sqlrcur,
						level,event,info);
	}
}
