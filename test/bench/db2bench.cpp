// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#include "../../config.h"

#include <rudiments/environment.h>

#include <sqlcli1.h>

#include "sqlrbench.h"

class db2bench : public sqlrbench {
	public:
		db2bench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

#define DB2_FETCH_AT_ONCE		10
#define DB2_MAX_SELECT_LIST_SIZE	256
#define DB2_MAX_ITEM_BUFFER_SIZE	32768

struct db2column {
	char		name[4096];
	uint16_t	namelength;
	// SQLColAttribute requires that these are signed, 32 bit integers
	int32_t		type;
	int32_t		length;
	int32_t		precision;
	int32_t		scale;
	int32_t		nullable;
	uint16_t	primarykey;
	uint16_t	unique;
	uint16_t	partofkey;
	uint16_t	unsignednumber;
	uint16_t	zerofill;
	uint16_t	binary;
	uint16_t	autoincrement;
};

class db2benchconnection : public sqlrbenchconnection {
	friend class db2benchcursor;
	public:
			db2benchconnection(const char *connectstring,
						const char *dbtype);

		bool	connect();
		bool	disconnect();

	private:
		const char	*dbname;
		const char	*lang;
		const char	*user;
		const char	*password;

		SQLHENV		env;
		SQLRETURN	erg;
		SQLHDBC		dbc;
};

class db2benchcursor : public sqlrbenchcursor {
	public:
			db2benchcursor(sqlrbenchconnection *con);

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		db2benchconnection	*db2bcon;

		SQLHSTMT	stmt;
		SQLRETURN	erg;
		SQLSMALLINT	ncols;

		db2column	column[DB2_MAX_SELECT_LIST_SIZE];
		char		field[DB2_MAX_SELECT_LIST_SIZE]
						[DB2_FETCH_AT_ONCE]
						[DB2_MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[DB2_MAX_SELECT_LIST_SIZE]
						[DB2_FETCH_AT_ONCE];
		#if (DB2VERSION>7)
		SQLUSMALLINT	rowstat[DB2_FETCH_AT_ONCE];
		#endif
};

db2bench::db2bench(const char *connectstring,
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
	con=new db2benchconnection(connectstring,db);
	cur=new db2benchcursor(con);
}

db2benchconnection::db2benchconnection(
				const char *connectstring,
				const char *db) :
				sqlrbenchconnection(connectstring,db) {
	dbname=getParam("db");
	lang=getParam("lang");
	user=getParam("user");
	password=getParam("password");

	environment::setValue("LANG",lang);
}

bool db2benchconnection::connect() {
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
	erg=SQLConnect(dbc,(SQLCHAR *)dbname,SQL_NTS,
				(SQLCHAR *)user,SQL_NTS,
				(SQLCHAR *)password,SQL_NTS);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLConnect failed\n");
		return false;
	}
	return true;
}

bool db2benchconnection::disconnect() {
	SQLDisconnect(dbc);
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
	return true;
}

db2benchcursor::db2benchcursor(sqlrbenchconnection *con) :
						sqlrbenchcursor(con) {
	db2bcon=(db2benchconnection *)con;
}

bool db2benchcursor::open() {

	erg=SQLAllocHandle(SQL_HANDLE_STMT,db2bcon->dbc,&stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLAllocHandle (STMT) failed\n");
		return false;
	}
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)DB2_FETCH_AT_ONCE,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLSetStmtAttr (ROW_ARRAY_SIZE) failed\n");
		return false;
	}

	#if (DB2VERSION>7)
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_STATUS_PTR,
					(SQLPOINTER)rowstat,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLSetStmtAttr (ROW_STATUS_PTR) failed\n");
		return false;
	}
	#endif

	return true;
}

bool db2benchcursor::query(const char *query, bool getcolumns) {

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
				field[i],DB2_MAX_ITEM_BUFFER_SIZE,
				(SQLINTEGER *)indicator[i]);
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
		int32_t	totalrows=0;
		for (;;) {
			erg=SQLFetchScroll(stmt,SQL_FETCH_NEXT,0);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				break;
			}

			#if (DB2VERSION>7)
			// An apparant bug in version 8.1 causes the
			// SQL_ATTR_ROW_NUMBER to always be 1, running through
			// the row status buffer appears to work though.
			uint32_t	index=0;
			while (index<DB2_FETCH_AT_ONCE &&
				(rowstat[index]==SQL_ROW_SUCCESS ||
				rowstat[index]==SQL_ROW_SUCCESS_WITH_INFO)) {
				index++;
			}
			rownumber=totalrows+index;
			#else
			SQLGetStmtAttr(stmt,SQL_ATTR_ROW_NUMBER,
					(SQLPOINTER)&rownumber,0,NULL);
			#endif

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
			totalrows=rownumber;
		}
	}

	SQLCloseCursor(stmt);
	return true;
}

bool db2benchcursor::close() {
	SQLFreeHandle(SQL_HANDLE_STMT,stmt);
	return true;
}

extern "C" {
	sqlrbench *new_db2bench(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new db2bench(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}
