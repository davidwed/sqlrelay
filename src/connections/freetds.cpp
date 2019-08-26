// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/environment.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/charstring.h>
#include <rudiments/character.h>
#include <rudiments/bytestring.h>
#include <rudiments/stdio.h>
#include <rudiments/snooze.h>

#include <datatypes.h>
#include <defines.h>
#include <config.h>

extern "C" {
	#include <ctpublic.h>
}

#ifdef HAVE_FREETDS_H
	#include <tdsver.h>
#endif

#ifndef HAVE_FREETDS_FUNCTION_DEFINITIONS
// in freetds prior to 0.61, the cs_ functions are not defined in any header
// file. C allows that...  C++ does not, so here they are
extern "C" {

extern	CS_INT	cs_ctx_alloc(CS_INT,CS_CONTEXT **);
extern	CS_INT	ct_init(CS_CONTEXT *,CS_INT);
extern	CS_INT	ct_callback(CS_CONTEXT *,CS_CONNECTION *,
					CS_INT,CS_INT,CS_VOID *);
extern	CS_INT	cs_config(CS_CONTEXT *,CS_INT,CS_INT,CS_VOID *,CS_INT,CS_INT *);
extern	CS_INT	ct_con_alloc(CS_CONTEXT *,CS_CONNECTION **);
extern	CS_INT	ct_con_props(CS_CONNECTION *,CS_INT,CS_INT,CS_VOID *,CS_INT,CS_INT *);
extern	CS_INT	cs_loc_alloc(CS_CONTEXT *,CS_LOCALE **);
extern	CS_INT	cs_locale(CS_CONTEXT *,CS_INT,CS_LOCALE *,CS_INT,CS_CHAR *,CS_INT,CS_INT *);
extern	CS_INT	ct_connect(CS_CONNECTION *,CS_CHAR *,CS_INT);
extern	CS_INT	ct_close(CS_CONNECTION *,CS_INT);
extern	CS_INT	ct_con_drop(CS_CONNECTION *);
extern	CS_INT	ct_exit(CS_CONTEXT *,CS_INT);
extern	CS_INT	cs_ctx_drop(CS_CONTEXT *);
extern	CS_INT	ct_cmd_alloc(CS_CONNECTION *,CS_COMMAND **);
extern	CS_INT	ct_command(CS_COMMAND *,CS_INT,CS_CHAR *,CS_INT,CS_INT);
extern	CS_INT	ct_send(CS_COMMAND *);
extern	CS_INT	ct_results(CS_COMMAND *,CS_INT *);
extern	CS_INT	ct_res_info(CS_COMMAND *,CS_INT,CS_VOID *,CS_INT,CS_INT *);
extern	CS_INT	ct_describe(CS_COMMAND *,CS_INT,CS_DATAFMT *);
extern	CS_INT	ct_bind(CS_COMMAND *,CS_INT,CS_DATAFMT *,CS_VOID *,CS_INT *,CS_SMALLINT *);
extern	CS_INT	ct_fetch(CS_COMMAND *,CS_INT,CS_INT,CS_INT,CS_INT *);
extern	CS_INT	ct_cmd_drop(CS_COMMAND *);
extern	CS_INT	cs_convert(CS_CONTEXT *,CS_DATAFMT *,CS_VOID *,CS_DATAFMT *,CS_VOID *,CS_INT *);
extern	CS_INT	cs_loc_drop(CS_CONTEXT *,CS_LOCALE *);
extern	CS_INT	ct_cancel(CS_CONNECTION *,CS_COMMAND *,CS_INT);
extern	CS_INT	ct_dynamic(CS_COMMAND *,CS_INT,CS_CHAR *,CS_INT,CS_CHAR *,CS_INT);

}
#endif

// this is here in case freetds ever supports cursors
//#define FREETDS_SUPPORTS_CURSORS

// some versions of freetds don't define this
#ifndef CS_UNSUPPORTED
	#define CS_UNSUPPORTED -10
#endif

struct datebind {
        int16_t         *year;
        int16_t         *month;
        int16_t         *day;
        int16_t         *hour;
        int16_t         *minute;
        int16_t         *second;
        int32_t         *microsecond;
        const char      **tz;
	bool		*isnegative;
};

class freetdsconnection;

class SQLRSERVER_DLLSPEC freetdscursor : public sqlrservercursor {
	friend class freetdsconnection;
	private:
				freetdscursor(sqlrserverconnection *conn,
								uint16_t id);
				~freetdscursor();
		void		allocateResultSetBuffers(int32_t columncount);
		void		deallocateResultSetBuffers();
		bool		open();
		bool		close();
		bool		prepareQuery(const char *query,
							uint32_t length);
		bool		supportsNativeBinds(const char *query,
							uint32_t length);
		void		encodeBlob(stringbuffer *buffer,
							const char *data,
							uint32_t datasize);
		#ifdef FREETDS_SUPPORTS_CURSORS
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
						bool isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						char *value, 
						uint32_t valuesize, 
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
						bool *isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		#endif
		bool		executeQuery(const char *query,
						uint32_t length);
		bool		knowsAffectedRows();
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
		bool		ignoreDateDdMmParameter(uint32_t col,
						const char *data,
						uint32_t size);
		bool		noRowsToReturn();
		bool		skipRow(bool *error);
		bool		fetchRow(bool *error);
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		nextRow();
		void		closeResultSet();
		void		discardResults();
		void		discardCursor();

		char		*cursorname;
		size_t		cursornamelength;

		void		checkRePrepare();

		uint32_t	majorversion;
		uint32_t	minorversion;
		uint32_t	patchlevel;

		CS_COMMAND	*languagecmd;
		CS_COMMAND	*cursorcmd;
		CS_COMMAND	*cmd;
		CS_INT		results;
		CS_INT		resultstype;
		CS_INT		ncols;
		bool		knowsaffectedrows;
		CS_INT		affectedrows;

		CS_INT		rowsread;
		CS_INT		row;
		CS_INT		maxrow;
		CS_INT		totalrows;

		CS_DATAFMT	*parameter;
		uint16_t	paramindex;
		CS_INT		*outbindtype;
		char		**outbindstrings;
		uint32_t	*outbindstringlengths;
		int64_t		**outbindints;
		double		**outbinddoubles;
		datebind	*outbinddates;
		uint16_t	outbindindex;

		int32_t		columncount;
		CS_DATAFMT	templatecolumn;
		CS_DATAFMT	*column;
		char		**data;
		CS_INT		**datalength;
		CS_SMALLINT	**nullindicator;

		char		*query;
		uint32_t	length;
		bool		prepared;
		bool		clean;

		freetdsconnection	*freetdsconn;
};


