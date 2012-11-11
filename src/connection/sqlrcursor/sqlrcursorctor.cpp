// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

sqlrcursor_svr::sqlrcursor_svr(sqlrconnection_svr *conn) {

	this->conn=conn;
	inbindcount=0;
	inbindvars=new bindvar_svr[conn->maxbindcount];
	outbindcount=0;
	outbindvars=new bindvar_svr[conn->maxbindcount];
	
	busy=false;

	createtemp.compile("(create|CREATE|declare|DECLARE)[ \\t\\r\\n]+((global|GLOBAL|local|LOCAL)?[ \\t\\r\\n]+)?(temp|TEMP|temporary|TEMPORARY)?[ \\t\\r\\n]+(table|TABLE)[ \\t\\r\\n]+");

	querybuffer=new char[conn->maxquerysize+1];
	querylength=0;
	querytree=NULL;
	queryresult=false;

	error=new char[conn->maxerrorlength];
	errorlength=0;
	errnum=0;
	liveconnection=true;

	commandstartsec=0;
	commandstartusec=0;
	querystartsec=0;
	querystartusec=0;
	queryendsec=0;
	queryendusec=0;
	commandendsec=0;
	commandendusec=0;

	fakeinputbindsforthisquery=false;

	sid_sqlrcur=NULL;
	if (conn->cfgfl->getSidEnabled()) {
		sid_sqlrcur=new sqlrcursor(conn->sid_sqlrcon);
		sql_injection_detection_parameters();
	}
	sid_egress=true;
}
