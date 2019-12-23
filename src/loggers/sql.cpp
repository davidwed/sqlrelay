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
		uint64_t	sec;
		uint64_t	usec;
		uint64_t	totalusec;
		bool		enabled;
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
	pid_t	pid=process::getProcessId();

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
				permissions::evalPermString("rw-------"))) {
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

	// don't do anything unless we got INFO/QUERY
	if (level!=SQLRLOGGER_LOGLEVEL_INFO || event!=SQLREVENT_QUERY) {
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

	// log query (and error, if there was one)
	stringbuffer	logentry;
	logentry.append(sqlrcur->getQueryBuffer());
	logentry.append(";\n");
	if (!charstring::isNullOrEmpty(sqlrcur->getErrorBuffer())) {
		logentry.append("-- ERROR: ");
		logentry.append(sqlrcur->getErrorBuffer());
		logentry.append("\n");
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
