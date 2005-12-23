// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

sqlrconnection_svr::sqlrconnection_svr() :
			daemonprocess(), listener(), debugfile() {

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
	serversockincount=0;

	inetport=0;
	authc=NULL;
	lastuserbuffer[0]=(char)NULL;
	lastpasswordbuffer[0]=(char)NULL;
	lastauthsuccess=false;

	autocommit=0;
	checkautocommit=0;
	performautocommit=0;

	maxquerysize=0;
	maxstringbindvaluelength=0;
	maxlobbindvaluelength=0;
	idleclienttimeout=-1;

	connected=false;

	// maybe someday these parameters will be configurable
	bindpool=new memorypool(512,128,100);

#ifdef INCLUDE_SID
	sid_sqlrcon=NULL;
#endif
}