class SQLRSERVER_DLLSPEC freetdsconnection : public sqlrserverconnection {
	friend class freetdscursor;
	public:
			freetdsconnection(sqlrservercontroller *cont);
			~freetdsconnection();
	private:
		void	handleConnectString();
		bool	logIn(const char **error, const char **warning);
		const char	*logInError(const char *error, uint16_t stage);
		sqlrservercursor	*newCursor(uint16_t id);
		void	deleteCursor(sqlrservercursor *curs);
		void	logOut();
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostNameQuery();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild,
						uint16_t objecttypes);
		const char	*getColumnListQuery(
						const char *table, bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		const char	*getLastInsertIdQuery();
		const char	*getNoopQuery();
		const char	*bindFormat();
		const char	*beginTransactionQuery();
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
		const char	*freetds;
		const char	*lang;
		const char	*server;
		const char	*db;
		const char	*charset;
		const char	*language;
		const char	*hostname;
		const char	*packetsize;

		const char	*identity;

		bool		dbused;

		char		*dbversion;

		bool		sybasedb;

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

		stringbuffer	loginerror;
};

stringbuffer	freetdsconnection::errorstring;
int64_t		freetdsconnection::errorcode;
bool		freetdsconnection::liveconnection;

freetdsconnection::freetdsconnection(sqlrservercontroller *cont) :
						sqlrserverconnection(cont) {
	dbused=false;
	dbversion=NULL;
	sybasedb=true;

	identity=NULL;
}

freetdsconnection::~freetdsconnection() {
	delete[] dbversion;
}

void freetdsconnection::handleConnectString() {

	sqlrserverconnection::handleConnectString();

	sybase=cont->getConnectStringValue("sybase");
	freetds=cont->getConnectStringValue("freetds");
	lang=cont->getConnectStringValue("lang");
	server=cont->getConnectStringValue("server");
	db=cont->getConnectStringValue("db");
	charset=cont->getConnectStringValue("charset");
	language=cont->getConnectStringValue("language");
	hostname=cont->getConnectStringValue("hostname");
	packetsize=cont->getConnectStringValue("packetsize");

	// freetds doesn't currently support array fetches
	cont->setFetchAtOnce(1);

	if (cont->getMaxColumnCount()==1) {
		// if max column count is set to 1 then force it
		// to 2 so the db version detection doesn't crash
		cont->setMaxColumnCount(2);
	}

	identity=cont->getConnectStringValue("identity");
}

