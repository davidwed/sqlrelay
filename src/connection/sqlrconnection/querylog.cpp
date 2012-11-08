// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <rudiments/file.h>
#include <rudiments/directory.h>
#include <rudiments/charstring.h>
#include <rudiments/permissions.h>
#include <rudiments/datetime.h>

// for gettimeofday()
#include <sys/time.h>

int strescape(char *str, char *buf, int limit)
// from oracpool my_strescape()
{
  register char *p = NULL, *q = buf;
  char *strend = str + charstring::length(str);

  for (p = str; p < strend; p++)
  { 

    if (q-buf >= limit-1)
        break;

    if (*p == '\n')
    { 
      *(q++) = '\\';
      *(q++) = 'n';
    }
    else if (*p == '\r')
    { 
      *(q++) = '\\';
      *(q++) = 'r';
    }
    else if (*p == '|')
    { 
      *(q++) = '\\';
      *(q++) = '|';
    }
    else if (*p == '\\')
    { 
      *(q++) = '\\';
      *(q++) = '\\';
    }
    else
    { 
      *(q++) = *p;
    }
  }

  *q = '\0';

  return (q - buf);
}

bool sqlrconnection_svr::writeQueryLog(sqlrcursor_svr *cursor, bool success) {

	// reinit the log if the file was switched
	ino_t	inode1=querylog.getInode();
	ino_t	inode2;
	if (!file::getInode(querylogname,&inode2) || inode1!=inode2) {
		// FIXME: implement this...
		//initQueryLog(NULL);
	}

	// get the number of seconds and microseconds since the epoch
	struct timeval	tv;
	gettimeofday(&tv,NULL);

	// from getcommand();
	double	totaltimediff=0.0;
	// FIXME: implement this...
		//(tv.tv_sec-my_cs->processclient_tv.tv_sec)+
		//(tv.tv_usec-my_cs->processclient_tv.tv_usec)/1000000.0;

	// from processQuery();
	double	qptimediff=0.0;
	// FIXME: implement this...
		//(tv.tv_sec-my_cs->processquery_tv.tv_sec)+
		//(tv.tv_usec-my_cs->processquery_tv.tv_usec)/1000000.0;


	// get error, if there was one
	// FIXME:  Errors should be handled in more structured way --replica
	static char	errorcodebuf[100+1];
	errorcodebuf[0]='\0';
	// FIXME: implement this...
	/*if (success) {
		charstring::copy(errorcodebuf,"0");
	} else if (cursor->sqlr_error[0]) {
		charstring::copy(errorcodebuf,cursor->sqlr_error,100);
	} else if (cursor->sqlrcmd_error[0]) {
		charstring::copy(errorcodebuf,cursor->sqlrcmd_error,100);
	} else {
		cursor->errorCode(errorcodebuf,100);
	}*/

	// write the query into a buffer and escape it
	static char	sqlbuf[7000+1];
	strescape(cursor->querybuffer,sqlbuf,7000);

	// write the client info into a buffer and escape it
	static char	infobuf[1024+1];
	strescape(clientinfo,infobuf,1024);

	// write the input bind values into a buffer
	char		bindbuf[1000+1];
	bindbuf[0]='\0';
	// FIXME: implement this...
	//descInputBinds(cursor,bindbuf,1000);

	// write the client address into a buffer
	static char	clientaddrbuf[100+1];
        charstring::copy(clientaddrbuf,"0.0.0.0");
	// FIXME: implement this...
        //charstring::copy(clientaddrbuf,
	//		nwzUtil::getClientAddr(&my_cs->clientaddr));
	
	// get the current date/time
	datetime	dt;
	dt.getSystemDateAndTime();

	// write everything into an output buffer, pipe-delimited
	snprintf(querylogbuf,sizeof(querylogbuf)-1,
		"%04d-%02d-%02d %02d:%02d:%02d|%d|%f|%s|%d|%s|%s|%f|%s|%s|\n",
		dt.getYear(),
		dt.getMonth(),
		dt.getDayOfMonth(),
		dt.getHour(),
		dt.getMinutes(),
		dt.getSeconds(),
		// FIXME: implement this...
		//my_index, 
		0,
		//totaltimediff,
		qptimediff, // temporarily use qptime diff for totaltimediff
		errorcodebuf,
		// FIXME: implement this...
		//cursor->returned_row,
		0,
        	infobuf,
		sqlbuf,
		qptimediff,
		clientaddrbuf,
		bindbuf
		);

	// write that buffer to the log file
	return (querylog.write(querylogbuf)==charstring::length(querylogbuf));
}
