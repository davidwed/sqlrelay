#include <connection/unixsocketseqfile.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include <rudiments/permissions.h>

#include <config.h>

#ifdef SERVER_DEBUG
void	unixsocketseqfile::setDebugLogger(logger *dl) {
	this->dl=dl;
}
#endif

int	unixsocketseqfile::getUnixSocket(char *tmpdir, char *unixsocketptr) {

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"getting unix socket...");
	#endif

	if ((sockseq=openSequenceFile(tmpdir,unixsocketptr))==-1 ||
					lockSequenceFile()==-1) {
		return 0;
	}
	if (getAndIncrementSequenceNumber(unixsocketptr)==-1) {
		unLockSequenceFile();
		close(sockseq);
		return 0;
	}
	if (unLockSequenceFile()==-1) {
		close(sockseq);
		return 0;
	}
	if (close(sockseq)==-1) {
		return 0;
	}

	#ifdef SERVER_DEBUG
	dl->write("connection",0,"done getting unix socket");
	#endif

	return 1;
}

int	unixsocketseqfile::openSequenceFile(char *tmpdir, char *unixsocketptr) {

	// open the sequence file and get the current port number
	char	*sockseqname=new char[strlen(tmpdir)+9];
	sprintf(sockseqname,"%s/sockseq",tmpdir);

	#ifdef SERVER_DEBUG
	char	*string=new char[8+strlen(sockseqname)+1];
	sprintf(string,"opening %s",sockseqname);
	dl->write("connection",1,string);
	delete[] string;
	#endif

	mode_t	oldumask=umask(011);
	int	sockseq=open(sockseqname,O_RDWR|O_CREAT,
				permissions::everyoneReadWrite());
	umask(oldumask);

	// handle error
	if (sockseq==-1) {
		fprintf(stderr,"Could not open: %s\n",sockseqname);
		fprintf(stderr,"Make sure that the file and directory are \n");
		fprintf(stderr,"readable and writable.\n\n");
		unixsocketptr[0]=(char)NULL;

		#ifdef SERVER_DEBUG
		string=new char[14+strlen(sockseqname)+1];
		sprintf(string,"couldn't open %s",sockseqname);
		dl->write("connection",1,string);
		delete[] string;
		#endif
	}

	delete[] sockseqname;

	return sockseq;
}

int	unixsocketseqfile::lockSequenceFile() {

	// lock the file in a platform-independent manner
	#ifdef SERVER_DEBUG
	dl->write("connection",1,"locking...");
	#endif

	struct	flock fl;
	fl.l_type=F_WRLCK;
	fl.l_whence=SEEK_END;
	fl.l_start=0;
	fl.l_len=0;
	return (fcntl(sockseq,F_SETLKW,&fl)!=-1);
}


int	unixsocketseqfile::getAndIncrementSequenceNumber(char *unixsocketptr) {

	// get the sequence number from the file
	long	buffer;
	int	size=read(sockseq,(void *)&buffer,sizeof(long));
	if (size<1) {
		buffer=0;
	}
	sprintf(unixsocketptr,"%d",buffer);

	#ifdef SERVER_DEBUG
	char	*string=new char[21+strlen(unixsocketptr)+1];
	sprintf(string,"got sequence number: %s",unixsocketptr);
	dl->write("connection",1,string);
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
	dl->write("connection",1,string);
	delete[] string;
	#endif

	// write the sequence number back to the file
	if (lseek(sockseq,0,SEEK_SET)==-1) {
		return 0;
	}
	return (write(sockseq,(void *)&buffer,sizeof(long))==sizeof(long));
}

int	unixsocketseqfile::unLockSequenceFile() {

	// unlock and close the file in a platform-independent manner
	#ifdef SERVER_DEBUG
	dl->write("connection",1,"unlocking...");
	#endif

	struct	flock fl;
	fl.l_type=F_UNLCK;
	fl.l_whence=SEEK_END;
	fl.l_start=0;
	fl.l_len=0;
	return (fcntl(sockseq,F_SETLKW,&fl)!=-1);
}
