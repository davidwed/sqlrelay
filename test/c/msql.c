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
		printf("usage: msql host port socket user password\n");
		exit(0);
	}


	// instantiation
	con=sqlrcon_alloc(argv[1],atoi(argv[2]), 
					argv[3],argv[4],argv[5],0,1);
	cur=sqlrcur_alloc(con);

	// get database type
	printf("IDENTIFY: \n");
	checkSuccessString(sqlrcon_identify(con),"msql");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccessInt(sqlrcon_ping(con),1);
	printf("\n");

	// drop existing table
	sqlrcur_sendQuery(cur,"drop table testtable");

	printf("CREATE TEMPTABLE: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable (testchar char(40), testdate date, testint int, testmoney money, testreal real, testtext text(40), testtime time, testuint uint)"),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values ('char1','01-Jan-2001',1,1.00,1.1,'text1','01:00:00',1)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values ('char2','01-Jan-2002',2,2.00,2.1,'text2','02:00:00',2)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values ('char3','01-Jan-2003',3,3.00,3.1,'text3','03:00:00',3)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values ('char4','01-Jan-2004',4,4.00,4.1,'text4','04:00:00',4)"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccessInt(sqlrcur_affectedRows(cur),-1);
	printf("\n");

	printf("BIND BY NAME: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8)");
	checkSuccessInt(sqlrcur_countBindVariables(cur),8);
	sqlrcur_inputBindString(cur,"var1","char5");
	sqlrcur_inputBindString(cur,"var2","01-Jan-2005");
	sqlrcur_inputBindLong(cur,"var3",5);
	sqlrcur_inputBindDouble(cur,"var4",5.00,3,2);
	sqlrcur_inputBindDouble(cur,"var5",5.1,2,1);
	sqlrcur_inputBindString(cur,"var6","text5");
	sqlrcur_inputBindString(cur,"var7","05:00:00");
	sqlrcur_inputBindLong(cur,"var8",5);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindString(cur,"var1","char6");
	sqlrcur_inputBindString(cur,"var2","01-Jan-2006");
	sqlrcur_inputBindLong(cur,"var3",6);
	sqlrcur_inputBindDouble(cur,"var4",6.00,3,2);
	sqlrcur_inputBindDouble(cur,"var5",6.1,2,1);
	sqlrcur_inputBindString(cur,"var6","text6");
	sqlrcur_inputBindString(cur,"var7","06:00:00");
	sqlrcur_inputBindLong(cur,"var8",6);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindString(cur,"var1","char7");
	sqlrcur_inputBindString(cur,"var2","01-Jan-2007");
	sqlrcur_inputBindLong(cur,"var3",7);
	sqlrcur_inputBindDouble(cur,"var4",7.00,3,2);
	sqlrcur_inputBindDouble(cur,"var5",7.1,2,1);
	sqlrcur_inputBindString(cur,"var6","text7");
	sqlrcur_inputBindString(cur,"var7","07:00:00");
	sqlrcur_inputBindLong(cur,"var8",7);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindString(cur,"var1","char8");
	sqlrcur_inputBindString(cur,"var2","01-Jan-2008");
	sqlrcur_inputBindLong(cur,"var3",8);
	sqlrcur_inputBindDouble(cur,"var4",8.00,3,2);
	sqlrcur_inputBindDouble(cur,"var5",8.1,2,1);
	sqlrcur_inputBindString(cur,"var6","text8");
	sqlrcur_inputBindString(cur,"var7","08:00:00");
	sqlrcur_inputBindLong(cur,"var8",8);
	sqlrcur_inputBindString(cur,"var9","junkvalue");
	sqlrcur_validateBinds(cur);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("SELECT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccessInt(sqlrcur_colCount(cur),8);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testdate");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testmoney");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testreal");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testtext");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testtime");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testuint");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testchar");
	checkSuccessString(cols[1],"testdate");
	checkSuccessString(cols[2],"testint");
	checkSuccessString(cols[3],"testmoney");
	checkSuccessString(cols[4],"testreal");
	checkSuccessString(cols[5],"testtext");
	checkSuccessString(cols[6],"testtime");
	checkSuccessString(cols[7],"testuint");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testchar"),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,1),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdate"),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,2),"INT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testint"),"INT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,3),"MONEY");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testmoney"),"MONEY");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,4),"REAL");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testreal"),"REAL");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,5),"TEXT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtext"),"TEXT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,6),"TIME");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtime"),"TIME");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,7),"UINT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testuint"),"UINT");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),40);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testchar"),40);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,1),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdate"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,2),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testint"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,3),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testmoney"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,4),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testreal"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,5),40);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtext"),40);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,6),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtime"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,7),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testuint"),4);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,0),5);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testchar"),5);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,1),11);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdate"),11);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,2),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testint"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,3),4);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testmoney"),4);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,4),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testreal"),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,5),5);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testtext"),5);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,6),8);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testtime"),8);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,7),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testuint"),1);
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"char1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"01-Jan-2001");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,3),"1.00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,4),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,5),"text1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,6),"01:00:00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,7),"1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,1),"01-Jan-2008");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,3),"8.00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,4),"8.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,5),"text8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,6),"08:00:00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,7),"8");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,0),5);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,1),11);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,2),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,3),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,4),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,5),5);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,6),8);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,7),1);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,0),5);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,1),11);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,2),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,3),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,4),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,5),5);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,6),8);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,7),1);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testchar"),"char1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdate"),"01-Jan-2001");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testmoney"),"1.00");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testreal"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testtext"),"text1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testtime"),"01:00:00");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testuint"),"1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testchar"),"char8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdate"),"01-Jan-2008");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testmoney"),"8.00");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testreal"),"8.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testtext"),"text8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testtime"),"08:00:00");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testuint"),"8");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testchar"),5);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdate"),11);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testmoney"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testreal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testtext"),5);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testtime"),8);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testuint"),1);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testchar"),5);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdate"),11);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testmoney"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testreal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testtext"),5);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testtime"),8);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testuint"),1);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=sqlrcur_getRow(cur,0);
	checkSuccessString(fields[0],"char1");
	checkSuccessString(fields[1],"01-Jan-2001");
	checkSuccessString(fields[2],"1");
	checkSuccessString(fields[3],"1.00");
	checkSuccessString(fields[4],"1.1");
	checkSuccessString(fields[5],"text1");
	checkSuccessString(fields[6],"01:00:00");
	checkSuccessString(fields[7],"1");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=sqlrcur_getRowLengths(cur,0);
	checkSuccessInt(fieldlens[0],5);
	checkSuccessInt(fieldlens[1],11);
	checkSuccessInt(fieldlens[2],1);
	checkSuccessInt(fieldlens[3],4);
	checkSuccessInt(fieldlens[4],3);
	checkSuccessInt(fieldlens[5],5);
	checkSuccessInt(fieldlens[6],8);
	checkSuccessInt(fieldlens[7],1);
	printf("\n");

	printf("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_sendQuery(cur,"drop table testtable1");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable1 (col1 int, col2 char(40), col3 real)"),1);
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
	sqlrcur_sendQuery(cur,"drop table testtable1");
	sqlrcur_sendQuery(cur,"create table testtable1 (col1 char(40), col2 char(40), col3 char(40))");
	sqlrcur_prepareQuery(cur,"insert into testtable1 values ('$(var1)','$(var2)','$(var3)')");
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
	sqlrcur_prepareQuery(cur,"insert into testtable1 values ($(var1),$(var2),$(var3))");
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
	sqlrcur_prepareQuery(cur,"insert into testtable1 values ($(var1),$(var2),$(var3))");
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
	sqlrcur_sendQuery(cur,"create table testtable1 (col1 char(40), col2 char(40), col3 char(40))");
	sqlrcur_getNullsAsNulls(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable1 values ('1',NULL,NULL)"),1);
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"char1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,1,0),"char2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"char3");
	printf("\n");
	checkSuccessInt(sqlrcur_firstRowIndex(cur),2);
	checkSuccessInt(sqlrcur_endOfResultSet(cur),0);
	checkSuccessInt(sqlrcur_rowCount(cur),4);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),"char7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
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
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testchar");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),40);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"CHAR");
	printf("\n");

	printf("SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"char1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,1,0),"char2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"char3");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,3,0),"char4");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,0),"char5");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,0),"char6");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),"char7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
	printf("\n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"char1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,1,0),"char2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"char3");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,3,0),"char4");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,0),"char5");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,0),"char6");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),"char7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
	printf("\n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"char1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,1,0),"char2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"char3");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,3,0),"char4");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,0),"char5");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,0),"char6");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,0),"char7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
	printf("\n");

	printf("SUSPENDED RESULT SET: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"char3");
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
	free(filename);
	printf("\n");

	printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccessInt(sqlrcur_colCount(cur),8);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testdate");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testmoney");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testreal");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testtext");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testtime");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testuint");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testchar");
	checkSuccessString(cols[1],"testdate");
	checkSuccessString(cols[2],"testint");
	checkSuccessString(cols[3],"testmoney");
	checkSuccessString(cols[4],"testreal");
	checkSuccessString(cols[5],"testtext");
	checkSuccessString(cols[6],"testtime");
	checkSuccessString(cols[7],"testuint");
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize(cur,0);
	free(filename);
	printf("\n");

	printf("FROM ONE CACHE FILE TO ANOTHER: \n");
	sqlrcur_cacheToFile(cur,"cachefile2");
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,"cachefile1"),1);
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,"cachefile2"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	printf("\n");

	printf("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	sqlrcur_cacheToFile(cur,"cachefile2");
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,"cachefile1"),1);
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,"cachefile2"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize(cur,0);
	printf("\n");

	printf("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	sqlrcur_cacheToFile(cur,"cachefile1");
	sqlrcur_setCacheTtl(cur,200);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,2,0),"char3");
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"char8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize(cur,0);
	free(filename);
	printf("\n");

	printf("COMMIT AND ROLLBACK: \n");
	secondcon=sqlrcon_alloc(argv[1],atoi(argv[2]), 
					argv[3],argv[4],argv[5],0,1);
	secondcur=sqlrcur_alloc(secondcon);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"char1");
	checkSuccessInt(sqlrcon_commit(con),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"char1");
	checkSuccessInt(sqlrcon_autoCommitOn(con),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values ('char10','01-Jan-2010',10,10.00,10.1,'text10','10:00:00',10)"),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,8,0),"char10");
	checkSuccessInt(sqlrcon_autoCommitOff(con),1);
	printf("\n");

	printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,2),"5");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,2),"6");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,2),"7");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),"8");
	id=sqlrcur_getResultSetId(cur);
	sqlrcur_suspendResultSet(cur);
	checkSuccessInt(sqlrcon_suspendSession(con),1);
	port=sqlrcon_getConnectionPort(con);
	socket=strdup(sqlrcon_getConnectionSocket(con));
	checkSuccessInt(sqlrcon_resumeSession(con,port,socket),1);
	checkSuccessInt(sqlrcur_resumeResultSet(cur,id),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,4,2),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,5,2),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,6,2),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),NULL);
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
