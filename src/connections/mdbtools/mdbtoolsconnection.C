// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <mdbtoolsconnection.h>

#include <config.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <datatypes.h>

#ifdef HAVE_MDB_RUN_QUERY
	// it's called mdb_sql_run_query in the .h file,
	// but mdb_run_query in the library
	extern "C" int mdb_run_query(MdbSQL *sql, char *query);
#else
	extern "C" MdbSQL * _mdb_sql(MdbSQL *sql);
	extern "C" int yyparse();
#endif

mdbtoolsconnection::mdbtoolsconnection() {
}

int mdbtoolsconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void mdbtoolsconnection::handleConnectString() {
	db=connectStringValue("db");
}

bool mdbtoolsconnection::logIn() {
	return true;
}

sqlrcursor *mdbtoolsconnection::initCursor() {
	return (sqlrcursor *)new mdbtoolscursor((sqlrconnection *)this);
}

void mdbtoolsconnection::deleteCursor(sqlrcursor *curs) {
	delete (mdbtoolscursor *)curs;
}

void mdbtoolsconnection::logOut() {
}

bool mdbtoolsconnection::ping() {
	return true;
}

char *mdbtoolsconnection::identify() {
	return "mdbtools";
}

bool mdbtoolsconnection::isTransactional() {
	return false;
}

bool mdbtoolsconnection::autoCommitOn() {
	// do nothing
	return true;
}

bool mdbtoolsconnection::autoCommitOff() {
	// do nothing
	return true;
}

bool mdbtoolsconnection::commit() {
	// do nothing
	return true;
}

bool mdbtoolsconnection::rollback() {
	// do nothing
	return true;
}

mdbtoolscursor::mdbtoolscursor(sqlrconnection *conn) : sqlrcursor(conn) {
	mdbtoolsconn=(mdbtoolsconnection *)conn;
}

bool mdbtoolscursor::openCursor(int id) {

	if (!sqlrcursor::openCursor(id)) {
		return false;
	}

	// handle db
	char	*dbval;
	if (mdbtoolsconn->db && mdbtoolsconn->db[0]) {
		dbval=mdbtoolsconn->db;
	} else {
		dbval="";
	}

	return mdb_sql_open(&mdbsql,dbval);
}

bool mdbtoolscursor::closeCursor() {

	if (!sqlrcursor::closeCursor()) {
		return false;
	}

	mdb_sql_exit(&mdbsql);
	return true;
}

bool mdbtoolscursor::executeQuery(const char *query,
					long length, bool execute) {

	// fake binds
	stringbuffer	*newquery=fakeInputBinds(query);

	// execute the query
	mdb_sql_reset(&mdbsql);
#ifdef HAVE_MDB_RUN_QUERY
	if (newquery) {
		if (!mdb_run_query(&mdbsql,newquery->getString())) {
			delete newquery;
			return false;
		}
	} else {
		if (!mdb_run_query(&mdbsql,(char *)query)) {
			return false;
		}
	}
#else
	if (newquery) {
		g_input_ptr=newquery->getString();
		_mdb_sql(&mdbsql);
		if (yyparse()) {
			delete newquery;
			return false;
		}
	} else {
		g_input_ptr=(char *)query;
		_mdb_sql(&mdbsql);
		if (yyparse()) {
			return false;
		}
	}
#endif

	return true;
}

char *mdbtoolscursor::getErrorMessage(bool *liveconnection) {
	*liveconnection=true;
	return "error";
}

void mdbtoolscursor::returnRowCounts() {
	// row count and affected row count are unknown
	conn->sendRowCounts((long)-1,(long)-1);
}

void mdbtoolscursor::returnColumnCount() {
	conn->sendColumnCount(mdbsql.num_columns);
}

void mdbtoolscursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// for each column...
	for (int i=0; i<mdbsql.num_columns; i++) {

		// get the column
		MdbSQLColumn	*col=(MdbSQLColumn *)
			g_ptr_array_index(mdbsql.columns,i);

		// send the column definition
		conn->sendColumnDefinition(col->name,
					charstring::length(col->name),
					UNKNOWN_DATATYPE,0,0,0,0,0,0,0,0,0,0,0);
	}
}

bool mdbtoolscursor::noRowsToReturn() {
	// if there were no columns then there can be no rows
	return (mdbsql.num_columns==0);
}

bool mdbtoolscursor::skipRow() {
	return fetchRow();
}

bool mdbtoolscursor::fetchRow() {
	return mdb_sql_fetch_row(&mdbsql,mdbsql.cur_table);
}

void mdbtoolscursor::returnRow() {

	MdbTableDef	*table;
	MdbColumn	*tablecolumn;
	MdbSQLColumn	*column;

	// run through the columns
	for (int col=0; col<mdbsql.num_columns; col++) {

		// find the corresponding column in the current table
		column=(MdbSQLColumn *)g_ptr_array_index(mdbsql.columns,col);
		table=mdbsql.cur_table;
		for (int tcol=0; tcol<table->num_cols; tcol++) {
			tablecolumn=(MdbColumn *)
				g_ptr_array_index(table->columns,tcol);
			if (!charstring::compare(tablecolumn->name,
							column->name)) {
				char	*data=mdb_col_to_string(
						mdbsql.mdb,
						mdbsql.mdb->pg_buf,
						tablecolumn->cur_value_start,
						tablecolumn->col_type,
						tablecolumn->cur_value_len);
				if (data) {
					conn->sendField(data,
						charstring::length(data));
				} else {
					conn->sendNullField();
				}
			}
		}
	}
}

void mdbtoolscursor::cleanUpData(bool freeresult, bool freebinds) {
}
