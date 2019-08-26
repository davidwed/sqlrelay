// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <rudiments/charstring.h>
#include <rudiments/process.h>
#include <rudiments/datetime.h>
#include <sqlrelay/sqlrclient.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("%s!=%s\n",value,success);
			printf("failure ");
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		printf("success ");
	} else {
		printf("%s!=%s\n",value,success);
		printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(const char *value, const char *success, size_t length) {

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("%s!=%s\n",value,success);
			printf("failure ");
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!strncmp(value,success,length)) {
		printf("success ");
	} else {
		printf("%s!=%s\n",value,success);
		printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("%d!=%d\n",value,success);
		printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(double value, double success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("%f!=%f\n",value,success);
		printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

int	main(int argc, char **argv) {

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);

	con->setClientInfo("extensionstest");


	printf("IGNORE SELECT DATABASE:\n");
	const char	*originaldb=con->getCurrentDatabase();
	checkSuccess((originaldb!=NULL),true);
	checkSuccess(con->selectDatabase("nonexistentdb"),true);
	checkSuccess(con->getCurrentDatabase(),originaldb);
	printf("\n\n");


	printf("TRANSLATE BIND VARIABLES:\n");
	cur->prepareQuery("select :1 from dual where 'hel''lo'='hel''lo' and 1=:2 and 2=:3");
	cur->validateBinds();
	cur->inputBind("1","hello");
	cur->inputBind("2",1);
	cur->inputBind("3",2);
	checkSuccess(cur->validBind("1"),true);
	checkSuccess(cur->validBind("2"),true);
	checkSuccess(cur->validBind("3"),true);
	checkSuccess(cur->validBind("4"),false);
	checkSuccess(cur->countBindVariables(),3);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"hello");
	cur->clearBinds();
	printf("\n");

	cur->prepareQuery("select @1 from dual where 'hel''lo'='hel''lo' and 1=@2 and 2=@3");
	cur->validateBinds();
	cur->inputBind("1","hello");
	cur->inputBind("2",1);
	cur->inputBind("3",2);
	checkSuccess(cur->validBind("1"),true);
	checkSuccess(cur->validBind("2"),true);
	checkSuccess(cur->validBind("3"),true);
	checkSuccess(cur->validBind("4"),false);
	checkSuccess(cur->countBindVariables(),3);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"hello");
	cur->clearBinds();
	printf("\n");

	cur->prepareQuery("select $1 from dual where 'hel''lo'='hel''lo' and 1=$2 and 2=$3");
	cur->validateBinds();
	cur->inputBind("1","hello");
	cur->inputBind("2",1);
	cur->inputBind("3",2);
	checkSuccess(cur->validBind("1"),true);
	checkSuccess(cur->validBind("2"),true);
	checkSuccess(cur->validBind("3"),true);
	checkSuccess(cur->validBind("4"),false);
	checkSuccess(cur->countBindVariables(),3);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"hello");
	cur->clearBinds();
	printf("\n");

	cur->prepareQuery("select ? from dual where 'hel''lo'='hel''lo' and 1=? and 2=?");
	cur->validateBinds();
	cur->inputBind("1","hello");
	cur->inputBind("2",1);
	cur->inputBind("3",2);
	checkSuccess(cur->validBind("1"),true);
	checkSuccess(cur->validBind("2"),true);
	checkSuccess(cur->validBind("3"),true);
	checkSuccess(cur->validBind("4"),false);
	checkSuccess(cur->countBindVariables(),3);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"hello");
	cur->clearBinds();
	printf("\n\n");


	printf("FAKE INPUT BIND VARIABLES:\n");
	cur->prepareQuery("select '',1,'',:hello,'''','\\'' from dual where 1=:one");
	cur->inputBind("hello","hello");
	cur->inputBind("one","1");
	cur->inputBind("nonexistentvar","nonexistentval");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"");
	checkSuccess(cur->getField(0,(uint32_t)1),"1");
	checkSuccess(cur->getField(0,(uint32_t)2),"");
	checkSuccess(cur->getField(0,(uint32_t)3),"hello");
	checkSuccess(cur->getField(0,(uint32_t)4),"'");
	checkSuccess(cur->getField(0,(uint32_t)5),"'");
	printf("\n\n");


	printf("ISOLATION LEVELS: \n");

	// set autocommit off
	checkSuccess(con->autoCommitOff(),1);

	// create a table
	cur->sendQuery("drop table testtable");
	checkSuccess(cur->sendQuery("create table testtable (col1 int)"),1);

	// open a second connection and set autocommit off there too
	secondcon=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	secondcur=new sqlrcursor(secondcon);
	checkSuccess(secondcon->autoCommitOff(),1);

	// change the isolation level
	checkSuccess(secondcur->sendQuery("alter session set isolation_level=serializable"),1);
	printf("\n");

	// in the second connection, select from the table, it should be empty
	checkSuccess(secondcur->sendQuery("select * from testtable"),1);
	checkSuccess(secondcur->rowCount(),0);

	// in the first connection, insert a row into the table
	checkSuccess(cur->sendQuery("insert into testtable values (1)"),1);

	// in the second connection, select again, it should still be empty
	checkSuccess(secondcur->sendQuery("select * from testtable"),1);
	checkSuccess(secondcur->rowCount(),0);

	// in the first connecton, commit
	checkSuccess(con->commit(),1);
	printf("\n");

	// in the second connection, select again, it should STILL be empty
	checkSuccess(secondcur->sendQuery("select * from testtable"),1);
	checkSuccess(secondcur->rowCount(),0);

	// end the second connections sesssion and select again,
	// finally it should see the row
	secondcon->endSession();
	checkSuccess(secondcur->sendQuery("select * from testtable"),1);
	checkSuccess(secondcur->rowCount(),1);

	// clean up
	delete secondcur;
	delete secondcon;
	delete cur;
	delete con;
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);
	checkSuccess(cur->sendQuery("drop table testtable"),1);
	con->setClientInfo("extensionstest");
	printf("\n\n");


	printf("SQLRCMD CSTAT: \n");
	checkSuccess(cur->sendQuery("sqlrcmd cstat"),1);
	checkSuccess(cur->colCount(),9);
	printf("\n");

	checkSuccess(cur->getColumnName((uint32_t)0),"INDEX");
	checkSuccess(cur->getColumnName(1),"MINE");
	checkSuccess(cur->getColumnName(2),"PROCESSID");
	checkSuccess(cur->getColumnName(3),"CONNECT");
	checkSuccess(cur->getColumnName(4),"STATE");
	checkSuccess(cur->getColumnName(5),"STATE_TIME");
	checkSuccess(cur->getColumnName(6),"CLIENT_ADDR");
	checkSuccess(cur->getColumnName(7),"CLIENT_INFO");
	checkSuccess(cur->getColumnName(8),"SQL_TEXT");
	printf("\n");

	checkSuccess(cur->getColumnType((uint32_t)0),"NUMBER");
	checkSuccess(cur->getColumnType(1),"VARCHAR2");
	checkSuccess(cur->getColumnType(2),"NUMBER");
	checkSuccess(cur->getColumnType(3),"NUMBER");
	checkSuccess(cur->getColumnType(4),"VARCHAR2");
	checkSuccess(cur->getColumnType(5),"NUMBER");
	checkSuccess(cur->getColumnType(6),"VARCHAR2");
	checkSuccess(cur->getColumnType(7),"VARCHAR2");
	checkSuccess(cur->getColumnType(8),"VARCHAR2");
	printf("\n");

	checkSuccess(cur->getColumnLength((uint32_t)0),10);
	checkSuccess(cur->getColumnLength(1),1);
	checkSuccess(cur->getColumnLength(2),10);
	checkSuccess(cur->getColumnLength(3),12);
	checkSuccess(cur->getColumnLength(4),25);
	checkSuccess(cur->getColumnLength(5),12);
	checkSuccess(cur->getColumnLength(6),24);
	checkSuccess(cur->getColumnLength(7),511);
	checkSuccess(cur->getColumnLength(8),511);
	printf("\n");

	checkSuccess(cur->getColumnPrecision((uint32_t)0),10);
	checkSuccess(cur->getColumnPrecision(1),0);
	checkSuccess(cur->getColumnPrecision(2),10);
	checkSuccess(cur->getColumnPrecision(3),12);
	checkSuccess(cur->getColumnPrecision(4),0);
	checkSuccess(cur->getColumnPrecision(5),12);
	checkSuccess(cur->getColumnPrecision(6),0);
	checkSuccess(cur->getColumnPrecision(7),0);
	checkSuccess(cur->getColumnPrecision(8),0);
	printf("\n");

	checkSuccess(cur->getColumnScale((uint32_t)0),0);
	checkSuccess(cur->getColumnScale(1),0);
	checkSuccess(cur->getColumnScale(2),0);
	checkSuccess(cur->getColumnScale(3),0);
	checkSuccess(cur->getColumnScale(4),0);
	checkSuccess(cur->getColumnScale(5),2);
	checkSuccess(cur->getColumnScale(6),0);
	checkSuccess(cur->getColumnScale(7),0);
	checkSuccess(cur->getColumnScale(8),0);
	printf("\n");

	checkSuccess(cur->getColumnIsNullable((uint32_t)0),0);
	checkSuccess(cur->getColumnIsNullable(1),0);
	checkSuccess(cur->getColumnIsNullable(2),0);
	checkSuccess(cur->getColumnIsNullable(3),0);
	checkSuccess(cur->getColumnIsNullable(4),0);
	checkSuccess(cur->getColumnIsNullable(5),0);
	checkSuccess(cur->getColumnIsNullable(6),0);
	checkSuccess(cur->getColumnIsNullable(7),1);
	checkSuccess(cur->getColumnIsNullable(8),1);
	printf("\n");

	checkSuccess(cur->rowCount(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"0");
	checkSuccess(cur->getField(0,(uint32_t)1),"*");
	checkSuccess(cur->getField(0,(uint32_t)4),"RETURN_RESULT_SET");
	// 127.0.0.1 on Windows
	//checkSuccess(cur->getField(0,(uint32_t)6),"UNIX");
	checkSuccess(cur->getField(0,(uint32_t)7),"extensionstest");
	checkSuccess(cur->getField(0,(uint32_t)8),"sqlrcmd cstat");
	printf("\n\n");


	printf("SQLRCMD GSTAT: \n");
	checkSuccess(cur->sendQuery("sqlrcmd gstat"),1);

	checkSuccess(cur->colCount(),2);

	checkSuccess(cur->getColumnName((uint32_t)0),"KEY");
	checkSuccess(cur->getColumnName(1),"VALUE");

	checkSuccess(cur->getColumnType((uint32_t)0),"VARCHAR2");
	checkSuccess(cur->getColumnType(1),"VARCHAR2");

	checkSuccess(cur->getColumnLength((uint32_t)0),40);
	checkSuccess(cur->getColumnLength(1),40);

	checkSuccess(cur->getField(0,(uint32_t)0),"start");
	checkSuccess(cur->getField(1,(uint32_t)0),"uptime");
	checkSuccess(cur->getField(2,(uint32_t)0),"now");
	checkSuccess(cur->getField(3,(uint32_t)0),"access_count");
	checkSuccess(cur->getField(4,(uint32_t)0),"query_total");
	checkSuccess(cur->getField(5,(uint32_t)0),"qpm");
	checkSuccess(cur->getField(6,(uint32_t)0),"qpm_1");
	checkSuccess(cur->getField(7,(uint32_t)0),"qpm_5");
	checkSuccess(cur->getField(8,(uint32_t)0),"qpm_15");
	checkSuccess(cur->getField(9,(uint32_t)0),"select_1");
	checkSuccess(cur->getField(10,(uint32_t)0),"select_5");
	checkSuccess(cur->getField(11,(uint32_t)0),"select_15");
	checkSuccess(cur->getField(12,(uint32_t)0),"insert_1");
	checkSuccess(cur->getField(13,(uint32_t)0),"insert_5");
	checkSuccess(cur->getField(14,(uint32_t)0),"insert_15");
	checkSuccess(cur->getField(15,(uint32_t)0),"update_1");
	checkSuccess(cur->getField(16,(uint32_t)0),"update_5");
	checkSuccess(cur->getField(17,(uint32_t)0),"update_15");
	checkSuccess(cur->getField(18,(uint32_t)0),"delete_1");
	checkSuccess(cur->getField(19,(uint32_t)0),"delete_5");
	checkSuccess(cur->getField(20,(uint32_t)0),"delete_15");
	checkSuccess(cur->getField(21,(uint32_t)0),"etc_1");
	checkSuccess(cur->getField(22,(uint32_t)0),"etc_5");
	checkSuccess(cur->getField(23,(uint32_t)0),"etc_15");
	checkSuccess(cur->getField(24,(uint32_t)0),"sqlrcmd_1");
	checkSuccess(cur->getField(25,(uint32_t)0),"sqlrcmd_5");
	checkSuccess(cur->getField(26,(uint32_t)0),"sqlrcmd_15");
	checkSuccess(cur->getField(27,(uint32_t)0),"max_listener");
	checkSuccess(cur->getField(28,(uint32_t)0),"max_listener_error");
	checkSuccess(cur->getField(29,(uint32_t)0),"busy_listener");
	checkSuccess(cur->getField(30,(uint32_t)0),"peak_listener");
	checkSuccess(cur->getField(31,(uint32_t)0),"connection");
	checkSuccess(cur->getField(32,(uint32_t)0),"session");
	checkSuccess(cur->getField(33,(uint32_t)0),"peak_session");
	checkSuccess(cur->getField(34,(uint32_t)0),"peak_session_1min");
	checkSuccess(cur->getField(35,(uint32_t)0),"peak_session_1min_time");
	printf("\n\n");


	printf("SESSION QUERIES: Date Format\n");
	checkSuccess(cur->sendQuery("select sysdate from dual"),1);
	datetime	dt;
	dt.getSystemDateAndTime();
	const char	*field=cur->getField(0,(uint32_t)0);
	char	*day=charstring::subString(field,0,1);
	char	*month=charstring::subString(field,3,4);
	char	*year=charstring::subString(field,6,9);
	char	*hour=charstring::subString(field,11,12);
	char	*minute=charstring::subString(field,14,15);
	checkSuccess((int)charstring::toInteger(day),(int)dt.getDayOfMonth());
	checkSuccess((int)charstring::toInteger(month),(int)dt.getMonth());
	checkSuccess((int)charstring::toInteger(year),(int)dt.getYear());
	checkSuccess((int)charstring::toInteger(hour),(int)dt.getHour());
	int	dbmin=(int)charstring::toInteger(minute);
	int	min=(int)dt.getMinutes();
	bool	success=((dbmin==min) || (dbmin==min-1) || (dbmin-1==min));
	checkSuccess(success,1);
	delete[] year;
	delete[] day;
	delete[] month;
	delete[] hour;
	delete[] minute;
	printf("\n\n");


	printf("FILTERS:\n");
	checkSuccess(cur->sendQuery("select * from badstring"),0);
	checkSuccess(cur->errorMessage(),"badstring encountered");
	checkSuccess(cur->sendQuery("select * from badregex"),0);
	checkSuccess(cur->errorMessage(),"badregex encountered");
	checkSuccess(cur->errorNumber(),100);
	checkSuccess(cur->sendQuery("select * from badpattern"),0);
	printf("\n\n");

	delete cur;
	delete con;

	return 0;
}
