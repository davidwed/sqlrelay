// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <mdbtoolsconnection.h>
#include <rudiments/rawbuffer.h>

#include <config.h>

#include <datatypes.h>

#if defined(HAVE_MDB_RUN_QUERY)
	// it's called mdb_sql_run_query in the .h file,
	// but mdb_run_query in the library
	extern "C" int mdb_run_query(MdbSQL *sql, char *query);
#elif !defined(HAVE_MDB_SQL_RUN_QUERY)
	extern "C" MdbSQL * _mdb_sql(MdbSQL *sql);
	extern "C" int yyparse();
#endif

extern void mdb_remove_backends();

mdbtoolsconnection::mdbtoolsconnection() : sqlrconnection_svr() {
}

uint16_t mdbtoolsconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void mdbtoolsconnection::handleConnectString() {
	db=connectStringValue("db");
}

bool mdbtoolsconnection::logIn(bool printerrors) {
	mdb_init_backends();
	return true;
}

sqlrcursor_svr *mdbtoolsconnection::initCursor() {
	return (sqlrcursor_svr *)new mdbtoolscursor((sqlrconnection_svr *)this);
}

void mdbtoolsconnection::deleteCursor(sqlrcursor_svr *curs) {
	delete (mdbtoolscursor *)curs;
}

void mdbtoolsconnection::logOut() {
#ifdef HAVE_MDB_REMOVE_BACKENDS
	mdb_remove_backends();
#endif
}

bool mdbtoolsconnection::ping() {
	return true;
}

const char *mdbtoolsconnection::identify() {
	return "mdbtools";
}

const char *mdbtoolsconnection::dbVersion() {
#ifdef MDB_VERSION_NO
	return MDB_VERSION_NO;
#else
	return "unknown";
#endif
}

bool mdbtoolsconnection::getListsByApiCalls() {
	return true;
}

bool mdbtoolsconnection::getDatabaseList(sqlrcursor_svr *cursor,
						const char *wild) {
	return ((mdbtoolscursor *)cursor)->getDatabaseList(wild);
}

bool mdbtoolsconnection::getTableList(sqlrcursor_svr *cursor,
						const char *wild) {
	return ((mdbtoolscursor *)cursor)->getTableList(wild);
}

bool mdbtoolsconnection::getColumnList(sqlrcursor_svr *cursor,
						const char *table,
						const char *wild) {
	return ((mdbtoolscursor *)cursor)->getColumnList(table,wild);
}

bool mdbtoolsconnection::isTransactional() {
	return false;
}

bool mdbtoolsconnection::setIsolationLevel(const char *isolevel) {
	// do nothing
	return true;
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

mdbtoolscursor::mdbtoolscursor(sqlrconnection_svr *conn) :
					sqlrcursor_svr(conn) {
	mdbtoolsconn=(mdbtoolsconnection *)conn;
	columnnames=NULL;
	mdbsql=(void *)new MdbSQL;
	rawbuffer::zero(mdbsql,sizeof(MdbSQL));
	mdb=NULL;
	currentlistindex=0;
	currenttable=NULL;
	currenttabledef=NULL;
	currentcolumn=NULL;
	currentcolumnsize=NULL;
	currentwild=NULL;
	cursortype=QUERY_CURSORTYPE;
}

mdbtoolscursor::~mdbtoolscursor() {
	delete[] columnnames;
	delete currentwild;
	delete (MdbSQL *)mdbsql;
}

bool mdbtoolscursor::openCursor(uint16_t id) {

	// handle db
	const char	*dbval;
	if (mdbtoolsconn->db && mdbtoolsconn->db[0]) {
		dbval=mdbtoolsconn->db;
	} else {
		dbval="";
	}

	return mdb_sql_open((MdbSQL *)mdbsql,const_cast<char *>(dbval));
}

bool mdbtoolscursor::closeCursor() {

	if (!sqlrcursor_svr::closeCursor()) {
		return false;
	}

	mdb_sql_exit((MdbSQL *)mdbsql);
	#ifdef HAVE_MDB_CLOSE
		if (mdb) {
			mdb_close(mdb);
		}
	#endif
	return true;
}

bool mdbtoolscursor::supportsNativeBinds() {
	return false;
}

bool mdbtoolscursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

	cursortype=QUERY_CURSORTYPE;

	// execute the query
	mdb_sql_reset((MdbSQL *)mdbsql);
#if defined(HAVE_MDB_RUN_QUERY)
	if (!mdb_run_query((MdbSQL *)mdbsql,(char *)query)) {
		return false;
	}
#elif defined(HAVE_MDB_SQL_RUN_QUERY)
	if (!mdb_sql_run_query((MdbSQL *)mdbsql,(char *)query)) {
		return false;
	}
#else
	g_input_ptr=(char *)query;
	_mdb_sql((MdbSQL *)mdbsql);
	if (yyparse()) {
		return false;
	}
#endif

	return true;
}

