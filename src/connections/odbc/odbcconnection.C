// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <odbcconnection.h>

#include <config.h>

#include <datatypes.h>

#include <stdlib.h>

int	odbcconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	odbcconnection::handleConnectString() {
	dsn=connectStringValue("dsn");
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom && !strcasecmp(autocom,"yes")));
}

int	odbcconnection::logIn() {

	// allocate environment handle
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return 0;
	}
	erg=SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,
				(void *)SQL_OV_ODBC3,0);
#else
	erg=SQLAllocEnv(&env);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_ENV,env);
#else
		SQLFreeEnv(env);
#endif
		return 0;
	}

	// allocate connection handle
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
#else
	erg=SQLAllocConnect(env,&dbc);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
#else
		SQLFreeConnect(dbc);
		SQLFreeEnv(env);
#endif
		return 0;
	}

	// set the connect timeout
#if (ODBCVER >= 0x0300)
	SQLSetConnectAttr(dbc,SQL_LOGIN_TIMEOUT,(SQLPOINTER *)5,0);
#endif

	// connect to the database
	erg=SQLConnect(dbc,(SQLCHAR *)dsn,SQL_NTS,
				(SQLCHAR *)getUser(),SQL_NTS,
				(SQLCHAR *)getPassword(),SQL_NTS);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
#else
		SQLFreeConnect(dbc);
		SQLFreeEnv(env);
#endif
		return 0;
	}
	return 1;
}

sqlrcursor	*odbcconnection::initCursor() {
	return (sqlrcursor *)new odbccursor((sqlrconnection *)this);
}

void	odbcconnection::deleteCursor(sqlrcursor *curs) {
	delete (odbccursor *)curs;
}

void	odbcconnection::logOut() {
	SQLDisconnect(dbc);
#if (ODBCVER >= 0x0300)
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
#else
	SQLFreeConnect(dbc);
	SQLFreeEnv(env);
#endif
}

int	odbcconnection::ping() {
	return 1;
}

char	*odbcconnection::identify() {
	return "odbc";
}

#if (ODBCVER >= 0x0300)
unsigned short	odbcconnection::autoCommitOn() {
	return (unsigned short)(SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_ON,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

unsigned short	odbcconnection::autoCommitOff() {
	return (unsigned short)(SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_OFF,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

int	odbcconnection::commit() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_COMMIT)==SQL_SUCCESS);
}

int	odbcconnection::rollback() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_ROLLBACK)==SQL_SUCCESS);
}
#endif

odbccursor::odbccursor(sqlrconnection *conn) : sqlrcursor(conn) {
	errormsg=NULL;
	odbcconn=(odbcconnection *)conn;
	stmt=NULL;
}

odbccursor::~odbccursor() {
	if (errormsg) {
		delete errormsg;
	}
}

int	odbccursor::prepareQuery(const char *query, long length) {

	if (stmt) {
#if (ODBCVER >= 0x0300)
		SQLFreeHandle(SQL_HANDLE_STMT,stmt);
#else
		SQLFreeStmt(stmt,SQL_DROP);
#endif
	}

	// allocate the cursor
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_STMT,odbcconn->dbc,&stmt);
#else
	erg=SQLAllocStmt(odbcconn->dbc,&stmt);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}

// this code is here in case unixodbc or iodbc ever 
// successfully support array fetches

/*#if (ODBCVER >= 0x0300)
	// set the row array size
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)FETCH_AT_ONCE,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
#endif*/

	// prepare the query
	erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}

	return 1;
}

int	odbccursor::inputBindString(const char *variable,
					unsigned short variablesize,
					const char *value,
					unsigned short valuesize,
					short *isnull) {

	if (*isnull==SQL_NULL_DATA) {
		erg=SQLBindParameter(stmt,
				atoi(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				(SQLINTEGER *)isnull);
	} else {
		erg=SQLBindParameter(stmt,
				atoi(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				(SQLINTEGER *)NULL);
	}
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
	return 1;
}

int	odbccursor::inputBindLong(const char *variable,
					unsigned short variablesize,
					unsigned long *value) {

	erg=SQLBindParameter(stmt,
				atoi(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_LONG,
				SQL_INTEGER,
				0,
				0,
				value,
				sizeof(long),
				(SQLINTEGER *)NULL);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
	return 1;
}

int	odbccursor::inputBindDouble(const char *variable,
					unsigned short variablesize,
					double *value,
					unsigned short precision,
					unsigned short scale) {

	erg=SQLBindParameter(stmt,
				atoi(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_DOUBLE,
				SQL_DECIMAL,
				precision,
				scale,
				value,
				sizeof(double),
				(SQLINTEGER *)NULL);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
	return 1;
}

int	odbccursor::outputBindString(const char *variable, 
					unsigned short variablesize,
					const char *value, 
					unsigned short valuesize, 
					short *isnull) {

	erg=SQLBindParameter(stmt,
				atoi(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				(SQLINTEGER *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
	return 1;
}

short	odbccursor::nonNullBindValue() {
	return 0;
}

short	odbccursor::nullBindValue() {
	return SQL_NULL_DATA;
}

int	odbccursor::bindValueIsNull(short isnull) {
	if (isnull==SQL_NULL_DATA) {
		return 1;
	}
	return 0;
}

int	odbccursor::executeQuery(const char *query, long length,
						unsigned short execute) {

	// initialize counts
	ncols=0;
	row=0;
	maxrow=0;
	totalrows=0;

	// execute the query
	erg=SQLExecute(stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}

	// get the column count
	erg=SQLNumResultCols(stmt,&ncols);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
	if (ncols>MAX_SELECT_LIST_SIZE) {
		ncols=MAX_SELECT_LIST_SIZE;
	}

	// run through the columns
	for (int i=0; i<ncols; i++) {

		if (conn->sendColumnInfo()) {
#if (ODBCVER >= 0x0300)
			// column name
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LABEL,
					col[i].name,MAX_ITEM_BUFFER_SIZE,
					(SQLSMALLINT *)&(col[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// column length
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_LENGTH,
					NULL,0,NULL,&(col[i].length));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}
	
			// column type
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_TYPE,
					NULL,0,NULL,&(col[i].type));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// column precision
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_PRECISION,
					NULL,0,NULL,&(col[i].precision));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// column scale
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_SCALE,
					NULL,0,NULL,&(col[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// column nullable
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_NULLABLE,
					NULL,0,NULL,&(col[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}
#else
			// column name
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_LABEL,
					col[i].name,MAX_ITEM_BUFFER_SIZE,
					(SQLSMALLINT *)&(col[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// column length
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_LENGTH,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].length));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// column type
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_TYPE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].type));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// column precision
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_PRECISION,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].precision));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// column scale
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_SCALE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// column nullable
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_NULLABLE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}
#endif
		}


		// bind the column to a buffer
		erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
				field[i],MAX_ITEM_BUFFER_SIZE,
				(SQLINTEGER *)&indicator[i]);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return 0;
		}
	}

	// get the row count
	erg=SQLRowCount(stmt,&affectedrows);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}

	return 1;
}

