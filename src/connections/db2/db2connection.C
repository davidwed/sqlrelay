// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <db2connection.h>

#include <datatypes.h>

#include <config.h>

#include <stdlib.h>

int	db2connection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	db2connection::handleConnectString() {

	// override legacy "server" parameter with modern "db" parameter
	server=connectStringValue("server");
	char	*tmp=connectStringValue("db");
	if (tmp && tmp[0]) {
		server=tmp;
	}

	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom && !strcasecmp(autocom,"yes")));
}

int	db2connection::logIn() {

	// allocate environment handle
	erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return 0;
	}

	// allocate connection handle
	erg=SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return 0;
	}

	// set the connect timeout
	SQLSetConnectAttr(dbc,SQL_LOGIN_TIMEOUT,(SQLPOINTER *)5,0);

	// connect to the database
	erg=SQLConnect(dbc,(SQLCHAR *)server,SQL_NTS,
				(SQLCHAR *)getUser(),SQL_NTS,
				(SQLCHAR *)getPassword(),SQL_NTS);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
		return 0;
	}
	return 1;
}

sqlrcursor	*db2connection::initCursor() {
	return (sqlrcursor *)new db2cursor((sqlrconnection *)this);
}

void	db2connection::deleteCursor(sqlrcursor *curs) {
	delete (db2cursor *)curs;
}

void	db2connection::logOut() {
	SQLDisconnect(dbc);
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
}

short	db2connection::nullBindValue() {
	return SQL_NULL_DATA;
}

int	db2connection::bindValueIsNull(short isnull) {
	if (isnull==SQL_NULL_DATA) {
		return 1;
	}
	return 0;
}

unsigned short	db2connection::autoCommitOn() {
	return (unsigned short)(SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_ON,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

unsigned short	db2connection::autoCommitOff() {
	return (unsigned short)(SQLSetConnectAttr(dbc,SQL_ATTR_AUTOCOMMIT,
				(SQLPOINTER)SQL_AUTOCOMMIT_OFF,
				sizeof(SQLINTEGER))==SQL_SUCCESS);
}

int	db2connection::commit() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_COMMIT)==SQL_SUCCESS);
}

int	db2connection::rollback() {
	return (SQLEndTran(SQL_HANDLE_ENV,env,SQL_ROLLBACK)==SQL_SUCCESS);
}

char	*db2connection::pingQuery() {
	return "values 1";
}

char	*db2connection::identify() {
	return "db2";
}

db2cursor::db2cursor(sqlrconnection *conn) : sqlrcursor(conn) {
	db2conn=(db2connection *)conn;
	errormsg=NULL;
	stmt=0;
}

db2cursor::~db2cursor() {
	if (errormsg) {
		delete errormsg;
	}
}

int	db2cursor::prepareQuery(const char *query, long length) {

	if (stmt) {
		SQLFreeHandle(SQL_HANDLE_STMT,stmt);
	}

	// allocate the cursor
	erg=SQLAllocHandle(SQL_HANDLE_STMT,db2conn->dbc,&stmt);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}

	// set the row array size
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)FETCH_AT_ONCE,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}

#if (DB2VERSION==8)
	// set the row status ptr
	erg=SQLSetStmtAttr(stmt,SQL_ATTR_ROW_STATUS_PTR,
				(SQLPOINTER)rowstat,0);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
#endif

	// prepare the query
	erg=SQLPrepare(stmt,(SQLCHAR *)query,length);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
	return 1;
}

