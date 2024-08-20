// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/filesystem.h>

class SQLRSERVER_DLLSPEC sqlrlogger_slowqueries : public sqlrlogger {
	public:
		sqlrlogger_slowqueries(domnode *parameters);
		~sqlrlogger_slowqueries();

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
		uint64_t	sec;
		uint64_t	usec;
		uint64_t	totalusec;
		bool		usecommand;
		bool		sync;

		datetime	dt;
		char		datebuffer[27];
		char		querysecbuffer[7];
};

sqlrlogger_slowqueries::sqlrlogger_slowqueries(domnode *parameters) :
						sqlrlogger(parameters) {
	querylogname=NULL;
	sec=charstring::convertToInteger(
				parameters->getAttributeValue("sec"));
	usec=charstring::convertToInteger(
				parameters->getAttributeValue("usec"));
	totalusec=sec*1000000+usec;
	usecommand=!charstring::compareIgnoringCase(
			parameters->getAttributeValue("timer"),"command");
	sync=charstring::isYes(parameters->getAttributeValue("sync"));
}

sqlrlogger_slowqueries::~sqlrlogger_slowqueries() {
	querylog.flushWriteBuffer(-1,-1);
	delete[] querylogname;
}

bool sqlrlogger_slowqueries::init(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon) {

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

static const char *days[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

bool sqlrlogger_slowqueries::run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrloglevel_t level,
					sqlrevent_t event,
					const char *info) {

	// don't log anything for the listener
	if (!sqlrcon) {
		return true;
	}

	// don't do anything unless we got INFO/QUERY
	if (level!=SQLRLOGGER_LOGLEVEL_INFO ||
			event!=SQLREVENT_QUERY_EXECUTED) {
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

	// calculate times
	uint64_t	startsec=(usecommand)?sqlrcur->getCommandStartSec():
						sqlrcur->getQueryStartSec();
	uint64_t	startusec=(usecommand)?sqlrcur->getCommandStartUSec():
						sqlrcur->getQueryStartUSec();
	uint64_t	endsec=(usecommand)?sqlrcur->getCommandEndSec():
						sqlrcur->getQueryEndSec();
	uint64_t	endusec=(usecommand)?sqlrcur->getCommandEndUSec():
						sqlrcur->getQueryEndUSec();

	uint64_t	queryusec=((endsec-startsec)*1000000)+
							endusec-startusec;
	double		querysec=((double)queryusec)/1000000.0;

	// log times
	if (queryusec>=totalusec) {

		// get the date/time as a string
		dt.initFromSystemDateTime();
		charstring::printf(datebuffer,sizeof(datebuffer),
					"%s %d %s % 2d  %02d:%02d:%02d",
					days[dt.getDayOfWeek()-1],
					dt.getYear(),
					dt.getMonthAbbreviation(),
					dt.getDayOfMonth(),
					dt.getHour(),
					dt.getMinute(),
					dt.getSecond());

		// get the query duration as a string
		charstring::printf(querysecbuffer,sizeof(querysecbuffer),
					"%.*f",6,querysec);
		
		// write the log entry
		if (querylog.write(datebuffer)!=
				(ssize_t)charstring::getLength(datebuffer) ||
			querylog.write(" :\n",3)!=3 ||
			querylog.write(sqlrcur->getQueryBuffer(),
					sqlrcur->getQuerySize())!=
				(ssize_t)sqlrcur->getQuerySize() ||
			querylog.write('\n')!=1 ||
			querylog.write("execution time: ",16)!= 16 ||
			querylog.write(querysecbuffer)!=
				(ssize_t)charstring::getLength(
							querysecbuffer) ||
			querylog.write('\n')!=1) {
			return false;
		}

		// flush, if we need to
		if (sync) {
			querylog.flushWriteBuffer(-1,-1);
		}
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrlogger *new_sqlrlogger_slowqueries(
						domnode *parameters) {
		return new sqlrlogger_slowqueries(parameters);
	}
}
