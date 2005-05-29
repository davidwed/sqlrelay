// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <odbcconnection.h>

#include <config.h>

#include <datatypes.h>

#include <stdlib.h>

uint16_t odbcconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void odbcconnection::handleConnectString() {
	dsn=connectStringValue("dsn");
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	const char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));
}

bool odbcconnection::logIn() {

	// allocate environment handle
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return false;
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
		return false;
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
		return false;
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
		return false;
	}
	return true;
}

sqlrcursor *odbcconnection::initCursor() {
	return (sqlrcursor *)new odbccursor((sqlrconnection *)this);
}

void odbcconnection::deleteCursor(sqlrcursor *curs) {
	delete (odbccursor *)curs;
}

void odbcconnection::logOut() {
	SQLDisconnect(dbc);
#if (ODBCVER >= 0x0300)
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
#else
	SQLFreeConnect(dbc);
	SQLFreeEnv(env);
#endif
}

bool odbcconnection::ping() {
	return true;
}

const char *odbcconnection::identify() {
	return "odbc";
}

#if (ODBCVER >= 0x0300)
bool odbcconnection::autoCommitOn() {
	return (SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_ON,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

bool odbcconnection::autoCommitOff() {
	return (SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_OFF,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

bool odbcconnection::commit() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_COMMIT)==SQL_SUCCESS);
}

bool odbcconnection::rollback() {
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

bool odbccursor::prepareQuery(const char *query, uint32_t length) {

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
		return false;
	}

// this code is here in case unixodbc or iodbc ever 
// successfully support array fetches

/*#if (ODBCVER >= 0x0300)
	// set the row array size
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)FETCH_AT_ONCE,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
#endif*/

	// prepare the query
	erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	return true;
}

bool odbccursor::inputBindString(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint16_t valuesize,
					short *isnull) {

	if (*isnull==SQL_NULL_DATA) {
		erg=SQLBindParameter(stmt,
				charstring::toLong(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)isnull
				#else
				(SQLINTEGER *)isnull
				#endif
				);
	} else {
		erg=SQLBindParameter(stmt,
				charstring::toLong(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)NULL
				#else
				(SQLINTEGER *)NULL
				#endif
				);
	}
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::inputBindLong(const char *variable,
					uint16_t variablesize,
					uint32_t *value) {

	erg=SQLBindParameter(stmt,
				charstring::toLong(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_LONG,
				SQL_INTEGER,
				0,
				0,
				value,
				sizeof(long),
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)NULL
				#else
				(SQLINTEGER *)NULL
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::inputBindDouble(const char *variable,
					uint16_t variablesize,
					double *value,
					uint32_t precision,
					uint32_t scale) {

	erg=SQLBindParameter(stmt,
				charstring::toLong(variable+1),
				SQL_PARAM_INPUT,
				SQL_C_DOUBLE,
				SQL_DECIMAL,
				precision,
				scale,
				value,
				sizeof(double),
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)NULL
				#else
				(SQLINTEGER *)NULL
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

bool odbccursor::outputBindString(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint16_t valuesize, 
					short *isnull) {

	erg=SQLBindParameter(stmt,
				charstring::toLong(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				(SQLPOINTER)value,
				valuesize,
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)isnull
				#else
				(SQLINTEGER *)isnull
				#endif
				);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

short odbccursor::nonNullBindValue() {
	return 0;
}

short odbccursor::nullBindValue() {
	return SQL_NULL_DATA;
}

bool odbccursor::bindValueIsNull(short isnull) {
	return (isnull==SQL_NULL_DATA);
}

bool odbccursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

	// initialize counts
	ncols=0;
	row=0;
	maxrow=0;
	totalrows=0;

	// execute the query
	erg=SQLExecute(stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	checkForTempTable(query,length);

	// get the column count
	erg=SQLNumResultCols(stmt,&ncols);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	if (ncols>MAX_SELECT_LIST_SIZE) {
		ncols=MAX_SELECT_LIST_SIZE;
	}

	// run through the columns
	for (SQLSMALLINT i=0; i<ncols; i++) {

		if (conn->sendColumnInfo()) {
#if (ODBCVER >= 0x0300)
			// column name
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_LABEL,
					col[i].name,MAX_ITEM_BUFFER_SIZE,
					(SQLSMALLINT *)&(col[i].namelength),
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)NULL
					#else
					NULL
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column length
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_LENGTH,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].length)
					#else
					&(col[i].length)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
	
			// column type
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_TYPE,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].type)
					#else
					&(col[i].type)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column precision
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_PRECISION,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].precision)
					#else
					&(col[i].precision)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column scale
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_SCALE,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].scale)
					#else
					&(col[i].scale)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_NULLABLE,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].nullable)
					#else
					&(col[i].nullable)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_UNSIGNED,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].unsignednumber)
					#else
					&(col[i].unsignednumber)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// zero fill

			// binary

			// autoincrement
			erg=SQLColAttribute(stmt,i+1,SQL_DESC_AUTO_UNIQUE_VALUE,
					NULL,0,NULL,
					#ifdef SQLCOLATTRIBUTE_SQLLEN
					(SQLLEN *)&(col[i].autoincrement)
					#else
					&(col[i].autoincrement)
					#endif
					);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