int	db2cursor::inputBindString(const char *variable,
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

int	db2cursor::inputBindLong(const char *variable,
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

int	db2cursor::inputBindDouble(const char *variable,
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

int	db2cursor::outputBindString(const char *variable, 
					unsigned short variablesize,
					char *value, 
					unsigned short valuesize, 
					short *isnull) {

	erg=SQLBindParameter(stmt,
				atoi(variable+1),
				SQL_PARAM_OUTPUT,
				SQL_C_CHAR,
				SQL_CHAR,
				0,
				0,
				value,
				valuesize,
				(SQLINTEGER *)isnull);
	if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
		return 0;
	}
	return 1;
}

int	db2cursor::executeQuery(const char *query, long length,
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

	checkForTempTable(query,length);

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

			// primary key

			// unique

			// part of key

			// unsigned number
			erg=SQLColAttribute(stmt,i+1,SQL_COLUMN_UNSIGNED,
					NULL,0,NULL,&(col[i].unsignednumber));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}

			// zero fill

			// binary

			// autoincrement
			erg=SQLColAttribute(stmt,i+1,
					SQL_COLUMN_AUTO_INCREMENT,
					NULL,0,NULL,&(col[i].autoincrement));
			if (erg!=SQL_SUCCESS && erg!=SQL_SUCCESS_WITH_INFO) {
				return 0;
			}
		}

		// bind the column to a buffer
		erg=SQLBindCol(stmt,i+1,SQL_C_CHAR,
				field[i],MAX_ITEM_BUFFER_SIZE,
				//indicator[i]);
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

char	*db2cursor::getErrorMessage(int *liveconnection) {

	SQLCHAR		error[501];
	SQLCHAR		state[10];
	SQLINTEGER	nativeerrnum;
	SQLSMALLINT	errnum;

	// need to use SQLGetDiagRec and SQLGetDiagField here...
	SQLError(db2conn->env,db2conn->dbc,
			stmt,state,&nativeerrnum,error,500,&errnum);
	if (errormsg) {
		delete errormsg;
	}
	errormsg=new stringbuffer();
	errormsg->append((const char *)error);

	*liveconnection=1;

	return errormsg->getString();
}

void	db2cursor::returnRowCounts() {
	conn->sendRowCounts((long)-1,(long)affectedrows);
}

void	db2cursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void	db2cursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// a useful variable
	int	type;

	// for each column...
	for (int i=0; i<ncols; i++) {

		unsigned short	binary=0;
		if (col[i].type==SQL_BIGINT) {
			type=BIGINT_DATATYPE;
		} else if (col[i].type==SQL_BINARY) {
			type=BINARY_DATATYPE;
			binary=1;
		} else if (col[i].type==SQL_BIT) {
			type=BIT_DATATYPE;
		} else if (col[i].type==SQL_CHAR) {
			type=CHAR_DATATYPE;
		} else if (col[i].type==SQL_TYPE_DATE) {
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
		} else if (col[i].type==SQL_TYPE_TIME) {
			type=TIME_DATATYPE;
		} else if (col[i].type==SQL_TYPE_TIMESTAMP) {
			type=TIMESTAMP_DATATYPE;
		} else if (col[i].type==SQL_TINYINT) {
			type=TINYINT_DATATYPE;
		} else if (col[i].type==SQL_VARBINARY) {
			type=VARBINARY_DATATYPE;
			binary=1;
		} else if (col[i].type==SQL_VARCHAR) {
			type=VARCHAR_DATATYPE;
		// DB2 has more datatypes than ODBC...
		} else if (col[i].type==SQL_GRAPHIC) {
			type=GRAPHIC_DATATYPE;
			binary=1;
		} else if (col[i].type==SQL_VARGRAPHIC) {
			type=VARGRAPHIC_DATATYPE;
			binary=1;
		} else if (col[i].type==SQL_LONGVARGRAPHIC) {
			type=LONGVARGRAPHIC_DATATYPE;
			binary=1;
		} else if (col[i].type==SQL_BLOB) {
			type=BLOB_DATATYPE;
			binary=1;
		} else if (col[i].type==SQL_CLOB) {
			type=CLOB_DATATYPE;
		} else if (col[i].type==SQL_DBCLOB) {
			type=DBCLOB_DATATYPE;
		} else if (col[i].type==SQL_DATALINK) {
			type=DATALINK_DATATYPE;
		} else if (col[i].type==SQL_USER_DEFINED_TYPE) {
			type=USER_DEFINED_TYPE_DATATYPE;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send column definition
		conn->sendColumnDefinition(col[i].name,col[i].namelength,type,
					col[i].length,col[i].precision,
					col[i].scale,col[i].nullable,0,0,
					0,col[i].unsignednumber,0,binary,
					col[i].autoincrement);
	}
}

int	db2cursor::noRowsToReturn() {

	// if there are no columns, then there can't be any rows either
	if (ncols) {
		return 0;
	}
	return 1;
}

int	db2cursor::skipRow() {
	if (fetchRow()) {
		row++;
		return 1;
	}
	return 0;
}

int	db2cursor::fetchRow() {

	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return 0;
	}
	if (row==0) {
		SQLFetchScroll(stmt,SQL_FETCH_NEXT,0);


#if (DB2VERSION==8)
		// An apparant bug in version 8.1 causes the SQL_ATTR_ROW_NUMBER
		// to always be 1, running through the row status buffer appears
		// to work.
		for (rownumber=0; rownumber<FETCH_AT_ONCE; rownumber++) {
			if (rowstat[rownumber]!=SQL_SUCCESS && 
				rowstat[rownumber]!=SQL_SUCCESS_WITH_INFO) {
				break;
			}
		}

		// FIXME: is this right?
		rownumber=rownumber+totalrows;	// 20040225 by akaishi
#else
		SQLGetStmtAttr(stmt,SQL_ATTR_ROW_NUMBER,
				(SQLPOINTER)&rownumber,0,NULL);
#endif

		if (rownumber==totalrows) {
			return 0;
		}
		maxrow=rownumber-totalrows;
		totalrows=rownumber;
	}
	return 1;
}

void	db2cursor::returnRow() {

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
}