bool freetdsconnection::logIn(const char **error, const char **warning) {

	// set sybase
	if (!charstring::isNullOrEmpty(sybase) &&
			!environment::setValue("SYBASE",sybase)) {
		*error=logInError(
			"Failed to set SYBASE environment variable.",1);
		return false;
	}

	// set freetds
	if (!charstring::isNullOrEmpty(freetds)) {
		if (!environment::setValue("FREETDS",freetds)) {
			*error=logInError(
				"Failed to set FREETDS "
				"environment variable.",1);
			return false;
		}
		if (!environment::setValue("FREETDSCONF",freetds)) {
			*error=logInError(
				"Failed to set FREETDSCONF "
				"environment variable.",1);
			return false;
		}
	}

	// set lang
	if (!charstring::isNullOrEmpty(lang) &&
			!environment::setValue("LANG",lang)) {
		*error=logInError(
			"Failed to set LANG environment variable.",1);
		return false;
	}

	// set server
	if (!charstring::isNullOrEmpty(server) &&
			!environment::setValue("DSQUERY",server)) {
		*error=logInError(
			"Failed to set DSQUERY environment variable.",2);
		return false;
	}

	// allocate a context
	context=(CS_CONTEXT *)NULL;
	if (cs_ctx_alloc(CS_VERSION_100,&context)!=CS_SUCCEED) {
		*error=logInError(
			"Failed to allocate a context structure",2);
		return false;
	}
	// init the context
	if (ct_init(context,CS_VERSION_100)!=CS_SUCCEED) {
		*error=logInError(
			"Failed to initialize a context structure",3);
		return false;
	}


	// configure the error handling callbacks
	if (cs_config(context,CS_SET,CS_MESSAGE_CB,
		(CS_VOID *)freetdsconnection::csMessageCallback,CS_UNUSED,
			(CS_INT *)NULL)
			!=CS_SUCCEED) {
		*error=logInError(
			"Failed to set a cslib error message callback",4);
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_CLIENTMSG_CB,
		(CS_VOID *)freetdsconnection::clientMessageCallback)
			!=CS_SUCCEED) {
		*error=logInError(
			"Failed to set a client error message callback",4);
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_SERVERMSG_CB,
		(CS_VOID *)freetdsconnection::serverMessageCallback)
			!=CS_SUCCEED) {
		*error=logInError(
			"Failed to set a server error message callback",4);
		return false;
	}


	// allocate a connection
	if (ct_con_alloc(context,&dbconn)!=CS_SUCCEED) {
		*error=logInError(
			"Failed to allocate a connection structure",4);
		return false;
	}


	// set the user to use
	const char	*user=cont->getUser();
	if (ct_con_props(dbconn,CS_SET,CS_USERNAME,
		(CS_VOID *)((!charstring::isNullOrEmpty(user))?user:""),
		CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		*error=logInError("Failed to set the user",5);
		return false;
	}


	// set the password to use
	const char	*password=cont->getPassword();
	if (ct_con_props(dbconn,CS_SET,CS_PASSWORD,
		(CS_VOID *)((!charstring::isNullOrEmpty(password))?password:""),
		CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		*error=logInError("Failed to set the password",5);
		return false;
	}

	// set application name
	if (ct_con_props(dbconn,CS_SET,CS_APPNAME,
		(CS_VOID *)"sqlrelay",
		CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		*error=logInError("Failed to set the application name",5);
		return false;
	}

	// set hostname
	if (!charstring::isNullOrEmpty(hostname) &&
		ct_con_props(dbconn,CS_SET,CS_HOSTNAME,
			(CS_VOID *)hostname,
			CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		*error=logInError("Failed to set the hostname",5);
		return false;
	}

	// set packetsize
	uint16_t	ps=charstring::toInteger(packetsize);
	if (!charstring::isNullOrEmpty(packetsize) &&
		ct_con_props(dbconn,CS_SET,CS_PACKETSIZE,
			(CS_VOID *)&ps,sizeof(ps),
			(CS_INT *)NULL)!=CS_SUCCEED) {
		*error=logInError("Failed to set the packetsize",5);
		return false;
	}

	#ifdef CS_SEC_ENCRYPTION
	CS_INT	enc=CS_TRUE;
	if (ct_con_props(dbconn,CS_SET,CS_SEC_ENCRYPTION,
			(CS_VOID *)&enc,CS_UNUSED,
			(CS_INT *)NULL)!=CS_SUCCEED) {
		*error=logInError("Failed to enable password encryption",5);
		return false;
	}
	#endif

	// init locale
	locale=NULL;
	if (cs_loc_alloc(context,&locale)!=CS_SUCCEED) {
		*error=logInError("Failed to allocate a locale structure",5);
		return false;
	}
	if (cs_locale(context,CS_SET,locale,CS_LC_ALL,(CS_CHAR *)NULL,
			CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
		*error=logInError("Failed to initialize a locale structure",6);
		return false;
	}

	// set language
	if (!charstring::isNullOrEmpty(language) &&
		cs_locale(context,CS_SET,locale,CS_SYB_LANG,
			(CS_CHAR *)language,CS_NULLTERM,(CS_INT *)NULL)!=
				CS_SUCCEED) {
		*error=logInError("Failed to set the language",6);
		return false;
	}

	// set charset
	if (!charstring::isNullOrEmpty(charset) &&
		cs_locale(context,CS_SET,locale,CS_SYB_CHARSET,
			(CS_CHAR *)charset,CS_NULLTERM,(CS_INT *)NULL)!=
				CS_SUCCEED) {
		*error=logInError("Failed to set the charset",6);
		return false;
	}

	// set locale
	if (ct_con_props(dbconn,CS_SET,CS_LOC_PROP,(CS_VOID *)locale,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
		*error=logInError("Failed to set the locale",6);
		return false;
	}

	// connect to the database
	if (ct_connect(dbconn,(CS_CHAR *)NULL,(CS_INT)0)!=CS_SUCCEED) {
		*error=logInError("Failed to connect to the database",6);
		return false;
	}

	// If the password has expired then the db may allow the login
	// but every query will fail.  "ping" the db here to see if we get
	// that error or not.
	bool	retval=true;
	CS_COMMAND	*cmd;
	if (ct_cmd_alloc(dbconn,&cmd)!=CS_SUCCEED) {
		*error=logInError("Failed to allocate ping command",6);
		return false;
	}
	if (ct_command(cmd,CS_LANG_CMD,(CS_CHAR *)"select 1",8,
						CS_UNUSED)!=CS_SUCCEED) {
		*error=logInError("Failed to create ping command",6);
		return false;
	}
	if (ct_send(cmd)!=CS_SUCCEED) {
		*error=logInError("Failed to send ping command",6);
		return false;
	}
	CS_INT	resultstype;
	if (ct_results(cmd,&resultstype)==CS_FAIL || resultstype==CS_CMD_FAIL) {
		*error=logInError(NULL,6);
		retval=false;
	}
	ct_cancel(NULL,cmd,CS_CANCEL_ALL);
	ct_cmd_drop(cmd);

	return retval;
}

const char *freetdsconnection::logInError(const char *error, uint16_t stage) {

	loginerror.clear();
	if (error) {
		loginerror.append(error)->append(": ");
	}
	if (errorstring.getStringLength()) {
		loginerror.append(errorstring.getString());
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

	return loginerror.getString();
}

sqlrservercursor *freetdsconnection::newCursor(uint16_t id) {
	return (sqlrservercursor *)new freetdscursor(
					(sqlrserverconnection *)this,id);
}

void freetdsconnection::deleteCursor(sqlrservercursor *curs) {
	delete (freetdscursor *)curs;
}

void freetdsconnection::logOut() {

	cs_loc_drop(context,locale);
	ct_close(dbconn,CS_UNUSED);
	ct_con_drop(dbconn);
	ct_exit(context,CS_UNUSED);
	cs_ctx_drop(context);
}

const char *freetdsconnection::identify() {
	return (identity)?identity:"freetds";
}

const char *freetdsconnection::dbVersion() {
	return dbversion;
}

const char *freetdsconnection::dbHostNameQuery() {
	return "select asehostname()";
}

const char *freetdsconnection::getDatabaseListQuery(bool wild) {
	if (sybasedb) {
		return "select '' as db, NULL";
	} else {
		return "select "
			"	distinct catalog_name, "
			"	NULL "
			"from "
			"	information_schema.schemata "
			"order by "
			"	catalog_name";
	}
}

const char *freetdsconnection::getTableListQuery(bool wild,
						uint16_t objecttypes) {
	if (sybasedb) {
		return (wild)?
			"select "
			"	NULL as table_cat, "
			"	NULL as table_schem, "
			"	name as table_name, "
			"	'TABLE' as table_type, "
			"	NULL as remarks, "
			"	NULL as extra "
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
			"	NULL, "
			"	NULL, "
			"	name, "
			"	'TABLE', "
			"	NULL "
			"from "
			"	sysobjects "
			"where "
			"	loginame is not NULL "
			"	and "
			"	type in ('U','V') "
			"order by "
			"	name";
	} else {
		return sqlrserverconnection::getTableListQuery(
						wild,objecttypes);
	}
}

const char *freetdsconnection::getColumnListQuery(
					const char *table, bool wild) {
	if (sybasedb) {
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
			"	'' as extra, "
			"	NULL "
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
			"	'' as extra, "
			"	NULL "
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
	} else {
		if (table && table[0]=='#') {
			if (wild) {
				return
				"select "
				"	column_name, "
				"	data_type, "
				"	character_maximum_length, "
				"	numeric_precision, "
				"	numeric_scale, "
				"	is_nullable, "
				"	'' as primarykey, "
				"	column_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	tempdb.information_schema.columns "
				"where "
				"	table_name like '%s____%%' "
				"	and "
				"	column_name like '%s' "
				"order by "
				"	ordinal_position";
			} else {
				return
				"select "
				"	column_name, "
				"	data_type, "
				"	character_maximum_length, "
				"	numeric_precision, "
				"	numeric_scale, "
				"	is_nullable, "
				"	'' as primarykey, "
				"	column_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	tempdb.information_schema.columns "
				"where "
				"	table_name like '%s____%%' "
				"order by "
				"	ordinal_position";
			}
		} else {
			if (wild) {
				return
				"select "
				"	column_name, "
				"	data_type, "
				"	character_maximum_length, "
				"	numeric_precision, "
				"	numeric_scale, "
				"	is_nullable, "
				"	'' as primarykey, "
				"	column_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	information_schema.columns "
				"where "
				"	table_name='%s' "
				"	and "
				"	column_name like '%s' "
				"order by "
				"	ordinal_position";
			} else {
				return
				"select "
				"	column_name, "
				"	data_type, "
				"	character_maximum_length, "
				"	numeric_precision, "
				"	numeric_scale, "
				"	is_nullable, "
				"	'' as primarykey, "
				"	column_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	information_schema.columns "
				"where "
				"	table_name='%s' "
				"order by "
				"	ordinal_position";
			}
		}
	}
}

const char *freetdsconnection::selectDatabaseQuery() {
	return "use %s";
}

const char *freetdsconnection::getCurrentDatabaseQuery() {
	return "select db_name()";
}

const char *freetdsconnection::getLastInsertIdQuery() {
	return "select @@identity";
}

const char *freetdsconnection::getNoopQuery() {
	return "waitfor delay '0:0'";
}

const char *freetdsconnection::bindFormat() {
	return "@*";
}

const char *freetdsconnection::beginTransactionQuery() {
	return "BEGIN TRANSACTION";
}

freetdscursor::freetdscursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {

	#if defined(VERSION_NO)
	char	*versionstring=charstring::duplicate(VERSION_NO);
	#elif defined(TDS_VERSION_NO)
	char	*versionstring=charstring::duplicate(TDS_VERSION_NO);
	#else
	char	*versionstring=new char[1024];
	CS_INT	outlen;
	if (ct_config(NULL,CS_GET,CS_VER_STRING,
			(void *)versionstring,1023,&outlen)==CS_SUCCEED) {
		versionstring[outlen]='\0';
	} else {
		versionstring=charstring::copy(versionstring,"freetds v0.00.0");
	}
	#endif

	char	*v=charstring::findFirst(versionstring,'v');
	if (v) {
		*v='\0';
		majorversion=charstring::toInteger(v+1);
		char	*firstdot=charstring::findFirst(v+1,'.');
		if (firstdot) {
			*firstdot='\0';
			minorversion=charstring::toInteger(firstdot+1);
			char	*seconddot=
				charstring::findFirst(firstdot+1,'.');
			if (seconddot) {
				*seconddot='\0';
				patchlevel=charstring::toInteger(seconddot+1);
			} else {
				patchlevel=0;
			}
		} else {
			minorversion=0;
			patchlevel=0;
		}
	} else {
		majorversion=0;
		minorversion=0;
		patchlevel=0;
	}
	delete[] versionstring;

	// Affected row count is generally supported in versions >= 0.53 but
	// appears to be broken in 0.61.
	knowsaffectedrows=(majorversion>0 ||
				(minorversion>=53 && minorversion!=61));

	prepared=false;
	freetdsconn=(freetdsconnection *)conn;
	cmd=NULL;
	languagecmd=NULL;
	cursorcmd=NULL;

	cursornamelength=charstring::integerLength(id);
	cursorname=charstring::parseNumber(id);

	uint16_t	maxbindcount=conn->cont->getConfig()->getMaxBindCount();
	parameter=new CS_DATAFMT[maxbindcount];
	outbindtype=new CS_INT[maxbindcount];
	outbindstrings=new char *[maxbindcount];
	outbindstringlengths=new uint32_t[maxbindcount];
	outbindints=new int64_t *[maxbindcount];
	outbinddoubles=new double *[maxbindcount];
	outbinddates=new datebind[maxbindcount];

	// replace the regular expressions used to detect creation of a
	// temporary table
	setCreateTempTablePattern("^(create|CREATE)[ 	\r\n]"
					"+(table|TABLE)[ 	\r\n]+#");

	allocateResultSetBuffers(conn->cont->getMaxColumnCount());

	// define a template column-bind definition...
	// get the field as a null terminated character string
	// no longer than MAX_ITEM_BUFFER_SIZE, override some
	templatecolumn.datatype=CS_CHAR_TYPE;
	templatecolumn.format=CS_FMT_NULLTERM;
	templatecolumn.maxlength=conn->cont->getMaxFieldLength();
	templatecolumn.scale=CS_UNUSED;
	templatecolumn.precision=CS_UNUSED;
	templatecolumn.status=CS_UNUSED;
	templatecolumn.count=conn->cont->getFetchAtOnce();
	templatecolumn.usertype=CS_UNUSED;
	templatecolumn.locale=NULL;
}

freetdscursor::~freetdscursor() {
	close();
	delete[] cursorname;
	delete[] parameter;
	delete[] outbindtype;
	delete[] outbindstrings;
	delete[] outbindstringlengths;
	delete[] outbindints;
	delete[] outbinddoubles;
	delete[] outbinddates;

	deallocateResultSetBuffers();
}

void freetdscursor::allocateResultSetBuffers(int32_t columncount) {

	if (!columncount) {
		this->columncount=0;
		column=NULL;
		data=NULL;
		datalength=NULL;
		nullindicator=NULL;
	} else {
		this->columncount=columncount;
		column=new CS_DATAFMT[columncount];
		data=new char *[columncount];
		datalength=new CS_INT *[columncount];
		nullindicator=new CS_SMALLINT *[columncount];
		uint32_t	fetchatonce=conn->cont->getFetchAtOnce();
		uint32_t	maxfieldlength=conn->cont->getMaxFieldLength();
		for (int32_t i=0; i<columncount; i++) {
			data[i]=new char[fetchatonce*maxfieldlength];
			datalength[i]=new CS_INT[fetchatonce];
			nullindicator[i]=new CS_SMALLINT[fetchatonce];
		}
	}
}

void freetdscursor::deallocateResultSetBuffers() {
	if (columncount) {
		delete[] column;
		for (int32_t i=0; i<columncount; i++) {
			delete[] data[i];
			delete[] datalength[i];
			delete[] nullindicator[i];
		}
		delete[] data;
		delete[] datalength;
		delete[] nullindicator;
		columncount=0;
	}
}

bool freetdscursor::open() {

	clean=true;

	if (ct_cmd_alloc(freetdsconn->dbconn,&languagecmd)!=CS_SUCCEED) {
		return false;
	}
	if (ct_cmd_alloc(freetdsconn->dbconn,&cursorcmd)!=CS_SUCCEED) {
		return false;
	}
	cmd=NULL;

	// switch to the correct database
	// (only do this once per connection)
	bool	retval=true;
	if (!charstring::isNullOrEmpty(freetdsconn->db) &&
					!freetdsconn->dbused) {
		uint32_t	len=charstring::length(freetdsconn->db)+4;
		char		*query=new char[len+1];
		charstring::printf(query,len+1,"use %s",freetdsconn->db);
		if (!(prepareQuery(query,len) && executeQuery(query,len))) {
			char		err[2048];
			uint32_t	errlen;
			int64_t		errn;
			bool		live;
			errorMessage(err,sizeof(err),&errlen,&errn,&live);
			stderror.printf("%s\n",err);
			retval=false;
		} else {
			freetdsconn->dbused=true;
		}
		closeResultSet();
		delete[] query;
	}

	if (!freetdsconn->dbversion) {

		// try the various queries that might return the version
		const char	*query[]={
			"select @@version",
			"sp_version installmaster",
			NULL
		};
		CS_INT		index[]={
			0,1,0
		};

		for (uint32_t i=0; query[i] && !freetdsconn->dbversion; i++) {

			const char	*q=query[i];
			int32_t		len=charstring::length(q);
			bool		error=false;

			if (prepareQuery(q,len) &&
					executeQuery(q,len) &&
					fetchRow(&error)) {
				freetdsconn->dbversion=
					charstring::duplicate(data[index[i]]);
			}

			closeResultSet();
		}

		// fall back to unknown
		if (!freetdsconn->dbversion) {
			freetdsconn->dbversion=
				charstring::duplicate("unknown");
		}

		// set sybasedb too, it's sybase if it's not microsoft
		freetdsconn->sybasedb=!charstring::contains(
					freetdsconn->dbversion,"Microsoft");
	}
	return retval;
}

bool freetdscursor::close() {

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

bool freetdscursor::prepareQuery(const char *query, uint32_t length) {

	// if the client aborts while a query is in the middle of running,
	// commit or rollback will be called, potentially before closeResultSet
	// is called and, since we're really only using 1 cursor, it will fail
	// unless closeResultSet gets called, so just to make sure, we'll call
	// it here
	closeResultSet();

	// initialize column count
	ncols=0;

	clean=true;

	this->query=(char *)query;
	this->length=length;

	paramindex=0;
	outbindindex=0;

	if ((!charstring::compare(query,"select",6) ||
		!charstring::compare(query,"SELECT",6)) &&
		character::isWhitespace(query[6])) {

		// initiate a cursor command
		cmd=cursorcmd;
		#ifdef FREETDS_SUPPORTS_CURSORS
		if (ct_cursor(cursorcmd,
				CS_CURSOR_DECLARE,
				(CS_CHAR *)cursorname,
				(CS_INT)cursornamelength,
				(CS_CHAR *)query,
				length,
				//CS_READ_ONLY)!=CS_SUCCEED) {
				CS_UNUSED)!=CS_SUCCEED) {
			return false;
		}
		#endif

	} else if ((!charstring::compare(query,"exec",4) ||
			!charstring::compare(query,"EXEC",4)) &&
					character::isWhitespace(query[4])) {

		// initiate an rpc command
		cmd=languagecmd;
		#ifdef FREETDS_SUPPORTS_CURSORS
		if (ct_command(languagecmd,
				CS_RPC_CMD,
				(CS_CHAR *)query+5,
				length-5,
				CS_UNUSED)!=CS_SUCCEED) {
			return false;
		}
		#endif

	} else if ((!charstring::compare(query,"execute",7) ||
			!charstring::compare(query,"EXECUTE",7)) &&
					character::isWhitespace(query[7])) {

		// initiate an rpc command
		cmd=languagecmd;
		#ifdef FREETDS_SUPPORTS_CURSORS
		if (ct_command(languagecmd,
				CS_RPC_CMD,
				(CS_CHAR *)query+8,
				length-8,
				CS_UNUSED)!=CS_SUCCEED) {
			return false;
		}
		#endif

	} else {

		// initiate a language command
		cmd=languagecmd;
		#ifdef FREETDS_SUPPORTS_CURSORS
		if (ct_command(languagecmd,
				CS_LANG_CMD,
				(CS_CHAR *)query,
				length,
				CS_UNUSED)!=CS_SUCCEED) {
			return false;
		}
		#endif
	}

	clean=false;
	prepared=true;
	return true;
}

bool freetdscursor::supportsNativeBinds(const char *query, uint32_t length) {
	#ifdef FREETDS_SUPPORTS_CURSORS
		return true;
	#else
		return false;
	#endif
}

void freetdscursor::encodeBlob(stringbuffer *buffer,
					const char *data, uint32_t datasize) {

	// sybase/mssqlserver wants each byte of blob data to be converted to
	// two hex characters and the whole thing to start with 0x
	// eg: hello - > 0x68656C6C6F

	buffer->append("0x");
	for (uint32_t i=0; i<datasize; i++) {
		buffer->append(conn->cont->asciiToHex(data[i]));
	}
}

void freetdscursor::checkRePrepare() {

	// Sybase doesn't allow you to rebind and re-execute when using 
	// ct_command.  You have to re-prepare too.  I'll make this transparent
	// to the user.
	if (!prepared) {
		prepareQuery(query,length);
	}
}

#ifdef FREETDS_SUPPORTS_CURSORS
bool freetdscursor::inputBind(const char *variable,
				uint16_t variablesize,
				const char *value,
				uint32_t valuesize,
				int16_t *isnull) {

	checkRePrepare();

	(CS_VOID)bytestring::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
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

bool freetdscursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value) {

	checkRePrepare();

	(CS_VOID)bytestring::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
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

bool freetdscursor::inputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {

	checkRePrepare();

	(CS_VOID)bytestring::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
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

bool freetdscursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t year,
				int16_t month,
				int16_t day,
				int16_t hour,
				int16_t minute,
				int16_t second,
				int32_t microsecond,
				const char *tz,
				bool isnegative,
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

bool freetdscursor::outputBind(const char *variable, 
				uint16_t variablesize,
				char *value, 
				uint32_t valuesize, 
				int16_t *isnull) {

	checkRePrepare();

	outbindtype[outbindindex]=CS_CHAR_TYPE;
	outbindstrings[outbindindex]=value;
	outbindstringlengths[outbindindex]=valuesize;
	outbindindex++;

	(CS_VOID)bytestring::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
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

bool freetdscursor::outputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {

	checkRePrepare();

	outbindtype[outbindindex]=CS_INT_TYPE;
	outbindints[outbindindex]=value;
	outbindindex++;

	(CS_VOID)bytestring::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
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

bool freetdscursor::outputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t *precision,
				uint32_t *scale,
				int16_t *isnull) {

	checkRePrepare();

	outbindtype[outbindindex]=CS_FLOAT_TYPE;
	outbinddoubles[outbindindex]=value;
	outbindindex++;

	(CS_VOID)bytestring::zero(&parameter[paramindex],
				sizeof(parameter[paramindex]));
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

bool freetdscursor::outputBind(const char *variable,
				uint16_t variablesize,
				int16_t *year,
				int16_t *month,
				int16_t *day,
				int16_t *hour,
				int16_t *minute,
				int16_t *second,
				int32_t *microsecond,
				const char **tz,
				bool *isnegative,
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
	outbinddates[outbindindex].isnegative=isnegative;
	outbindindex++;

	bytestring::zero(&parameter[paramindex],sizeof(parameter[paramindex]));
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
#endif

bool freetdscursor::executeQuery(const char *query, uint32_t length) {

	// initialize results (We use CS_UNSUPPORTED so that if the query
	// fails to execute, discardResults won't attempt to cancel any
	// non-existent result sets.  Doing that crashes FreeTDS.)
	results=CS_UNSUPPORTED;

	// clear out any errors
	freetdsconn->errorcode=0;
	freetdsconn->liveconnection=true;

	if (ct_command(cmd,CS_LANG_CMD,
			(CS_CHAR *)query,length,
			CS_UNUSED)!=CS_SUCCEED) {
		return false;
	}
	clean=false;

	// initialize row counts
	affectedrows=0;
	row=0;
	maxrow=0;
	totalrows=0;

	#ifdef FREETDS_SUPPORTS_CURSORS
	if (cmd==cursorcmd) {
		if (ct_cursor(cursorcmd,CS_CURSOR_ROWS,
					NULL,CS_UNUSED,
					NULL,CS_UNUSED,
					(CS_INT)conn->cont->getFetchAtOnce())!=
					CS_SUCCEED) {
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
		closeResultSet();
		return false;
	}

	for (;;) {

		results=ct_results(cmd,&resultstype);

		// handle the end of all result sets
		if (results==CS_END_RESULTS) {
			break;
		}

		// handle failed queries
		if (results==CS_FAIL || resultstype==CS_CMD_FAIL) {
			closeResultSet();
			return false;
		}

		// Queries can generate multiple result sets.
		//
		// A DML/DDL query will just send a CS_CMD_SUCCEED.
		//
		// If we're not using cursors, then selects will also just
		// send a single CS_ROW_RESULT.
		//
		// But...
		//
		// If a cursor is used to execute a select, then each
		// ct_cursor() call generates a results set, and then the
		// ct_send also generates a CS_ROW_RESULT result set.
		//
		// RPC queries (EXEC some-stored-procedures or direct
		// TransactSQL may generate a series of result sets including
		// CS_CMD_SUCCEED, CS_CMD_DONE, CS_STATUS_RESULT, CS_ROW_RESULT
		// or CS_COMPUTE_RESULT result sets, in any combination or
		// order.
		//
		// Currently SQL Relay only supports 1 result set per query, so
		// for a given query, we only really care about one result set,
		// the CS_PARAM_RESULT, CS_ROW_RESULT, CS_CURSOR_RESULT, or
		// CS_COMPUTE_RESULT.  We'll grab whichever of those we get
		// first, and ignore the rest.

		if (resultstype==CS_CMD_SUCCEED) {
			// If we got CS_CMD_SUCCEED, then try to get the
			// affected row count.  The query might have been
			// DML/DDL, or this could be one of the result sets of
			// a stored procedure or direct TransactSQL.  We need
			// to do this here because we're going to cancel this
			// result set below.
			affectedrows=0;
			if (knowsaffectedrows) {
				if (ct_res_info(cmd,CS_ROW_COUNT,
					(CS_VOID *)&affectedrows,
					CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
					return false;
				}
			}
		}  else if (resultstype==CS_PARAM_RESULT ||
				resultstype==CS_ROW_RESULT ||
				resultstype==CS_CURSOR_RESULT ||
				resultstype==CS_COMPUTE_RESULT) {
			break;
		}

		// the result set was a type that we want to ignore
		if (ct_cancel(NULL,cmd,CS_CANCEL_CURRENT)==CS_FAIL) {
			freetdsconn->liveconnection=false;
			// FIXME: call ct_close(CS_FORCE_CLOSE)?
			return false;
		}
	}

	checkForTempTable(query,length);

	// reset the prepared flag
	prepared=false;

	// For queries which return rows or parameters (output bind variables),
	// get the column count and bind columns.
	bool	moneycolumn=false;
	if (resultstype==CS_ROW_RESULT ||
			resultstype==CS_CURSOR_RESULT ||
			resultstype==CS_COMPUTE_RESULT ||
			resultstype==CS_PARAM_RESULT) {

		// get the column count
		if (ct_res_info(cmd,CS_NUMDATA,(CS_VOID *)&ncols,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
			return false;
		}

		// allocate buffers and limit column count if necessary
		uint32_t	maxcolumncount=conn->cont->getMaxColumnCount();
		if (!maxcolumncount) {
			// see note in discardResults for
			// why we're doing this here
			deallocateResultSetBuffers();
			allocateResultSetBuffers(ncols);
		} else if ((uint32_t)ncols>maxcolumncount) {
			ncols=maxcolumncount;
		}

		// bind columns
		for (CS_INT i=0; i<ncols; i++) {

			// dealing with money columns cause freetds < 0.53 to
			// crash, take care of that here...
			if (majorversion==0 && minorversion<53
							&& !moneycolumn) {
				CS_DATAFMT	moneytest;
				ct_describe(cmd,i+1,&moneytest);
				if (moneytest.datatype==CS_MONEY_TYPE ||
					moneytest.datatype==CS_MONEY4_TYPE) {
					moneycolumn=true;
					freetdsconn->errorstring.clear();
					freetdsconn->errorstring.append(
						"FreeTDS versions prior to "
						"0.53 do not support MONEY "
						"or SMALLMONEY datatypes. "
						"Please upgrade SQL Relay to "
						"a version compiled against "
						"FreeTDS >= 0.53 ");
				}
			}

			// reset the column-bind
			column[i]=templatecolumn;

			// actually...
			// if we're getting the output bind variables of a
			// stored procedure that returns dates, then use the
			// datetime type instead...
			if (resultstype==CS_PARAM_RESULT &&
				outbindtype[i]==CS_DATETIME_TYPE) {
				column[i].datatype=CS_DATETIME_TYPE;
				column[i].format=CS_FMT_UNUSED;
				column[i].maxlength=sizeof(CS_DATETIME);
			}
	
			// bind the columns for the fetches
			if (ct_bind(cmd,i+1,
					&column[i],
					(CS_VOID *)data[i],
					datalength[i],
					nullindicator[i])!=CS_SUCCEED) {
				break;
			}

			// describe the columns
			if (conn->cont->getSendColumnInfo()==SEND_COLUMN_INFO) {
				if (ct_describe(cmd,i+1,
						&column[i])!=CS_SUCCEED) {
					break;
				}
			}
		}

	}

	// If we got a moneycolumn (and version<0.53) then cancel the
	// result set.  Otherwise FreeTDS will spew "unknown marker"
	// errors to the screen when closeResultSet() is called.
	if (moneycolumn) {
		if (ct_cancel(NULL,cmd,CS_CANCEL_CURRENT)==CS_FAIL) {
			freetdsconn->liveconnection=false;
			// FIXME: call ct_close(CS_FORCE_CLOSE)?
			return false;
		}
		return false;
	}


	// if we're doing an rpc query, the result set should be a single
	// row of output parameter results, fetch it and populate the output
	// bind variables...
	if (resultstype==CS_PARAM_RESULT) {

		if (ct_fetch(cmd,
				CS_UNUSED,
				CS_UNUSED,
				CS_UNUSED,
				&rowsread)!=CS_SUCCEED || !rowsread) {
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
				bytestring::copy(outbindstrings[i],
						data[i],length);
			} else if (outbindtype[i]==CS_INT_TYPE) {
				*outbindints[i]=
					charstring::toInteger(data[i]);
			} else if (outbindtype[i]==CS_FLOAT_TYPE) {
				*outbinddoubles[i]=
					charstring::toFloatC(data[i]);
			} else if (outbindtype[i]==CS_DATETIME_TYPE) {

				// convert to a CS_DATEREC
				CS_DATEREC	dr;
				cs_dt_crack(freetdsconn->context,
						CS_DATETIME_TYPE,
						(CS_VOID *)data[i],&dr);

				datebind	*db=&outbinddates[i];
				*(db->year)=dr.dateyear;
				*(db->month)=dr.datemonth+1;
				*(db->day)=dr.datedmonth;
				*(db->hour)=dr.datehour;
				*(db->minute)=dr.dateminute;
				*(db->second)=dr.datesecond;
				*(db->microsecond)=dr.datemsecond;
				*(db->tz)=NULL;
				*(db->isnegative)=false;
			}
		}


		discardResults();
		ncols=0;
	}

	// return success only if no error was generated
	return (!freetdsconn->errorcode);
}

bool freetdscursor::knowsAffectedRows() {
	return knowsaffectedrows;
}

uint64_t freetdscursor::affectedRows() {
	return affectedrows;
}

uint32_t freetdscursor::colCount() {
	return ncols;
}

const char *freetdscursor::getColumnName(uint32_t col) {
	return column[col].name;
}

uint16_t freetdscursor::getColumnType(uint32_t col) {
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
		#ifdef CS_BIGINT_TYPE
		case CS_BIGINT_TYPE:
			return BIGINT_DATATYPE;
		#endif
		#ifdef CS_UBIGINT_TYPE
		case CS_UBIGINT_TYPE:
			return UBIGINT_DATATYPE;
		#endif
		#ifdef CS_UNIQUE_TYPE
		case CS_UNIQUE_TYPE:
			return UNIQUEIDENTIFIER_DATATYPE;
		#endif
		default:
			return UNKNOWN_DATATYPE;
	}
}

uint32_t freetdscursor::getColumnLength(uint32_t col) {
	// limit the column size
	uint32_t	maxfieldlength=conn->cont->getMaxFieldLength();
	if ((uint32_t)column[col].maxlength>maxfieldlength) {
		column[col].maxlength=maxfieldlength;
	}
	return column[col].maxlength;
}

uint32_t freetdscursor::getColumnPrecision(uint32_t col) {
	return column[col].precision;
}

uint32_t freetdscursor::getColumnScale(uint32_t col) {
	return column[col].scale;
}

uint16_t freetdscursor::getColumnIsNullable(uint32_t col) {
	return (column[col].status&CS_CANBENULL);
}

uint16_t freetdscursor::getColumnIsPartOfKey(uint32_t col) {
	return (column[col].status&(CS_KEY|CS_VERSION_KEY));
}

uint16_t freetdscursor::getColumnIsUnsigned(uint32_t col) {
	return (getColumnType(col)==USHORT_DATATYPE);
}

uint16_t freetdscursor::getColumnIsBinary(uint32_t col) {
	return (getColumnType(col)==IMAGE_DATATYPE);
}

uint16_t freetdscursor::getColumnIsAutoIncrement(uint32_t col) {
	return (column[col].status&CS_IDENTITY);
}

bool freetdscursor::ignoreDateDdMmParameter(uint32_t col,
					const char *data, uint32_t size) {

	// This is for a very FreeTDS/MSSQL Server-specific issue...
	//
	// If we're translating dates in the result set then we have to be
	// careful about dates in the yyyy-xx-xx format.
	//
	// FreeTDS recognizes Sybase date/datetime columns and MSSQL datetime
	// columns as type CS_DATETIME_TYPE and formats them according to the
	// rules defined in locales.conf for the locale of the SQL Relay server.
	//
	// However, FreeTDS recognizes MSSQL date columns as type CS_CHAR_TYPE
	// and formats them in yyyy-mm-dd format universally.
	//
	// The date conversion routines take any fields that look like a date
	// and translate them to the specified format.  Unfortunately if you're
	// in a region where dates are formatted dd/mm/yyyy (for example) then
	// dateddmm="yes" needs to be set so string literals like "03/04/2005"
	// will be recognized as April 3, 2005.  This would cause problems when
	// fetching date columns from MSSQL so we have to ignore dateddmm in
	// that case.
	//
	// Ideally we'd only ignore dateddmm for MSSQL, on CS_DATETIME_TYPE
	// columns that appeared to be in the yyyy-xx-xx format.  But, since
	// date columns are returned as CS_CHAR_TYPE then we can't.  So, we
	// end up ignoring dateddmm for everything in yyyy-xx-xx format, whether
	// fetched from a date column or from a string column or from a
	// string literal in the original query.
	//
	// That means that if date translation is in effect, if dates are stored
	// in string fields in yyyy-xx-xx format, then they must be stored in
	// yyyy-mm-dd format.
	//
	// It also means that if date translation in in effect then unless the
	// translatedatetimes translation module is being used to normalize
	// string literals in the original query to some other format, then
	// they have to be in yyyy-mm-dd format as well.

	// Override the dateddmm parameter if we're using MSSQL Server and the
	// fields is in yyyy-xx-xx format and appears to be a date.
	return (!freetdsconn->sybasedb &&
			size==10 &&
			data[4]=='-' && data[7]=='-' &&
			charstring::isNumber(data,4) &&
			charstring::isNumber(data+5,2) &&
			charstring::isNumber(data+8,2));
}

bool freetdscursor::noRowsToReturn() {
	// unless the query was a successful select, send no data
	return (resultstype!=CS_ROW_RESULT &&
			resultstype!=CS_CURSOR_RESULT &&
			resultstype!=CS_COMPUTE_RESULT);
}

bool freetdscursor::skipRow(bool *error) {
	if (fetchRow(error)) {
		row++;
		return true;
	}
	return false;
}

bool freetdscursor::fetchRow(bool *error) {

	*error=false;
	// FIXME: set error if an error occurs

	if (row==(CS_INT)conn->cont->getFetchAtOnce()) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return false;
	}
	if (!row) {
		CS_RETCODE	fetchresult=ct_fetch(cmd,CS_UNUSED,
							CS_UNUSED,
							CS_UNUSED,
							&rowsread);

		// This is essential with freetds.
		// http://www.freetds.org/faq.html#pending
		//
		// Basically the TDS protocol doesn't handle multiple
		// simultaneous queries per connection.  You can open multiple
		// cursors but you can't use them at the same time.  Somehow
		// Sybase gets around this but FreeTDS doesn't.  In particular,
		// unless all rows and all results sets have been fetched or
		// cancelled for all cursors, another cursor cannot run another
		// query.  Since TDS supports multiple result sets per query,
		// it's not enough to just fetch all of the rows, all result
		// sets must be fetched or cancelled as well until ct_results
		// returns CS_END_RESULTS or CS_CANCELLED.  Since SQL Relay only
		// supports one result set per query, we can go ahead and cancel
		// any remaining result sets here.  closeResultSet would do this
		// for us but not before another query gets run on the same
		// cursor, so we must explicitly call it here too in case
		// someone wants to do something with another cursor.
		if (fetchresult==CS_END_DATA) {
			discardResults();
		}

		if (fetchresult!=CS_SUCCEED || !rowsread) {
			if (fetchresult==CS_FAIL || fetchresult==CS_ROW_FAIL) {
				*error=true;
			}
			return false;
		}
		maxrow=rowsread;
		totalrows=totalrows+rowsread;
	}
	return true;
}

void freetdscursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {

	// handle NULLs
	if (nullindicator[col][row]==-1) {
		*null=true;
		return;
	}

	uint32_t	maxfieldlength=conn->cont->getMaxFieldLength();

	// Empty TEXT fields don't get properly converted
	// to null-terminated strings.  Handle them.
	if (column[col].datatype==CS_TEXT_TYPE && !datalength[col][row]) {
		data[col][row*maxfieldlength]='\0';
		datalength[col][row]=1;
	}

	// handle normal datatypes
	*field=&data[col][row*maxfieldlength];
	*fieldlength=datalength[col][row]-1;
}

void freetdscursor::nextRow() {
	row++;
}

void freetdscursor::closeResultSet() {

	if (clean) {
		return;
	}

	discardResults();
	discardCursor();

	clean=true;
}

void freetdscursor::discardResults() {

	// if there are any unprocessed result sets, process them
	if (results==CS_SUCCEED) {
		do {
			if (ct_cancel(NULL,cmd,CS_CANCEL_CURRENT)==CS_FAIL) {
				freetdsconn->liveconnection=false;
				// FIXME: call ct_close(CS_FORCE_CLOSE)?
			}
			results=ct_results(cmd,&resultstype);
		} while (results==CS_SUCCEED);
	}

	if (results==CS_FAIL) {
		if (ct_cancel(NULL,cmd,CS_CANCEL_ALL)==CS_FAIL) {
			freetdsconn->liveconnection=false;
			// FIXME: call ct_close(CS_FORCE_CLOSE)?
		}
	}

	// Deallocating the result set buffers here causes a problem, but only
	// in the freetds connection.
	// When using freetds, we have to call discardResults() when we hit
	// the end of the result set (see note in fetchRow()) but if we
	// deallocate the result set buffers at that point, then subsequent
	// attempts to fetch column info, will result in a reference-after-free.
	// So, unlike the in the sybase/sap connection code we defer the
	// deallocate until right before the next allocate, in prepareQuery().
	/*if (!conn->cont->getMaxColumnCount()) {
		deallocateResultSetBuffers();
	}*/
}


void freetdscursor::discardCursor() {

	#ifdef FREETDS_SUPPORTS_CURSORS
	if (cmd==cursorcmd) {
		if (ct_cursor(cursorcmd,CS_CURSOR_CLOSE,
					NULL,CS_UNUSED,
					NULL,CS_UNUSED,
					CS_DEALLOC)==CS_SUCCEED) {
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
	if (errorcode) {
		return CS_SUCCEED;
	}

	errorcode=msgp->msgnumber;

	errorstring.clear();
	errorstring.append("Client Library error: ")->append(msgp->msgstring);
	errorstring.append(" severity(")->
		append((int32_t)CS_SEVERITY(msgp->msgnumber))->append(")");
	errorstring.append(" layer(")->
		append((int32_t)CS_LAYER(msgp->msgnumber))->append(")");
	errorstring.append(" origin(")->
		append((int32_t)CS_ORIGIN(msgp->msgnumber))->append(")");
	errorstring.append(" number(")->
		append((int32_t)CS_NUMBER(msgp->msgnumber))->append(")");

	if (msgp->osstringlen>0) {
		errorstring.append("  Operating System Error: ");
		errorstring.append(msgp->osstring);
	}

	// for a timeout message,
	// set liveconnection to false
	if (CS_SEVERITY(msgp->msgnumber)==CS_SV_RETRY_FAIL &&
		CS_LAYER(msgp->msgnumber)==63 &&
		CS_ORIGIN(msgp->msgnumber)==63 &&
		CS_NUMBER(msgp->msgnumber)==63) {
		liveconnection=false;

	// for a read from sql server failed message,
	// set liveconnection to false
	} else if (CS_SEVERITY(msgp->msgnumber)==78 &&
		CS_LAYER(msgp->msgnumber)==0 &&
		CS_ORIGIN(msgp->msgnumber)==0 &&
		(CS_NUMBER(msgp->msgnumber)==36 ||
		CS_NUMBER(msgp->msgnumber)==38)) {
		liveconnection=false;
	}
	// FIXME: sybase connection has another case, do we need it?

	return CS_SUCCEED;
}

CS_RETCODE freetdsconnection::clientMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp) {
	if (errorcode) {
		return CS_SUCCEED;
	}

	errorcode=msgp->msgnumber;

	errorstring.clear();
	errorstring.append("Client Library error: ")->append(msgp->msgstring);
	errorstring.append(" severity(")->
		append((int32_t)CS_SEVERITY(msgp->msgnumber))->append(")");
	errorstring.append(" layer(")->
		append((int32_t)CS_LAYER(msgp->msgnumber))->append(")");
	errorstring.append(" origin(")->
		append((int32_t)CS_ORIGIN(msgp->msgnumber))->append(")");
	errorstring.append(" number(")->
		append((int32_t)CS_NUMBER(msgp->msgnumber))->append(")");

	if (msgp->osstringlen>0) {
		errorstring.append("  Operating System Error: ");
		errorstring.append(msgp->osstring);
	}

	// for a timeout message,
	// set liveconnection to false
	if (CS_SEVERITY(msgp->msgnumber)==CS_SV_RETRY_FAIL &&
		CS_LAYER(msgp->msgnumber)==63 &&
		CS_ORIGIN(msgp->msgnumber)==63 &&
		CS_NUMBER(msgp->msgnumber)==63) {
		liveconnection=false;

	// for a read from sql server failed message,
	// set liveconnection to false
	} else if (CS_SEVERITY(msgp->msgnumber)==78 &&
		CS_LAYER(msgp->msgnumber)==0 &&
		CS_ORIGIN(msgp->msgnumber)==0 &&
		(CS_NUMBER(msgp->msgnumber)==36 ||
		CS_NUMBER(msgp->msgnumber)==38)) {
		liveconnection=false;
	}
	// FIXME: sybase connection has another case, do we need it?

	return CS_SUCCEED;
}

CS_RETCODE freetdsconnection::serverMessageCallback(CS_CONTEXT *ctxt, 
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp) {

	// This is a special case, for some reason, "use db" queries
	// throw a warning, ignore them.
	if ((CS_NUMBER(msgp->msgnumber)==5701 &&
			(CS_SEVERITY(msgp->msgnumber)==10 ||
				CS_SEVERITY(msgp->msgnumber)==0)) ||
		(CS_NUMBER(msgp->msgnumber)==69 &&
				CS_SEVERITY(msgp->msgnumber)==22)) {
		return CS_SUCCEED;
	}

	if (errorcode) {
		return CS_SUCCEED;
	}

	errorcode=msgp->msgnumber;

	errorstring.clear();
	errorstring.append("Server message: ")->append(msgp->text);
	errorstring.append(" severity(")->
		append((int32_t)CS_SEVERITY(msgp->msgnumber))->append(")");
	errorstring.append(" number(")->
		append((int32_t)CS_NUMBER(msgp->msgnumber))->append(")");
	errorstring.append(" state(")->
		append((int32_t)msgp->state)->append(")");
	errorstring.append(" line(")->
		append((int32_t)msgp->line)->append(")");
	errorstring.append("  Server Name:")->append(msgp->svrname);
	errorstring.append("  Procedure Name:")->append(msgp->proc);

	return CS_SUCCEED;
}

const char *freetdsconnection::tempTableDropPrefix() {
	return "#";
}

bool freetdsconnection::commit() {
	cont->closeAllResultSets();
	return sqlrserverconnection::commit();
}

bool freetdsconnection::rollback() {
	cont->closeAllResultSets();
	return sqlrserverconnection::rollback();
}

void freetdsconnection::errorMessage(char *errorbuffer,
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
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_freetdsconnection(
						sqlrservercontroller *cont) {
		return new freetdsconnection(cont);
	}
}
