// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <rudiments/environment.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>

#include <config.h>
#include <datatypes.h>

extern "C" {
	#include <ctpublic.h>
}

#include <stdio.h>
#include <stdlib.h>

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096

class sybaseconnection : public sqlrconnection_svr {
	friend class sybasecursor;
	public:
			sybaseconnection(sqlrcontroller_svr *cont);
			~sybaseconnection();
	private:
		void		handleConnectString();
		bool		logIn(bool printerrors);
		void		logInError(const char *error, uint16_t stage);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		const char	*identify();
		const char	*dbVersion();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		const char	*getLastInsertIdQuery();
		const char	*bindFormat();
		const char	*beginTransactionQuery();
		char		bindVariablePrefix();
		const char	*tempTableDropPrefix();
		bool		commit();
		bool		rollback();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);

		CS_CONTEXT	*context;
		CS_LOCALE	*locale;
		CS_CONNECTION	*dbconn;

		const char	*sybase;
		const char	*lang;
		const char	*server;
		const char	*db;
		const char	*charset;
		const char	*language;
		const char	*hostname;
		const char	*packetsize;

		bool		dbused;

		char		*dbversion;

		static	stringbuffer	errorstring;
		static	int64_t		errorcode;
		static	bool		liveconnection;

		static	CS_RETCODE	csMessageCallback(CS_CONTEXT *ctxt,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	clientMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	serverMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp);
};

struct datebind {
        int16_t         *year;
        int16_t         *month;
        int16_t         *day;
        int16_t         *hour;
        int16_t         *minute;
        int16_t         *second;
        int32_t         *microsecond;
        const char      **tz;
};

