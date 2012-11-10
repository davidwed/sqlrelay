// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

sqlrconnection_svr::sqlrconnection_svr() : daemonprocess(), listener() {

	cmdl=NULL;
	cfgfl=NULL;
	semset=NULL;
	idmemory=NULL;

	updown=NULL;

	dbselected=false;
	originaldb=NULL;

	tmpdir=NULL;

	unixsocket=NULL;
	unixsocketptr=NULL;
	serversockun=NULL;
	serversockin=NULL;
	serversockincount=0;

	inetport=0;
	authc=NULL;
	lastuserbuffer[0]='\0';
	lastpasswordbuffer[0]='\0';
	lastauthsuccess=false;

	commitorrollback=false;

	txerror=NULL;
	txerrnum=0;
	txliveconnection=false;

	autocommit=false;
	autocommitforthissession=false;
	fakeautocommit=false;

	translatebegins=true;
	faketransactionblocks=false;
	faketransactionblocksautocommiton=false;
	intransactionblock=false;

	fakeinputbinds=false;
	translatebinds=false;

	isolationlevel=NULL;

	ignoreselectdb=false;

	maxquerysize=0;
	maxstringbindvaluelength=0;
	maxlobbindvaluelength=0;
	idleclienttimeout=-1;

	handoffindex=0;

	connected=false;
	inclientsession=false;
	loggedin=false;

	// maybe someday these parameters will be configurable
	bindpool=new memorypool(512,128,100);
	bindmappingspool=new memorypool(512,128,100);
	inbindmappings=new namevaluepairs;
	outbindmappings=new namevaluepairs;

	sqlp=NULL;
	sqlt=NULL;
	sqlw=NULL;

	debugsqltranslation=false;
	debugtriggers=false;

	cur=NULL;

	sid_sqlrcon=NULL;

	pidfile=NULL;

	clientinfolen=0;

	decrementonclose=false;
	silent=false;
}
