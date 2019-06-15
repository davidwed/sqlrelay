// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/bytestring.h>
#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	#include <rudiments/file.h>
#endif
#include <rudiments/sys.h>

#include <defines.h>
#include <datatypes.h>
#include <config.h>

#include <libpq-fe.h>

//#undef HAVE_POSTGRESQL_PQSENDQUERYPREPARED

class SQLRSERVER_DLLSPEC postgresqlconnection : public sqlrserverconnection {
	friend class postgresqlcursor;
	public:
			postgresqlconnection(sqlrservercontroller *cont);
			~postgresqlconnection();
	private:
		void		handleConnectString();
		bool		logIn(const char **error, const char **warning);
		bool		logIn(const char **error,
					const char **warning,
					const char *database);
		const char	*logInError(const char *errmsg);
		sqlrservercursor	*newCursor(uint16_t id);
		void		deleteCursor(sqlrservercursor *curs);
		void		logOut();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		const char	*dbIpAddressQuery();
		const char	*dbIpAddress();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild,
						uint16_t objecttypes);
		const char	*getColumnListQuery(
					const char *table, bool wild);
		bool		selectDatabase(const char *database);
		const char	*getCurrentDatabaseQuery();
		bool		getLastInsertId(uint64_t *id);
		const char	*getLastInsertIdQuery();
		const char	*noopQuery();
		const char	*bindFormat();

		dictionary< int32_t, char *>	datatypes;
		dictionary< int32_t, char *>	tables;

		PGconn	*pgconn;

		const char	*host;
		const char	*port;
		const char	*options;
		const char	*db;
		const char	*sslmode;
		uint16_t	typemangling;
		uint16_t	tablemangling;
		const char	*charset;
		char		*dbversion;
		char		*hostname;

#ifdef HAVE_POSTGRESQL_PQCONNECTDB
		stringbuffer	conninfo;
#endif

		stringbuffer	errormessage;

#ifdef HAVE_POSTGRESQL_PQOIDVALUE
		Oid	currentoid;
#endif
		char	*lastinsertidquery;

		const char	*identity;

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	private:
		file	devnull;
#endif
};

class SQLRSERVER_DLLSPEC postgresqlcursor : public sqlrservercursor {
	friend class postgresqlconnection;
	private:
				postgresqlcursor(sqlrserverconnection *conn,
								uint16_t id);
				~postgresqlcursor();
#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
		bool		prepareQuery(const char *query,
						uint32_t length);
#endif
		bool		supportsNativeBinds(const char *query,
							uint32_t length);
		void		encodeBlob(stringbuffer *buffer,
							const char *data,
							uint32_t datasize);
#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
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
		bool		inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
#endif
		bool		executeQuery(const char *query,
						uint32_t length);
#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
		void		errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection);
#endif
		bool		knowsRowCount();
		uint64_t	rowCount();
		uint64_t	affectedRows();
		uint32_t	colCount();
		uint16_t	columnTypeFormat();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		const char	*getColumnTypeName(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
#ifdef HAVE_POSTGRESQL_PQFTABLE
		const char	*getColumnTable(uint32_t col);
#endif
		bool		noRowsToReturn();
		bool		fetchRow(bool *error);
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		closeResultSet();

		PGresult	*pgresult;
		ExecStatusType	pgstatus;
		int		ncols;
		int		nrows;
		uint64_t	affectedrows;
		int		currentrow;

		char		typenamebuffer[32];
		char		tablenamebuffer[32];

		postgresqlconnection	*postgresqlconn;

#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
		uint16_t	maxbindcount;
		char		**bindvalues;
		int		*bindlengths;
		int		*bindformats;
		int		bindcounter;

		bool		bindformaterror;
#endif

#if defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE)
		bool		justexecuted;
#endif
};

#ifdef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
static void nullNoticeProcessor(void *arg, const char *message) {
}
#endif

postgresqlconnection::postgresqlconnection(sqlrservercontroller *cont) :
						sqlrserverconnection(cont) {
	dbversion=NULL;
	datatypes.setTrackInsertionOrder(false);
	tables.setTrackInsertionOrder(false);
	pgconn=NULL;
#ifdef HAVE_POSTGRESQL_PQOIDVALUE
	currentoid=InvalidOid;
#endif
	lastinsertidquery=NULL;
	identity=NULL;
	hostname=NULL;
}

postgresqlconnection::~postgresqlconnection() {
#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	devnull.close();
#endif
	delete[] dbversion;
	delete[] lastinsertidquery;
	delete[] hostname;
}

