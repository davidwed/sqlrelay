// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/directory.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/datetime.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>
#include <defines.h>

class SQLRSERVER_DLLSPEC sqlrlogger_custom_nw : public sqlrlogger {
	public:
			sqlrlogger_custom_nw(sqlrloggers *ls,
						domnode *parameters);
			~sqlrlogger_custom_nw();

		bool	init(sqlrlistener *sqlrl, sqlrserverconnection *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrevent_t event,
					const char *info);
	private:
		int	strescape(const char *str, char *buf, int limit);
		bool	descInputBinds(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						char *buf, int limit);
		file	querylog;
		char	*querylogname;
		char	querylogbuf[102400];
		bool	enabled;
};

sqlrlogger_custom_nw::sqlrlogger_custom_nw(sqlrloggers *ls,
						domnode *parameters) :
						sqlrlogger(ls,parameters) {
	querylogname=NULL;
	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
}

sqlrlogger_custom_nw::~sqlrlogger_custom_nw() {
	delete[] querylogname;
}

bool sqlrlogger_custom_nw::init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon) {
	debugFunction();

	if (!enabled) {
		return true;
	}

	const char	*logdir=
			(sqlrcon)?sqlrcon->cont->getLogDir():sqlrl->getLogDir();
	const char	*id=
			(sqlrcon)?sqlrcon->cont->getId():sqlrl->getId();

	// create the directory
	delete[] querylogname;
	charstring::printf(&querylogname,"%s/%s",logdir,id);
	directory::create(querylogname,
			permissions::evalPermString("rwxrwxrwx"));

	// create the log file name
	delete[] querylogname;
	charstring::printf(&querylogname,"%s/%s/query.log",logdir,id);

	// create the new log file
	querylog.close();
	return querylog.open(querylogname,O_WRONLY|O_CREAT|O_APPEND,
				permissions::evalPermString("rw-------"));
}

bool sqlrlogger_custom_nw::run(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrlogger_loglevel_t level,
				sqlrevent_t event,
				const char *info) {
	debugFunction();

	if (!enabled) {
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
			init(sqlrl,sqlrcon);
		}
	}

	// get error, if there was one
	static char	errorcodebuf[100+1];
	errorcodebuf[0]='\0';
	if (charstring::isNullOrEmpty(sqlrcur->getErrorBuffer())) {
		charstring::copy(errorcodebuf,"0");
	} else {
		charstring::printf(errorcodebuf,100,"%s",
					sqlrcur->getErrorBuffer());
	}

	// escape the query
	static char	sqlbuf[7000+1];
	strescape(sqlrcur->getQueryBuffer(),sqlbuf,7000);

	// escape the client info
	static char	infobuf[1024+1];
	strescape(sqlrcon->cont->getClientInfo(),infobuf,1024);

	// escape the input bind variables
	char	bindbuf[1000+1];
	descInputBinds(sqlrcon,sqlrcur,bindbuf,1000);

	// get the execution time
	uint64_t	sec=sqlrcur->getCommandEndSec()-
				sqlrcur->getCommandStartSec();
	uint64_t	usec=sqlrcur->getCommandEndUSec()-
				sqlrcur->getCommandStartUSec();
	
	// get the current date/time
	datetime	dt;
	dt.getSystemDateAndTime();

	// write everything into an output buffer, pipe-delimited
	charstring::printf(querylogbuf,sizeof(querylogbuf)-1,
		"%04d-%02d-%02d %02d:%02d:%02d|%d|%f|%s|%lld|%s|%s|%f|%s|%s|\n",
		dt.getYear(),
		dt.getMonth(),
		dt.getDayOfMonth(),
		dt.getHour(),
		dt.getMinutes(),
		dt.getSeconds(),
		sqlrcon->cont->getStatisticsIndex(),
		((double)(sec*1000000+usec))/1000000.0,
		errorcodebuf,
		(long long)sqlrcur->getTotalRowsFetched(),
		infobuf,
		sqlbuf,
		((double)(sec*1000000+usec))/1000000.0,
		sqlrcon->cont->getClientAddr(),
		bindbuf
		);

	// write that buffer to the log file
	return ((size_t)querylog.write(querylogbuf)==
				charstring::length(querylogbuf));
}

int sqlrlogger_custom_nw::strescape(const char *str, char *buf, int limit) {
	// from oracpool my_strescape()
	char		*q=buf;
	const char	*strend=str+charstring::length(str);
	for (const char *p=str; p<strend; p++) {
		if (q-buf>=limit-1) {
			break;
		} else if (*p=='\n') { 
			*(q++)='\\';
			*(q++)='n';
		} else if (*p=='\r') { 
			*(q++)='\\';
			*(q++)='r';
		} else if (*p=='|') { 
			*(q++)='\\';
			*(q++)='|';
		} else if (*p=='\\') { 
			*(q++)='\\';
			*(q++)='\\';
		} else { 
			*(q++)=*p;
		}
	}
	*q='\0';
	return (q-buf);
}

bool sqlrlogger_custom_nw::descInputBinds(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						char *buf, int limit) {

	char		*c=buf;	
	int		remain_len=limit;
	int		write_len=0;
	static char	bindstrbuf[512+1];

	*c='\0';

	// fill the buffers
	sqlrserverbindvar	*inbinds=sqlrcon->cont->getInputBinds(sqlrcur);
	for (uint16_t i=0; i<sqlrcon->cont->getInputBindCount(sqlrcur); i++) {

		sqlrserverbindvar	*bv=&(inbinds[i]);
	
		write_len=charstring::printf(
				c,remain_len,"[%s => ",bv->variable);
		c+=write_len;

		remain_len-=write_len;
		if (remain_len<=0) {
			return false;
		}

		if (bv->type==SQLRSERVERBINDVARTYPE_NULL) {
			write_len=charstring::printf(c,remain_len,"NULL]");
		} else if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
			strescape(bv->value.stringval,bindstrbuf,512);
			write_len=charstring::printf(
					c,remain_len,"'%s']",bindstrbuf);
		} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {
			write_len=charstring::printf(
					c,remain_len,"'%lld']",
					(long long)bv->value.integerval);
		} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
			write_len=charstring::printf(
					c,remain_len,"%f]",
					bv->value.doubleval.value);
		} else if (bv->type==SQLRSERVERBINDVARTYPE_BLOB ||
				bv->type==SQLRSERVERBINDVARTYPE_CLOB) {
			write_len=charstring::printf(
					c,remain_len,"LOB]");
		}

		c+=write_len;
		remain_len-=write_len;

		if (remain_len<=0) {
			return false;
		}
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrlogger *new_sqlrlogger_custom_nw(
						sqlrloggers *ls,
						domnode *parameters) {
		return new sqlrlogger_custom_nw(ls,parameters);
	}
}
