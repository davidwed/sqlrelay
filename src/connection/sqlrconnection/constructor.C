// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

sqlrconnection::sqlrconnection() : daemonprocess(), listener(), debugfile() {

	cmdl=NULL;
	cfgfl=NULL;
	ipcptr=NULL;
	lsnrcom=NULL;
	sclrcom=NULL;
	ussf=NULL;

	updown=NULL;

	tmpdir=NULL;

	init=0;

	unixsocket=NULL;
	unixsocketptr=NULL;
	serversockun=NULL;
	serversockin=NULL;

	inetport=0;
	authc=NULL;
	lastuserbuffer[0]=(char)NULL;
	lastpasswordbuffer[0]=(char)NULL;
	lastauthsuccess=0;

	autocommit=0;
	checkautocommit=0;
	performautocommit=0;

	// maybe someday these parameters will be configurable
	bindpool=new memorypool(512,128,100);
}
