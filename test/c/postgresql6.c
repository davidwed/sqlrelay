// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

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
			printf("failure ");
			sqlrcur_free(cur);
			sqlrcon_free(con);
			exit(0);
		}
	}

	if (!strcmp(value,success)) {
		printf("success ");
	} else {
		printf("failure ");
		sqlrcur_free(cur);
		sqlrcon_free(con);
		exit(0);
	}
}

void checkSuccessInt(int value, int success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("failure ");
		sqlrcur_free(cur);
		sqlrcon_free(con);
		exit(0);
	}
}

int	main(int argc, char **argv) {

	const char	*dbtype;
	char	*subvars[4]={"var1","var2","var3",NULL};
	char	*subvalstrings[3]={"hi","hello","bye"};
	long	subvallongs[3]={1,2,3};
	double	subvaldoubles[3]={10.55,10.556,10.5556};
	unsigned short	precs[3]={4,5,6};
	unsigned short	scales[3]={2,3,4};
	const char	*numvar;
	const char	*stringvar;
	const char	*floatvar;
	const char * const *cols;
	const char * const *fields;
	int	port;
	const char	*socket;
	int	id;
	const char	*filename;
	long	*fieldlens;

	// usage...
	if (argc<5) {
		printf("usage: postgresql host port socket user password\n");
		exit(0);
	}


	// instantiation
	con=sqlrcon_alloc(argv[1],atoi(argv[2]), 
					argv[3],argv[4],argv[5],0,1);
	cur=sqlrcur_alloc(con);

	printf("IDENTIFY: \n");
	checkSuccessString(sqlrcon_identify(con),"postgresql");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccessInt(sqlrcon_ping(con),1);
	printf("\n");

	// drop existing table
	sqlrcur_sendQuery(cur,"drop table testtable");

	printf("CREATE TEMPTABLE: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
	printf("\n");

	printf("BEGIN TRANSCTION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"begin"),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (3,3.3,3.3,3,'testchar3','testvarchar3','01/01/2003','03:00:00',NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (4,4.4,4.4,4,'testchar4','testvarchar4','01/01/2004','04:00:00',NULL)"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccessInt(sqlrcur_affectedRows(cur),1);
	printf("\n");

	printf("BIND BY NAME: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8)");
	checkSuccessInt(sqlrcur_countBindVariables(cur),8);
	sqlrcur_inputBindLong(cur,"var1",5);
	sqlrcur_inputBindDouble(cur,"var2",5.5,4,2);
	sqlrcur_inputBindDouble(cur,"var3",5.5,4,2);
	sqlrcur_inputBindLong(cur,"var4",5);
	sqlrcur_inputBindString(cur,"var5","testchar5");
	sqlrcur_inputBindString(cur,"var6","testvarchar5");
	sqlrcur_inputBindString(cur,"var7","01/01/2005");
	sqlrcur_inputBindString(cur,"var8","05:00:00");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",6);
	sqlrcur_inputBindDouble(cur,"var2",6.6,4,2);
	sqlrcur_inputBindDouble(cur,"var3",6.6,4,2);
	sqlrcur_inputBindLong(cur,"var4",6);
	sqlrcur_inputBindString(cur,"var5","testchar6");
	sqlrcur_inputBindString(cur,"var6","testvarchar6");
	sqlrcur_inputBindString(cur,"var7","01/01/2006");
	sqlrcur_inputBindString(cur,"var8","06:00:00");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",7);
	sqlrcur_inputBindDouble(cur,"var2",7.7,4,2);
	sqlrcur_inputBindDouble(cur,"var3",7.7,4,2);
	sqlrcur_inputBindLong(cur,"var4",7);
	sqlrcur_inputBindString(cur,"var5","testchar7");
	sqlrcur_inputBindString(cur,"var6","testvarchar7");
	sqlrcur_inputBindString(cur,"var7","01/01/2007");
	sqlrcur_inputBindString(cur,"var8","07:00:00");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",8);
	sqlrcur_inputBindDouble(cur,"var2",8.8,4,2);
	sqlrcur_inputBindDouble(cur,"var3",8.8,4,2);
	sqlrcur_inputBindLong(cur,"var4",8);
	sqlrcur_inputBindString(cur,"var5","testchar8");
	sqlrcur_inputBindString(cur,"var6","testvarchar8");
	sqlrcur_inputBindString(cur,"var7","01/01/2008");
	sqlrcur_inputBindString(cur,"var8","08:00:00");
	sqlrcur_inputBindString(cur,"var9","junkvalue");
	sqlrcur_validateBinds(cur);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("SELECT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccessInt(sqlrcur_colCount(cur),9);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testreal");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testsmallint");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testvarchar");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testdate");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testtime");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"testtimestamp");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testint");
	checkSuccessString(cols[1],"testfloat");
	checkSuccessString(cols[2],"testreal");
	checkSuccessString(cols[3],"testsmallint");
	checkSuccessString(cols[4],"testchar");
	checkSuccessString(cols[5],"testvarchar");
	checkSuccessString(cols[6],"testdate");
	checkSuccessString(cols[7],"testtime");
	checkSuccessString(cols[8],"testtimestamp");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"int4");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testint"),"int4");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,1),"float8");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testfloat"),"float8");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,2),"float8");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testreal"),"float8");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,3),"int2");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testsmallint"),"int2");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,4),"bpchar");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testchar"),"bpchar");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,5),"varchar");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testvarchar"),"varchar");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,6),"date");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdate"),"date");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,7),"time");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtime"),"time");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,8),"timestamp");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtimestamp"),"timestamp");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testint"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,1),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testfloat"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,2),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testreal"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,3),2);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testsmallint"),2);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,4),44);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testchar"),44);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,5),44);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testvarchar"),44);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,6),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdate"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,7),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtime"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,8),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtimestamp"),4);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,0),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testint"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,1),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testfloat"),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,2),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testreal"),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,3),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,4),40);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testchar"),40);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,5),12);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testvarchar"),12);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,6),10);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdate"),10);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,7),8);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testtime"),8);
	printf("\n");

	printf("ROW COUNT: \n");
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	printf("\n");

	printf("TOTAL ROWS: \n");
	checkSuccessInt(sqlrcur_totalRows(cur),8);
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,3),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,4),"testchar1                               ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,5),"testvarchar1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,6),"01-01-2001");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,7),"01:00:00");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,1),"8.8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),"8.8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,3),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,4),"testchar8                               ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,5),"testvarchar8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,6),"01-01-2008");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,7),"08:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,1),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,2),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,3),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,4),40);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,5),12);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,6),10);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,7),8);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,1),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,2),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,3),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,4),40);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,5),12);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,6),10);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,7),8);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testfloat"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testreal"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testsmallint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testchar"),"testchar1                               ");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testvarchar"),"testvarchar1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdate"),"01-01-2001");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testtime"),"01:00:00");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testfloat"),"8.8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testreal"),"8.8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testsmallint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testchar"),"testchar8                               ");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testvarchar"),"testvarchar8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdate"),"01-01-2008");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testtime"),"08:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testreal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testchar"),40);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testvarchar"),12);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdate"),10);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testtime"),8);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testreal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testchar"),40);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testvarchar"),12);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdate"),10);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testtime"),8);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=sqlrcur_getRow(cur,0);
	checkSuccessString(fields[0],"1");
	checkSuccessString(fields[1],"1.1");
	checkSuccessString(fields[2],"1.1");
	checkSuccessString(fields[3],"1");
	checkSuccessString(fields[4],"testchar1                               ");
	checkSuccessString(fields[5],"testvarchar1");
	checkSuccessString(fields[6],"01-01-2001");
	checkSuccessString(fields[7],"01:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=sqlrcur_getRowLengths(cur,0);
	checkSuccessInt(fieldlens[0],1);
	checkSuccessInt(fieldlens[1],3);
	checkSuccessInt(fieldlens[2],3);
	checkSuccessInt(fieldlens[3],1);
	checkSuccessInt(fieldlens[4],40);
	checkSuccessInt(fieldlens[5],12);
	checkSuccessInt(fieldlens[6],10);
	checkSuccessInt(fieldlens[7],8);
	printf("\n");

	printf("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select $(var1),'$(var2)',$(var3)");
	sqlrcur_subLong(cur,"var1",1);
	sqlrcur_subString(cur,"var2","hello");
	sqlrcur_subDouble(cur,"var3",10.5556,6,4);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"hello");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"10.5556");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select $(var1),$(var2),$(var3)");
	sqlrcur_subLongs(cur,subvars,subvallongs);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");
	
	printf("FIELDS: \n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"3");
	printf("\n");
	
	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select '$(var1)','$(var2)','$(var3)'");
	sqlrcur_subStrings(cur,subvars,subvalstrings);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"hi");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"hello");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"bye");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select $(var1),$(var2),$(var3)");
	sqlrcur_subDoubles(cur,subvars,subvaldoubles,precs,scales);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"10.55");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"10.556");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"10.5556");
	printf("\n");

	printf("NULLS as Nulls: \n");
	sqlrcur_getNullsAsNulls(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select NULL,1,NULL"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select NULL,1,NULL"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"1");
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
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),4);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"int4");
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
	checkSuccessInt(sqlrcur_colCount(cur),9);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testreal");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testsmallint");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testvarchar");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testdate");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testtime");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"testtimestamp");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testint");
	checkSuccessString(cols[1],"testfloat");
	checkSuccessString(cols[2],"testreal");
	checkSuccessString(cols[3],"testsmallint");
	checkSuccessString(cols[4],"testchar");
	checkSuccessString(cols[5],"testvarchar");
	checkSuccessString(cols[6],"testdate");
	checkSuccessString(cols[7],"testtime");
	checkSuccessString(cols[8],"testtimestamp");
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
	secondcon=sqlrcon_alloc(argv[1],
				atoi(argv[2]), 
				argv[3],argv[4],argv[5],0,1);
	secondcur=sqlrcur_alloc(secondcon);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"0");
	checkSuccessInt(sqlrcon_commit(con),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"8");
	//checkSuccessInt(sqlrcon_autoCommitOn(con),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (10,10.1,10.1,10,'testchar10','testvarchar10','01/01/2010','10:00:00',NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"9");
	//checkSuccessInt(sqlrcon_autoCommitOff(con),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),0);
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
}
