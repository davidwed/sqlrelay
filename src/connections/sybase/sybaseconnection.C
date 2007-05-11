// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sybaseconnection.h>

#include <config.h>
#include <datatypes.h>

#include <rudiments/stringbuffer.h>
#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>

#include <stdio.h>
#include <stdlib.h>

stringbuffer	*sybaseconnection::errorstring;
bool		sybaseconnection::deadconnection;


sybaseconnection::sybaseconnection() : sqlrconnection_svr() {
	errorstring=NULL;
	dbused=false;
	dbversion=NULL;
}

sybaseconnection::~sybaseconnection() {
	delete errorstring;
	delete[] dbversion;
}

uint16_t sybaseconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void sybaseconnection::handleConnectString() {
	sybase=connectStringValue("sybase");
	lang=connectStringValue("lang");
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	server=connectStringValue("server");
	db=connectStringValue("db");
	charset=connectStringValue("charset");
	language=connectStringValue("language");
	hostname=connectStringValue("hostname");
	packetsize=connectStringValue("packetsize");
}

bool sybaseconnection::logIn(bool printerrors) {

	// set sybase
	if (sybase && sybase[0] && !environment::setValue("SYBASE",sybase)) {
		logInError("Failed to set SYBASE environment variable.",1);
		return false;
	}

	// set lang
	if (lang && lang[0] && !environment::setValue("LANG",lang)) {
		logInError("Failed to set LANG environment variable.",1);
		return false;
	}

	// set server
	if (server && server[0] && !environment::setValue("DSQUERY",server)) {
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
		(CS_VOID *)sybaseconnection::csMessageCallback,CS_UNUSED,
			(CS_INT *)NULL)
			!=CS_SUCCEED) {
		logInError("failed to set a cslib error message callback",4);
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_CLIENTMSG_CB,
		(CS_VOID *)sybaseconnection::clientMessageCallback)
			!=CS_SUCCEED) {
		logInError("failed to set a client error message callback",4);
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_SERVERMSG_CB,
		(CS_VOID *)sybaseconnection::serverMessageCallback)
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
	const char	*user=getUser();
	if (ct_con_props(dbconn,CS_SET,CS_USERNAME,
			(CS_VOID *)((user && user[0])?user:""),
			CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the user",5);
		return false;
	}


	// set the password to use
	const char	*password=getPassword();
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
	uint16_t	ps=charstring::toInteger(packetsize);
	if (packetsize && packetsize[0] &&
		ct_con_props(dbconn,CS_SET,CS_PACKETSIZE,
				(CS_VOID *)&ps,sizeof(ps),
				(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the packetsize",5);
		return false;
	}

	// FIXME: support this
	// set encryption
	/*if (encryption && charstring::toInteger(encryption)==1) {
		// FIXME: need to set CS_SEC_CHALLENGE/CS_SEC_NEGOTIATE
		// parameters too
		CS_INT	enc=CS_TRUE;
		if (ct_con_props(dbconn,CS_SET,CS_SEC_ENCRYPTION,
			(CS_VOID *)&enc,
			CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			logInError("failed to set the encryption",5);
			return false;
		}
	}*/

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

void sybaseconnection::logInError(const char *error, uint16_t stage) {

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

sqlrcursor_svr *sybaseconnection::initCursor() {
	return (sqlrcursor_svr *)new sybasecursor((sqlrconnection_svr *)this);
}

void sybaseconnection::deleteCursor(sqlrcursor_svr *curs) {
	delete (sybasecursor *)curs;
}

void sybaseconnection::logOut() {
	cs_loc_drop(context,locale);
	ct_close(dbconn,CS_UNUSED);
	ct_con_drop(dbconn);
	ct_exit(context,CS_UNUSED);
	cs_ctx_drop(context);
}

const char *sybaseconnection::identify() {
	return "sybase";
}

const char *sybaseconnection::dbVersion() {
	return dbversion;
}

const char *sybaseconnection::bindFormat() {
	return "@*";
}

char sybaseconnection::bindVariablePrefix() {
	return '@';
}

sybasecursor::sybasecursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {
	prepared=false;
	sybaseconn=(sybaseconnection *)conn;
	cmd=NULL;
	languagecmd=NULL;
	cursorcmd=NULL;
	cursorname=NULL;

	// replace the regular expression used to detect creation of a
	// temporary table
	createtemp.compile("(create|CREATE)[ \\t\\r\\n]+(table|TABLE)[ \\t\\r\\n]+#");
	createtemp.study();

	cursorquery.compile("^(select|SELECT)[ \\t\\r\\n]+");
	cursorquery.study();

	rpcquery.compile("^(execute|exec|EXECUTE|EXEC)[ \\t\\r\\n]+");
	rpcquery.study();
}

sybasecursor::~sybasecursor() {
	closeCursor();
	delete[] cursorname;
}

bool sybasecursor::openCursor(uint16_t id) {

	clean=true;

	cursorname=charstring::parseNumber(id);

	if (ct_cmd_alloc(sybaseconn->dbconn,&languagecmd)!=CS_SUCCEED) {
		return false;
	}
	if (ct_cmd_alloc(sybaseconn->dbconn,&cursorcmd)!=CS_SUCCEED) {
		return false;
	}
	cmd=NULL;

	// switch to the correct database, get dbversion
	// (only do this once per connection)
	bool	retval=true;
	if (sybaseconn->db && sybaseconn->db[0] && !sybaseconn->dbused) {
		int32_t	len=charstring::length(sybaseconn->db)+4;
		char	query[len+1];
		snprintf(query,len+1,"use %s",sybaseconn->db);
		if (!(prepareQuery(query,len) &&
				executeQuery(query,len,true))) {
			bool	live;
			fprintf(stderr,"%s\n",errorMessage(&live));
			retval=false;
		} else {
			sybaseconn->dbused=true;
		}
		cleanUpData(true,true);
	}

	if (!sybaseconn->dbversion) {
		char	*query="sp_version installmaster";
		int32_t	len=charstring::length(query);
		if (!(prepareQuery(query,len) &&
				executeQuery(query,len,true) &&
				fetchRow())) {
			bool	live;
			fprintf(stderr,"%s\n",errorMessage(&live));
			retval=false;
		} else {
			const char	*space=
				charstring::findFirst(data[1][0],' ');
			sybaseconn->dbversion=
				charstring::duplicate(data[1][0],
							space-data[1][0]);
		}
		cleanUpData(true,true);
	}
	return (retval && sqlrcursor_svr::openCursor(id));
}

bool sybasecursor::closeCursor() {
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

bool sybasecursor::prepareQuery(const char *query, uint32_t length) {

	clean=true;

	this->query=(char *)query;
	this->length=length;

	paramindex=0;
	outbindindex=0;

	isrpcquery=false;

	if (cursorquery.match(query)) {

		// initiate a cursor command
		cmd=cursorcmd;
		if (ct_cursor(cursorcmd,CS_CURSOR_DECLARE,
					(CS_CHAR *)cursorname,CS_NULLTERM,
					(CS_CHAR *)query,length,
					CS_READ_ONLY)!=CS_SUCCEED) {
			return false;
		}

	} else if (rpcquery.match(query)) {

		// initiate an rpc command
		isrpcquery=true;
		cmd=languagecmd;
		if (ct_command(languagecmd,CS_RPC_CMD,
				(CS_CHAR *)rpcquery.getSubstringEnd(0),
				length-rpcquery.getSubstringEndOffset(0),
				CS_UNUSED)!=CS_SUCCEED) {
			return false;
		}

	} else {

		// initiate a language command
		cmd=languagecmd;
		if (ct_command(languagecmd,CS_LANG_CMD,
					(CS_CHAR *)query,length,
					CS_UNUSED)!=CS_SUCCEED) {
			return false;
		}
	}

	clean=false;
	prepared=true;
	return true;
}

void sybasecursor::checkRePrepare() {

	// Sybase doesn't allow you to rebind and re-execute when using 
	// ct_command.  You have to re-prepare too.  I'll make this transparent
	// to the user.
	if (!prepared) {
		prepareQuery(query,length);
	}
}

bool sybasecursor::inputBindString(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint16_t valuesize,
						int16_t *isnull) {

	checkRePrepare();

	(CS_VOID)rawbuffer::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		charstring::copy(parameter[paramindex].name,variable);
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

bool sybasecursor::inputBindInteger(const char *variable,
						uint16_t variablesize,
						int64_t *value) {

	checkRePrepare();

	(CS_VOID)rawbuffer::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		charstring::copy(parameter[paramindex].name,variable);
		parameter[paramindex].namelen=variablesize;
	}
	parameter[paramindex].datatype=CS_INT_TYPE;
	parameter[paramindex].maxlength=CS_UNUSED;
	parameter[paramindex].status=CS_INPUTVALUE;
	parameter[paramindex].locale=NULL;
	if (ct_param(cmd,&parameter[paramindex],
		(CS_VOID *)value,sizeof(int64_t),0)!=CS_SUCCEED) {
		return false;
	}
	paramindex++;
	return true;
}

bool sybasecursor::inputBindDouble(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale) {

	checkRePrepare();

	(CS_VOID)rawbuffer::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		charstring::copy(parameter[paramindex].name,variable);
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

bool sybasecursor::outputBindString(const char *variable, 
					uint16_t variablesize,
					char *value, 
					uint16_t valuesize, 
					int16_t *isnull) {

	checkRePrepare();

	outbindtype[outbindindex]=CS_CHAR_TYPE;
	outbindstrings[outbindindex]=value;
	outbindstringlengths[outbindindex]=valuesize;
	outbindindex++;

	(CS_VOID)rawbuffer::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		charstring::copy(parameter[paramindex].name,variable);
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
}

bool sybasecursor::outputBindInteger(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull) {

	checkRePrepare();

	outbindtype[outbindindex]=CS_INT_TYPE;
	outbindints[outbindindex]=value;
	outbindindex++;

	(CS_VOID)rawbuffer::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		charstring::copy(parameter[paramindex].name,variable);
		parameter[paramindex].namelen=variablesize;
	}
	parameter[paramindex].datatype=CS_INT_TYPE;
	parameter[paramindex].maxlength=CS_UNUSED;
	parameter[paramindex].status=CS_RETURN;
	parameter[paramindex].locale=NULL;
	if (ct_param(cmd,&parameter[paramindex],
			(CS_VOID *)NULL,0,
			(CS_SMALLINT)*isnull)!=CS_SUCCEED) {
		return false;
	}
	paramindex++;
	return true;
}

bool sybasecursor::outputBindDouble(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull) {

	checkRePrepare();

	outbindtype[outbindindex]=CS_FLOAT_TYPE;
	outbinddoubles[outbindindex]=value;
	outbindindex++;

	(CS_VOID)rawbuffer::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]=(char)NULL;
		parameter[paramindex].namelen=0;
	} else {
		charstring::copy(parameter[paramindex].name,variable);
		parameter[paramindex].namelen=variablesize;
	}
	parameter[paramindex].datatype=CS_FLOAT_TYPE;
	parameter[paramindex].maxlength=CS_UNUSED;
	parameter[paramindex].status=CS_RETURN;
	parameter[paramindex].locale=NULL;
	if (ct_param(cmd,&parameter[paramindex],
			(CS_VOID *)NULL,0,
			(CS_SMALLINT)*isnull)!=CS_SUCCEED) {
		return false;
	}
	paramindex++;
	return true;
}

bool sybasecursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

	// clear out any errors
	if (sybaseconn->errorstring) {
		sybaseconn->deadconnection=false;
		delete sybaseconn->errorstring;
		sybaseconn->errorstring=NULL;
	}

	// initialize return values
	ncols=0;
	affectedrows=0;
	row=0;
	maxrow=0;
	totalrows=0;

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
				// For rpc commands, there could be several
				// result sets - CS_STATUS_RESULT,
				// maybe a CS_PARAM_RESULT and maybe a
				// CS_ROW_RESULT, we're not guaranteed
				// what order they'll come in though, what
				// a pickle...
				// For now, we care about the CS_PARAM_RESULT,
				// or the CS_ROW_RESULT, whichever we get first,
				// presumably there will only be 1 row in the
				// CS_PARAM_RESULT...
				if (resultstype==CS_PARAM_RESULT ||
						resultstype==CS_ROW_RESULT) {
					break;
				}
			} else {
				// For non-rpc language commands (non-selects),
				// there should be only one result set.
				break;
			}

		} else if (resultstype==CS_ROW_RESULT ||
					resultstype==CS_CURSOR_RESULT ||
					resultstype==CS_COMPUTE_RESULT) {
			// For cursor commands (selects), each call to
			// ct_cursor will have generated a result set.  There
			// will be result sets for the CS_CURSOR_DECLARE,
			// CS_CURSOR_ROWS and CS_CURSOR_OPEN calls.  We need to
			// skip past the first 2, unless they failed.  If they
			// failed, it will be caught above.
			break;
		}

		// if we got here, then we don't want to process this result
		// set, cancel it and move on to the next one...
		if (ct_cancel(NULL,cmd,CS_CANCEL_CURRENT)==CS_FAIL) {
			sybaseconn->deadconnection=true;
			// FIXME: call ct_close(CS_FORCE_CLOSE)
			return false;
		}
	}

	checkForTempTable(query,length);

	// reset the prepared flag
	prepared=false;

	// For queries which return rows or parameters (output bind variables),
	// get the column count and bind columns.  For DML queries, get the
	// affected row count.
	affectedrows=0;
	if (resultstype==CS_ROW_RESULT ||
			resultstype==CS_CURSOR_RESULT ||
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
		for (CS_INT i=0; i<ncols; i++) {

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
		if (ct_res_info(cmd,CS_ROW_COUNT,(CS_VOID *)&affectedrows,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			return false;
		} 
	}

	// if we're doing an rpc query, the result set should be a single
	// row of output parameter results, fetch it and populate the output
	// bind variables...
	if (isrpcquery && resultstype==CS_PARAM_RESULT) {

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
			if (outbindtype[i]==CS_CHAR_TYPE) {
				CS_INT	length=outbindstringlengths[i];
				if (datalength[i][0]<length) {
					length=datalength[i][0];
				}
				rawbuffer::copy(outbindstrings[i],
						data[i][0],length);
			} else if (outbindtype[i]==CS_INT_TYPE) {
				*outbindints[i]=charstring::toInteger(
								data[i][0]);
			} else if (outbindtype[i]==CS_FLOAT_TYPE) {
				*outbinddoubles[i]=charstring::toFloat(
								data[i][0]);
			}
		}

		discardResults();
		ncols=0;
	}

	// return success only if no error was generated
	if (sybaseconn->errorstring) {
		return false;
	}
	return true;
}