char	*odbccursor::getErrorMessage(int *liveconnection) {

	SQLCHAR		error[501];
	SQLCHAR		state[10];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errnum;

	// need to use SQLGetDiagRec and SQLGetDiagField here...
	SQLError(odbcconn->env,odbcconn->dbc,stmt,state,&nativeerrnum,
							error,500,&errnum);
	if (errormsg) {
		delete errormsg;
	}
	errormsg=new stringbuffer();
	errormsg->append((const char *)error);

	*liveconnection=1;

	return errormsg->getString();
}

void	odbccursor::returnRowCounts() {

	// send row counts
	conn->sendRowCounts((long)-1,(long)affectedrows);
}

void	odbccursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void	odbccursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// a useful variable
	int	type;

	// for each column...
	for (int i=0; i<ncols; i++) {

		if (col[i].type==SQL_BIGINT) {
			type=BIGINT_DATATYPE;
		} else if (col[i].type==SQL_BINARY) {
			type=BINARY_DATATYPE;
		} else if (col[i].type==SQL_BIT) {
			type=BIT_DATATYPE;
		} else if (col[i].type==SQL_CHAR) {
			type=CHAR_DATATYPE;
		} else if (col[i].type==SQL_DATE) {
			type=DATE_DATATYPE;
		} else if (col[i].type==SQL_DECIMAL) {
			type=DECIMAL_DATATYPE;
		} else if (col[i].type==SQL_DOUBLE) {
			type=DOUBLE_DATATYPE;
		} else if (col[i].type==SQL_FLOAT) {
			type=FLOAT_DATATYPE;
		} else if (col[i].type==SQL_INTEGER) {
			type=INTEGER_DATATYPE;
		} else if (col[i].type==SQL_LONGVARBINARY) {
			type=LONGVARBINARY_DATATYPE;
		} else if (col[i].type==SQL_LONGVARCHAR) {
			type=LONGVARCHAR_DATATYPE;
		} else if (col[i].type==SQL_NUMERIC) {
			type=NUMERIC_DATATYPE;
		} else if (col[i].type==SQL_REAL) {
			type=REAL_DATATYPE;
		} else if (col[i].type==SQL_SMALLINT) {
			type=SMALLINT_DATATYPE;
		} else if (col[i].type==SQL_TIME) {
			type=TIME_DATATYPE;
		} else if (col[i].type==SQL_TIMESTAMP) {
			type=TIMESTAMP_DATATYPE;
		} else if (col[i].type==SQL_TINYINT) {
			type=TINYINT_DATATYPE;
		} else if (col[i].type==SQL_VARBINARY) {
			type=VARBINARY_DATATYPE;
		} else if (col[i].type==SQL_VARCHAR) {
			type=VARCHAR_DATATYPE;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send column definition
		conn->sendColumnDefinition(col[i].name,col[i].namelength,type,
						col[i].length,col[i].precision,
						col[i].scale,0,0,0,
						0,0,0,0,0);
	}
}

int	odbccursor::noRowsToReturn() {

	// if there are no columns, then there can't be any rows either
	if (ncols==0) {
		return 1;
	}
	return 0;
}

int	odbccursor::skipRow() {
	return fetchRow();
}

int	odbccursor::fetchRow() {

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#if (ODBCVER >= 0x0300)
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return 0;
	}
	if (row==0) {
		SQLFetchScroll(stmt,SQL_FETCH_NEXT,0);
		SQLGetStmtAttr(stmt,SQL_ATTR_ROW_NUMBER,
				(SQLPOINTER)&rownumber,0,NULL);
		if (rownumber==totalrows) {
			return 0;
		}
		maxrow=rownumber-totalrows;
		totalrows=rownumber;
	}
	return 1;
#else*/
	erg=SQLFetch(stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
	return 1;
//#endif
}

void	odbccursor::returnRow() {

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#if (ODBCVER >= 0x0300)
	for (int index=0; index<ncols; index++) {

		// handle a null field
		if (indicator[index][row]==SQL_NULL_DATA) {
			conn->sendNullField();
			continue;
		}

		// handle a non-null field
		conn->sendField(field[index][row],indicator[index][row]);
	}
	row++;
#else*/
	for (int index=0; index<ncols; index++) {

		// handle a null field
		if (indicator[index]==SQL_NULL_DATA) {
			conn->sendNullField();
			continue;
		}

		// handle a non-null field
		conn->sendField(field[index],indicator[index]);
	}
//#endif
}
