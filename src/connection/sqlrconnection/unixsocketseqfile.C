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

bool sqlrconnection::getUnixSocket(const char *tmpdir, char *unixsocketptr) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"getting unix socket...");
	#endif

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

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done getting unix socket");
	#endif

	return true;
}

bool sqlrconnection::openSequenceFile(file *sockseq,
				const char *tmpdir, char *unixsocketptr) {

	// open the sequence file and get the current port number
	char	*sockseqname=new char[charstring::length(tmpdir)+9];
	sprintf(sockseqname,"%s/sockseq",tmpdir);

	#ifdef SERVER_DEBUG
	char	*string=new char[8+charstring::length(sockseqname)+1];
	sprintf(string,"opening %s",sockseqname);
	debugPrint("connection",1,string);
	delete[] string;
	#endif

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

		#ifdef SERVER_DEBUG
		string=new char[14+charstring::length(sockseqname)+1];
		sprintf(string,"couldn't open %s",sockseqname);
		debugPrint("connection",1,string);
		delete[] string;
		#endif
	}

	delete[] sockseqname;

	return success;
}

bool sqlrconnection::lockSequenceFile(file *sockseq) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"locking...");
	#endif

	return sockseq->lockFile(F_WRLCK);
}


bool sqlrconnection::getAndIncrementSequenceNumber(file *sockseq,
							char *unixsocketptr) {

	// get the sequence number from the file
	int32_t	buffer;
	if (sockseq->read(&buffer)!=sizeof(int32_t)) {
		buffer=0;
	}
	sprintf(unixsocketptr,"%d",buffer);

	#ifdef SERVER_DEBUG
	char	*string=new char[21+charstring::length(unixsocketptr)+1];
	sprintf(string,"got sequence number: %s",unixsocketptr);
	debugPrint("connection",1,string);
	delete[] string;
	#endif

	// increment the sequence number
	if (buffer==pow(2,31)) {
		buffer=0;
	} else {
		buffer=buffer+1;
	}

	#ifdef SERVER_DEBUG
	string=new char[50];
	sprintf(string,"writing new sequence number: %d",buffer);
	debugPrint("connection",1,string);
	delete[] string;
	#endif

	// write the sequence number back to the file
	if (sockseq->setPositionRelativeToBeginning((off_t)0)==-1) {
		return false;
	}
	return (sockseq->write(buffer)==sizeof(int32_t));
}

bool sqlrconnection::unLockSequenceFile(file *sockseq) {

	// unlock and close the file in a platform-independent manner
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"unlocking...");
	#endif

	return sockseq->unlockFile();
}
