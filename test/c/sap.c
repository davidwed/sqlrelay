// Copyright (c) 1999-2018 David Muse
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
	checkSuccessString(sqlrcon_identify(con),"sap");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccessInt(sqlrcon_ping(con),1);
	printf("\n");

	// drop existing table
	sqlrcur_sendQuery(cur,"drop table testtable");

	printf("CREATE TEMPTABLE: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable (testint int, testsmallint smallint, testtinyint tinyint, testreal real, testfloat float, testdecimal decimal(4,1), testnumeric numeric(4,1), testmoney money, testsmallmoney smallmoney, testdatetime datetime, testsmalldatetime smalldatetime, testchar char(40), testvarchar varchar(40), testbit bit)"),1);
	printf("\n");

	printf("BEGIN TRANSACTION: \n");
	//checkSuccessInt(sqlrcur_sendQuery(cur,"begin tran"),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,1,1,1.1,1.1,1.1,1.1,1.00,1.00,'01-Jan-2001 01:00:00','01-Jan-2001 01:00:00','testchar1','testvarchar1',1)"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccessInt(sqlrcur_affectedRows(cur),1);
	printf("\n");

	printf("BIND BY POSITION: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable values (@var1,@var2,@var3,@var4,@var5,@var6,@var7,@var8,@var9,@var10,@var11,@var12,@var13,@var14)");
	checkSuccessInt(sqlrcur_countBindVariables(cur),14);
	sqlrcur_inputBindLong(cur,"1",2);
	sqlrcur_inputBindLong(cur,"2",2);
	sqlrcur_inputBindLong(cur,"3",2);
	sqlrcur_inputBindDouble(cur,"4",2.2,2,1);
	sqlrcur_inputBindDouble(cur,"5",2.2,2,1);
	sqlrcur_inputBindDouble(cur,"6",2.2,2,1);
	sqlrcur_inputBindDouble(cur,"7",2.2,2,1);
	sqlrcur_inputBindDouble(cur,"8",2.00,3,2);
	sqlrcur_inputBindDouble(cur,"9",2.00,3,2);
	sqlrcur_inputBindString(cur,"10","01-Jan-2002 02:00:00");
	sqlrcur_inputBindString(cur,"11","01-Jan-2002 02:00:00");
	sqlrcur_inputBindString(cur,"12","testchar2");
	sqlrcur_inputBindString(cur,"13","testvarchar2");
	sqlrcur_inputBindLong(cur,"14",1);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"1",3);
	sqlrcur_inputBindLong(cur,"2",3);
	sqlrcur_inputBindLong(cur,"3",3);
	sqlrcur_inputBindDouble(cur,"4",3.3,2,1);
	sqlrcur_inputBindDouble(cur,"5",3.3,2,1);
	sqlrcur_inputBindDouble(cur,"6",3.3,2,1);
	sqlrcur_inputBindDouble(cur,"7",3.3,2,1);
	sqlrcur_inputBindDouble(cur,"8",3.00,3,2);
	sqlrcur_inputBindDouble(cur,"9",3.00,3,2);
	sqlrcur_inputBindString(cur,"10","01-Jan-2003 03:00:00");
	sqlrcur_inputBindString(cur,"11","01-Jan-2003 03:00:00");
	sqlrcur_inputBindString(cur,"12","testchar3");
	sqlrcur_inputBindString(cur,"13","testvarchar3");
	sqlrcur_inputBindLong(cur,"14",1);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"1",4);
	sqlrcur_inputBindLong(cur,"2",4);
	sqlrcur_inputBindLong(cur,"3",4);
	sqlrcur_inputBindDouble(cur,"4",4.4,2,1);
	sqlrcur_inputBindDouble(cur,"5",4.4,2,1);
	sqlrcur_inputBindDouble(cur,"6",4.4,2,1);
	sqlrcur_inputBindDouble(cur,"7",4.4,2,1);
	sqlrcur_inputBindDouble(cur,"8",4.00,3,2);
	sqlrcur_inputBindDouble(cur,"9",4.00,3,2);
	sqlrcur_inputBindString(cur,"10","01-Jan-2004 04:00:00");
	sqlrcur_inputBindString(cur,"11","01-Jan-2004 04:00:00");
	sqlrcur_inputBindString(cur,"12","testchar4");
	sqlrcur_inputBindString(cur,"13","testvarchar4");
	sqlrcur_inputBindLong(cur,"14",1);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("BIND BY NAME: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",5);
	sqlrcur_inputBindLong(cur,"var2",5);
	sqlrcur_inputBindLong(cur,"var3",5);
	sqlrcur_inputBindDouble(cur,"var4",5.5,2,1);
	sqlrcur_inputBindDouble(cur,"var5",5.5,2,1);
	sqlrcur_inputBindDouble(cur,"var6",5.5,2,1);
	sqlrcur_inputBindDouble(cur,"var7",5.5,2,1);
	sqlrcur_inputBindDouble(cur,"var8",5.00,3,2);
	sqlrcur_inputBindDouble(cur,"var9",5.00,3,2);
	sqlrcur_inputBindString(cur,"var10","01-Jan-2005 05:00:00");
	sqlrcur_inputBindString(cur,"var11","01-Jan-2005 05:00:00");
	sqlrcur_inputBindString(cur,"var12","testchar5");
	sqlrcur_inputBindString(cur,"var13","testvarchar5");
	sqlrcur_inputBindLong(cur,"var14",1);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",6);
	sqlrcur_inputBindLong(cur,"var2",6);
	sqlrcur_inputBindLong(cur,"var3",6);
	sqlrcur_inputBindDouble(cur,"var4",6.6,2,1);
	sqlrcur_inputBindDouble(cur,"var5",6.6,2,1);
	sqlrcur_inputBindDouble(cur,"var6",6.6,2,1);
	sqlrcur_inputBindDouble(cur,"var7",6.6,2,1);
	sqlrcur_inputBindDouble(cur,"var8",6.00,3,2);
	sqlrcur_inputBindDouble(cur,"var9",6.00,3,2);
	sqlrcur_inputBindString(cur,"var10","01-Jan-2006 06:00:00");
	sqlrcur_inputBindString(cur,"var11","01-Jan-2006 06:00:00");
	sqlrcur_inputBindString(cur,"var12","testchar6");
	sqlrcur_inputBindString(cur,"var13","testvarchar6");
	sqlrcur_inputBindLong(cur,"var14",1);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",7);
	sqlrcur_inputBindLong(cur,"var2",7);
	sqlrcur_inputBindLong(cur,"var3",7);
	sqlrcur_inputBindDouble(cur,"var4",7.7,2,1);
	sqlrcur_inputBindDouble(cur,"var5",7.7,2,1);
	sqlrcur_inputBindDouble(cur,"var6",7.7,2,1);
	sqlrcur_inputBindDouble(cur,"var7",7.7,2,1);
	sqlrcur_inputBindDouble(cur,"var8",7.00,3,2);
	sqlrcur_inputBindDouble(cur,"var9",7.00,3,2);
	sqlrcur_inputBindString(cur,"var10","01-Jan-2007 07:00:00");
	sqlrcur_inputBindString(cur,"var11","01-Jan-2007 07:00:00");
	sqlrcur_inputBindString(cur,"var12","testchar7");
	sqlrcur_inputBindString(cur,"var13","testvarchar7");
	sqlrcur_inputBindLong(cur,"var14",1);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",8);
	sqlrcur_inputBindLong(cur,"var2",8);
	sqlrcur_inputBindLong(cur,"var3",8);
	sqlrcur_inputBindDouble(cur,"var4",8.8,2,1);
	sqlrcur_inputBindDouble(cur,"var5",8.8,2,1);
	sqlrcur_inputBindDouble(cur,"var6",8.8,2,1);
	sqlrcur_inputBindDouble(cur,"var7",8.8,2,1);
	sqlrcur_inputBindDouble(cur,"var8",8.00,3,2);
	sqlrcur_inputBindDouble(cur,"var9",8.00,3,2);
	sqlrcur_inputBindString(cur,"var10","01-Jan-2008 08:00:00");
	sqlrcur_inputBindString(cur,"var11","01-Jan-2008 08:00:00");
	sqlrcur_inputBindString(cur,"var12","testchar8");
	sqlrcur_inputBindString(cur,"var13","testvarchar8");
	sqlrcur_inputBindLong(cur,"var14",1);
	sqlrcur_inputBindString(cur,"var15","junkvalue");
	sqlrcur_validateBinds(cur);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("SELECT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testint"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccessInt(sqlrcur_colCount(cur),14);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testsmallint");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testtinyint");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testreal");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testdecimal");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testnumeric");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testmoney");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"testsmallmoney");
	checkSuccessString(sqlrcur_getColumnName(cur,9),"testdatetime");
	checkSuccessString(sqlrcur_getColumnName(cur,10),"testsmalldatetime");
	checkSuccessString(sqlrcur_getColumnName(cur,11),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,12),"testvarchar");
	checkSuccessString(sqlrcur_getColumnName(cur,13),"testbit");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testint");
	checkSuccessString(cols[1],"testsmallint");
	checkSuccessString(cols[2],"testtinyint");
	checkSuccessString(cols[3],"testreal");
	checkSuccessString(cols[4],"testfloat");
	checkSuccessString(cols[5],"testdecimal");
	checkSuccessString(cols[6],"testnumeric");
	checkSuccessString(cols[7],"testmoney");
	checkSuccessString(cols[8],"testsmallmoney");
	checkSuccessString(cols[9],"testdatetime");
	checkSuccessString(cols[10],"testsmalldatetime");
	checkSuccessString(cols[11],"testchar");
	checkSuccessString(cols[12],"testvarchar");
	checkSuccessString(cols[13],"testbit");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"INT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testint"),"INT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,1),"SMALLINT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testsmallint"),"SMALLINT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,2),"TINYINT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtinyint"),"TINYINT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,3),"REAL");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testreal"),"REAL");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,4),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testfloat"),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,5),"DECIMAL");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdecimal"),"DECIMAL");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,6),"NUMERIC");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testnumeric"),"NUMERIC");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,7),"MONEY");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testmoney"),"MONEY");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,8),"SMALLMONEY");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testsmallmoney"),"SMALLMONEY");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,9),"DATETIME");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdatetime"),"DATETIME");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,10),"SMALLDATETIME");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testsmalldatetime"),"SMALLDATETIME");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,11),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testchar"),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,12),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testvarchar"),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,13),"BIT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testbit"),"BIT");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testint"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,1),2);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testsmallint"),2);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,2),1);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtinyint"),1);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,3),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testreal"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,4),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testfloat"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,5),35);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdecimal"),35);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,6),35);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testnumeric"),35);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,7),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testmoney"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,8),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testsmallmoney"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,9),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdatetime"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,10),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testsmalldatetime"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,11),40);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testchar"),40);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,12),40);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testvarchar"),40);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,13),1);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testbit"),1);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,0),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testint"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,1),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,2),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testtinyint"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,3),18);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testreal"),18);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,4),18);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testfloat"),18);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,5),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdecimal"),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,6),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testnumeric"),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,7),4);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testmoney"),4);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,8),4);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testsmallmoney"),4);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,9),19);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdatetime"),19);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,10),19);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testsmalldatetime"),19);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,11),40);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testchar"),40);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,12),12);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testvarchar"),12);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,13),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testbit"),1);
	printf("\n");

	printf("ROW COUNT: \n");
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	printf("\n");

	printf("TOTAL ROWS: \n");
	checkSuccessInt(sqlrcur_totalRows(cur),0);
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"1");
	//checkSuccessString(sqlrcur_getFieldByIndex(cur,0,3),"1.1");
	//checkSuccessString(sqlrcur_getFieldByIndex(cur,0,4),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,5),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,6),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,7),"1.00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,8),"1.00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,9),"Jan  1 2001  1:00AM");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,10),"Jan  1 2001  1:00AM");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,11),"testchar1                               ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,12),"testvarchar1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,13),"1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,1),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),"8");
	//checkSuccessString(sqlrcur_getFieldByIndex(cur,7,3),"8.8");
	//checkSuccessString(sqlrcur_getFieldByIndex(cur,7,4),"8.8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,5),"8.8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,6),"8.8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,7),"8.00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,8),"8.00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,9),"Jan  1 2008  8:00AM");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,10),"Jan  1 2008  8:00AM");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,11),"testchar8                               ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,12),"testvarchar8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,13),"1");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,1),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,2),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,3),18);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,4),18);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,5),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,6),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,7),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,8),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,9),19);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,10),19);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,11),40);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,12),12);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,13),1);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,1),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,2),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,3),18);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,4),18);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,5),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,6),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,7),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,8),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,9),19);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,10),19);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,11),40);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,12),12);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,13),1);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testsmallint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testtinyint"),"1");
	//checkSuccessString(sqlrcur_getFieldByName(cur,0,"testreal"),"1.1");
	//checkSuccessString(sqlrcur_getFieldByName(cur,0,"testfloat"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdecimal"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testnumeric"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testmoney"),"1.00");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testsmallmoney"),"1.00");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdatetime"),"Jan  1 2001  1:00AM");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testsmalldatetime"),"Jan  1 2001  1:00AM");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testchar"),"testchar1                               ");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testvarchar"),"testvarchar1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testbit"),"1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testsmallint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testtinyint"),"8");
	//checkSuccessString(sqlrcur_getFieldByName(cur,7,"testreal"),"8.8");
	//checkSuccessString(sqlrcur_getFieldByName(cur,7,"testfloat"),"8.8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdecimal"),"8.8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testnumeric"),"8.8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testmoney"),"8.00");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testsmallmoney"),"8.00");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdatetime"),"Jan  1 2008  8:00AM");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testsmalldatetime"),"Jan  1 2008  8:00AM");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testchar"),"testchar8                               ");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testvarchar"),"testvarchar8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testbit"),"1");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testtinyint"),1);
	//checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testreal"),3);
	//checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdecimal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testnumeric"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testmoney"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testsmallmoney"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdatetime"),19);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testsmalldatetime"),19);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testchar"),40);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testvarchar"),12);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testbit"),1);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testtinyint"),1);
	//checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testreal"),3);
	//checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdecimal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testnumeric"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testmoney"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testsmallmoney"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdatetime"),19);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testsmalldatetime"),19);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testchar"),40);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testvarchar"),12);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testbit"),1);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=sqlrcur_getRow(cur,0);
	checkSuccessString(fields[0],"1");
	checkSuccessString(fields[1],"1");
	checkSuccessString(fields[2],"1");
	//checkSuccessString(fields[3],"1.1");
	//checkSuccessString(fields[4],"1.1");
	checkSuccessString(fields[5],"1.1");
	checkSuccessString(fields[6],"1.1");
	checkSuccessString(fields[7],"1.00");
	checkSuccessString(fields[8],"1.00");
	checkSuccessString(fields[9],"Jan  1 2001  1:00AM");
	checkSuccessString(fields[10],"Jan  1 2001  1:00AM");
	checkSuccessString(fields[11],"testchar1                               ");
	checkSuccessString(fields[12],"testvarchar1");
	checkSuccessString(fields[13],"1");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=sqlrcur_getRowLengths(cur,0);
	checkSuccessInt(fieldlens[0],1);
	checkSuccessInt(fieldlens[1],1);
	checkSuccessInt(fieldlens[2],1);
	//checkSuccessInt(fieldlens[3],3);
	//checkSuccessInt(fieldlens[4],3);
	checkSuccessInt(fieldlens[5],3);
	checkSuccessInt(fieldlens[6],3);
	checkSuccessInt(fieldlens[7],4);
	checkSuccessInt(fieldlens[8],4);
	checkSuccessInt(fieldlens[9],19);
	checkSuccessInt(fieldlens[10],19);
	checkSuccessInt(fieldlens[11],40);
	checkSuccessInt(fieldlens[12],12);
	checkSuccessInt(fieldlens[13],1);
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
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"INT");
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
	checkSuccessInt(sqlrcur_colCount(cur),14);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testsmallint");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testtinyint");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testreal");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testdecimal");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testnumeric");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testmoney");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"testsmallmoney");
	checkSuccessString(sqlrcur_getColumnName(cur,9),"testdatetime");
	checkSuccessString(sqlrcur_getColumnName(cur,10),"testsmalldatetime");
	checkSuccessString(sqlrcur_getColumnName(cur,11),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,12),"testvarchar");
	checkSuccessString(sqlrcur_getColumnName(cur,13),"testbit");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testint");
	checkSuccessString(cols[1],"testsmallint");
	checkSuccessString(cols[2],"testtinyint");
	checkSuccessString(cols[3],"testreal");
	checkSuccessString(cols[4],"testfloat");
	checkSuccessString(cols[5],"testdecimal");
	checkSuccessString(cols[6],"testnumeric");
	checkSuccessString(cols[7],"testmoney");
	checkSuccessString(cols[8],"testsmallmoney");
	checkSuccessString(cols[9],"testdatetime");
	checkSuccessString(cols[10],"testsmalldatetime");
	checkSuccessString(cols[11],"testchar");
	checkSuccessString(cols[12],"testvarchar");
	checkSuccessString(cols[13],"testbit");
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

	return 0;
}
