// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/regularexpression.h>
#include <rudiments/sys.h>

#include <datatypes.h>
#include <defines.h>
#include <config.h>

extern "C" {
#ifdef SQLITE3
	#include <sqlite3.h>
#else
	#include <sqlite.h>
#endif
}

#ifndef SQLITE3
	#define	sqlite3_open			sqlite_open
	#define	sqlite3_close			sqlite_close
	#define	sqlite3_get_table		sqlite_get_table
	#define	sqlite3_errmsg			sqlite_errmsg
	#define	sqlite3_free_table		sqlite_free_table
	#define	sqlite3_last_insert_rowid	sqlite_last_insert_rowid
	#define sqlite3_free(mem)		sqlite_free((char *)mem)
#endif

#ifndef HAVE_SQLITE3_MALLOC
	#include <stdlib.h>
	#define sqlite3_malloc			malloc
#endif

class SQLRSERVER_DLLSPEC sqliteconnection : public sqlrserverconnection {
	friend class sqlitecursor;
	public:
				sqliteconnection(sqlrservercontroller *cont);
				~sqliteconnection();
	private:
		void		handleConnectString();
		bool		logIn(const char **error, const char **warning);
		sqlrservercursor	*newCursor(uint16_t id);
		void		deleteCursor(sqlrservercursor *curs);
		void		logOut();
		bool		ping();
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild,
						uint16_t objecttypes);
		const char	*getColumnListQuery(
						const char *table, bool wild);
		#ifdef SQLITE_TRANSACTIONAL
		const char	*setIsolationLevelQuery();
		#endif
		bool		selectDatabase(const char *database);
		char		*getCurrentDatabase();
		bool		getLastInsertId(uint64_t *id);
		const char	*noopQuery();
		#ifdef SQLITE3
		char		*duplicate(const char *str);
		#endif
		#ifndef SQLITE_TRANSACTIONAL
		bool		isTransactional();
		bool		commit();
		bool		rollback();
		#endif
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);

		void		clearErrors();

		char		*db;

		const char	*identity;

		#ifdef SQLITE3
		sqlite3	*sqliteptr;
		#else
		sqlite	*sqliteptr;
		#endif
		char	*errmesg;
		int64_t	errcode;

		char	*hostname;
};

class SQLRSERVER_DLLSPEC sqlitecursor : public sqlrservercursor {
	friend class sqliteconnection;
	private:
				sqlitecursor(sqlrserverconnection *conn,
								uint16_t id);
				~sqlitecursor();

		bool		supportsNativeBinds(const char *query,
							uint32_t length);

		#ifdef HAVE_SQLITE3_STMT
		bool		prepareQuery(const char *query,
						uint32_t length);
		int32_t		getBindVariableIndex(
						const char *variable,
						uint16_t variablesize);
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
		#endif
		bool		executeQuery(const char *query,
						uint32_t length);
		int		runQuery(const char *query);
		void		selectLastInsertRowId();
		bool		knowsRowCount();
		uint64_t	rowCount();
		bool		knowsAffectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		#ifdef HAVE_SQLITE3_STMT
		uint16_t	getColumnType(uint32_t col);
		const char	*getColumnTable(uint32_t col);
		#endif
		bool		noRowsToReturn();
		bool		skipRow(bool *error);
		bool		fetchRow(bool *error);
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		closeResultSet();

		char		**columnnames;
		int		ncolumn;
		int		nrow;
		bool		lastinsertrowid;

		#ifdef HAVE_SQLITE3_STMT
		char		**columntables;
		int		*columntypes;
		sqlite3_stmt	*stmt;
		bool		justexecuted;
		char		*lastinsertrowidstr;
		#else
		char		**result;
		int		rowindex;
		#endif

		regularexpression	selectlastinsertrowid;

		sqliteconnection	*sqliteconn;
};


sqliteconnection::sqliteconnection(sqlrservercontroller *cont) :
					sqlrserverconnection(cont) {
	identity=NULL;
	sqliteptr=NULL;
	errmesg=NULL;
	errcode=0;
	hostname=NULL;
	db=NULL;
}

sqliteconnection::~sqliteconnection() {
	clearErrors();
	delete[] hostname;
	delete[] db;
}

void sqliteconnection::handleConnectString() {
	db=charstring::duplicate(cont->getConnectStringValue("db"));
	identity=cont->getConnectStringValue("identity");

	cont->setFetchAtOnce(1);
	cont->setMaxColumnCount(0);
	cont->setMaxFieldLength(0);
}

