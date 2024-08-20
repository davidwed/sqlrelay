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
		sqlrlogger_sql(domnode *parameters);
		~sqlrlogger_sql();

		bool	init(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrloglevel_t level,
					sqlrevent_t event,
					const char *info);
	private:
		char		*querylogname;
		file		querylog;
		bool		sync;
		sqlrevent_t	queryevent;
		bool		logpidchange;
		bool		logerrors;
		pid_t		pid;
};

sqlrlogger_sql::sqlrlogger_sql(domnode *parameters) : sqlrlogger(parameters) {
	querylogname=NULL;
	sync=charstring::isYes(parameters->getAttributeValue("sync"));
	const char	*qestr=parameters->getAttributeValue("queryevent");
	if (charstring::isNullOrEmpty(qestr)) {
		qestr="executed";
	}
	if (!charstring::compare(qestr,"received")) {
		queryevent=SQLREVENT_QUERY_RECEIVED;
	} else if (!charstring::compare(qestr,"prepared")) {
		queryevent=SQLREVENT_QUERY_PREPARED;
	} else if (!charstring::compare(qestr,"executed")) {
		queryevent=SQLREVENT_QUERY_EXECUTED;
	}
	logpidchange=!charstring::isNo(
			parameters->getAttributeValue("logpidchange"));
	logerrors=!charstring::isNo(
			parameters->getAttributeValue("logerrors"));
}

sqlrlogger_sql::~sqlrlogger_sql() {
	querylog.flushWriteBuffer(-1,-1);
	delete[] querylogname;
}

bool sqlrlogger_sql::init(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon) {

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
				sqlrcon->cont->getPaths()->getLogDir(),
				sqlrcon->cont->getId(),(long)pid);

	// remove any old log file
	file::remove(querylogname);

	// create the new log file
	if (!querylog.create(querylogname,
				permissions::parsePermString("rw-------"))) {
		return false;
	}

	// optimize
	querylog.setWriteBufferSize(
			filesystem::getOptimumTransferBlockSize(querylogname));
	return true;
}

bool sqlrlogger_sql::run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrloglevel_t level,
					sqlrevent_t event,
					const char *info) {

	// don't log anything for the listener
	if (!sqlrcon) {
		return true;
	}

	// don't do anything unless we got INFO/QUERY/TX
	if (level!=SQLRLOGGER_LOGLEVEL_INFO ||
		(event!=queryevent &&
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

	// log pid changes
	if (logpidchange && process::getProcessId()!=pid) {
		pid=process::getProcessId();
		if (querylog.write("-- pid changed to ",18)!=18) {
			return false;
		}
		querylog.printf("%lld",(uint64_t)pid);
		if (querylog.write('\n')!=1) {
			return false;
		}
	}

	// log query (and error, if there was one)
	if (event==queryevent) {
		if ((uint32_t)querylog.write(sqlrcur->getQueryBuffer(),
						sqlrcur->getQuerySize())!=
						sqlrcur->getQuerySize() ||
			querylog.write(";\n",2)!=2) {
			return false;
		}
		if (logerrors && sqlrcur->getErrorSize()) {
			if (querylog.write("-- ERROR: ",10)!=10 ||
				(uint32_t)querylog.write(
						sqlrcur->getErrorBuffer(),
						sqlrcur->getErrorSize())!=
						sqlrcur->getErrorSize() ||
				querylog.write('\n')!=1) {
				return false;
			}
		}
	} else {
		if (event==SQLREVENT_BEGIN_TRANSACTION) {
			if (querylog.write("begin;\n",7)!=7) {
				return false;
			}
		} else if (event==SQLREVENT_ROLLBACK) {
			if (querylog.write("rollback;\n")!=10) {
				return false;
			}
		} else if (event==SQLREVENT_COMMIT) {
			if (querylog.write("commit;\n")!=8) {
				return false;
			}
		}
		if (logerrors && sqlrcon->cont->getErrorSize()) {
			if (querylog.write("-- ERROR: ",10)!=10 ||
				(uint32_t)querylog.write(
						sqlrcon->getErrorBuffer(),
						sqlrcon->getErrorSize())!=
						sqlrcon->getErrorSize() ||
				querylog.write('\n')!=1) {
				return false;
			}
		}
	}

	// flush, if we need to
	if (sync) {
		querylog.flushWriteBuffer(-1,-1);
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrlogger *new_sqlrlogger_sql(domnode *parameters) {
		return new sqlrlogger_sql(parameters);
	}
}
