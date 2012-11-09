// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <rudiments/file.h>
#include <rudiments/filesystem.h>
#include <rudiments/directory.h>
#include <rudiments/charstring.h>
#include <rudiments/permissions.h>
#include <rudiments/datetime.h>
#include <rudiments/process.h>

int strescape(const char *str, char *buf, int limit) {
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

bool descInputBinds(sqlrcursor_svr *cursor, char *buf, int limit) {

	char		*c=buf;	
	int		remain_len=limit;
	int		write_len=0;
	static char	bindstrbuf[512+1];

	*c=0;

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
						bv->value.integerval);
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

bool sqlrconnection_svr::initQueryLog() {

	// get the pid
	pid_t	pid=process::getProcessId();

	// build up the query log name
	size_t	querylognamelen;
	delete[] querylogname;
	if (charstring::length(cmdl->getLocalStateDir())) {
		querylognamelen=charstring::length(cmdl->getLocalStateDir())+30+
				charstring::length(cmdl->getId())+10+20+1;
		querylogname=new char[querylognamelen];
		snprintf(querylogname,querylognamelen,
				"%s/sqlrelay/log/sqlr-connection-%s"
				"-querylog.%d",
				cmdl->getLocalStateDir(),cmdl->getId(),pid);
	} else {
		querylognamelen=charstring::length(LOG_DIR)+17+
				charstring::length(cmdl->getId())+10+20+1;
		querylogname=new char[querylognamelen];
		snprintf(querylogname,querylognamelen,
				"%s/sqlr-connection-%s-querylog.%d",
				LOG_DIR,cmdl->getId(),pid);
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

bool sqlrconnection_svr::writeQueryLog(sqlrcursor_svr *cursor) {

	// reinit the log if the file was switched
	ino_t	inode1=querylog.getInode();
	ino_t	inode2;
	if (!file::getInode(querylogname,&inode2) || inode1!=inode2) {
		querylog.flushWriteBuffer(-1,-1);
		querylog.close();
		initQueryLog();
	}

// Original stuff...

	stringbuffer	logentry;
	logentry.append("query:\n")->append(cursor->stats.query)->append("\n");
	logentry.append("time: ")->append((uint64_t)cursor->stats.sec);
	logentry.append(".");
	char	*usec=charstring::parseNumber((uint64_t)cursor->stats.usec,6);
	logentry.append(usec);
	delete[] usec;
	logentry.append("\n");
	if (querylog.write(logentry.getString(),
				logentry.getStringLength())!=
					logentry.getStringLength()) {
		return false;
	}


// Neowiz stuff...

	// get error, if there was one
	static char	errorcodebuf[100+1];
	errorcodebuf[0]='\0';
	if (cursor->stats.result) {
		charstring::copy(errorcodebuf,"0");
	} else {
		snprintf(errorcodebuf,100,cursor->stats.error);
	}

	// escape the query
	static char	sqlbuf[7000+1];
	strescape(cursor->stats.query,sqlbuf,7000);

	// escape the client info
	static char	infobuf[1024+1];
	strescape(clientinfo,infobuf,1024);

	// escape the input bind variables
	char	bindbuf[1000+1];
	descInputBinds(cursor,bindbuf,1000);

	// get the client address
	char	*clientaddrbuf=NULL;
	if (clientsock) {
		clientaddrbuf=clientsock->getPeerAddress();
		if (!clientaddrbuf) {
			clientaddrbuf=charstring::duplicate("UNIX");
		}
	} else {
		clientaddrbuf=charstring::duplicate("internal");
	}
	
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
		handoffindex, 
		// FIXME: validate that this is really the time they
		// intend to store here
		cursor->stats.sec+cursor->stats.usec/1000000.0,
		errorcodebuf,
		(cursor->lastrowvalid)?cursor->lastrow:0,
        	infobuf,
		sqlbuf,
		// FIXME: validate that this is really the time they
		// intend to store here
		cursor->stats.sec+cursor->stats.usec/1000000.0,
		clientaddrbuf,
		bindbuf
		);

	// clean up
	delete[] clientaddrbuf;

	// write that buffer to the log file
	return (querylog.write(querylogbuf)==charstring::length(querylogbuf));
}
