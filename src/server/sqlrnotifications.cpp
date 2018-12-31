// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>
#include <rudiments/sys.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrnotificationdeclarations.cpp"
	}
#endif

class sqlrnotificationplugin {
	public:
		sqlrnotification	*n;
		dynamiclib		*dl;
};

class sqlrnotificationsprivate {
	friend class sqlrnotifications;
	private:
		const char	*_libexecdir;
		const char	*_tmpdir;
		char		*_tmpfilename;

		domnode	*_transports;

		singlylinkedlist< sqlrnotificationplugin * >	_llist;
};

sqlrnotifications::sqlrnotifications(sqlrpaths *sqlrpth) {
	debugFunction();
	pvt=new sqlrnotificationsprivate;
	pvt->_libexecdir=sqlrpth->getLibExecDir();
	pvt->_tmpdir=sqlrpth->getTmpDir();
	pvt->_tmpfilename=new char[charstring::length(
					sqlrpth->getTmpDir())+6+1];
}

sqlrnotifications::~sqlrnotifications() {
	debugFunction();
	unload();
	delete[] pvt->_tmpfilename;
	delete pvt;
}

bool sqlrnotifications::load(domnode *parameters) {
	debugFunction();

	unload();

	pvt->_transports=parameters->getFirstTagChild("transports");

	// run through the notification list
	for (domnode *notification=parameters->getFirstTagChild();
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
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		sqlrnotificationplugin	*sqlrnp=node->getValue();
		delete sqlrnp->n;
		delete sqlrnp->dl;
		delete sqlrnp;
	}
	pvt->_llist.clear();
}

