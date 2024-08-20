// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/process.h>
#include <rudiments/sys.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <config.h>

class sqlrnotificationprivate {
	friend class sqlrnotification;
	private:
};

sqlrnotification::sqlrnotification(domnode *parameters) :
					sqlrservermodule(NULL,parameters) {
	pvt=new sqlrnotificationprivate;
}

sqlrnotification::~sqlrnotification() {
	delete pvt;
}

bool sqlrnotification::run(sqlrlistener *sqlrl,
			sqlrserverconnection *sqlrcon,
			sqlrservercursor *sqlrcur,
			sqlrevent_t event,
			const char *info) {
	return true;
}

bool sqlrnotification::sendNotification(sqlrlistener *sqlrl,
						sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *address,
						const char *transportid,
						const char *subject,
						const char *templatefile,
						sqlrevent_t event,
						const char *info) {
	debugFunction();

	// get the paths
	sqlrpaths	*p=(sqlrl)?sqlrl->getPaths():sqlrcon->cont->getPaths();

	// get the temp dir
	const char	*tmpdir=p->getTmpDir();

	// create a buffer for the temp file name
	char	*tmpfilename=new char[charstring::getLength(tmpdir)+6+1];
	

	// get the event string
	const char	*eventstring=(sqlrl)?sqlrl->getEventType(event):
					sqlrcon->cont->getEventType(event);

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

		charstring::copy(tmpfilename,tmpdir);
		charstring::append(tmpfilename,"XXXXXX");
		int32_t	tfd=file::createTemporaryFile(tmpfilename,
				permissions::parsePermString("rw-------"));
		if (tfd==-1) {
			delete[] subj;
			delete[] msg;
			delete[] tmpfilename;
			return false;
		}

		debugPrintf("message file: %s\n",tmpfilename);

		// write the message to the temp file
		file	tf;
		tf.setFileDescriptor(tfd);
		if (tf.write(msg,charstring::getLength(msg))!=
					(ssize_t)charstring::getLength(msg)) {
			tf.close();
			file::remove(tmpfilename);
			delete[] subj;
			delete[] msg;
			delete[] tmpfilename;
			return false;
		}
		tf.close();

#ifndef _WIN32
		// build mail command
		stringbuffer	mailcmd;

		mailcmd.append(agent)->append(" ");
		mailcmd.append("-s \"")->append(subj)->append("\" ");
		mailcmd.append("\"")->append(address)->append("\" ");
		mailcmd.append("< ")->append(tmpfilename);
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
		args[a++]=tmpfilename;
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
		file::remove(tmpfilename);

	} else if (!charstring::compare(url,"debug")) {
		stdoutput.printf("======================================="
				"=======================================\n"
				"%s\n",msg);
	}
	// FIXME: implement other transports

	// clean up
	delete[] subj;
	delete[] msg;
	delete[] tmpfilename;

	return true;
}

domnode *sqlrnotification::getTransport(const char *transportid) {
	for (domnode *tnode=getParameters()->getParent()->
					getFirstTagChild("transport");
				!tnode->isNullNode();
				tnode=tnode->getNextTagSibling("transport")) {
		if (!charstring::compare(transportid,
					tnode->getAttributeValue("id"))) {
			return tnode;
		}
	}
	return getParameters()->getNullNode();
}

char *sqlrnotification::substitutions(sqlrlistener *sqlrl,
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
	dt.initFromSystemDateTime();
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
