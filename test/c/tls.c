// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclientwrapper.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

sqlrcon	con;
sqlrcur	cur;
sqlrcur	bindcur;
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

void checkSuccessStringWithLength(const char *value, const char *success,
								int length) {

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

	if (!strncmp(value,success,length)) {
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

void checkSuccessDouble(double value, double success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("\"%f\"!=\"%f\"",value,success);
		printf("failure ");
		sqlrcur_free(cur);
		sqlrcon_free(con);
		exit(1);
	}
}

int	main(int argc, char **argv) {

	const char	*bindvars[6]={"1","2","3","4","5",NULL};
	const char	*bindvals[5]={"4","testchar4","testvarchar4",
						"01-JAN-2004","testlong4"};
	const char	*subvars[4]={"var1","var2","var3",NULL};
	const char	*subvalstrings[3]={"hi","hello","bye"};
	int64_t		subvallongs[3]={1,2,3};
	double		subvaldoubles[3]={10.55,10.556,10.5556};
	uint32_t	precs[3]={4,5,6};
	uint32_t	scales[3]={2,3,4};
	int64_t		numvar;
	const char	*clobvar;
	char	clobval[8*1024+1];
	long	clobvarlength;
	const char	*blobvar;
	long	blobvarlength;
	const char	*stringvar;
	double		floatvar;
	const char * const *cols;
	const char * const *fields;
	uint16_t	port;
	const char	*socket;
	uint16_t	id;
	char	*filename;
	const char	*arraybindvars[6]={"var1","var2","var3",
						"var4","var5",NULL};
	const char	*arraybindvals[5]={"7","testchar7","testvarchar7",
						"01-JAN-2007","testlong7"};
	uint32_t	*fieldlens;
	char	testval[4001];
	int	i;
	char	query[4000+25];
	const char	*clobbindvar;

	const char	*cert="/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem";
	const char	*ca="/usr/local/firstworks/etc/sqlrelay.conf.d/ca.pem";
	#ifdef _WIN32
		cert="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\client.pfx";
		ca="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\ca.pfx";
	#endif


	// instantiation
	con=sqlrcon_alloc("sqlrelay",9000,"/tmp/test.socket",NULL,NULL,0,1);
	cur=sqlrcur_alloc(con);
	sqlrcon_enableTls(con,NULL,cert,NULL,NULL,"ca",ca,0);

	// get database type
	printf("IDENTIFY: \n");
	checkSuccessString(sqlrcon_identify(con),"oracle");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccessInt(sqlrcon_ping(con),1);
	printf("\n");

	printf("BIND VALIDATION: \n");
	sqlrcur_sendQuery(cur,"drop table testtable1");
	sqlrcur_sendQuery(cur,"create table testtable1 (col1 varchar2(20), col2 varchar2(20), col3 varchar2(20))");
	sqlrcur_prepareQuery(cur,"insert into testtable1 values ($(var1),$(var2),$(var3))");
	sqlrcur_inputBindLong(cur,"var1",1);
	sqlrcur_inputBindLong(cur,"var2",2);
	sqlrcur_inputBindLong(cur,"var3",3);
	sqlrcur_subString(cur,"var1",":var1");
	checkSuccessInt(sqlrcur_validBind(cur,"var1"),1);
	checkSuccessInt(sqlrcur_validBind(cur,"var2"),0);
	checkSuccessInt(sqlrcur_validBind(cur,"var3"),0);
	checkSuccessInt(sqlrcur_validBind(cur,"var4"),0);
	printf("\n");
	sqlrcur_subString(cur,"var2",":var2");
	checkSuccessInt(sqlrcur_validBind(cur,"var1"),1);
	checkSuccessInt(sqlrcur_validBind(cur,"var2"),1);
	checkSuccessInt(sqlrcur_validBind(cur,"var3"),0);
	checkSuccessInt(sqlrcur_validBind(cur,"var4"),0);
	printf("\n");
	sqlrcur_subString(cur,"var3",":var3");
	checkSuccessInt(sqlrcur_validBind(cur,"var1"),1);
	checkSuccessInt(sqlrcur_validBind(cur,"var2"),1);
	checkSuccessInt(sqlrcur_validBind(cur,"var3"),1);
	checkSuccessInt(sqlrcur_validBind(cur,"var4"),0);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_sendQuery(cur,"drop table testtable1");
	printf("\n");

	// drop existing table
	sqlrcur_sendQuery(cur,"drop table testtable");

	printf("CREATE TEMPTABLE: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long, testclob clob, testblob blob)"),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1','testclob1',empty_blob())"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccessInt(sqlrcur_affectedRows(cur),1);
	printf("\n");

	printf("BIND BY POSITION: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
	checkSuccessInt(sqlrcur_countBindVariables(cur),7);
	sqlrcur_inputBindLong(cur,"1",2);
	sqlrcur_inputBindString(cur,"2","testchar2");
	sqlrcur_inputBindString(cur,"3","testvarchar2");
	sqlrcur_inputBindDate(cur,"4",2002,1,1,-1,-1,-1,0,NULL,0);
	sqlrcur_inputBindString(cur,"5","testlong2");
	sqlrcur_inputBindClob(cur,"6","testclob2",9);
	sqlrcur_inputBindBlob(cur,"7","testblob2",9);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"1",3);
	sqlrcur_inputBindString(cur,"2","testchar3");
	sqlrcur_inputBindString(cur,"3","testvarchar3");
	sqlrcur_inputBindDate(cur,"4",2003,1,1,-1,-1,-1,0,NULL,0);
	sqlrcur_inputBindString(cur,"5","testlong3");
	sqlrcur_inputBindClob(cur,"6","testclob3",9);
	sqlrcur_inputBindBlob(cur,"7","testblob3",9);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("ARRAY OF BINDS BY POSITION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindStrings(cur,bindvars,bindvals);
	sqlrcur_inputBindClob(cur,"6","testclob4",9);
	sqlrcur_inputBindBlob(cur,"7","testblob4",9);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("BIND BY NAME: \n");
	sqlrcur_prepareQuery(cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
	sqlrcur_inputBindLong(cur,"var1",5);
	sqlrcur_inputBindString(cur,"var2","testchar5");
	sqlrcur_inputBindString(cur,"var3","testvarchar5");
	sqlrcur_inputBindDate(cur,"var4",2005,1,1,-1,-1,-1,0,NULL,0);
	sqlrcur_inputBindString(cur,"var5","testlong5");
	sqlrcur_inputBindClob(cur,"var6","testclob5",9);
	sqlrcur_inputBindBlob(cur,"var7","testblob5",9);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",6);
	sqlrcur_inputBindString(cur,"var2","testchar6");
	sqlrcur_inputBindString(cur,"var3","testvarchar6");
	sqlrcur_inputBindDate(cur,"var4",2006,1,1,-1,-1,-1,0,NULL,0);
	sqlrcur_inputBindString(cur,"var5","testlong6");
	sqlrcur_inputBindClob(cur,"var6","testclob6",9);
	sqlrcur_inputBindBlob(cur,"var7","testblob6",9);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("ARRAY OF BINDS BY NAME: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindStrings(cur,arraybindvars,arraybindvals);
	sqlrcur_inputBindClob(cur,"var6","testclob7",9);
	sqlrcur_inputBindBlob(cur,"var7","testblob7",9);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_inputBindLong(cur,"var1",8);
	sqlrcur_inputBindString(cur,"var2","testchar8");
	sqlrcur_inputBindString(cur,"var3","testvarchar8");
	sqlrcur_inputBindDate(cur,"var4",2008,1,1,-1,-1,-1,0,NULL,0);
	sqlrcur_inputBindString(cur,"var5","testlong8");
	sqlrcur_inputBindClob(cur,"var6","testclob8",9);
	sqlrcur_inputBindBlob(cur,"var7","testblob8",9);
	sqlrcur_inputBindString(cur,"var9","junkvalue");
	sqlrcur_validateBinds(cur);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("OUTPUT BIND BY NAME: \n");
	sqlrcur_prepareQuery(cur,"begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;");
	sqlrcur_defineOutputBindInteger(cur,"numvar");
	sqlrcur_defineOutputBindString(cur,"stringvar",10);
	sqlrcur_defineOutputBindDouble(cur,"floatvar");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	numvar=sqlrcur_getOutputBindInteger(cur,"numvar");
	stringvar=sqlrcur_getOutputBindString(cur,"stringvar");
	floatvar=sqlrcur_getOutputBindDouble(cur,"floatvar");
	checkSuccessInt(numvar,1);
	checkSuccessString(stringvar,"hello");
	checkSuccessDouble(floatvar,2.5);
	printf("\n");

	printf("OUTPUT BIND BY POSITION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_defineOutputBindInteger(cur,"1");
	sqlrcur_defineOutputBindString(cur,"2",10);
	sqlrcur_defineOutputBindDouble(cur,"3");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	numvar=sqlrcur_getOutputBindInteger(cur,"1");
	stringvar=sqlrcur_getOutputBindString(cur,"2");
	floatvar=sqlrcur_getOutputBindDouble(cur,"3");
	checkSuccessInt(numvar,1);
	checkSuccessString(stringvar,"hello");
	checkSuccessDouble(floatvar,2.5);
	printf("\n");

	printf("OUTPUT BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds(cur);
	sqlrcur_defineOutputBindInteger(cur,"numvar");
	sqlrcur_defineOutputBindString(cur,"stringvar",10);
	sqlrcur_defineOutputBindDouble(cur,"floatvar");
	sqlrcur_defineOutputBindString(cur,"dummyvar",10);
	sqlrcur_validateBinds(cur);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	numvar=sqlrcur_getOutputBindInteger(cur,"numvar");
	stringvar=sqlrcur_getOutputBindString(cur,"stringvar");
	floatvar=sqlrcur_getOutputBindDouble(cur,"floatvar");
	checkSuccessInt(numvar,1);
	checkSuccessString(stringvar,"hello");
	checkSuccessDouble(floatvar,2.5);
	printf("\n");

	printf("SELECT: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccessInt(sqlrcur_colCount(cur),7);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"TESTNUMBER");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"TESTCHAR");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"TESTVARCHAR");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"TESTDATE");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"TESTLONG");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"TESTCLOB");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"TESTBLOB");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"TESTNUMBER");
	checkSuccessString(cols[1],"TESTCHAR");
	checkSuccessString(cols[2],"TESTVARCHAR");
	checkSuccessString(cols[3],"TESTDATE");
	checkSuccessString(cols[4],"TESTLONG");
	checkSuccessString(cols[5],"TESTCLOB");
	checkSuccessString(cols[6],"TESTBLOB");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"NUMBER");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTNUMBER"),"NUMBER");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,1),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTCHAR"),"CHAR");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,2),"VARCHAR2");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTVARCHAR"),"VARCHAR2");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,3),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTDATE"),"DATE");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,4),"LONG");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTLONG"),"LONG");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,5),"CLOB");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTCLOB"),"CLOB");
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,6),"BLOB");
	checkSuccessString(sqlrcur_getColumnTypeByName(cur,"TESTBLOB"),"BLOB");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),22);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTNUMBER"),22);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,1),40);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTCHAR"),40);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,2),40);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTVARCHAR"),40);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,3),7);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTDATE"),7);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,4),0);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTLONG"),0);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,5),0);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTCLOB"),0);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,6),0);
	checkSuccessInt(sqlrcur_getColumnLengthByName(cur,"TESTBLOB"),0);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,0),1);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTNUMBER"),1);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,1),40);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTCHAR"),40);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,2),12);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTVARCHAR"),12);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,3),9);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTDATE"),9);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,4),9);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTLONG"),9);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,5),9);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTCLOB"),9);
	checkSuccessInt(sqlrcur_getLongestByIndex(cur,6),9);
	checkSuccessInt(sqlrcur_getLongestByName(cur,"TESTBLOB"),9);
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
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"testchar1                               ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"testvarchar1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,3),"01-JAN-01");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,4),"testlong1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,5),"testclob1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,6),"");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,1),"testchar8                               ");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,2),"testvarchar8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,3),"01-JAN-08");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,4),"testlong8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,5),"testclob8");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,6),"testblob8");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,1),40);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,2),12);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,3),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,4),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,5),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,0,6),0);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,0),1);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,1),40);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,2),12);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,3),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,4),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,5),9);
	checkSuccessInt(sqlrcur_getFieldLengthByIndex(cur,7,6),9);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTNUMBER"),"1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTCHAR"),"testchar1                               ");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTVARCHAR"),"testvarchar1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTDATE"),"01-JAN-01");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTLONG"),"testlong1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTCLOB"),"testclob1");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTBLOB"),"");
	printf("\n");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTNUMBER"),"8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTCHAR"),"testchar8                               ");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTVARCHAR"),"testvarchar8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTDATE"),"01-JAN-08");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTLONG"),"testlong8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTCLOB"),"testclob8");
	checkSuccessString(sqlrcur_getFieldByName(cur,7,"TESTBLOB"),"testblob8");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTNUMBER"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTCHAR"),40);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTVARCHAR"),12);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTDATE"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTLONG"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTCLOB"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,0,"TESTBLOB"),0);
	printf("\n");
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTNUMBER"),1);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTCHAR"),40);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTVARCHAR"),12);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTDATE"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTLONG"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTCLOB"),9);
	checkSuccessInt(sqlrcur_getFieldLengthByName(cur,7,"TESTBLOB"),9);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=sqlrcur_getRow(cur,0);
	checkSuccessString(fields[0],"1");
	checkSuccessString(fields[1],"testchar1                               ");
	checkSuccessString(fields[2],"testvarchar1");
	checkSuccessString(fields[3],"01-JAN-01");
	checkSuccessString(fields[4],"testlong1");
	checkSuccessString(fields[5],"testclob1");
	checkSuccessString(fields[6],"");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=sqlrcur_getRowLengths(cur,0);
	checkSuccessInt(fieldlens[0],1);
	checkSuccessInt(fieldlens[1],40);
	checkSuccessInt(fieldlens[2],12);
	checkSuccessInt(fieldlens[3],9);
	checkSuccessInt(fieldlens[4],9);
	checkSuccessInt(fieldlens[5],9);
	checkSuccessInt(fieldlens[6],0);
	printf("\n");

	printf("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select $(var1),'$(var2)',$(var3) from dual");
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

	printf("OUTPUT BIND: \n");
	sqlrcur_prepareQuery(cur,"begin :var1:='hello'; end;");
	sqlrcur_defineOutputBindString(cur,"var1",10);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	checkSuccessString(sqlrcur_getOutputBindString(cur,"var1"),"hello");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select $(var1),$(var2),$(var3) from dual");
	sqlrcur_subLongs(cur,subvars,subvallongs);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");
	
	printf("FIELDS: \n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"3");
	printf("\n");
	
	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select '$(var1)','$(var2)','$(var3)' from dual");
	sqlrcur_subStrings(cur,subvars,subvalstrings);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"hi");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"hello");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"bye");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery(cur,"select $(var1),$(var2),$(var3) from dual");
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select NULL,1,NULL from dual"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select NULL,1,NULL from dual"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),"");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),"");
	sqlrcur_getNullsAsNulls(cur);
	printf("\n");

	printf("RESULT SET BUFFER SIZE: \n");
	checkSuccessInt(sqlrcur_getResultSetBufferSize(cur),0);
	sqlrcur_setResultSetBufferSize(cur,2);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),NULL);
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),0);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),NULL);
	sqlrcur_getColumnInfo(cur);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
	checkSuccessString(sqlrcur_getColumnName(cur,0),"TESTNUMBER");
	checkSuccessInt(sqlrcur_getColumnLengthByIndex(cur,0),22);
	checkSuccessString(sqlrcur_getColumnTypeByIndex(cur,0),"NUMBER");
	printf("\n");

	printf("SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
	filename=strdup(sqlrcur_getCacheFileName(cur));
	checkSuccessString(filename,"cachefile1");
	sqlrcur_cacheOff(cur);
	checkSuccessInt(sqlrcur_openCachedResultSet(cur,filename),1);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,7,0),"8");
	free(filename);
	printf("\n");

	printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccessInt(sqlrcur_colCount(cur),7);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccessString(sqlrcur_getColumnName(cur,0),"TESTNUMBER");
	checkSuccessString(sqlrcur_getColumnName(cur,1),"TESTCHAR");
	checkSuccessString(sqlrcur_getColumnName(cur,2),"TESTVARCHAR");
	checkSuccessString(sqlrcur_getColumnName(cur,3),"TESTDATE");
	checkSuccessString(sqlrcur_getColumnName(cur,4),"TESTLONG");
	checkSuccessString(sqlrcur_getColumnName(cur,5),"TESTCLOB");
	checkSuccessString(sqlrcur_getColumnName(cur,6),"TESTBLOB");
	cols=sqlrcur_getColumnNames(cur);
	checkSuccessString(cols[0],"TESTNUMBER");
	checkSuccessString(cols[1],"TESTCHAR");
	checkSuccessString(cols[2],"TESTVARCHAR");
	checkSuccessString(cols[3],"TESTDATE");
	checkSuccessString(cols[4],"TESTLONG");
	checkSuccessString(cols[5],"TESTCLOB");
	checkSuccessString(cols[6],"TESTBLOB");
	printf("\n");

	printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize(cur,2);
	sqlrcur_cacheToFile(cur,"cachefile1");
	sqlrcur_setCacheTtl(cur,200);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
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
				"/tmp/test.socket",NULL,NULL,0,1);
	secondcur=sqlrcur_alloc(secondcon);
	sqlrcon_enableTls(secondcon,NULL,cert,NULL,NULL,"ca",ca,0);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"0");
	checkSuccessInt(sqlrcon_commit(con),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"8");
	checkSuccessInt(sqlrcon_autoCommitOn(con),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10','testclob10',NULL)"),1);
	checkSuccessInt(sqlrcur_sendQuery(secondcur,"select count(*) from testtable"),1);
	checkSuccessString(sqlrcur_getFieldByIndex(secondcur,0,0),"9");
	checkSuccessInt(sqlrcon_autoCommitOff(con),1);
	printf("\n");

	printf("CLOB AND BLOB OUTPUT BIND: \n");
	sqlrcur_sendQuery(cur,"drop table testtable1");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create table testtable1 (testclob clob, testblob blob)"),1);
	sqlrcur_prepareQuery(cur,"insert into testtable1 values ('hello',:var1)");
	sqlrcur_inputBindBlob(cur,"var1","hello",5);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_prepareQuery(cur,"begin select testclob into :clobvar from testtable1;  select testblob into :blobvar from testtable1; end;");
	sqlrcur_defineOutputBindClob(cur,"clobvar");
	sqlrcur_defineOutputBindBlob(cur,"blobvar");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	clobvar=sqlrcur_getOutputBindClob(cur,"clobvar");
	clobvarlength=sqlrcur_getOutputBindLength(cur,"clobvar");
	blobvar=sqlrcur_getOutputBindBlob(cur,"blobvar");
	blobvarlength=sqlrcur_getOutputBindLength(cur,"blobvar");
	checkSuccessStringWithLength(clobvar,"hello",5);
	checkSuccessInt(clobvarlength,5);
	checkSuccessStringWithLength(blobvar,"hello",5);
	checkSuccessInt(blobvarlength,5);
	sqlrcur_sendQuery(cur,"drop table testtable1");
	printf("\n");

	printf("NULL AND EMPTY CLOBS AND CLOBS: \n");
	sqlrcur_getNullsAsNulls(cur);
	sqlrcur_sendQuery(cur,"create table testtable1 (testclob1 clob, testclob2 clob, testblob1 blob, testblob2 blob)");
	sqlrcur_prepareQuery(cur,"insert into testtable1 values (:var1,:var2,:var3,:var4)");
	sqlrcur_inputBindClob(cur,"var1","",0);
	sqlrcur_inputBindClob(cur,"var2",NULL,0);
	sqlrcur_inputBindBlob(cur,"var3","",0);
	sqlrcur_inputBindBlob(cur,"var4",NULL,0);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_sendQuery(cur,"select * from testtable1");
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,0),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,1),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,2),NULL);
	checkSuccessString(sqlrcur_getFieldByIndex(cur,0,3),NULL);
	sqlrcur_sendQuery(cur,"drop table testtable1");
	printf("\n");

	printf("CURSOR BINDS: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"create or replace package types as type cursorType is ref cursor; end;"),1);
	checkSuccessInt(sqlrcur_sendQuery(cur,"create or replace function sp_testtable return types.cursortype as l_cursor    types.cursorType; begin open l_cursor for select * from testtable; return l_cursor; end;"),1);
	sqlrcur_prepareQuery(cur,"begin  :curs:=sp_testtable; end;");
	sqlrcur_defineOutputBindCursor(cur,"curs");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	bindcur=sqlrcur_getOutputBindCursor(cur,"curs");
	checkSuccessInt(sqlrcur_fetchFromBindCursor(bindcur),1);
	checkSuccessString(sqlrcur_getFieldByIndex(bindcur,0,0),"1");
	checkSuccessString(sqlrcur_getFieldByIndex(bindcur,1,0),"2");
	checkSuccessString(sqlrcur_getFieldByIndex(bindcur,2,0),"3");
	checkSuccessString(sqlrcur_getFieldByIndex(bindcur,3,0),"4");
	checkSuccessString(sqlrcur_getFieldByIndex(bindcur,4,0),"5");
	checkSuccessString(sqlrcur_getFieldByIndex(bindcur,5,0),"6");
	checkSuccessString(sqlrcur_getFieldByIndex(bindcur,6,0),"7");
	checkSuccessString(sqlrcur_getFieldByIndex(bindcur,7,0),"8");
	sqlrcur_free(bindcur);
	printf("\n");

	printf("LONG CLOB: \n");
	sqlrcur_sendQuery(cur,"drop table testtable2");
	sqlrcur_sendQuery(cur,"create table testtable2 (testclob clob)");
	sqlrcur_prepareQuery(cur,"insert into testtable2 values (:clobval)");
	for (i=0; i<8*1024; i++) {
		clobval[i]='C';
	}
	clobval[8*1024]='\0';
	sqlrcur_inputBindClob(cur,"clobval",clobval,8*1024);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_sendQuery(cur,"select testclob from testtable2");
	checkSuccessString(clobval,sqlrcur_getFieldByName(cur,0,"TESTCLOB"));
	sqlrcur_prepareQuery(cur,"begin select testclob into :clobbindval from testtable2; end;");
	sqlrcur_defineOutputBindClob(cur,"clobbindval");
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	clobbindvar=sqlrcur_getOutputBindClob(cur,"clobbindval");
	checkSuccessInt(sqlrcur_getOutputBindLength(cur,"clobbindval"),8*1024);
	checkSuccessString(clobval,clobbindvar);
	sqlrcur_sendQuery(cur,"drop table testtable2");
	printf("\n");


	printf("LONG OUTPUT BIND\n");
	sqlrcur_sendQuery(cur,"drop table testtable2");
	sqlrcur_sendQuery(cur,"create table testtable2 (testval varchar2(4000))");
	testval[4000]='\0';
	sqlrcur_prepareQuery(cur,"insert into testtable2 values (:testval)");
	for (i=0; i<4000; i++) {
		testval[i]='C';
	}
	sqlrcur_inputBindString(cur,"testval",testval);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_sendQuery(cur,"select testval from testtable2");
	checkSuccessString(testval,sqlrcur_getFieldByName(cur,0,"TESTVAL"));
	sprintf(query,"begin :bindval:='%s'; end;",testval);
	sqlrcur_prepareQuery(cur,query);
	sqlrcur_defineOutputBindString(cur,"bindval",4000);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	checkSuccessInt(sqlrcur_getOutputBindLength(cur,"bindval"),4000);
	checkSuccessString(sqlrcur_getOutputBindString(cur,"bindval"),testval);
	sqlrcur_sendQuery(cur,"drop table testtable2");
	printf("\n");

	printf("NEGATIVE INPUT BIND\n");
	sqlrcur_sendQuery(cur,"create table testtable2 (testval number)");
	sqlrcur_prepareQuery(cur,"insert into testtable2 values (:testval)");
	sqlrcur_inputBindLong(cur,"testval",-1);
	checkSuccessInt(sqlrcur_executeQuery(cur),1);
	sqlrcur_sendQuery(cur,"select testval from testtable2");
	checkSuccessString(sqlrcur_getFieldByName(cur,0,"TESTVAL"),"-1");
	sqlrcur_sendQuery(cur,"drop table testtable2");
	printf("\n");

	printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),1);
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
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),0);
	checkSuccessInt(sqlrcur_sendQuery(cur,"select * from testtable order by testnumber"),0);
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
