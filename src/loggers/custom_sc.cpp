// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrlistener.h>
#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <sqlrlogger.h>
#include <cmdline.h>
#include <rudiments/charstring.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/datetime.h>
#include <debugprint.h>

class custom_sc : public sqlrlogger {
	public:
			custom_sc(xmldomnode *parameters);

		bool	init(sqlrlistener *sqlrl, sqlrconnection_svr *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrlogger_eventtype_t event,
					const char *info);
	private:
		file	querylog;
		char	*querylogname;
		stringbuffer		defaultquerylogpath;
		sqlrlogger_loglevel_t	loglevel;
		stringbuffer		logbuffer;
};

custom_sc::custom_sc(xmldomnode *parameters) : sqlrlogger(parameters) {
	querylogname=NULL;
	loglevel=SQLRLOGGER_LOGLEVEL_ERROR;
}

bool custom_sc::init(sqlrlistener *sqlrl, sqlrconnection_svr *sqlrcon) {
	debugFunction();

	// get log level
	const char	*ll=parameters->getAttributeValue("loglevel");
	if (!charstring::compareIgnoringCase(ll,"info")) {
		loglevel=SQLRLOGGER_LOGLEVEL_INFO;
	} else if (!charstring::compareIgnoringCase(ll,"warning")) {
		loglevel=SQLRLOGGER_LOGLEVEL_WARNING;
	} else {
		loglevel=SQLRLOGGER_LOGLEVEL_ERROR;
	}

	// get log path and name
	const char	*path=parameters->getAttributeValue("path");
	if (!charstring::length(path)) {
		cmdline	*cmdl=(sqlrcon)?sqlrcon->cont->cmdl:sqlrl->cmdl;
		defaultquerylogpath.clear();
		const char	*logdir=LOG_DIR;
		if (charstring::length(cmdl->getLocalStateDir())) {
			logdir=cmdl->getLocalStateDir();
		}
		defaultquerylogpath.append(logdir)->append("/sqlrelay/log");
		path=defaultquerylogpath.getString();
	}
	const char	*name=parameters->getAttributeValue("name");
	if (!charstring::length(name)) {
		name="sqlrelay.log";
	}

	// build up the query log name
	size_t	querylognamelen;
	delete[] querylogname;
	querylognamelen=charstring::length(path)+1+charstring::length(name)+1;
	querylogname=new char[querylognamelen];
	charstring::printf(querylogname,querylognamelen,"%s/%s",path,name);

	// create the new log file
	querylog.close();
	return querylog.open(querylogname,O_WRONLY|O_CREAT|O_APPEND,
				permissions::evalPermString("rw-------"));
}

bool custom_sc::run(sqlrlistener *sqlrl,
				sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				sqlrlogger_loglevel_t level,
				sqlrlogger_eventtype_t event,
				const char *info) {
	debugFunction();

	// bail if log level is too low
	if (level<loglevel) {
		return true;
	}

	// reinit the log if the file was switched
	ino_t	inode1=querylog.getInode();
	ino_t	inode2;
	if (!file::getInode(querylogname,&inode2) || inode1!=inode2) {
		init(sqlrl,sqlrcon);
	}

	// get the current date
	datetime	dt;
	dt.getSystemDateAndTime();

	// clear log buffer
	logbuffer.clear();

	// append the date
	char	datebuffer[20];
	charstring::printf(datebuffer,20,"%04d-%02d-%02d %02d:%02d:%02d",
				dt.getYear(),dt.getMonth(),dt.getDayOfMonth(),
				dt.getHour(),dt.getMinutes(),dt.getSeconds());
	logbuffer.append(datebuffer)->append(' ');

	// append the event type and log level
	// (except for db errors which are handled specially)
	if (event!=SQLRLOGGER_EVENTTYPE_DB_ERROR) {
		logbuffer.append(eventType(event))->append(' ');
		logbuffer.append(logLevel(level))->append(": ");
	}

	// get the client IP, it's needed for some events
	const char	*clientaddr="unknown";
	if (sqlrcon && sqlrcon->cont->connstats->clientaddr) {
		clientaddr=sqlrcon->cont->connstats->clientaddr;
	}

	// handle each event differently...
	switch (event) {
		case SQLRLOGGER_EVENTTYPE_CLIENT_CONNECTED:
			logbuffer.append("Client ");
			logbuffer.append(clientaddr);
			logbuffer.append(" connected");
			break;
		case SQLRLOGGER_EVENTTYPE_CLIENT_CONNECTION_REFUSED:
			logbuffer.append("Client ");
			logbuffer.append(clientaddr);
			logbuffer.append(" connection refused");
			break;
		case SQLRLOGGER_EVENTTYPE_CLIENT_DISCONNECTED:
			logbuffer.append("Client ");
			logbuffer.append(clientaddr);
			logbuffer.append(" disconnected");
			break;
		case SQLRLOGGER_EVENTTYPE_CLIENT_PROTOCOL_ERROR:
			logbuffer.append("Client ");
			logbuffer.append(clientaddr);
			logbuffer.append(" protocol error");
			break;
		case SQLRLOGGER_EVENTTYPE_DB_LOGIN:
			logbuffer.append("SQL Relay logged in to DB ");
			logbuffer.append(sqlrcon->cont->dbipaddress);
			break;
		case SQLRLOGGER_EVENTTYPE_DB_LOGOUT:
			logbuffer.append("SQL Relay logged out of DB ");
			logbuffer.append(sqlrcon->cont->dbipaddress);
			break;
		case SQLRLOGGER_EVENTTYPE_DB_ERROR:
			{
			const char	*colon=charstring::findFirst(info,':');
			if (colon) {
				logbuffer.append(info,colon-info)->append(' ');
				logbuffer.append(logLevel(level))->append(": ");
				logbuffer.append(colon+2);
			} else {
				logbuffer.append(eventType(event))->append(' ');
				logbuffer.append(logLevel(level))->append(": ");
				logbuffer.append(info);
			}
			}
			break;
		case SQLRLOGGER_EVENTTYPE_INTERNAL_ERROR:
			logbuffer.append("SQL Relay internal error");
			break;
		default:
			// ignore all other events
			return true;
	}

	// append info, if there was any
	// (except for db errors which are handled specially)
	if (charstring::length(info) && event!=SQLRLOGGER_EVENTTYPE_DB_ERROR) {
		logbuffer.append(": ");
		logbuffer.append(info);
	}

	// carriage return
	logbuffer.append("\n");

	// since all connection daemons are writing to the same file,
	// we must lock it prior to the write
	if (!querylog.lockFile(F_WRLCK)) {
		return false;
	}

	// write the buffer to the log file
	bool	retval=((size_t)querylog.write(logbuffer.getString(),
						logbuffer.getStringLength())==
						logbuffer.getStringLength());

	// unlock the log file
	querylog.unlockFile();
	
	return retval;
}

extern "C" {
	sqlrlogger	*new_custom_sc(xmldomnode *parameters) {
		return new custom_sc(parameters);
	}
}