bool sqliteconnection::logIn(const char **error, const char **warning) {
#ifdef SQLITE_TRANSACTIONAL
	#ifdef SQLITE3
		if (sqlite3_open(db,&sqliteptr)==SQLITE_OK) {
			return true;
		}
		errmesg=duplicate(sqlite3_errmsg(sqliteptr));
		errcode=sqlite3_errcode(sqliteptr);
	#else
		if ((sqliteptr=sqlite3_open(db,666,&errmesg))) {
			return true;
		}
	#endif
	if (errmesg) {
		*error=errmesg;
	}
	return false;
#else
	return true;
#endif
}

sqlrservercursor *sqliteconnection::newCursor(uint16_t id) {
	return (sqlrservercursor *)new sqlitecursor(
					(sqlrserverconnection *)this,id);
}

void sqliteconnection::deleteCursor(sqlrservercursor *curs) {
	delete (sqlitecursor *)curs;
}

void sqliteconnection::logOut() {
	#ifdef SQLITE_TRANSACTIONAL
	if (sqliteptr) {
		sqlite3_close(sqliteptr);
	}
	#endif
}

bool sqliteconnection::ping() {
	return true;
}

const char *sqliteconnection::identify() {
	return (identity)?identity:"sqlite";
}

const char *sqliteconnection::dbVersion() {
	#ifdef SQLITE_VERSION
	return SQLITE_VERSION;
	#else
	return "unknown";
	#endif
}

const char *sqliteconnection::dbHostName() {
	if (!hostname) {
		hostname=sys::getHostName();
	}
	return hostname;
}

const char *sqliteconnection::getDatabaseListQuery(bool wild) {
	//return "pragma database_list";
	return "select '',NULL";
}

const char *sqliteconnection::getTableListQuery(bool wild,
						uint16_t objecttypes) {
	return (wild)?
		"select "
		"	NULL as table_cat, "
		"	NULL as table_schem, "
		"	tbl_name as table_name, "
		"	'TABLE' as table_type, "
		"	NULL as remarks, "
		"	NULL as extra "
		"from "
		"(select "
		"	tbl_name "
		"from "
		"	sqlite_master "
		"where "
		"	type in ('table','view') "
		"union all "
		"select "
		"	tbl_name "
		"from "
		"	sqlite_temp_master "
		"where "
		"	type in ('table','view')) "
		"where "
		"	tbl_name like '%s' "
		"order by "
		"	tbl_name":

		"select "
		"	NULL as table_cat, "
		"	NULL as table_schem, "
		"	tbl_name as table_name, "
		"	'TABLE' as table_type, "
		"	NULL as remarks, "
		"	NULL as extra "
		"from "
		"(select "
		"	tbl_name "
		"from "
		"	sqlite_master "
		"where "
		"	type in ('table','view') "
		"union all "
		"select "
		"	tbl_name "
		"from "
		"	sqlite_temp_master "
		"where "
		"	type in ('table','view')) "
		"order by "
		"	tbl_name";
}

const char *sqliteconnection::getColumnListQuery(
					const char *table, bool wild) {
	return "select "
		"	'' as column_name, "
		"	'' as data_type, "
		"	'' as length, "
		"	'' as precision, "
		"	'' as scale, "
		"	'' as nullable, "
		"	'' as key, "
		"	'' as column_default, "
		"	'' as extra, "
		"	NULL";
}

#ifdef SQLITE_TRANSACTIONAL
const char *sqliteconnection::setIsolationLevelQuery() {
	return "pragma %s";
}
#endif

bool sqliteconnection::selectDatabase(const char *database) {

	// keep track of the original db and host
	char	*originaldb=db;

	// reset the db/host
	db=charstring::duplicate(database);

	cont->clearError();

	// log out and log back in to the specified database
	logOut();
	const char	*error;
	const char	*warning;
	if (!logIn(&error,&warning)) {

		// Set the error.  We can't get the message from sqlite3_errmsg,
		// because sqliteptr will be NULL.  So, we'll just return the
		// generic SQL Relay error for these kinds of things.
		cont->setError(SQLR_ERROR_DBNOTFOUND_STRING,
				SQLR_ERROR_DBNOTFOUND,true);

		// log back in to the original database, we'll assume that works
		delete[] db;
		db=originaldb;
		logOut();
		logIn(&error,&warning);
		return false;
	}

	// clean up
	delete[] originaldb;
	return true;
}