void sqlrnotifications::loadNotification(domnode *notification) {

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
	modulename.append(pvt->_libexecdir);
	modulename.append(SQLR);
	modulename.append("notification_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	dynamiclib	*dl=new dynamiclib();
	if (!dl->open(modulename.getString(),true,true)) {
		stdoutput.printf(
			"failed to load notification module: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
		delete[] error;
		delete dl;
		return;
	}

	// load the notification itself
	stringbuffer	functionname;
	functionname.append("new_sqlrnotification_")->append(module);
	sqlrnotification *(*newNotification)(sqlrnotifications *,
							domnode *)=
		(sqlrnotification *(*)(sqlrnotifications *,
						domnode *))
				dl->getSymbol(functionname.getString());
	if (!newNotification) {
		stdoutput.printf("failed to load notification: %s\n",module);
		char	*error=dl->getError();
		stdoutput.printf("%s\n",(error)?error:"");
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
	pvt->_llist.append(sqlrnp);
}

void sqlrnotifications::run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info) {
	debugFunction();
	for (singlylinkedlistnode< sqlrnotificationplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->n->run(sqlrl,sqlrcon,sqlrcur,event,info);
	}
}

void sqlrnotifications::endTransaction(bool commit) {
	for (singlylinkedlistnode< sqlrnotificationplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->n->endTransaction(commit);
	}
}

void sqlrnotifications::endSession() {
	for (singlylinkedlistnode< sqlrnotificationplugin * > *node=
						pvt->_llist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->n->endSession();
	}
}

bool sqlrnotifications::sendNotification(sqlrlistener *sqlrl,
						sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *address,
						const char *transportid,
						const char *subject,
						const char *templatefile,
						sqlrevent_t event,
						const char *info) {
	debugFunction();

	// get the event string
	const char	*eventstring=eventType(event);

	// get the transport info, falling back to "mail"
	domnode	*transport=getTransport(transportid);
	const char	*url=transport->getAttributeValue("url");
	if (charstring::isNullOrEmpty(url)) {
		url="mail";
	}
	const char	*agent=transport->getAttributeValue("agent");
	if (charstring::isNullOrEmpty(agent)) {
		#ifndef _WIN32
			agent="mail";
		#else
			agent="blat.exe";
		#endif
	}

	debugPrintf("notifying %s with %s about "
			"event %s with info \"%s\" via %s\n",
			address,eventstring,eventstring,info,url);

	// get the subject and perform substitutions
	if (charstring::isNullOrEmpty(subject)) {
		subject=SQL_RELAY" Notification: @event@";
	}
	char	*subj=substitutions(sqlrl,sqlrcon,sqlrcur,
					subject,eventstring,info);

	// get the message template file and perform substiutions
	char	*msg=NULL;
	file	tfile;
	if (!charstring::isNullOrEmpty(templatefile) &&
				tfile.open(templatefile,O_RDONLY)) {
		char	*message=tfile.getContents();
		msg=substitutions(sqlrl,sqlrcon,sqlrcur,
					message,eventstring,info);
		delete[] message;
	} else {
		msg=substitutions(sqlrl,sqlrcon,sqlrcur,
				SQL_RELAY" Notification\n\n"
				"Event          : @event@\n"
				"Event Info     : @eventinfo@\n"
				"Date           : @datetime@\n"
				"Host Name      : @hostname@\n"
				"Instance       : @instance@\n"
				"Process Id     : @pid@\n"
				"Client Address : @clientaddr@\n"
				"Client Info    : @clientinfo@\n"
				"User           : @user@\n"
				"Query          : \n@query@\n",
				eventstring,info);
	}
	
	// handle transports...
	if (!charstring::compare(url,"mail")) {

		charstring::copy(pvt->_tmpfilename,pvt->_tmpdir);
		charstring::append(pvt->_tmpfilename,"XXXXXX");
		int32_t	tfd=file::createTemporaryFile(pvt->_tmpfilename,
				permissions::evalPermString("rw-------"));
		if (tfd==-1) {
			delete[] subj;
			delete[] msg;
			return false;
		}

		debugPrintf("message file: %s\n",pvt->_tmpfilename);

		// write the message to the temp file
		file	tf;
		tf.setFileDescriptor(tfd);
		if (tf.write(msg,charstring::length(msg))!=
					(ssize_t)charstring::length(msg)) {
			tf.close();
			file::remove(pvt->_tmpfilename);
			delete[] subj;
			delete[] msg;
			return false;
		}
		tf.close();

#ifndef _WIN32
		// build mail command
		stringbuffer	mailcmd;

		mailcmd.append(agent)->append(" ");
		mailcmd.append("-s \"")->append(subj)->append("\" ");
		mailcmd.append("\"")->append(address)->append("\" ");
		mailcmd.append("< ")->append(pvt->_tmpfilename);
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
#else
		// launch mail command
		const char	*args[100];
		uint32_t	a=0;
		args[a++]=agent;
		args[a++]=pvt->_tmpfilename;
		args[a++]="-to";
		args[a++]=address;
		args[a++]="-subject";
		args[a++]=subj;
		args[a++]="-q";
		args[a++]=NULL;
		pid_t	pid=process::spawn(agent,args,false);
#endif

		debugPrintf("pid: %d\n",pid);

		// wait for the command to finish
		if (pid!=-1) {
			process::wait(pid);
		}

		// clean up
		file::remove(pvt->_tmpfilename);

	} else if (!charstring::compare(url,"debug")) {
		stdoutput.printf("======================================="
				"=======================================\n"
				"%s\n",msg);
	}
	// FIXME: implement other transports

	// clean up
	delete[] subj;
	delete[] msg;

	return true;
}

domnode *sqlrnotifications::getTransport(const char *transportid) {
	for (domnode *tnode=pvt->_transports->getFirstTagChild("transport");
				!tnode->isNullNode();
				tnode=tnode->getNextTagSibling("transport")) {
		if (!charstring::compare(transportid,
					tnode->getAttributeValue("id"))) {
			return tnode;
		}
	}
	return pvt->_transports->getNullNode();
}

char *sqlrnotifications::substitutions(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *str,
					const char *event,
					const char *info) {
	debugFunction();

	debugPrintf("input string:\n%s\n\n",str);

	// output buffer
	stringbuffer	outbuf;

	// get various bits of data for substitutions
	datetime	dt;
	dt.getSystemDateAndTime();
	char	*hostname=NULL;
	char	*pid=NULL;

	const char *ch=str;
	while (*ch) {

		// FIXME: implement more of these
		const char	*value=NULL;
		if (!charstring::compare(ch,"@event@",7)) {
			debugPrintf("event: ");
			value=event;
			ch+=7;
		} else if (!charstring::compare(ch,"@eventinfo@",11)) {
			debugPrintf("eventinfo: ");
			value=info;
			ch+=11;
		} else if (!charstring::compare(ch,"@datetime@",10)) {
			debugPrintf("datetime: ");
			value=dt.getString();
			ch+=10;
		} else if (!charstring::compare(ch,"@hostname@",10)) {
			debugPrintf("hostname: ");
			if (!hostname) {
				hostname=sys::getHostName();
			}
			value=hostname;
			ch+=10;
		} else if (!charstring::compare(ch,"@instance@",10)) {
			debugPrintf("instance: ");
			value=((sqlrcon)?sqlrcon->cont->getId():sqlrl->getId());
			ch+=10;
		} else if (!charstring::compare(ch,"@pid@",5)) {
			debugPrintf("pid: ");
			if (!pid) {
				pid=charstring::parseNumber(
					(int64_t)process::getProcessId());
			}
			value=pid;
			ch+=5;
		} else if (!charstring::compare(ch,"@clientaddr@",12)) {
			debugPrintf("clientaddr: ");
			value=(sqlrcon)?sqlrcon->cont->getClientAddr():"";
			ch+=12;
		} else if (!charstring::compare(ch,"@clientinfo@",12)) {
			debugPrintf("clientinfo: ");
			value=(sqlrcon)?sqlrcon->cont->getClientInfo():"";
			ch+=12;
		} else if (!charstring::compare(ch,"@user@",6)) {
			debugPrintf("user: ");
			value=(sqlrcon)?sqlrcon->cont->getCurrentUser():"";
			ch+=6;
		} else if (!charstring::compare(ch,"@query@",7)) {
			debugPrintf("query: ");
			value=(sqlrcon)?sqlrcon->cont->getCurrentQuery():"";
			ch+=7;
		}

		if (value) {
			debugPrintf(" %s\n",value);
			outbuf.append(value);
		} else {
			outbuf.append(*ch);
 			ch++;
		}
	}

	debugPrintf("\noutput string:\n%s\n\n",outbuf.getString());

	delete[] hostname;
	delete[] pid;

	return outbuf.detachString();
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
	"FILTER_VIOLATION",
	"INTERNAL_ERROR",
	"INTERNAL_WARNING",
	"DEBUG_MESSAGE",
	"SCHEDULE_VIOLATION",
	"INTEGRITY_VIOLATION",
	"TRANSLATION_FAILURE",
	"PARSE_FAILURE",
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
