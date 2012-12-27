// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <rudiments/regularexpression.h>
#include <rudiments/system.h>

#include <datatypes.h>
#include <config.h>

extern "C" {
#ifdef SQLITE3
	#include <sqlite3.h>
#else
	#include <sqlite.h>
#endif
}

#include <stdio.h>

#ifndef SQLITE3
	#define	sqlite3_open			sqlite_open
	#define	sqlite3_close			sqlite_close
	#define	sqlite3_get_table		sqlite_get_table
	#define	sqlite3_errmsg			sqlite_errmsg
	#define	sqlite3_free_table		sqlite_free_table
	#define	sqlite3_last_insert_rowid	sqlite_last_insert_rowid
	#define sqlite3_free(mem)		sqlite_free((char *)mem)
#endif

class sqliteconnection : public sqlrconnection_svr {
	friend class sqlitecursor;
	public:
				sqliteconnection(sqlrcontroller_svr *cont);
	private:
		void		handleConnectString();
		bool		logIn(const char **error);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		bool		ping();
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(bool wild);
#ifdef SQLITE_TRANSACTIONAL
		const char	*setIsolationLevelQuery();
#endif
		bool		getLastInsertId(uint64_t *id);
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

		const char	*db;

#ifdef SQLITE3
		sqlite3	*sqliteptr;
#else
		sqlite	*sqliteptr;
#endif
		char	*errmesg;
		int64_t	errcode;
};

class sqlitecursor : public sqlrcursor_svr {
	friend class sqliteconnection;
	private:
				sqlitecursor(sqlrconnection_svr *conn);
				~sqlitecursor();
/*#ifdef HAVE_SQLITE3_BIND_INT
		bool		prepareQuery(const char *query,
						uint32_t length);
#endif*/
		bool		supportsNativeBinds();

/*#ifdef HAVE_SQLITE3_BIND_INT
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
#endif*/
		bool		executeQuery(const char *query,
						uint32_t length);
		int		runQuery(const char *query);
		void		selectLastInsertRowId();
		bool		knowsRowCount();
		uint64_t	rowCount();
		bool		knowsAffectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		cleanUpData();

		char		**result;
		char		**columnnames;
		int		nrow;
		int		ncolumn;
		int		rowindex;
		bool		lastinsertrowid;

#ifdef HAVE_SQLITE3_BIND_INT
#endif

		regularexpression	selectlastinsertrowid;

		sqliteconnection	*sqliteconn;
};


sqliteconnection::sqliteconnection(sqlrcontroller_svr *cont) :
					sqlrconnection_svr(cont) {
	sqliteptr=NULL;
	errmesg=NULL;
	errcode=0;
}

void sqliteconnection::handleConnectString() {
	db=cont->connectStringValue("db");
}

