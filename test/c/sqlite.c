// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include "../../config.h"
#include <sqlrelay/sqlrclientwrapper.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

sqlrcon	con;
sqlrcur	cur;
sqlrcon	secondcon;
sqlrcur	secondcur;

void checkSuccessString(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("\"%s\"!=\"%s\"",value,success);
			printf("failure ");
			sqlrcur_free(cur);
			sqlrcon_free(con);
			exit(1);
		}
	}

	if (!strcmp(value,success)) {
		printf("success ");
	} else {
		printf("\"%s\"!=\"%s\"",value,success);
		printf("failure ");
		sqlrcur_free(cur);
		sqlrcon_free(con);
		exit(1);
	}
}

void checkSuccessInt(int value, int success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("\"%d\"!=\"%d\"",value,success);
		printf("failure ");
		sqlrcur_free(cur);
		sqlrcon_free(con);
		exit(1);
	}
}

int	main(int argc, char **argv) {

	const char	*subvars[4]={"var1","var2","var3",NULL};
	const char	*subvalstrings[3]={"hi","hello","bye"};
	int64_t		subvallongs[3]={1,2,3};
	double		subvaldoubles[3]={10.55,10.556,10.5556};
	uint32_t	precs[3]={4,5,6};
	uint32_t	scales[3]={2,3,4};
	const char * const *cols;
	const char * const *fields;
	uint16_t	port;
	const char	*socket;
	uint16_t	id;
	char		*filename;
	uint32_t	*fieldlens;

	// instantiation
	con=sqlrcon_alloc("sqlrelay",9000,
				"/tmp/test.socket","test","test",0,1);
	cur=sqlrcur_alloc(con);

	// get database type
	printf("IDENTIFY: \n");
	checkSuccessString(sqlrcon_identify(con),"sqlite");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccessInt(sqlrcon_ping(con),1);
	printf("\n");

	// drop existing table
	sqlrcur_sendQuery(cur,"begin transaction");
	sqlrcur_sendQuery(cur,"drop table testtable");
	sqlrcon_commit(con);

	// create a new table
	printf("CREATE TEMPTABLE: \n");
	sqlrcur_sendQuery(cur,"begin transaction");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable (testint int, testfloat float, testchar char(40), testvarchar varchar(40))"),1);
	sqlrcon_commit(con);
	printf("\n");

	printf("INSERT: \n");
	sqlrcur_sendQuery(cur,"begin transaction");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,1.1,'testchar1','testvarchar1')"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (2,2.2,'testchar2','testvarchar2')"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (3,3.3,'testchar3','testvarchar3')"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (4,4.4,'testchar4','testvarchar4')"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccessInt(sqlrcur_affectedRows(cur),0);
	printf("\n");

	printf("BIND BY NAME: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable values (:var1,:var2,:var3,:var4)");
	checkSuccessInt(sqlrcur_countBindVariables(cur),4);
	sqlrcur_inputBindLong(cur,"var1",5);
	sqlrcur_inputBindDouble(cur,"var2",5.5,4,1);
	sqlrcur_inputBindString(cur,"var3","testchar5");
	sqlrcur_inputBindString(cur,"var4","testvarchar5");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",6);
	sqlrcur_inputBindDouble(cur,"var2",6.6,4,1);
	sqlrcur_inputBindString(cur,"var3","testchar6");
	sqlrcur_inputBindString(cur,"var4","testvarchar6");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",7);
	sqlrcur_inputBindDouble(cur,"var2",7.7,4,1);
	sqlrcur_inputBindString(cur,"var3","testchar7");
	sqlrcur_inputBindString(cur,"var4","testvarchar7");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",8);
	sqlrcur_inputBindDouble(cur,"var2",8.8,4,1);
	sqlrcur_inputBindString(cur,"var3","testchar8");
	sqlrcur_inputBindString(cur,"var4","testvarchar8");
	sqlrcur_validateBinds(cur);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("SELECT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccessInt(sqlrcur_colCount(cur),4);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testvarchar");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testint");
	checkSuccessString(cols[1],"testfloat");
	checkSuccessString(cols[2],"testchar");
	checkSuccessString(cols[3],"testvarchar");
	printf("\n");

	printf("COLUMN TYPES: \n");
	#ifdef HAVE_SQLITE3_STMT
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"INTEGER");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testint"),"INTEGER");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,1),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testfloat"),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,2),"STRING");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testchar"),"STRING");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,3),"STRING");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testvarchar"),"STRING");
	#else
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"UNKNOWN");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testint"),"UNKNOWN");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,1),"UNKNOWN");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testfloat"),"UNKNOWN");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,2),"UNKNOWN");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testchar"),"UNKNOWN");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,3),"UNKNOWN");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testvarchar"),"UNKNOWN");
	#endif
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),0);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testint"),0);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,1),0);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testfloat"),0);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,2),0);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testchar"),0);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,3),0);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testvarchar"),0);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,0),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testint"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,1),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testfloat"),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,2),9);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testchar"),9);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,3),12);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testvarchar"),12);
	printf("\n");

	printf("ROW COUNT: \n");
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	printf("\n");

	printf("TOTAL ROWS: \n");
	#ifdef HAVE_SQLITE3_STMT
	checkSuccessInt(sqlrcur_totalRows(cur),0);
	#else
	checkSuccessInt(sqlrcur_totalRows(cur),8);
	#endif
	printf("\n");

	printf("FIRST ROW INDEX: \n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),0);
	printf("\n");

	printf("END OF RESULT SET: \n");
	checkSuccessInt(sqlrcur_endOfResultSet(cur),1);
	printf("\n");

	printf("FIELDS BY INDEX: \n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"testchar1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,3),"testvarchar1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,1),"8.8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),"testchar8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,3),"testvarchar8");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,1),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,2),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,3),12);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,1),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,2),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,3),12);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testfloat"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testchar"),"testchar1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testvarchar"),"testvarchar1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testfloat"),"8.8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testchar"),"testchar8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testvarchar"),"testvarchar8");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testchar"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testvarchar"),12);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testchar"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testvarchar"),12);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=sqlrcur_getRow(cur,0);
	checkSuccessString(fields[0],"1");
	checkSuccessString(fields[1],"1.1");
	checkSuccessString(fields[2],"testchar1");
	checkSuccessString(fields[3],"testvarchar1");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=sqlrcur_getRowLengths(cur,0);
	checkSuccessInt(fieldlens[0],1);
	checkSuccessInt(fieldlens[1],3);
	checkSuccessInt(fieldlens[2],9);
	checkSuccessInt(fieldlens[3],12);
	printf("\n");

	printf("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_sendQuery(cur,"drop table testtable1");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable1 (col1 int, col2 char, col3 float)"),1);
	sqlrcur_prepareQuery(cur,"insert into testtable1 values ($(var1),'$(var2)',$(var3))");
	sqlrcur_subLong(cur,"var1",1);
	sqlrcur_subString(cur,"var2","hello");
	sqlrcur_subDouble(cur,"var3",10.5556,6,4);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"hello");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"10.5556");
	checkSuccessInt(sqlrcur_sendQuery(cur,"delete from testtable1"),1);
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable1 values ('$(var1)','$(var2)','$(var3)')");
	sqlrcur_subStrings(cur,subvars,subvalstrings);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"hi");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"hello");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"bye");
	checkSuccessInt(sqlrcur_sendQuery(cur,"delete from testtable1"),1);
	printf("\n");


	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable1 values ($(var1),'$(var2)',$(var3))");
	sqlrcur_subLongs(cur,subvars,subvallongs);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"3.0");
	checkSuccessInt(sqlrcur_sendQuery(cur,"delete from testtable1"),1);
	printf("\n");


	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable1 values ($(var1),'$(var2)',$(var3))");
	sqlrcur_subDoubles(cur,subvars,subvaldoubles,precs,scales);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"10.55");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"10.556");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"10.5556");
	checkSuccessInt(sqlrcur_sendQuery(cur,"delete from testtable1"),1);
	printf("\n");


	printf("NULLS as Nulls: \n");
	sqlrcur_getNullsAsNulls(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable1 values (1,NULL,NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"");
	sqlrcur_getNullsAsNulls(cur);
	printf("\n");

	printf("RESULT SET BUFFER SIZE: \n");
	checkSuccessInt(sqlrcur_getResultSetBufferSize(cur),0);
	sqlrcur_setResultSetBufferSize(cur,2);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	checkSuccessInt(sqlrcur_getResultSetBufferSize(cur),2);
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),0);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),0);
	checkSuccessInt(sqlrcur_rowCount(cur),2);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,1,0),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"3");
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),2);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),0);
	checkSuccessInt(sqlrcur_rowCount(cur),4);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),"7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),6);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),0);
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),8);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),1);
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	printf("\n");

	printf("DONT GET COLUMN INFO: \n");
	sqlrcur_dontGetColumnInfo(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),NULL);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),0);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),NULL);
	sqlrcur_getColumnInfo(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testint");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),0);
	#ifdef HAVE_SQLITE3_STMT
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"INTEGER");
	#else
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"UNKNOWN");
	#endif
	printf("\n");

	printf("SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,1,0),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"3");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,3,0),"4");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,0),"5");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,0),"6");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),"7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	printf("\n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,1,0),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"3");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,3,0),"4");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,0),"5");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,0),"6");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),"7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	printf("\n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,1,0),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"3");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,3,0),"4");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,0),"5");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,0),"6");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),"7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	printf("\n");

	printf("SUSPENDED RESULT SET: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"3");
	id=sqlrcur_getResultSetId(cur);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	checkSuccessInt(sqlrcur_resumeResultSet(cur,id),1);
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),4);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),0);
	checkSuccessInt(sqlrcur_rowCount(cur),6);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),6);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),0);
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),8);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),1);
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	sqlrcur_setResultSetBufferSize(cur,0);
	printf("\n");

	printf("CACHED RESULT SET: \n");
	sqlrcur_cacheToFile(cur,"cachefile1");
	sqlrcur_setCacheTtl(cur,200);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	filename=strdup(sqlrcur_getCacheFileName(cur));
	checkSuccessString(filename,"cachefile1");
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,filename),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	free(filename);
	printf("\n");

	printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccessInt(sqlrcur_colCount(cur),4);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testvarchar");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testint");
	checkSuccessString(cols[1],"testfloat");
	checkSuccessString(cols[2],"testchar");
	checkSuccessString(cols[3],"testvarchar");
	printf("\n");

	printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	sqlrcur_cacheToFile(cur,"cachefile1");
	sqlrcur_setCacheTtl(cur,200);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	filename=strdup(sqlrcur_getCacheFileName(cur));
	checkSuccessString(filename,"cachefile1");
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,filename),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize(cur,0);
	free(filename);
	printf("\n");

	printf("FROM ONE CACHE FILE TO ANOTHER: \n");
	sqlrcur_cacheToFile(cur,"cachefile2");
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,"cachefile1"),1);
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,"cachefile2"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	printf("\n");

	printf("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	sqlrcur_cacheToFile(cur,"cachefile2");
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,"cachefile1"),1);
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,"cachefile2"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize(cur,0);
	printf("\n");

	printf("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	sqlrcur_cacheToFile(cur,"cachefile1");
	sqlrcur_setCacheTtl(cur,200);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"3");
	filename=strdup(sqlrcur_getCacheFileName(cur));
	checkSuccessString(filename,"cachefile1");
	id=sqlrcur_getResultSetId(cur);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	printf("\n");
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	checkSuccessInt(sqlrcur_resumeCachedResultSet(cur,id,filename),1);
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),4);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),0);
	checkSuccessInt(sqlrcur_rowCount(cur),6);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),6);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),0);
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),8);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),1);
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	sqlrcur_cacheOff(cur);
	printf("\n");
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,filename),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize(cur,0);
	free(filename);
	printf("\n");

	printf("COMMIT AND ROLLBACK: \n");
	secondcon=sqlrcon_alloc("sqlrelay",9000,
				"/tmp/test.socket","test","test",0,1);
	secondcur=sqlrcur_alloc(secondcon);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"0");
	checkSuccessInt(sqlrcon_commit(con),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"8");
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"insert into testtable values (10,10.1,'testchar10','testvarchar10')"),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"9");
	printf("\n");

	printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,0),"5");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,0),"6");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),"7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	id=sqlrcur_getResultSetId(cur);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	checkSuccessInt(sqlrcur_resumeResultSet(cur,id),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,0),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,0),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),NULL);
	printf("\n");

	// drop existing table
	sqlrcur_sendQuery(cur,"drop table testtable");

	// invalid queries...
	printf("INVALID QUERIES: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),0);
	printf("\n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,2,3,4)"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,2,3,4)"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,2,3,4)"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,2,3,4)"),0);
	printf("\n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable"),0);
	printf("\n");

	return 0;
}
