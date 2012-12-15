// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

sqlrcontroller_svr::sqlrcontroller_svr() : daemonprocess(), listener() {

	conn=NULL;

	cmdl=NULL;
	cfgfl=NULL;
	semset=NULL;
	idmemory=NULL;
	connstats=NULL;

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

	autocommitforthissession=false;

	translatebegins=true;
	faketransactionblocks=false;
	faketransactionblocksautocommiton=false;
	intransactionblock=false;

	fakeinputbinds=false;
	translatebinds=false;

	isolationlevel=NULL;

	ignoreselectdb=false;

	maxquerysize=0;
	maxbindcount=0;
	maxbindnamelength=0;
	maxstringbindvaluelength=0;
	maxlobbindvaluelength=0;
	maxerrorlength=0;
	idleclienttimeout=-1;

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
	sqlrlg=NULL;
	sqlrq=NULL;
	sqlrpe=NULL;

	debugsqltranslation=false;
	debugtriggers=false;

	cur=NULL;

	pidfile=NULL;

	clientinfo=NULL;
	clientinfolen=0;

	decrementonclose=false;
	silent=false;

	loggedinsec=0;
	loggedinusec=0;
}
