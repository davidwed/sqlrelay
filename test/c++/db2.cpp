// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
			stdoutput.printf("failure ");
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
stdoutput.printf("%d!=%d\n",charstring::length(value),charstring::length(success));
		stdoutput.printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%d!=%d\n",value,success);
		stdoutput.printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(double value, double success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%f!=%f\n",value,success);
		stdoutput.printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

int	main(int argc, char **argv) {

	const char	*bindvars[13]={"1","2","3","4","5","6","7","8","9","10","11","12",NULL};
	const char	*bindvals[12]={"4","4","4","4.4","4.4","4.4",
			"testchar4","testvarchar4","01/01/2004","04:00:00","testclob4",NULL};
	const char	*subvars[4]={"var1","var2","var3",NULL};
	const char	*subvalstrings[3]={"hi","hello","bye"};
	int64_t		subvallongs[3]={1,2,3};
	double		subvaldoubles[3]={10.55,10.556,10.5556};
	uint32_t	precs[3]={4,5,6};
	uint32_t	scales[3]={2,3,4};
	const char * const *cols;
	const char * const *fields;
	uint16_t	port;
	char		*socket;
	uint16_t	id;
	char		*filename;
	uint32_t	*fieldlens;

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);

	// get database type
	stdoutput.printf("IDENTIFY: \n");
	checkSuccess(con->identify(),"db2");
	stdoutput.printf("\n");

	// ping
	stdoutput.printf("PING: \n");
	checkSuccess(con->ping(),1);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	stdoutput.printf("CREATE TEMPTABLE: \n");
	checkSuccess(cur->sendQuery("create table testtable (testsmallint smallint, testint integer, testbigint bigint, testdecimal decimal(10,2), testreal real, testdouble double, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp, testclob clob, testblob blob)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable values (1,1,1,1.1,1.1,1.1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL,'testclob1',blob('testblob1'))"),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY POSITION: \n");
	cur->prepareQuery("insert into testtable values (?,?,?,?,?,?,?,?,?,?,NULL,?,?)");
	checkSuccess(cur->countBindVariables(),12);
	cur->inputBind("1",2);
	cur->inputBind("2",2);
	cur->inputBind("3",2);
	cur->inputBind("4",2.2,4,2);
	cur->inputBind("5",2.2,4,2);
	cur->inputBind("6",2.2,4,2);
	cur->inputBind("7","testchar2");
	cur->inputBind("8","testvarchar2");
	cur->inputBind("9",2002,1,1,-1,-1,-1,-1,NULL,false);
	cur->inputBind("10",-1,-1,-1,2,0,0,0,NULL,false);
	cur->inputBindClob("11","testclob2",9);
	cur->inputBindBlob("12","testblob2",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",3);
	cur->inputBind("2",3);
	cur->inputBind("3",3);
	cur->inputBind("4",3.3,4,2);
	cur->inputBind("5",3.3,4,2);
	cur->inputBind("6",3.3,4,2);
	cur->inputBind("7","testchar3");
	cur->inputBind("8","testvarchar3");
	cur->inputBind("9",2003,1,1,-1,-1,-1,-1,NULL,false);
	cur->inputBind("10",-1,-1,-1,3,0,0,0,NULL,false);
	cur->inputBindClob("11","testclob3",9);
	cur->inputBindBlob("12","testblob3",9);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY OF BINDS BY POSITION: \n");
	cur->clearBinds();
	cur->inputBinds(bindvars,bindvals);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable values (5,5,5,5.5,5.5,5.5,'testchar5','testvarchar5','01/01/2005','05:00:00',NULL,'testclob5',blob('testblob5'))"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (6,6,6,6.6,6.6,6.6,'testchar6','testvarchar6','01/01/2006','06:00:00',NULL,'testclob6',blob('testblob6'))"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (7,7,7,7.7,7.7,7.7,'testchar7','testvarchar7','01/01/2007','07:00:00',NULL,'testclob7',blob('testblob7'))"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (8,8,8,8.8,8.8,8.8,'testchar8','testvarchar8','01/01/2008','08:00:00',NULL,'testclob8',blob('testblob8'))"),1);
	stdoutput.printf("\n");

	stdoutput.printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),1);
	stdoutput.printf("\n");

	stdoutput.printf("STORED PROCEDURE: \n");
	// return multiple values
	cur->sendQuery("drop procedure testproc");
	checkSuccess(cur->sendQuery("create procedure testproc(in in1 int, in in2 double, in in3 varchar(20), in in4 clob, in in5 blob, out out1 int, out out2 double, out out3 varchar(20), out out4 clob, out out5 blob) language sql begin set out1 = in1; set out2 = in2; set out3 = in3; set out4 = in4; set out5 = in5; end"),1);
	cur->prepareQuery("call testproc(?,?,?,?,?,?,?,?,?,?)");
	cur->inputBind("1",1);
	cur->inputBind("2",1.1,2,1);
	cur->inputBind("3","hello");
	cur->inputBindClob("4","clob",4);
	cur->inputBindBlob("5","blob",4);
	cur->defineOutputBindInteger("6");
	cur->defineOutputBindDouble("7");
	cur->defineOutputBindString("8",20);
	cur->defineOutputBindClob("9");
	cur->defineOutputBindBlob("10");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindInteger("6"),1);
	checkSuccess(cur->getOutputBindDouble("7"),1.1);
	checkSuccess(cur->getOutputBindString("8"),"hello");
	checkSuccess(cur->getOutputBindClob("9"),"clob");
	checkSuccess(cur->getOutputBindBlob("10"),"blob");
	checkSuccess(cur->sendQuery("drop procedure testproc"),1);
	stdoutput.printf("\n");

	stdoutput.printf("STORED PROCEDURE RETURNING RESULT SET: \n");
	checkSuccess(cur->sendQuery("create procedure testproc() result set 1 language sql begin declare c1 cursor with return for select * from testtable; open c1; end"),1);
	checkSuccess(cur->sendQuery("call testproc()"),1);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->sendQuery("drop procedure testproc"),1);
	stdoutput.printf("\n");

	stdoutput.printf("LONG BLOB: \n");
	cur->sendQuery("drop table testtable1");
	cur->sendQuery("create table testtable1 (testclob clob)");
	cur->prepareQuery("insert into testtable1 values (?)");
	char	clobval[20*1024+1];
	for (int i=0; i<20*1024; i++) {
		clobval[i]='C';
	}
	clobval[20*1024]='\0';
	cur->inputBindClob("1",clobval,20*1024);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testclob from testtable1");
	checkSuccess(cur->getFieldLength(0,"TESTCLOB"),20*1024);
	checkSuccess(cur->getField(0,"TESTCLOB"),clobval);
	checkSuccess(cur->sendQuery("create procedure testproc(in in1 clob, out out1 clob) language sql begin set out1 = in1; end"),1);
	cur->prepareQuery("call testproc(?,?)");
	cur->inputBindClob("1",clobval,20*1024);
	cur->defineOutputBindClob("2");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindLength("2"),20*1024);
	checkSuccess(cur->getOutputBindClob("2"),clobval);
	checkSuccess(cur->sendQuery("drop procedure testproc"),1);
	stdoutput.printf("\n");

	stdoutput.printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),13);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES: \n");
	checkSuccess(cur->getColumnName(0),"TESTSMALLINT");
	checkSuccess(cur->getColumnName(1),"TESTINT");
	checkSuccess(cur->getColumnName(2),"TESTBIGINT");
	checkSuccess(cur->getColumnName(3),"TESTDECIMAL");
	checkSuccess(cur->getColumnName(4),"TESTREAL");
	checkSuccess(cur->getColumnName(5),"TESTDOUBLE");
	checkSuccess(cur->getColumnName(6),"TESTCHAR");
	checkSuccess(cur->getColumnName(7),"TESTVARCHAR");
	checkSuccess(cur->getColumnName(8),"TESTDATE");
	checkSuccess(cur->getColumnName(9),"TESTTIME");
	checkSuccess(cur->getColumnName(10),"TESTTIMESTAMP");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"TESTSMALLINT");
	checkSuccess(cols[1],"TESTINT");
	checkSuccess(cols[2],"TESTBIGINT");
	checkSuccess(cols[3],"TESTDECIMAL");
	checkSuccess(cols[4],"TESTREAL");
	checkSuccess(cols[5],"TESTDOUBLE");
	checkSuccess(cols[6],"TESTCHAR");
	checkSuccess(cols[7],"TESTVARCHAR");
	checkSuccess(cols[8],"TESTDATE");
	checkSuccess(cols[9],"TESTTIME");
	checkSuccess(cols[10],"TESTTIMESTAMP");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType((uint32_t)0),"SMALLINT");
	checkSuccess(cur->getColumnType("TESTSMALLINT"),"SMALLINT");
	checkSuccess(cur->getColumnType(1),"INTEGER");
	checkSuccess(cur->getColumnType("TESTINT"),"INTEGER");
	checkSuccess(cur->getColumnType(2),"BIGINT");
	checkSuccess(cur->getColumnType("TESTBIGINT"),"BIGINT");
	checkSuccess(cur->getColumnType(3),"DECIMAL");
	checkSuccess(cur->getColumnType("TESTDECIMAL"),"DECIMAL");
	checkSuccess(cur->getColumnType(4),"REAL");
	checkSuccess(cur->getColumnType("TESTREAL"),"REAL");
	checkSuccess(cur->getColumnType(5),"DOUBLE");
	checkSuccess(cur->getColumnType("TESTDOUBLE"),"DOUBLE");
	checkSuccess(cur->getColumnType(6),"CHAR");
	checkSuccess(cur->getColumnType("TESTCHAR"),"CHAR");
	checkSuccess(cur->getColumnType(7),"VARCHAR");
	checkSuccess(cur->getColumnType("TESTVARCHAR"),"VARCHAR");
	checkSuccess(cur->getColumnType(8),"DATE");
	checkSuccess(cur->getColumnType("TESTDATE"),"DATE");
	checkSuccess(cur->getColumnType(9),"TIME");
	checkSuccess(cur->getColumnType("TESTTIME"),"TIME");
	checkSuccess(cur->getColumnType(10),"TIMESTAMP");
	checkSuccess(cur->getColumnType("TESTTIMESTAMP"),"TIMESTAMP");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),2);
	checkSuccess(cur->getColumnLength("TESTSMALLINT"),2);
	checkSuccess(cur->getColumnLength(1),4);
	checkSuccess(cur->getColumnLength("TESTINT"),4);
	checkSuccess(cur->getColumnLength(2),8);
	checkSuccess(cur->getColumnLength("TESTBIGINT"),8);
	checkSuccess(cur->getColumnLength(3),12);
	checkSuccess(cur->getColumnLength("TESTDECIMAL"),12);
	checkSuccess(cur->getColumnLength(4),4);
	checkSuccess(cur->getColumnLength("TESTREAL"),4);
	checkSuccess(cur->getColumnLength(5),8);
	checkSuccess(cur->getColumnLength("TESTDOUBLE"),8);
	checkSuccess(cur->getColumnLength(6),40);
	checkSuccess(cur->getColumnLength("TESTCHAR"),40);
	checkSuccess(cur->getColumnLength(7),40);
	checkSuccess(cur->getColumnLength("TESTVARCHAR"),40);
	checkSuccess(cur->getColumnLength(8),6);
	checkSuccess(cur->getColumnLength("TESTDATE"),6);
	checkSuccess(cur->getColumnLength(9),6);
	checkSuccess(cur->getColumnLength("TESTTIME"),6);
	checkSuccess(cur->getColumnLength(10),16);
	checkSuccess(cur->getColumnLength("TESTTIMESTAMP"),16);
	stdoutput.printf("\n");

	stdoutput.printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest("TESTSMALLINT"),1);
	checkSuccess(cur->getLongest(1),1);
	checkSuccess(cur->getLongest("TESTINT"),1);
	checkSuccess(cur->getLongest(2),1);
	checkSuccess(cur->getLongest("TESTBIGINT"),1);
	checkSuccess(cur->getLongest(3),4);
	checkSuccess(cur->getLongest("TESTDECIMAL"),4);
	//checkSuccess(cur->getLongest(4),3);
	//checkSuccess(cur->getLongest("TESTREAL"),3);
	//checkSuccess(cur->getLongest(5),3);
	//checkSuccess(cur->getLongest("TESTDOUBLE"),3);
	checkSuccess(cur->getLongest(6),40);
	checkSuccess(cur->getLongest("TESTCHAR"),40);
	checkSuccess(cur->getLongest(7),12);
	checkSuccess(cur->getLongest("TESTVARCHAR"),12);
	checkSuccess(cur->getLongest(8),10);
	checkSuccess(cur->getLongest("TESTDATE"),10);
	checkSuccess(cur->getLongest(9),8);
	checkSuccess(cur->getLongest("TESTTIME"),8);
	stdoutput.printf("\n");

	stdoutput.printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("TOTAL ROWS: \n");
	checkSuccess(cur->totalRows(),0);
	stdoutput.printf("\n");

	stdoutput.printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	stdoutput.printf("\n");

	stdoutput.printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),"1");
	checkSuccess(cur->getField(0,3),"1.10");
	//checkSuccess(cur->getField(0,4),"1.1");
	//checkSuccess(cur->getField(0,5),"1.1");
	checkSuccess(cur->getField(0,6),"testchar1                               ");
	checkSuccess(cur->getField(0,7),"testvarchar1");
	checkSuccess(cur->getField(0,8),"2001-01-01");
	checkSuccess(cur->getField(0,9),"01:00:00");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(7,1),"8");
	checkSuccess(cur->getField(7,2),"8");
	checkSuccess(cur->getField(7,3),"8.80");
	//checkSuccess(cur->getField(7,4),"8.8");
	//checkSuccess(cur->getField(7,5),"8.8");
	checkSuccess(cur->getField(7,6),"testchar8                               ");
	checkSuccess(cur->getField(7,7),"testvarchar8");
	checkSuccess(cur->getField(7,8),"2008-01-01");
	checkSuccess(cur->getField(7,9),"08:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(0,1),1);
	checkSuccess(cur->getFieldLength(0,2),1);
	checkSuccess(cur->getFieldLength(0,3),4);
	//checkSuccess(cur->getFieldLength(0,4),3);
	//checkSuccess(cur->getFieldLength(0,5),3);
	checkSuccess(cur->getFieldLength(0,6),40);
	checkSuccess(cur->getFieldLength(0,7),12);
	checkSuccess(cur->getFieldLength(0,8),10);
	checkSuccess(cur->getFieldLength(0,9),8);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(7,1),1);
	checkSuccess(cur->getFieldLength(7,2),1);
	checkSuccess(cur->getFieldLength(7,3),4);
	//checkSuccess(cur->getFieldLength(7,4),3);
	//checkSuccess(cur->getFieldLength(7,5),3);
	checkSuccess(cur->getFieldLength(7,6),40);
	checkSuccess(cur->getFieldLength(7,7),12);
	checkSuccess(cur->getFieldLength(7,8),10);
	checkSuccess(cur->getFieldLength(7,9),8);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"TESTSMALLINT"),"1");
	checkSuccess(cur->getField(0,"TESTINT"),"1");
	checkSuccess(cur->getField(0,"TESTBIGINT"),"1");
	checkSuccess(cur->getField(0,"TESTDECIMAL"),"1.10");
	//checkSuccess(cur->getField(0,"TESTREAL"),"1.1");
	//checkSuccess(cur->getField(0,"TESTDOUBLE"),"1.1");
	checkSuccess(cur->getField(0,"TESTCHAR"),"testchar1                               ");
	checkSuccess(cur->getField(0,"TESTVARCHAR"),"testvarchar1");
	checkSuccess(cur->getField(0,"TESTDATE"),"2001-01-01");
	checkSuccess(cur->getField(0,"TESTTIME"),"01:00:00");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,"TESTSMALLINT"),"8");
	checkSuccess(cur->getField(7,"TESTINT"),"8");
	checkSuccess(cur->getField(7,"TESTBIGINT"),"8");
	checkSuccess(cur->getField(7,"TESTDECIMAL"),"8.80");
	//checkSuccess(cur->getField(7,"TESTREAL"),"8.8");
	//checkSuccess(cur->getField(7,"TESTDOUBLE"),"8.8");
	checkSuccess(cur->getField(7,"TESTCHAR"),"testchar8                               ");
	checkSuccess(cur->getField(7,"TESTVARCHAR"),"testvarchar8");
	checkSuccess(cur->getField(7,"TESTDATE"),"2008-01-01");
	checkSuccess(cur->getField(7,"TESTTIME"),"08:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"TESTSMALLINT"),1);
	checkSuccess(cur->getFieldLength(0,"TESTINT"),1);
	checkSuccess(cur->getFieldLength(0,"TESTBIGINT"),1);
	checkSuccess(cur->getFieldLength(0,"TESTDECIMAL"),4);
	//checkSuccess(cur->getFieldLength(0,"TESTREAL"),3);
	//checkSuccess(cur->getFieldLength(0,"TESTDOUBLE"),3);
	checkSuccess(cur->getFieldLength(0,"TESTCHAR"),40);
	checkSuccess(cur->getFieldLength(0,"TESTVARCHAR"),12);
	checkSuccess(cur->getFieldLength(0,"TESTDATE"),10);
	checkSuccess(cur->getFieldLength(0,"TESTTIME"),8);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,"TESTSMALLINT"),1);
	checkSuccess(cur->getFieldLength(7,"TESTINT"),1);
	checkSuccess(cur->getFieldLength(7,"TESTBIGINT"),1);
	checkSuccess(cur->getFieldLength(7,"TESTDECIMAL"),4);
	//checkSuccess(cur->getFieldLength(7,"TESTREAL"),3);
	//checkSuccess(cur->getFieldLength(7,"TESTDOUBLE"),3);
	checkSuccess(cur->getFieldLength(7,"TESTCHAR"),40);
	checkSuccess(cur->getFieldLength(7,"TESTVARCHAR"),12);
	checkSuccess(cur->getFieldLength(7,"TESTDATE"),10);
	checkSuccess(cur->getFieldLength(7,"TESTTIME"),8);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	checkSuccess(fields[1],"1");
	checkSuccess(fields[2],"1");
	checkSuccess(fields[3],"1.10");
	//checkSuccess(fields[4],"1.1");
	//checkSuccess(fields[5],"1.1");
	checkSuccess(fields[6],"testchar1                               ");
	checkSuccess(fields[7],"testvarchar1");
	checkSuccess(fields[8],"2001-01-01");
	checkSuccess(fields[9],"01:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	checkSuccess(fieldlens[1],1);
	checkSuccess(fieldlens[2],1);
	checkSuccess(fieldlens[3],4);
	//checkSuccess(fieldlens[4],3);
	//checkSuccess(fieldlens[5],3);
	checkSuccess(fieldlens[6],40);
	checkSuccess(fieldlens[7],12);
	checkSuccess(fieldlens[8],10);
	checkSuccess(fieldlens[9],8);
	stdoutput.printf("\n");

	stdoutput.printf("INDIVIDUAL SUBSTITUTIONS: \n");
	cur->prepareQuery("values ($(var1),'$(var2)','$(var3)')");
	cur->substitution("var1",1);
	cur->substitution("var2","hello");
	cur->substitution("var3",10.5556,6,4);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"10.5556");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("values ('$(var1)','$(var2)','$(var3)')");
	cur->substitutions(subvars,subvalstrings);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"hi");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"bye");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("values ($(var1),$(var2),$(var3))");
	cur->substitutions(subvars,subvallongs);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"2");
	checkSuccess(cur->getField(0,2),"3");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("values ($(var1),$(var2),$(var3))");
	cur->substitutions(subvars,subvaldoubles,precs,scales);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"10.55");
	checkSuccess(cur->getField(0,1),"10.556");
	checkSuccess(cur->getField(0,2),"10.5556");
	stdoutput.printf("\n");

	stdoutput.printf("NULLS as Nulls: \n");
	cur->sendQuery("drop table testtable1");
	cur->sendQuery("create table testtable1 (col1 char(1), col2 char(1), col3 char(1))");
	cur->getNullsAsNulls();
	checkSuccess(cur->sendQuery("insert into testtable1 values ('1',NULL,NULL)"),1);
	checkSuccess(cur->sendQuery("select * from testtable1"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),NULL);
	checkSuccess(cur->getField(0,2),NULL);
	cur->getNullsAsEmptyStrings();
	checkSuccess(cur->sendQuery("select * from testtable1"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"");
	checkSuccess(cur->getField(0,2),"");
	checkSuccess(cur->sendQuery("drop table testtable1"),1);
	cur->getNullsAsNulls();
	stdoutput.printf("\n");
	
	stdoutput.printf("RESULT SET BUFFER SIZE: \n");
	checkSuccess(cur->getResultSetBufferSize(),0);
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	checkSuccess(cur->getResultSetBufferSize(),2);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),0);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),2);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),2);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),4);
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("DONT GET COLUMN INFO: \n");
	cur->dontGetColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	checkSuccess(cur->getColumnName((uint32_t)0),NULL);
	checkSuccess(cur->getColumnLength((uint32_t)0),0);
	checkSuccess(cur->getColumnType((uint32_t)0),NULL);
	cur->getColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	checkSuccess(cur->getColumnName((uint32_t)0),"TESTSMALLINT");
	checkSuccess(cur->getColumnLength((uint32_t)0),2);
	checkSuccess(cur->getColumnType((uint32_t)0),"SMALLINT");
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->getField(3,(uint32_t)0),"4");
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->getField(3,(uint32_t)0),"4");
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->getField(3,(uint32_t)0),"4");
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED RESULT SET: \n");
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeResultSet(id),1);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),4);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),6);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	cur->setResultSetBufferSize(0);
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET: \n");
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(cur->colCount(),13);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(cur->getColumnName(0),"TESTSMALLINT");
	checkSuccess(cur->getColumnName(1),"TESTINT");
	checkSuccess(cur->getColumnName(2),"TESTBIGINT");
	checkSuccess(cur->getColumnName(3),"TESTDECIMAL");
	checkSuccess(cur->getColumnName(4),"TESTREAL");
	checkSuccess(cur->getColumnName(5),"TESTDOUBLE");
	checkSuccess(cur->getColumnName(6),"TESTCHAR");
	checkSuccess(cur->getColumnName(7),"TESTVARCHAR");
	checkSuccess(cur->getColumnName(8),"TESTDATE");
	checkSuccess(cur->getColumnName(9),"TESTTIME");
	checkSuccess(cur->getColumnName(10),"TESTTIMESTAMP");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"TESTSMALLINT");
	checkSuccess(cols[1],"TESTINT");
	checkSuccess(cols[2],"TESTBIGINT");
	checkSuccess(cols[3],"TESTDECIMAL");
	checkSuccess(cols[4],"TESTREAL");
	checkSuccess(cols[5],"TESTDOUBLE");
	checkSuccess(cols[6],"TESTCHAR");
	checkSuccess(cols[7],"TESTVARCHAR");
	checkSuccess(cols[8],"TESTDATE");
	checkSuccess(cols[9],"TESTTIME");
	checkSuccess(cols[10],"TESTTIMESTAMP");
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("FROM ONE CACHE FILE TO ANOTHER: \n");
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");

	stdoutput.printf("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	cur->setResultSetBufferSize(0);
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	stdoutput.printf("\n");
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeCachedResultSet(id,filename),1);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),4);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),6);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	cur->cacheOff();
	stdoutput.printf("\n");
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeResultSet(id),1);
	checkSuccess(cur->getField(4,(uint32_t)0),NULL);
	checkSuccess(cur->getField(5,(uint32_t)0),NULL);
	checkSuccess(cur->getField(6,(uint32_t)0),NULL);
	checkSuccess(cur->getField(7,(uint32_t)0),NULL);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");
	stdoutput.printf("\n");

	// invalid queries...
	stdoutput.printf("INVALID QUERIES: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),0);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	stdoutput.printf("\n");

	return 0;
}
