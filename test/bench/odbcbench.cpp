// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#include "../../config.h"

#include <rudiments/environment.h>

#include <sql.h>
#include <sqlext.h>

#include "sqlrbench.h"

class odbcbench : public sqlrbench {
	public:
		odbcbench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

#define ODBC_FETCH_AT_ONCE		10
#define ODBC_MAX_SELECT_LIST_SIZE	256
#define ODBC_MAX_ITEM_BUFFER_SIZE	32768

struct odbccolumn {
	char		name[4096];
	uint16_t	namelength;
	// SQLColAttribute requires that these are signed, 32 bit integers
	SQLLEN		type;
	SQLLEN		length;
	SQLLEN		precision;
	SQLLEN		scale;
	SQLLEN		nullable;
	SQLLEN		unsignednumber;
	SQLLEN		autoincrement;
};

class odbcbenchconnection : public sqlrbenchconnection {
	friend class odbcbenchcursor;
	public:
			odbcbenchconnection(const char *connectstring,
						const char *dbtype);

		bool	connect();
		bool	disconnect();

	private:
		const char	*dsn;
		const char	*user;
		const char	*password;
		const char	*dbname;

		SQLHENV		env;
		SQLRETURN	erg;
		SQLHDBC		dbc;
};

class odbcbenchcursor : public sqlrbenchcursor {
	public:
			odbcbenchcursor(sqlrbenchconnection *con);

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		odbcbenchconnection	*odbcbcon;

		SQLHSTMT	stmt;
		SQLRETURN	erg;
		SQLSMALLINT	ncols;

		odbccolumn	column[ODBC_MAX_SELECT_LIST_SIZE];
		char		field[ODBC_MAX_SELECT_LIST_SIZE]
						[ODBC_FETCH_AT_ONCE]
						[ODBC_MAX_ITEM_BUFFER_SIZE];
		SQLLEN		indicator[ODBC_MAX_SELECT_LIST_SIZE]
						[ODBC_FETCH_AT_ONCE];
};

odbcbench::odbcbench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug) :
					sqlrbench(connectstring,db,
						queries,rows,cols,colsize,
						samples,rsbs,debug) {
	con=new odbcbenchconnection(connectstring,db);
	cur=new odbcbenchcursor(con);
}

odbcbenchconnection::odbcbenchconnection(
				const char *connectstring,
				const char *db) :
				sqlrbenchconnection(connectstring,db) {
	dsn=getParam("dsn");
	user=getParam("user");
	password=getParam("password");
	dbname=getParam("db");
}

bool odbcbenchconnection::connect() {
	erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLAllocHandle (ENV) failed\n");
		return false;
	}
	erg=SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLAllocHandle (DBC) failed\n");
		return false;
	}
	erg=SQLSetConnectAttr(dbc,SQL_ATTR_CURRENT_CATALOG,
				(SQLPOINTER *)db,SQL_NTS);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLSetConnectAttr failed\n");
		return false;
	}
	erg=SQLConnect(dbc,(SQLCHAR *)dsn,SQL_NTS,
				(SQLCHAR *)user,SQL_NTS,
				(SQLCHAR *)password,SQL_NTS);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLConnect failed\n");
		return false;
	}
	return true;
}

bool odbcbenchconnection::disconnect() {
	SQLDisconnect(dbc);
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
	return true;
}

odbcbenchcursor::odbcbenchcursor(sqlrbenchconnection *con) :
						sqlrbenchcursor(con) {
	odbcbcon=(odbcbenchconnection *)con;
}

bool odbcbenchcursor::open() {

	erg=SQLAllocHandle(SQL_HANDLE_STMT,odbcbcon->dbc,&stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLAllocHandle (STMT) failed\n");
		return false;
	}
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)ODBC_FETCH_AT_ONCE,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLSetStmtAttr (ROW_ARRAY_SIZE) failed\n");
		return false;
	}

	return true;
}

bool odbcbenchcursor::query(const char *query, bool getcolumns) {

	erg=SQLPrepare(stmt,(SQLCHAR *)query,charstring::length(query));
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLPrepare failed\n");
		return false;
	}
	erg=SQLExecute(stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLExecute failed\n");
		return false;
	}
	erg=SQLNumResultCols(stmt,&ncols);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLNumResultCols failed\n");
		return false;
	}

	for (SQLSMALLINT i=0; i<ncols; i++) {

		if (getcolumns) {

			// column name
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LABEL,
					column[i].name,4096,
					(SQLSMALLINT *)&(column[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column length
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LENGTH,
					NULL,0,NULL,&(column[i].length));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column type
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_TYPE,
					NULL,0,NULL,&(column[i].type));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column precision
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_PRECISION,
					NULL,0,NULL,&(column[i].precision));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column scale
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_SCALE,
					NULL,0,NULL,&(column[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_NULLABLE,
					NULL,0,NULL,&(column[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// unsigned number
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_UNSIGNED,
					NULL,0,NULL,
					&(column[i].unsignednumber));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// autoincrement
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_AUTO_INCREMENT,
					NULL,0,NULL,&(column[i].autoincrement));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
		}

		// bind field and null indicator
		erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
				field[i],ODBC_MAX_ITEM_BUFFER_SIZE,
				indicator[i]);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			stdoutput.printf("SQLBindCol failed\n");
			return false;
		}
	}

	// run through the cols

	// run through the rows
	if (ncols) {

		int32_t	oldrow=0;
		int32_t	rownumber=0;
		for (;;) {
			erg=SQLFetchScroll(stmt,SQL_FETCH_NEXT,0);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				break;
			}

			SQLGetStmtAttr(stmt,SQL_ATTR_ROW_NUMBER,
					(SQLPOINTER)&rownumber,0,NULL);

			if (rownumber==oldrow) {
				break;
			}

			for (int32_t row=0; row<rownumber-oldrow; row++) {
				for (SQLSMALLINT col=0; col<ncols; col++) {

					if (indicator[col][row]==
							SQL_NULL_DATA) {
						//stdoutput.printf("NULL,");
					} else {
						//stdoutput.printf(
							//"%s,",field[col][row]);
					}
				}
				//stdoutput.printf("\n");
			}
			oldrow=rownumber;
		}
	}

	SQLCloseCursor(stmt);
	return true;
}

bool odbcbenchcursor::close() {
	SQLFreeHandle(SQL_HANDLE_STMT,stmt);
	return true;
}

extern "C" {
	sqlrbench *new_odbcbench(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new odbcbench(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}
