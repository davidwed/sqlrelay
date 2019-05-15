// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#include "../../config.h"

#include <rudiments/environment.h>

#include <infxcli.h>

#include "sqlrbench.h"

class informixbench : public sqlrbench {
	public:
		informixbench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

#define INFORMIX_FETCH_AT_ONCE		10
#define INFORMIX_MAX_SELECT_LIST_SIZE	256
#define INFORMIX_MAX_ITEM_BUFFER_SIZE	32768

struct informixcolumn {
	char		name[4096];
	SQLSMALLINT	namelength;
	SQLLEN		type;
	SQLLEN		precision;
	SQLLEN		scale;
	SQLLEN		flags;
	SQLLEN		primarykey;
	SQLLEN		unique;
	SQLLEN		partofkey;
	SQLLEN		unsignednumber;
	SQLLEN		zerofill;
	SQLLEN		binary;
	SQLLEN		autoincrement;
};

class informixbenchconnection : public sqlrbenchconnection {
	friend class informixbenchcursor;
	public:
			informixbenchconnection(const char *connectstring,
						const char *dbtype);

		bool	connect();
		bool	disconnect();

	private:
		const char	*informixdir;
		const char	*servername;
		const char	*dbname;
		const char	*user;
		const char	*password;
		stringbuffer	dsn;

		SQLHENV		env;
		SQLRETURN	erg;
		SQLHDBC		dbc;
};

class informixbenchcursor : public sqlrbenchcursor {
	public:
			informixbenchcursor(sqlrbenchconnection *con);

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		informixbenchconnection	*informixbcon;

		SQLHSTMT	stmt;
		SQLRETURN	erg;
		SQLSMALLINT	ncols;

		informixcolumn	column[INFORMIX_MAX_SELECT_LIST_SIZE];
		char		field[INFORMIX_MAX_SELECT_LIST_SIZE]
						[INFORMIX_FETCH_AT_ONCE]
						[INFORMIX_MAX_ITEM_BUFFER_SIZE];
		SQLLEN		indicator[INFORMIX_MAX_SELECT_LIST_SIZE]
						[INFORMIX_FETCH_AT_ONCE];
};

informixbench::informixbench(const char *connectstring,
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
	con=new informixbenchconnection(connectstring,db);
	cur=new informixbenchcursor(con);
}


informixbenchconnection::informixbenchconnection(
				const char *connectstring,
				const char *db) :
				sqlrbenchconnection(connectstring,db) {
	informixdir=getParam("informixdir");
	servername=getParam("servername");
	dbname=getParam("db");
	user=getParam("user");
	password=getParam("password");
}

bool informixbenchconnection::connect() {

	if (charstring::length(informixdir) &&
		!environment::setValue("INFORMIXDIR",informixdir)) {
		stdoutput.printf("Failed to set INFORMIXDIR "
					"environment variable");
		return false;
	}

	dsn.clear();
	if (!charstring::isNullOrEmpty(servername)) {
		dsn.append("Servername=")->append(servername);
	}
	if (!charstring::isNullOrEmpty(dbname)) {
		if (dsn.getStringLength()) {
			dsn.append(";");
		}
		dsn.append("Database=")->append(dbname);
	}
	if (!charstring::isNullOrEmpty(user)) {
		if (dsn.getStringLength()) {
			dsn.append(";");
		}
		dsn.append("LogonID=")->append(user);
	}
	if (!charstring::isNullOrEmpty(password)) {
		if (dsn.getStringLength()) {
			dsn.append(";");
		}
		dsn.append("pwd=")->append(password);
	}

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

	erg=SQLDriverConnect(dbc,NULL,
				(SQLCHAR *)dsn.getString(),
				dsn.getStringLength(),
				NULL,0,NULL,SQL_DRIVER_COMPLETE);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLDriverConnect failed\n");
		return false;
	}
	return true;
}

bool informixbenchconnection::disconnect() {
	SQLDisconnect(dbc);
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
	return true;
}

informixbenchcursor::informixbenchcursor(sqlrbenchconnection *con) :
							sqlrbenchcursor(con) {
	informixbcon=(informixbenchconnection *)con;
}

bool informixbenchcursor::open() {

	erg=SQLAllocHandle(SQL_HANDLE_STMT,informixbcon->dbc,&stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLAllocHandle (STMT) failed\n");
		return false;
	}
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)INFORMIX_FETCH_AT_ONCE,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		stdoutput.printf("SQLSetStmtAttr (ROW_ARRAY_SIZE) failed\n");
		return false;
	}
	return true;
}

bool informixbenchcursor::query(const char *query, bool getcolumns) {

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
					&(column[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// informix doesn't support column length,
			// so we'll just use the precision

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
			// Informix doesn't support SQL_COLUMN_NULLABLE.
			// Nullability is just part of the "flags".
			erg=SQLColAttribute(stmt,i+1,SQL_INFX_ATTR_FLAGS,
					NULL,0,NULL,&(column[i].flags));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_UNSIGNED,
					NULL,0,NULL,
					&(column[i].unsignednumber));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// zero fill

			// binary

			// autoincrement
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_AUTO_INCREMENT,
					NULL,0,NULL,&(column[i].autoincrement));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
		}

		if (column[i].type==SQL_LONGVARBINARY ||
			column[i].type==SQL_INFX_UDT_BLOB) {
			erg=SQLBindCol(stmt,i+1,SQL_C_BINARY,
					field[i],
					INFORMIX_MAX_ITEM_BUFFER_SIZE,
					indicator[i]);
		} else {
			erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
					field[i],
					INFORMIX_MAX_ITEM_BUFFER_SIZE,
					indicator[i]);
		}
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

bool informixbenchcursor::close() {
	SQLFreeHandle(SQL_HANDLE_STMT,stmt);
	return true;
}

extern "C" {
	sqlrbench *new_informixbench(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new informixbench(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}