void postgresqlconnection::handleConnectString() {

	sqlrserverconnection::handleConnectString();

	host=cont->getConnectStringValue("host");
	port=cont->getConnectStringValue("port");
	options=cont->getConnectStringValue("options");
	db=cont->getConnectStringValue("db");
	sslmode=cont->getConnectStringValue("sslmode");
	const char	*typemang=cont->getConnectStringValue("typemangling");
	if (!typemang || charstring::isNo(typemang)) {
		typemangling=0;
	} else if (charstring::isYes(typemang)) {
		typemangling=1;
	} else {
		typemangling=2;
	}
	const char	*tablemang=cont->getConnectStringValue("tablemangling");
	if (!tablemang || charstring::isNo(tablemang)) {
		tablemangling=0;
	} else {
		tablemangling=2;
	}
	charset=cont->getConnectStringValue("charset");
	const char	*lastinsertidfunc=
			cont->getConnectStringValue("lastinsertidfunction");
	if (lastinsertidfunc) {
		stringbuffer	liiquery;
		liiquery.append("select ");
		liiquery.append(lastinsertidfunc);
		lastinsertidquery=liiquery.detachString();
	}
	identity=cont->getConnectStringValue("identity");

	// postgresql doesn't support multi-row fetches
	cont->setFetchAtOnce(1);
	cont->setMaxFieldLength(0);
}

bool postgresqlconnection::logIn(const char **error, const char **warning) {
	return logIn(error,warning,db);
}

bool postgresqlconnection::logIn(const char **error,
					const char **warning,
					const char *database) {

	// clear the datatype dictionary
	if (typemangling==2) {
		datatypes.clearAndArrayDeleteValues();
	}

	// clear the table dictionary
	if (tablemangling==2) {
		tables.clearAndArrayDeleteValues();
	}

	// log in
#ifdef HAVE_POSTGRESQL_PQCONNECTDB
	conninfo.clear();
	conninfo.append("user=")->append(cont->getUser());
	conninfo.append(" password=")->append(cont->getPassword());
	if (!charstring::isNullOrEmpty(host)) {
		conninfo.append(" host=")->append(host);
	}
	if (!charstring::isNullOrEmpty(port)) {
		conninfo.append(" port=")->append(port);
	}
	if (!charstring::isNullOrEmpty(options)) {
		conninfo.append(" options=")->append(options);
	}
	if (!charstring::isNullOrEmpty(database)) {
		conninfo.append(" dbname=")->append(database);
	}
	// sslmode isn't supported by older versions of postgresql, and
	// including it at all will cause PQconnectdb to fail.  Remove it
	// altogether if it's omitted or disabled.
	if (!charstring::isNullOrEmpty(sslmode) &&
			charstring::compare(sslmode,"disable")) {
		conninfo.append(" sslmode=")->append(sslmode);
	}
	pgconn=PQconnectdb(conninfo.getString());
#else
	pgconn=PQsetdbLogin(host,port,options,NULL,database,
				cont->getUser(),cont->getPassword());
#endif

	// check the status of the login
	if (PQstatus(pgconn)==CONNECTION_BAD) {
		*error=logInError("Log in failed");
		logOut();
		return false;
	}

#ifdef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	// make sure that no messages get sent to the console
	PQsetNoticeProcessor(pgconn,nullNoticeProcessor,NULL);
#else
	if (devnull.open("/dev/null",O_RDONLY)) {
		devnull.duplicate(1);
		devnull.duplicate(2);
	}
#endif

#if defined(HAVE_POSTGRESQL_PQSETCLIENTENCODING)
	if (charstring::length(charset)) {
		PQsetClientEncoding(pgconn,charset);
	}
#endif

	// build the datatype dictionary
	if (typemangling==2) {
		PGresult	*result=PQexec(pgconn,
					"select oid,typname from pg_type");
		if (!result) {
			*error=logInError("Get datatypes failed");
			return false;
		}
		for (int i=0; i<PQntuples(result); i++) {
			datatypes.setValue(
				charstring::toInteger(PQgetvalue(result,i,0)),
				charstring::duplicate(PQgetvalue(result,i,1)));
		}
		PQclear(result);
	}

	// build the table dictionary
	if (tablemangling==2) {
		PGresult	*result=PQexec(pgconn,
					"select oid,relname from pg_class");
		if (!result) {
			*error=logInError("Get tables failed");
			return false;
		}
		for (int i=0; i<PQntuples(result); i++) {
			tables.setValue(
				charstring::toInteger(PQgetvalue(result,i,0)),
				charstring::duplicate(PQgetvalue(result,i,1)));
		}
		PQclear(result);
	}

#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
	// don't use bind variables against older servers
	if (PQprotocolVersion(pgconn)<3) {
		cont->setFakeInputBinds(true);
	}
#endif

	return true;
}

const char *postgresqlconnection::logInError(const char *errmsg) {

	errormessage.clear();
	errormessage.append(errmsg)->append(": ");

	// get the error message from postgresql
	const char	*message=PQerrorMessage(pgconn);
	errormessage.append(message);
	return errormessage.getString();
}

sqlrservercursor *postgresqlconnection::newCursor(uint16_t id) {
	return (sqlrservercursor *)new
			postgresqlcursor((sqlrserverconnection *)this,id);
}

void postgresqlconnection::deleteCursor(sqlrservercursor *curs) {
	delete (postgresqlcursor *)curs;
}

void postgresqlconnection::logOut() {

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	devnull.close();
#endif

	if (pgconn) {
		PQfinish(pgconn);
		pgconn=NULL;
	}

	// clear the datatype dictionary
	if (typemangling==2) {
		for (avltreenode< dictionarynode<int32_t,char *> *>
					*node=datatypes.getTree()->getFirst();
					node; node=node->getNext()) {
			delete[] node->getValue()->getValue();
		}
		datatypes.clear();
	}

	// clear the table dictionary
	if (typemangling==2) {
		for (avltreenode< dictionarynode<int32_t,char *> *>
					*node=tables.getTree()->getFirst();
					node; node=node->getNext()) {
			delete[] node->getValue()->getValue();
		}
		tables.clear();
	}
}

void postgresqlconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	const char	*errorstring=PQerrorMessage(pgconn);
	*errorlength=charstring::length(errorstring);
	charstring::safeCopy(errorbuffer,errorbufferlength,
					errorstring,*errorlength);
	// PostgreSQL doesn't have an error number per-se.  We'll set it
	// to 1 though, because 0 typically means "no error has occurred"
	// and some apps respond that way if errorcode is set to 0.
	// This ends up being important when using:
	// Oracle dblink -> ODBC -> SQL Relay -> PostgreSQL
	*errorcode=1;
	*liveconnection=(PQstatus(pgconn)==CONNECTION_OK);
}

const char *postgresqlconnection::identify() {
	return (identity)?identity:"postgresql";
}

const char *postgresqlconnection::dbVersion() {
	delete[] dbversion;
#if defined(HAVE_POSTGRESQL_PQSERVERVERSION)
	dbversion=charstring::parseNumber((uint64_t)PQserverVersion(pgconn));
#else
#if defined(HAVE_POSTGRESQL_PQPARAMETERSTATUS)
	dbversion=charstring::duplicate(PQparameterStatus(pgconn,
							"server_version"));
#else
	PGresult	*result=PQexec(pgconn,"select version()");
	if (!result) {
		return NULL;
	}

	const char	*versionstring=PQgetvalue(result,0,0);
	char		**list;
	uint64_t	listlength;
	charstring::split(versionstring," ",true,&list,&listlength);
	if (listlength>=2) {
		dbversion=list[1];
		list[1]=NULL;
	}
	for (uint64_t i=0; i<listlength; i++) {
		delete[] list[i];
	}
	delete[] list;

	PQclear(result);
#endif
	char		**parts;
	uint64_t	partslength;
	charstring::split(dbversion,".",true,&parts,&partslength);
	if (partslength==3) {
		int64_t	minor=charstring::toInteger(parts[1]);
		int64_t	patch=charstring::toInteger(parts[2]);
		charstring::printf(dbversion,
					charstring::length(dbversion)+1,
					"%s%02lld%02lld",
					parts[0],
					(long long)minor,(long long)patch);
	}
	for (uint64_t i=0; i<partslength; i++) {
		delete[] parts[i];
	}
	delete[] parts;
#endif
	return dbversion;
}

const char *postgresqlconnection::dbHostName() {
	const char	*dbhostname=sqlrserverconnection::dbHostName();
	if (charstring::length(dbhostname)) {
		return dbhostname;
	}
	if (!hostname) {
		hostname=sys::getHostName();
	}
	return hostname;
}

const char *postgresqlconnection::dbIpAddressQuery() {
	return "select inet_server_addr()";
}

const char *postgresqlconnection::dbIpAddress() {
	const char	*ipaddress=sqlrserverconnection::dbIpAddress();
	return (charstring::length(ipaddress))?ipaddress:"127.0.0.1";
}

const char *postgresqlconnection::getDatabaseListQuery(bool wild) {
	return (wild)?
		"select "
		"	datname, "
		"	NULL "
		"from "
		"	pg_database "
		"where "
		"	datname like '%s' "
		"order by "
		"	datname":

		"select "
		"	datname, "
		"	NULL "
		"from "
		"	pg_database "
		"order by "
		"	datname";
}

const char *postgresqlconnection::getTableListQuery(bool wild,
						uint16_t objecttypes) {
	return sqlrserverconnection::getTableListQuery(wild,objecttypes,
					" and table_schema = 'public' ");
}

const char *postgresqlconnection::getColumnListQuery(
					const char *table, bool wild) {
	return (wild)?
		"select "
		"	table_catalog as table_cat, "
		"	table_schema as table_schem, "
		"	table_name as table_name, "
		"	column_name, "
		"	null as data_type, " // case this...
		"	data_type as type_name, "
		"	case "
		"		when numeric_scale is null "
		"			then character_maximum_length "
		"		else numeric_precision "
		"	end as column_size, "
		"	null as buffer_length, "
			// length in bytes of data transferred during fetch
		"	numeric_scale as decimal_digits, "
		"	numeric_precision_radix as num_prec_radix, "
		"	case "
		"		when is_nullable = 'NO' "
		"			then 0 "
		"		when is_nullable = 'YES' "
		"			then 1 "
		"		else 2 "
		"	end as nullable, "
		"	null as remarks, "
		"	column_default, "
		"	null as sql_data_type, "
			// type (int)
		"	null as sql_datetime_sub, "
			// subtype (int) for datetime/interval, otherwise null
		"	character_octet_length as char_octet_length, "
		"	ordinal_position, "
		"	is_nullable, "
		"	null as extra "
		"from "
		"	information_schema.columns "
		"where "
		"	table_schema='public' "
		"	and "
		"	table_name='%s' "
		"	and "
		"	column_name like '%s' "
		"order by "
		"	ordinal_position":

		"select "
		"	table_catalog as table_cat, "
		"	table_schema as table_schem, "
		"	table_name as table_name, "
		"	column_name, "
		"	null as data_type, " // case this...
		"	data_type as type_name, "
		"	case "
		"		when numeric_scale is null "
		"			then character_maximum_length "
		"		else numeric_precision "
		"	end as column_size, "
		"	null as buffer_length, "
		"	numeric_scale as decimal_digits, "
		"	numeric_precision_radix as num_prec_radix, "
		"	case "
		"		when is_nullable = 'NO' "
		"			then 0 "
		"		when is_nullable = 'YES' "
		"			then 1 "
		"		else 2 "
		"	end as nullable, "
		"	null as remarks, "
		"	column_default, "
		"	null as sql_data_type, "
		"	null as sql_datetime_sub, "
		"	character_octet_length as char_octet_length, "
		"	ordinal_position, "
		"	is_nullable, "
		"	null as extra "
		"from "
		"	information_schema.columns "
		"where "
		"	table_schema='public' "
		"	and "
		"	table_name='%s' "
		"order by "
		"	ordinal_position";
}

