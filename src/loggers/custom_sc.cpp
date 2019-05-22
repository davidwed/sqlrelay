// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/datetime.h>
#include <rudiments/error.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>
#include <config.h>

class SQLRSERVER_DLLSPEC sqlrlogger_custom_sc : public sqlrlogger {
	public:
			sqlrlogger_custom_sc(sqlrloggers *ls,
						domnode *parameters);
			~sqlrlogger_custom_sc();

		bool	init(sqlrlistener *sqlrl, sqlrserverconnection *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrevent_t event,
					const char *info);
	private:
		file	querylog;
		char	*querylogname;
		sqlrlogger_loglevel_t	loglevel;
		stringbuffer		logbuffer;
		bool			enabled;
};

sqlrlogger_custom_sc::sqlrlogger_custom_sc(sqlrloggers *ls,
						domnode *parameters) :
						sqlrlogger(ls,parameters) {
	querylogname=NULL;
	loglevel=SQLRLOGGER_LOGLEVEL_ERROR;
	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
}

sqlrlogger_custom_sc::~sqlrlogger_custom_sc() {
	delete[] querylogname;
}

bool sqlrlogger_custom_sc::init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	// get log level
	const char	*ll=getParameters()->getAttributeValue("loglevel");
	if (!charstring::compareIgnoringCase(ll,"info")) {
		loglevel=SQLRLOGGER_LOGLEVEL_INFO;
	} else if (!charstring::compareIgnoringCase(ll,"warning")) {
		loglevel=SQLRLOGGER_LOGLEVEL_WARNING;
	} else {
		loglevel=SQLRLOGGER_LOGLEVEL_ERROR;
	}

	// get log path and name
	const char	*path=getParameters()->getAttributeValue("path");
	if (!charstring::length(path)) {
		path=(sqlrcon)?sqlrcon->cont->getLogDir():sqlrl->getLogDir();
	}
	const char	*name=getParameters()->getAttributeValue("name");
	if (!charstring::length(name)) {
		name="sqlrelay.log";
	}

	// build up the query log name
	delete[] querylogname;
	charstring::printf(&querylogname,"%s/%s",path,name);

	// create the new log file
	querylog.close();
	return querylog.open(querylogname,O_WRONLY|O_CREAT|O_APPEND,
				permissions::evalPermString("rw-------"));
}

bool sqlrlogger_custom_sc::run(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrlogger_loglevel_t level,
				sqlrevent_t event,
				const char *info) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	// bail if log level is too low
	if (level<loglevel) {
		return true;
	}

	// reinit the log if the file was switched
	file	querylog2;
	if (querylog2.open(querylogname,O_RDONLY)) {
		ino_t	inode1=querylog.getInode();
		ino_t	inode2=querylog2.getInode();
		querylog2.close();
		if (inode1!=inode2) {
			init(sqlrl,sqlrcon);
		}
	}

	// get the current date
	datetime	dt;
	dt.getSystemDateAndTime();

	// clear log buffer
	logbuffer.clear();

	// append the date
	char	datebuffer[20];
	charstring::printf(datebuffer,sizeof(datebuffer),
				"%04d-%02d-%02d %02d:%02d:%02d",
				dt.getYear(),dt.getMonth(),dt.getDayOfMonth(),
				dt.getHour(),dt.getMinutes(),dt.getSeconds());
	logbuffer.append(datebuffer)->append(' ');

	// append the event type and log level
	// (except for db errors/warnings which are handled specially)
	if (event!=SQLREVENT_DB_ERROR && event!=SQLREVENT_DB_WARNING) {
		logbuffer.append(getLoggers()->eventType(event))->append(' ');
		logbuffer.append(getLoggers()->logLevel(level))->append(": ");
	}

	// get the client IP, it's needed for some events
	const char	*clientaddr="unknown";
	if (sqlrcon && sqlrcon->cont->getClientAddr()) {
		clientaddr=sqlrcon->cont->getClientAddr();
	}

	// handle each event differently...
	switch (event) {
		case SQLREVENT_CLIENT_CONNECTED:
			logbuffer.append("Client ");
			logbuffer.append(clientaddr);
			logbuffer.append(" connected");
			break;
		case SQLREVENT_CLIENT_CONNECTION_REFUSED:
			logbuffer.append("Client ");
			logbuffer.append(clientaddr);
			logbuffer.append(" connection refused");
			break;
		case SQLREVENT_CLIENT_DISCONNECTED:
			logbuffer.append("Client ");
			logbuffer.append(clientaddr);
			logbuffer.append(" disconnected");
			break;
		case SQLREVENT_CLIENT_PROTOCOL_ERROR:
			logbuffer.append("Client ");
			logbuffer.append(clientaddr);
			logbuffer.append(" protocol error");
			break;
		case SQLREVENT_DB_LOGIN:
			logbuffer.append(SQLRELAY);
			logbuffer.append(" logged in to DB ");
			logbuffer.append(sqlrcon->cont->dbIpAddress());
			break;
		case SQLREVENT_DB_LOGOUT:
			logbuffer.append(SQLRELAY);
			logbuffer.append(" logged out of DB ");
			logbuffer.append(sqlrcon->cont->dbIpAddress());
			break;
		case SQLREVENT_DB_ERROR:
		case SQLREVENT_DB_WARNING:
			{
			const char	*colon=charstring::findFirst(info,':');
			if (colon) {
				logbuffer.append(info,colon-info)->append(' ');
				logbuffer.append(getLoggers()->
						logLevel(level))->append(": ");
				logbuffer.append(colon+2);
			} else {
				logbuffer.append(getLoggers()->
						eventType(event))->append(' ');
				logbuffer.append(getLoggers()->
						logLevel(level))->append(": ");
				logbuffer.append(info);
			}
			}
			break;
		case SQLREVENT_INTERNAL_ERROR:
			logbuffer.append(SQLRELAY);
			logbuffer.append(" internal error");
			break;
		case SQLREVENT_INTERNAL_WARNING:
			logbuffer.append(SQLRELAY);
			logbuffer.append(" internal warning");
			break;
		default:
			// ignore all other events
			return true;
	}

	// append info, if there was any
	// (except for db errors/warnings which are handled specially)
	if (charstring::length(info) &&
		(event!=SQLREVENT_DB_ERROR && event!=SQLREVENT_DB_WARNING)) {
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
	SQLRSERVER_DLLSPEC sqlrlogger *new_sqlrlogger_custom_sc(
						sqlrloggers *ls,
						domnode *parameters) {
		return new sqlrlogger_custom_sc(ls,parameters);
	}
}
