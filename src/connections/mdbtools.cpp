// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/bytestring.h>
#include <rudiments/regularexpression.h>
#include <rudiments/file.h>
#include <rudiments/sys.h>

#include <datatypes.h>
#include <defines.h>
#include <config.h>

extern "C" {
	#include <mdbsql.h>
}

#if defined(HAVE_MDB_RUN_QUERY)
	// it's called mdb_sql_run_query in the .h file,
	// but mdb_run_query in the library
	extern "C" int mdb_run_query(MdbSQL *sql, char *query);
#elif !defined(HAVE_MDB_SQL_RUN_QUERY)
	extern "C" MdbSQL * _mdb_sql(MdbSQL *sql);
	extern "C" int yyparse();
#endif

extern void mdb_remove_backends();

class SQLRSERVER_DLLSPEC mdbtoolsconnection : public sqlrserverconnection {
	friend class mdbtoolscursor;
	public:
			mdbtoolsconnection(sqlrservercontroller *cont);
			~mdbtoolsconnection();
	private:
		void	handleConnectString();
		bool	logIn(const char **error, const char **warning);
		sqlrservercursor	*newCursor(uint16_t id);
		void	deleteCursor(sqlrservercursor *curs);
		void	logOut();
		bool	isTransactional();
		bool	ping();
		bool	selectDatabase(const char *database);
		char	*getCurrentDatabase();
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		bool	getListsByApiCalls();
		bool	getDatabaseList(sqlrservercursor *cursor,
						const char *wild);
		bool	getTableList(sqlrservercursor *cursor,
						const char *wild,
						uint16_t objecttypes);
		bool	getColumnList(sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		bool	setIsolationLevel(const char *isolevel);
		const char	*noopQuery();
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	supportsAutoCommit();
		bool	commit();
		bool	rollback();
		void	errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t	*errorcode,
					bool *liveconnection);

		char		*db;
		const char	*identity;
		char		*hostname;
};

enum cursortype_t {
	QUERY_CURSORTYPE=0,
	DB_LIST_CURSORTYPE,
	TABLE_LIST_CURSORTYPE,
	COLUMN_LIST_CURSORTYPE,
	NOOP_CURSORTYPE
};

class SQLRSERVER_DLLSPEC mdbtoolscursor : public sqlrservercursor {
	friend class mdbtoolsconnection;
	private:
				mdbtoolscursor(sqlrserverconnection *conn,
								uint16_t id);
				~mdbtoolscursor();
		bool		open();
		bool		close();
		bool		supportsNativeBinds(const char *query,
							uint32_t length);
		bool		executeQuery(const char *query,
							uint32_t length);
		bool		knowsAffectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow(bool *error);
		void		getField(uint32_t col,
						const char **field,
						uint64_t *fieldlength,
						bool *blob,
						bool *null);
		void		closeResultSet();
		bool		getDatabaseList(const char *wild);
		bool		getTableList(const char *wild,
						uint16_t objecttypes);
		bool		getColumnList(const char *table,
						const char *wild);
		void		resetListValues(const char *wild);
		bool		matchCurrentWild(const char *value);

		mdbtoolsconnection	*mdbtoolsconn;

		void		*mdbsql;

		MdbHandle	*mdb;
		uint32_t	currentlistindex;
		MdbCatalogEntry	*currenttable;
		MdbTableDef	*currenttabledef;
		MdbColumn	*currentcolumn;
		char		*currentcolumnsize;
		char		*currentcolumnprec;
		char		*currentcolumnscale;
		regularexpression	*currentwild;

		cursortype_t	cursortype;
};

mdbtoolsconnection::mdbtoolsconnection(sqlrservercontroller *cont) :
						sqlrserverconnection(cont) {
	db=NULL;
	identity=NULL;
	hostname=NULL;
}

mdbtoolsconnection::~mdbtoolsconnection() {
	delete[] db;
	delete[] hostname;
}

void mdbtoolsconnection::handleConnectString() {

	sqlrserverconnection::handleConnectString();

	db=charstring::duplicate(cont->getConnectStringValue("db"));
	identity=cont->getConnectStringValue("identity");

	cont->setFetchAtOnce(1);
	cont->setMaxColumnCount(0);
	cont->setMaxFieldLength(0);
}

