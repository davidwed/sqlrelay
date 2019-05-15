// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include "../../config.h"

// windows needs this and it doesn't appear to hurt on other platforms
#include <rudiments/private/winsock.h>

#include <rudiments/private/inttypes.h>

#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <sqltypes.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

SQLRETURN	erg;
SQLHENV		env;
SQLHDBC		dbc;
SQLHSTMT	stmt;

void checkSuccessString(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("\"%s\"!=\"%s\" ",value,success);
			printf("failure\n");
			//sqlrcur_free(cur);
			//sqlrcon_free(con);
			exit(1);
		}
	}

	if (!strcmp(value,success)) {
		printf("success ");
	} else {
		printf("\"%s\"!=\"%s\" ",value,success);
		printf("failure\n");
		//sqlrcur_free(cur);
		//sqlrcon_free(con);
		exit(1);
	}
}

void checkSuccessInt(int value, int success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("\"%d\"!=\"%d\" ",value,success);
		printf("failure\n");
		//sqlrcur_free(cur);
		//sqlrcon_free(con);
		exit(1);
	}
}

int	main(int argc, char **argv) {

	SQLCHAR		*dsn;
	SQLCHAR		*user;
	SQLCHAR		*password;

	// allocate environemnt handle
	printf("ENV HANDLE: \n");
#if (ODBCVER >= 0x3000)
	erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);

#if defined(SQL_OV_ODBC3_80)
	erg=SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,
				(SQLPOINTER)SQL_OV_ODBC3_80,0);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);
#elif defined(SQL_OV_ODBC3)
	erg=SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,
				(SQLPOINTER)SQL_OV_ODBC3,0);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);
#else
	erg=SQLSetEnvAttr(env,SQL_ATTR_ODBC_VERSION,
				(SQLPOINTER)SQL_OV_ODBC2,0);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);
#endif

#else
	erg=SQLAllocEnv(&env);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);
#endif
	printf("\n");

	// allocate connection handle
	printf("CONNECTION HANDLE: \n");
#if (ODBCVER >= 0x0300)
	erg=SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);
#else
	erg=SQLAllocConnect(env,&dbc);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);
#endif
	printf("\n");

	// connect
	printf("CONNECT: \n");
	dsn=(SQLCHAR *)"sqlrodbc";
	user=(SQLCHAR *)"test";
	password=(SQLCHAR *)"test";
	erg=SQLConnect(dbc,dsn,SQL_NTS,user,SQL_NTS,password,SQL_NTS);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);
	printf("\n");



	printf("BIND: \n");
	erg=SQLAllocHandle(SQL_HANDLE_STMT,dbc,&stmt);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);

	SQLCHAR *placeholder1=(SQLCHAR *)"PARAM DATA1";
	SQLLEN	strlenorindptr1=-115;
	erg=SQLBindParameter(stmt,
				1,
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_LONGVARCHAR,
				0,
				0,
				(SQLPOINTER)placeholder1,
				0,
				&strlenorindptr1);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);

	SQLCHAR	*placeholder2=(SQLCHAR *)"PARAM DATA2";
	SQLLEN	strlenorindptr2=-116;
	erg=SQLBindParameter(stmt,
				2,
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_LONGVARCHAR,
				0,
				0,
				(SQLPOINTER)placeholder2,
				0,
				&strlenorindptr2);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);

	SQLCHAR	*placeholder3=(SQLCHAR *)"PARAM DATA3";
	SQLLEN	strlenorindptr3=-118;
	erg=SQLBindParameter(stmt,
				3,
				SQL_PARAM_INPUT,
				SQL_C_CHAR,
				SQL_LONGVARCHAR,
				0,
				0,
				(SQLPOINTER)placeholder3,
				0,
				&strlenorindptr3);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);

	erg=SQLExecDirect(stmt,(SQLCHAR *)"select ?,?,?",SQL_NTS);
	checkSuccessInt((erg==SQL_NEED_DATA)?1:0,1);

	SQLPOINTER	buffer=NULL;
	erg=SQLParamData(stmt,&buffer);
	checkSuccessInt((erg==SQL_NEED_DATA)?1:0,1);
	checkSuccessString((const char *)buffer,"PARAM DATA1");

	SQLCHAR	*val=(SQLCHAR *)"param data 1 woohoooo";
	erg=SQLPutData(stmt,val,6);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);
	erg=SQLPutData(stmt,val+6,6);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);
	erg=SQLPutData(stmt,val+12,9);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);

	erg=SQLParamData(stmt,&buffer);
	checkSuccessInt((erg==SQL_NEED_DATA)?1:0,1);
	checkSuccessString((const char *)buffer,"PARAM DATA2");

	val=(SQLCHAR *)"param data 2 woohoooo";
	erg=SQLPutData(stmt,val,6);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);
	erg=SQLPutData(stmt,val+6,6);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);
	erg=SQLPutData(stmt,val+12,9);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);

	erg=SQLParamData(stmt,&buffer);
	checkSuccessInt((erg==SQL_NEED_DATA)?1:0,1);
	checkSuccessString((const char *)buffer,"PARAM DATA3");

	val=(SQLCHAR *)"param data 3 woohoooo";
	erg=SQLPutData(stmt,val,6);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);
	erg=SQLPutData(stmt,val+6,6);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);
	erg=SQLPutData(stmt,val+12,9);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);

	erg=SQLParamData(stmt,&buffer);
	checkSuccessInt((erg==SQL_SUCCESS)?1:0,1);

	erg=SQLFetch(stmt);
	checkSuccessInt((erg==SQL_SUCCESS || erg==SQL_SUCCESS_WITH_INFO)?1:0,1);
	printf("\n");

	return 0;
}