bool mdbtoolscursor::getDatabaseList(const char *wild) {
	cursortype=DB_LIST_CURSORTYPE;
	resetListValues(wild);
	return true;
}

bool mdbtoolscursor::getTableList(const char *wild) {

	cursortype=TABLE_LIST_CURSORTYPE;

	// open the database for non-sql access
	#ifdef HAVE_MDB_CLOSE
		if (mdb) {
			mdb_close(mdb);
		}
	#endif
	const char	*dbval;
	if (mdbtoolsconn->db && mdbtoolsconn->db[0]) {
		dbval=mdbtoolsconn->db;
	} else {
		dbval="";
	}
	#ifdef HAVE_MDB_OPEN_2_PARAM
		mdb=mdb_open(dbval,MDB_NOFLAGS);
	#else
		mdb=mdb_open(const_cast<char *>(dbval));
	#endif
	if (!mdb) {
		return false;
	}

	// read the catalog
	if (!mdb_read_catalog(mdb,MDB_ANY)) {
		return false;
	}

	// reset current list values
	resetListValues(wild);

	return true;
}

bool mdbtoolscursor::getColumnList(const char *table, const char *wild) {

	cursortype=COLUMN_LIST_CURSORTYPE;

	// open the database for non-sql access
	#ifdef HAVE_MDB_CLOSE
		if (mdb) {
			mdb_close(mdb);
		}
	#endif
	const char	*dbval;
	if (mdbtoolsconn->db && mdbtoolsconn->db[0]) {
		dbval=mdbtoolsconn->db;
	} else {
		dbval="";
	}
	#ifdef HAVE_MDB_OPEN_2_PARAM
		mdb=mdb_open(dbval,MDB_NOFLAGS);
	#else
		mdb=mdb_open(const_cast<char *>(dbval));
	#endif
	if (!mdb) {
		return false;
	}

	// read the catalog
	if (!mdb_read_catalog(mdb,MDB_ANY)) {
		return false;
	}

	// reset current list values
	resetListValues(wild);

	// find the specified table in the catalog
	for (uint32_t i=0; i<mdb->num_catalog; i++) {
		currenttable=(MdbCatalogEntry *)
				g_ptr_array_index(mdb->catalog,i);
		if (currenttable->object_type==MDB_TABLE &&
			!charstring::compare(currenttable->object_name,table)) {
			currenttabledef=mdb_read_table(currenttable);
			if (!currenttabledef) {
				return false;
			}
			mdb_read_columns(currenttabledef);
			return true;
		}
	}
	return false;
}

void mdbtoolscursor::resetListValues(const char *wild) {
	currentlistindex=0;
	currenttable=NULL;
	currenttabledef=NULL;
	currentcolumn=NULL;
	currentcolumnsize=NULL;
	currentcolumnprec=NULL;
	currentcolumnscale=NULL;

	// convert the wildcard to a regular expression
	if (charstring::length(wild)) {
		stringbuffer	retval;
		for (const char *c=wild; *c; c++) {
			if (*c=='%') {
				retval.append(".*");
			} else {
				retval.append(*c);
			}
		}
		currentwild=new regularexpression(retval.getString());
		currentwild->study();
	}
}

bool mdbtoolscursor::matchCurrentWild(const char *value) {
	return (!currentwild || currentwild->match(value));
}

void mdbtoolscursor::errorMessage(const char **errorstring,
					int64_t *errorcode,
					bool *liveconnection) {
	*errorstring="error";
	*errorcode=0;
	*liveconnection=true;
}

bool mdbtoolscursor::knowsRowCount() {
	return false;
}

uint64_t mdbtoolscursor::rowCount() {
	return 0;
}

bool mdbtoolscursor::knowsAffectedRows() {
	return false;
}

uint64_t mdbtoolscursor::affectedRows() {
	return 0;
}

uint32_t mdbtoolscursor::colCount() {
	if (cursortype==QUERY_CURSORTYPE) {
		return ((MdbSQL *)mdbsql)->num_columns;
	} else if (cursortype==COLUMN_LIST_CURSORTYPE) {
		return 5;
	}
	return 1;
}