bool mdbtoolsconnection::logIn(const char **error, const char **warning) {
	mdb_init_backends();
	return file::exists(db);
}

sqlrservercursor *mdbtoolsconnection::newCursor(uint16_t id) {
	return (sqlrservercursor *)new mdbtoolscursor(
					(sqlrserverconnection *)this,id);
}

void mdbtoolsconnection::deleteCursor(sqlrservercursor *curs) {
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

bool mdbtoolsconnection::selectDatabase(const char *database) {

	// keep track of the original db and host
	char	*originaldb=db;

	// reset the db/host
	db=charstring::duplicate(database);

	cont->clearError();

	// log out and log back in to the specified database
	logOut();
	if (!logIn(NULL,NULL)) {

		// Set the error.
		cont->setError(SQLR_ERROR_DBNOTFOUND_STRING,
				SQLR_ERROR_DBNOTFOUND,true);

		// log back in to the original database, we'll assume that works
		delete[] db;
		db=originaldb;
		logOut();
		logIn(NULL,NULL);
		return false;
	}

	// clean up
	delete[] originaldb;
	return true;
}

char *mdbtoolsconnection::getCurrentDatabase() {
	return charstring::duplicate(db);
}

const char *mdbtoolsconnection::identify() {
	return (identity)?identity:"mdbtools";
}

const char *mdbtoolsconnection::dbVersion() {
#ifdef MDB_VERSION_NO
	return MDB_VERSION_NO;
#else
	return "unknown";
#endif
}

const char *mdbtoolsconnection::dbHostName() {
	if (!hostname) {
		hostname=sys::getHostName();
	}
	return hostname;
}

bool mdbtoolsconnection::getListsByApiCalls() {
	return true;
}

bool mdbtoolsconnection::getDatabaseList(sqlrservercursor *cursor,
						const char *wild) {
	return ((mdbtoolscursor *)cursor)->getDatabaseList(wild);
}

bool mdbtoolsconnection::getTableList(sqlrservercursor *cursor,
						const char *wild,
						uint16_t objecttypes) {
	return ((mdbtoolscursor *)cursor)->getTableList(wild,objecttypes);
}

bool mdbtoolsconnection::getColumnList(sqlrservercursor *cursor,
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

const char *mdbtoolsconnection::noopQuery() {
	return "noop";
}

bool mdbtoolsconnection::autoCommitOn() {
	// do nothing
	return true;
}

bool mdbtoolsconnection::autoCommitOff() {
	// do nothing
	return true;
}

bool mdbtoolsconnection::supportsAutoCommit() {
	// fake this
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

void mdbtoolsconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	charstring::safeCopy(errorbuffer,errorbufferlength,"error",5);
	*errorlength=5;
	// MDBTools doesn't have an error number per-se.  We'll set it
	// to 1 though, because 0 typically means "no error has occurred"
	// and some apps respond that way if errorcode is set to 0.
	// This ends up being important when using:
	// Oracle dblink -> ODBC -> SQL Relay -> MDBTools
	*errorcode=1;
	*liveconnection=true;
}

mdbtoolscursor::mdbtoolscursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	mdbtoolsconn=(mdbtoolsconnection *)conn;
	mdbsql=(void *)new MdbSQL;
	bytestring::zero(mdbsql,sizeof(MdbSQL));
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
	delete currentwild;
	delete (MdbSQL *)mdbsql;
}

bool mdbtoolscursor::open() {

	// handle db
	const char	*dbval;
	if (!charstring::isNullOrEmpty(mdbtoolsconn->db)) {
		dbval=mdbtoolsconn->db;
	} else {
		dbval="";
	}

	return mdb_sql_open((MdbSQL *)mdbsql,const_cast<char *>(dbval));
}

bool mdbtoolscursor::close() {

	if (!sqlrservercursor::close()) {
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

bool mdbtoolscursor::supportsNativeBinds(const char *query, uint32_t length) {
	return false;
}

bool mdbtoolscursor::executeQuery(const char *query, uint32_t length) {

	if (!charstring::compare(query,"noop")) {
		cursortype=NOOP_CURSORTYPE;
		return true;
	}

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

bool mdbtoolscursor::getTableList(const char *wild, uint16_t objecttypes) {

	cursortype=TABLE_LIST_CURSORTYPE;

	// open the database for non-sql access
	#ifdef HAVE_MDB_CLOSE
		if (mdb) {
			mdb_close(mdb);
		}
	#endif
	const char	*dbval;
	if (!charstring::isNullOrEmpty(mdbtoolsconn->db)) {
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

bool mdbtoolscursor::getColumnList(const char *table,
					const char *wild) {

	cursortype=COLUMN_LIST_CURSORTYPE;

	// open the database for non-sql access
	#ifdef HAVE_MDB_CLOSE
		if (mdb) {
			mdb_close(mdb);
		}
	#endif
	const char	*dbval;
	if (!charstring::isNullOrEmpty(mdbtoolsconn->db)) {
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

bool mdbtoolscursor::knowsAffectedRows() {
	return false;
}

uint32_t mdbtoolscursor::colCount() {
	if (cursortype==NOOP_CURSORTYPE) {
		return 0;
	} else if (cursortype==QUERY_CURSORTYPE) {
		return ((MdbSQL *)mdbsql)->num_columns;
	} else if (cursortype==COLUMN_LIST_CURSORTYPE) {
		return 5;
	}
	return 1;
}

const char *mdbtoolscursor::getColumnName(uint32_t col) {
	if (cursortype==QUERY_CURSORTYPE) {
		return ((MdbSQLColumn *)g_ptr_array_index(
				((MdbSQL *)mdbsql)->columns,col))->name;
	} else if (cursortype==DB_LIST_CURSORTYPE) {
		if (col==0) {
			return "DATABASE";
		}
	} else if (cursortype==TABLE_LIST_CURSORTYPE) {
		if (col==0) {
			return "TABLE";
		} else if (col==1) {
			return "TYPE";
		}
	} else if (cursortype==COLUMN_LIST_CURSORTYPE) {
		switch (col) {
			case 0:
				return "NAME";
			case 1:
				return "TYPE";
			case 2:
				return "SIZE";
			case 3:
				return "PRECISION";
			case 4:
				return "SCALE";
		}
	}
	return NULL;
}

bool mdbtoolscursor::noRowsToReturn() {
	if (cursortype==NOOP_CURSORTYPE) {
		return true;
	} else if (cursortype==QUERY_CURSORTYPE) {
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

bool mdbtoolscursor::fetchRow(bool *error) {

	*error=false;

	if (cursortype==NOOP_CURSORTYPE) {
		return false;
	}
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

				char	*data=NULL;

				// mdb_col_to_string returns garbage for
				// booleans where cur_value_start is 0, so
				// for those, just leave data NULL.
				if (tablecolumn->col_type!=MDB_BOOL ||
						tablecolumn->cur_value_start) {

					#ifdef HAVE_MDB_COL_TO_STRING_5_PARAM
					data=mdb_col_to_string(
						((MdbSQL *)mdbsql)->mdb,
						((MdbSQL *)mdbsql)->mdb->pg_buf,
						tablecolumn->cur_value_start,
						tablecolumn->col_type,
						tablecolumn->cur_value_len);
					#else
					data=mdb_col_to_string(
						((MdbSQL *)mdbsql)->mdb,
						tablecolumn->cur_value_start,
						tablecolumn->col_type,
						tablecolumn->cur_value_len);
					#endif

				}

				if (data) {
					*field=data;
					*fieldlength=charstring::length(data);
				} else {
					*null=true;
				}
				break;
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
		} else if (col==1 && currenttable) {
			*field="TABLE";
			*fieldlength=5;
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
				#ifdef MDB_DATETIME
				case MDB_DATETIME:
					*field="DATETIME";
					break;
				#endif
				#ifdef MDB_SDATETIME
				case MDB_SDATETIME:
					*field="SDATETIME";
					break;
				#endif
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

void mdbtoolscursor::closeResultSet() {

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

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_mdbtoolsconnection(
						sqlrservercontroller *cont) {
		return new mdbtoolsconnection(cont);
	}
}
