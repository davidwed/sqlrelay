// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqliteconnection.h>

#include <datatypes.h>

#include <stdio.h>

#ifndef SQLITE3
	#define	sqlite3_open			sqlite_open
	#define	sqlite3_close			sqlite_close
	#define	sqlite3_get_table		sqlite_get_table
	#define	sqlite3_errmsg			sqlite_errmsg
	#define	sqlite3_free_table		sqlite_free_table
	#define	sqlite3_last_insert_rowid	sqlite_last_insert_rowid
#endif


sqliteconnection::sqliteconnection() : sqlrconnection_svr() {
	sqliteptr=NULL;
	errmesg=NULL;
}

uint16_t sqliteconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

bool sqliteconnection::supportsNativeBinds() {
	return false;
}

void sqliteconnection::handleConnectString() {
	db=connectStringValue("db");
}

bool sqliteconnection::logIn() {
#ifdef SQLITE_TRANSACTIONAL
#ifdef SQLITE3
	if (sqlite3_open(db,&sqliteptr)==SQLITE_OK) {
#else
	if ((sqliteptr=sqlite3_open(db,666,&errmesg))) {
#endif
		return true;
	}
#ifdef SQLITE3
	errmesg=charstring::duplicate(sqlite3_errmsg(sqliteptr));
#endif
	if (errmesg) {
		fprintf(stderr,"%s\n",errmesg);
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

sqlitecursor::sqlitecursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {

	result=NULL;
	columnnames=NULL;
	nrow=0;
	ncolumn=0;
	rowindex=0;

	sqliteconn=(sqliteconnection *)conn;

	selectlastinsertrowid.compile("^\\s*(select|SELECT)\\s+"
				"(last|LAST)\\s+(insert|INSERT)\\s+"
				"(rowid|ROWID)");
	selectlastinsertrowid.study();
}

sqlitecursor::~sqlitecursor() {
	cleanUpData(true,true);
}

bool sqlitecursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

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
		} else if (success==SQLITE_ERROR && sqliteconn->errmesg && 
				!charstring::compare(sqliteconn->errmesg,
							"no such table:",14)) {

			cleanUpData(true,true);
			// If for some reason, querying sqlite_master doesn't
			// return SQLITE_SCHEMA, rerun the original query and
			// jump out of the loop.
			if (runQuery("select * from sqlite_master")
							!=SQLITE_SCHEMA) {
				cleanUpData(true,true);
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
			charstring::duplicate(
				sqlite3_errmsg(sqliteconn->sqliteptr));
#else
	if (!(sqliteconn->sqliteptr=
			sqlite_open(sqliteconn->db,666,
						&sqliteconn->errmesg))) {
#endif
		return false;
	}
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
		delete[] sqliteconn->errmesg;
		sqliteconn->errmesg=NULL;
	}

	// clean up old column names
	if (columnnames) {
		for (int i=0; i<ncolumn; i++) {
			delete[] columnnames[i];
		}
		delete[] columnnames;
		columnnames=NULL;
	}

	// reset counters
	nrow=0;
	ncolumn=0;
	rowindex=0;

	// handle special case of selecting the last row id
	if (selectlastinsertrowid.match(query)) {
		selectLastInsertRowId();
		return SQLITE_OK;
	}

	// run the appropriate query
	return sqlite3_get_table(sqliteconn->sqliteptr,
					query,
					&result,&nrow,&ncolumn,
					&sqliteconn->errmesg);
}

void sqlitecursor::selectLastInsertRowId() {

	// fake a result set with 1 field
	nrow=1;
	ncolumn=1;
	result=new char * [2];
	result[0]=charstring::duplicate("LASTINSERTROWID");
	result[1]=charstring::parseNumber(sqlite3_last_insert_rowid(
							sqliteconn->sqliteptr));
}

const char *sqlitecursor::errorMessage(bool *liveconnection) {
	*liveconnection=true;
	if (sqliteconn->errmesg &&
		(!charstring::compare(sqliteconn->errmesg,
					"access permission denied",24) ||
		!charstring::compare(sqliteconn->errmesg,
					"not a directory",15))) {
		*liveconnection=false;
	}
	return sqliteconn->errmesg;
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

uint64_t sqlitecursor::affectedRows() {
	return 0;
}

uint32_t sqlitecursor::colCount() {
	return ncolumn;
}

const char * const * sqlitecursor::columnNames() {
	return columnnames;
}

uint16_t sqlitecursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_IDS;
}

void sqlitecursor::returnColumnInfo() {

	if (!columnnames) {
		return;
	}

	// sqlite is kind of strange, the row of 
	// the result set is the column names
	for (int32_t i=0; i<ncolumn; i++) {

		// column type and size are unknown in sqlite
		conn->sendColumnDefinition(columnnames[i],
					charstring::length(columnnames[i]),
					UNKNOWN_DATATYPE,0,0,0,0,0,0,
					0,0,0,0,0);
	}
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

void sqlitecursor::returnRow() {

	if (!result) {
		return;
	}

	// sqlite is kind of strange, the result set is not returned
	// in a 2-d array of pointers to rows/columns, but rather
	// a 1-d array pointing to fields.  You have to manually keep
	// track of which column you're on.
	for (int32_t i=0; i<ncolumn; i++) {
		if (result[rowindex]) {
			conn->sendField(result[rowindex],
					charstring::length(result[rowindex]));
		} else {
			conn->sendNullField();
		}
		rowindex++;
	}
}

void sqlitecursor::cleanUpData(bool freeresult, bool freebinds) {

	if (freeresult && result) {
		sqlite3_free_table(result);
		result=NULL;
	}
}
