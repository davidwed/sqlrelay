// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define	MAX_ITEM_BUFFER_SIZE	4096

#ifdef HAVE_UNIXODBC
char		field[MAX_SELECT_LIST_SIZE]
			[FETCH_AT_ONCE][MAX_ITEM_BUFFER_SIZE];
SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];
#else
char		field[MAX_SELECT_LIST_SIZE][MAX_ITEM_BUFFER_SIZE];
SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE];
#endif

int main(int argc, char **argv) {

	if (argc<4) {
		printf("usage: odbctest dsn query iterations\n");
		exit(0);
	}

	char	*dsn=argv[1];
	char	*query=argv[2];
	int	iterations=atoi(argv[3]);
	int	length=strlen(query);

	SQLHENV		env;
	SQLHDBC		dbc;
	SQLHSTMT	stmt;
	SQLSMALLINT	ncols;

	// init the timer
	time_t	starttime=time(NULL);
	clock();

	for (int count=0; count<iterations; count++) {

#ifdef HAVE_UNIXODBC
		SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
		SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
#else
		SQLAllocEnv(&env);
		SQLAllocConnect(env,&dbc);
#endif
		SQLConnect(dbc,(SQLCHAR *)dsn,SQL_NTS,
				(SQLCHAR *)"",SQL_NTS,
				(SQLCHAR *)"",SQL_NTS);

		// execute the query
#ifdef HAVE_UNIXODBC
		SQLAllocHandle(SQL_HANDLE_STMT,dbc,&stmt);
		SQLSetStmtAttr(stmt,SQL_ATTR_ROW_ARRAY_SIZE,
				(SQLPOINTER)FETCH_AT_ONCE,0);
#else
		SQLAllocStmt(dbc,&stmt);
#endif
		SQLPrepare(stmt,(SQLCHAR *)query,length);
		SQLExecute(stmt);
		SQLNumResultCols(stmt,&ncols);

		for (int i=0; i<ncols; i++) {
			SQLBindCol(stmt,i+1,SQL_C_CHAR,
					field[i],MAX_ITEM_BUFFER_SIZE,
					(SQLINTEGER *)indicator[i]);
		}

#ifdef HAVE_UNIXODBC
		// run through the rows
		int	oldrow=0;
		int	rownumber;
		for (;;) {
			SQLFetchScroll(stmt,SQL_FETCH_NEXT,0);
			SQLGetStmtAttr(stmt,SQL_ATTR_ROW_NUMBER,
					(SQLPOINTER)&rownumber,0,NULL);
			if (rownumber==oldrow) {
				break;
			}

			for (int row=oldrow; row<rownumber; row++) {
				for (int col=0; col<ncols; col++) {

					if (indicator[col][row]==
							SQL_NULL_DATA) {
						printf("NULL,");
					} else {
						printf("%s,",field[col][row]);
					}
				}
				printf("\n");
			}
			oldrow=rownumber;
		}
#else
		SQLFetch(stmt);
		for (int col=0; col<ncols; col++) {

			if (indicator[col]==SQL_NULL_DATA) {
				printf("NULL,");
			} else {
				printf("%s,",field[col]);
			}
		}
		printf("\n");
#endif

		// free the result set
#ifdef HAVE_UNIXODBC
		SQLFreeHandle(SQL_HANDLE_STMT,stmt);
#else
		SQLFreeStmt(stmt,SQL_DROP);
#endif

		// log off
		SQLDisconnect(dbc);
#ifdef HAVE_UNIXODBC
		SQLFreeHandle(SQL_HANDLE_DBC,dbc);
		SQLFreeHandle(SQL_HANDLE_ENV,env);
#else
		SQLFreeConnect(&dbc);
		SQLFreeEnv(&env);
#endif
	}

	printf("total system time used: %d\n",clock());
	printf("total real time: %d\n",time(NULL)-starttime);
}
