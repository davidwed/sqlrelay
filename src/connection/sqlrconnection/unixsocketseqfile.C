// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <stdio.h>

// for umask
#include <sys/types.h>
#include <sys/stat.h>

#include <math.h>

#include <rudiments/permissions.h>

#include <config.h>

bool sqlrconnection_svr::getUnixSocket(const char *tmpdir, char *unixsocketptr) {

	dbgfile.debugPrint("connection",0,"getting unix socket...");

	file	sockseq;
	if (!openSequenceFile(&sockseq,tmpdir,unixsocketptr) ||
						!lockSequenceFile(&sockseq)) {
		return false;
	}
	if (!getAndIncrementSequenceNumber(&sockseq,unixsocketptr)) {
		unLockSequenceFile(&sockseq);
		sockseq.close();
		return false;
	}
	if (!unLockSequenceFile(&sockseq)) {
		sockseq.close();
		return false;
	}
	if (!sockseq.close()) {
		return false;
	}

	dbgfile.debugPrint("connection",0,"done getting unix socket");

	return true;
}

bool sqlrconnection_svr::openSequenceFile(file *sockseq,
				const char *tmpdir, char *unixsocketptr) {

	// open the sequence file and get the current port number
	size_t	sockseqnamelen=charstring::length(tmpdir)+9;
	char	*sockseqname=new char[sockseqnamelen];
	snprintf(sockseqname,sockseqnamelen,"%s/sockseq",tmpdir);

	size_t	stringlen=8+charstring::length(sockseqname)+1;
	char	*string=new char[stringlen];
	snprintf(string,stringlen,"opening %s",sockseqname);
	dbgfile.debugPrint("connection",1,string);
	delete[] string;

	mode_t	oldumask=umask(011);
	bool	success=sockseq->open(sockseqname,O_RDWR|O_CREAT,
				permissions::everyoneReadWrite());
	umask(oldumask);

	// handle error
	if (!success) {
		fprintf(stderr,"Could not open: %s\n",sockseqname);
		fprintf(stderr,"Make sure that the file and directory are \n");
		fprintf(stderr,"readable and writable.\n\n");
		unixsocketptr[0]=(char)NULL;

		stringlen=14+charstring::length(sockseqname)+1;
		string=new char[stringlen];
		snprintf(string,stringlen,"couldn't open %s",sockseqname);
		dbgfile.debugPrint("connection",1,string);
		delete[] string;
	}

	delete[] sockseqname;

	return success;
}

bool sqlrconnection_svr::lockSequenceFile(file *sockseq) {

	dbgfile.debugPrint("connection",1,"locking...");

	return sockseq->lockFile(F_WRLCK);
}


bool sqlrconnection_svr::getAndIncrementSequenceNumber(file *sockseq,
							char *unixsocketptr) {

	// get the sequence number from the file
	int32_t	buffer;
	if (sockseq->read(&buffer)!=sizeof(int32_t)) {
		buffer=0;
	}
	sprintf(unixsocketptr,"%d",buffer);

	size_t	stringlen=21+charstring::length(unixsocketptr)+1;
	char	*string=new char[stringlen];
	snprintf(string,stringlen,"got sequence number: %s",unixsocketptr);
	dbgfile.debugPrint("connection",1,string);
	delete[] string;

	// increment the sequence number
	// (the (double) cast is required for solaris with -compat=4)
	if (buffer==pow((double)2,31)) {
		buffer=0;
	} else {
		buffer=buffer+1;
	}

	string=new char[50];
	snprintf(string,50,"writing new sequence number: %d",buffer);
	dbgfile.debugPrint("connection",1,string);
	delete[] string;

	// write the sequence number back to the file
	if (sockseq->setPositionRelativeToBeginning(0)==-1) {
		return false;
	}
	return (sockseq->write(buffer)==sizeof(int32_t));
}

bool sqlrconnection_svr::unLockSequenceFile(file *sockseq) {

	// unlock and close the file in a platform-independent manner
	dbgfile.debugPrint("connection",1,"unlocking...");

	return sockseq->unlockFile();
}