char *sqliteconnection::getCurrentDatabase() {
	return charstring::duplicate(db);
}

bool sqliteconnection::getLastInsertId(uint64_t *id) {
	*id=sqlite3_last_insert_rowid(sqliteptr);
	return true;
}

const char *sqliteconnection::noopQuery() {
	return "pragma noop";
}

#ifndef SQLITE_TRANSACTIONAL
bool sqliteconnection::isTransactional() {
	return false;
}

bool sqliteconnection::commit() {
	return true;
}

bool sqliteconnection::rollback() {
	return true;
}
#endif

void sqliteconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	// set return values
	*errorlength=charstring::length(errmesg);
	charstring::safeCopy(errorbuffer,errorbufferlength,
					errmesg,*errorlength);
	*errorcode=errcode;
	*liveconnection=true;
	if (errmesg &&
		(!charstring::compare(errmesg,"access permission denied",24) ||
		!charstring::compare(errmesg,"not a directory",15))) {
		*liveconnection=false;
	}
}

void sqliteconnection::clearErrors() {
	if (errmesg) {
		#ifdef HAVE_SQLITE3_FREE_WITH_CHAR
			sqlite3_free(errmesg);
		#else
			sqlite3_free((void *)errmesg);
		#endif
		errmesg=NULL;
		errcode=0;
	}
}

sqlitecursor::sqlitecursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {

	columnnames=NULL;
	ncolumn=0;
	nrow=0;
	lastinsertrowid=false;
	#ifdef HAVE_SQLITE3_STMT
	columntables=NULL;
	columntypes=NULL;
	stmt=NULL;
	justexecuted=false;
	lastinsertrowidstr=NULL;
	#else
	rowindex=0;
	result=NULL;
	#endif

	sqliteconn=(sqliteconnection *)conn;

	selectlastinsertrowid.setPattern("^[ 	\r\n]*(select|SELECT)[ 	\r\n]+"
				"(last|LAST)[ 	\r\n]+(insert|INSERT)[ 	\r\n]+"
				"(rowid|ROWID)");
	selectlastinsertrowid.study();
}

sqlitecursor::~sqlitecursor() {

	// clean up old column names
	if (columnnames) {
		for (int i=0; i<ncolumn; i++) {
			delete[] columnnames[i];
		}
		delete[] columnnames;
	}

	#ifdef HAVE_SQLITE3_STMT
	// clean up old column tables
	if (columntables) {
		for (int i=0; i<ncolumn; i++) {
			delete[] columntables[i];
		}
		delete[] columntables;
	}

	// clean up old column types
	if (columntypes) {
		delete[] columntypes;
	}
	#endif

	closeResultSet();
	#ifdef HAVE_SQLITE3_STMT
	sqlite3_finalize(stmt);
	delete[] lastinsertrowidstr;
	#endif
}

bool sqlitecursor::supportsNativeBinds(const char *query, uint32_t length) {
	#ifdef HAVE_SQLITE3_STMT
	return true;
	#else
	return false;
	#endif
}

#ifdef HAVE_SQLITE3_STMT
bool sqlitecursor::prepareQuery(const char *query, uint32_t length) {

	// reinit justexecuted flag
	justexecuted=false;

	// initialize column count
	ncolumn=0;

	// clear any errors
	sqliteconn->clearErrors();

	// don't prepare "select last insert rowid" queries
	if (selectlastinsertrowid.match(query)) {
		return true;
	}

	// completely reset the statement
	sqlite3_finalize(stmt);

	// prepare the query
	// try again if it fails with SQLITE_SCHEMA
	int	res=SQLITE_SCHEMA;
	while (res==SQLITE_SCHEMA) {
		#ifdef HAVE_SQLITE3_PREPARE_V2
			res=sqlite3_prepare_v2
		#else
			res=sqlite3_prepare
		#endif
			(sqliteconn->sqliteptr,query,length,&stmt,NULL);
	}
	if (res==SQLITE_OK) {
		return true;
	}
	sqliteconn->errcode=res;
	sqliteconn->errmesg=sqliteconn->duplicate(
				sqlite3_errmsg(sqliteconn->sqliteptr));
	return false;
}

