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
	const char	*dbversion;
	uint32_t	majorversion;


	// instantiation
	con=sqlrcon_alloc("sqlrelay",9000,
			"/tmp/test.socket","testuser","testpassword",0,1);
	cur=sqlrcur_alloc(con);

	// get database type
	printf("IDENTIFY: \n");
	checkSuccessString(sqlrcon_identify(con),"mysql");
	printf("\n");

	// get the db version
	dbversion=sqlrcon_dbVersion(con);
	majorversion=dbversion[0]-'0';

	// ping
	printf("PING: \n");
	checkSuccessInt(sqlrcon_ping(con),1);
	printf("\n");

	// drop existing table
	sqlrcur_sendQuery(cur,"drop table testtable");

	// create a new table
	printf("CREATE TEMPTABLE: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(2,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testtext text, testvarchar varchar(40), testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testtimestamp timestamp)"),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','text1','varchar1','tinytext1','mediumtext1','longtext1',NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','text2','varchar2','tinytext2','mediumtext2','longtext2',NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (3,3,3,3,3,3.1,3.1,3.1,'2003-01-01','03:00:00','2003-01-01 03:00:00','2003','char3','text3','varchar3','tinytext3','mediumtext3','longtext3',NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (4,4,4,4,4,4.1,4.1,4.1,'2004-01-01','04:00:00','2004-01-01 04:00:00','2004','char4','text4','varchar4','tinytext4','mediumtext4','longtext4',NULL)"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccessInt(sqlrcur_affectedRows(cur),1);
	printf("\n");

	printf("BIND BY POSITION: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,NULL)");
	checkSuccessInt(sqlrcur_countBindVariables(cur),18);
	sqlrcur_inputBindLong(cur,"1",5);
	sqlrcur_inputBindLong(cur,"2",5);
	sqlrcur_inputBindLong(cur,"3",5);
	sqlrcur_inputBindLong(cur,"4",5);
	sqlrcur_inputBindLong(cur,"5",5);
	sqlrcur_inputBindDouble(cur,"6",5.1,2,1);
	sqlrcur_inputBindDouble(cur,"7",5.1,2,1);
	sqlrcur_inputBindDouble(cur,"8",5.1,2,1);
	sqlrcur_inputBindString(cur,"9","2005-01-01");
	sqlrcur_inputBindString(cur,"10","05:00:00");
	sqlrcur_inputBindString(cur,"11","2005-01-01 05:00:00");
	sqlrcur_inputBindString(cur,"12","2005");
	sqlrcur_inputBindString(cur,"13","char5");
	sqlrcur_inputBindString(cur,"14","text5");
	sqlrcur_inputBindString(cur,"15","varchar5");
	sqlrcur_inputBindString(cur,"16","tinytext5");
	sqlrcur_inputBindString(cur,"17","mediumtext5");
	sqlrcur_inputBindString(cur,"18","longtext5");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"1",6);
	sqlrcur_inputBindLong(cur,"2",6);
	sqlrcur_inputBindLong(cur,"3",6);
	sqlrcur_inputBindLong(cur,"4",6);
	sqlrcur_inputBindLong(cur,"5",6);
	sqlrcur_inputBindDouble(cur,"6",6.1,2,1);
	sqlrcur_inputBindDouble(cur,"7",6.1,2,1);
	sqlrcur_inputBindDouble(cur,"8",6.1,2,1);
	sqlrcur_inputBindString(cur,"9","2006-01-01");
	sqlrcur_inputBindString(cur,"10","06:00:00");
	sqlrcur_inputBindString(cur,"11","2006-01-01 06:00:00");
	sqlrcur_inputBindString(cur,"12","2006");
	sqlrcur_inputBindString(cur,"13","char6");
	sqlrcur_inputBindString(cur,"14","text6");
	sqlrcur_inputBindString(cur,"15","varchar6");
	sqlrcur_inputBindString(cur,"16","tinytext6");
	sqlrcur_inputBindString(cur,"17","mediumtext6");
	sqlrcur_inputBindString(cur,"18","longtext6");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"1",7);
	sqlrcur_inputBindLong(cur,"2",7);
	sqlrcur_inputBindLong(cur,"3",7);
	sqlrcur_inputBindLong(cur,"4",7);
	sqlrcur_inputBindLong(cur,"5",7);
	sqlrcur_inputBindDouble(cur,"6",7.1,2,1);
	sqlrcur_inputBindDouble(cur,"7",7.1,2,1);
	sqlrcur_inputBindDouble(cur,"8",7.1,2,1);
	sqlrcur_inputBindString(cur,"9","2007-01-01");
	sqlrcur_inputBindString(cur,"10","07:00:00");
	sqlrcur_inputBindString(cur,"11","2007-01-01 07:00:00");
	sqlrcur_inputBindString(cur,"12","2007");
	sqlrcur_inputBindString(cur,"13","char7");
	sqlrcur_inputBindString(cur,"14","text7");
	sqlrcur_inputBindString(cur,"15","varchar7");
	sqlrcur_inputBindString(cur,"16","tinytext7");
	sqlrcur_inputBindString(cur,"17","mediumtext7");
	sqlrcur_inputBindString(cur,"18","longtext7");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("BIND BY POSITION WITH VALIDATION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"1",8);
	sqlrcur_inputBindLong(cur,"2",8);
	sqlrcur_inputBindLong(cur,"3",8);
	sqlrcur_inputBindLong(cur,"4",8);
	sqlrcur_inputBindLong(cur,"5",8);
	sqlrcur_inputBindDouble(cur,"6",8.1,2,1);
	sqlrcur_inputBindDouble(cur,"7",8.1,2,1);
	sqlrcur_inputBindDouble(cur,"8",8.1,2,1);
	sqlrcur_inputBindString(cur,"9","2008-01-01");
	sqlrcur_inputBindString(cur,"10","08:00:00");
	sqlrcur_inputBindString(cur,"11","2008-01-01 08:00:00");
	sqlrcur_inputBindString(cur,"12","2008");
	sqlrcur_inputBindString(cur,"13","char8");
	sqlrcur_inputBindString(cur,"14","text8");
	sqlrcur_inputBindString(cur,"15","varchar8");
	sqlrcur_inputBindString(cur,"16","tinytext8");
	sqlrcur_inputBindString(cur,"17","mediumtext8");
	sqlrcur_inputBindString(cur,"18","longtext8");
	sqlrcur_validateBinds(cur);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("SELECT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccessInt(sqlrcur_colCount(cur),19);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testtinyint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testsmallint");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testmediumint");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testbigint");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testreal");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testdecimal");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"testdate");
	checkSuccessString(sqlrcur_getColumnName(cur,9),"testtime");
	checkSuccessString(sqlrcur_getColumnName(cur,10),"testdatetime");
	checkSuccessString(sqlrcur_getColumnName(cur,11),"testyear");
	checkSuccessString(sqlrcur_getColumnName(cur,12),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,13),"testtext");
	checkSuccessString(sqlrcur_getColumnName(cur,14),"testvarchar");
	checkSuccessString(sqlrcur_getColumnName(cur,15),"testtinytext");
	checkSuccessString(sqlrcur_getColumnName(cur,16),"testmediumtext");
	checkSuccessString(sqlrcur_getColumnName(cur,17),"testlongtext");
	checkSuccessString(sqlrcur_getColumnName(cur,18),"testtimestamp");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testtinyint");
	checkSuccessString(cols[1],"testsmallint");
	checkSuccessString(cols[2],"testmediumint");
	checkSuccessString(cols[3],"testint");
	checkSuccessString(cols[4],"testbigint");
	checkSuccessString(cols[5],"testfloat");
	checkSuccessString(cols[6],"testreal");
	checkSuccessString(cols[7],"testdecimal");
	checkSuccessString(cols[8],"testdate");
	checkSuccessString(cols[9],"testtime");
	checkSuccessString(cols[10],"testdatetime");
	checkSuccessString(cols[11],"testyear");
	checkSuccessString(cols[12],"testchar");
	checkSuccessString(cols[13],"testtext");
	checkSuccessString(cols[14],"testvarchar");
	checkSuccessString(cols[15],"testtinytext");
	checkSuccessString(cols[16],"testmediumtext");
	checkSuccessString(cols[17],"testlongtext");
	checkSuccessString(cols[18],"testtimestamp");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"TINYINT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,1),"SMALLINT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,2),"MEDIUMINT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,3),"INT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,4),"BIGINT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,5),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,6),"REAL");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,7),"DECIMAL");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,8),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,9),"TIME");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,10),"DATETIME");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,11),"YEAR");
	if (majorversion==3) {
		checkSuccessString(
			sqlrcur_getColumnTypeByIndex(cur,12),"VARSTRING");
	} else {
		checkSuccessString(
			sqlrcur_getColumnTypeByIndex(cur,12),"STRING");
	}
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,13),"BLOB");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,14),"VARSTRING");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,15),"TINYBLOB");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,16),"MEDIUMBLOB");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,17),"LONGBLOB");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,18),"TIMESTAMP");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtinyint"),"TINYINT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testsmallint"),"SMALLINT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testmediumint"),"MEDIUMINT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testint"),"INT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testbigint"),"BIGINT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testfloat"),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testreal"),"REAL");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdecimal"),"DECIMAL");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdate"),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtime"),"TIME");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testdatetime"),"DATETIME");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testyear"),"YEAR");
	if (majorversion==3) {
		checkSuccessString(
		sqlrcur_getColumnTypeByName(cur,"testchar"),"VARSTRING");
	} else {
		checkSuccessString(
		sqlrcur_getColumnTypeByName(cur,"testchar"),"STRING");
	}
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtext"),"BLOB");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testvarchar"),"VARSTRING");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtinytext"),"TINYBLOB");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testmediumtext"),"MEDIUMBLOB");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testlongtext"),"LONGBLOB");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"testtimestamp"),"TIMESTAMP");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),1);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,1),2);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,2),3);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,3),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,4),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,5),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,6),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,7),6);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,8),3);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,9),3);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,10),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,11),1);
	//checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,12),40);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,13),65535);
	//checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,14),41);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,15),255);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,16),16777215);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,17),2147483647);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,18),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtinyint"),1);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testsmallint"),2);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testmediumint"),3);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testint"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testbigint"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testfloat"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testreal"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdecimal"),6);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdate"),3);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtime"),3);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testdatetime"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testyear"),1);
	//checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testchar"),40);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtext"),65535);
	//checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testvarchar"),41);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtinytext"),255);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testmediumtext"),16777215);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testlongtext"),2147483647);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"testtimestamp"),4);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,0),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,1),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,2),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,3),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,4),1);
	//checkSuccessInt(sqlrcur_getLongestByIndex(cur,5),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,6),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,7),3);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,8),10);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,9),8);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,10),19);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,11),4);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,12),5);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,13),5);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,14),8);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,15),9);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,16),11);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,17),9);
	if (majorversion==3) {
		checkSuccessInt(sqlrcur_getLongestByIndex(cur,18),14);
	} else {
		checkSuccessInt(sqlrcur_getLongestByIndex(cur,18),19);
	}
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testtinyint"),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testmediumint"),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testint"),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testbigint"),1);
	//checkSuccessInt(sqlrcur_getLongestByName(cur,"testfloat"),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testreal"),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdecimal"),3);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdate"),10);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testtime"),8);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testdatetime"),19);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testyear"),4);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testchar"),5);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testtext"),5);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testvarchar"),8);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testtinytext"),9);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testmediumtext"),11);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"testlongtext"),9);
	if (majorversion==3) {
		checkSuccessInt(
			sqlrcur_getLongestByName(cur,"testtimestamp"),14);
	} else {
		checkSuccessInt(
			sqlrcur_getLongestByName(cur,"testtimestamp"),19);
	}
	printf("\n");

	printf("ROW COUNT: \n");
	checkSuccessInt(sqlrcur_rowCount(cur),8);
	printf("\n");

	printf("TOTAL ROWS: \n");
	// older versions of mysql know this
	//checkSuccessInt(sqlrcur_totalRows(cur),0);
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,3),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,4),"1");
	//checkSuccessString(sqlrcur_getFieldByIndex(cur,0,5),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,6),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,7),"1.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,8),"2001-01-01");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,9),"01:00:00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,10),"2001-01-01 01:00:00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,11),"2001");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,12),"char1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,13),"text1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,14),"varchar1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,15),"tinytext1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,16),"mediumtext1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,17),"longtext1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,1),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,3),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,4),"8");
	//checkSuccessString(sqlrcur_getFieldByIndex(cur,7,5),"8.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,6),"8.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,7),"8.1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,8),"2008-01-01");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,9),"08:00:00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,10),"2008-01-01 08:00:00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,11),"2008");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,12),"char8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,13),"text8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,14),"varchar8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,15),"tinytext8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,16),"mediumtext8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,17),"longtext8");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,1),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,2),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,3),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,4),1);
	//checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,5),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,6),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,7),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,8),10);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,9),8);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,10),19);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,11),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,12),5);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,13),5);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,14),8);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,15),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,16),11);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,17),9);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,1),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,2),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,3),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,4),1);
	//checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,5),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,6),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,7),3);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,8),10);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,9),8);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,10),19);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,11),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,12),5);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,13),5);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,14),8);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,15),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,16),11);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,17),9);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testtinyint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testsmallint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testmediumint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testint"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testbigint"),"1");
	//checkSuccessString(sqlrcur_getFieldByName(cur,0,"testfloat"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testreal"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdecimal"),"1.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdate"),"2001-01-01");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testtime"),"01:00:00");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testdatetime"),"2001-01-01 01:00:00");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testyear"),"2001");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testchar"),"char1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testtext"),"text1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testvarchar"),"varchar1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testtinytext"),"tinytext1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testmediumtext"),"mediumtext1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"testlongtext"),"longtext1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testtinyint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testsmallint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testmediumint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testint"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testbigint"),"8");
	//checkSuccessString(sqlrcur_getFieldByName(cur,7,"testfloat"),"8.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testreal"),"8.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdecimal"),"8.1");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdate"),"2008-01-01");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testtime"),"08:00:00");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testdatetime"),"2008-01-01 08:00:00");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testyear"),"2008");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testchar"),"char8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testtext"),"text8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testvarchar"),"varchar8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testtinytext"),"tinytext8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testmediumtext"),"mediumtext8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"testlongtext"),"longtext8");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testtinyint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testmediumint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testbigint"),1);
	//checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testreal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdecimal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdate"),10);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testtime"),8);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testdatetime"),19);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testyear"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testchar"),5);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testtext"),5);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testvarchar"),8);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testtinytext"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testmediumtext"),11);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"testlongtext"),9);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testtinyint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testsmallint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testmediumint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testint"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testbigint"),1);
	//checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testfloat"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testreal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdecimal"),3);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdate"),10);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testtime"),8);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testdatetime"),19);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testyear"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testchar"),5);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testtext"),5);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testvarchar"),8);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testtinytext"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testmediumtext"),11);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"testlongtext"),9);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=sqlrcur_getRow(cur,0);
	checkSuccessString(fields[0],"1");
	checkSuccessString(fields[1],"1");
	checkSuccessString(fields[2],"1");
	checkSuccessString(fields[3],"1");
	checkSuccessString(fields[4],"1");
	//checkSuccessString(fields[5],"1.1");
	checkSuccessString(fields[6],"1.1");
	checkSuccessString(fields[7],"1.1");
	checkSuccessString(fields[8],"2001-01-01");
	checkSuccessString(fields[9],"01:00:00");
	checkSuccessString(fields[10],"2001-01-01 01:00:00");
	checkSuccessString(fields[11],"2001");
	checkSuccessString(fields[12],"char1");
	checkSuccessString(fields[13],"text1");
	checkSuccessString(fields[14],"varchar1");
	checkSuccessString(fields[15],"tinytext1");
	checkSuccessString(fields[16],"mediumtext1");
	checkSuccessString(fields[17],"longtext1");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=sqlrcur_getRowLengths(cur,0);
	checkSuccessInt(fieldlens[0],1);
	checkSuccessInt(fieldlens[1],1);
	checkSuccessInt(fieldlens[2],1);
	checkSuccessInt(fieldlens[3],1);
	checkSuccessInt(fieldlens[4],1);
	//checkSuccessInt(fieldlens[5],3);
	checkSuccessInt(fieldlens[6],3);
	checkSuccessInt(fieldlens[7],3);
	checkSuccessInt(fieldlens[8],10);
	checkSuccessInt(fieldlens[9],8);
	checkSuccessInt(fieldlens[10],19);
	checkSuccessInt(fieldlens[11],4);
	checkSuccessInt(fieldlens[12],5);
	checkSuccessInt(fieldlens[13],5);
	checkSuccessInt(fieldlens[14],8);
	checkSuccessInt(fieldlens[15],9);
	checkSuccessInt(fieldlens[16],11);
	checkSuccessInt(fieldlens[17],9);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),NULL);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),0);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),NULL);
	printf("\n");
	sqlrcur_getColumnInfo(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testtinyint");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),1);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"TINYINT");
	printf("\n");

	printf("SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
	filename=strdup(sqlrcur_getCacheFileName(cur));
	checkSuccessString(filename,"cachefile1");
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,filename),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	free(filename);
	printf("\n");

	printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccessInt(sqlrcur_colCount(cur),19);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"testtinyint");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"testsmallint");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"testmediumint");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"testint");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"testbigint");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"testfloat");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"testreal");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"testdecimal");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"testdate");
	checkSuccessString(sqlrcur_getColumnName(cur,9),"testtime");
	checkSuccessString(sqlrcur_getColumnName(cur,10),"testdatetime");
	checkSuccessString(sqlrcur_getColumnName(cur,11),"testyear");
	checkSuccessString(sqlrcur_getColumnName(cur,12),"testchar");
	checkSuccessString(sqlrcur_getColumnName(cur,13),"testtext");
	checkSuccessString(sqlrcur_getColumnName(cur,14),"testvarchar");
	checkSuccessString(sqlrcur_getColumnName(cur,15),"testtinytext");
	checkSuccessString(sqlrcur_getColumnName(cur,16),"testmediumtext");
	checkSuccessString(sqlrcur_getColumnName(cur,17),"testlongtext");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"testtinyint");
	checkSuccessString(cols[1],"testsmallint");
	checkSuccessString(cols[2],"testmediumint");
	checkSuccessString(cols[3],"testint");
	checkSuccessString(cols[4],"testbigint");
	checkSuccessString(cols[5],"testfloat");
	checkSuccessString(cols[6],"testreal");
	checkSuccessString(cols[7],"testdecimal");
	checkSuccessString(cols[8],"testdate");
	checkSuccessString(cols[9],"testtime");
	checkSuccessString(cols[10],"testdatetime");
	checkSuccessString(cols[11],"testyear");
	checkSuccessString(cols[12],"testchar");
	checkSuccessString(cols[13],"testtext");
	checkSuccessString(cols[14],"testvarchar");
	checkSuccessString(cols[15],"testtinytext");
	checkSuccessString(cols[16],"testmediumtext");
	checkSuccessString(cols[17],"testlongtext");
	printf("\n");

	printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	sqlrcur_cacheToFile(cur,"cachefile1");
	sqlrcur_setCacheTtl(cur,200);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),1);
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
	// Note: Mysql's default isolation level is repeatable-read,
	// not read-committed like most other db's.  Both sessions must
	// commit to see the changes that each other has made.
	secondcon=sqlrcon_alloc("sqlrelay",9000,
			"/tmp/test.socket","testuser","testpassword",0,1);
	secondcur=sqlrcur_alloc(secondcon);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	if (majorversion>3) {
		checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"0");
	} else {
		checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"8");
	}
	checkSuccessInt(sqlrcon_commit(con),1);
	checkSuccessInt(sqlrcon_commit(secondcon),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"8");
	checkSuccessInt(sqlrcon_autoCommitOn(con),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (10,10,10,10,10,10.1,10.1,1.1,'2010-01-01','10:00:00','2010-01-01 10:00:00','2010','char10','text10','varchar10','tinytext10','mediumtext10','longtext10',NULL)"),1);
	checkSuccessInt(sqlrcon_commit(secondcon),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"9");
	checkSuccessInt(sqlrcon_autoCommitOff(con),1);
	sqlrcon_commit(secondcon);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testtinyint"),0);
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
