// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrloggers.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <debugprint.h>

#include <rudiments/xmldomnode.h>

#include <config.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
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
		sqlrloggerplugin	*sqlrlp=node->getData();
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

	// load the logger module
	stringbuffer	modulename;
	modulename.append(LIBEXECDIR);
	modulename.append("/sqlrlogger_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		printf("failed to load logger module: %s\n",module);
		char	*error=dl->getError();
		printf("%s\n",error);
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
		printf("failed to create logger: %s\n",module);
		char	*error=dl->getError();
		printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrlogger	*lg=(*newLogger)(logger);

	// add the plugin to the list
	sqlrloggerplugin	*sqlrlp=new sqlrloggerplugin;
	sqlrlp->lg=lg;
	sqlrlp->dl=dl;
	llist.append(sqlrlp);
}

void sqlrloggers::initLoggers(sqlrconnection_svr *sqlrcon) {
	debugFunction();
	for (linkedlistnode< sqlrloggerplugin * > *node=llist.getFirstNode();
						node; node=node->getNext()) {
		node->getData()->lg->init(sqlrcon);
	}
}

void sqlrloggers::runLoggers(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrlogger_eventtype_t event,
					const char *info) {
	debugFunction();
	for (linkedlistnode< sqlrloggerplugin * > *node=llist.getFirstNode();
						node; node=node->getNext()) {
		node->getData()->lg->run(sqlrcon,sqlrcur,level,event,info);
	}
}
