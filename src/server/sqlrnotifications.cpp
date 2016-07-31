// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/xmldomnode.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>
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
	unload();
}

bool sqlrnotifications::load(xmldomnode *parameters) {
	debugFunction();

	unload();

	transports=parameters->getFirstTagChild("transports");

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

void sqlrnotifications::unload() {
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
	sqlrnotification *(*newNotification)(sqlrnotifications *, xmldomnode *)=
		(sqlrnotification *(*)(sqlrnotifications *, xmldomnode *))
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
	sqlrnotification	*n=(*newNotification)(this,notification);

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

void sqlrnotifications::init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon) {
	debugFunction();
	for (singlylinkedlistnode< sqlrnotificationplugin * > *node=
						llist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->n->init(sqlrl,sqlrcon);
	}
}

void sqlrnotifications::run(sqlrlistener *sqlrl,
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

// FIXME: push up and consolidate
static const char *eventtypes[]={
	"CLIENT_CONNECTED",
	"CLIENT_CONNECTION_REFUSED",
	"CLIENT_DISCONNECTED",
	"CLIENT_PROTOCOL_ERROR",
	"DB_LOGIN",
	"DB_LOGOUT",
	"DB_ERROR",
	"DB_WARNING",
	"QUERY",
	"INTERNAL_ERROR",
	"INTERNAL_WARNING",
	"DEBUG_MESSAGE",
	"SCHEDULE_VIOLATION",
	NULL
};

const char *sqlrnotifications::eventType(sqlrevent_t event) {
	return eventtypes[(uint16_t)event];
}

sqlrevent_t sqlrnotifications::eventType(const char *event) {
	uint16_t	retval=SQLREVENT_CLIENT_CONNECTED;
	for (const char * const *ev=eventtypes; *ev; ev++) {
		if (!charstring::compareIgnoringCase(event,*ev)) {
			break;
		}
		retval++;
	}
	return (sqlrevent_t)retval;
}

bool sqlrnotifications::sendNotification(const char *address,
						const char *transportid,
						const char *subject,
						const char *templatefile,
						sqlrevent_t event,
						const char *info) {
	debugFunction();

	// get the recipient and transport info
	xmldomnode	*transport=getTransport(transportid);
	const char	*url=transport->getAttributeValue("url");

	debugPrintf("notifying %s with %s about "
			"event %s with info \"%s\" via %s\n",
			address,templatefile,eventType(event),info,url);

	// get the subject and perform substitutions
	const char	*subj=subject;
	if (charstring::isNullOrEmpty(subj)) {
		subj=SQL_RELAY" Notification";
	}
	// FIXME: perform substitutions

	// get the tempate file and perform substiutions
	file	tfile;
	if (!tfile.open(templatefile,O_RDONLY)) {
		return false;
	}
	char	*message=tfile.getContents();
	// FIXME: perform substitutions
	
	// handle different transports...
	if (!charstring::compare(url,"mail")) {

#ifndef _WIN32
		// write the message to a temp file
		// FIXME: make this location configurable or
		// use SQL Relay's temp directory
		char	tempfile[12]="/tmp/XXXXXX";
		int32_t	tfd=file::createTemporaryFile(tempfile);
		if (tfd==-1) {
			delete[] message;
			return false;
		}

		// write the message to the temp file
		file	tf;
		tf.setFileDescriptor(tfd);
		if (tf.write(message,charstring::length(message))!=
					(ssize_t)charstring::length(message)) {
			delete[] message;
			return false;
		}
		tf.close();

		// build mail command
		stringbuffer	mailcmd;
		mailcmd.append("mail -s \"");
		mailcmd.append(subj);
		mailcmd.append("\"");
		mailcmd.append(" \"")->append(address)->append("\"");
		mailcmd.append(" < ")->append(tempfile);
		mailcmd.append(" 2> /dev/null");

		debugPrintf("%s\n",mailcmd.getString());

		// launch mail command
		const char	*args[100];
		uint32_t	a=0;
		args[a++]="sh";
		args[a++]="-c";
		args[a++]=mailcmd.getString();
		args[a++]=NULL;
		pid_t	pid=process::spawn("/bin/sh",args,false);

		debugPrintf("pid: %d\n",pid);

		// wait for the command to finish
		if (pid!=-1) {
			process::getChildStateChange(pid,true,true,true,
							NULL,NULL,NULL,NULL);
		}

		// clean up
		file::remove(tempfile);

#else
		// FIXME: is there a local mail program we can run on Windows?
		delete[] message;
		return false;
#endif

	} else if (!charstring::compare(url,"smtp:",5) ||
			!charstring::compare(url,"smtps:",6)) {

		// FIXME: implement this...

	}

	// clean up
	delete[] message;

	return true;
}

xmldomnode *sqlrnotifications::getTransport(const char *transportid) {
	for (xmldomnode *tnode=transports->getFirstTagChild("transport");
				!tnode->isNullNode();
				tnode=tnode->getNextTagSibling("transport")) {
		if (!charstring::compare(transportid,
					tnode->getAttributeValue("id"))) {
			return tnode;
		}
	}
	return transports->getNullNode();
}