bool postgresqlconnection::selectDatabase(const char *database) {

	cont->clearError();

	// log out and log back in to the specified database
	logOut();
	const char	*error=NULL;
	const char	*warning=NULL;
	if (!logIn(&error,&warning,database)) {

		// Set the error, but don't use the error that was returned
		// from logIn() because it will have a message prepended to it.
		// Also, we can't get the message from PQerrorMessage, because
		// if PQconnect fails then pgconn will be NULL and
		// PQerrorMessage will just return a message saying that it's
		// NULL.  So, we'll just return the generic SQL Relay error
		// for these kinds of things.
		cont->setError(SQLR_ERROR_DBNOTFOUND_STRING,
				SQLR_ERROR_DBNOTFOUND,true);

		// log back in to the original database, we'll assume that works
		logOut();
		logIn(&error,&warning);
		return false;
	}
	return true;
}

const char *postgresqlconnection::getCurrentDatabaseQuery() {
	return "select current_database()";
}

bool postgresqlconnection::getLastInsertId(uint64_t *id) {
#ifdef HAVE_POSTGRESQL_PQOIDVALUE
	if (lastinsertidquery) {
		return sqlrserverconnection::getLastInsertId(id);
	}
	*id=(currentoid!=InvalidOid)?currentoid:0;
	return true;
#else
	return false;
#endif
}

const char *postgresqlconnection::getLastInsertIdQuery() {
	return lastinsertidquery;
}

const char *postgresqlconnection::noopQuery() {
	return "do language plpgsql $$declare dummy int; begin end$$";
}

const char *postgresqlconnection::bindFormat() {
#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
	return "$1";
#else
	return sqlrserverconnection::bindFormat();
#endif
}

postgresqlcursor::postgresqlcursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	postgresqlconn=(postgresqlconnection *)conn;
	pgresult=NULL;
#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
	maxbindcount=conn->cont->getConfig()->getMaxBindCount();
	bindvalues=new char *[maxbindcount];
	bytestring::zero(bindvalues,maxbindcount*sizeof(char *));
	bindlengths=new int[maxbindcount];
	bindformats=new int[maxbindcount];
	bindcounter=0;
	bindformaterror=false;
#endif
#if defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE)
	justexecuted=false;
#endif
}

postgresqlcursor::~postgresqlcursor() {
#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
	for (uint16_t i=0; i<bindcounter; i++) {
		delete[] bindvalues[i];
	}
	delete[] bindvalues;
	delete[] bindlengths;
	delete[] bindformats;
#endif
}

#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
bool postgresqlcursor::prepareQuery(const char *query, uint32_t length) {

	// initialize the column count
	ncols=0;

	// reset bind counter
	bindcounter=0;

	// reset the bind format error flag
	bindformaterror=false;

	// prepare the query
	pgresult=PQprepare(postgresqlconn->pgconn,"",query,0,NULL);

	// handle some kind of outright failure
	if (!pgresult) {
		return false;
	}

	// handle errors
	bool	result=true;
	pgstatus=PQresultStatus(pgresult);
	if (pgstatus==PGRES_BAD_RESPONSE ||
		pgstatus==PGRES_NONFATAL_ERROR ||
		pgstatus==PGRES_FATAL_ERROR) {
		result=false;
	}

	// clean up
	PQclear(pgresult);
	pgresult=NULL;

	return result;
}

