// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <rudiments/process.h>
#include <rudiments/snooze.h>
#include <rudiments/stdio.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success \n");
			return;
		} else {
			stdoutput.printf("failure %s!=%s\n",value,success);
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success \n");
	} else {
		stdoutput.printf("failure %s!=%s\n",value,success);
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(const char *value, const char *success, size_t length) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success \n");
			return;
		} else {
			stdoutput.printf("failure %s!=%s\n",value,success);
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success,length)) {
		stdoutput.printf("success \n");
	} else {
		stdoutput.printf("failure %s!=%s\n",value,success);
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success \n");
	} else {
		stdoutput.printf("failure %d!=%d\n",value,success);
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(double value, double success) {

	if (value==success) {
		stdoutput.printf("success \n");
	} else {
		stdoutput.printf("failure %f!=%f\n",value,success);
		delete cur;
		delete con;
		process::exit(1);
	}
}

int	main(int argc, char **argv) {

	const char * const *cols;
	const char * const *fields;
	uint32_t	*fieldlens;

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);

	stdoutput.printf("IDENTIFY: \n");
	checkSuccess(con->identify(),"router");
	stdoutput.printf("\n");

	// ping
	stdoutput.printf("PING: \n");
	checkSuccess(con->ping(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FILTERED-OUT QUERIES: \n");
	checkSuccess(cur->sendQuery("create table junktable (col1 int)"),0);
	checkSuccess(cur->sendQuery("insert into junktable values (1)"),0);
	checkSuccess(cur->sendQuery("update junktable set col1=2"),0);
	checkSuccess(cur->sendQuery("delete from junktable"),0);
	checkSuccess(cur->sendQuery("drop table junktable (col1 int)"),0);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable1");
	cur->sendQuery("drop table testtable2");

	stdoutput.printf("CREATE TESTTABLES: \n");
	checkSuccess(cur->sendQuery("create table testtable1 (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
	checkSuccess(cur->sendQuery("create table testtable2 (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("BEGIN TRANSCTION: \n");
	checkSuccess(cur->sendQuery("begin"),1);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable1 values (1,1.1,1.1,1,'testchar1','testvarchar1','2001-01-01','01:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable1 values (2,2.2,2.2,2,'testchar2','testvarchar2','2002-01-01','02:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable1 values (3,3.3,3.3,3,'testchar3','testvarchar3','2003-01-01','03:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable1 values (4,4.4,4.4,4,'testchar4','testvarchar4','2004-01-01','04:00:00',NULL)"),1);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable2 values (1,1.1,1.1,1,'testchar1','testvarchar1','2001-01-01','01:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable2 values (2,2.2,2.2,2,'testchar2','testvarchar2','2002-01-01','02:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable2 values (3,3.3,3.3,3,'testchar3','testvarchar3','2003-01-01','03:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable2 values (4,4.4,4.4,4,'testchar4','testvarchar4','2004-01-01','04:00:00',NULL)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY NAME: \n");
	cur->prepareQuery("insert into testtable1 values (?,?,?,?,?,?,?,?,NULL)");
	checkSuccess(cur->countBindVariables(),8);
	cur->inputBind("1",5);
	cur->inputBind("2",5.5,4,2);
	cur->inputBind("3",5.5,4,2);
	cur->inputBind("4",5);
	cur->inputBind("5","testchar5");
	cur->inputBind("6","testvarchar5");
	cur->inputBind("7","2005-01-01");
	cur->inputBind("8","05:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",6);
	cur->inputBind("2",6.6,4,2);
	cur->inputBind("3",6.6,4,2);
	cur->inputBind("4",6);
	cur->inputBind("5","testchar6");
	cur->inputBind("6","testvarchar6");
	cur->inputBind("7","2006-01-01");
	cur->inputBind("8","06:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",7);
	cur->inputBind("2",7.7,4,2);
	cur->inputBind("3",7.7,4,2);
	cur->inputBind("4",7);
	cur->inputBind("5","testchar7");
	cur->inputBind("6","testvarchar7");
	cur->inputBind("7","2007-01-01");
	cur->inputBind("8","07:00:00");
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("1",8);
	cur->inputBind("2",8.8,4,2);
	cur->inputBind("3",8.8,4,2);
	cur->inputBind("4",8);
	cur->inputBind("5","testchar8");
	cur->inputBind("6","testvarchar8");
	cur->inputBind("7","2008-01-01");
	cur->inputBind("8","08:00:00");
	cur->inputBind("9","junkvalue");
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY NAME: \n");
	cur->prepareQuery("insert into testtable2 values (?,?,?,?,?,?,?,?,NULL)");
	checkSuccess(cur->countBindVariables(),8);
	cur->inputBind("1",5);
	cur->inputBind("2",5.5,4,2);
	cur->inputBind("3",5.5,4,2);
	cur->inputBind("4",5);
	cur->inputBind("5","testchar5");
	cur->inputBind("6","testvarchar5");
	cur->inputBind("7","2005-01-01");
	cur->inputBind("8","05:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",6);
	cur->inputBind("2",6.6,4,2);
	cur->inputBind("3",6.6,4,2);
	cur->inputBind("4",6);
	cur->inputBind("5","testchar6");
	cur->inputBind("6","testvarchar6");
	cur->inputBind("7","2006-01-01");
	cur->inputBind("8","06:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",7);
	cur->inputBind("2",7.7,4,2);
	cur->inputBind("3",7.7,4,2);
	cur->inputBind("4",7);
	cur->inputBind("5","testchar7");
	cur->inputBind("6","testvarchar7");
	cur->inputBind("7","2007-01-01");
	cur->inputBind("8","07:00:00");
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("1",8);
	cur->inputBind("2",8.8,4,2);
	cur->inputBind("3",8.8,4,2);
	cur->inputBind("4",8);
	cur->inputBind("5","testchar8");
	cur->inputBind("6","testvarchar8");
	cur->inputBind("7","2008-01-01");
	cur->inputBind("8","08:00:00");
	cur->inputBind("9","junkvalue");
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	checkSuccess(con->commit(),1);
	snooze::macrosnooze(2,0);

	stdoutput.printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable1 order by testint"),1);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),9);
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

	stdoutput.printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType((uint32_t)0),"INT");
	checkSuccess(cur->getColumnType("testint"),"INT");
	checkSuccess(cur->getColumnType(1),"FLOAT");
	checkSuccess(cur->getColumnType("testfloat"),"FLOAT");
	checkSuccess(cur->getColumnType(2),"REAL");
	checkSuccess(cur->getColumnType("testreal"),"REAL");
	checkSuccess(cur->getColumnType(3),"SMALLINT");
	checkSuccess(cur->getColumnType("testsmallint"),"SMALLINT");
	checkSuccess(cur->getColumnType(4),"STRING");
	checkSuccess(cur->getColumnType("testchar"),"STRING");
	checkSuccess(cur->getColumnType(5),"VARSTRING");
	checkSuccess(cur->getColumnType("testvarchar"),"VARSTRING");
	checkSuccess(cur->getColumnType(6),"DATE");
	checkSuccess(cur->getColumnType("testdate"),"DATE");
	checkSuccess(cur->getColumnType(7),"TIME");
	checkSuccess(cur->getColumnType("testtime"),"TIME");
	checkSuccess(cur->getColumnType(8),"TIMESTAMP");
	checkSuccess(cur->getColumnType("testtimestamp"),"TIMESTAMP");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),4);
	checkSuccess(cur->getColumnLength("testint"),4);
	checkSuccess(cur->getColumnLength(1),4);
	checkSuccess(cur->getColumnLength("testfloat"),4);
	checkSuccess(cur->getColumnLength(2),8);
	checkSuccess(cur->getColumnLength("testreal"),8);
	checkSuccess(cur->getColumnLength(3),2);
	checkSuccess(cur->getColumnLength("testsmallint"),2);
	// not reliable... some mysql client return the number of bytes
	// rather than chars, which can be 3 times the number of chars if
	// the db supports multibyte characters
	//checkSuccess(cur->getColumnLength(4),40);
	//checkSuccess(cur->getColumnLength("testchar"),40);
	//checkSuccess(cur->getColumnLength(5),41);
	//checkSuccess(cur->getColumnLength("testvarchar"),41);
	checkSuccess(cur->getColumnLength(6),3);
	checkSuccess(cur->getColumnLength("testdate"),3);
	checkSuccess(cur->getColumnLength(7),3);
	checkSuccess(cur->getColumnLength("testtime"),3);
	checkSuccess(cur->getColumnLength(8),4);
	checkSuccess(cur->getColumnLength("testtimestamp"),4);
	stdoutput.printf("\n");

	/*stdoutput.printf("LONGEST COLUMN: \n");
	// FIXME: weird, this returns 0 but the next one works
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest("testint"),1);
	checkSuccess(cur->getLongest(1),3);
	checkSuccess(cur->getLongest("testfloat"),3);
	checkSuccess(cur->getLongest(2),3);
	checkSuccess(cur->getLongest("testreal"),3);
	checkSuccess(cur->getLongest(3),1);
	checkSuccess(cur->getLongest("testsmallint"),1);
	checkSuccess(cur->getLongest(4),9);
	checkSuccess(cur->getLongest("testchar"),9);
	checkSuccess(cur->getLongest(5),12);
	checkSuccess(cur->getLongest("testvarchar"),12);
	checkSuccess(cur->getLongest(6),10);
	checkSuccess(cur->getLongest("testdate"),10);
	checkSuccess(cur->getLongest(7),8);
	checkSuccess(cur->getLongest("testtime"),8);
	stdoutput.printf("\n");*/

	stdoutput.printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("TOTAL ROWS: \n");
	checkSuccess(cur->totalRows(),8);
	stdoutput.printf("\n");

	stdoutput.printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	stdoutput.printf("\n");

	stdoutput.printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	//checkSuccess(cur->getField(0,1),"1.1");
	//checkSuccess(cur->getField(0,2),"1.1");
	checkSuccess(cur->getField(0,3),"1");
	checkSuccess(cur->getField(0,4),"testchar1");
	checkSuccess(cur->getField(0,5),"testvarchar1");
	checkSuccess(cur->getField(0,6),"2001-01-01");
	checkSuccess(cur->getField(0,7),"01:00:00");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	//checkSuccess(cur->getField(7,1),"8.8");
	//checkSuccess(cur->getField(7,2),"8.8");
	checkSuccess(cur->getField(7,3),"8");
	checkSuccess(cur->getField(7,4),"testchar8");
	checkSuccess(cur->getField(7,5),"testvarchar8");
	checkSuccess(cur->getField(7,6),"2008-01-01");
	checkSuccess(cur->getField(7,7),"08:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	//checkSuccess(cur->getFieldLength(0,1),3);
	//checkSuccess(cur->getFieldLength(0,2),3);
	checkSuccess(cur->getFieldLength(0,3),1);
	checkSuccess(cur->getFieldLength(0,4),9);
	checkSuccess(cur->getFieldLength(0,5),12);
	checkSuccess(cur->getFieldLength(0,6),10);
	checkSuccess(cur->getFieldLength(0,7),8);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	//checkSuccess(cur->getFieldLength(7,1),3);
	//checkSuccess(cur->getFieldLength(7,2),3);
	checkSuccess(cur->getFieldLength(7,3),1);
	checkSuccess(cur->getFieldLength(7,4),9);
	checkSuccess(cur->getFieldLength(7,5),12);
	checkSuccess(cur->getFieldLength(7,6),10);
	checkSuccess(cur->getFieldLength(7,7),8);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"testint"),"1");
	//checkSuccess(cur->getField(0,"testfloat"),"1.1");
	//checkSuccess(cur->getField(0,"testreal"),"1.1");
	checkSuccess(cur->getField(0,"testsmallint"),"1");
	checkSuccess(cur->getField(0,"testchar"),"testchar1");
	checkSuccess(cur->getField(0,"testvarchar"),"testvarchar1");
	checkSuccess(cur->getField(0,"testdate"),"2001-01-01");
	checkSuccess(cur->getField(0,"testtime"),"01:00:00");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,"testint"),"8");
	//checkSuccess(cur->getField(7,"testfloat"),"8.8");
	//checkSuccess(cur->getField(7,"testreal"),"8.8");
	checkSuccess(cur->getField(7,"testsmallint"),"8");
	checkSuccess(cur->getField(7,"testchar"),"testchar8");
	checkSuccess(cur->getField(7,"testvarchar"),"testvarchar8");
	checkSuccess(cur->getField(7,"testdate"),"2008-01-01");
	checkSuccess(cur->getField(7,"testtime"),"08:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"testint"),1);
	//checkSuccess(cur->getFieldLength(0,"testfloat"),3);
	//checkSuccess(cur->getFieldLength(0,"testreal"),3);
	checkSuccess(cur->getFieldLength(0,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(0,"testchar"),9);
	checkSuccess(cur->getFieldLength(0,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(0,"testdate"),10);
	checkSuccess(cur->getFieldLength(0,"testtime"),8);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,"testint"),1);
	//checkSuccess(cur->getFieldLength(7,"testfloat"),3);
	//checkSuccess(cur->getFieldLength(7,"testreal"),3);
	checkSuccess(cur->getFieldLength(7,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(7,"testchar"),9);
	checkSuccess(cur->getFieldLength(7,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(7,"testdate"),10);
	checkSuccess(cur->getFieldLength(7,"testtime"),8);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	//checkSuccess(fields[1],"1.1");
	//checkSuccess(fields[2],"1.1");
	checkSuccess(fields[3],"1");
	checkSuccess(fields[4],"testchar1");
	checkSuccess(fields[5],"testvarchar1");
	checkSuccess(fields[6],"2001-01-01");
	checkSuccess(fields[7],"01:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	//checkSuccess(fieldlens[1],3);
	//checkSuccess(fieldlens[2],3);
	checkSuccess(fieldlens[3],1);
	checkSuccess(fieldlens[4],9);
	checkSuccess(fieldlens[5],12);
	checkSuccess(fieldlens[6],10);
	checkSuccess(fieldlens[7],8);
	stdoutput.printf("\n");

	checkSuccess(con->commit(),1);
	snooze::macrosnooze(2,0);

	stdoutput.printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable1 order by testint"),1);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),9);
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

	stdoutput.printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType((uint32_t)0),"INT");
	checkSuccess(cur->getColumnType("testint"),"INT");
	checkSuccess(cur->getColumnType(1),"FLOAT");
	checkSuccess(cur->getColumnType("testfloat"),"FLOAT");
	checkSuccess(cur->getColumnType(2),"REAL");
	checkSuccess(cur->getColumnType("testreal"),"REAL");
	checkSuccess(cur->getColumnType(3),"SMALLINT");
	checkSuccess(cur->getColumnType("testsmallint"),"SMALLINT");
	checkSuccess(cur->getColumnType(4),"STRING");
	checkSuccess(cur->getColumnType("testchar"),"STRING");
	checkSuccess(cur->getColumnType(5),"VARSTRING");
	checkSuccess(cur->getColumnType("testvarchar"),"VARSTRING");
	checkSuccess(cur->getColumnType(6),"DATE");
	checkSuccess(cur->getColumnType("testdate"),"DATE");
	checkSuccess(cur->getColumnType(7),"TIME");
	checkSuccess(cur->getColumnType("testtime"),"TIME");
	checkSuccess(cur->getColumnType(8),"TIMESTAMP");
	checkSuccess(cur->getColumnType("testtimestamp"),"TIMESTAMP");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),4);
	checkSuccess(cur->getColumnLength("testint"),4);
	checkSuccess(cur->getColumnLength(1),4);
	checkSuccess(cur->getColumnLength("testfloat"),4);
	checkSuccess(cur->getColumnLength(2),8);
	checkSuccess(cur->getColumnLength("testreal"),8);
	checkSuccess(cur->getColumnLength(3),2);
	checkSuccess(cur->getColumnLength("testsmallint"),2);
	//checkSuccess(cur->getColumnLength(4),40);
	//checkSuccess(cur->getColumnLength("testchar"),40);
	//checkSuccess(cur->getColumnLength(5),41);
	//checkSuccess(cur->getColumnLength("testvarchar"),41);
	checkSuccess(cur->getColumnLength(6),3);
	checkSuccess(cur->getColumnLength("testdate"),3);
	checkSuccess(cur->getColumnLength(7),3);
	checkSuccess(cur->getColumnLength("testtime"),3);
	checkSuccess(cur->getColumnLength(8),4);
	checkSuccess(cur->getColumnLength("testtimestamp"),4);
	stdoutput.printf("\n");

	stdoutput.printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest("testint"),1);
	//checkSuccess(cur->getLongest(1),3);
	//checkSuccess(cur->getLongest("testfloat"),3);
	//checkSuccess(cur->getLongest(2),3);
	//checkSuccess(cur->getLongest("testreal"),3);
	checkSuccess(cur->getLongest(3),1);
	checkSuccess(cur->getLongest("testsmallint"),1);
	checkSuccess(cur->getLongest(4),9);
	checkSuccess(cur->getLongest("testchar"),9);
	checkSuccess(cur->getLongest(5),12);
	checkSuccess(cur->getLongest("testvarchar"),12);
	checkSuccess(cur->getLongest(6),10);
	checkSuccess(cur->getLongest("testdate"),10);
	checkSuccess(cur->getLongest(7),8);
	checkSuccess(cur->getLongest("testtime"),8);
	stdoutput.printf("\n");

	stdoutput.printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("TOTAL ROWS: \n");
	checkSuccess(cur->totalRows(),8);
	stdoutput.printf("\n");

	stdoutput.printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	stdoutput.printf("\n");

	stdoutput.printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	//checkSuccess(cur->getField(0,1),"1.1");
	//checkSuccess(cur->getField(0,2),"1.1");
	checkSuccess(cur->getField(0,3),"1");
	checkSuccess(cur->getField(0,4),"testchar1");
	checkSuccess(cur->getField(0,5),"testvarchar1");
	checkSuccess(cur->getField(0,6),"2001-01-01");
	checkSuccess(cur->getField(0,7),"01:00:00");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	//checkSuccess(cur->getField(7,1),"8.8");
	//checkSuccess(cur->getField(7,2),"8.8");
	checkSuccess(cur->getField(7,3),"8");
	checkSuccess(cur->getField(7,4),"testchar8");
	checkSuccess(cur->getField(7,5),"testvarchar8");
	checkSuccess(cur->getField(7,6),"2008-01-01");
	checkSuccess(cur->getField(7,7),"08:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	//checkSuccess(cur->getFieldLength(0,1),3);
	//checkSuccess(cur->getFieldLength(0,2),3);
	checkSuccess(cur->getFieldLength(0,3),1);
	checkSuccess(cur->getFieldLength(0,4),9);
	checkSuccess(cur->getFieldLength(0,5),12);
	checkSuccess(cur->getFieldLength(0,6),10);
	checkSuccess(cur->getFieldLength(0,7),8);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	//checkSuccess(cur->getFieldLength(7,1),3);
	//checkSuccess(cur->getFieldLength(7,2),3);
	checkSuccess(cur->getFieldLength(7,3),1);
	checkSuccess(cur->getFieldLength(7,4),9);
	checkSuccess(cur->getFieldLength(7,5),12);
	checkSuccess(cur->getFieldLength(7,6),10);
	checkSuccess(cur->getFieldLength(7,7),8);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"testint"),"1");
	//checkSuccess(cur->getField(0,"testfloat"),"1.1");
	//checkSuccess(cur->getField(0,"testreal"),"1.1");
	checkSuccess(cur->getField(0,"testsmallint"),"1");
	checkSuccess(cur->getField(0,"testchar"),"testchar1");
	checkSuccess(cur->getField(0,"testvarchar"),"testvarchar1");
	checkSuccess(cur->getField(0,"testdate"),"2001-01-01");
	checkSuccess(cur->getField(0,"testtime"),"01:00:00");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,"testint"),"8");
	//checkSuccess(cur->getField(7,"testfloat"),"8.8");
	//checkSuccess(cur->getField(7,"testreal"),"8.8");
	checkSuccess(cur->getField(7,"testsmallint"),"8");
	checkSuccess(cur->getField(7,"testchar"),"testchar8");
	checkSuccess(cur->getField(7,"testvarchar"),"testvarchar8");
	checkSuccess(cur->getField(7,"testdate"),"2008-01-01");
	checkSuccess(cur->getField(7,"testtime"),"08:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"testint"),1);
	//checkSuccess(cur->getFieldLength(0,"testfloat"),3);
	//checkSuccess(cur->getFieldLength(0,"testreal"),3);
	checkSuccess(cur->getFieldLength(0,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(0,"testchar"),9);
	checkSuccess(cur->getFieldLength(0,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(0,"testdate"),10);
	checkSuccess(cur->getFieldLength(0,"testtime"),8);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,"testint"),1);
	//checkSuccess(cur->getFieldLength(7,"testfloat"),3);
	//checkSuccess(cur->getFieldLength(7,"testreal"),3);
	checkSuccess(cur->getFieldLength(7,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(7,"testchar"),9);
	checkSuccess(cur->getFieldLength(7,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(7,"testdate"),10);
	checkSuccess(cur->getFieldLength(7,"testtime"),8);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	//checkSuccess(fields[1],"1.1");
	//checkSuccess(fields[2],"1.1");
	checkSuccess(fields[3],"1");
	checkSuccess(fields[4],"testchar1");
	checkSuccess(fields[5],"testvarchar1");
	checkSuccess(fields[6],"2001-01-01");
	checkSuccess(fields[7],"01:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	//checkSuccess(fieldlens[1],3);
	//checkSuccess(fieldlens[2],3);
	checkSuccess(fieldlens[3],1);
	checkSuccess(fieldlens[4],9);
	checkSuccess(fieldlens[5],12);
	checkSuccess(fieldlens[6],10);
	checkSuccess(fieldlens[7],8);
	stdoutput.printf("\n");

	stdoutput.printf("COMMIT AND ROLLBACK: \n");
	secondcon=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	secondcur=new sqlrcursor(secondcon);
	checkSuccess(con->commit(),1);
	snooze::macrosnooze(2,0);
	checkSuccess(secondcon->commit(),1);
	snooze::macrosnooze(2,0);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable1"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"8");
	checkSuccess(secondcur->sendQuery("select count(*) from testtable2"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"8");
	checkSuccess(con->autoCommitOn(),1);
	checkSuccess(secondcon->autoCommitOn(),1);
	checkSuccess(cur->sendQuery("insert into testtable1 values (10,10.1,10.1,10,'testchar10','testvarchar10','2010-01-01','10:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable2 values (10,10.1,10.1,10,'testchar10','testvarchar10','2010-01-01','10:00:00',NULL)"),1);
	snooze::macrosnooze(2,0);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable1"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"9");
	checkSuccess(secondcur->sendQuery("select count(*) from testtable2"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"9");
	checkSuccess(con->autoCommitOff(),1);
	checkSuccess(secondcon->autoCommitOff(),1);
	checkSuccess(cur->sendQuery("begin"),1);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable1");
	cur->sendQuery("drop table testtable2");

	stdoutput.printf("\n");

	return 0;
}
