// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqliteconnection.h>

#include <datatypes.h>

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/stat.h>

sqliteconnection::sqliteconnection() : sqlrconnection() {
	sqliteptr=NULL;
	errmesg=NULL;
}

int	sqliteconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	sqliteconnection::handleConnectString() {
	db=connectStringValue("db");
}

int	sqliteconnection::logIn() {
#ifdef SQLITE_TRANSACTIONAL
	if ((sqliteptr=sqlite_open(db,666,&errmesg))) {
		return 1;
	}
	if (errmesg) {
		fprintf(stderr,"%s\n",errmesg);
	}
	return 0;
#else
	return 1;
#endif
}

sqlrcursor	*sqliteconnection::initCursor() {
	return (sqlrcursor *)new sqlitecursor((sqlrconnection *)this);
}

void	sqliteconnection::deleteCursor(sqlrcursor *curs) {
	delete (sqlitecursor *)curs;
}

void	sqliteconnection::logOut() {
#ifdef SQLITE_TRANSACTIONAL
	if (sqliteptr) {
		sqlite_close(sqliteptr);
	}
#endif
}

int	sqliteconnection::ping() {
	return 1;
}

char	*sqliteconnection::identify() {
	return "sqlite";
}

#ifndef SQLITE_TRANSACTIONAL
int	sqliteconnection::isTransactional() {
	return 0;
}

int	sqliteconnection::commit() {
	return 1;
}

int	sqliteconnection::rollback() {
	return 1;
}
#endif

sqlitecursor::sqlitecursor(sqlrconnection *conn) : sqlrcursor(conn) {

	result=NULL;
	newquery=NULL;
	nrow=0;
	ncolumn=0;
	rowindex=0;

	sqliteconn=(sqliteconnection *)conn;
}

sqlitecursor::~sqlitecursor() {
	cleanUpData();
}

int	sqlitecursor::executeQuery(const char *query, long length,
						unsigned short execute) {

	// fake binds
	newquery=fakeInputBinds(query);

	// execute the query
	int retval=0;
#ifdef SQLITE_TRANSACTIONAL
	for (;;) {

		retval=runQuery(newquery,query);

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
		if (retval==SQLITE_SCHEMA) {
			continue;
		} else if (retval==SQLITE_ERROR && sqliteconn->errmesg && 
			!strncmp(sqliteconn->errmesg,"no such table:",14)) {

			cleanUpData();
			// If for some reason, querying sqlite_master doesn't
			// return SQLITE_SCHEMA, rerun the original query and
			// jump out of the loop.
			if (runQuery(NULL,"select * from sqlite_master")
							!=SQLITE_SCHEMA) {
				cleanUpData();
				newquery=fakeInputBinds(query);
				retval=runQuery(newquery,query);
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
		sqlite_close(sqliteconn->sqliteptr);
	}
	if (!(sqliteconn->sqliteptr=
			sqlite_open(sqliteconn->db,666,
						&sqliteconn->errmesg))) {
		return 0;
	}
	retval=runQuery(newquery,query);
#endif

	// set the rowindex past the column names
	rowindex=rowindex+ncolumn;

	return (retval==SQLITE_OK);
}

int	sqlitecursor::runQuery(stringbuffer *newquery, const char *query) {

	// clear any errors
	if (sqliteconn->errmesg) {
		delete[] sqliteconn->errmesg;
		sqliteconn->errmesg=NULL;
	}

	// reset counters
	nrow=0;
	ncolumn=0;
	rowindex=0;

	// run the appropriate query
	if (newquery) {
		return sqlite_get_table(sqliteconn->sqliteptr,
				newquery->getString(),
				&result,&nrow,&ncolumn,
				&sqliteconn->errmesg);
	} else {
		return sqlite_get_table(sqliteconn->sqliteptr,
				query,
				&result,&nrow,&ncolumn,
				&sqliteconn->errmesg);
	}
}

char	*sqlitecursor::getErrorMessage(int *liveconnection) {
	*liveconnection=1;
	if (sqliteconn->errmesg &&
		(!strncmp(sqliteconn->errmesg,"access permission denied",24) ||
		!strncmp(sqliteconn->errmesg,"not a directory",15))) {
		*liveconnection=0;
	}
	return sqliteconn->errmesg;
}

void	sqlitecursor::returnRowCounts() {

	// affected row counts are unknown in sqlite
	conn->sendRowCounts((long)nrow,(long)-1);
}

void	sqlitecursor::returnColumnCount() {
	conn->sendColumnCount(ncolumn);
}

void	sqlitecursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	if (!result) {
		return;
	}

	// sqlite is kind of strange, the row of 
	// the result set is the column names
	for (int i=0; i<ncolumn; i++) {

		// column type and size are unknown in sqlite
		conn->sendColumnDefinition(result[i],
					strlen(result[i]),
					UNKNOWN_DATATYPE,0,0,0,0,0,0);
	}
}

int	sqlitecursor::noRowsToReturn() {
	return (nrow==0);
}

int	sqlitecursor::skipRow() {
	rowindex=rowindex+ncolumn;
	return 1;
}

int	sqlitecursor::fetchRow() {
	// have to check for nrow+1 because the 
	// first row is actually the column names
	return rowindex<(ncolumn*(nrow+1));
}

void	sqlitecursor::returnRow() {

	if (!result) {
		return;
	}

	// sqlite is kind of strange, the result set is not returned
	// in a 2-d array of pointers to rows/columns, but rather
	// a 1-d array pointing to fields.  You have to manually keep
	// track of which column you're on.
	for (int i=0; i<ncolumn; i++) {
		if (result[rowindex]) {
			conn->sendField(result[rowindex],
					strlen(result[rowindex]));
		} else {
			conn->sendNullField();
		}
		rowindex++;
	}
}

void	sqlitecursor::cleanUpData() {
	if (newquery) {
		delete newquery;
		newquery=NULL;
	}
	if (result) {
		sqlite_free_table(result);
		result=NULL;
	}
}
