// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/filesystem.h>
#include <rudiments/stringbuffer.h>

class SQLRSERVER_DLLSPEC sqlrlogger_sql : public sqlrlogger {
	public:
			sqlrlogger_sql(sqlrloggers *ls, domnode *parameters);
			~sqlrlogger_sql();

		bool	init(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrevent_t event,
					const char *info);
	private:
		char		*querylogname;
		file		querylog;
		bool		enabled;
		pid_t		pid;
};

sqlrlogger_sql::sqlrlogger_sql(sqlrloggers *ls, domnode *parameters) :
						sqlrlogger(ls,parameters) {
	querylogname=NULL;
	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
}

sqlrlogger_sql::~sqlrlogger_sql() {
	querylog.flushWriteBuffer(-1,-1);
	delete[] querylogname;
}

bool sqlrlogger_sql::init(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon) {

	if (!enabled) {
		return true;
	}

	// don't log anything for the listener
	if (!sqlrcon) {
		return true;
	}

	// get the pid
	pid=process::getProcessId();

	// build up the query log name
	delete[] querylogname;
	charstring::printf(&querylogname,
				"%s/sqlr-connection-%s-querylog.%ld",
				sqlrcon->cont->getLogDir(),
				sqlrcon->cont->getId(),(long)pid);

	// remove any old log file
	file::remove(querylogname);

	// create the new log file
	if (!querylog.create(querylogname,
				permissions::parsePermString("rw-------"))) {
		return false;
	}

	// optimize
	filesystem	fs;
	fs.open(querylogname);
	querylog.setWriteBufferSize(fs.getOptimumTransferBlockSize());
	return true;
}

bool sqlrlogger_sql::run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrevent_t event,
					const char *info) {

	if (!enabled) {
		return true;
	}

	// don't log anything for the listener
	if (!sqlrcon) {
		return true;
	}

	// don't do anything unless we got INFO/QUERY/TX
	if (level!=SQLRLOGGER_LOGLEVEL_INFO ||
		(event!=SQLREVENT_QUERY &&
		event!=SQLREVENT_BEGIN_TRANSACTION &&
		event!=SQLREVENT_ROLLBACK &&
		event!=SQLREVENT_COMMIT)) {
		return true;
	}

	// reinit the log if the file was switched
	file	querylog2;
	if (querylog2.open(querylogname,O_RDONLY)) {
		ino_t	inode1=querylog.getInode();
		ino_t	inode2=querylog2.getInode();
		querylog2.close();
		if (inode1!=inode2) {
			querylog.flushWriteBuffer(-1,-1);
			querylog.close();
			init(sqlrl,sqlrcon);
		}
	}

	stringbuffer	logentry;

	// log pid changes
	if (process::getProcessId()!=pid) {
		pid=process::getProcessId();
		logentry.append("-- pid changed to ");
		logentry.append((uint64_t)pid);
		logentry.append('\n');
	}

	// log query (and error, if there was one)
	if (event==SQLREVENT_QUERY) {
		logentry.append(sqlrcon->cont->getQueryBuffer(sqlrcur));
		logentry.append(";\n");
		if (sqlrcon->cont->getErrorLength(sqlrcur)) {
			logentry.append("-- ERROR: ");
			logentry.append(sqlrcon->cont->getErrorBuffer(sqlrcur));
			logentry.append("\n");
		}
	} else {
		if (event==SQLREVENT_BEGIN_TRANSACTION) {
			logentry.append("begin;\n");
		} else if (event==SQLREVENT_ROLLBACK) {
			logentry.append("rollback;\n");
		} else if (event==SQLREVENT_COMMIT) {
			logentry.append("commit;\n");
		}
		if (sqlrcon->cont->getErrorLength()) {
			logentry.append("-- ERROR: ");
			logentry.append(sqlrcon->cont->getErrorBuffer());
			logentry.append("\n");
		}
	}
	if ((size_t)querylog.write(logentry.getString(),
				logentry.getStringLength())!=
				logentry.getStringLength()) {
		return false;
	}
	//querylog.flushWriteBuffer(-1,-1);
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrlogger *new_sqlrlogger_sql(
						sqlrloggers *ls,
						domnode *parameters) {
		return new sqlrlogger_sql(ls,parameters);
	}
}
