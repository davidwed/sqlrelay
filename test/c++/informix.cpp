// Copyright (c) 2001  David Muse
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
			process::exit(0);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
		stdoutput.printf("failure ");
		delete cur;
		delete con;
		process::exit(0);
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
		process::exit(0);
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
		process::exit(0);
	}
}

int	main(int argc, char **argv) {

	const char	*bindvars[17]={"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16",NULL};
	const char	*bindvals[17]={"t","4","4","4","4","4.4","4.4","4.4","4.4","testchar4","testnchar4","testvarchar4","testnvarchar4","testlvarchar4","01/01/2004","2004-01-01 04:00:00",NULL};
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
	con=new sqlrconnection("sqlrserver",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);

	// get database type
	stdoutput.printf("IDENTIFY: \n");
	checkSuccess(con->identify(),"informix");
	stdoutput.printf("\n");

	// ping
	stdoutput.printf("PING: \n");
	checkSuccess(con->ping(),1);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	stdoutput.printf("CREATE TEMPTABLE: \n");
	checkSuccess(cur->sendQuery("create table testtable (testboolean boolean, testsmallint smallint, testint integer, testbigint bigint, testint8 int8, testdecimal decimal(10,2), testmoney money, testsmallfloat smallfloat, testfloat float, testchar char(40), testnchar nchar(40), testvarchar varchar(40), testnvarchar nvarchar(40), testlvarchar lvarchar(40), testdate date, testdatetime datetime year to second, testtext text, testbyte byte)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable values ('t',1,1,1,1,1.1,1.1,1.1,1.1,'testchar1','testnchar1','testvarchar1','testnvarchar1','testlvarchar1','01/01/2001','2001-01-01 01:00:00','testtext1',null)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY POSITION: \n");
	cur->prepareQuery("insert into testtable values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
	checkSuccess(cur->countBindVariables(),18);
	cur->inputBind("1","t");
	cur->inputBind("2",2);
	cur->inputBind("3",2);
	cur->inputBind("4",2);
	cur->inputBind("5",2);
	cur->inputBind("6",2.2,4,2);
	cur->inputBind("7",2.2,4,2);
	cur->inputBind("8",2.2,4,2);
	cur->inputBind("9",2.2,4,2);
	cur->inputBind("10","testchar2");
	cur->inputBind("11","testnchar2");
	cur->inputBind("12","testvarchar2");
	cur->inputBind("13","testvarnchar2");
	cur->inputBind("14","testvarlchar2");
	cur->inputBind("15","01/01/2002");
	cur->inputBind("16","2002-01-01 02:00:00");
	cur->inputBindClob("17","testtext1",9);
	cur->inputBindBlob("18","testbyte1",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1","t");
	cur->inputBind("2",3);
	cur->inputBind("3",3);
	cur->inputBind("4",3);
	cur->inputBind("5",3);
	cur->inputBind("6",3.3,4,2);
	cur->inputBind("7",3.3,4,2);
	cur->inputBind("8",3.3,4,2);
	cur->inputBind("9",3.3,4,2);
	cur->inputBind("10","testchar3");
	cur->inputBind("11","testnchar3");
	cur->inputBind("12","testvarchar3");
	cur->inputBind("13","testvarnchar3");
	cur->inputBind("14","testvarlchar3");
	cur->inputBind("15","01/01/2003");
	cur->inputBind("16","2003-01-01 03:00:00");
	cur->inputBindClob("17","testtext3",9);
	cur->inputBindBlob("18","testbyte3",9);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY OF BINDS BY POSITION: \n");
	cur->clearBinds();
	cur->prepareQuery("insert into testtable values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,null,null)");
	cur->inputBinds(bindvars,bindvals);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable values ('t',5,5,5,5,5.5,5.5,5.5,5.5,'testchar5','testnchar5','testvarchar5','testnvarchar5','testlvarchar5','01/01/2005','2005-01-01 05:00:00','testtext5',null)"),1);
	checkSuccess(cur->sendQuery("insert into testtable values ('t',6,6,6,6,6.6,6.6,6.6,6.6,'testchar6','testnchar6','testvarchar6','testnvarchar6','testlvarchar6','01/01/2006','2006-01-01 06:00:00','testtext6',null)"),1);
	checkSuccess(cur->sendQuery("insert into testtable values ('t',7,7,7,7,7.7,7.7,7.7,7.7,'testchar7','testnchar7','testvarchar7','testnvarchar7','testlvarchar7','01/01/2007','2007-01-01 07:00:00','testtext7',null)"),1);
	checkSuccess(cur->sendQuery("insert into testtable values ('t',8,8,8,8,8.8,8.8,8.8,8.8,'testchar8','testnchar8','testvarchar8','testnvarchar8','testlvarchar8','01/01/2008','2008-01-01 08:00:00','testtext8',null)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),1);
	stdoutput.printf("\n");

	stdoutput.printf("STORED PROCEDURE: \n");
	// return multiple values
	cur->sendQuery("drop procedure testproc");
	checkSuccess(cur->sendQuery("create procedure testproc(in1 int, in2 float, in3 varchar(20), out out1 int, out out2 float, out out3 varchar(20)) let out1 = in1; let out2 = in2; let out3 = in3; end procedure;"),1);
	cur->prepareQuery("{call testproc(?,?,?,?,?,?)}");
	cur->inputBind("1",1);
	cur->inputBind("2",1.1,2,1);
	cur->inputBind("3","hello");
	cur->defineOutputBindInteger("4");
	cur->defineOutputBindDouble("5");
	cur->defineOutputBindString("6",20);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindInteger("4"),1);
	checkSuccess(cur->getOutputBindDouble("5"),1.1);
	checkSuccess(cur->getOutputBindString("6"),"hello");
	checkSuccess(cur->sendQuery("drop procedure testproc"),1);
	stdoutput.printf("\n");

	stdoutput.printf("STORED PROCEDURE RETURNING RESULT SET: \n");
	checkSuccess(cur->sendQuery("create procedure testproc() returning boolean, smallint, varchar(40); define out1 boolean; define out2 smallint; define out3 varchar(40); foreach select testboolean,testsmallint,testvarchar into out1,out2,out3 from testtable return out1,out2,out3 with resume; end foreach; end procedure;"),1);
	checkSuccess(cur->sendQuery("{call testproc()}"),1);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->sendQuery("drop procedure testproc"),1);
	stdoutput.printf("\n");

	stdoutput.printf("LONG BLOB: \n");
	cur->sendQuery("drop table testtable1");
	cur->sendQuery("create table testtable1 (testtext text)");
	cur->prepareQuery("insert into testtable1 values (?)");
	char	textval[20*1024+1];
	for (int i=0; i<20*1024; i++) {
		textval[i]='C';
	}
	textval[20*1024]='\0';
	cur->inputBindClob("1",textval,20*1024);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testtext from testtable1");
	checkSuccess(cur->getFieldLength(0,"testtext"),20*1024);
	checkSuccess(cur->getField(0,"testtext"),textval);
	// for some reason stored procedures can only use clob types,
	// rather than text
	checkSuccess(cur->sendQuery("create procedure testproc(in1 clob, out out1 clob) let out1 = in1; end procedure;"),1);
	cur->prepareQuery("{call testproc(?,?)}");
	cur->inputBindClob("1",textval,20*1024);
	cur->defineOutputBindClob("2");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindLength("2"),20*1024);
	checkSuccess(cur->getOutputBindClob("2"),textval);
	checkSuccess(cur->sendQuery("drop procedure testproc"),1);
	stdoutput.printf("\n");

	stdoutput.printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),18);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES: \n");
	checkSuccess(cur->getColumnName(0),"testboolean");
	checkSuccess(cur->getColumnName(1),"testsmallint");
	checkSuccess(cur->getColumnName(2),"testint");
	checkSuccess(cur->getColumnName(3),"testbigint");
	checkSuccess(cur->getColumnName(4),"testint8");
	checkSuccess(cur->getColumnName(5),"testdecimal");
	checkSuccess(cur->getColumnName(6),"testmoney");
	checkSuccess(cur->getColumnName(7),"testsmallfloat");
	checkSuccess(cur->getColumnName(8),"testfloat");
	checkSuccess(cur->getColumnName(9),"testchar");
	checkSuccess(cur->getColumnName(10),"testnchar");
	checkSuccess(cur->getColumnName(11),"testvarchar");
	checkSuccess(cur->getColumnName(12),"testnvarchar");
	checkSuccess(cur->getColumnName(13),"testlvarchar");
	checkSuccess(cur->getColumnName(14),"testdate");
	checkSuccess(cur->getColumnName(15),"testdatetime");
	checkSuccess(cur->getColumnName(16),"testtext");
	checkSuccess(cur->getColumnName(17),"testbyte");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testboolean");
	checkSuccess(cols[1],"testsmallint");
	checkSuccess(cols[2],"testint");
	checkSuccess(cols[3],"testbigint");
	checkSuccess(cols[4],"testint8");
	checkSuccess(cols[5],"testdecimal");
	checkSuccess(cols[6],"testmoney");
	checkSuccess(cols[7],"testsmallfloat");
	checkSuccess(cols[8],"testfloat");
	checkSuccess(cols[9],"testchar");
	checkSuccess(cols[10],"testnchar");
	checkSuccess(cols[11],"testvarchar");
	checkSuccess(cols[12],"testnvarchar");
	checkSuccess(cols[13],"testlvarchar");
	checkSuccess(cols[14],"testdate");
	checkSuccess(cols[15],"testdatetime");
	checkSuccess(cols[16],"testtext");
	checkSuccess(cols[17],"testbyte");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType((uint32_t)0),"BOOLEAN");
	checkSuccess(cur->getColumnType("TESTBOOLEAN"),"BOOLEAN");
	checkSuccess(cur->getColumnType(1),"SMALLINT");
	checkSuccess(cur->getColumnType("TESTSMALLINT"),"SMALLINT");
	checkSuccess(cur->getColumnType(2),"INTEGER");
	checkSuccess(cur->getColumnType("TESTINT"),"INTEGER");
	checkSuccess(cur->getColumnType(3),"BIGINT");
	checkSuccess(cur->getColumnType("TESTBIGINT"),"BIGINT");
	checkSuccess(cur->getColumnType(4),"INT8");
	checkSuccess(cur->getColumnType("TESTINT8"),"INT8");
	checkSuccess(cur->getColumnType(5),"DECIMAL");
	checkSuccess(cur->getColumnType("TESTDECIMAL"),"DECIMAL");
	//checkSuccess(cur->getColumnType(6),"MONEY");
	//checkSuccess(cur->getColumnType("TESTMONEY"),"MONEY");
	checkSuccess(cur->getColumnType(6),"DECIMAL");
	checkSuccess(cur->getColumnType("TESTMONEY"),"DECIMAL");
	checkSuccess(cur->getColumnType(7),"SMALLFLOAT");
	checkSuccess(cur->getColumnType("TESTSMALLFLOAT"),"SMALLFLOAT");
	checkSuccess(cur->getColumnType(8),"FLOAT");
	checkSuccess(cur->getColumnType("TESTFLOAT"),"FLOAT");
	checkSuccess(cur->getColumnType(9),"CHAR");
	checkSuccess(cur->getColumnType("TESTCHAR"),"CHAR");
	//checkSuccess(cur->getColumnType(10),"NCHAR");
	//checkSuccess(cur->getColumnType("TESTNCHAR"),"NCHAR");
	checkSuccess(cur->getColumnType(10),"CHAR");
	checkSuccess(cur->getColumnType("TESTNCHAR"),"CHAR");
	checkSuccess(cur->getColumnType(11),"VARCHAR");
	checkSuccess(cur->getColumnType("TESTVARCHAR"),"VARCHAR");
	//checkSuccess(cur->getColumnType(12),"NVARCHAR");
	//checkSuccess(cur->getColumnType("TESTNVARCHAR"),"NVARCHAR");
	checkSuccess(cur->getColumnType(12),"VARCHAR");
	checkSuccess(cur->getColumnType("TESTNVARCHAR"),"VARCHAR");
	//checkSuccess(cur->getColumnType(13),"LVARCHAR");
	//checkSuccess(cur->getColumnType("TESTLVARCHAR"),"LVARCHAR");
	checkSuccess(cur->getColumnType(13),"VARCHAR");
	checkSuccess(cur->getColumnType("TESTLVARCHAR"),"VARCHAR");
	checkSuccess(cur->getColumnType(14),"DATE");
	checkSuccess(cur->getColumnType("TESTDATE"),"DATE");
	checkSuccess(cur->getColumnType(15),"DATETIME");
	checkSuccess(cur->getColumnType("TESTDATETIME"),"DATETIME");
	checkSuccess(cur->getColumnType(16),"TEXT");
	checkSuccess(cur->getColumnType("TESTTEXT"),"TEXT");
	checkSuccess(cur->getColumnType(17),"BYTE");
	checkSuccess(cur->getColumnType("TESTBYTE"),"BYTE");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),1);
	checkSuccess(cur->getColumnLength("TESTBOOLEAN"),1);
	checkSuccess(cur->getColumnLength(1),5);
	checkSuccess(cur->getColumnLength("TESTSMALLINT"),5);
	checkSuccess(cur->getColumnLength(2),10);
	checkSuccess(cur->getColumnLength("TESTINT"),10);
	checkSuccess(cur->getColumnLength(3),20);
	checkSuccess(cur->getColumnLength("TESTBIGINT"),20);
	checkSuccess(cur->getColumnLength(4),20);
	checkSuccess(cur->getColumnLength("TESTINT8"),20);
	checkSuccess(cur->getColumnLength(5),10);
	checkSuccess(cur->getColumnLength("TESTDECIMAL"),10);
	checkSuccess(cur->getColumnLength(6),16);
	checkSuccess(cur->getColumnLength("TESTMONEY"),16);
	checkSuccess(cur->getColumnLength(7),7);
	checkSuccess(cur->getColumnLength("TESTSMALLFLOAT"),7);
	checkSuccess(cur->getColumnLength(8),15);
	checkSuccess(cur->getColumnLength("TESTFLOAT"),15);
	checkSuccess(cur->getColumnLength(9),40);
	checkSuccess(cur->getColumnLength("TESTCHAR"),40);
	checkSuccess(cur->getColumnLength(10),40);
	checkSuccess(cur->getColumnLength("TESTNCHAR"),40);
	checkSuccess(cur->getColumnLength(11),40);
	checkSuccess(cur->getColumnLength("TESTVARCHAR"),40);
	checkSuccess(cur->getColumnLength(12),40);
	checkSuccess(cur->getColumnLength("TESTNVARCHAR"),40);
	checkSuccess(cur->getColumnLength(13),40);
	checkSuccess(cur->getColumnLength("TESTLVARCHAR"),40);
	checkSuccess(cur->getColumnLength(14),10);
	checkSuccess(cur->getColumnLength("TESTDATE"),10);
	checkSuccess(cur->getColumnLength(15),19);
	checkSuccess(cur->getColumnLength("TESTDATETIME"),19);
	checkSuccess(cur->getColumnLength(16),2147483647);
	checkSuccess(cur->getColumnLength("TESTTEXT"),2147483647);
	//checkSuccess(cur->getColumnLength(17),2157483647);
	//checkSuccess(cur->getColumnLength("TESTBYTE"),2157483647);
	stdoutput.printf("\n");

	stdoutput.printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest("TESTBOOLEAN"),1);
	checkSuccess(cur->getLongest(1),1);
	checkSuccess(cur->getLongest("TESTSMALLINT"),1);
	checkSuccess(cur->getLongest(2),1);
	checkSuccess(cur->getLongest("TESTINT"),1);
	checkSuccess(cur->getLongest(3),1);
	checkSuccess(cur->getLongest("TESTBIGINT"),1);
	checkSuccess(cur->getLongest(4),1);
	checkSuccess(cur->getLongest("TESTINT8"),1);
	checkSuccess(cur->getLongest(5),4);
	checkSuccess(cur->getLongest("TESTDECIMAL"),4);
	checkSuccess(cur->getLongest(6),4);
	checkSuccess(cur->getLongest("TESTMONEY"),4);
	checkSuccess(cur->getLongest(7),3);
	checkSuccess(cur->getLongest("TESTSMALLFLOAT"),3);
	checkSuccess(cur->getLongest(8),3);
	checkSuccess(cur->getLongest("TESTFLOAT"),3);
	checkSuccess(cur->getLongest(9),40);
	checkSuccess(cur->getLongest("TESTCHAR"),40);
	checkSuccess(cur->getLongest(10),40);
	checkSuccess(cur->getLongest("TESTNCHAR"),40);
	checkSuccess(cur->getLongest(11),12);
	checkSuccess(cur->getLongest("TESTVARCHAR"),12);
	checkSuccess(cur->getLongest(12),13);
	checkSuccess(cur->getLongest("TESTNVARCHAR"),13);
	checkSuccess(cur->getLongest(13),13);
	checkSuccess(cur->getLongest("TESTLVARCHAR"),13);
	checkSuccess(cur->getLongest(14),10);
	checkSuccess(cur->getLongest("TESTDATE"),10);
	checkSuccess(cur->getLongest(15),19);
	checkSuccess(cur->getLongest("TESTDATETIME"),19);
	checkSuccess(cur->getLongest(16),9);
	checkSuccess(cur->getLongest("TESTTEXT"),9);
	checkSuccess(cur->getLongest(17),9);
	checkSuccess(cur->getLongest("TESTBYTE"),9);
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
	checkSuccess(cur->getField(0,3),"1");
	checkSuccess(cur->getField(0,4),"1");
	checkSuccess(cur->getField(0,5),"1.10");
	checkSuccess(cur->getField(0,6),"1.10");
	checkSuccess(cur->getField(0,7),"1.1");
	checkSuccess(cur->getField(0,8),"1.1");
	checkSuccess(cur->getField(0,9),"testchar1                               ");
	checkSuccess(cur->getField(0,10),"testnchar1                              ");
	checkSuccess(cur->getField(0,11),"testvarchar1");
	checkSuccess(cur->getField(0,12),"testnvarchar1");
	checkSuccess(cur->getField(0,13),"testlvarchar1");
	checkSuccess(cur->getField(0,14),"2001-01-01");
	checkSuccess(cur->getField(0,15),"2001-01-01 01:00:00");
	checkSuccess(cur->getField(0,16),"testtext1");
	checkSuccess(cur->getField(0,17),"");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"1");
	checkSuccess(cur->getField(7,1),"8");
	checkSuccess(cur->getField(7,2),"8");
	checkSuccess(cur->getField(7,3),"8");
	checkSuccess(cur->getField(7,4),"8");
	checkSuccess(cur->getField(7,5),"8.80");
	checkSuccess(cur->getField(7,6),"8.80");
	checkSuccess(cur->getField(7,7),"8.8");
	checkSuccess(cur->getField(7,8),"8.8");
	checkSuccess(cur->getField(7,9),"testchar8                               ");
	checkSuccess(cur->getField(7,10),"testnchar8                              ");
	checkSuccess(cur->getField(7,11),"testvarchar8");
	checkSuccess(cur->getField(7,12),"testnvarchar8");
	checkSuccess(cur->getField(7,13),"testlvarchar8");
	checkSuccess(cur->getField(7,14),"2008-01-01");
	checkSuccess(cur->getField(7,15),"2008-01-01 08:00:00");
	checkSuccess(cur->getField(7,16),"testtext8");
	checkSuccess(cur->getField(7,17),"");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(0,1),1);
	checkSuccess(cur->getFieldLength(0,2),1);
	checkSuccess(cur->getFieldLength(0,3),1);
	checkSuccess(cur->getFieldLength(0,4),1);
	checkSuccess(cur->getFieldLength(0,5),4);
	checkSuccess(cur->getFieldLength(0,6),4);
	checkSuccess(cur->getFieldLength(0,7),3);
	checkSuccess(cur->getFieldLength(0,8),3);
	checkSuccess(cur->getFieldLength(0,9),40);
	checkSuccess(cur->getFieldLength(0,10),40);
	checkSuccess(cur->getFieldLength(0,11),12);
	checkSuccess(cur->getFieldLength(0,12),13);
	checkSuccess(cur->getFieldLength(0,14),10);
	checkSuccess(cur->getFieldLength(0,15),19);
	checkSuccess(cur->getFieldLength(0,16),9);
	checkSuccess(cur->getFieldLength(0,17),0);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(7,1),1);
	checkSuccess(cur->getFieldLength(7,2),1);
	checkSuccess(cur->getFieldLength(7,3),1);
	checkSuccess(cur->getFieldLength(7,4),1);
	checkSuccess(cur->getFieldLength(7,5),4);
	checkSuccess(cur->getFieldLength(7,6),4);
	checkSuccess(cur->getFieldLength(7,7),3);
	checkSuccess(cur->getFieldLength(7,8),3);
	checkSuccess(cur->getFieldLength(7,9),40);
	checkSuccess(cur->getFieldLength(7,10),40);
	checkSuccess(cur->getFieldLength(7,11),12);
	checkSuccess(cur->getFieldLength(7,12),13);
	checkSuccess(cur->getFieldLength(7,14),10);
	checkSuccess(cur->getFieldLength(7,15),19);
	checkSuccess(cur->getFieldLength(7,16),9);
	checkSuccess(cur->getFieldLength(7,17),0);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"TESTBOOLEAN"),"1");
	checkSuccess(cur->getField(0,"TESTSMALLINT"),"1");
	checkSuccess(cur->getField(0,"TESTINT"),"1");
	checkSuccess(cur->getField(0,"TESTBIGINT"),"1");
	checkSuccess(cur->getField(0,"TESTINT8"),"1");
	checkSuccess(cur->getField(0,"TESTDECIMAL"),"1.10");
	checkSuccess(cur->getField(0,"TESTMONEY"),"1.10");
	checkSuccess(cur->getField(0,"TESTSMALLFLOAT"),"1.1");
	checkSuccess(cur->getField(0,"TESTFLOAT"),"1.1");
	checkSuccess(cur->getField(0,"TESTCHAR"),"testchar1                               ");
	checkSuccess(cur->getField(0,"TESTNCHAR"),"testnchar1                              ");
	checkSuccess(cur->getField(0,"TESTVARCHAR"),"testvarchar1");
	checkSuccess(cur->getField(0,"TESTNVARCHAR"),"testnvarchar1");
	checkSuccess(cur->getField(0,"TESTLVARCHAR"),"testlvarchar1");
	checkSuccess(cur->getField(0,"TESTDATE"),"2001-01-01");
	checkSuccess(cur->getField(0,"TESTDATETIME"),"2001-01-01 01:00:00");
	checkSuccess(cur->getField(0,"TESTTEXT"),"testtext1");
	checkSuccess(cur->getField(0,"TESTBYTE"),"");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,"TESTBOOLEAN"),"1");
	checkSuccess(cur->getField(7,"TESTSMALLINT"),"8");
	checkSuccess(cur->getField(7,"TESTINT"),"8");
	checkSuccess(cur->getField(7,"TESTBIGINT"),"8");
	checkSuccess(cur->getField(7,"TESTINT8"),"8");
	checkSuccess(cur->getField(7,"TESTDECIMAL"),"8.80");
	checkSuccess(cur->getField(7,"TESTMONEY"),"8.80");
	checkSuccess(cur->getField(7,"TESTSMALLFLOAT"),"8.8");
	checkSuccess(cur->getField(7,"TESTFLOAT"),"8.8");
	checkSuccess(cur->getField(7,"TESTCHAR"),"testchar8                               ");
	checkSuccess(cur->getField(7,"TESTNCHAR"),"testnchar8                              ");
	checkSuccess(cur->getField(7,"TESTVARCHAR"),"testvarchar8");
	checkSuccess(cur->getField(7,"TESTNVARCHAR"),"testnvarchar8");
	checkSuccess(cur->getField(7,"TESTLVARCHAR"),"testlvarchar8");
	checkSuccess(cur->getField(7,"TESTDATE"),"2008-01-01");
	checkSuccess(cur->getField(7,"TESTDATETIME"),"2008-01-01 08:00:00");
	checkSuccess(cur->getField(7,"TESTTEXT"),"testtext8");
	checkSuccess(cur->getField(7,"TESTBYTE"),"");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"TESTBOOLEAN"),1);
	checkSuccess(cur->getFieldLength(0,"TESTSMALLINT"),1);
	checkSuccess(cur->getFieldLength(0,"TESTINT"),1);
	checkSuccess(cur->getFieldLength(0,"TESTBIGINT"),1);
	checkSuccess(cur->getFieldLength(0,"TESTINT8"),1);
	checkSuccess(cur->getFieldLength(0,"TESTDECIMAL"),4);
	checkSuccess(cur->getFieldLength(0,"TESTMONEY"),4);
	checkSuccess(cur->getFieldLength(0,"TESTSMALLFLOAT"),3);
	checkSuccess(cur->getFieldLength(0,"TESTFLOAT"),3);
	checkSuccess(cur->getFieldLength(0,"TESTCHAR"),40);
	checkSuccess(cur->getFieldLength(0,"TESTNCHAR"),40);
	checkSuccess(cur->getFieldLength(0,"TESTVARCHAR"),12);
	checkSuccess(cur->getFieldLength(0,"TESTNVARCHAR"),13);
	checkSuccess(cur->getFieldLength(0,"TESTLVARCHAR"),13);
	checkSuccess(cur->getFieldLength(0,"TESTDATE"),10);
	checkSuccess(cur->getFieldLength(0,"TESTDATETIME"),19);
	checkSuccess(cur->getFieldLength(0,"TESTTEXT"),9);
	checkSuccess(cur->getFieldLength(0,"TESTBYTE"),0);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,"TESTBOOLEAN"),1);
	checkSuccess(cur->getFieldLength(7,"TESTSMALLINT"),1);
	checkSuccess(cur->getFieldLength(7,"TESTINT"),1);
	checkSuccess(cur->getFieldLength(7,"TESTBIGINT"),1);
	checkSuccess(cur->getFieldLength(7,"TESTINT8"),1);
	checkSuccess(cur->getFieldLength(7,"TESTDECIMAL"),4);
	checkSuccess(cur->getFieldLength(7,"TESTMONEY"),4);
	checkSuccess(cur->getFieldLength(7,"TESTSMALLFLOAT"),3);
	checkSuccess(cur->getFieldLength(7,"TESTFLOAT"),3);
	checkSuccess(cur->getFieldLength(7,"TESTCHAR"),40);
	checkSuccess(cur->getFieldLength(7,"TESTNCHAR"),40);
	checkSuccess(cur->getFieldLength(7,"TESTVARCHAR"),12);
	checkSuccess(cur->getFieldLength(7,"TESTNVARCHAR"),13);
	checkSuccess(cur->getFieldLength(7,"TESTLVARCHAR"),13);
	checkSuccess(cur->getFieldLength(7,"TESTDATE"),10);
	checkSuccess(cur->getFieldLength(7,"TESTDATETIME"),19);
	checkSuccess(cur->getFieldLength(7,"TESTTEXT"),9);
	checkSuccess(cur->getFieldLength(7,"TESTBYTE"),0);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	checkSuccess(fields[1],"1");
	checkSuccess(fields[2],"1");
	checkSuccess(fields[3],"1");
	checkSuccess(fields[4],"1");
	checkSuccess(fields[5],"1.10");
	checkSuccess(fields[6],"1.10");
	checkSuccess(fields[7],"1.1");
	checkSuccess(fields[8],"1.1");
	checkSuccess(fields[9],"testchar1                               ");
	checkSuccess(fields[10],"testnchar1                              ");
	checkSuccess(fields[11],"testvarchar1");
	checkSuccess(fields[12],"testnvarchar1");
	checkSuccess(fields[13],"testlvarchar1");
	checkSuccess(fields[14],"2001-01-01");
	checkSuccess(fields[15],"2001-01-01 01:00:00");
	checkSuccess(fields[16],"testtext1");
	checkSuccess(fields[17],"");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	checkSuccess(fieldlens[1],1);
	checkSuccess(fieldlens[2],1);
	checkSuccess(fieldlens[3],1);
	checkSuccess(fieldlens[4],1);
	checkSuccess(fieldlens[5],4);
	checkSuccess(fieldlens[6],4);
	checkSuccess(fieldlens[7],3);
	checkSuccess(fieldlens[8],3);
	checkSuccess(fieldlens[9],40);
	checkSuccess(fieldlens[10],40);
	checkSuccess(fieldlens[11],12);
	checkSuccess(fieldlens[12],13);
	checkSuccess(fieldlens[14],10);
	checkSuccess(fieldlens[15],19);
	checkSuccess(fieldlens[16],9);
	checkSuccess(fieldlens[17],0);
	stdoutput.printf("\n");

	stdoutput.printf("INDIVIDUAL SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),'$(var2)','$(var3)' from sysmaster:sysdual");
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
	cur->prepareQuery("select '$(var1)','$(var2)','$(var3)' from sysmaster:sysdual");
	cur->substitutions(subvars,subvalstrings);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"hi");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"bye");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3) from sysmaster:sysdual");
	cur->substitutions(subvars,subvallongs);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"2");
	checkSuccess(cur->getField(0,2),"3");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3) from sysmaster:sysdual");
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
	checkSuccess(cur->getField(0,(uint32_t)1),"1");
	checkSuccess(cur->getField(1,(uint32_t)1),"2");
	checkSuccess(cur->getField(2,(uint32_t)1),"3");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),2);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),4);
	checkSuccess(cur->getField(6,(uint32_t)1),"7");
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)1),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("DONT GET COLUMN INFO: \n");
	cur->dontGetColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	checkSuccess(cur->getColumnName((uint32_t)1),NULL);
	checkSuccess(cur->getColumnLength((uint32_t)1),0);
	checkSuccess(cur->getColumnType((uint32_t)1),NULL);
	cur->getColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	checkSuccess(cur->getColumnName((uint32_t)1),"testsmallint");
	checkSuccess(cur->getColumnLength((uint32_t)1),5);
	checkSuccess(cur->getColumnType((uint32_t)1),"SMALLINT");
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)1),"1");
	checkSuccess(cur->getField(1,(uint32_t)1),"2");
	checkSuccess(cur->getField(2,(uint32_t)1),"3");
	checkSuccess(cur->getField(3,(uint32_t)1),"4");
	checkSuccess(cur->getField(4,(uint32_t)1),"5");
	checkSuccess(cur->getField(5,(uint32_t)1),"6");
	checkSuccess(cur->getField(6,(uint32_t)1),"7");
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)1),"1");
	checkSuccess(cur->getField(1,(uint32_t)1),"2");
	checkSuccess(cur->getField(2,(uint32_t)1),"3");
	checkSuccess(cur->getField(3,(uint32_t)1),"4");
	checkSuccess(cur->getField(4,(uint32_t)1),"5");
	checkSuccess(cur->getField(5,(uint32_t)1),"6");
	checkSuccess(cur->getField(6,(uint32_t)1),"7");
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)1),"1");
	checkSuccess(cur->getField(1,(uint32_t)1),"2");
	checkSuccess(cur->getField(2,(uint32_t)1),"3");
	checkSuccess(cur->getField(3,(uint32_t)1),"4");
	checkSuccess(cur->getField(4,(uint32_t)1),"5");
	checkSuccess(cur->getField(5,(uint32_t)1),"6");
	checkSuccess(cur->getField(6,(uint32_t)1),"7");
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED RESULT SET: \n");
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	checkSuccess(cur->getField(2,(uint32_t)1),"3");
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
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)1),NULL);
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
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(cur->colCount(),18);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(cur->getColumnName(0),"testboolean");
	checkSuccess(cur->getColumnName(1),"testsmallint");
	checkSuccess(cur->getColumnName(2),"testint");
	checkSuccess(cur->getColumnName(3),"testbigint");
	checkSuccess(cur->getColumnName(4),"testint8");
	checkSuccess(cur->getColumnName(5),"testdecimal");
	checkSuccess(cur->getColumnName(6),"testmoney");
	checkSuccess(cur->getColumnName(7),"testsmallfloat");
	checkSuccess(cur->getColumnName(8),"testfloat");
	checkSuccess(cur->getColumnName(9),"testchar");
	checkSuccess(cur->getColumnName(10),"testnchar");
	checkSuccess(cur->getColumnName(11),"testvarchar");
	checkSuccess(cur->getColumnName(12),"testnvarchar");
	checkSuccess(cur->getColumnName(13),"testlvarchar");
	checkSuccess(cur->getColumnName(14),"testdate");
	checkSuccess(cur->getColumnName(15),"testdatetime");
	checkSuccess(cur->getColumnName(16),"testtext");
	checkSuccess(cur->getColumnName(17),"testbyte");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testboolean");
	checkSuccess(cols[1],"testsmallint");
	checkSuccess(cols[2],"testint");
	checkSuccess(cols[3],"testbigint");
	checkSuccess(cols[4],"testint8");
	checkSuccess(cols[5],"testdecimal");
	checkSuccess(cols[6],"testmoney");
	checkSuccess(cols[7],"testsmallfloat");
	checkSuccess(cols[8],"testfloat");
	checkSuccess(cols[9],"testchar");
	checkSuccess(cols[10],"testnchar");
	checkSuccess(cols[11],"testvarchar");
	checkSuccess(cols[12],"testnvarchar");
	checkSuccess(cols[13],"testlvarchar");
	checkSuccess(cols[14],"testdate");
	checkSuccess(cols[15],"testdatetime");
	checkSuccess(cols[16],"testtext");
	checkSuccess(cols[17],"testbyte");
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
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	checkSuccess(cur->getField(8,(uint32_t)1),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("FROM ONE CACHE FILE TO ANOTHER: \n");
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	checkSuccess(cur->getField(8,(uint32_t)1),NULL);
	stdoutput.printf("\n");

	stdoutput.printf("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	checkSuccess(cur->getField(8,(uint32_t)1),NULL);
	cur->setResultSetBufferSize(0);
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testsmallint"),1);
	checkSuccess(cur->getField(2,(uint32_t)1),"3");
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
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)1),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	cur->cacheOff();
	stdoutput.printf("\n");
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	checkSuccess(cur->getField(8,(uint32_t)1),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getField(4,(uint32_t)1),"5");
	checkSuccess(cur->getField(5,(uint32_t)1),"6");
	checkSuccess(cur->getField(6,(uint32_t)1),"7");
	checkSuccess(cur->getField(7,(uint32_t)1),"8");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeResultSet(id),1);
	checkSuccess(cur->getField(4,(uint32_t)1),NULL);
	checkSuccess(cur->getField(5,(uint32_t)1),NULL);
	checkSuccess(cur->getField(6,(uint32_t)1),NULL);
	checkSuccess(cur->getField(7,(uint32_t)1),NULL);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");
	stdoutput.printf("\n");

	// clobs/blobs in particular
	/*stdoutput.printf("CLOB/BLOB: \n");
	checkSuccess(cur->sendQuery("create table testtable (testclob clob)"),1);
	cur->prepareQuery("insert into testtable values (?)");
	cur->inputBindClob("1","hello",5);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("drop table testtable");
	stdoutput.printf("\n");*/

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
}