bool postgresqlcursor::inputBind(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {

	// "variable" should be something like ?1,?2,?3, etc.
	// If it's something like ?var1,?var2,?var3, etc. then it'll be
	// converted to 0.  1 will be subtracted and after the cast it will
	// be converted to 65535 and will cause the if below to fail.
	uint16_t	pos=charstring::toInteger(variable+1)-1;

	// validate bind index
	if (pos>=maxbindcount) {
		bindformaterror=true;
		return true;
	}

	if (*isnull) {
		bindvalues[pos]=NULL;
		bindlengths[pos]=0;
	} else {
		bindvalues[pos]=charstring::duplicate(value,valuesize);
		bindlengths[pos]=valuesize;
	}
	bindformats[pos]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBind(const char *variable, 
					uint16_t variablesize,
					int64_t *value) {

	// "variable" should be something like ?1,?2,?3, etc.
	// If it's something like ?var1,?var2,?var3, etc. then it'll be
	// converted to 0.  1 will be subtracted and after the cast it will
	// be converted to 65535 and will cause the if below to fail.
	uint16_t	pos=charstring::toInteger(variable+1)-1;

	// validate bind index
	if (pos>=maxbindcount) {
		bindformaterror=true;
		return true;
	}

	bindvalues[pos]=charstring::parseNumber(*value);
	bindlengths[pos]=charstring::length(bindvalues[pos]);
	bindformats[pos]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBind(const char *variable, 
					uint16_t variablesize,
					double *value,
					uint32_t precision,
					uint32_t scale) {

	// "variable" should be something like ?1,?2,?3, etc.
	// If it's something like ?var1,?var2,?var3, etc. then it'll be
	// converted to 0.  1 will be subtracted and after the cast it will
	// be converted to 65535 and will cause the if below to fail.
	uint16_t	pos=charstring::toInteger(variable+1)-1;

	// validate bind index
	if (pos>=maxbindcount) {
		bindformaterror=true;
		return true;
	}

	bindvalues[pos]=charstring::parseNumber(*value,precision,scale);
	bindlengths[pos]=charstring::length(bindvalues[pos]);
	bindformats[pos]=0;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBindBlob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {

	// "variable" should be something like ?1,?2,?3, etc.
	// If it's something like ?var1,?var2,?var3, etc. then it'll be
	// converted to 0.  1 will be subtracted and after the cast it will
	// be converted to 65535 and will cause the if below to fail.
	uint16_t	pos=charstring::toInteger(variable+1)-1;

	// validate bind index
	if (pos>=maxbindcount) {
		bindformaterror=true;
		return true;
	}

	if (*isnull) {
		bindvalues[pos]=NULL;
		bindlengths[pos]=0;
	} else {
		bindvalues[pos]=static_cast<char *>
				(bytestring::duplicate(value,valuesize));
		bindlengths[pos]=valuesize;
	}
	bindformats[pos]=1;
	bindcounter++;
	return true;
}

bool postgresqlcursor::inputBindClob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {

	// "variable" should be something like ?1,?2,?3, etc.
	// If it's something like ?var1,?var2,?var3, etc. then it'll be
	// converted to 0.  1 will be subtracted and after the cast it will
	// be converted to 65535 and will cause the if below to fail.
	uint16_t	pos=charstring::toInteger(variable+1)-1;

	// validate bind index
	if (pos>=maxbindcount) {
		bindformaterror=true;
		return true;
	}

	if (*isnull) {
		bindvalues[pos]=NULL;
		bindlengths[pos]=0;
	} else {
		bindvalues[pos]=charstring::duplicate(value,valuesize);
		bindlengths[pos]=valuesize;
	}
	bindformats[pos]=0;
	bindcounter++;
	return true;
}
#endif

bool postgresqlcursor::supportsNativeBinds(const char *query, uint32_t length) {
#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
	return true;
#else
	return false;
#endif
}

void postgresqlcursor::encodeBlob(stringbuffer *buffer,
					const char *data, uint32_t datasize) {

	// postgresql wants non-printable characters converted to octal with
	// a preceeding slash
	// postgresql also wants it to be quoted

	buffer->append("'");
	for (uint32_t i=0; i<datasize; i++) {
		if (data[i]<' ' || data[i]>'~' ||
			data[i]=='\'' || data[i]=='\\') {
			buffer->append('\\');
			buffer->append(conn->cont->asciiToOctal(data[i]));
		} else {
			buffer->append(data[i]);
		}
	}
	buffer->append("'");
}

bool postgresqlcursor::executeQuery(const char *query, uint32_t length) {

	// initialize the row counts
	nrows=0;
	currentrow=-1;

#if defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE)
	int	result=1;
	if (bindcounter) {
		result=PQsendQueryPrepared(postgresqlconn->pgconn,"",
						bindcounter,bindvalues,
						bindlengths,bindformats,0);
		bindcounter=0;
	} else {
		result=PQsendQuery(postgresqlconn->pgconn,query);
	}

	// handle some kind of outright failure
	if (!result) {
		return false;
	}

	// set single-row mode
	if (!PQsetSingleRowMode(postgresqlconn->pgconn)) {
		return false;
	}

	// get the result (and the first row)
	pgresult=PQgetResult(postgresqlconn->pgconn);

	justexecuted=true;
	currentrow=0;
#elif defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
	if (bindcounter) {
		pgresult=PQexecPrepared(postgresqlconn->pgconn,"",
					bindcounter,bindvalues,
					bindlengths,bindformats,0);
		bindcounter=0;
	} else {
		pgresult=PQexec(postgresqlconn->pgconn,query);
	}
#else
	pgresult=PQexec(postgresqlconn->pgconn,query);
#endif

	// handle some kind of outright failure
	if (!pgresult) {
		return false;
	}

	// handle errors
	ExecStatusType	pgstatus=PQresultStatus(pgresult);
	if (pgstatus==PGRES_BAD_RESPONSE ||
		pgstatus==PGRES_NONFATAL_ERROR ||
		pgstatus==PGRES_FATAL_ERROR) {
		return false;
	}

	// get the col count
	ncols=PQnfields(pgresult);

	// validate column count
	uint32_t	maxcolumncount=conn->cont->getMaxColumnCount();
	if (maxcolumncount && (uint32_t)ncols>maxcolumncount) {
		stringbuffer	err;
		err.append(SQLR_ERROR_MAXSELECTLIST_STRING);
		err.append(" (")->append(ncols)->append('>');
		err.append(maxcolumncount);
		err.append(')');
		conn->cont->setError(this,err.getString(),
					SQLR_ERROR_MAXSELECTLIST,true);
		return false;
	}

	checkForTempTable(query,length);

#if !(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
	// get the row count
	nrows=PQntuples(pgresult);
#endif

	// get the affected row count
	const char	*affrows=PQcmdTuples(pgresult);
	affectedrows=0;
	if (!charstring::isNullOrEmpty(affrows)) {
		affectedrows=charstring::toInteger(affrows);
	}

#ifdef HAVE_POSTGRESQL_PQOIDVALUE
	// get the oid of the inserted row (if this was an insert)
	Oid	coid=PQoidValue(pgresult);
	if (coid!=InvalidOid) {
		postgresqlconn->currentoid=coid;
	}
#endif

	// force re-fetch of column info
	setResultSetHeaderHasBeenHandled(false);

	return true;
}

#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
void postgresqlcursor::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	const char	*errorstring=
			(bindformaterror)?
				SQLR_ERROR_INVALIDBINDVARIABLEFORMAT_STRING:
				PQerrorMessage(postgresqlconn->pgconn);
	*errorlength=charstring::length(errorstring);
	charstring::safeCopy(errorbuffer,errorbufferlength,
					errorstring,*errorlength);
	// PostgreSQL doesn't have an error number per-se.  We'll set it
	// to 1 though, because 0 typically means "no error has occurred"
	// and some apps respond that way if errorcode is set to 0.
	// This ends up being important when using:
	// Oracle dblink -> ODBC -> SQL Relay -> PostgreSQL
	*errorcode=(bindformaterror)?SQLR_ERROR_INVALIDBINDVARIABLEFORMAT:1;
	*liveconnection=(PQstatus(postgresqlconn->pgconn)==CONNECTION_OK);
}
#endif

bool postgresqlcursor::knowsRowCount() {
#if defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE)
	return false;
#else
	return true;
#endif
}

uint64_t postgresqlcursor::rowCount() {
	return nrows;
}

uint64_t postgresqlcursor::affectedRows() {
	return affectedrows;
}

uint32_t postgresqlcursor::colCount() {
	return ncols;
}

uint16_t postgresqlcursor::columnTypeFormat() {
	if (postgresqlconn->typemangling==1) {
		return (uint16_t)COLUMN_TYPE_IDS;
	} else {
		return (uint16_t)COLUMN_TYPE_NAMES;
	}
}

const char *postgresqlcursor::getColumnName(uint32_t col) {
	return PQfname(pgresult,col);
}

uint16_t postgresqlcursor::getColumnType(uint32_t col) {
	// Types are strange in POSTGRESQL, there are no actual
	// types, only internal numbers that correspond to 
	// types which are defined in a database table 
	// somewhere.
	// If typemangling is turned on, translate to standard
	// types, otherwise return the type number.
	switch (PQftype(pgresult,col)) {
		case 16: //bool
			return BOOL_DATATYPE;
		case 17: //bytea
			return BYTEA_DATATYPE;
		case 18: //char
			return CHAR_DATATYPE;
		case 19: //name
			return NAME_DATATYPE;
		case 20: //int8
			return INT8_DATATYPE;
		case 21: //int2
			return INT2_DATATYPE;
		case 22: //int2vector
			return INT2VECTOR_DATATYPE;
		case 23: //int4
			return INT4_DATATYPE;
		case 24: //regproc
			return REGPROC_DATATYPE;
		case 25: //text
			return TEXT_DATATYPE;
		case 26: //oid
			return OID_DATATYPE;
		case 27: //tid
			return TID_DATATYPE;
		case 28: //xid
			return XID_DATATYPE;
		case 29: //cid
			return CID_DATATYPE;
		case 30: //oidvector
			return OIDVECTOR_DATATYPE;
		case 71: //pg_type
			return PG_TYPE_DATATYPE;
		case 75: //pg_attribute
			return PG_ATTRIBUTE_DATATYPE;
		case 81: //pg_proc
			return PG_PROC_DATATYPE;
		case 83: //pg_class
			return PG_CLASS_DATATYPE;
		case 210: //smgr
			return SMGR_DATATYPE;
		case 600: //point
			return POINT_DATATYPE;
		case 601: //lseg
			return LSEG_DATATYPE;
		case 602: //path
			return PATH_DATATYPE;
		case 603: //box
			return BOX_DATATYPE;
		case 604: //polygon
			return POLYGON_DATATYPE;
		case 628: //line
			return LINE_DATATYPE;
		case 629: //_line
			return _LINE_DATATYPE;
		case 651: //_cidr
			return _CIDR_DATATYPE;
		case 700: //float4
			return FLOAT4_DATATYPE;
		case 701: //float8
			return FLOAT8_DATATYPE;
		case 702: //abstime
			return ABSTIME_DATATYPE;
		case 703: //reltime
			return RELTIME_DATATYPE;
		case 704: //tinterval
			return TINTERVAL_DATATYPE;
		case 718: //circle
			return CIRCLE_DATATYPE;
		case 719: //_circle
			return _CIRCLE_DATATYPE;
		case 790: //money
			return MONEY_DATATYPE;
		case 791: //_money
			return _MONEY_DATATYPE;
		case 829: //macaddr
			return MACADDR_DATATYPE;
		case 869: //inet
			return INET_DATATYPE;
		case 650: //cidr
			return CIDR_DATATYPE;
		case 1000: //_bool
			return _BOOL_DATATYPE;
		case 1001: //_bytea
			return _BYTEA_DATATYPE;
		case 1002: //_char
			return _CHAR_DATATYPE;
		case 1003: //_name
			return _NAME_DATATYPE;
		case 1005: //_int2
			return _INT2_DATATYPE;
		case 1006: //_int2vector
			return _INT2VECTOR_DATATYPE;
		case 1007: //_int4
			return _INT4_DATATYPE;
		case 1008: //_regproc
			return _REGPROC_DATATYPE;
		case 1009: //_text
			return _TEXT_DATATYPE;
		case 1010: //_tid
			return _TID_DATATYPE;
		case 1011: //_xid
			return _XID_DATATYPE;
		case 1012: //_cid
			return _CID_DATATYPE;
		case 1013: //_oidvector
			return _OIDVECTOR_DATATYPE;
		case 1014: //_bpchar
			return _BPCHAR_DATATYPE;
		case 1015: //_varchar
			return _VARCHAR_DATATYPE;
		case 1016: //_int8
			return _INT8_DATATYPE;
		case 1017: //_point
			return _POINT_DATATYPE;
		case 1018: //_lseg
			return _LSEG_DATATYPE;
		case 1019: //_path
			return _PATH_DATATYPE;
		case 1020: //_box
			return _BOX_DATATYPE;
		case 1021: //_float4
			return _FLOAT4_DATATYPE;
		case 1022: //_float8
			return _FLOAT8_DATATYPE;
		case 1023: //_abstime
			return _ABSTIME_DATATYPE;
		case 1024: //_reltime
			return _RELTIME_DATATYPE;
		case 1025: //_tinterval
			return _TINTERVAL_DATATYPE;
		case 1027: //_polygon
			return _POLYGON_DATATYPE;
		case 1028: //_oid
			return _OID_DATATYPE;
		case 1033: //aclitem
			return ACLITEM_DATATYPE;
		case 1034: //_aclitem
			return _ACLITEM_DATATYPE;
		case 1040: //_macaddr
			return _MACADDR_DATATYPE;
		case 1041: //_inet
			return _INET_DATATYPE;
		case 1042: //bpchar
			return BPCHAR_DATATYPE;
		case 1043: //varchar
			return VARCHAR_DATATYPE;
		case 1082: //date
			return DATE_DATATYPE;
		case 1083: //time
			return TIME_DATATYPE;
		case 1114: //timestamp
		case 1296:
			return TIMESTAMP_DATATYPE;
		case 1115: //_timestamp
			return _TIMESTAMP_DATATYPE;
		case 1182: //_date
			return _DATE_DATATYPE;
		case 1183: //_time
			return _TIME_DATATYPE;
		case 1184: //timestamptz
			return TIMESTAMPTZ_DATATYPE;
		case 1185: //_timestamptz
			return _TIMESTAMPTZ_DATATYPE;
		case 1186: //interval
			return INTERVAL_DATATYPE;
		case 1187: //_interval
			return _INTERVAL_DATATYPE;
		case 1231: //_numeric
			return _NUMERIC_DATATYPE;
		case 1266: //timetz
			return TIMETZ_DATATYPE;
		case 1270: //_timetz
			return _TIMETZ_DATATYPE;
		case 1560: //bit
			return BIT_DATATYPE;
		case 1561: //_bit
			return _BIT_DATATYPE;
		case 1562: //varbit
			return VARBIT_DATATYPE;
		case 1563: //_varbit
			return _VARBIT_DATATYPE;
		case 1700: //numeric
			return NUMERIC_DATATYPE;
		case 1790: //refcursor
			return REFCURSOR_DATATYPE;
		case 2201: //_refcursor
			return _REFCURSOR_DATATYPE;
		case 2202: //regprocedure
			return REGPROCEDURE_DATATYPE;
		case 2203: //regoper
			return REGOPER_DATATYPE;
		case 2204: //regoperator
			return REGOPERATOR_DATATYPE;
		case 2205: //regclass
			return REGCLASS_DATATYPE;
		case 2206: //regtype
			return REGTYPE_DATATYPE;
		case 2207: //_regprocedure
			return _REGPROCEDURE_DATATYPE;
		case 2208: //_regoper
			return _REGOPER_DATATYPE;
		case 2209: //_regoperator
			return _REGOPERATOR_DATATYPE;
		case 2210: //_regclass
			return _REGCLASS_DATATYPE;
		case 2211: //_regtype
			return _REGTYPE_DATATYPE;
		case 2249: //record
			return RECORD_DATATYPE;
		case 2275: //cstring
			return CSTRING_DATATYPE;
		case 2276: //any
			return ANY_DATATYPE;
		case 2277: //anyarray
			return ANYARRAY_DATATYPE;
		case 2278: //void
			return VOID_DATATYPE;
		case 2279: //trigger
			return TRIGGER_DATATYPE;
		case 2280: //language_handler
			return LANGUAGE_HANDLER_DATATYPE;
		case 2281: //internal
			return INTERNAL_DATATYPE;
		case 2282: //opaque
			return OPAQUE_DATATYPE;
		case 2283: //anyelement
			return ANYELEMENT_DATATYPE;
		case 705: //unknown
		default:
			return UNKNOWN_DATATYPE;
	}
}

const char *postgresqlcursor::getColumnTypeName(uint32_t col) {
	// Types are strange in POSTGRESQL, there are no actual
	// types, only internal numbers that correspond to 
	// types which are defined in a database table 
	// somewhere.
	// typemangling=0 means return the internal number as a string
	// typemangling=1 means translate to a standard datatype
	// 			(handled by getColumnType above)
	// typemangling=2 means return the name as a string
	Oid	pgfieldtype=PQftype(pgresult,col);
	if (!postgresqlconn->typemangling) {
		charstring::printf(typenamebuffer,sizeof(typenamebuffer),
						"%d",(int32_t)pgfieldtype);
		return typenamebuffer;
	}
	return postgresqlconn->datatypes.getValue((int32_t)pgfieldtype);
}

uint32_t postgresqlcursor::getColumnLength(uint32_t col) {
	int32_t	size=PQfsize(pgresult,col);
#ifdef HAVE_POSTGRESQL_PQFMOD
	if (size<0) {
		size=PQfmod(pgresult,col);
	}
#endif
	if (size<0) {
		size=0;
	}
	return size;
}

uint16_t postgresqlcursor::getColumnIsBinary(uint32_t col) {
	// is this binary data (all columns will contain binary data if it is)
	int16_t	binary=false;
#ifdef HAVE_POSTGRESQL_PQBINARYTUPLES
	binary=PQbinaryTuples(pgresult);
#endif
	return binary;
}

#ifdef HAVE_POSTGRESQL_PQFTABLE
const char *postgresqlcursor::getColumnTable(uint32_t col) {
	// PQftable returns an oid rather than a table name, so we have to map
	// it to a table name.
	// tablemangling=0 means return the internal number as a string
	// tablemangling=2 means return the name as a string
	Oid	pgfieldtable=PQftable(pgresult,col);
	if (!postgresqlconn->tablemangling) {
		charstring::printf(tablenamebuffer,sizeof(tablenamebuffer),
						"%d",(int32_t)pgfieldtable);
		return tablenamebuffer;
	}
	return postgresqlconn->tables.getValue((int32_t)pgfieldtable);
}
#endif

bool postgresqlcursor::noRowsToReturn() {
#if defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE)
	// if there are no columns, then there can't be any rows either
	return (ncols)?false:true;
#else
	return (!nrows);
#endif
}

bool postgresqlcursor::fetchRow(bool *error) {

	*error=false;
	// FIXME: set error if an error occurs

#if defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE)
	if (!justexecuted) {
		PQclear(pgresult);
		pgresult=PQgetResult(postgresqlconn->pgconn);
	} else {
		justexecuted=false;
	}
	// The docs say call PQgetResult until it returns null, but it will
	// actually return non-null one time when called after the end of the
	// result set.  Fortunately, we can detect the true end with
	// PQresultStatus.
	if (PQresultStatus(pgresult)==PGRES_SINGLE_TUPLE && pgresult) {
		return true;
	}
	return false;
#else
	if (currentrow<nrows-1) {
		currentrow++;
		return true;
	}
	return false;
#endif
}

void postgresqlcursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {

	// handle NULLs
	if (PQgetisnull(pgresult,currentrow,col)) {
		*null=true;
		return;
	}

	// handle normal datatypes
	*field=PQgetvalue(pgresult,currentrow,col);
	*fieldlength=PQgetlength(pgresult,currentrow,col);
}

void postgresqlcursor::closeResultSet() {

#if (defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)) || \
		(defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE))
	for (uint16_t i=0; i<bindcounter; i++) {
		delete[] bindvalues[i];
		bindvalues[i]=NULL;
	}
#endif

#if defined(HAVE_POSTGRESQL_PQSENDQUERYPREPARED) && \
		defined(HAVE_POSTGRESQL_PQSETSINGLEROWMODE)
	for (;;) {
		if (pgresult) {
			PQclear(pgresult);
		} else {
			break;
		}
		pgresult=PQgetResult(postgresqlconn->pgconn);
	}
	justexecuted=false;
#else
	if (pgresult) {
		PQclear(pgresult);
		pgresult=NULL;
	}
#endif
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_postgresqlconnection(
						sqlrservercontroller *cont) {
		return new postgresqlconnection(cont);
	}
}
