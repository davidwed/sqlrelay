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

void checkSuccessString(char *value, char *success) {

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

	char	*dbtype;
	char	*subvars[4]={"var1","var2","var3",NULL};
	char	*subvalstrings[3]={"hi","hello","bye"};
	long	subvallongs[3]={1,2,3};
	double	subvaldoubles[3]={10.55,10.556,10.5556};
	unsigned short	precs[3]={4,5,6};
	unsigned short	scales[3]={2,3,4};
	char	*numvar;
	char	*stringvar;
	char	*floatvar;
	char	**cols;
	char	**fields;
	int	port;
	char	*socket;
	int	id;
	char	*filename;
	long	*fieldlens;

	// usage...
	if (argc<5) {
		printf("usage: lago host port socket user password\n");
		exit(0);
	}


	// instantiation
	con=sqlrcon_alloc(argv[1],atoi(argv[2]), 
					argv[3],argv[4],argv[5],0,1);
	cur=sqlrcur_alloc(con);

	// get database type
	printf("IDENTIFY: \n");
	checkSuccessString(sqlrcon_identify(con),"lago");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccessInt(sqlrcon_ping(con),1);
	printf("\n");

	// drop existing table
	sqlrcur_sendQuery(cur,"drop table testtable");

	printf("CREATE TEMPTABLE: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable (testsmallint smallint, testint int, testfloat float, testdouble double, testdecimal decimal(1,1), testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (1,1,1.1,1.1,1.1,'testchar1','testvarchar1','20010101','010000')"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (2,2,2.1,2.1,2.1,'testchar2','testvarchar2','20020101','020000')"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (3,3,3.1,3.1,3.1,'testchar3','testvarchar3','20030101','030000')"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (4,4,4.1,4.1,4.1,'testchar4','testvarchar4','20040101','040000')"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccessInt(sqlrcur_affectedRows(cur),-1);
	printf("\n");

	printf("BIND BY NAME: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8,:var9)");
	sqlrcur_inputBindLong(cur,"var1",5);
	sqlrcur_inputBindLong(cur,"var2",5);
	sqlrcur_inputBindDouble(cur,"var3",5.1,2,1);
	sqlrcur_inputBindDouble(cur,"var4",5.1,2,1);
	sqlrcur_inputBindDouble(cur,"var5",5.1,2,1);
	sqlrcur_inputBindString(cur,"var6","testchar5");
	sqlrcur_inputBindString(cur,"var7","testvarchar5");
	sqlrcur_inputBindString(cur,"var8","20050101");
	sqlrcur_inputBindString(cur,"var9","050000");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",6);
	sqlrcur_inputBindLong(cur,"var2",6);
	sqlrcur_inputBindDouble(cur,"var3",6.1,2,1);
	sqlrcur_inputBindDouble(cur,"var4",6.1,2,1);
	sqlrcur_inputBindDouble(cur,"var5",6.1,2,1);
	sqlrcur_inputBindString(cur,"var6","testchar6");
	sqlrcur_inputBindString(cur,"var7","testvarchar6");
	sqlrcur_inputBindString(cur,"var8","20060101");
	sqlrcur_inputBindString(cur,"var9","060000");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",7);
	sqlrcur_inputBindLong(cur,"var2",7);
	sqlrcur_inputBindDouble(cur,"var3",7.1,2,1);
	sqlrcur_inputBindDouble(cur,"var4",7.1,2,1);
	sqlrcur_inputBindDouble(cur,"var5",7.1,2,1);
	sqlrcur_inputBindString(cur,"var6","testchar7");
	sqlrcur_inputBindString(cur,"var7","testvarchar7");
	sqlrcur_inputBindString(cur,"var8","20070101");
	sqlrcur_inputBindString(cur,"var9","070000");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",8);
	sqlrcur_inputBindLong(cur,"var2",8);
	sqlrcur_inputBindDouble(cur,"var3",8.1,2,1);
	sqlrcur_inputBindDouble(cur,"var4",8.1,2,1);
	sqlrcur_inputBindDouble(cur,"var5",8.1,2,1);
	sqlrcur_inputBindString(cur,"var6","testchar8");
	sqlrcur_inputBindString(cur,"var7","testvarchar8");
	sqlrcur_inputBindString(cur,"var8","20080101");
	sqlrcur_inputBindString(cur,"var9","080000");
	sqlrcur_inputBindString(cur,"var10","junkvalue");
	sqlrcur_validateBinds(cur);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("SELECT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccessInt(sqlrcur_colCount(cur),10);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testsmallint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testdouble");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testdecimal");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testvarchar");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testdate");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"testtime");
	checkSuccessString(sqlrcur_getColumnName(cur,9),"testtimestamp");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testsmallint");
	checkSuccessString(cols[1],"testint");
	checkSuccessString(cols[2],"testfloat");
	checkSuccessString(cols[3],"testdouble");
	checkSuccessString(cols[4],"testdecimal");
	checkSuccessString(cols[5],"testchar");
	checkSuccessString(cols[6],"testvarchar");
	checkSuccessString(cols[7],"testdate");
	checkSuccessString(cols[8],"testtime");
	checkSuccessString(cols[9],"testtimestamp");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"SMALLINT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testsmallint"),"SMALLINT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,1),"INT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testint"),"INT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,2),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testfloat"),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,3),"DOUBLE");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdouble"),"DOUBLE");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,4),"DOUBLE");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdecimal"),"DOUBLE");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,5),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testchar"),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,6),"VARCHAR");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testvarchar"),"VARCHAR");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,7),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdate"),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,8),"TIME");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtime"),"TIME");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,9),"TIMESTAMP");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtimestamp"),"TIMESTAMP");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),2);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testsmallint"),2);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,1),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testint"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,2),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testfloat"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,3),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdouble"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,4),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdecimal"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,5),40);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testchar"),40);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,6),40);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testvarchar"),40);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,7),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdate"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,8),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtime"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,9),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtimestamp"),8);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,0),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,1),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testint"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,2),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testfloat"),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,3),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdouble"),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,4),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdecimal"),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,5),40);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testchar"),40);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,6),12);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testvarchar"),12);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,7),11);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdate"),11);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,8),8);
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,3),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,4),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,5),"testchar1                               ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,6),"testvarchar1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,7)," 1-Jan-2001");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,8),"01:00:00");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,1),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),"8.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,3),"8.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,4),"8.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,5),"testchar8                               ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,6),"testvarchar8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,7)," 1-Jan-2008");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,8),"08:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,1),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,2),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,3),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,4),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,5),40);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,6),12);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,7),11);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,8),8);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,1),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,2),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,3),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,4),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,5),40);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,6),12);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,7),11);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,8),8);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testsmallint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testfloat"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdouble"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdecimal"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testchar"),"testchar1                               ");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testvarchar"),"testvarchar1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdate")," 1-Jan-2001");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testtime"),"01:00:00");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testsmallint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testfloat"),"8.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdouble"),"8.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdecimal"),"8.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testchar"),"testchar8                               ");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testvarchar"),"testvarchar8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdate")," 1-Jan-2008");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testtime"),"08:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdouble"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdecimal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testchar"),40);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testvarchar"),12);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdate"),11);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testtime"),8);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdouble"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdecimal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testchar"),40);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testvarchar"),12);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdate"),11);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testtime"),8);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=sqlrcur_getRow(cur,0);
	checkSuccessString(fields[0],"1");
	checkSuccessString(fields[1],"1");
	checkSuccessString(fields[2],"1.1");
	checkSuccessString(fields[3],"1.1");
	checkSuccessString(fields[4],"1.1");
	checkSuccessString(fields[5],"testchar1                               ");
	checkSuccessString(fields[6],"testvarchar1");
	checkSuccessString(fields[7]," 1-Jan-2001");
	checkSuccessString(fields[8],"01:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=sqlrcur_getRowLengths(cur,0);
	checkSuccessInt(fieldlens[0],1);
	checkSuccessInt(fieldlens[1],1);
	checkSuccessInt(fieldlens[2],3);
	checkSuccessInt(fieldlens[3],3);
	checkSuccessInt(fieldlens[4],3);
	checkSuccessInt(fieldlens[5],40);
	checkSuccessInt(fieldlens[6],12);
	checkSuccessInt(fieldlens[7],11);
	checkSuccessInt(fieldlens[8],8);
	printf("\n");

	printf("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_sendQuery(cur,"drop table testtable1");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable1 (col1 int, col2 varchar(40), col3 real)"),1);
	sqlrcur_prepareQuery(cur,"insert into testtable1 (col1, col2, col3) values ($(var1),'$(var2)',$(var3))");
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
	sqlrcur_sendQuery(cur,"drop table testtable1");
	sqlrcur_sendQuery(cur,"create table testtable1 (col1 char(2), col2 char(5), col3 char(3))");
	sqlrcur_prepareQuery(cur,"insert into testtable1 (col1, col2, col3) values ('$(var1)','$(var2)','$(var3)')");
	sqlrcur_subStrings(cur,subvars,subvalstrings);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"hi");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"hello");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"bye");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_sendQuery(cur,"drop table testtable1");
	sqlrcur_sendQuery(cur,"create table testtable1 (col1 int, col2 int, col3 int)");
	sqlrcur_prepareQuery(cur,"insert into testtable1 (col1, col2, col3) values ($(var1),$(var2),$(var3))");
	sqlrcur_subLongs(cur,subvars,subvallongs);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"3");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_sendQuery(cur,"drop table testtable1");
	sqlrcur_sendQuery(cur,"create table testtable1 (col1 real, col2 real, col3 real)");
	sqlrcur_prepareQuery(cur,"insert into testtable1 (col1, col2, col3) values ($(var1),$(var2),$(var3))");
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
	sqlrcur_sendQuery(cur,"drop table testtable1");
	sqlrcur_sendQuery(cur,"create table testtable1 (col1 char(1), col2 char(1), col3 char(1))");
	sqlrcur_getNullsAsNulls(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable1 (col1, col2, col3) values ('1',NULL,NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"");
	checkSuccessInt(sqlrcur_sendQuery(cur,"drop table testtable1"),1);
	printf("\n");

	printf("RESULT SET BUFFER SIZE: \n");
	checkSuccessInt(sqlrcur_getResultSetBufferSize(cur),0);
	sqlrcur_setResultSetBufferSize(cur,2);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),NULL);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),0);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),NULL);
	sqlrcur_getColumnInfo(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testsmallint");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),2);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"SMALLINT");
	printf("\n");

	printf("SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=sqlrcon_getConnectionSocket(con);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=sqlrcon_getConnectionSocket(con);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=sqlrcon_getConnectionSocket(con);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"3");
	id=sqlrcur_getResultSetId(cur);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=sqlrcon_getConnectionSocket(con);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
	filename=strdup(sqlrcur_getCacheFileName(cur));
	checkSuccessString(filename,"cachefile1");
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,filename),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	free(filename);
	printf("\n");

	printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccessInt(sqlrcur_colCount(cur),10);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testsmallint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testdouble");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testdecimal");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testvarchar");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testdate");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"testtime");
	checkSuccessString(sqlrcur_getColumnName(cur,9),"testtimestamp");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testsmallint");
	checkSuccessString(cols[1],"testint");
	checkSuccessString(cols[2],"testfloat");
	checkSuccessString(cols[3],"testdouble");
	checkSuccessString(cols[4],"testdecimal");
	checkSuccessString(cols[5],"testchar");
	checkSuccessString(cols[6],"testvarchar");
	checkSuccessString(cols[7],"testdate");
	checkSuccessString(cols[8],"testtime");
	checkSuccessString(cols[9],"testtimestamp");
	printf("\n");

	printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	sqlrcur_cacheToFile(cur,"cachefile1");
	sqlrcur_setCacheTtl(cur,200);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"3");
	filename=strdup(sqlrcur_getCacheFileName(cur));
	checkSuccessString(filename,"cachefile1");
	id=sqlrcur_getResultSetId(cur);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=sqlrcon_getConnectionSocket(con);
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
	secondcon=sqlrcon_alloc(argv[1],atoi(argv[2]), 
					argv[3],argv[4],argv[5],0,1);
	secondcur=sqlrcur_alloc(secondcon);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select * from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"1");
	checkSuccessInt(sqlrcon_commit(con),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select * from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"1");
	checkSuccessInt(sqlrcon_autoCommitOn(con),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable (testsmallint, testint, testfloat, testdouble, testdecimal, testchar, testvarchar, testdate, testtime) values (10,10,10.1,10.1,10.1,'testchar10','testvarchar10','20100101','100000')"),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select * from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,8,0),"10");
	checkSuccessInt(sqlrcon_autoCommitOff(con),1);
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
}
