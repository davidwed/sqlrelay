// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <freetdsconnection.h>
#include <tdsver.h>

#include <config.h>
#ifndef HAVE_FREETDS_FUNCTION_DEFINITIONS
	#include <ctfunctions.h>
#endif
#include <datatypes.h>

#include <rudiments/stringbuffer.h>
#include <rudiments/charstring.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __GNUC__
stringbuffer	*freetdsconnection::errorstring;
bool		freetdsconnection::deadconnection;
#endif


freetdsconnection::freetdsconnection() {
	errorstring=NULL;
	env=new environment();
}

freetdsconnection::~freetdsconnection() {
	delete errorstring;
	delete env;
}

int freetdsconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void freetdsconnection::handleConnectString() {
	sybase=connectStringValue("sybase");
	lang=connectStringValue("lang");
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	server=connectStringValue("server");
	db=connectStringValue("db");
	charset=connectStringValue("charset");
	language=connectStringValue("language");
	encryption=connectStringValue("encryption");
	hostname=connectStringValue("hostname");
	packetsize=connectStringValue("packetsize");
}

bool freetdsconnection::logIn() {

	// set sybase
	if (sybase && sybase[0] && !env->setValue("SYBASE",sybase)) {
		logInError("Failed to set SYBASE environment variable.",1);
		return false;
	}

	// set lang
	if (lang && lang[0] && !env->setValue("LANG",lang)) {
		logInError("Failed to set LANG environment variable.",1);
		return false;
	}

	// set server
	if (server && server[0] && !env->setValue("DSQUERY",server)) {
		logInError("Failed to set DSQUERY environment variable.",2);
		return false;
	}

	// allocate a context
	context=(CS_CONTEXT *)NULL;
	if (cs_ctx_alloc(CS_VERSION_100,&context)!=CS_SUCCEED) {
		logInError("failed to allocate a context structure",2);
		return false;
	}
	// init the context
	if (ct_init(context,CS_VERSION_100)!=CS_SUCCEED) {
		logInError("failed to initialize a context structure",3);
		return false;
	}


	// configure the error handling callbacks
	if (cs_config(context,CS_SET,CS_MESSAGE_CB,
		(CS_VOID *)freetdsconnection::csMessageCallback,CS_UNUSED,
			(CS_INT *)NULL)
			!=CS_SUCCEED) {
		logInError("failed to set a cslib error message callback",4);
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_CLIENTMSG_CB,
		(CS_VOID *)freetdsconnection::clientMessageCallback)
			!=CS_SUCCEED) {
		logInError("failed to set a client error message callback",4);
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_SERVERMSG_CB,
		(CS_VOID *)freetdsconnection::serverMessageCallback)
			!=CS_SUCCEED) {
		logInError("failed to set a server error message callback",4);
		return false;
	}


	// allocate a connection
	if (ct_con_alloc(context,&dbconn)!=CS_SUCCEED) {
		logInError("failed to allocate a connection structure",4);
		return false;
	}


	// set the user to use
	char	*user=getUser();
	if (ct_con_props(dbconn,CS_SET,CS_USERNAME,
			(CS_VOID *)((user && user[0])?user:""),
			CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the user",5);
		return false;
	}


	// set the password to use
	char	*password=getPassword();
	if (ct_con_props(dbconn,CS_SET,CS_PASSWORD,
			(CS_VOID *)((password && password[0])?password:""),
			CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the password",5);
		return false;
	}

	// set application name
	if (ct_con_props(dbconn,CS_SET,CS_APPNAME,(CS_VOID *)"sqlrelay",
			CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the application name",5);
		return false;
	}

	// set hostname
	if (hostname && hostname[0] &&
		ct_con_props(dbconn,CS_SET,CS_HOSTNAME,(CS_VOID *)hostname,
				CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the hostname",5);
		return false;
	}

	// set packetsize
	if (packetsize && packetsize[0] &&
		ct_con_props(dbconn,CS_SET,CS_PACKETSIZE,
				(CS_VOID *)atoi(packetsize),
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the packetsize",5);
		return false;
	}

	// set encryption
	if (encryption && atoi(encryption)==1) {
		enc=CS_TRUE;
		if (ct_con_props(dbconn,CS_SET,CS_PACKETSIZE,
			(CS_VOID *)&enc,
			CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the encryption",5);
			return false;
		}
	}

	// init locale
	locale=NULL;
	if (cs_loc_alloc(context,&locale)!=CS_SUCCEED) {
		logInError("failed to allocate a locale structure",5);
		return false;
	}
	if (cs_locale(context,CS_SET,locale,CS_LC_ALL,(CS_CHAR *)NULL,
			CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to initialize a locale structure",6);
		return false;
	}

	// set language
	if (language && language[0] &&
		cs_locale(context,CS_SET,locale,CS_SYB_LANG,
			(CS_CHAR *)language,CS_NULLTERM,(CS_INT *)NULL)!=
				CS_SUCCEED) {
		logInError("failed to set the language",6);
		return false;
	}

	// set charset
	if (charset && charset[0] &&
		cs_locale(context,CS_SET,locale,CS_SYB_CHARSET,
			(CS_CHAR *)charset,CS_NULLTERM,(CS_INT *)NULL)!=
				CS_SUCCEED) {
		logInError("failed to set the charset",6);
		return false;
	}

	// set locale
	if (ct_con_props(dbconn,CS_SET,CS_LOC_PROP,(CS_VOID *)locale,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the locale",6);
		return false;
	}

	// connect to the database
	if (ct_connect(dbconn,(CS_CHAR *)NULL,(CS_INT)0)!=CS_SUCCEED) {
		logInError("failed to connect to the database",6);
		return false;
	}
	return true;
}

void freetdsconnection::logInError(const char *error, int stage) {

	fprintf(stderr,"%s\n",error);

	if (errorstring) {
		fprintf(stderr,"%s\n",errorstring->getString());
	}

	if (stage>5) {
		cs_loc_drop(context,locale);
	}
	if (stage>4) {
		ct_con_drop(dbconn);
	}
	if (stage>3) {
		ct_exit(context,CS_UNUSED);
	}
	if (stage>2) {
		cs_ctx_drop(context);
	}
}

sqlrcursor *freetdsconnection::initCursor() {
	return (sqlrcursor *)new freetdscursor((sqlrconnection *)this);
}

void freetdsconnection::deleteCursor(sqlrcursor *curs) {
	delete (freetdscursor *)curs;
}

void freetdsconnection::logOut() {

	cs_loc_drop(context,locale);
	ct_close(dbconn,CS_UNUSED);
	ct_con_drop(dbconn);
	ct_exit(context,CS_UNUSED);
	cs_ctx_drop(context);
}

char *freetdsconnection::identify() {
	return "freetds";
}

char freetdsconnection::bindVariablePrefix() {
	return '@';
}

freetdscursor::freetdscursor(sqlrconnection *conn) : sqlrcursor(conn) {

	#if defined(VERSION_NO)
	char	*versionstring=strdup(VERSION_NO);
	#elif defined(TDS_VERSION_NO)
	char	*versionstring=strdup(TDS_VERSION_NO);
	#else
	char	*versionstring=strdup("freetds v0.00.0");
	#endif

	char	*v=strchr(versionstring,'v');
	*v=(char)NULL;
	majorversion=atoi(v+1);

	char	*firstdot=strchr(v+1,'.');
	*firstdot=(char)NULL;
	minorversion=atoi(firstdot+1);

	char	*seconddot=strchr(firstdot+1,'.');
	*seconddot=(char)NULL;
	patchlevel=atoi(seconddot+1);


	prepared=false;
	freetdsconn=(freetdsconnection *)conn;
	cmd=NULL;
	languagecmd=NULL;
	cursorcmd=NULL;
	cursorname=NULL;

	// replace the regular expressions used to detect creation of a
	// temporary table
	createtemp.compile("(create|CREATE)[ \\t\\r\\n]+(table|TABLE)[ \\t\\r\\n]+#");
	createtemp.study();

	cursorquery.compile("^(select|SELECT)[ \\t\\r\\n]+");
	cursorquery.study();

	rpcquery.compile("^(execute|EXECUTE|exec|EXEC)[ \\t\\r\\n]+");
	rpcquery.study();
}

freetdscursor::~freetdscursor() {
	closeCursor();
	delete[] cursorname;
}

bool freetdscursor::openCursor(int id) {

	clean=true;

	cursorname=charstring::parseNumber((long)id);

	if (ct_cmd_alloc(freetdsconn->dbconn,&languagecmd)!=CS_SUCCEED) {
		return false;
	}
	if (ct_cmd_alloc(freetdsconn->dbconn,&cursorcmd)!=CS_SUCCEED) {
		return false;
	}
	cmd=NULL;

	// switch to the correct database
	bool	retval=true;
	if (freetdsconn->db && freetdsconn->db[0]) {
		int	len=strlen(freetdsconn->db)+4;
		char	query[len+1];
		sprintf(query,"use %s",freetdsconn->db);
		if (!(prepareQuery(query,len) &&
				executeQuery(query,len,true))) {
			bool	live;
			fprintf(stderr,"%s\n",getErrorMessage(&live));
			retval=false;
		}
		cleanUpData(true,true);
	}
	return (retval && sqlrcursor::openCursor(id));
}

bool freetdscursor::closeCursor() {
	bool	retval=true;
	if (languagecmd) {
		retval=(ct_cmd_drop(languagecmd)==CS_SUCCEED);
		languagecmd=NULL;
	}
	if (cursorcmd) {
		retval=(retval && (ct_cmd_drop(cursorcmd)==CS_SUCCEED));
		cursorcmd=NULL;
	}
	cmd=NULL;
	return retval;
}

bool freetdscursor::prepareQuery(const char *query, long length) {

	clean=true;

	this->query=(char *)query;
	this->length=length;

	paramindex=0;
	outbindindex=0;

	isrpcquery=false;

	// This code is here in case freetds ever actually supports cursors...
	//if (cursorquery.match(query)) {
#ifdef CS_CURSOR_DECLARE
	if (false) {

		// initiate a cursor command
		cmd=cursorcmd;
		if (ct_cursor(cursorcmd,CS_CURSOR_DECLARE,
				(CS_CHAR *)cursorname,CS_NULLTERM,
				(CS_CHAR *)query,length,
				CS_UNUSED)!=CS_SUCCEED) {
				//CS_READ_ONLY)!=CS_SUCCEED) {
			return false;
		}

	//} else if (rpcquery.match(query)) {
	} else
#endif
	if (false) {

		// initiate a language command
		cmd=languagecmd;
		isrpcquery=true;
		if (ct_command(languagecmd,CS_RPC_CMD,
			(CS_CHAR *)rpcquery.getSubstringEnd(0),
			length-rpcquery.getSubstringEndOffset(0),
			CS_UNUSED)!=CS_SUCCEED) {
			return false;
		}

	//} else {
	} else if (false) {

		// initiate a language command
		cmd=languagecmd;
		if (ct_command(languagecmd,CS_LANG_CMD,
				(CS_CHAR *)query,length,
				CS_UNUSED)!=CS_SUCCEED) {
			return false;
		}
	} else {
		cmd=languagecmd;
	}

	//clean=false;
	prepared=true;
	return true;
}

void freetdscursor::checkRePrepare() {

	// Sybase doesn't allow you to rebind and re-execute when using 
	// ct_command.  You have to re-prepare too.  I'll make this transparent
	// to the user.
	if (!prepared) {
		prepareQuery(query,length);
	}
}

/*bool freetdscursor::inputBindString(const char *variable,
						unsigned short variablesize,
						const char *value,
						unsigned short valuesize,
						short *isnull) {

	checkRePrepare();

	(CS_VOID)memset(&parameter[paramindex],0,
			sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		strcpy(parameter[paramindex].name,variable);
		parameter[paramindex].namelen=variablesize;
	}
	parameter[paramindex].datatype=CS_CHAR_TYPE;
	parameter[paramindex].maxlength=CS_UNUSED;
	parameter[paramindex].status=CS_INPUTVALUE;
	parameter[paramindex].locale=NULL;
	if (ct_param(cmd,&parameter[paramindex],
		(CS_VOID *)value,valuesize,0)!=CS_SUCCEED) {
		return false;
	}
	paramindex++;
	return true;
}

bool freetdscursor::inputBindLong(const char *variable,
						unsigned short variablesize,
						unsigned long *value) {

	checkRePrepare();

	(CS_VOID)memset(&parameter[paramindex],0,
			sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		strcpy(parameter[paramindex].name,variable);
		parameter[paramindex].namelen=variablesize;
	}
	parameter[paramindex].datatype=CS_INT_TYPE;
	parameter[paramindex].maxlength=CS_UNUSED;
	parameter[paramindex].status=CS_INPUTVALUE;
	parameter[paramindex].locale=NULL;
	if (ct_param(cmd,&parameter[paramindex],
		(CS_VOID *)value,sizeof(long),0)!=CS_SUCCEED) {
		return false;
	}
	paramindex++;
	return true;
}

bool freetdscursor::inputBindDouble(const char *variable,
						unsigned short variablesize,
						double *value,
						unsigned short precision,
						unsigned short scale) {

	checkRePrepare();

	(CS_VOID)memset(&parameter[paramindex],0,
			sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		strcpy(parameter[paramindex].name,variable);
		parameter[paramindex].namelen=variablesize;
	}
	parameter[paramindex].datatype=CS_FLOAT_TYPE;
	parameter[paramindex].maxlength=CS_UNUSED;
	parameter[paramindex].status=CS_INPUTVALUE;
	parameter[paramindex].precision=precision;
	parameter[paramindex].scale=scale;
	parameter[paramindex].locale=NULL;
	if (ct_param(cmd,&parameter[paramindex],
		(CS_VOID *)value,sizeof(double),0)!=CS_SUCCEED) {
		return false;
	}
	paramindex++;
	return true;
}

bool freetdscursor::outputBindString(const char *variable, 
					unsigned short variablesize,
					char *value, 
					unsigned short valuesize, 
					short *isnull) {

	outbindvalues[outbindindex]=value;
	outbindvaluelengths[outbindindex]=valuesize;
	outbindindex++;

	(CS_VOID)memset(&parameter[paramindex],0,
			sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		strcpy(parameter[paramindex].name,variable);
		parameter[paramindex].namelen=variablesize;
	}
	parameter[paramindex].datatype=CS_CHAR_TYPE;
	parameter[paramindex].maxlength=valuesize;
	parameter[paramindex].status=CS_RETURN;
	parameter[paramindex].locale=NULL;
	if (ct_param(cmd,&parameter[paramindex],
			(CS_VOID *)NULL,0,
			(CS_SMALLINT)*isnull)!=CS_SUCCEED) {
		return false;
	}
	paramindex++;
	return true;
}*/

bool freetdscursor::executeQuery(const char *query, long length, bool execute) {

	// clear out any errors
	if (freetdsconn->errorstring) {
		freetdsconn->deadconnection=false;
		delete freetdsconn->errorstring;
		freetdsconn->errorstring=NULL;
	}

	// this code is here in case freetds ever supports cursors
	if (true) {
		stringbuffer	*newquery=fakeInputBinds(query);
		if (newquery) {
			if (ct_command(cmd,CS_LANG_CMD,
					newquery->getString(),
					strlen(newquery->getString()),
					CS_UNUSED)!=CS_SUCCEED) {
				delete newquery;
				return false;
			}
			delete newquery;
		} else {
			if (ct_command(cmd,CS_LANG_CMD,
					(CS_CHAR *)query,length,
					CS_UNUSED)!=CS_SUCCEED) {
				return false;
			}
		}
		clean=false;
	}

	// initialize return values
	ncols=0;
	affectedrows=0;
	row=0;
	maxrow=0;
	totalrows=0;

#ifdef CS_CURSOR_DECLARE
	if (cmd==cursorcmd) {
		if (ct_cursor(cursorcmd,CS_CURSOR_ROWS,
					NULL,CS_UNUSED,
					NULL,CS_UNUSED,
					(CS_INT)FETCH_AT_ONCE)!=CS_SUCCEED) {
			return false;
		}
		if (ct_cursor(cursorcmd,CS_CURSOR_OPEN,
					NULL,CS_UNUSED,
					NULL,CS_UNUSED,
					CS_UNUSED)!=CS_SUCCEED) {
			return false;
		}
	}
#endif

	if (ct_send(cmd)!=CS_SUCCEED) {
		cleanUpData(true,true);
		return false;
	}

	for (;;) {

		results=ct_results(cmd,&resultstype);

		if (results==CS_FAIL || resultstype==CS_CMD_FAIL) {
			cleanUpData(true,true);
			return false;
		}

		if (cmd==languagecmd) {

			if (isrpcquery) {
				// For rpc commands, there should be two
				// result sets - CS_STATUS_RESULT,
				// maybe a CS_PARAM_RESULT and maybe a
				// CS_ROW_RESULT, we're not guaranteed
				// what order they'll come in though, what
				// a pickle...
				// For now, we care about the CS_PARAM_RESULT,
				// presumably there will only be 1 row in it...
				if (resultstype==CS_PARAM_RESULT) {
					break;
				}
			} else {
				// For language commands, there should be only
				// one result set.
				break;
			}

		} else if (resultstype==CS_ROW_RESULT ||
#ifdef CS_CURSOR_DECLARE
					resultstype==CS_CURSOR_RESULT ||
#endif
					resultstype==CS_COMPUTE_RESULT) {
			// For cursor commands, each call to ct_cursor will
			// have generated a result set.  There will be result
			// sets for the CS_CURSOR_DECLARE, CS_CURSOR_ROWS and
			// CS_CURSOR_OPEN calls.  We need to skip past the
			// first 2, unless they failed.  If they failed, it
			// will be caught above.
			break;
		}

		if (ct_cancel(NULL,cmd,CS_CANCEL_CURRENT)==CS_FAIL) {
			freetdsconn->deadconnection=true;
			// FIXME: call ct_close(CS_FORCE_CLOSE)
			// maybe return false
			return false;
		}
	}

	checkForTempTable(query,length);

	// reset the prepared flag
	prepared=false;

	// For queries which return rows or parameters (output bind variables),
	// get the column count and bind columns.  For DML queries, get the
	// affected row count.
	// Affected row count is only supported in versio>=0.53 but appears
	// to be broken in 0.61 as well
	if (majorversion==0 && (minorversion<53 || minorversion==61)) {
		affectedrows=-1;
	} else {
		affectedrows=0;
	}

	bool	moneycolumn=false;
	if (resultstype==CS_ROW_RESULT ||
#ifdef CS_CURSOR_DECLARE
			resultstype==CS_CURSOR_RESULT ||
#endif
			resultstype==CS_COMPUTE_RESULT ||
			resultstype==CS_PARAM_RESULT) {

		if (ct_res_info(cmd,CS_NUMDATA,(CS_VOID *)&ncols,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			return false;
		}

		if (ncols>MAX_SELECT_LIST_SIZE) {
			ncols=MAX_SELECT_LIST_SIZE;
		}

		// bind columns
		for (int i=0; i<(int)ncols; i++) {

			// dealing with money columns cause freetds < 0.53 to
			// crash, take care of that here...
			if (majorversion==0 && minorversion<53
							&& !moneycolumn) {
				CS_DATAFMT	moneytest;
				ct_describe(cmd,i+1,&moneytest);
				if (moneytest.datatype==CS_MONEY_TYPE ||
					moneytest.datatype==CS_MONEY4_TYPE) {
					moneycolumn=true;
					if (freetdsconn->errorstring) {
						delete freetdsconn->errorstring;
					}
					freetdsconn->errorstring=
						new stringbuffer();
					freetdsconn->errorstring->append(
						"FreeTDS versions prior to ");
					freetdsconn->errorstring->append( 
						"0.53 do not support MONEY ");
					freetdsconn->errorstring->append( 
						"or SMALLMONEY datatypes. ");
					freetdsconn->errorstring->append( 
						"Please upgrade SQL Relay to ");
					freetdsconn->errorstring->append( 
						"a version compiled against ");
					freetdsconn->errorstring->append( 
						"FreeTDS >= 0.53 ");
				}
			}
	
			// get the field as a null terminated character string
			// no longer than MAX_ITEM_BUFFER_SIZE, override some
			// other values that might have been set also
			column[i].datatype=CS_CHAR_TYPE;
			column[i].format=CS_FMT_NULLTERM;
			column[i].maxlength=MAX_ITEM_BUFFER_SIZE;
			column[i].scale=CS_UNUSED;
			column[i].precision=CS_UNUSED;
			column[i].status=CS_UNUSED;
			column[i].count=FETCH_AT_ONCE;
			column[i].usertype=CS_UNUSED;
			column[i].locale=NULL;
	
			// bind the columns for the fetches
			if (ct_bind(cmd,i+1,&column[i],(CS_VOID *)data[i],
				datalength[i],nullindicator[i])!=CS_SUCCEED) {
				break;
			}
		}

	} else if (resultstype==CS_CMD_SUCCEED) {
		if (majorversion==0 && (minorversion>52 && minorversion!=61) &&
			ct_res_info(cmd,CS_ROW_COUNT,(CS_VOID *)&affectedrows,
					CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			return false;
		} 
	}

	// If we got a moneycolumn (and version<0.53) then cancel the
	// result set.  Otherwise FreeTDS will spew "unknown marker"
	// errors to the screen when cleanUpData() is called.
	if (moneycolumn) {
		if (ct_cancel(NULL,cmd,CS_CANCEL_CURRENT)==CS_FAIL) {
			freetdsconn->deadconnection=true;
			// FIXME: call ct_close(CS_FORCE_CLOSE)
			// maybe return false
			return false;
		}
		return false;
	}


	// if we're doing an rpc query, the result set should be a single
	// row of output parameter results, fetch it and populate the output
	// bind variables...
	if (isrpcquery) {

		if (ct_fetch(cmd,CS_UNUSED,CS_UNUSED,CS_UNUSED,
				&rowsread)!=CS_SUCCEED && !rowsread) {
			return false;
		}
		
		// copy data into output bind values
		CS_INT	maxindex=outbindindex;
		if (ncols<outbindindex) {
			// this shouldn't happen...
			maxindex=ncols;
		}
		for (CS_INT i=0; i<maxindex; i++) {
			CS_INT	length=outbindvaluelengths[i];
			if (datalength[i][0]<length) {
				length=datalength[i][0];
			}
			memcpy(outbindvalues[i],data[i][0],length);
		}

		discardResults();
		ncols=0;
	}

	// return success only if no error was generated
	if (freetdsconn->errorstring) {
		return false;
	}
	return true;
}

char *freetdscursor::getErrorMessage(bool *liveconnection) {
	if (freetdsconn->deadconnection) {
		*liveconnection=false;
	} else {
		*liveconnection=true;
	}
	if (freetdsconn->errorstring) {
		return freetdsconn->errorstring->getString();
	} else {
		return NULL;
	}
}

void freetdscursor::returnRowCounts() {

	// send row counts (actual row count unknown in sybase)
	conn->sendRowCounts((long)-1,(long)affectedrows);
}

void freetdscursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void freetdscursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// unless the query was a successful select, send no header
	if (resultstype!=CS_ROW_RESULT &&
#ifdef CS_CURSOR_DECLARE
			resultstype!=CS_CURSOR_RESULT &&
#endif
			resultstype!=CS_COMPUTE_RESULT) {
		return;
	}

	// gonna need this later
	int	type;

	// for each column...
	for (int i=0; i<(int)ncols; i++) {

		// get the column description
		if (ct_describe(cmd,i+1,&column[i])!=CS_SUCCEED) {
			break;
		}
	
		// set the datatype
		unsigned short	binary=0;
		if (column[i].datatype==CS_CHAR_TYPE) {
			type=CHAR_DATATYPE;
		} else if (column[i].datatype==CS_INT_TYPE) {
			type=INT_DATATYPE;
		} else if (column[i].datatype==CS_SMALLINT_TYPE) {
			type=SMALLINT_DATATYPE;
		} else if (column[i].datatype==CS_TINYINT_TYPE) {
			type=TINYINT_DATATYPE;
		} else if (column[i].datatype==CS_MONEY_TYPE) {
			type=MONEY_DATATYPE;
		} else if (column[i].datatype==CS_DATETIME_TYPE) {
			type=DATETIME_DATATYPE;
		} else if (column[i].datatype==CS_NUMERIC_TYPE) {
			type=NUMERIC_DATATYPE;
		} else if (column[i].datatype==CS_DECIMAL_TYPE) {
			type=DECIMAL_DATATYPE;
		} else if (column[i].datatype==CS_DATETIME4_TYPE) {
			type=SMALLDATETIME_DATATYPE;
		} else if (column[i].datatype==CS_MONEY4_TYPE) {
			type=SMALLMONEY_DATATYPE;
		} else if (column[i].datatype==CS_IMAGE_TYPE) {
			type=IMAGE_DATATYPE;
			binary=1;
		} else if (column[i].datatype==CS_BINARY_TYPE) {
			type=BINARY_DATATYPE;
		} else if (column[i].datatype==CS_BIT_TYPE) {
			type=BIT_DATATYPE;
		} else if (column[i].datatype==CS_REAL_TYPE) {
			type=REAL_DATATYPE;
		} else if (column[i].datatype==CS_FLOAT_TYPE) {
			type=FLOAT_DATATYPE;
		} else if (column[i].datatype==CS_TEXT_TYPE) {
			type=TEXT_DATATYPE;
		} else if (column[i].datatype==CS_VARCHAR_TYPE) {
			type=VARCHAR_DATATYPE;
		} else if (column[i].datatype==CS_VARBINARY_TYPE) {
			type=VARBINARY_DATATYPE;
		} else if (column[i].datatype==CS_LONGCHAR_TYPE) {
			type=LONGCHAR_DATATYPE;
		} else if (column[i].datatype==CS_LONGBINARY_TYPE) {
			type=LONGBINARY_DATATYPE;
		} else if (column[i].datatype==CS_LONG_TYPE) {
			type=LONG_DATATYPE;
		} else if (column[i].datatype==CS_ILLEGAL_TYPE) {
			type=ILLEGAL_DATATYPE;
		} else if (column[i].datatype==CS_SENSITIVITY_TYPE) {
			type=SENSITIVITY_DATATYPE;
		} else if (column[i].datatype==CS_BOUNDARY_TYPE) {
			type=BOUNDARY_DATATYPE;
		} else if (column[i].datatype==CS_VOID_TYPE) {
			type=VOID_DATATYPE;
		} else if (column[i].datatype==CS_USHORT_TYPE) {
			type=USHORT_DATATYPE;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// limit the column size
		if (column[i].maxlength>MAX_ITEM_BUFFER_SIZE) {
			column[i].maxlength=MAX_ITEM_BUFFER_SIZE;
		}

		// send the column definition
		conn->sendColumnDefinition(column[i].name,
					strlen(column[i].name),
					type,
					column[i].maxlength,
					column[i].precision,
					column[i].scale,
					(column[i].status&CS_CANBENULL),
					0,
					0,
					(column[i].status&
						(CS_KEY|CS_VERSION_KEY)),
					(type==USHORT_DATATYPE),
					0,
					binary,
					(column[i].status&CS_IDENTITY));
	}
}

bool freetdscursor::noRowsToReturn() {
	// unless the query was a successful select, send no data
	return (resultstype!=CS_ROW_RESULT &&
#ifdef CS_CURSOR_DECLARE
			resultstype!=CS_CURSOR_RESULT &&
#endif
			resultstype!=CS_COMPUTE_RESULT);
}

bool freetdscursor::skipRow() {
	if (fetchRow()) {
		row++;
		return true;
	}
	return false;
}

bool freetdscursor::fetchRow() {
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return false;
	}
	if (!row) {
		if (ct_fetch(cmd,CS_UNUSED,CS_UNUSED,CS_UNUSED,
				&rowsread)!=CS_SUCCEED && !rowsread) {
			return false;
		}
		maxrow=rowsread;
		totalrows=totalrows+rowsread;
	}
	return true;
}

void freetdscursor::returnRow() {

	// send each row back
	for (int col=0; col<(int)ncols; col++) {
		if (nullindicator[col][row]>-1 && datalength[col][row]) {
			conn->sendField(data[col][row],datalength[col][row]-1);
		} else {
			conn->sendNullField();
		}
	}
	row++;
}


void freetdscursor::cleanUpData(bool freeresult, bool freebinds) {

	if (clean) {
		return;
	}

	if (freeresult) {
		discardResults();
		discardCursor();
	}

	clean=true;
}

void freetdscursor::discardResults() {

	// if there are any unprocessed result sets, process them
	if (results==CS_SUCCEED) {
		do {
			if (ct_cancel(NULL,cmd,CS_CANCEL_CURRENT)==CS_FAIL) {
				freetdsconn->deadconnection=true;
				// FIXME: call ct_close(CS_FORCE_CLOSE)
				// maybe return false
			}
			results=ct_results(cmd,&resultstype);
		} while (results==CS_SUCCEED);
	}

	if (results==CS_FAIL) {
		if (ct_cancel(NULL,cmd,CS_CANCEL_ALL)==CS_FAIL) {
			freetdsconn->deadconnection=true;
			// FIXME: call ct_close(CS_FORCE_CLOSE)
			// maybe return false
		}
	}
}


void freetdscursor::discardCursor() {

#ifdef CS_CURSOR_DECLARE
	if (cmd==cursorcmd) {
		if (ct_cursor(cursorcmd,CS_CURSOR_CLOSE,NULL,CS_UNUSED,
				NULL,CS_UNUSED,CS_DEALLOC)==CS_SUCCEED) {
			if (ct_send(cursorcmd)==CS_SUCCEED) {
				results=ct_results(cmd,&resultstype);
				discardResults();
			}
		}
	}
#endif
}

CS_RETCODE freetdsconnection::csMessageCallback(CS_CONTEXT *ctxt, 
						CS_CLIENTMSG *msgp) {
	if (errorstring) {
		return CS_SUCCEED;
	}
	errorstring=new stringbuffer();

	errorstring->append("Client Library error:\n");
	errorstring->append("	severity(")->
				append((long)CS_SEVERITY(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	layer(")->
				append((long)CS_LAYER(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	origin(")->
				append((long)CS_ORIGIN(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	number(")->
				append((long)CS_NUMBER(msgp->msgnumber))->
				append(")\n");
	errorstring->append("Error:	")->append(msgp->msgstring)->
				append("\n");

	if (msgp->osstringlen>0) {
		errorstring->append("Operating System Error:\n");
		errorstring->append("\n	")->append(msgp->osstring)->
							append("\n");
	}

	//printf("csMessageCallback:\n%s\n",errorstring->getString());

	// for a timeout message, set deadconnection to 1
	if (CS_SEVERITY(msgp->msgnumber)==CS_SV_RETRY_FAIL &&
		CS_LAYER(msgp->msgnumber)==63 &&
		CS_ORIGIN(msgp->msgnumber)==63 &&
		CS_NUMBER(msgp->msgnumber)==63) {
		deadconnection=true;

	// for a read from sql server failed message, set deadconnection to 1
	} else if (CS_SEVERITY(msgp->msgnumber)==78 &&
		CS_LAYER(msgp->msgnumber)==0 &&
		CS_ORIGIN(msgp->msgnumber)==0 &&
		CS_NUMBER(msgp->msgnumber)==36) {
		deadconnection=true;
	}

	return CS_SUCCEED;
}

CS_RETCODE freetdsconnection::clientMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp) {
	if (errorstring) {
		return CS_SUCCEED;
	}
	errorstring=new stringbuffer();

	errorstring->append("Client Library error:\n");
	errorstring->append("	severity(")->
				append((long)CS_SEVERITY(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	layer(")->
				append((long)CS_LAYER(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	origin(")->
				append((long)CS_ORIGIN(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	number(")->
				append((long)CS_NUMBER(msgp->msgnumber))->
				append(")\n");
	errorstring->append("Error:	")->append(msgp->msgstring)->
				append("\n");

	if (msgp->osstringlen>0) {
		errorstring->append("Operating System Error:\n");
		errorstring->append("\n	")->append(msgp->osstring)->
							append("\n");
	}

	//printf("clientMessageCallback:\n%s\n",errorstring->getString());

	// for a timeout message, set deadconnection to 1
	if (CS_SEVERITY(msgp->msgnumber)==CS_SV_RETRY_FAIL &&
		CS_NUMBER(msgp->msgnumber)==63 &&
		CS_ORIGIN(msgp->msgnumber)==63 &&
		CS_LAYER(msgp->msgnumber)==63) {
		deadconnection=true;

	// for a read from sql server failed message, set deadconnection to 1
	} else if (CS_SEVERITY(msgp->msgnumber)==78 &&
		CS_LAYER(msgp->msgnumber)==0 &&
		CS_ORIGIN(msgp->msgnumber)==0 &&
		CS_NUMBER(msgp->msgnumber)==36) {
		deadconnection=true;
	}

	return CS_SUCCEED;
}

CS_RETCODE freetdsconnection::serverMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp) {

	// This is a special case, for some reason, "use db" queries
	// throw a warning, ignore them.
	if (msgp->msgnumber==5701 &&
			(msgp->severity==10 || msgp->severity==0)) {
		return CS_SUCCEED;
	}

	if (errorstring) {
		return CS_SUCCEED;
	}
	errorstring=new stringbuffer();

	errorstring->append("Server message:\n");
	errorstring->append("	severity(")->
				append((long)msgp->severity)->
				append(")\n");
	errorstring->append("	number(")->
				append((long)msgp->msgnumber)->
				append(")\n");
	errorstring->append("	state(")->
				append((long)msgp->state)->
				append(")\n");
	errorstring->append("	line(")->
				append((long)msgp->line)->
				append(")\n");
	errorstring->append("Server Name:\n")->
				append(msgp->svrname)->
				append("\n");
	errorstring->append("Procedure Name:\n")->
				append(msgp->proc)->
				append("\n");
	errorstring->append("Error:	")->
				append(msgp->text)->
				append("\n");

	//printf("serverMessageCallback:\n%s\n",errorstring->getString());

	return CS_SUCCEED;
}


void freetdsconnection::dropTempTable(sqlrcursor *cursor,
					const char *tablename) {
	stringbuffer	dropquery;
	dropquery.append("drop table #")->append(tablename);
	if (cursor->prepareQuery(dropquery.getString(),
					dropquery.getStringLength())) {
		cursor->executeQuery(dropquery.getString(),
					dropquery.getStringLength(),1);
	}
	cursor->cleanUpData(true,true);
}
