// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <freetdsconnection.h>
#include <rudiments/charstring.h>

#include <config.h>
#include <datatypes.h>

#include <stdlib.h>

#ifdef __GNUC__
stringbuffer	*freetdsconnection::errorstring;
int		freetdsconnection::deadconnection;
#endif

freetdsconnection::freetdsconnection() {
	errorstring=NULL;
	env=new environment();
}

freetdsconnection::~freetdsconnection() {
	delete errorstring;
	delete env;
}

int	freetdsconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	freetdsconnection::handleConnectString() {
	sybase=connectStringValue("sybase");
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

int	freetdsconnection::logIn() {

	// set sybase environment variable
	if (sybase && sybase[0] && !env->setValue("SYBASE",sybase)) {
		logInError("Failed to set SYBASE environment variable.",1);
		return 0;
	}

	// set dsquery environment variable
	if (server && server[0] && !env->setValue("DSQUERY",server)) {
		logInError("Failed to set DSQUERY environment variable.",2);
		return 0;
	}

	// allocate a context
	context=(CS_CONTEXT *)NULL;
	if (cs_ctx_alloc(CS_VERSION_100,&context)!=CS_SUCCEED) {
		logInError("failed to allocate a context structure",2);
		return 0;
	}
	// init the context
	if (ct_init(context,CS_VERSION_100)!=CS_SUCCEED) {
		logInError("failed to initialize a context structure",3);
		return 0;
	}


	// configure the error handling callbacks
	if (cs_config(context,CS_SET,CS_MESSAGE_CB,
		(CS_VOID *)freetdsconnection::csMessageCallback,CS_UNUSED,
			(CS_INT *)NULL)
			!=CS_SUCCEED) {
		logInError("failed to set a cslib error message callback",4);
		return 0;
	}
	if (ct_callback(context,NULL,CS_SET,CS_CLIENTMSG_CB,
		(CS_VOID *)freetdsconnection::clientMessageCallback)
			!=CS_SUCCEED) {
		logInError("failed to set a client error message callback",4);
		return 0;
	}
	if (ct_callback(context,NULL,CS_SET,CS_SERVERMSG_CB,
		(CS_VOID *)freetdsconnection::serverMessageCallback)
			!=CS_SUCCEED) {
		logInError("failed to set a server error message callback",4);
		return 0;
	}


	// allocate a connection
	if (ct_con_alloc(context,&dbconn)!=CS_SUCCEED) {
		logInError("failed to allocate a connection structure",4);
		return 0;
	}


	// set the user to use
	char	*user=getUser();
	if (ct_con_props(dbconn,CS_SET,CS_USERNAME,
			(CS_VOID *)((user && user[0])?user:""),
			CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the user",5);
		return 0;
	}


	// set the password to use
	char	*password=getPassword();
	if (ct_con_props(dbconn,CS_SET,CS_PASSWORD,
			(CS_VOID *)((password && password[0])?password:""),
			CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the password",5);
		return 0;
	}

	// set application name
	if (ct_con_props(dbconn,CS_SET,CS_APPNAME,(CS_VOID *)"sqlrelay",
			CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the application name",5);
		return 0;
	}

	// set hostname
	if (hostname && hostname[0] &&
		ct_con_props(dbconn,CS_SET,CS_HOSTNAME,(CS_VOID *)hostname,
				CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the hostname",5);
		return 0;
	}

	// set packetsize
	if (packetsize && packetsize[0] &&
		ct_con_props(dbconn,CS_SET,CS_PACKETSIZE,
				(CS_VOID *)atoi(packetsize),
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the packetsize",5);
		return 0;
	}

	// set encryption
	if (encryption && atoi(encryption)==1) {
		enc=CS_TRUE;
		if (ct_con_props(dbconn,CS_SET,CS_PACKETSIZE,
			(CS_VOID *)&enc,
			CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the encryption",5);
			return 0;
		}
	}

	// init locale
	locale=NULL;
	if (cs_loc_alloc(context,&locale)!=CS_SUCCEED) {
		logInError("failed to allocate a locale structure",5);
		return 0;
	}
	if (cs_locale(context,CS_SET,locale,CS_LC_ALL,(CS_CHAR *)NULL,
			CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to initialize a locale structure",6);
		return 0;
	}

	// set language
	if (language && language[0] &&
		cs_locale(context,CS_SET,locale,CS_SYB_LANG,
			(CS_CHAR *)language,CS_NULLTERM,(CS_INT *)NULL)!=
				CS_SUCCEED) {
		logInError("failed to set the language",6);
		return 0;
	}

	// set charset
	if (charset && charset[0] &&
		cs_locale(context,CS_SET,locale,CS_SYB_CHARSET,
			(CS_CHAR *)charset,CS_NULLTERM,(CS_INT *)NULL)!=
				CS_SUCCEED) {
		logInError("failed to set the charset",6);
		return 0;
	}

	// set locale
	if (ct_con_props(dbconn,CS_SET,CS_LOC_PROP,(CS_VOID *)locale,
			CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the locale",6);
		return 0;
	}

	// connect to the database
	if (ct_connect(dbconn,(CS_CHAR *)NULL,(CS_INT)0)!=CS_SUCCEED) {
		logInError("failed to connect to the database",6);
		return 0;
	}
	return 1;
}

void	freetdsconnection::logInError(const char *error, int stage) {

	fprintf(stderr,"%s",error);

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

sqlrcursor	*freetdsconnection::initCursor() {
	return (sqlrcursor *)new freetdscursor((sqlrconnection *)this);
}

void	freetdsconnection::deleteCursor(sqlrcursor *curs) {
	delete (freetdscursor *)curs;
}

void	freetdsconnection::logOut() {

	cs_loc_drop(context,locale);
	ct_close(dbconn,CS_UNUSED);
	ct_con_drop(dbconn);
	ct_exit(context,CS_UNUSED);
	cs_ctx_drop(context);
}

char	*freetdsconnection::identify() {
	return "freetds";
}

char	freetdsconnection::bindVariablePrefix() {
	return '@';
}


freetdscursor::freetdscursor(sqlrconnection *conn) : sqlrcursor(conn) {
	freetdsconn=(freetdsconnection *)conn;
	cmd=NULL;
	#ifdef VERSION_NO
		tdsversion=atof(VERSION_NO+9);
	#else
		#ifdef TDS_VERSION_NO
			tdsversion=atof(TDS_VERSION_NO+9);
		#else
			tdsversion=0;
		#endif
	#endif
}

freetdscursor::~freetdscursor() {
	if (cmd) {
		ct_cmd_drop(cmd);
	}
}

int	freetdscursor::openCursor(int id) {

	// switch to the correct database
	int	retval=1;
	if (freetdsconn->db && freetdsconn->db[0]) {
		int	len=strlen(freetdsconn->db)+4;
		char	*query=new char[len+1];
		sprintf(query,"use %s",freetdsconn->db);
		if (!(prepareQuery(query,len) && executeQuery(query,len,1))) {
			int	live;
			fprintf(stderr,"%s\n",getErrorMessage(&live));
			retval=0;
		}
		delete[] query;
		cleanUpData();
	}
	return retval;
}

int	freetdscursor::prepareQuery(const char *query, long length) {

	// this code is here just in case freetds ever supports bind vars
	//paramindex=0;

	if (cmd) {
		ct_cmd_drop(cmd);
	}
	if (ct_cmd_alloc(freetdsconn->dbconn,&cmd)!=CS_SUCCEED) {
		return 0;
	}
	return 1;

	// if freetds ever supports bind vars, the ct_command 
	// call needs to be moved here
}

// this code is here just in case freetds ever supports bind vars
/*int	freetdscursor::inputBindString(const char *variable,
						unsigned short variablesize,
						const char *value,
						unsigned short valuesize,
						short *isnull) {

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

	// don't need to use isnull with sybase because if value is NULL
	// and valuesize==0, then the same thing is achieved
	if (ct_param(cmd,&parameter[paramindex],
			(CS_VOID *)value,valuesize,0)!=CS_SUCCEED) {
		return 0;
	}
	paramindex++;
	return 1;
}

int	freetdscursor::inputBindLong(const char *variable,
						unsigned short variablesize,
						unsigned long *value) {

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
		return 0;
	}
	paramindex++;
	return 1;
}

int	freetdscursor::inputBindDouble(const char *variable,
						unsigned short variablesize,
						double *value,
						unsigned short precision,
						unsigned short scale) {

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
		return 0;
	}
	paramindex++;
	return 1;
}

int	freetdsconnection::outputBindString(const char *variable, 
						unsigned short variablesize,
						char *value, 
						unsigned short valuesize, 
						short *isnull) {

	// this code is here in case SQL Relay ever supports rpc commands

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
			(CS_VOID *)value,valuesize,0)!=CS_SUCCEED) {
		return 0;
	}
	paramindex++;
	return 1;
}*/

int	freetdscursor::executeQuery(const char *query, long length,
						unsigned short execute) {

	returnedcolumns=0;

	// clear out any errors
	if (freetdsconn->errorstring) {
		freetdsconn->deadconnection=0;
		delete freetdsconn->errorstring;
		freetdsconn->errorstring=NULL;
	}

	// if freetds ever supports bind vars, the ct_command call
	// needs to be moved up into prepareQuery()
	
	// since freetds doesn't currently support bind vars, fake them
	stringbuffer	*newquery=fakeInputBinds(query);

	// initiate a language command
	if (newquery) {
		if (ct_command(cmd,CS_LANG_CMD,newquery->getString(),
			strlen(newquery->getString()),CS_UNUSED)!=CS_SUCCEED) {
			delete newquery;
			return 0;
		}
		delete newquery;
	} else {
		if (ct_command(cmd,CS_LANG_CMD,(CS_CHAR *)query,length,
						CS_UNUSED)!=CS_SUCCEED) {
			return 0;
		}
	}

	// initialize return values
	ncols=0;
	affectedrows=0;
	row=0;
	maxrow=0;
	totalrows=0;

	// send the command
	if (ct_send(cmd)!=CS_SUCCEED) {
		return 0;
	}

	// Get the results. Sybase is weird... A query can return multiple
	// result sets?  We're only interested in the first one,
	// the rest will be cancelled.  Return a dead database on failure
	if (ct_results(cmd,&results_type)==CS_FAIL) {
		ct_cancel(freetdsconn->dbconn,NULL,CS_CANCEL_ALL);
		freetdsconn->deadconnection=1;
		return 0;
	}

	// handle a failed command
	if ((int)results_type==CS_CMD_FAIL) {
		ct_cancel(freetdsconn->dbconn,NULL,CS_CANCEL_ALL);
		return 0;
	}

	// For queries which return rows or parameters (output bind variables),
	// get the column count.  For DML queries, get the affected row count.
	// Affected row count is only supported in FreeTDS version>=0.53.
	if (tdsversion<0.53) {
		affectedrows=-1;
	} else {
		affectedrows=0;
	}
	if (results_type==CS_ROW_RESULT) {
		if (ct_res_info(cmd,CS_NUMDATA,(CS_VOID *)&ncols,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			return 0;
		}
		if (ncols>MAX_SELECT_LIST_SIZE) {
			ncols=MAX_SELECT_LIST_SIZE;
		}
	} else if (results_type==CS_CMD_SUCCEED) {
		if (tdsversion>0.52 && ct_res_info(cmd,CS_ROW_COUNT,
				(CS_VOID *)&affectedrows,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			return 0;
		} 
	}

	// bind the columns
	int	moneycolumn=0;
	for (int i=0; i<(int)ncols; i++) {

		// dealing with money columns cause freetds < 0.53 to
		// crash, take care of that here...
		if (tdsversion<0.53 && !moneycolumn) {
			CS_DATAFMT	moneytest;
			ct_describe(cmd,i+1,&moneytest);
			if (moneytest.datatype==CS_MONEY_TYPE ||
				moneytest.datatype==CS_MONEY4_TYPE) {
				moneycolumn=1;
				if (freetdsconn->errorstring) {
					delete freetdsconn->errorstring;
				}
				freetdsconn->errorstring=new stringbuffer();
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
		// no longer than MAX_ITEM_BUFFER_SIZE, override some other
		// values that might have been set also
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

	// If we got a moneycolumn (and tdsversion<0.53) then cancel the
	// result set.  Otherwise FreeTDS will spew "unknown marker"
	// errors to the screen when cleanUpData() is called.
	if (moneycolumn) {
		ct_cancel(freetdsconn->dbconn,NULL,CS_CANCEL_ALL);
	}

	// return success only if no error was generated
	if (freetdsconn->errorstring) {
		return 0;
	}
	return 1;
}

char	*freetdscursor::getErrorMessage(int *liveconnection) {
	if (freetdsconn->deadconnection) {
		*liveconnection=0;
	} else {
		*liveconnection=1;
	}
	if (freetdsconn->errorstring) {
		return freetdsconn->errorstring->getString();
	} else {
		return NULL;
	}
}

void	freetdscursor::returnRowCounts() {

	// send row counts (actual row count unknown in freetds)
	conn->sendRowCounts((long)-1,(long)affectedrows);
}

void	freetdscursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void	freetdscursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	returnedcolumns=1;

	// unless the query was a successful select, send no header
	if (results_type!=CS_ROW_RESULT) {
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
					column[i].scale,0,0,0);
	}
}

int	freetdscursor::noRowsToReturn() {

	// unless the query was a successful select, send no data
	if (results_type!=CS_ROW_RESULT) {
		return 1;
	}
	return 0;
}

int	freetdscursor::skipRow() {
	if (fetchRow()) {
		row++;
		return 1;
	}
	return 0;
}

int	freetdscursor::fetchRow() {

	// this code is here just in case freetds ever supports array fetches
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return 0;
	}
	if (row==0) {
		int result;
		if ((result=ct_fetch(cmd,
				CS_UNUSED,CS_UNUSED,CS_UNUSED,
				&rowsread))!=CS_SUCCEED && rowsread==0) {
			return 0;
		}
		maxrow=rowsread;
		totalrows=totalrows+rowsread;
	}
	return 1;
}

void	freetdscursor::returnRow() {

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


void	freetdscursor::cleanUpData() {

	// Cancel the rest of the result sets.
	// FreeTDS gets pissed off if you try to step through the result sets,
	// cancelling each one if you didn't ct_describe it earlier, or if you
	// didn't fetch all of the rows.  It doesn't seem to mind if you cancel
	// all the result sets at once though.
	ct_cancel(freetdsconn->dbconn,NULL,CS_CANCEL_ALL);
}

CS_RETCODE	freetdsconnection::csMessageCallback(CS_CONTEXT *ctxt, 
						CS_CLIENTMSG *msgp) {
	if (errorstring) {
		delete errorstring;
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
	errorstring->append("Error:	")->
			append(msgp->msgstring)->
			append("\n");

	if (msgp->osstringlen>0) {
		errorstring->append("Operating System Error:\n\n ");
		errorstring->append(msgp->osstring)->append("\n");
	}

	// for a timeout message, set deadconnection to 1
	if (CS_SEVERITY(msgp->msgnumber)==CS_SV_RETRY_FAIL &&
		CS_LAYER(msgp->msgnumber)==63 &&
		CS_ORIGIN(msgp->msgnumber)==63 &&
		CS_NUMBER(msgp->msgnumber)==63) {
		deadconnection=1;
	}

	return CS_SUCCEED;
}

CS_RETCODE	freetdsconnection::clientMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp) {
	if (errorstring) {
		delete errorstring;
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
	errorstring->append("Error:	")->
			append(msgp->msgstring)->
			append("\n");

	if (msgp->osstringlen>0) {
		errorstring->append("Operating System Error:\n\n ");
		errorstring->append(msgp->osstring)->append("\n");
	}

	// for a timeout message, set deadconnection to 1
	if (CS_SEVERITY(msgp->msgnumber)==CS_SV_RETRY_FAIL &&
		CS_NUMBER(msgp->msgnumber)==63 &&
		CS_ORIGIN(msgp->msgnumber)==63 &&
		CS_LAYER(msgp->msgnumber)==63) {
		deadconnection=1;
	}

	return CS_SUCCEED;
}

CS_RETCODE	freetdsconnection::serverMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp) {

	// This is a special case, for some reason, "use db" queries
	// throw a warning, ignore them.
	if (msgp->msgnumber==5701 &&
			(msgp->severity==10 || msgp->severity==0)) {
		return CS_SUCCEED;
	}

	if (errorstring) {
		delete errorstring;
	}
	errorstring=new stringbuffer();

	errorstring->append("Server message:\n");
	errorstring->append("	severity(")->
			append((long)msgp->severity)->append(")\n");
	errorstring->append("	number(")->
			append((long)msgp->msgnumber)->append(")\n");
	errorstring->append("	state(")->
			append((long)msgp->state)->append(")\n");
	errorstring->append("	line(")->
			append((long)msgp->line)->append(")\n");
	errorstring->append("Server Name:	")->
			append(msgp->svrname)->append("\n");
	errorstring->append("Procedure Name:	")->
			append(msgp->proc)->append("\n");
	errorstring->append("Error:	")->
			append(msgp->text)->append("\n");

	return CS_SUCCEED;
}
