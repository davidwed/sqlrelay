// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

sqlrconnection::sqlrconnection() : daemonprocess(), listener(), debugfile() {

	cmdl=NULL;
	cfgfl=NULL;
	semset=NULL;
	idmemory=NULL;

	updown=NULL;

	tmpdir=NULL;

	unixsocket=NULL;
	unixsocketptr=NULL;
	serversockun=NULL;
	serversockin=NULL;

	inetport=0;
	authc=NULL;
	lastuserbuffer[0]=(char)NULL;
	lastpasswordbuffer[0]=(char)NULL;
	lastauthsuccess=false;

	autocommit=0;
	checkautocommit=0;
	performautocommit=0;

	maxquerysize=MAXQUERYSIZE;
	maxstringbindvaluelength=MAXSTRINGBINDVALUELENGTH;
	maxlobbindvaluelength=MAXLOBBINDVALUELENGTH;
	idleclienttimeout=DEFAULT_IDLECLIENTTIMEOUT;

	connected=false;

	// maybe someday these parameters will be configurable
	bindpool=new memorypool(512,128,100);
}