bool sqliteconnection::logIn(const char **error) {
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

sqlrcursor_svr *sqliteconnection::initCursor() {
	return (sqlrcursor_svr *)new sqlitecursor((sqlrconnection_svr *)this);
}

void sqliteconnection::deleteCursor(sqlrcursor_svr *curs) {
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
	return "sqlite";
}

const char *sqliteconnection::dbVersion() {
#ifdef SQLITE_VERSION
	return SQLITE_VERSION;
#else
	return "unknown";
#endif
}

const char *sqliteconnection::dbHostName() {
	return rudiments::system::getHostName();
}

const char *sqliteconnection::getDatabaseListQuery(bool wild) {
	return "pragma database_list";
}

const char *sqliteconnection::getTableListQuery(bool wild) {
	return (wild)?
		"select "
		"	tbl_name "
		"from "
		"(select "
		"	tbl_name "
		"from "
		"	sqlite_master "
		"where "
		"	type in ('table','view') "
		"	and "
		"	tbl_name like '%s' "
		"union all "
		"select "
		"	tbl_name "
		"from "
		"	sqlite_temp_master "
		"where "
		"	type in ('table','view') "
		"	and "
		"	tbl_name like '%s') "
		"order by "
		"	tbl_name":

		"select "
		"	tbl_name "
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

const char *sqliteconnection::getColumnListQuery(bool wild) {
	return "select "
		"	'' as column_name, "
		"	'' as data_type, "
		"	'' as length, "
		"	'' as precision, "
		"	'' as scale, "
		"	'' as nullable, "
		"	'' as key, "
		"	'' as column_default, "
		"	'' as extra ";
}

#ifdef SQLITE_TRANSACTIONAL
const char *sqliteconnection::setIsolationLevelQuery() {
	return "pragma %s";
}
#endif

bool sqliteconnection::getLastInsertId(uint64_t *id) {
	*id=sqlite3_last_insert_rowid(sqliteptr);
	return true;
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

sqlitecursor::sqlitecursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {

	result=NULL;
	columnnames=NULL;
	nrow=0;
	ncolumn=0;
	rowindex=0;
	lastinsertrowid=false;

	sqliteconn=(sqliteconnection *)conn;

	selectlastinsertrowid.compile("^\\s*(select|SELECT)\\s+"
				"(last|LAST)\\s+(insert|INSERT)\\s+"
				"(rowid|ROWID)");
	selectlastinsertrowid.study();
}

sqlitecursor::~sqlitecursor() {
	cleanUpData();
}

bool sqlitecursor::supportsNativeBinds() {
	return false;
}

/*bool sqlitecursor::supportsNativeBinds() {
#ifdef HAVE_SQLITE3_BIND_INT
	return true;
#else
	return false;
#endif
}

#ifdef HAVE_SQLITE3_BIND_INT
bool sqlitecursor::prepareQuery(const char *query, uint32_t length) {
	return true;
}

bool sqlitecursor::inputBind(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {
	return true;
}

bool sqlitecursor::inputBind(const char *variable, 
				uint16_t variablesize,
				int64_t *value) {
	return true;
}

bool sqlitecursor::inputBind(const char *variable, 
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {
	return true;
}

bool sqlitecursor::inputBindBlob(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {
	return true;
}

bool sqlitecursor::inputBindClob(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {
	return true;
}
#endif*/

bool sqlitecursor::executeQuery(const char *query, uint32_t length) {

	// execute the query
	int	success=0;
#ifdef SQLITE_TRANSACTIONAL
	for (;;) {

		success=runQuery(query);

		// If we get a SQLITE_SCHEMA return value, we should retry
		// the query.
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
			continue;
		} else if (success==SQLITE_ERROR &&
				sqliteconn->errmesg && 
				!charstring::compare(sqliteconn->errmesg,
							"no such table:",14)) {

			cleanUpData();
			// If for some reason, querying sqlite_master doesn't
			// return SQLITE_SCHEMA, rerun the original query and
			// jump out of the loop.
			if (runQuery("select * from sqlite_master")
							!=SQLITE_SCHEMA) {
				cleanUpData();
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
	columnnames=new char * [ncolumn];
	for (int i=0; i<ncolumn; i++) {
		columnnames[i]=charstring::duplicate(result[i]);
	}

	// set the rowindex past the column names
	rowindex=rowindex+ncolumn;

	return (success==SQLITE_OK);
}

int sqlitecursor::runQuery(const char *query) {

	// clear any errors
	if (sqliteconn->errmesg) {
		sqlite3_free((void *)sqliteconn->errmesg);
		sqliteconn->errmesg=NULL;
		sqliteconn->errcode=0;
	}

	// clean up old column names
	if (columnnames) {
		for (int i=0; i<ncolumn; i++) {
			delete[] columnnames[i];
		}
		delete[] columnnames;
		columnnames=NULL;
	}

	// reset counters and flags
	nrow=0;
	ncolumn=0;
	rowindex=0;
	lastinsertrowid=false;

	// handle special case of selecting the last row id
	if (selectlastinsertrowid.match(query)) {
		lastinsertrowid=true;
		selectLastInsertRowId();
		return SQLITE_OK;
	}

	// run the appropriate query
	int	retval=sqlite3_get_table(sqliteconn->sqliteptr,
					query,
					&result,&nrow,&ncolumn,
					&sqliteconn->errmesg);
	if (retval==SQLITE_ERROR) {
		sqliteconn->errcode=sqlite3_errcode(sqliteconn->sqliteptr);
	}
	return retval;
}

void sqlitecursor::selectLastInsertRowId() {

	// fake a result set with 1 field
	nrow=1;
	ncolumn=1;
	result=new char * [2];
	result[0]=charstring::duplicate("LASTINSERTROWID");
	result[1]=charstring::parseNumber((int64_t)sqlite3_last_insert_rowid(
							sqliteconn->sqliteptr));
}

bool sqlitecursor::knowsRowCount() {
	return true;
}

uint64_t sqlitecursor::rowCount() {
	return nrow;
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

bool sqlitecursor::noRowsToReturn() {
	return (!nrow);
}

bool sqlitecursor::skipRow() {
	rowindex=rowindex+ncolumn;
	return true;
}

bool sqlitecursor::fetchRow() {
	// have to check for nrow+1 because the 
	// first row is actually the column names
	return (rowindex<(ncolumn*(nrow+1)));
}

void sqlitecursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {

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
}

void sqlitecursor::cleanUpData() {

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
	sqlrconnection_svr *new_sqliteconnection(sqlrcontroller_svr *cont) {
		return new sqliteconnection(cont);
	}
}