#else
			// column name
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_LABEL,
					col[i].name,MAX_ITEM_BUFFER_SIZE,
					(SQLSMALLINT *)&(col[i].namelength),
					NULL);
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column length
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_LENGTH,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].length));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column type
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_TYPE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].type));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column precision
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_PRECISION,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].precision));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column scale
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_SCALE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].scale));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// column nullable
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_NULLABLE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].nullable));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttributes(stmt,i+1,SQL_COLUMN_UNSIGNED,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].unsignednumber));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}

			// zero fill

			// binary

			// autoincrement
#ifdef SQL_DESC_AUTO_UNIQUE_VALUE
			erg=SQLColAttributes(stmt,i+1,
					SQL_COLUMN_AUTO_UNIQUE_VALUE,
					NULL,0,NULL,
					(SQLINTEGER *)&(col[i].autoincrement));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return false;
			}
#else
			col[i].autoincrement=0;
#endif
#endif
		}


		// bind the column to a buffer
		erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
				field[i],MAX_ITEM_BUFFER_SIZE,
				#ifdef SQLBINDPARAMETER_SQLLEN
				(SQLLEN *)&indicator[i]
				#else
				(SQLINTEGER *)&indicator[i]
				#endif
				);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
	}

	// get the row count
#ifdef SQLROWCOUNT_SQLLEN
	erg=SQLRowCount(stmt,(SQLLEN *)&affectedrows);
#else
	erg=SQLRowCount(stmt,&affectedrows);
#endif
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
}

const char *odbccursor::getErrorMessage(bool *liveconnection) {

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

	*liveconnection=true;

	return errormsg->getString();
}

void odbccursor::returnRowCounts() {

	// send row counts
	conn->sendRowCounts(false,0,true,affectedrows);
}

void odbccursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void odbccursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// a useful variable
	uint16_t	type;

	// for each column...
	for (SQLSMALLINT i=0; i<ncols; i++) {

		uint16_t	binary=0;
		if (col[i].type==SQL_BIGINT) {
			type=BIGINT_DATATYPE;
		} else if (col[i].type==SQL_BINARY) {
			type=BINARY_DATATYPE;
			binary=1;
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
			binary=1;
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
			binary=1;
		} else if (col[i].type==SQL_VARCHAR) {
			type=VARCHAR_DATATYPE;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send column definition
		conn->sendColumnDefinition(col[i].name,col[i].namelength,type,
						col[i].length,col[i].precision,
						col[i].scale,col[i].nullable,
						0,0,0,
						col[i].unsignednumber,0,binary,
						col[i].autoincrement);
	}
}

bool odbccursor::noRowsToReturn() {
	// if there are no columns, then there can't be any rows either
	return (!ncols);
}

bool odbccursor::skipRow() {
	return fetchRow();
}

bool odbccursor::fetchRow() {

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#if (ODBCVER >= 0x0300)
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return false;
	}
	if (!row) {
		erg=SQLFetchScroll(stmt,SQL_FETCH_NEXT,0);
		if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
			return false;
		}
		SQLGetStmtAttr(stmt,SQL_ATTR_ROW_NUMBER,
				(SQLPOINTER)&rownumber,0,NULL);
		if (rownumber==totalrows) {
			return false;
		}
		maxrow=rownumber-totalrows;
		totalrows=rownumber;
	}
	return true;
#else*/
	erg=SQLFetch(stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return false;
	}
	return true;
//#endif
}

void odbccursor::returnRow() {

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#if (ODBCVER >= 0x0300)
	for (SQLSMALLINT index=0; index<ncols; index++) {

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
	for (SQLSMALLINT index=0; index<ncols; index++) {

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
