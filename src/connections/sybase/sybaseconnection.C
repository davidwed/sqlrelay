// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sybaseconnection.h>

#include <config.h>
#include <datatypes.h>

#include <rudiments/stringbuffer.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __GNUC__
static	stringbuffer	*sybaseconnection::errorstring;
static	int		sybaseconnection::deadconnection;
#endif


sybaseconnection::sybaseconnection() {
	errorstring=NULL;
	sybaseenv=NULL;
	dsqueryenv=NULL;
}

sybaseconnection::~sybaseconnection() {
	delete[] sybaseenv;
	delete[] dsqueryenv;
	delete errorstring;
}

int	sybaseconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	sybaseconnection::handleConnectString() {
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

int	sybaseconnection::logIn() {

	// set sybase
	if (sybase && sybase[0]) {
		sybaseenv=new char[strlen(sybase)+8];
		sprintf(sybaseenv,"SYBASE=%s",sybase);
		if (!setEnv("SYBASE",sybase,sybaseenv)) {
			logInError("Failed to set SYBASE environment variable.",1);
			return 0;
		}
	}

	// set server
	if (server && server[0]) {
		dsqueryenv=new char[strlen(server)+9];
		sprintf(dsqueryenv,"DSQUERY=%s",server);
		if (!setEnv("DSQUERY",server,dsqueryenv)) {
			logInError("Failed to set DSQUERY environment variable.",2);
			return 0;
		}
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
		(CS_VOID *)sybaseconnection::csMessageCallback,CS_UNUSED,
			(CS_INT *)NULL)
			!=CS_SUCCEED) {
		logInError("failed to set a cslib error message callback",4);
		return 0;
	}
	if (ct_callback(context,NULL,CS_SET,CS_CLIENTMSG_CB,
		(CS_VOID *)sybaseconnection::clientMessageCallback)
			!=CS_SUCCEED) {
		logInError("failed to set a client error message callback",4);
		return 0;
	}
	if (ct_callback(context,NULL,CS_SET,CS_SERVERMSG_CB,
		(CS_VOID *)sybaseconnection::serverMessageCallback)
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
	if (user && user[0]) {
		if (ct_con_props(dbconn,CS_SET,CS_USERNAME,(CS_VOID *)user,
				CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the user",5);
			return 0;
		}
	} else {
		if (ct_con_props(dbconn,CS_SET,CS_USERNAME,(CS_VOID *)"",
				CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the user",5);
			return 0;
		}
	}


	// set the password to use
	char	*password=getPassword();
	if (password && password[0]) {
		if (ct_con_props(dbconn,CS_SET,CS_PASSWORD,(CS_VOID *)password,
				CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the password",5);
			return 0;
		}
	} else {
		if (ct_con_props(dbconn,CS_SET,CS_PASSWORD,(CS_VOID *)"",
				CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the password",5);
			return 0;
		}
	}

	// set application name
	if (ct_con_props(dbconn,CS_SET,CS_APPNAME,(CS_VOID *)"sqlrelay",
			CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the application name",5);
		return 0;
	}

	// set hostname
	if (hostname && hostname[0]) {
		if (ct_con_props(dbconn,CS_SET,CS_HOSTNAME,(CS_VOID *)hostname,
				CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the hostname",5);
			return 0;
		}
	}

	// set packetsize
	if (packetsize && packetsize[0]) {
		if (ct_con_props(dbconn,CS_SET,CS_PACKETSIZE,
				(CS_VOID *)atoi(packetsize),
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the packetsize",5);
			return 0;
		}
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
	if (language && language[0]) {
		if (cs_locale(context,CS_SET,locale,CS_SYB_LANG,
			(CS_CHAR *)language,CS_NULLTERM,(CS_INT *)NULL)!=
				CS_SUCCEED) {
			logInError("failed to set the language",6);
			return 0;
		}
	}

	// set charset
	if (charset && charset[0]) {
		if (cs_locale(context,CS_SET,locale,CS_SYB_CHARSET,
			(CS_CHAR *)charset,CS_NULLTERM,(CS_INT *)NULL)!=
				CS_SUCCEED) {
			logInError("failed to set the charset",6);
			return 0;
		}
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

void	sybaseconnection::logInError(const char *error, int stage) {
	errorstring=new stringbuffer();
	errorstring->append(error);

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
	if (stage>1) {
		delete[] dsqueryenv;
		dsqueryenv=NULL;
	}
	if (stage>0) {
		delete[] sybaseenv;
		sybaseenv=NULL;
	}
}

sqlrcursor	*sybaseconnection::initCursor() {
	return (sqlrcursor *)new sybasecursor((sqlrconnection *)this);
}

void	sybaseconnection::deleteCursor(sqlrcursor *curs) {
	delete (sybasecursor *)curs;
}

void	sybaseconnection::logOut() {

	cs_loc_drop(context,locale);
	ct_close(dbconn,CS_UNUSED);
	ct_con_drop(dbconn);
	ct_exit(context,CS_UNUSED);
	cs_ctx_drop(context);

	delete[] sybaseenv;
	sybaseenv=NULL;
	delete[] dsqueryenv;
	dsqueryenv=NULL;
}

int	sybaseconnection::ping() {
	sybasecursor	cur(this);
	if (cur.openCursor(-1) && 
		cur.prepareQuery("select 1",8) && 
		cur.executeQuery("select 1",8,1)) {
		cur.cleanUpData();
		cur.closeCursor();
		return 1;
	}
	cur.closeCursor();
	return 0;
}

char	*sybaseconnection::identify() {
	return "sybase";
}

char	sybaseconnection::bindVariablePrefix() {
	return '@';
}

sybasecursor::sybasecursor(sqlrconnection *conn) : sqlrcursor(conn) {
	prepared=0;
	sybaseconn=(sybaseconnection *)conn;
	cmd=NULL;
}

sybasecursor::~sybasecursor() {
	if (cmd) {
		ct_cmd_drop(cmd);
	}
}

int	sybasecursor::openCursor(int id) {

	// switch to the correct database
	int	retval=1;
	if (sybaseconn->db && sybaseconn->db[0]) {
		int	len=strlen(sybaseconn->db)+4;
		char	*query=new char[len+1];
		sprintf(query,"use %s",sybaseconn->db);
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

int	sybasecursor::prepareQuery(const char *query, long length) {

	this->query=(char *)query;
	this->length=length;

	paramindex=0;

	if (cmd) {
		ct_cmd_drop(cmd);
	}
	if (ct_cmd_alloc(sybaseconn->dbconn,&cmd)!=CS_SUCCEED) {
		return 0;
	}

	// initiate a language command
	if (ct_command(cmd,CS_LANG_CMD,(CS_CHAR *)query,
				length,CS_UNUSED)!=CS_SUCCEED) {
		return 0;
	}

	prepared=1;
	return 1;
}

void	sybasecursor::checkRePrepare() {

	// Sybase doesn't allow you to rebind and re-execute when using 
	// ct_command.  You have to re-prepare too.  I'll make this transparent
	// to the user.
	if (!prepared) {
		prepareQuery(query,length);
	}
}

int	sybasecursor::inputBindString(const char *variable,
						unsigned short variablesize,
						const char *value,
						unsigned short valuesize,
						short *isnull) {

	checkRePrepare();

	(CS_VOID)memset(&parameter[paramindex],0,
			sizeof(parameter[paramindex]));
	if (isNumber(variable+1,variablesize-1)) {
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
		return 0;
	}
	paramindex++;
	return 1;
}

int	sybasecursor::inputBindLong(const char *variable,
						unsigned short variablesize,
						unsigned long *value) {

	checkRePrepare();

	(CS_VOID)memset(&parameter[paramindex],0,
			sizeof(parameter[paramindex]));
	if (isNumber(variable+1,variablesize-1)) {
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

int	sybasecursor::inputBindDouble(const char *variable,
						unsigned short variablesize,
						double *value,
						unsigned short precision,
						unsigned short scale) {

	checkRePrepare();

	(CS_VOID)memset(&parameter[paramindex],0,
			sizeof(parameter[paramindex]));
	if (isNumber(variable+1,variablesize-1)) {
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

int	sybasecursor::outputBindString(const char *variable, 
					unsigned short variablesize,
					char *value, 
					unsigned short valuesize, 
					short *isnull) {

	// this code is here in case SQL Relay ever supports rpc commands

	/*(CS_VOID)memset(&parameter[paramindex],0,
			sizeof(parameter[paramindex]));
	if (isNumber(variable+1,variablesize-1)) {
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
			(CS_VOID *)value,valuesize,
			(CS_SMALLINT)*isnull)!=CS_SUCCEED) {
		return 0;
	}
	paramindex++;*/
	return 1;
}

int	sybasecursor::executeQuery(const char *query, long length,
						unsigned short execute) {

	// clear out any errors
	if (sybaseconn->errorstring) {
		sybaseconn->deadconnection=0;
		delete sybaseconn->errorstring;
		sybaseconn->errorstring=NULL;
	}

	// initialize return values
	ncols=0;
	affectedrows=0;
	row=0;
	maxrow=0;
	totalrows=0;

	if (ct_send(cmd)!=CS_SUCCEED) {
		return 0;
	}

	// Get the results. Sybase is weird... A query can return multiple
	// result sets.  We're only interested in the first one, the rest will 
	// be cancelled.  Return a dead database on failure
	if (ct_results(cmd,&results_type)==CS_FAIL) {
		ct_cancel(sybaseconn->dbconn,NULL,CS_CANCEL_ALL);
		sybaseconn->deadconnection=1;
		return 0;
	}

	// handle a failed command
	if ((int)results_type==CS_CMD_FAIL) {
		ct_cancel(sybaseconn->dbconn,NULL,CS_CANCEL_ALL);
		return 0;
	}

	// reset the prepared flag
	prepared=0;


	// For queries which return rows or parameters (output bind variables),
	// get the column count.  For DML queries, get the affected row count.
	affectedrows=0;
	if (results_type==CS_ROW_RESULT) {
		if (ct_res_info(cmd,CS_NUMDATA,(CS_VOID *)&ncols,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			return 0;
		}
		if (ncols>MAX_SELECT_LIST_SIZE) {
			ncols=MAX_SELECT_LIST_SIZE;
		}
	} else if (results_type==CS_CMD_SUCCEED) {
		if (ct_res_info(cmd,CS_ROW_COUNT,(CS_VOID *)&affectedrows,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			return 0;
		} 
	}

	// bind columns
	for (int i=0; i<(int)ncols; i++) {

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
		if (results_type==CS_ROW_RESULT) {
			if (ct_bind(cmd,i+1,&column[i],(CS_VOID *)data[i],
				datalength[i],nullindicator[i])!=CS_SUCCEED) {
				break;
			}
		}
	}

	// return success only if no error was generated
	if (sybaseconn->errorstring) {
		return 0;
	}
	return 1;
}

char	*sybasecursor::getErrorMessage(int *liveconnection) {
	if (sybaseconn->deadconnection) {
		*liveconnection=0;
	} else {
		*liveconnection=1;
	}
	if (sybaseconn->errorstring) {
		return sybaseconn->errorstring->getString();
	} else {
		return NULL;
	}
}

void	sybasecursor::returnRowCounts() {

	// send row counts (actual row count unknown in sybase)
	conn->sendRowCounts((long)-1,(long)affectedrows);
}

void	sybasecursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void	sybasecursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

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
					column[i].maxlength);
	}
}

int	sybasecursor::noRowsToReturn() {

	// unless the query was a successful select, send no data
	if (results_type!=CS_ROW_RESULT) {
		return 1;
	}
	return 0;
}

int	sybasecursor::skipRow() {
	if (fetchRow()) {
		row++;
		return 1;
	}
	return 0;
}

int	sybasecursor::fetchRow() {
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return 0;
	}
	if (row==0) {
		if (ct_fetch(cmd,CS_UNUSED,CS_UNUSED,CS_UNUSED,
				&rowsread)!=CS_SUCCEED && rowsread==0) {
			return 0;
		}
		maxrow=rowsread;
		totalrows=totalrows+rowsread;
	}
	return 1;
}

void	sybasecursor::returnRow() {

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


void	sybasecursor::cleanUpData() {

	// cancel the rest of the result sets
	CS_INT	return_code;
	while ((return_code=ct_results(cmd,&results_type))==CS_SUCCEED) {
		ct_cancel(sybaseconn->dbconn,cmd,CS_CANCEL_CURRENT);
	}

	// return a dead database on absolute failre
	if (return_code==CS_FAIL) {
		ct_cancel(NULL,cmd,CS_CANCEL_ALL);
		sybaseconn->deadconnection=1;
	}
}

CS_RETCODE	sybaseconnection::csMessageCallback(CS_CONTEXT *ctxt, 
						CS_CLIENTMSG *msgp) {
	if (errorstring) {
		delete errorstring;
	}
	errorstring=new stringbuffer();

	errorstring->append("Client Library error:\n");
	errorstring->append("	severity(%d)\n")->
				append((long)CS_SEVERITY(msgp->msgnumber));
	errorstring->append("	layer(%d)\n")->
				append((long)CS_LAYER(msgp->msgnumber));
	errorstring->append("	origin(%d)\n")->
				append((long)CS_ORIGIN(msgp->msgnumber));
	errorstring->append("	number(%d)\n")->
				append((long)CS_NUMBER(msgp->msgnumber));
	errorstring->append("Error:	%s\n")->append(msgp->msgstring);

	if (msgp->osstringlen>0) {
		errorstring->append("Operating System Error:\n");
		errorstring->append("\n	%s\n")->append(msgp->osstring);
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

CS_RETCODE	sybaseconnection::clientMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp) {
	if (errorstring) {
		delete errorstring;
	}
	errorstring=new stringbuffer();

	errorstring->append("Client Library error:\n");
	errorstring->append("	severity(%d)\n")->
				append((long)CS_SEVERITY(msgp->msgnumber));
	errorstring->append("	layer(%d)\n")->
				append((long)CS_LAYER(msgp->msgnumber));
	errorstring->append("	origin(%d)\n")->
				append((long)CS_ORIGIN(msgp->msgnumber));
	errorstring->append("	number(%d)\n")->
				append((long)CS_NUMBER(msgp->msgnumber));
	errorstring->append("Error:	%s\n")->append(msgp->msgstring);

	if (msgp->osstringlen>0) {
		errorstring->append("Operating System Error:\n");
		errorstring->append("\n	%s\n")->append(msgp->osstring);
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

CS_RETCODE	sybaseconnection::serverMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp) {

	// This is a special case, for some reason, "use db" queries
	// throw a warning, ignore them.
	if (msgp->msgnumber==5701 && msgp->severity==10) {
		return CS_SUCCEED;
	}

	if (errorstring) {
		delete errorstring;
	}
	errorstring=new stringbuffer();

	errorstring->append("Server message:\n");
	errorstring->append("	severity(%d)\n")->append((long)msgp->severity);
	errorstring->append("	number(%d)\n")->append((long)msgp->msgnumber);
	errorstring->append("	state(%d)\n")->append((long)msgp->state);
	errorstring->append("	line(%d)\n")->append((long)msgp->line);
	errorstring->append("Server Name:\n%s\n")->append(msgp->svrname);
	errorstring->append("Procedure Name:\n%s\n")->append(msgp->proc);
	errorstring->append("Error:\n%s\n")->append(msgp->text);

	return CS_SUCCEED;
}