const char * const *mdbtoolscursor::columnNames() {

	if (cursortype==QUERY_CURSORTYPE) {
		columnnames=new char *[((MdbSQL *)mdbsql)->num_columns];
		for (unsigned int i=0; i<((MdbSQL *)mdbsql)->num_columns; i++) {
			MdbSQLColumn	*col=(MdbSQLColumn *)
				g_ptr_array_index(
					((MdbSQL *)mdbsql)->columns,i);
			columnnames[i]=col->name;
		}
	} else if (cursortype==DB_LIST_CURSORTYPE) {
		columnnames=new char*[1];
		columnnames[0]=(char *)"DATABASE";
	} else if (cursortype==TABLE_LIST_CURSORTYPE) {
		columnnames=new char*[1];
		columnnames[0]=(char *)"TABLE";
	} else if (cursortype==COLUMN_LIST_CURSORTYPE) {
		columnnames=new char*[3];
		columnnames[0]=(char *)"NAME";
		columnnames[1]=(char *)"TYPE";
		columnnames[2]=(char *)"SIZE";
		columnnames[3]=(char *)"PRECISION";
		columnnames[4]=(char *)"SCALE";
	}
	return columnnames;
}

uint16_t mdbtoolscursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_IDS;
}

void mdbtoolscursor::returnColumnInfo() {

	if (cursortype==QUERY_CURSORTYPE) {

		// for each column...
		for (unsigned int i=0; i<((MdbSQL *)mdbsql)->num_columns; i++) {

			// get the column
			MdbSQLColumn	*col=(MdbSQLColumn *)
				g_ptr_array_index(
					((MdbSQL *)mdbsql)->columns,i);

			// send the column definition
			conn->sendColumnDefinition(col->name,
					charstring::length(col->name),
					UNKNOWN_DATATYPE,0,0,0,0,0,0,0,0,0,0,0);
		}

	} else if (cursortype==DB_LIST_CURSORTYPE) {
		conn->sendColumnDefinition("DATABASE",8,
				UNKNOWN_DATATYPE,0,0,0,0,0,0,0,0,0,0,0);
	} else if (cursortype==TABLE_LIST_CURSORTYPE) {
		conn->sendColumnDefinition("TABLE",5,
				UNKNOWN_DATATYPE,0,0,0,0,0,0,0,0,0,0,0);
	} else if (cursortype==COLUMN_LIST_CURSORTYPE) {
		conn->sendColumnDefinition("NAME",4,
				UNKNOWN_DATATYPE,0,0,0,0,0,0,0,0,0,0,0);
		conn->sendColumnDefinition("TYPE",4,
				UNKNOWN_DATATYPE,0,0,0,0,0,0,0,0,0,0,0);
		conn->sendColumnDefinition("SIZE",4,
				UNKNOWN_DATATYPE,0,0,0,0,0,0,0,0,0,0,0);
		conn->sendColumnDefinition("PRECISION",9,
				UNKNOWN_DATATYPE,0,0,0,0,0,0,0,0,0,0,0);
		conn->sendColumnDefinition("SCALE",5,
				UNKNOWN_DATATYPE,0,0,0,0,0,0,0,0,0,0,0);
	}
}

bool mdbtoolscursor::noRowsToReturn() {

	if (cursortype==QUERY_CURSORTYPE) {
		// if there were no columns then there can be no rows
		return (((MdbSQL *)mdbsql)->num_columns==0);
	} else if (cursortype==DB_LIST_CURSORTYPE) {
		return false;
	} else if (cursortype==TABLE_LIST_CURSORTYPE) {
		return (mdb->num_catalog==0);
	} else if (cursortype==COLUMN_LIST_CURSORTYPE) {
		return (currenttabledef->num_cols==0);
	}
	return true;
}

bool mdbtoolscursor::skipRow() {
	return fetchRow();
}

bool mdbtoolscursor::fetchRow() {
	if (cursortype==QUERY_CURSORTYPE) {
		#ifdef HAVE_MDB_SQL_FETCH_ROW
			return mdb_sql_fetch_row((MdbSQL *)mdbsql,
						((MdbSQL *)mdbsql)->cur_table);
		#else
			return mdb_fetch_row(((MdbSQL *)mdbsql)->cur_table);
		#endif
	} else if (cursortype==DB_LIST_CURSORTYPE) {
		if (currentlistindex==0) {
			currentlistindex++;
			return true;
		}
		return false;
	} else if (cursortype==TABLE_LIST_CURSORTYPE) {
		for (;;) {
			if (currentlistindex==mdb->num_catalog) {
				currenttable=NULL;
				return false;
			}
			currenttable=(MdbCatalogEntry *)
					g_ptr_array_index(
						mdb->catalog,
						currentlistindex);
			currentlistindex++;
			if (currenttable->object_type==MDB_TABLE &&
				matchCurrentWild(currenttable->object_name)) {
				return true;
			}
		}
	} else if (cursortype==COLUMN_LIST_CURSORTYPE) {
		for (;;) {
			if (currentlistindex==currenttabledef->num_cols) {
				currentcolumn=NULL;
				return false;
			}
			currentcolumn=(MdbColumn *)
					g_ptr_array_index(
						currenttabledef->columns,
						currentlistindex);
			currentlistindex++;
			if (matchCurrentWild(currentcolumn->name)) {
				return true;
			}
		}
	}
	return false;
}

void mdbtoolscursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {

	if (cursortype==QUERY_CURSORTYPE) {

		// find the corresponding column in the current table
		MdbTableDef	*table=((MdbSQL *)mdbsql)->cur_table;
		MdbSQLColumn	*column=(MdbSQLColumn *)
			g_ptr_array_index(((MdbSQL *)mdbsql)->columns,col);

		for (uint32_t tcol=0; tcol<table->num_cols; tcol++) {

			MdbColumn	*tablecolumn=(MdbColumn *)
				g_ptr_array_index(table->columns,tcol);

			if (!charstring::compare(tablecolumn->name,
							column->name)) {

				#ifdef HAVE_MDB_COL_TO_STRING_5_PARAM
					char	*data=mdb_col_to_string(
						((MdbSQL *)mdbsql)->mdb,
						((MdbSQL *)mdbsql)->mdb->pg_buf,
						tablecolumn->cur_value_start,
						tablecolumn->col_type,
						tablecolumn->cur_value_len);
				#else
					char	*data=mdb_col_to_string(
						((MdbSQL *)mdbsql)->mdb,
						tablecolumn->cur_value_start,
						tablecolumn->col_type,
						tablecolumn->cur_value_len);
				#endif

				if (data) {
					*field=data;
					*fieldlength=charstring::length(data);
					return;
				} else {
					*null=true;
				}
			}
		}

	} else if (cursortype==DB_LIST_CURSORTYPE) {
		if (col==0) {
			*field=mdbtoolsconn->db;
			*fieldlength=charstring::length(*field);
		}
	} else if (cursortype==TABLE_LIST_CURSORTYPE) {
		if (col==0 && currenttable) {
			*field=currenttable->object_name;
			*fieldlength=charstring::length(*field);
		}
	} else if (cursortype==COLUMN_LIST_CURSORTYPE) {
		*field=NULL;
		if (col==0 && currentcolumn) {
			*field=currentcolumn->name;
		} else if (col==1 && currentcolumn) {
			switch (currentcolumn->col_type) {
				case MDB_BOOL:
					*field="BOOL";
					break;
				case MDB_BYTE:
					*field="BYTE";
					break;
				case MDB_INT:
					*field="INT";
					break;
				case MDB_LONGINT:
					*field="LONGINT";
					break;
				case MDB_MONEY:
					*field="MONEY";
					break;
				case MDB_FLOAT:
					*field="FLOAT";
					break;
				case MDB_DOUBLE:
					*field="DOUBLE";
					break;
				case MDB_SDATETIME:
					*field="SDATETIME";
					break;
				case MDB_TEXT:
					*field="TEXT";
					break;
				case MDB_OLE:
					*field="OLE";
					break;
				case MDB_MEMO:
					*field="MEMO";
					break;
				case MDB_REPID:
					*field="REPID";
					break;
				case MDB_NUMERIC:
					*field="NUMERIC";
					break;
				default:
					*field="UNKNOWN";
					break;
			}
		} else if (col==2 && currentcolumn) {
			delete[] currentcolumnsize;
			currentcolumnsize=charstring::parseNumber(
						currentcolumn->col_size);
			*field=currentcolumnsize;
		} else if (col==3 && currentcolumn) {
			delete[] currentcolumnprec;
			currentcolumnprec=charstring::parseNumber(
						currentcolumn->col_prec);
			*field=currentcolumnprec;
		} else if (col==4 && currentcolumn) {
			delete[] currentcolumnscale;
			currentcolumnscale=charstring::parseNumber(
						currentcolumn->col_scale);
			*field=currentcolumnscale;
		}
		*fieldlength=charstring::length(*field);
	}
}

void mdbtoolscursor::cleanUpData(bool freeresult, bool freebinds) {

	delete[] columnnames;
	columnnames=NULL;

	if (cursortype==COLUMN_LIST_CURSORTYPE) {
		delete[] currentcolumnsize;
		currentcolumnsize=NULL;
		delete[] currentcolumnprec;
		currentcolumnprec=NULL;
		delete[] currentcolumnscale;
		currentcolumnscale=NULL;
	}
	if (cursortype!=QUERY_CURSORTYPE) {
		delete currentwild;
		currentwild=NULL;
	}
}