int32_t sqlitecursor::getBindVariableIndex(const char *variable,
						uint16_t variablesize) {
	if (charstring::isInteger(variable+1,variablesize-1)) {
		return charstring::toInteger(variable+1);
	}
	return sqlite3_bind_parameter_index(stmt,variable);
}

bool sqlitecursor::inputBind(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {
	return (sqlite3_bind_text(stmt,
				getBindVariableIndex(variable,variablesize),
				value,valuesize,SQLITE_STATIC)==SQLITE_OK);
}

bool sqlitecursor::inputBind(const char *variable, 
				uint16_t variablesize,
				int64_t *value) {
	return (sqlite3_bind_int64(stmt,
				getBindVariableIndex(variable,variablesize),
				*value)==SQLITE_OK);
}

bool sqlitecursor::inputBind(const char *variable, 
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {
	return (sqlite3_bind_double(stmt,
				getBindVariableIndex(variable,variablesize),
				*value)==SQLITE_OK);
}

bool sqlitecursor::inputBindBlob(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {
	return (sqlite3_bind_blob(stmt,
				getBindVariableIndex(variable,variablesize),
				value,valuesize,SQLITE_STATIC)==SQLITE_OK);
}
#endif

bool sqlitecursor::executeQuery(const char *query, uint32_t length) {

	// execute the query
	int	success=0;
#ifdef SQLITE_TRANSACTIONAL
	for (;;) {

		success=runQuery(query);

		// If we get a SQLITE_SCHEMA return value, we should retry
		// the query, once.
		//
		// If we get an SQLITE_ERROR and the error is "no such table:"
		// then we need to workaround a bug/feature.  If you create a 
		// table, it's not visible to other sessions until the 
		// sqlite_master table is queried.  In this case, a query 
		// against the sqlite_master table should result in an 
		// SQLITE_SCHEMA return value.
		//
		// For any other return values, jump out of the loop.
		if (success==SQLITE_SCHEMA) {

			// If we're using the statement API then we need to
			// reprepare the statement.  According to the API
			// docs this shouldn't happen with sqlite3_prepare_v2.
			// This appears to be generally true, but with 
			// version 3.6.20 it does.
			#if defined(HAVE_SQLITE3_STMT)
				if (!prepareQuery(query,length)) {
					break;
				}
			#endif
			continue;
		} else if (success==SQLITE_ERROR &&
				sqliteconn->errmesg && 
				!charstring::compare(sqliteconn->errmesg,
							"no such table:",14)) {

			closeResultSet();
			// If for some reason, querying sqlite_master doesn't
			// return SQLITE_SCHEMA, rerun the original query and
			// jump out of the loop.
			if (runQuery("select * from sqlite_master")
							!=SQLITE_SCHEMA) {
				closeResultSet();
				success=runQuery(query);
				break;
			}
		} else {
			break;
		}
	}
#else
	// For non-transactional sqlite, the db must be opened and closed
	// before each query or the results of ddl/dml queries are never
	// visible to other sessions.
	if (sqliteconn->sqliteptr) {
		sqlite3_close(sqliteconn->sqliteptr);
	}
	#ifdef SQLITE3
	if (sqlite3_open(sqliteconn->db,&(sqliteconn->sqliteptr))!=SQLITE_OK) {
		sqliteconn->errmesg=
			sqliteconn->duplicate(
				sqlite3_errmsg(sqliteconn->sqliteptr));
		sqliteconn->errcode=
			sqlite3_errcode(sqliteconn->sqliteptr);
		return false;
	}
	#else
	if (!(sqliteconn->sqliteptr=
			sqlite_open(sqliteconn->db,666,
						&sqliteconn->errmesg))) {
		return false;
	}
	#endif
	success=runQuery(query);
#endif

	checkForTempTable(query,length);

	// cache off the columns so they can be returned later if the result
	// set is suspended/resumed
	#ifdef HAVE_SQLITE3_STMT
	columntables=new char *[ncolumn];
	columnnames=new char *[ncolumn];
	columntypes=new int[ncolumn];
	if (lastinsertrowid) {
		columntables[0]=charstring::duplicate("");
		columnnames[0]=charstring::duplicate("LASTINSERTROWID");
		columntypes[0]=INTEGER_DATATYPE;
	} else {
		for (int i=0; i<ncolumn; i++) {
			columntables[i]=
				charstring::duplicate(
					#ifdef HAVE_SQLITE3_COLUMN_TABLE_NAME
					sqlite3_column_table_name(stmt,i)
					#else
					""
					#endif
					);
			columnnames[i]=charstring::duplicate(
					sqlite3_column_name(stmt,i));
			columntypes[i]=sqlite3_column_type(stmt,i);
		}
	}
	#else
	columnnames=new char *[ncolumn];
	for (int i=0; i<ncolumn; i++) {
		columnnames[i]=charstring::duplicate(result[i]);
	}
	rowindex=rowindex+ncolumn;
	#endif

	return (success==SQLITE_OK);
}

int sqlitecursor::runQuery(const char *query) {

	// clear any errors
	sqliteconn->clearErrors();

	// clean up old column names
	if (columnnames) {
		for (int i=0; i<ncolumn; i++) {
			delete[] columnnames[i];
		}
		delete[] columnnames;
		columnnames=NULL;
	}

	#ifdef HAVE_SQLITE3_STMT
	// clean up old column tables
	if (columntables) {
		for (int i=0; i<ncolumn; i++) {
			delete[] columntables[i];
		}
		delete[] columntables;
		columntables=NULL;
	}

	// clean up old column types
	if (columntypes) {
		delete[] columntypes;
		columntypes=NULL;
	}
	#endif

	// reset counters and flags
	nrow=0;
	#ifndef HAVE_SQLITE3_STMT
	ncolumn=0;
	rowindex=0;
	#endif
	lastinsertrowid=false;

	// handle special case of selecting the last row id
	if (selectlastinsertrowid.match(query)) {
		lastinsertrowid=true;
		#ifdef HAVE_SQLITE3_STMT
		justexecuted=true;
		#endif
		selectLastInsertRowId();
		return SQLITE_OK;
	}

#ifdef HAVE_SQLITE3_STMT
	// sqlite3_step executes the query and fetches the first row.  There's
	// no way to just execute the query, to see if there was an error or
	// not, without also fetching the first row.
	int	res=sqlite3_step(stmt);

	// error of some kind
	if (res!=SQLITE_DONE && res!=SQLITE_ROW) {
		sqliteconn->errcode=res;
		#ifndef HAVE_SQLITE3_PREPARE_V2
		// When using sqlite3_step with sqlite3_prepare, if
		// sqlite3_step returns SQLITE_ERROR then you have to call
		// sqlite3_reset or sqlite3_finalize to get the specific error
		// code.  You don't have to do this when using
		// sqlite3_prepare_v2.  In that case, sqlite3_step will return
		// the error code directly.
		if (res==SQLITE_ERROR) {
			sqliteconn->errcode=sqlite3_reset(stmt);
		}
		#endif
		sqliteconn->errmesg=
			sqliteconn->duplicate(
				sqlite3_errmsg(sqliteconn->sqliteptr));

		// if the error code was SQLITE_SCHEMA then return that,
		// otherwise return a generic SQLITE_ERROR
		return (sqliteconn->errcode==SQLITE_SCHEMA)?
					SQLITE_SCHEMA:SQLITE_ERROR;
	}

	// SQLITE_DONE or SQLITE_ROW
	ncolumn=sqlite3_column_count(stmt);
	nrow=(res!=SQLITE_DONE);
	justexecuted=true;
	return SQLITE_OK;
#else
	// run the appropriate query
	int	retval=sqlite3_get_table(sqliteconn->sqliteptr,
					query,
					&result,&nrow,&ncolumn,
					&sqliteconn->errmesg);
	if (retval==SQLITE_ERROR) {
		sqliteconn->errcode=sqlite3_errcode(sqliteconn->sqliteptr);
	}
	return retval;
#endif
}

void sqlitecursor::selectLastInsertRowId() {

	// fake a result set with 1 field
	nrow=1;
	ncolumn=1;
	#ifdef HAVE_SQLITE3_STMT
	lastinsertrowidstr=charstring::parseNumber(
					(int64_t)sqlite3_last_insert_rowid(
							sqliteconn->sqliteptr));
	#else
	result=new char *[2];
	result[0]=charstring::duplicate("LASTINSERTROWID");
	result[1]=charstring::parseNumber((int64_t)sqlite3_last_insert_rowid(
							sqliteconn->sqliteptr));
	#endif
}

bool sqlitecursor::knowsRowCount() {
	#ifdef HAVE_SQLITE3_STMT
	return false;
	#else
	return true;
	#endif
}

uint64_t sqlitecursor::rowCount() {
	#ifdef HAVE_SQLITE3_STMT
	return 0;
	#else
	return nrow;
	#endif
}

bool sqlitecursor::knowsAffectedRows() {
	return false;
}

uint32_t sqlitecursor::colCount() {
	return ncolumn;
}

const char *sqlitecursor::getColumnName(uint32_t col) {
	return columnnames[col];
}

#ifdef HAVE_SQLITE3_STMT
const char *sqlitecursor::getColumnTable(uint32_t col) {
	return columntables[col];
}

uint16_t sqlitecursor::getColumnType(uint32_t col) {
	switch (columntypes[col]) {
		case SQLITE_INTEGER:
			return INTEGER_DATATYPE;
		case SQLITE_FLOAT:
			return FLOAT_DATATYPE;
		case SQLITE_TEXT:
			return STRING_DATATYPE;
		case SQLITE_BLOB:
			return BLOB_DATATYPE;
		case SQLITE_NULL:
			return NULL_DATATYPE;
		default:
			return UNKNOWN_DATATYPE;
	}
}
#endif

bool sqlitecursor::noRowsToReturn() {
	return (!nrow);
}

bool sqlitecursor::skipRow(bool *error) {
	#ifdef HAVE_SQLITE3_STMT
	return fetchRow(error);
	#else
	rowindex=rowindex+ncolumn;
	return true;
	#endif
}

bool sqlitecursor::fetchRow(bool *error) {

	*error=false;

	#ifdef HAVE_SQLITE3_STMT
	if (justexecuted) {
		justexecuted=false;
		return true;
	}
	if (lastinsertrowid) {
		return false;
	}
	int	result=sqlite3_step(stmt);
	if (result==SQLITE_ERROR) {
		*error=true;
	}
	return (result==SQLITE_ROW);
	#else
	// have to check for nrow+1 because the 
	// first row is actually the column names
	return (rowindex<(ncolumn*(nrow+1)));
	#endif
}

void sqlitecursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {

#ifdef HAVE_SQLITE3_STMT

	// handle lastinsertrowid specially
	if (lastinsertrowid) {
		*field=lastinsertrowidstr;
		*fieldlength=charstring::length(*field);
		*blob=false;
		*null=false;
		return;
	}

	// Get the type before calling sqlite3_column_text.
	// sqlite3_column_text does a type conversion and the result of
	// sqlite3_column_type is undefined after the conversion.
	int	dtype=sqlite3_column_type(stmt,col);
	*field=(const char *)((dtype==SQLITE_BLOB)?
				sqlite3_column_blob(stmt,col):
				sqlite3_column_text(stmt,col));
	*fieldlength=sqlite3_column_bytes(stmt,col);
	*null=(*field==NULL);

	// set the blob indiciator false, otherwise we'll have to implement
	// methods for fetching the blob in chunks and there's no need to
	// do that, for now at least
	*blob=false;
#else
	// sqlite is kind of strange, the result set is not returned
	// in a 2-d array of pointers to rows/columns, but rather
	// a 1-d array pointing to fields.  You have to manually keep
	// track of which column you're on.
	if (result[rowindex]) {
		*field=result[rowindex];
		*fieldlength=charstring::length(result[rowindex]);
	} else {
		*null=true;
	}
	rowindex++;
#endif
}

void sqlitecursor::closeResultSet() {

#ifdef HAVE_SQLITE3_STMT
	if (lastinsertrowidstr) {
		delete[] lastinsertrowidstr;
		lastinsertrowidstr=NULL;
	}
	sqlite3_reset(stmt);
#else
	if (result) {
		if (lastinsertrowid) {
			delete[] result[0];
			delete[] result[1];
			delete[] result;
		} else {
			sqlite3_free_table(result);
		}
		result=NULL;
	}
#endif
}

#ifdef SQLITE3
char *sqliteconnection::duplicate(const char *str) {
	if (!str) {
		return NULL;
	}
	size_t	length=charstring::length(str);
	char	*buffer=(char *)sqlite3_malloc(length+1);
	charstring::copy(buffer,str,length);
	buffer[length]='\0';
	return buffer;
}
#endif

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_sqliteconnection(
						sqlrservercontroller *cont) {
		return new sqliteconnection(cont);
	}
}
