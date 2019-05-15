// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
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
			stdoutput.printf("failure: %s",cur->errorMessage());
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
		stdoutput.printf("failure: %s",cur->errorMessage());
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%d\"!=\"%d\"\n",value,success);
		stdoutput.printf("failure: %s",cur->errorMessage());
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(double value, double success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%f\"!=\"%f\"\n",value,success);
		stdoutput.printf("failure: %s",cur->errorMessage());
		delete cur;
		delete con;
		process::exit(1);
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
	char		*socket;
	uint16_t	id;
	char		*filename;
	uint32_t	*fieldlens;

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);

	stdoutput.printf("IDENTIFY: \n");
	checkSuccess(con->identify(),"postgresql");
	stdoutput.printf("\n");

	// ping
	stdoutput.printf("PING: \n");
	checkSuccess(con->ping(),1);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	stdoutput.printf("CREATE TEMPTABLE: \n");
	checkSuccess(cur->sendQuery("create table testtable (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp, testtext text, testbytea bytea)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("BEGIN TRANSCTION: \n");
	checkSuccess(cur->sendQuery("begin"),1);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL,'testtext1','testbytea1')"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',NULL,'testtext2','testbytea2')"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (3,3.3,3.3,3,'testchar3','testvarchar3','01/01/2003','03:00:00',NULL,'testtext3','testbytea3')"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (4,4.4,4.4,4,'testchar4','testvarchar4','01/01/2004','04:00:00',NULL,'testtext4','testbytea4')"),1);
	stdoutput.printf("\n");

	stdoutput.printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY POSITION: \n");
	cur->prepareQuery("insert into testtable values ($1,$2,$3,$4,$5,$6,$7,$8,NULL,$9,$10)");
	checkSuccess(cur->countBindVariables(),10);
	cur->inputBind("1",5);
	cur->inputBind("2",5.5,4,2);
	cur->inputBind("3",5.5,4,2);
	cur->inputBind("4",5);
	cur->inputBind("5","testchar5");
	cur->inputBind("6","testvarchar5");
	cur->inputBind("7","01/01/2005");
	cur->inputBind("8","05:00:00");
	cur->inputBindClob("9","testtext5",9);
	cur->inputBindBlob("10","testbytea5",10);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",6);
	cur->inputBind("2",6.6,4,2);
	cur->inputBind("3",6.6,4,2);
	cur->inputBind("4",6);
	cur->inputBind("5","testchar6");
	cur->inputBind("6","testvarchar6");
	cur->inputBind("7","01/01/2006");
	cur->inputBind("8","06:00:00");
	cur->inputBindClob("9","testtext6",9);
	cur->inputBindBlob("10","testbytea6",10);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",7);
	cur->inputBind("2",7.7,4,2);
	cur->inputBind("3",7.7,4,2);
	cur->inputBind("4",7);
	cur->inputBind("5","testchar7");
	cur->inputBind("6","testvarchar7");
	cur->inputBind("7","01/01/2007");
	cur->inputBind("8","07:00:00");
	cur->inputBindClob("9","testtext7",9);
	cur->inputBindBlob("10","testbytea8",10);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY POSITION WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("1",8);
	cur->inputBind("2",8.8,4,2);
	cur->inputBind("3",8.8,4,2);
	cur->inputBind("4",8);
	cur->inputBind("5","testchar8");
	cur->inputBind("6","testvarchar8");
	cur->inputBind("7","01/01/2008");
	cur->inputBind("8","08:00:00");
	cur->inputBindClob("9","testtext8",9);
	cur->inputBindClob("10","testbytea8",10);
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),11);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES: \n");
	checkSuccess(cur->getColumnName(0),"testint");
	checkSuccess(cur->getColumnName(1),"testfloat");
	checkSuccess(cur->getColumnName(2),"testreal");
	checkSuccess(cur->getColumnName(3),"testsmallint");
	checkSuccess(cur->getColumnName(4),"testchar");
	checkSuccess(cur->getColumnName(5),"testvarchar");
	checkSuccess(cur->getColumnName(6),"testdate");
	checkSuccess(cur->getColumnName(7),"testtime");
	checkSuccess(cur->getColumnName(8),"testtimestamp");
	checkSuccess(cur->getColumnName(9),"testtext");
	checkSuccess(cur->getColumnName(10),"testbytea");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testint");
	checkSuccess(cols[1],"testfloat");
	checkSuccess(cols[2],"testreal");
	checkSuccess(cols[3],"testsmallint");
	checkSuccess(cols[4],"testchar");
	checkSuccess(cols[5],"testvarchar");
	checkSuccess(cols[6],"testdate");
	checkSuccess(cols[7],"testtime");
	checkSuccess(cols[8],"testtimestamp");
	checkSuccess(cols[9],"testtext");
	checkSuccess(cols[10],"testbytea");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType((uint32_t)0),"int4");
	checkSuccess(cur->getColumnType("testint"),"int4");
	checkSuccess(cur->getColumnType(1),"float8");
	checkSuccess(cur->getColumnType("testfloat"),"float8");
	checkSuccess(cur->getColumnType(2),"float4");
	checkSuccess(cur->getColumnType("testreal"),"float4");
	checkSuccess(cur->getColumnType(3),"int2");
	checkSuccess(cur->getColumnType("testsmallint"),"int2");
	checkSuccess(cur->getColumnType(4),"bpchar");
	checkSuccess(cur->getColumnType("testchar"),"bpchar");
	checkSuccess(cur->getColumnType(5),"varchar");
	checkSuccess(cur->getColumnType("testvarchar"),"varchar");
	checkSuccess(cur->getColumnType(6),"date");
	checkSuccess(cur->getColumnType("testdate"),"date");
	checkSuccess(cur->getColumnType(7),"time");
	checkSuccess(cur->getColumnType("testtime"),"time");
	checkSuccess(cur->getColumnType(8),"timestamp");
	checkSuccess(cur->getColumnType("testtimestamp"),"timestamp");
	checkSuccess(cur->getColumnType(9),"text");
	checkSuccess(cur->getColumnType("testtext"),"text");
	checkSuccess(cur->getColumnType(10),"bytea");
	checkSuccess(cur->getColumnType("testbytea"),"bytea");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),4);
	checkSuccess(cur->getColumnLength("testint"),4);
	checkSuccess(cur->getColumnLength(1),8);
	checkSuccess(cur->getColumnLength("testfloat"),8);
	checkSuccess(cur->getColumnLength(2),4);
	checkSuccess(cur->getColumnLength("testreal"),4);
	checkSuccess(cur->getColumnLength(3),2);
	checkSuccess(cur->getColumnLength("testsmallint"),2);
	checkSuccess(cur->getColumnLength(4),44);
	checkSuccess(cur->getColumnLength("testchar"),44);
	checkSuccess(cur->getColumnLength(5),44);
	checkSuccess(cur->getColumnLength("testvarchar"),44);
	checkSuccess(cur->getColumnLength(6),4);
	checkSuccess(cur->getColumnLength("testdate"),4);
	checkSuccess(cur->getColumnLength(7),8);
	checkSuccess(cur->getColumnLength("testtime"),8);
	checkSuccess(cur->getColumnLength(8),8);
	checkSuccess(cur->getColumnLength("testtimestamp"),8);
	checkSuccess(cur->getColumnLength(9),0);
	checkSuccess(cur->getColumnLength("testtext"),0);
	checkSuccess(cur->getColumnLength(10),0);
	checkSuccess(cur->getColumnLength("testbytea"),0);
	stdoutput.printf("\n");

	stdoutput.printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest("testint"),1);
	checkSuccess(cur->getLongest(1),3);
	checkSuccess(cur->getLongest("testfloat"),3);
	checkSuccess(cur->getLongest(2),3);
	checkSuccess(cur->getLongest("testreal"),3);
	checkSuccess(cur->getLongest(3),1);
	checkSuccess(cur->getLongest("testsmallint"),1);
	checkSuccess(cur->getLongest(4),40);
	checkSuccess(cur->getLongest("testchar"),40);
	checkSuccess(cur->getLongest(5),12);
	checkSuccess(cur->getLongest("testvarchar"),12);
	checkSuccess(cur->getLongest(6),10);
	checkSuccess(cur->getLongest("testdate"),10);
	checkSuccess(cur->getLongest(7),8);
	checkSuccess(cur->getLongest("testtime"),8);
	checkSuccess(cur->getLongest(9),9);
	checkSuccess(cur->getLongest("testtext"),9);
	checkSuccess(cur->getLongest(10),22);
	checkSuccess(cur->getLongest("testbytea"),22);
	stdoutput.printf("\n");

	stdoutput.printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	/*stdoutput.printf("TOTAL ROWS: \n");
	checkSuccess(cur->totalRows(),8);
	stdoutput.printf("\n");*/

	stdoutput.printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	stdoutput.printf("\n");

	stdoutput.printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"1.1");
	checkSuccess(cur->getField(0,2),"1.1");
	checkSuccess(cur->getField(0,3),"1");
	checkSuccess(cur->getField(0,4),"testchar1                               ");
	checkSuccess(cur->getField(0,5),"testvarchar1");
	checkSuccess(cur->getField(0,6),"2001-01-01");
	checkSuccess(cur->getField(0,7),"01:00:00");
	checkSuccess(cur->getField(0,9),"testtext1");
	checkSuccess(cur->getField(0,10),"\\x74657374627974656131");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(7,1),"8.8");
	checkSuccess(cur->getField(7,2),"8.8");
	checkSuccess(cur->getField(7,3),"8");
	checkSuccess(cur->getField(7,4),"testchar8                               ");
	checkSuccess(cur->getField(7,5),"testvarchar8");
	checkSuccess(cur->getField(7,6),"2008-01-01");
	checkSuccess(cur->getField(7,7),"08:00:00");
	checkSuccess(cur->getField(7,9),"testtext8");
	checkSuccess(cur->getField(7,10),"\\x74657374627974656138");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(0,1),3);
	checkSuccess(cur->getFieldLength(0,2),3);
	checkSuccess(cur->getFieldLength(0,3),1);
	checkSuccess(cur->getFieldLength(0,4),40);
	checkSuccess(cur->getFieldLength(0,5),12);
	checkSuccess(cur->getFieldLength(0,6),10);
	checkSuccess(cur->getFieldLength(0,7),8);
	checkSuccess(cur->getFieldLength(0,9),9);
	checkSuccess(cur->getFieldLength(0,10),22);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(7,1),3);
	checkSuccess(cur->getFieldLength(7,2),3);
	checkSuccess(cur->getFieldLength(7,3),1);
	checkSuccess(cur->getFieldLength(7,4),40);
	checkSuccess(cur->getFieldLength(7,5),12);
	checkSuccess(cur->getFieldLength(7,6),10);
	checkSuccess(cur->getFieldLength(7,7),8);
	checkSuccess(cur->getFieldLength(7,9),9);
	checkSuccess(cur->getFieldLength(7,10),22);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"testint"),"1");
	checkSuccess(cur->getField(0,"testfloat"),"1.1");
	checkSuccess(cur->getField(0,"testreal"),"1.1");
	checkSuccess(cur->getField(0,"testsmallint"),"1");
	checkSuccess(cur->getField(0,"testchar"),"testchar1                               ");
	checkSuccess(cur->getField(0,"testvarchar"),"testvarchar1");
	checkSuccess(cur->getField(0,"testdate"),"2001-01-01");
	checkSuccess(cur->getField(0,"testtime"),"01:00:00");
	checkSuccess(cur->getField(0,"testtext"),"testtext1");
	checkSuccess(cur->getField(0,"testbytea"),"\\x74657374627974656131");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,"testint"),"8");
	checkSuccess(cur->getField(7,"testfloat"),"8.8");
	checkSuccess(cur->getField(7,"testreal"),"8.8");
	checkSuccess(cur->getField(7,"testsmallint"),"8");
	checkSuccess(cur->getField(7,"testchar"),"testchar8                               ");
	checkSuccess(cur->getField(7,"testvarchar"),"testvarchar8");
	checkSuccess(cur->getField(7,"testdate"),"2008-01-01");
	checkSuccess(cur->getField(7,"testtime"),"08:00:00");
	checkSuccess(cur->getField(7,"testtext"),"testtext8");
	checkSuccess(cur->getField(7,"testbytea"),"\\x74657374627974656138");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"testint"),1);
	checkSuccess(cur->getFieldLength(0,"testfloat"),3);
	checkSuccess(cur->getFieldLength(0,"testreal"),3);
	checkSuccess(cur->getFieldLength(0,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(0,"testchar"),40);
	checkSuccess(cur->getFieldLength(0,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(0,"testdate"),10);
	checkSuccess(cur->getFieldLength(0,"testtime"),8);
	checkSuccess(cur->getFieldLength(0,"testtext"),9);
	checkSuccess(cur->getFieldLength(0,"testbytea"),22);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,"testint"),1);
	checkSuccess(cur->getFieldLength(7,"testfloat"),3);
	checkSuccess(cur->getFieldLength(7,"testreal"),3);
	checkSuccess(cur->getFieldLength(7,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(7,"testchar"),40);
	checkSuccess(cur->getFieldLength(7,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(7,"testdate"),10);
	checkSuccess(cur->getFieldLength(7,"testtime"),8);
	checkSuccess(cur->getFieldLength(7,"testtext"),9);
	checkSuccess(cur->getFieldLength(7,"testbytea"),22);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	checkSuccess(fields[1],"1.1");
	checkSuccess(fields[2],"1.1");
	checkSuccess(fields[3],"1");
	checkSuccess(fields[4],"testchar1                               ");
	checkSuccess(fields[5],"testvarchar1");
	checkSuccess(fields[6],"2001-01-01");
	checkSuccess(fields[7],"01:00:00");
	checkSuccess(fields[9],"testtext1");
	checkSuccess(fields[10],"\\x74657374627974656131");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	checkSuccess(fieldlens[1],3);
	checkSuccess(fieldlens[2],3);
	checkSuccess(fieldlens[3],1);
	checkSuccess(fieldlens[4],40);
	checkSuccess(fieldlens[5],12);
	checkSuccess(fieldlens[6],10);
	checkSuccess(fieldlens[7],8);
	checkSuccess(fieldlens[9],9);
	checkSuccess(fieldlens[10],22);
	stdoutput.printf("\n");

	stdoutput.printf("INDIVIDUAL SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),'$(var2)',$(var3)");
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
	cur->prepareQuery("select $(var1),$(var2),$(var3)");
	cur->substitutions(subvars,subvallongs);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");
	
	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"2");
	checkSuccess(cur->getField(0,2),"3");
	stdoutput.printf("\n");
	
	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select '$(var1)','$(var2)','$(var3)'");
	cur->substitutions(subvars,subvalstrings);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"hi");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"bye");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3)");
	cur->substitutions(subvars,subvaldoubles,precs,scales);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"10.55");
	checkSuccess(cur->getField(0,1),"10.556");
	checkSuccess(cur->getField(0,2),"10.5556");
	stdoutput.printf("\n");

	stdoutput.printf("NULLS as Nulls: \n");
	cur->getNullsAsNulls();
	checkSuccess(cur->sendQuery("select NULL,1,NULL"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),NULL);
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),NULL);
	cur->getNullsAsEmptyStrings();
	checkSuccess(cur->sendQuery("select NULL,1,NULL"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"");
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),"");
	cur->getNullsAsNulls();
	stdoutput.printf("\n");

	stdoutput.printf("RESULT SET BUFFER SIZE: \n");
	checkSuccess(cur->getResultSetBufferSize(),0);
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getColumnName((uint32_t)0),NULL);
	checkSuccess(cur->getColumnLength((uint32_t)0),0);
	checkSuccess(cur->getColumnType((uint32_t)0),NULL);
	cur->getColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getColumnName((uint32_t)0),"testint");
	checkSuccess(cur->getColumnLength((uint32_t)0),4);
	checkSuccess(cur->getColumnType((uint32_t)0),"int4");
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(cur->colCount(),11);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(cur->getColumnName((uint32_t)0),"testint");
	checkSuccess(cur->getColumnName(1),"testfloat");
	checkSuccess(cur->getColumnName(2),"testreal");
	checkSuccess(cur->getColumnName(3),"testsmallint");
	checkSuccess(cur->getColumnName(4),"testchar");
	checkSuccess(cur->getColumnName(5),"testvarchar");
	checkSuccess(cur->getColumnName(6),"testdate");
	checkSuccess(cur->getColumnName(7),"testtime");
	checkSuccess(cur->getColumnName(8),"testtimestamp");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testint");
	checkSuccess(cols[1],"testfloat");
	checkSuccess(cols[2],"testreal");
	checkSuccess(cols[3],"testsmallint");
	checkSuccess(cols[4],"testchar");
	checkSuccess(cols[5],"testvarchar");
	checkSuccess(cols[6],"testdate");
	checkSuccess(cols[7],"testtime");
	checkSuccess(cols[8],"testtimestamp");
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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

	stdoutput.printf("COMMIT AND ROLLBACK: \n");
	secondcon=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	secondcur=new sqlrcursor(secondcon);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"0");
	checkSuccess(con->commit(),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"8");
	//checkSuccess(con->autoCommitOn(),1);
	checkSuccess(cur->sendQuery("insert into testtable values (10,10.1,10.1,10,'testchar10','testvarchar10','01/01/2010','10:00:00',NULL)"),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"9");
	//checkSuccess(con->autoCommitOff(),1);
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

	// temporary tables
	stdoutput.printf("TEMPORARY TABLES: \n");
	cur->sendQuery("drop table temptable\n");
	cur->sendQuery("-- this should be skipped\n"
			"-- so should this\n"
			"-- and the whitespace below too\n"
			"        				\n\n"
			"create temporary table temptable (col1 int)");
	checkSuccess(cur->sendQuery("insert into temptable values (1)"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptable"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	con->endSession();
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select count(*) from temptable"),0);
	cur->sendQuery("drop table temptable\n");
	stdoutput.printf("\n");

	// stored procedures
	stdoutput.printf("STORED PROCEDURES: \n");
	// return no values
	cur->sendQuery("drop function testfunc(int,float,char(20))");
	checkSuccess(cur->sendQuery("create function testfunc(int,float,char(20)) returns void as ' declare in1 int; in2 float; in3 char(20); begin in1:=$1; in2:=$2; in3:=$3; return; end;' language plpgsql"),1);
	cur->prepareQuery("select testfunc($1,$2,$3)");
	cur->inputBind("1",1);
	cur->inputBind("2",1.1,4,2);
	cur->inputBind("3","hello");
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("drop function testfunc(int,float,char(20))");
	stdoutput.printf("\n");
	// return single value
	cur->sendQuery("drop function testfunc(int,float,char(20))");
	checkSuccess(cur->sendQuery("create function testfunc(int,float,char(20)) returns int as ' begin return $1; end;' language plpgsql"),1);
	cur->prepareQuery("select * from testfunc($1,$2,$3)");
	cur->inputBind("1",1);
	cur->inputBind("2",1.1,4,2);
	cur->inputBind("3","hello");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	cur->sendQuery("drop function testfunc(int,float,char(20))");
	stdoutput.printf("\n");
	// return multiple values
	cur->sendQuery("drop function testfunc(int,char(20))");
	checkSuccess(cur->sendQuery("create function testfunc(int,float,char(20)) returns record as ' declare output record; begin select $1,$2,$3 into output; return output; end;' language plpgsql"),1);
	cur->prepareQuery("select * from testfunc($1,$2,$3) as (col1 int, col2 float, col3 bpchar)");
	cur->inputBind("1",1);
	cur->inputBind("2",1.1,4,2);
	cur->inputBind("3","hello");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(charstring::toFloatC(cur->getField(0,1)),1.1);
	checkSuccess(cur->getField(0,2),"hello");
	cur->sendQuery("drop function testfunc(int,float,char(20))");
	stdoutput.printf("\n");
	// return result set
	cur->sendQuery("drop function testfunc()");
	checkSuccess(cur->sendQuery("create function testfunc() returns setof record as ' declare output record; begin for output in select * from testtable loop return next output; end loop; return; end;' language plpgsql"),1);
	checkSuccess(cur->sendQuery("select * from testfunc() as (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp, testtext text, testbytea bytea)"),1);
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	cur->sendQuery("drop function testfunc()");
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	// invalid queries...
	stdoutput.printf("INVALID QUERIES: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),0);
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
