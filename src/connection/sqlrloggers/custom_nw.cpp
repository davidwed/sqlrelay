// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrloggers/custom_nw.h>
#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <cmdline.h>
#include <rudiments/charstring.h>
#include <rudiments/directory.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/filesystem.h>
#include <rudiments/datetime.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqlrlogger	*new_custom_nw(xmldomnode *parameters) {
		return new custom_nw(parameters);
	}
}

custom_nw::custom_nw(xmldomnode *parameters) : sqlrlogger(parameters) {
	querylogname=NULL;
}

bool custom_nw::init(sqlrconnection_svr *sqlrcon) {
	debugFunction();

	cmdline	*cmdl=sqlrcon->cont->cmdl;

	// build up the query log name
	size_t	querylognamelen;
	delete[] querylogname;
	if (charstring::length(cmdl->getLocalStateDir())) {

		// create the directory
		querylognamelen=charstring::length(cmdl->getLocalStateDir())+14+
				charstring::length(cmdl->getId())+1+1;
		querylogname=new char[querylognamelen];
		snprintf(querylogname,querylognamelen,
				"%s/sqlrelay/log/%s",
				cmdl->getLocalStateDir(),cmdl->getId());
		directory::create(querylogname,
				permissions::evalPermString("rwxrwxrwx"));

		// create the log file name
		querylognamelen=charstring::length(cmdl->getLocalStateDir())+14+
				charstring::length(cmdl->getId())+10+1;
		delete[] querylogname;
		querylogname=new char[querylognamelen];
		snprintf(querylogname,querylognamelen,
				"%s/sqlrelay/log/%s/query.log",
				cmdl->getLocalStateDir(),cmdl->getId());
	} else {

		// create the directory
		querylognamelen=charstring::length(LOG_DIR)+1+
				charstring::length(cmdl->getId())+1+1;
		querylogname=new char[querylognamelen];
		snprintf(querylogname,querylognamelen,
				"%s/%s",LOG_DIR,cmdl->getId());
		directory::create(querylogname,
				permissions::evalPermString("rwxrwxrwx"));

		// create the log file name
		querylognamelen=charstring::length(LOG_DIR)+1+
				charstring::length(cmdl->getId())+10+1;
		delete[] querylogname;
		querylogname=new char[querylognamelen];
		snprintf(querylogname,querylognamelen,
				"%s/%s/query.log",LOG_DIR,cmdl->getId());
	}

	// create the new log file
	querylog.close();
	return querylog.open(querylogname,O_WRONLY|O_CREAT|O_APPEND,
				permissions::evalPermString("rw-------"));
}

bool custom_nw::run(sqlrconnection_svr *sqlrcon, sqlrcursor_svr *sqlrcur) {
	debugFunction();

	// reinit the log if the file was switched
	ino_t	inode1=querylog.getInode();
	ino_t	inode2;
	if (!file::getInode(querylogname,&inode2) || inode1!=inode2) {
		init(sqlrcon);
	}

	// get error, if there was one
	static char	errorcodebuf[100+1];
	errorcodebuf[0]='\0';
	if (sqlrcur->queryresult) {
		charstring::copy(errorcodebuf,"0");
	} else {
		snprintf(errorcodebuf,100,sqlrcur->error);
	}

	// escape the query
	static char	sqlbuf[7000+1];
	strescape(sqlrcur->querybuffer,sqlbuf,7000);

	// escape the client info
	static char	infobuf[1024+1];
	strescape(sqlrcon->cont->clientinfo,infobuf,1024);

	// escape the input bind variables
	char	bindbuf[1000+1];
	descInputBinds(sqlrcur,bindbuf,1000);

	// get the client address
	char	*clientaddrbuf=NULL;
	if (sqlrcon->cont->clientsock) {
		clientaddrbuf=sqlrcon->cont->clientsock->getPeerAddress();
		if (!clientaddrbuf) {
			clientaddrbuf=charstring::duplicate("UNIX");
		}
	} else {
		clientaddrbuf=charstring::duplicate("internal");
	}

	// get the execution time
	uint64_t	sec=sqlrcur->commandendsec-sqlrcur->commandstartsec;
	uint64_t	usec=sqlrcur->commandendusec-sqlrcur->commandstartusec;
	
	// get the current date/time
	datetime	dt;
	dt.getSystemDateAndTime();

	// write everything into an output buffer, pipe-delimited
	snprintf(querylogbuf,sizeof(querylogbuf)-1,
		"%04d-%02d-%02d %02d:%02d:%02d|%d|%f|%s|%lld|%s|%s|%f|%s|%s|\n",
		dt.getYear(),
		dt.getMonth(),
		dt.getDayOfMonth(),
		dt.getHour(),
		dt.getMinutes(),
		dt.getSeconds(),
		sqlrcon->cont->connstats->index,
		sec+usec/1000000.0,
		errorcodebuf,
		(long long)((sqlrcur->lastrowvalid)?sqlrcur->lastrow:0),
        	infobuf,
		sqlbuf,
		sec+usec/1000000.0,
		clientaddrbuf,
		bindbuf
		);

	// clean up
	delete[] clientaddrbuf;

	// write that buffer to the log file
	return ((size_t)querylog.write(querylogbuf)==
				charstring::length(querylogbuf));
}

int custom_nw::strescape(const char *str, char *buf, int limit) {
	// from oracpool my_strescape()
	register char	*q=buf;
	const char	*strend=str+charstring::length(str);
	for (register const char *p=str; p<strend; p++) {
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

bool custom_nw::descInputBinds(sqlrcursor_svr *cursor, char *buf, int limit) {

	char		*c=buf;	
	int		remain_len=limit;
	int		write_len=0;
	static char	bindstrbuf[512+1];

	*c='\0';

	// fill the buffers
	for (uint16_t i=0; i<cursor->inbindcount; i++) {

		bindvar_svr	*bv=&(cursor->inbindvars[i]);
	
		write_len=snprintf(c,remain_len,"[%s => ",bv->variable);
		c+=write_len;

		remain_len-=write_len;
		if (remain_len<=0) {
			return false;
		}

		if (bv->type==NULL_BIND) {
			write_len=snprintf(c,remain_len,"NULL]");
		} else if (bv->type==STRING_BIND) {
			strescape(bv->value.stringval,bindstrbuf,512);
			write_len=snprintf(c,remain_len,"'%s']",bindstrbuf);
		} else if (bv->type==INTEGER_BIND) {
			write_len=snprintf(c,remain_len,"'%lld']",
					(long long)bv->value.integerval);
		} else if (bv->type==DOUBLE_BIND) {
			write_len=snprintf(c,remain_len,"%lf]",
					bv->value.doubleval.value);
		} else if (bv->type==BLOB_BIND || bv->type==CLOB_BIND) {
			write_len=snprintf(c,remain_len,"LOB]");
		}

		c+=write_len;
		remain_len-=write_len;

		if (remain_len<=0) {
			return false;
		}
	}
	return true;
}
