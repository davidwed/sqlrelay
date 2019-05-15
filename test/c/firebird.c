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

	const char	*bindvars[12]={"1","2","3","4","5","6",
				"7","8","9","10","11",NULL};
	const char	*bindvals[11]={"4","4","4.4","4.4","4.4","4.4",
				"01-JAN-2004","04:00:00",
				"testchar4","testvarchar4",NULL};
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
	int16_t		id;
	char		*filename;
	uint32_t	*fieldlens;

	// instantiation
	con=sqlrcon_alloc("sqlrelay",9000,
				"/tmp/test.socket","test","test",0,1);
	cur=sqlrcur_alloc(con);

	// get database type
	printf("IDENTIFY: \n");
	checkSuccessString(sqlrcon_identify(con),"firebird");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccessInt(sqlrcon_ping(con),1);
	printf("\n");

	// clean up table
	sqlrcur_sendQuery(cur,"delete from testtable");
	sqlrcon_commit(con);

	printf("INSERT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,1,1.1,1.1,1.1,1.1,'01-JAN-2001','01:00:00','testchar1','testvarchar1',NULL,NULL)"),1);
	printf("\n");


	printf("BIND BY POSITION: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable values (?,?,?,?,?,?,?,?,?,?,?,NULL)");
	checkSuccessInt(sqlrcur_countBindVariables(cur),11);
	sqlrcur_inputBindLong(cur,"1",2);
	sqlrcur_inputBindLong(cur,"2",2);
	sqlrcur_inputBindDouble(cur,"3",2.2,2,1);
	sqlrcur_inputBindDouble(cur,"4",2.2,2,1);
	sqlrcur_inputBindDouble(cur,"5",2.2,2,1);
	sqlrcur_inputBindDouble(cur,"6",2.2,2,1);
	sqlrcur_inputBindString(cur,"7","01-JAN-2002");
	sqlrcur_inputBindString(cur,"8","02:00:00");
	sqlrcur_inputBindString(cur,"9","testchar2");
	sqlrcur_inputBindString(cur,"10","testvarchar2");
	sqlrcur_inputBindString(cur,"11",(char *)NULL);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"1",3);
	sqlrcur_inputBindLong(cur,"2",3);
	sqlrcur_inputBindDouble(cur,"3",3.3,2,1);
	sqlrcur_inputBindDouble(cur,"4",3.3,2,1);
	sqlrcur_inputBindDouble(cur,"5",3.3,2,1);
	sqlrcur_inputBindDouble(cur,"6",3.3,2,1);
	sqlrcur_inputBindString(cur,"7","01-JAN-2003");
	sqlrcur_inputBindString(cur,"8","03:00:00");
	sqlrcur_inputBindString(cur,"9","testchar3");
	sqlrcur_inputBindString(cur,"10","testvarchar3");
	sqlrcur_inputBindString(cur,"11",(char *)NULL);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("ARRAY OF BINDS BY POSITION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindStrings(cur,bindvars,bindvals);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (5,5,5.5,5.5,5.5,5.5,'01-JAN-2005','05:00:00','testchar5','testvarchar5',NULL,NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (6,6,6.6,6.6,6.6,6.6,'01-JAN-2006','06:00:00','testchar6','testvarchar6',NULL,NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (7,7,7.7,7.7,7.7,7.7,'01-JAN-2007','07:00:00','testchar7','testvarchar7',NULL,NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (8,8,8.8,8.8,8.8,8.8,'01-JAN-2008','08:00:00','testchar8','testvarchar8',NULL,NULL)"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccessInt(sqlrcur_affectedRows(cur),0);
	printf("\n");

	printf("STORED PROCEDURE: \n");
	sqlrcur_prepareQuery(cur,"select * from testproc(?,?,?,NULL)");
	sqlrcur_inputBindLong(cur,"1",1);
	sqlrcur_inputBindDouble(cur,"2",1.1,2,1);
	sqlrcur_inputBindString(cur,"3","hello");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,(uint32_t)0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"1.1000");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"hello");
	sqlrcur_prepareQuery(cur,"execute procedure testproc ?, ?, ?, NULL");
	sqlrcur_inputBindLong(cur,"1",1);
	sqlrcur_inputBindDouble(cur,"2",1.1,2,1);
	sqlrcur_inputBindString(cur,"3","hello");
	sqlrcur_defineOutputBindInteger(cur,"1");
	sqlrcur_defineOutputBindDouble(cur,"2");
	sqlrcur_defineOutputBindString(cur,"3",20);
	sqlrcur_defineOutputBindBlob(cur,"4");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	checkSuccessInt(sqlrcur_getOutputBindInteger(cur,"1"),1);
	//checkSuccess(sqlrcur_getOutputBindDouble(cur,"2"),1.1);
	checkSuccessString(sqlrcur_getOutputBindString(cur,"3"),"hello               ");
	printf("\n");

	printf("SELECT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccessInt(sqlrcur_colCount(cur),12);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"TESTINTEGER");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"TESTSMALLINT");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"TESTDECIMAL");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"TESTNUMERIC");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"TESTFLOAT");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"TESTDOUBLE");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"TESTDATE");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"TESTTIME");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"TESTCHAR");
	checkSuccessString(sqlrcur_getColumnName(cur,9),"TESTVARCHAR");
	checkSuccessString(sqlrcur_getColumnName(cur,10),"TESTTIMESTAMP");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"TESTINTEGER");
	checkSuccessString(cols[1],"TESTSMALLINT");
	checkSuccessString(cols[2],"TESTDECIMAL");
	checkSuccessString(cols[3],"TESTNUMERIC");
	checkSuccessString(cols[4],"TESTFLOAT");
	checkSuccessString(cols[5],"TESTDOUBLE");
	checkSuccessString(cols[6],"TESTDATE");
	checkSuccessString(cols[7],"TESTTIME");
	checkSuccessString(cols[8],"TESTCHAR");
	checkSuccessString(cols[9],"TESTVARCHAR");
	checkSuccessString(cols[10],"TESTTIMESTAMP");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"INTEGER");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTINTEGER"),"INTEGER");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,1),"SMALLINT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTSMALLINT"),"SMALLINT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,2),"DECIMAL");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTDECIMAL"),"DECIMAL");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,3),"NUMERIC");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTNUMERIC"),"NUMERIC");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,4),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTFLOAT"),"FLOAT");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,5),"DOUBLE PRECISION");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTDOUBLE"),"DOUBLE PRECISION");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,6),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTDATE"),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,7),"TIME");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTTIME"),"TIME");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,8),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTCHAR"),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,9),"VARCHAR");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTVARCHAR"),"VARCHAR");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,10),"TIMESTAMP");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTTIMESTAMP"),"TIMESTAMP");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTINTEGER"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,1),2);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTSMALLINT"),2);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,2),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTDECIMAL"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,3),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTNUMERIC"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,4),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTFLOAT"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,5),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTDOUBLE"),8);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,6),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTDATE"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,7),4);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTTIME"),4);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,8),50);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTCHAR"),50);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,9),50);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTVARCHAR"),50);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,10),8);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTTIMESTAMP"),8);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,0),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTINTEGER"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,1),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTSMALLINT"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,2),4);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTDECIMAL"),4);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,3),4);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTNUMERIC"),4);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,4),6);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTFLOAT"),6);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,5),6);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTDOUBLE"),6);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,6),10);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTDATE"),10);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,7),8);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTTIME"),8);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,8),50);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTCHAR"),50);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,9),12);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTVARCHAR"),12);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,10),0);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTTIMESTAMP"),0);
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"1.10");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,3),"1.10");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,4),"1.1000");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,5),"1.1000");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,6),"2001:01:01");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,7),"01:00:00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,8),"testchar1                                         ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,9),"testvarchar1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,1),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),"8.80");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,3),"8.80");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,4),"8.8000");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,5),"8.8000");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,6),"2008:01:01");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,7),"08:00:00");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,8),"testchar8                                         ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,9),"testvarchar8");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,1),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,2),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,3),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,4),6);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,5),6);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,6),10);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,7),8);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,8),50);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,9),12);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,1),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,2),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,3),4);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,4),6);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,5),6);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,6),10);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,7),8);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,8),50);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,9),12);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTINTEGER"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTSMALLINT"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTDECIMAL"),"1.10");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTNUMERIC"),"1.10");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTFLOAT"),"1.1000");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTDOUBLE"),"1.1000");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTDATE"),"2001:01:01");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTTIME"),"01:00:00");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTCHAR"),"testchar1                                         ");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTVARCHAR"),"testvarchar1");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTINTEGER"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTSMALLINT"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTDECIMAL"),"8.80");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTNUMERIC"),"8.80");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTFLOAT"),"8.8000");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTDOUBLE"),"8.8000");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTDATE"),"2008:01:01");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTTIME"),"08:00:00");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTCHAR"),"testchar8                                         ");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTVARCHAR"),"testvarchar8");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTINTEGER"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTSMALLINT"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTDECIMAL"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTNUMERIC"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTFLOAT"),6);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTDOUBLE"),6);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTDATE"),10);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTTIME"),8);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTCHAR"),50);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTVARCHAR"),12);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTINTEGER"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTSMALLINT"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTDECIMAL"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTNUMERIC"),4);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTFLOAT"),6);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTDOUBLE"),6);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTDATE"),10);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTTIME"),8);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTCHAR"),50);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTVARCHAR"),12);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=sqlrcur_getRow(cur,0);
	checkSuccessString(fields[0],"1");
	checkSuccessString(fields[1],"1");
	checkSuccessString(fields[2],"1.10");
	checkSuccessString(fields[3],"1.10");
	checkSuccessString(fields[4],"1.1000");
	checkSuccessString(fields[5],"1.1000");
	checkSuccessString(fields[6],"2001:01:01");
	checkSuccessString(fields[7],"01:00:00");
	checkSuccessString(fields[8],"testchar1                                         ");
	checkSuccessString(fields[9],"testvarchar1");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=sqlrcur_getRowLengths(cur,0);
	checkSuccessInt(fieldlens[0],1);
	checkSuccessInt(fieldlens[1],1);
	checkSuccessInt(fieldlens[2],4);
	checkSuccessInt(fieldlens[3],4);
	checkSuccessInt(fieldlens[4],6);
	checkSuccessInt(fieldlens[5],6);
	checkSuccessInt(fieldlens[6],10);
	checkSuccessInt(fieldlens[7],8);
	checkSuccessInt(fieldlens[8],50);
	checkSuccessInt(fieldlens[9],12);
	printf("\n");

	printf("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select $(var1),'$(var2)','$(var3)' from rdb$database");
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
	sqlrcur_prepareQuery(cur,"select '$(var1)','$(var2)','$(var3)' from rdb$database");
	sqlrcur_subStrings(cur,subvars,subvalstrings);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"hi");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"hello");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"bye");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select $(var1),$(var2),$(var3) from rdb$database");
	sqlrcur_subLongs(cur,subvars,subvallongs);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"3");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select $(var1),$(var2),$(var3) from rdb$database");
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select 1,NULL,NULL from rdb$database"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select 1,NULL,NULL from rdb$database"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"");
	sqlrcur_getNullsAsNulls(cur);
	printf("\n");

	printf("RESULT SET BUFFER SIZE: \n");
	checkSuccessInt(sqlrcur_getResultSetBufferSize(cur),0);
	sqlrcur_setResultSetBufferSize(cur,2);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),NULL);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),0);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),NULL);
	sqlrcur_getColumnInfo(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),"TESTINTEGER");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),4);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"INTEGER");
	printf("\n");

	printf("SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
	filename=strdup(sqlrcur_getCacheFileName(cur));
	checkSuccessString(filename,"cachefile1");
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,filename),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	free(filename);
	printf("\n");

	printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccessInt(sqlrcur_colCount(cur),12);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"TESTINTEGER");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"TESTSMALLINT");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"TESTDECIMAL");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"TESTNUMERIC");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"TESTFLOAT");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"TESTDOUBLE");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"TESTDATE");
	checkSuccessString(sqlrcur_getColumnName(cur,7),"TESTTIME");
	checkSuccessString(sqlrcur_getColumnName(cur,8),"TESTCHAR");
	checkSuccessString(sqlrcur_getColumnName(cur,9),"TESTVARCHAR");
	checkSuccessString(sqlrcur_getColumnName(cur,10),"TESTTIMESTAMP");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"TESTINTEGER");
	checkSuccessString(cols[1],"TESTSMALLINT");
	checkSuccessString(cols[2],"TESTDECIMAL");
	checkSuccessString(cols[3],"TESTNUMERIC");
	checkSuccessString(cols[4],"TESTFLOAT");
	checkSuccessString(cols[5],"TESTDOUBLE");
	checkSuccessString(cols[6],"TESTDATE");
	checkSuccessString(cols[7],"TESTTIME");
	checkSuccessString(cols[8],"TESTCHAR");
	checkSuccessString(cols[9],"TESTVARCHAR");
	checkSuccessString(cols[10],"TESTTIMESTAMP");
	printf("\n");

	printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	sqlrcur_cacheToFile(cur,"cachefile1");
	sqlrcur_setCacheTtl(cur,200);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
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
	checkSuccessInt(sqlrcon_autoCommitOn(con),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (10,10,10.1,10.1,10.1,10.1,'01-JAN-2010','10:00:00','testchar10','testvarchar10',NULL,NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"9");
	checkSuccessInt(sqlrcon_autoCommitOff(con),1);
	printf("\n");

	printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testinteger"),1);
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

	// clean up table
	sqlrcon_commit(con);
	sqlrcur_sendQuery(cur,"delete from testtable");
	sqlrcon_commit(con);
	printf("\n");

	// invalid queries...
	printf("INVALID QUERIES: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1 order by testinteger"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1 order by testinteger"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1 order by testinteger"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable1 order by testinteger"),0);
	printf("\n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable1 values (1,2,3,4)"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable1 values (1,2,3,4)"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable1 values (1,2,3,4)"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable1 values (1,2,3,4)"),0);
	printf("\n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable"),0);
	printf("\n");

	return 0;
}
