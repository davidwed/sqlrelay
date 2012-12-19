// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <sqlrlogger.h>
#include <cmdline.h>
#include <rudiments/charstring.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/datetime.h>
#include <rudiments/error.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class custom_sc : public sqlrlogger {
	public:
			custom_sc(xmldomnode *parameters);

		bool	init(sqlrconnection_svr *sqlrcon);
		bool	run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						sqlrlogger_loglevel_t level,
						sqlrlogger_eventtype_t event);
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

bool custom_sc::init(sqlrconnection_svr *sqlrcon) {
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
		cmdline	*cmdl=sqlrcon->cont->cmdl;
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
	snprintf(querylogname,querylognamelen,"%s/%s",path,name);

	// create the new log file
	querylog.close();
	return querylog.open(querylogname,O_WRONLY|O_CREAT|O_APPEND,
				permissions::evalPermString("rw-------"));
}

bool custom_sc::run(sqlrconnection_svr *sqlrcon,
			sqlrcursor_svr *sqlrcur,
			sqlrlogger_loglevel_t level,
			sqlrlogger_eventtype_t event) {
	debugFunction();

	// bail if log level is too low
	if (level<loglevel) {
		return true;
	}

	// reinit the log if the file was switched
	ino_t	inode1=querylog.getInode();
	ino_t	inode2;
	if (!file::getInode(querylogname,&inode2) || inode1!=inode2) {
		init(sqlrcon);
	}

	// get the current date
	datetime	dt;
	dt.getSystemDateAndTime();

	// clear log buffer
	logbuffer.clear();

	// append the date to the log buffer
	char	datebuffer[20];
	snprintf(datebuffer,20,"%04d-%02d-%02d %02d:%02d:%02d",
			dt.getYear(),dt.getMonth(),dt.getDayOfMonth(),
			dt.getHour(),dt.getMinutes(),dt.getSeconds());
	logbuffer.append(datebuffer);

	// for all events except db-errors, append a string
	// representation of the event type and log level
	if (event!=SQLRLOGGER_EVENTTYPE_DB_ERROR) {
		logbuffer.append(eventType(event))->append(' ');
		logbuffer.append(logLevel(level))->append(": ");
	}

	// handle each event differently...
	switch (event) {
		case SQLRLOGGER_EVENTTYPE_CLI_CONNECTED:
			logbuffer.append("Client <IP> connected.");
			break;
		case SQLRLOGGER_EVENTTYPE_CLI_CONNECTION_REFUSED:
			logbuffer.append("Client <IP> attempt to connect.  "
					"Connecton refused: Wrong user or "
					"wrong password");
			break;
		case SQLRLOGGER_EVENTTYPE_CLI_DISCONNECTED:
			logbuffer.append("Client <IP> connection reset "
					"by remote host.");
			break;
		case SQLRLOGGER_EVENTTYPE_CLI_SOCKET_ERROR:
			logbuffer.append(error::getErrorString());
			break;
		case SQLRLOGGER_EVENTTYPE_DB_CONNECTED:
			logbuffer.append("SQLRelay connected to DB <IP>");
			break;
		case SQLRLOGGER_EVENTTYPE_DB_DISCONNECTED:
			logbuffer.append("DB <IP> connection reset by "
					"remote host");
			break;
		case SQLRLOGGER_EVENTTYPE_DB_SOCKET_ERROR:
			logbuffer.append(error::getErrorString());
			break;
		case SQLRLOGGER_EVENTTYPE_DB_ERROR:
			logbuffer.append("OCI-????? ");
			logbuffer.append(logLevel(level))->append(": <error>");
			break;
		case SQLRLOGGER_EVENTTYPE_SQLR_INTERNAL:
			logbuffer.append("<internal error>");
			break;
		default:
			// ignore all other events
			return true;
	}
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