const char *sybasecursor::errorMessage(bool *liveconnection) {
	if (sybaseconn->deadconnection) {
		*liveconnection=false;
	} else {
		*liveconnection=true;
	}
	if (sybaseconn->errorstring) {
		return sybaseconn->errorstring->getString();
	} else {
		return NULL;
	}
}

bool sybasecursor::knowsRowCount() {
	return false;
}

uint64_t sybasecursor::rowCount() {
	return 0;
}

bool sybasecursor::knowsAffectedRows() {
	return true;
}

uint64_t sybasecursor::affectedRows() {
	return affectedrows;
}

uint32_t sybasecursor::colCount() {
	return ncols;
}

const char * const * sybasecursor::columnNames() {
	for (CS_INT i=0; i<ncols; i++) {
		if (ct_describe(cmd,i+1,&column[i])!=CS_SUCCEED) {
			break;
		}
		columnnames[i]=column[i].name;
	}
	return columnnames;
}

uint16_t sybasecursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_IDS;
}

void sybasecursor::returnColumnInfo() {

	// unless the query was a successful select, send no header
	if (resultstype!=CS_ROW_RESULT &&
			resultstype!=CS_CURSOR_RESULT &&
			resultstype!=CS_COMPUTE_RESULT) {
		return;
	}

	// gonna need this later
	int16_t	type;

	// for each column...
	for (CS_INT i=0; i<ncols; i++) {

		// get the column description
		if (ct_describe(cmd,i+1,&column[i])!=CS_SUCCEED) {
			break;
		}
	
		// set the datatype
		uint16_t	binary=0;
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
					charstring::length(column[i].name),
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

bool sybasecursor::noRowsToReturn() {
	// unless the query was a successful select, send no data
	return (resultstype!=CS_ROW_RESULT &&
			resultstype!=CS_CURSOR_RESULT &&
			resultstype!=CS_COMPUTE_RESULT);
}

bool sybasecursor::skipRow() {
	if (fetchRow()) {
		row++;
		return true;
	}
	return false;
}

bool sybasecursor::fetchRow() {
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

void sybasecursor::returnRow() {

	// send each row back
	for (CS_INT col=0; col<ncols; col++) {
		if (nullindicator[col][row]>-1 && datalength[col][row]) {
			conn->sendField(data[col][row],datalength[col][row]-1);
		} else {
			conn->sendNullField();
		}
	}
	row++;
}


void sybasecursor::cleanUpData(bool freeresult, bool freebinds) {

	if (clean) {
		return;
	}

	if (freeresult) {
		discardResults();
		discardCursor();
	}

	clean=true;
}

void sybasecursor::discardResults() {

	// if there are any unprocessed result sets, process them
	if (results==CS_SUCCEED) {
		do {
			if (ct_cancel(NULL,cmd,CS_CANCEL_CURRENT)==CS_FAIL) {
				sybaseconn->deadconnection=true;
				// FIXME: call ct_close(CS_FORCE_CLOSE)
				// maybe return false
			}
			results=ct_results(cmd,&resultstype);
		} while (results==CS_SUCCEED);
	}

	if (results==CS_FAIL) {
		if (ct_cancel(NULL,cmd,CS_CANCEL_ALL)==CS_FAIL) {
			sybaseconn->deadconnection=true;
			// FIXME: call ct_close(CS_FORCE_CLOSE)
			// maybe return false
		}
	}
}


void sybasecursor::discardCursor() {

	if (cmd==cursorcmd) {
		if (ct_cursor(cursorcmd,CS_CURSOR_CLOSE,NULL,CS_UNUSED,
				NULL,CS_UNUSED,CS_DEALLOC)==CS_SUCCEED) {
			if (ct_send(cursorcmd)==CS_SUCCEED) {
				results=ct_results(cmd,&resultstype);
				discardResults();
			}
		}
	}
}

CS_RETCODE sybaseconnection::csMessageCallback(CS_CONTEXT *ctxt, 
						CS_CLIENTMSG *msgp) {
	if (errorstring) {
		return CS_SUCCEED;
	}
	errorstring=new stringbuffer();

	errorstring->append("Client Library error:\n");
	errorstring->append("	severity(")->
				append((int32_t)CS_SEVERITY(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	layer(")->
				append((int32_t)CS_LAYER(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	origin(")->
				append((int32_t)CS_ORIGIN(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	number(")->
				append((int32_t)CS_NUMBER(msgp->msgnumber))->
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
	} else
	// for a net-libraryoperation terminated due to disconnect,
	// set deadconnection to 1
	if (CS_SEVERITY(msgp->msgnumber)==5 &&
		CS_LAYER(msgp->msgnumber)==5 &&
		CS_ORIGIN(msgp->msgnumber)==3 &&
		CS_NUMBER(msgp->msgnumber)==6) {
		deadconnection=true;
	}
	// FIXME: freetds connection has another case, do we need it?

	return CS_SUCCEED;
}

CS_RETCODE sybaseconnection::clientMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp) {
	if (errorstring) {
		return CS_SUCCEED;
	}
	errorstring=new stringbuffer();

	errorstring->append("Client Library error:\n");
	errorstring->append("	severity(")->
				append((int32_t)CS_SEVERITY(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	layer(")->
				append((int32_t)CS_LAYER(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	origin(")->
				append((int32_t)CS_ORIGIN(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	number(")->
				append((int32_t)CS_NUMBER(msgp->msgnumber))->
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
	} else
	// for a net-libraryoperation terminated due to disconnect,
	// set deadconnection to 1
	if (CS_SEVERITY(msgp->msgnumber)==5 &&
		CS_LAYER(msgp->msgnumber)==5 &&
		CS_ORIGIN(msgp->msgnumber)==3 &&
		CS_NUMBER(msgp->msgnumber)==6) {
		deadconnection=true;
	}
	// FIXME: freetds connection has another case, do we need it?

	return CS_SUCCEED;
}

CS_RETCODE sybaseconnection::serverMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp) {

	// This is a special case, for some reason, "use db" queries
	// throw a warning, ignore them.
	if ((CS_NUMBER(msgp->msgnumber)==5701 &&
			CS_SEVERITY(msgp->msgnumber)==10) ||
		(CS_NUMBER(msgp->msgnumber)==69 &&
			CS_SEVERITY(msgp->msgnumber)==22)) {
		return CS_SUCCEED;
	}

	if (errorstring) {
		return CS_SUCCEED;
	}
	errorstring=new stringbuffer();

	errorstring->append("Server message:\n");
	errorstring->append("	severity(")->
				append((int32_t)CS_SEVERITY(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	number(")->
				append((int32_t)CS_NUMBER(msgp->msgnumber))->
				append(")\n");
	errorstring->append("	state(")->
				append((int32_t)msgp->state)->append(")\n");
	errorstring->append("	line(")->
				append((int32_t)msgp->line)->append(")\n");
	errorstring->append("Server Name:\n")->
				append(msgp->svrname)->append("\n");
	errorstring->append("Procedure Name:\n")->
				append(msgp->proc)->append("\n");
	errorstring->append("Error:	")->
				append(msgp->text)->append("\n");

	//printf("serverMessageCallback:\n%s\n",errorstring->getString());

	return CS_SUCCEED;
}


void sybaseconnection::dropTempTable(sqlrcursor_svr *cursor,
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

bool sybaseconnection::commit() {
	cleanUpAllCursorData(true,true);
	return sqlrconnection_svr::commit();
}

bool sybaseconnection::rollback() {
	cleanUpAllCursorData(true,true);
	return sqlrconnection_svr::rollback();
}
