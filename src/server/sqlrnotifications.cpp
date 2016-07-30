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
		#include "sqlrnotificationdeclarations.cpp"
	}
#endif

sqlrnotifications::sqlrnotifications(sqlrpaths *sqlrpth) {
	debugFunction();
	libexecdir=sqlrpth->getLibExecDir();
}

sqlrnotifications::~sqlrnotifications() {
	debugFunction();
	unloadNotifications();
}

bool sqlrnotifications::loadNotifications(xmldomnode *parameters) {
	debugFunction();

	unloadNotifications();

	// run through the notification list
	for (xmldomnode *notification=parameters->getFirstTagChild();
			!notification->isNullNode();
			notification=notification->getNextTagSibling()) {

		debugPrintf("loading notification ...\n");

		// load notification
		loadNotification(notification);
	}
	return true;
}

void sqlrnotifications::unloadNotifications() {
	debugFunction();
	for (singlylinkedlistnode< sqlrnotificationplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		sqlrnotificationplugin	*sqlrnp=node->getValue();
		delete sqlrnp->n;
		delete sqlrnp->dl;
		delete sqlrnp;
	}
	llist.clear();
}

void sqlrnotifications::loadNotification(xmldomnode *notification) {

	debugFunction();

	// ignore non-notifications
	if (charstring::compare(notification->getName(),"notification")) {
		return;
	}

	// get the notification name
	const char	*module=notification->getAttributeValue("module");
	if (!charstring::length(module)) {
		// try "file", that's what it used to be called
		module=notification->getAttributeValue("file");
		if (!charstring::length(module)) {
			return;
		}
	}

	debugPrintf("loading notification: %s\n",module);

#ifdef SQLRELAY_ENABLE_SHARED
	// load the notification module
	stringbuffer	modulename;
	modulename.append(libexecdir);
	modulename.append(SQLR);
	modulename.append("notification_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf(
			"failed to load notification module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		delete dl;
		return;
	}

	// load the notification itself
	stringbuffer	functionname;
	functionname.append("new_sqlrnotification_")->append(module);
	sqlrnotification *(*newNotification)(xmldomnode *)=
			(sqlrnotification *(*)(xmldomnode *))
				dl->getSymbol(functionname.getString());
	if (!newNotification) {
		stdoutput.printf("failed to create notification: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",error);
		delete[] error;
		dl->close();
		delete dl;
		return;
	}
	sqlrnotification	*n=(*newNotification)(notification);

#else

	dynamiclib	*dl=NULL;
	sqlrnotification	*n;
	#include "sqlrnotificationassignments.cpp"
	{
		n=NULL;
	}
#endif

	// add the plugin to the list
	sqlrnotificationplugin	*sqlrnp=new sqlrnotificationplugin;
	sqlrnp->n=n;
	sqlrnp->dl=dl;
	llist.append(sqlrnp);
}

void sqlrnotifications::initNotifications(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon) {
	debugFunction();
	for (singlylinkedlistnode< sqlrnotificationplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->n->init(sqlrl,sqlrcon);
	}
}

void sqlrnotifications::runNotifications(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info) {
	debugFunction();
	for (singlylinkedlistnode< sqlrnotificationplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->n->run(sqlrl,sqlrcon,sqlrcur,event,info);
	}
}
