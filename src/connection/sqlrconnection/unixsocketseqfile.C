// Copyright (c) 1999-2004  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include <rudiments/permissions.h>

#include <config.h>

bool sqlrconnection::getUnixSocket(char *tmpdir, char *unixsocketptr) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"getting unix socket...");
	#endif

	if ((sockseq=openSequenceFile(tmpdir,unixsocketptr))==-1 ||
						!lockSequenceFile()) {
		return false;
	}
	if (!getAndIncrementSequenceNumber(unixsocketptr)) {
		unLockSequenceFile();
		close(sockseq);
		return false;
	}
	if (unLockSequenceFile()==-1) {
		close(sockseq);
		return false;
	}
	if (close(sockseq)==-1) {
		return false;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done getting unix socket");
	#endif

	return true;
}

int sqlrconnection::openSequenceFile(char *tmpdir, char *unixsocketptr) {

	// open the sequence file and get the current port number
	char	*sockseqname=new char[strlen(tmpdir)+9];
	sprintf(sockseqname,"%s/sockseq",tmpdir);

	#ifdef SERVER_DEBUG
	char	*string=new char[8+strlen(sockseqname)+1];
	sprintf(string,"opening %s",sockseqname);
	debugPrint("connection",1,string);
	delete[] string;
	#endif

	mode_t	oldumask=umask(011);
	int	sockseqfd=open(sockseqname,O_RDWR|O_CREAT,
				permissions::everyoneReadWrite());
	umask(oldumask);

	// handle error
	if (sockseqfd==-1) {
		fprintf(stderr,"Could not open: %s\n",sockseqname);
		fprintf(stderr,"Make sure that the file and directory are \n");
		fprintf(stderr,"readable and writable.\n\n");
		unixsocketptr[0]=(char)NULL;

		#ifdef SERVER_DEBUG
		string=new char[14+strlen(sockseqname)+1];
		sprintf(string,"couldn't open %s",sockseqname);
		debugPrint("connection",1,string);
		delete[] string;
		#endif
	}

	delete[] sockseqname;

	return sockseqfd;
}

bool sqlrconnection::lockSequenceFile() {

	// lock the file in a platform-independent manner
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"locking...");
	#endif

	struct	flock fl;
	fl.l_type=F_WRLCK;
	fl.l_whence=SEEK_END;
	fl.l_start=0;
	fl.l_len=0;
	return (fcntl(sockseq,F_SETLKW,&fl)!=-1);
}


bool sqlrconnection::getAndIncrementSequenceNumber(char *unixsocketptr) {

	// get the sequence number from the file
	long	buffer;
	int	size=read(sockseq,(void *)&buffer,sizeof(long));
	if (size<1) {
		buffer=0;
	}
	sprintf(unixsocketptr,"%ld",buffer);

	#ifdef SERVER_DEBUG
	char	*string=new char[21+strlen(unixsocketptr)+1];
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
	sprintf(string,"writing new sequence number: %ld",buffer);
	debugPrint("connection",1,string);
	delete[] string;
	#endif

	// write the sequence number back to the file
	if (lseek(sockseq,0,SEEK_SET)==-1) {
		return false;
	}
	return (write(sockseq,(void *)&buffer,sizeof(long))==sizeof(long));
}

bool sqlrconnection::unLockSequenceFile() {

	// unlock and close the file in a platform-independent manner
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"unlocking...");
	#endif

	struct	flock fl;
	fl.l_type=F_UNLCK;
	fl.l_whence=SEEK_END;
	fl.l_start=0;
	fl.l_len=0;
	return (fcntl(sockseq,F_SETLKW,&fl)!=-1);
}
