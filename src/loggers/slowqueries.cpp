// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrlistener.h>
#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlrlogger.h>
#include <cmdline.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/filesystem.h>
#include <rudiments/stringbuffer.h>

class slowqueries : public sqlrlogger {
	public:
			slowqueries(xmldomnode *parameters);
			~slowqueries();

		bool	init(sqlrlistener *sqlrl, sqlrconnection_svr *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrlogger_eventtype_t event,
					const char *info);
	private:
		char		*querylogname;
		file		querylog;
		uint64_t	sec;
		uint64_t	usec;
};

slowqueries::slowqueries(xmldomnode *parameters) : sqlrlogger(parameters) {
	querylogname=NULL;
	sec=charstring::toInteger(parameters->getAttributeValue("sec"));
	usec=charstring::toInteger(parameters->getAttributeValue("usec"));
}

slowqueries::~slowqueries() {
	querylog.flushWriteBuffer(-1,-1);
	delete[] querylogname;
}

bool slowqueries::init(sqlrlistener *sqlrl, sqlrconnection_svr *sqlrcon) {

	// don't log anything for the listener
	if (!sqlrcon) {
		return true;
	}

	// get the pid
	pid_t	pid=process::getProcessId();

	cmdline	*cmdl=sqlrcon->cont->cmdl;

	// build up the query log name
	size_t	querylognamelen;
	delete[] querylogname;
	if (charstring::length(cmdl->getLocalStateDir())) {
		querylognamelen=charstring::length(cmdl->getLocalStateDir())+30+
				charstring::length(cmdl->getId())+10+20+1;
		querylogname=new char[querylognamelen];
		charstring::printf(querylogname,querylognamelen,
					"%s/sqlrelay/log/sqlr-connection-%s"
					"-querylog.%ld",
					cmdl->getLocalStateDir(),
					cmdl->getId(),(long)pid);
	} else {
		querylognamelen=charstring::length(LOG_DIR)+17+
				charstring::length(cmdl->getId())+10+20+1;
		querylogname=new char[querylognamelen];
		charstring::printf(querylogname,querylognamelen,
					"%s/sqlr-connection-%s-querylog.%ld",
					LOG_DIR,cmdl->getId(),(long)pid);
	}

	// remove any old log file
	file::remove(querylogname);

	// create the new log file
	if (!querylog.create(querylogname,
				permissions::evalPermString("rw-------"))) {
		return false;
	}

	// optimize
	filesystem	fs;
	fs.initialize(querylogname);
	querylog.setWriteBufferSize(fs.getOptimumTransferBlockSize());
	return true;
}

bool slowqueries::run(sqlrlistener *sqlrl,
				sqlrconnection_svr *sqlrcon,
				sqlrcursor_svr *sqlrcur,
				sqlrlogger_loglevel_t level,
				sqlrlogger_eventtype_t event,
				const char *info) {

	// don't log anything for the listener
	if (!sqlrcon) {
		return true;
	}

	// don't do anything unless we got INFO/QUERY
	if (level!=SQLRLOGGER_LOGLEVEL_INFO ||
		event!=SQLRLOGGER_EVENTTYPE_QUERY) {
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

	uint64_t	querysec=sqlrcur->queryendsec-sqlrcur->querystartsec;
	uint64_t	queryusec=sqlrcur->queryendusec-sqlrcur->querystartusec;

	if (querysec>sec || (querysec==sec && queryusec>=usec)) {

		stringbuffer	logentry;
		logentry.append("query:\n")->append(sqlrcur->querybuffer);
		logentry.append("\n");
		logentry.append("time: ")->append(sec);
		logentry.append(".");
		char	*usecstr=charstring::parseNumber(usec,6);
		logentry.append(usecstr);
		delete[] usecstr;
		logentry.append("\n");
		if ((size_t)querylog.write(logentry.getString(),
					logentry.getStringLength())!=
						logentry.getStringLength()) {
			return false;
		}
	}
	return true;
}

extern "C" {
	sqlrlogger	*new_slowqueries(xmldomnode *parameters) {
		return new slowqueries(parameters);
	}
}