class sybasecursor : public sqlrcursor_svr {
	friend class sybaseconnection;
	private:
				sybasecursor(sqlrconnection_svr *conn);
				~sybasecursor();
		bool		open(uint16_t id);
		bool		close();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		inputBind(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBind(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		bool		inputBind(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						char *value, 
						uint16_t valuesize, 
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		executeQuery(const char *query,
						uint32_t length);
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		uint16_t	getColumnIsNullable(uint32_t col);
		uint16_t	getColumnIsPartOfKey(uint32_t col);
		uint16_t	getColumnIsUnsigned(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
		uint16_t	getColumnIsAutoIncrement(uint32_t col);
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		nextRow();
		void		cleanUpData();
		void		discardResults();
		void		discardCursor();

		char		*cursorname;
		size_t		cursornamelength;

		void		checkRePrepare();

		CS_COMMAND	*languagecmd;
		CS_COMMAND	*cursorcmd;
		CS_COMMAND	*cmd;
		CS_INT		results;
		CS_INT		resultstype;
		CS_INT		ncols;
		CS_INT		affectedrows;

		CS_INT		rowsread;
		CS_INT		row;
		CS_INT		maxrow;
		CS_INT		totalrows;

		CS_DATAFMT	*parameter;
		uint16_t	paramindex;
		CS_INT		*outbindtype;
		char		**outbindstrings;
		uint16_t	*outbindstringlengths;
		int64_t		**outbindints;
		double		**outbinddoubles;
		datebind	*outbinddates;
		uint16_t	outbindindex;

		CS_DATAFMT	column[MAX_SELECT_LIST_SIZE];
		char		data[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE]
					[MAX_ITEM_BUFFER_SIZE];
		CS_INT		datalength[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE];
		CS_SMALLINT	nullindicator[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE];

		const char	*query;
		uint32_t	length;
		bool		prepared;
		bool		clean;

		regularexpression	cursorquery;
		regularexpression	rpcquery;

		bool			isrpcquery;

		sybaseconnection	*sybaseconn;
};


stringbuffer	sybaseconnection::errorstring;
int64_t		sybaseconnection::errorcode;
bool		sybaseconnection::liveconnection;


sybaseconnection::sybaseconnection(sqlrcontroller_svr *cont) :
					sqlrconnection_svr(cont) {
	dbused=false;
	dbversion=NULL;
}

sybaseconnection::~sybaseconnection() {
	delete[] dbversion;
}

void sybaseconnection::handleConnectString() {
	sybase=cont->connectStringValue("sybase");
	lang=cont->connectStringValue("lang");
	cont->setUser(cont->connectStringValue("user"));
	cont->setPassword(cont->connectStringValue("password"));
	server=cont->connectStringValue("server");
	db=cont->connectStringValue("db");
	charset=cont->connectStringValue("charset");
	language=cont->connectStringValue("language");
	hostname=cont->connectStringValue("hostname");
	packetsize=cont->connectStringValue("packetsize");
	cont->fakeinputbinds=
		!charstring::compare(
				cont->connectStringValue("fakebinds"),
				"yes");
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
	const char	*user=cont->getUser();
	if (ct_con_props(dbconn,CS_SET,CS_USERNAME,
			(CS_VOID *)((user && user[0])?user:""),
			(CS_INT)charstring::length(user),
			(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the user",5);
		return false;
	}


	// set the password to use
	const char	*password=cont->getPassword();
	if (ct_con_props(dbconn,CS_SET,CS_PASSWORD,
			(CS_VOID *)((password && password[0])?password:""),
			(CS_INT)charstring::length(password),
			(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the password",5);
		return false;
	}

	// set application name
	if (ct_con_props(dbconn,CS_SET,CS_APPNAME,(CS_VOID *)"sqlrelay",8,
			(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the application name",5);
		return false;
	}

	// set hostname
	if (hostname && hostname[0] &&
		ct_con_props(dbconn,CS_SET,CS_HOSTNAME,(CS_VOID *)hostname,
				(CS_INT)charstring::length(hostname),
				(CS_INT *)NULL)!=CS_SUCCEED) {
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
			(CS_CHAR *)language,
			(CS_INT)charstring::length(language),
			(CS_INT *)NULL)!=CS_SUCCEED) {
		logInError("failed to set the language",6);
		return false;
	}

	// set charset
	if (charset && charset[0] &&
		cs_locale(context,CS_SET,locale,CS_SYB_CHARSET,
			(CS_CHAR *)charset,
			(CS_INT)charstring::length(charset),
			(CS_INT *)NULL)!=CS_SUCCEED) {
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

	if (errorstring.getStringLength()) {
		fprintf(stderr,"%s\n",errorstring.getString());
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

const char *sybaseconnection::getDatabaseListQuery(bool wild) {
	return "select '' as db";
}

const char *sybaseconnection::getTableListQuery(bool wild) {
	return (wild)?
		"select "
		"	name "
		"from "
		"	sysobjects "
		"where "
		"	loginame is not NULL "
		"	and "
		"	type in ('U','V') "
		"	and "
		"	name like '%s' "
		"order by "
		"	name":

		"select "
		"	name "
		"from "
		"	sysobjects "
		"where "
		"	loginame is not NULL "
		"	and "
		"	type in ('U','V') "
		"order by "
		"	name";
}

const char *sybaseconnection::getColumnListQuery(bool wild) {
	return (wild)?
		"select "
		"	syscolumns.name, "
		"	systypes.name as type, "
		"	syscolumns.length, "
		"	syscolumns.prec, "
		"	syscolumns.scale, "
		"	(syscolumns.status&8)/8 as nullable, "
		"	'' as primarykey, "
		"	'' column_default, "
		"	'' as extra "
		"from "
		"	sysobjects, "
		"	syscolumns, "
		"	systypes "
		"where "
		"	sysobjects.type in ('S','U','V') "
		"	and "
		"	sysobjects.name='%s' "
		"	and "
		"	syscolumns.id=sysobjects.id "
		"	and "
		"	syscolumns.name like '%s' "
		"	and "
		"	systypes.usertype=syscolumns.usertype "
		"order by "
		"	syscolumns.colid":

		"select "
		"	syscolumns.name, "
		"	systypes.name as type, "
		"	syscolumns.length, "
		"	syscolumns.prec, "
		"	syscolumns.scale, "
		"	(syscolumns.status&8)/8 as nullable, "
		"	'' as primarykey, "
		"	'' column_default, "
		"	'' as extra "
		"from "
		"	sysobjects, "
		"	syscolumns, "
		"	systypes "
		"where "
		"	sysobjects.type in ('S','U','V') "
		"	and "
		"	sysobjects.name='%s' "
		"	and "
		"	syscolumns.id=sysobjects.id "
		"	and "
		"	systypes.usertype=syscolumns.usertype "
		"order by "
		"	syscolumns.colid";
}

const char *sybaseconnection::selectDatabaseQuery() {
	return "use %s";
}

const char *sybaseconnection::getCurrentDatabaseQuery() {
	return "select db_name()";
}

const char *sybaseconnection::getLastInsertIdQuery() {
	return "select @@identity";
}

const char *sybaseconnection::bindFormat() {
	return "@*";
}

const char *sybaseconnection::beginTransactionQuery() {
	return "BEGIN TRANSACTION";
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
	cursornamelength=0;

	parameter=new CS_DATAFMT[conn->cont->maxbindcount];
	outbindtype=new CS_INT[conn->cont->maxbindcount];
	outbindstrings=new char *[conn->cont->maxbindcount];
	outbindstringlengths=new uint16_t[conn->cont->maxbindcount];
	outbindints=new int64_t *[conn->cont->maxbindcount];
	outbinddoubles=new double *[conn->cont->maxbindcount];
	outbinddates=new datebind[conn->cont->maxbindcount];

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
	close();
	delete[] cursorname;
	delete[] parameter;
	delete[] outbindtype;
	delete[] outbindstrings;
	delete[] outbindstringlengths;
	delete[] outbindints;
	delete[] outbinddoubles;
	delete[] outbinddates;
}

bool sybasecursor::open(uint16_t id) {

	clean=true;

	cursornamelength=charstring::integerLength(id);
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
		if (!(prepareQuery(query,len) && executeQuery(query,len))) {
			char		err[2048];
			uint32_t	errlen;
			int64_t		errn;
			bool		live;
			errorMessage(err,sizeof(err),&errlen,&errn,&live);
			fprintf(stderr,"%s\n",err);
			retval=false;
		} else {
			sybaseconn->dbused=true;
		}
		cleanUpData();
	}

	if (!sybaseconn->dbversion) {
		const char	*query="sp_version installmaster";
		int32_t		len=charstring::length(query);
		if (!(prepareQuery(query,len) &&
				executeQuery(query,len) &&
				fetchRow())) {
			sybaseconn->dbversion=
				charstring::duplicate("unknown");
		} else {
			const char	*space=
				charstring::findFirst(data[1][0],' ');
			sybaseconn->dbversion=
				charstring::duplicate(data[1][0],
							space-data[1][0]);
		}
		cleanUpData();
	}
	return retval;
}

bool sybasecursor::close() {
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
		// (don't use CS_NULLTERM for the 4th parameter, it randomly
		// causes weird things to happen)
		cmd=cursorcmd;
		if (ct_cursor(cursorcmd,CS_CURSOR_DECLARE,
					(CS_CHAR *)cursorname,
					(CS_INT)cursornamelength,
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

bool sybasecursor::inputBind(const char *variable,
				uint16_t variablesize,
				const char *value,
				uint32_t valuesize,
				int16_t *isnull) {

	checkRePrepare();

	rawbuffer::zero(&parameter[paramindex],sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]='\0';
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

bool sybasecursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value) {

	checkRePrepare();

	rawbuffer::zero(&parameter[paramindex],sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]='\0';
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

bool sybasecursor::inputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {

	checkRePrepare();

	rawbuffer::zero(&parameter[paramindex],sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]='\0';
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

static const char *monthname[]={
	"Jan","Feb","Mar","Apr","May","Jun",
	"Jul","Aug","Sep","Oct","Nov","Dec",
	NULL
};

bool sybasecursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t year,
				int16_t month,
				int16_t day,
				int16_t hour,
				int16_t minute,
				int16_t second,
				int32_t microsecond,
				const char *tz,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {

	checkRePrepare();

	// Sybase requires this format: "Jan 2 2012 4:5:3:000PM"
	if (month<1) {
		month=1;
	}
	if (month>12) {
		month=12;
	}
	const char	*ampm="AM";
	if (hour==0) {
		hour=12;
	} else if (hour==12) {
		ampm="PM";
	} else if (hour>12) {
		hour=hour-12;
		ampm="PM";
	}
	charstring::copy(buffer,monthname[month-1]);
	charstring::append(buffer," ");
	charstring::append(buffer,(int64_t)day);
	charstring::append(buffer," ");
	charstring::append(buffer,(int64_t)year);
	charstring::append(buffer," ");
	charstring::append(buffer,(int64_t)hour);
	charstring::append(buffer,":");
	charstring::append(buffer,(int64_t)minute);
	charstring::append(buffer,":");
	charstring::append(buffer,(int64_t)second);
	charstring::append(buffer,":");
	charstring::append(buffer,(int64_t)microsecond);
	charstring::append(buffer,ampm);
	return inputBind(variable,variablesize,
				buffer,charstring::length(buffer),isnull);
}

bool sybasecursor::outputBind(const char *variable, 
				uint16_t variablesize,
				char *value, 
				uint16_t valuesize, 
				int16_t *isnull) {

	checkRePrepare();

	outbindtype[outbindindex]=CS_CHAR_TYPE;
	outbindstrings[outbindindex]=value;
	outbindstringlengths[outbindindex]=valuesize;
	outbindindex++;

	rawbuffer::zero(&parameter[paramindex],sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]='\0';
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

bool sybasecursor::outputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {

	checkRePrepare();

	outbindtype[outbindindex]=CS_INT_TYPE;
	outbindints[outbindindex]=value;
	outbindindex++;

	rawbuffer::zero(&parameter[paramindex],sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]='\0';
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

bool sybasecursor::outputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t *precision,
				uint32_t *scale,
				int16_t *isnull) {

	checkRePrepare();

	outbindtype[outbindindex]=CS_FLOAT_TYPE;
	outbinddoubles[outbindindex]=value;
	outbindindex++;

	rawbuffer::zero(&parameter[paramindex],sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]='\0';
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

bool sybasecursor::outputBind(const char *variable,
				uint16_t variablesize,
				int16_t *year,
				int16_t *month,
				int16_t *day,
				int16_t *hour,
				int16_t *minute,
				int16_t *second,
				int32_t *microsecond,
				const char **tz,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {
	checkRePrepare();

	outbindtype[outbindindex]=CS_DATETIME_TYPE;
	outbinddates[outbindindex].year=year;
	outbinddates[outbindindex].month=month;
	outbinddates[outbindindex].day=day;
	outbinddates[outbindindex].hour=hour;
	outbinddates[outbindindex].minute=minute;
	outbinddates[outbindindex].second=second;
	outbinddates[outbindindex].microsecond=microsecond;
	outbinddates[outbindindex].tz=tz;
	outbindindex++;

	rawbuffer::zero(&parameter[paramindex],sizeof(parameter[paramindex]));
	if (charstring::isInteger(variable+1,variablesize-1)) {
		parameter[paramindex].name[0]='\0';
		parameter[paramindex].namelen=0;
	} else {
		charstring::copy(parameter[paramindex].name,variable);
		parameter[paramindex].namelen=variablesize;
	}
	parameter[paramindex].datatype=CS_DATETIME_TYPE;
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

bool sybasecursor::executeQuery(const char *query, uint32_t length) {

	// clear out any errors
	sybaseconn->errorstring.clear();
	sybaseconn->errorcode=0;
	sybaseconn->liveconnection=true;

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
		cleanUpData();
		return false;
	}

	for (;;) {

		results=ct_results(cmd,&resultstype);

		if (results==CS_FAIL || resultstype==CS_CMD_FAIL) {
			cleanUpData();
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
			sybaseconn->liveconnection=false;
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
			// other values that might have been set also...
			//
			// however, if we're getting the output bind variables
			// of a stored procedure that returns dates, then use
			// the datetime type instead...
			if (isrpcquery && outbindtype[i]==CS_DATETIME_TYPE) {
				column[i].datatype=CS_DATETIME_TYPE;
				column[i].format=CS_FMT_UNUSED;
				column[i].maxlength=sizeof(CS_DATETIME);
			} else {
				column[i].datatype=CS_CHAR_TYPE;
				column[i].format=CS_FMT_NULLTERM;
				column[i].maxlength=MAX_ITEM_BUFFER_SIZE;
			}
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

			// describe the columns
			if (conn->cont->sendColumnInfo()) {
				if (ct_describe(cmd,i+1,&column[i])!=
								CS_SUCCEED) {
					break;
				}
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
			} else if (outbindtype[i]==CS_DATETIME_TYPE) {

				// convert to a CS_DATEREC
				CS_DATEREC	dr;
				rawbuffer::zero(&dr,sizeof(CS_DATEREC));
				cs_dt_crack(sybaseconn->context,
						CS_DATETIME_TYPE,
						(CS_VOID *)data[i][0],&dr);

				datebind	*db=&outbinddates[i];
				*(db->year)=dr.dateyear;
				*(db->month)=dr.datemonth+1;
				*(db->day)=dr.datedmonth;
				*(db->hour)=dr.datehour;
				*(db->minute)=dr.dateminute;
				*(db->second)=dr.datesecond;
				*(db->microsecond)=dr.datesecfrac;
				*(db->tz)=NULL;
			}
		}

		discardResults();
		ncols=0;
	}

	// For non-rpc language commands, we need to discard the result sets
	// here or subsequent select queries in other cursors will fail until
	// this cursor is cleaned up.  They will fail with:
	// Client Library error:
	// severity(1)
	// layer(1)
	// origin(1)
	// number(49)
	// Error:	ct_send(): user api layer: external error: This routine cannot be called because another command structure has results pending.
	if (cmd==languagecmd && !isrpcquery) {
		discardResults();
	}

	// return success only if no error was generated
	if (sybaseconn->errorstring.getStringLength()) {
		return false;
	}
	return true;
}

uint64_t sybasecursor::affectedRows() {
	return affectedrows;
}

uint32_t sybasecursor::colCount() {
	return ncols;
}

const char *sybasecursor::getColumnName(uint32_t col) {
	return column[col].name;
}

uint16_t sybasecursor::getColumnType(uint32_t col) {
	switch (column[col].datatype) {
		case CS_CHAR_TYPE:
			return CHAR_DATATYPE;
		case CS_INT_TYPE:
			return INT_DATATYPE;
		case CS_SMALLINT_TYPE:
			return SMALLINT_DATATYPE;
		case CS_TINYINT_TYPE:
			return TINYINT_DATATYPE;
		case CS_MONEY_TYPE:
			return MONEY_DATATYPE;
		case CS_DATETIME_TYPE:
			return DATETIME_DATATYPE;
		case CS_NUMERIC_TYPE:
			return NUMERIC_DATATYPE;
		case CS_DECIMAL_TYPE:
			return DECIMAL_DATATYPE;
		case CS_DATETIME4_TYPE:
			return SMALLDATETIME_DATATYPE;
		case CS_MONEY4_TYPE:
			return SMALLMONEY_DATATYPE;
		case CS_IMAGE_TYPE:
			return IMAGE_DATATYPE;
		case CS_BINARY_TYPE:
			return BINARY_DATATYPE;
		case CS_BIT_TYPE:
			return BIT_DATATYPE;
		case CS_REAL_TYPE:
			return REAL_DATATYPE;
		case CS_FLOAT_TYPE:
			return FLOAT_DATATYPE;
		case CS_TEXT_TYPE:
			return TEXT_DATATYPE;
		case CS_VARCHAR_TYPE:
			return VARCHAR_DATATYPE;
		case CS_VARBINARY_TYPE:
			return VARBINARY_DATATYPE;
		case CS_LONGCHAR_TYPE:
			return LONGCHAR_DATATYPE;
		case CS_LONGBINARY_TYPE:
			return LONGBINARY_DATATYPE;
		case CS_LONG_TYPE:
			return LONG_DATATYPE;
		case CS_ILLEGAL_TYPE:
			return ILLEGAL_DATATYPE;
		case CS_SENSITIVITY_TYPE:
			return SENSITIVITY_DATATYPE;
		case CS_BOUNDARY_TYPE:
			return BOUNDARY_DATATYPE;
		case CS_VOID_TYPE:
			return VOID_DATATYPE;
		case CS_USHORT_TYPE:
			return USHORT_DATATYPE;
		default:
			return UNKNOWN_DATATYPE;
	}
}

uint32_t sybasecursor::getColumnLength(uint32_t col) {
	// limit the column size
	if (column[col].maxlength>MAX_ITEM_BUFFER_SIZE) {
		column[col].maxlength=MAX_ITEM_BUFFER_SIZE;
	}
	return column[col].maxlength;
}

uint32_t sybasecursor::getColumnPrecision(uint32_t col) {
	return column[col].precision;
}

uint32_t sybasecursor::getColumnScale(uint32_t col) {
	return column[col].scale;
}

uint16_t sybasecursor::getColumnIsNullable(uint32_t col) {
	return (column[col].status&CS_CANBENULL);
}

uint16_t sybasecursor::getColumnIsPartOfKey(uint32_t col) {
	return (column[col].status&(CS_KEY|CS_VERSION_KEY));
}

uint16_t sybasecursor::getColumnIsUnsigned(uint32_t col) {
	return (getColumnType(col)==USHORT_DATATYPE);
}

uint16_t sybasecursor::getColumnIsBinary(uint32_t col) {
	return (getColumnType(col)==IMAGE_DATATYPE);
}

uint16_t sybasecursor::getColumnIsAutoIncrement(uint32_t col) {
	return (column[col].status&CS_IDENTITY);
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

void sybasecursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {

	// handle normal datatypes
	if (nullindicator[col][row]>-1 && datalength[col][row]) {
		*field=data[col][row];
		*fieldlength=datalength[col][row]-1;
		return;
	}

	// handle NULLs
	*null=true;
}

void sybasecursor::nextRow() {
	row++;
}

void sybasecursor::cleanUpData() {

	if (clean) {
		return;
	}

	discardResults();
	discardCursor();

	clean=true;
}

void sybasecursor::discardResults() {

	// if there are any unprocessed result sets, process them
	if (results==CS_SUCCEED) {
		do {
			if (ct_cancel(NULL,cmd,CS_CANCEL_CURRENT)==CS_FAIL) {
				sybaseconn->liveconnection=false;
				// FIXME: call ct_close(CS_FORCE_CLOSE)
				// maybe return false
			}
			results=ct_results(cmd,&resultstype);
		} while (results==CS_SUCCEED);
	}

	if (results==CS_FAIL) {
		if (ct_cancel(NULL,cmd,CS_CANCEL_ALL)==CS_FAIL) {
			sybaseconn->liveconnection=false;
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
	if (errorstring.getStringLength()) {
		return CS_SUCCEED;
	}

	errorcode=msgp->msgnumber;

	errorstring.append("Client Library error:\n");
	errorstring.append("	severity(")->
				append((int32_t)CS_SEVERITY(msgp->msgnumber))->
				append(")\n");
	errorstring.append("	layer(")->
				append((int32_t)CS_LAYER(msgp->msgnumber))->
				append(")\n");
	errorstring.append("	origin(")->
				append((int32_t)CS_ORIGIN(msgp->msgnumber))->
				append(")\n");
	errorstring.append("	number(")->
				append((int32_t)CS_NUMBER(msgp->msgnumber))->
				append(")\n");
	errorstring.append("Error:	")->append(msgp->msgstring)->
				append("\n");

	if (msgp->osstringlen>0) {
		errorstring.append("Operating System Error:\n");
		errorstring.append("\n	")->append(msgp->osstring)->
							append("\n");
	}

	// for a timeout message,
	// set liveconnection to false
	if (CS_SEVERITY(msgp->msgnumber)==CS_SV_RETRY_FAIL &&
		CS_LAYER(msgp->msgnumber)==63 &&
		CS_ORIGIN(msgp->msgnumber)==63 &&
		CS_NUMBER(msgp->msgnumber)==63) {
		liveconnection=false;
	} else
	// for a net-libraryoperation terminated due to disconnect,
	// set liveconnection to false
	if (CS_SEVERITY(msgp->msgnumber)==5 &&
		CS_LAYER(msgp->msgnumber)==5 &&
		CS_ORIGIN(msgp->msgnumber)==3 &&
		CS_NUMBER(msgp->msgnumber)==6) {
		liveconnection=false;
	}
	// FIXME: freetds connection has another case, do we need it?

	return CS_SUCCEED;
}

CS_RETCODE sybaseconnection::clientMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp) {
	if (errorstring.getStringLength()) {
		return CS_SUCCEED;
	}

	errorcode=msgp->msgnumber;

	errorstring.append("Client Library error:\n");
	errorstring.append("	severity(")->
				append((int32_t)CS_SEVERITY(msgp->msgnumber))->
				append(")\n");
	errorstring.append("	layer(")->
				append((int32_t)CS_LAYER(msgp->msgnumber))->
				append(")\n");
	errorstring.append("	origin(")->
				append((int32_t)CS_ORIGIN(msgp->msgnumber))->
				append(")\n");
	errorstring.append("	number(")->
				append((int32_t)CS_NUMBER(msgp->msgnumber))->
				append(")\n");
	errorstring.append("Error:	")->append(msgp->msgstring)->
				append("\n");

	if (msgp->osstringlen>0) {
		errorstring.append("Operating System Error:\n");
		errorstring.append("\n	")->append(msgp->osstring)->
							append("\n");
	}

	// for a timeout message,
	// set liveconnection to false
	if (CS_SEVERITY(msgp->msgnumber)==CS_SV_RETRY_FAIL &&
		CS_NUMBER(msgp->msgnumber)==63 &&
		CS_ORIGIN(msgp->msgnumber)==63 &&
		CS_LAYER(msgp->msgnumber)==63) {
		liveconnection=false;
	} else
	// for a net-libraryoperation terminated due to disconnect,
	// set liveconnection to false
	if (CS_SEVERITY(msgp->msgnumber)==5 &&
		CS_LAYER(msgp->msgnumber)==5 &&
		CS_ORIGIN(msgp->msgnumber)==3 &&
		CS_NUMBER(msgp->msgnumber)==6) {
		liveconnection=false;
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

	if (errorstring.getStringLength()) {
		return CS_SUCCEED;
	}

	errorcode=msgp->msgnumber;

	errorstring.append("Server message:\n");
	errorstring.append("	severity(")->
				append((int32_t)CS_SEVERITY(msgp->msgnumber))->
				append(")\n");
	errorstring.append("	number(")->
				append((int32_t)CS_NUMBER(msgp->msgnumber))->
				append(")\n");
	errorstring.append("	state(")->
				append((int32_t)msgp->state)->append(")\n");
	errorstring.append("	line(")->
				append((int32_t)msgp->line)->append(")\n");
	errorstring.append("Server Name:\n")->
				append(msgp->svrname)->append("\n");
	errorstring.append("Procedure Name:\n")->
				append(msgp->proc)->append("\n");
	errorstring.append("Error:	")->
				append(msgp->text)->append("\n");

	return CS_SUCCEED;
}

const char *sybaseconnection::tempTableDropPrefix() {
	return "#";
}

bool sybaseconnection::commit() {
	cont->cleanUpAllCursorData();
	return sqlrconnection_svr::commit();
}

bool sybaseconnection::rollback() {
	cont->cleanUpAllCursorData();
	return sqlrconnection_svr::rollback();
}

void sybaseconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	*errorlength=this->errorstring.getStringLength();
	charstring::safeCopy(errorbuffer,errorbufferlength,
				this->errorstring.getString(),*errorlength);
	*liveconnection=this->liveconnection;
	*errorcode=this->errorcode;
}

extern "C" {
	sqlrconnection_svr *new_sybaseconnection(sqlrcontroller_svr *cont) {
		return new sybaseconnection(cont);
	}
}
