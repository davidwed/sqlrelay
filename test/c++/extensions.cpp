// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <rudiments/charstring.h>
#include <rudiments/process.h>
#include <rudiments/datetime.h>
#include <rudiments/signalclasses.h>
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
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("%s!=%s\n",value,success);
			stdoutput.printf("failure ");
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%s!=%s\n",value,success);
		stdoutput.printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(const char *value, const char *success, size_t length) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("%s!=%s\n",value,success);
			stdoutput.printf("failure ");
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!strncmp(value,success,length)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%s!=%s\n",value,success);
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

	// The extensionstest instance uses connections="0", so the
	// sqlr-connection process exits after each client session.  When the
	// client sends its final endSession(), and the server can receive it,
	// process it, and exit, before the client's final write() system call
	// returns, causing the client to receive a SIGPIPE.  It all depends on
	// timing though, and doesn't happen every time.  We'll ignore SIGPIPE
	// here to manage this.
	#ifdef SIGPIPE
	signalset	set;
	set.removeAllSignals();
	set.addSignal(SIGPIPE);
	signalmanager::ignoreSignals(&set);
	#endif

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);

	con->setClientInfo("extensionstest");


	stdoutput.printf("IGNORE SELECT DATABASE:\n");
	char	*originaldb=charstring::duplicate(con->getCurrentDatabase());
	checkSuccess((originaldb!=NULL),true);
	checkSuccess(con->selectDatabase("nonexistentdb"),true);
	checkSuccess(con->getCurrentDatabase(),originaldb);
	delete[] originaldb;
	stdoutput.printf("\n\n");


	stdoutput.printf("TRANSLATE BIND VARIABLES:\n");
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
	stdoutput.printf("\n");

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
	stdoutput.printf("\n");

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
	stdoutput.printf("\n");

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
	stdoutput.printf("\n\n");


	stdoutput.printf("FAKE INPUT BIND VARIABLES:\n");
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
	stdoutput.printf("\n\n");


	stdoutput.printf("ISOLATION LEVELS: \n");

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
	stdoutput.printf("\n");

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
	stdoutput.printf("\n");

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
	stdoutput.printf("\n\n");


	stdoutput.printf("SQLRCMD CSTAT: \n");
	checkSuccess(cur->sendQuery("sqlrcmd cstat"),1);
	checkSuccess(cur->colCount(),9);
	stdoutput.printf("\n");

	checkSuccess(cur->getColumnName((uint32_t)0),"INDEX");
	checkSuccess(cur->getColumnName(1),"MINE");
	checkSuccess(cur->getColumnName(2),"PROCESSID");
	checkSuccess(cur->getColumnName(3),"CONNECT");
	checkSuccess(cur->getColumnName(4),"STATE");
	checkSuccess(cur->getColumnName(5),"STATE_TIME");
	checkSuccess(cur->getColumnName(6),"CLIENT_ADDR");
	checkSuccess(cur->getColumnName(7),"CLIENT_INFO");
	checkSuccess(cur->getColumnName(8),"SQL_TEXT");
	stdoutput.printf("\n");

	checkSuccess(cur->getColumnType((uint32_t)0),"NUMBER");
	checkSuccess(cur->getColumnType(1),"VARCHAR2");
	checkSuccess(cur->getColumnType(2),"NUMBER");
	checkSuccess(cur->getColumnType(3),"NUMBER");
	checkSuccess(cur->getColumnType(4),"VARCHAR2");
	checkSuccess(cur->getColumnType(5),"NUMBER");
	checkSuccess(cur->getColumnType(6),"VARCHAR2");
	checkSuccess(cur->getColumnType(7),"VARCHAR2");
	checkSuccess(cur->getColumnType(8),"VARCHAR2");
	stdoutput.printf("\n");

	checkSuccess(cur->getColumnLength((uint32_t)0),10);
	checkSuccess(cur->getColumnLength(1),1);
	checkSuccess(cur->getColumnLength(2),10);
	checkSuccess(cur->getColumnLength(3),12);
	checkSuccess(cur->getColumnLength(4),25);
	checkSuccess(cur->getColumnLength(5),12);
	checkSuccess(cur->getColumnLength(6),24);
	checkSuccess(cur->getColumnLength(7),511);
	checkSuccess(cur->getColumnLength(8),511);
	stdoutput.printf("\n");

	checkSuccess(cur->getColumnPrecision((uint32_t)0),10);
	checkSuccess(cur->getColumnPrecision(1),0);
	checkSuccess(cur->getColumnPrecision(2),10);
	checkSuccess(cur->getColumnPrecision(3),12);
	checkSuccess(cur->getColumnPrecision(4),0);
	checkSuccess(cur->getColumnPrecision(5),12);
	checkSuccess(cur->getColumnPrecision(6),0);
	checkSuccess(cur->getColumnPrecision(7),0);
	checkSuccess(cur->getColumnPrecision(8),0);
	stdoutput.printf("\n");

	checkSuccess(cur->getColumnScale((uint32_t)0),0);
	checkSuccess(cur->getColumnScale(1),0);
	checkSuccess(cur->getColumnScale(2),0);
	checkSuccess(cur->getColumnScale(3),0);
	checkSuccess(cur->getColumnScale(4),0);
	checkSuccess(cur->getColumnScale(5),2);
	checkSuccess(cur->getColumnScale(6),0);
	checkSuccess(cur->getColumnScale(7),0);
	checkSuccess(cur->getColumnScale(8),0);
	stdoutput.printf("\n");

	checkSuccess(cur->getColumnIsNullable((uint32_t)0),0);
	checkSuccess(cur->getColumnIsNullable(1),0);
	checkSuccess(cur->getColumnIsNullable(2),0);
	checkSuccess(cur->getColumnIsNullable(3),0);
	checkSuccess(cur->getColumnIsNullable(4),0);
	checkSuccess(cur->getColumnIsNullable(5),0);
	checkSuccess(cur->getColumnIsNullable(6),0);
	checkSuccess(cur->getColumnIsNullable(7),1);
	checkSuccess(cur->getColumnIsNullable(8),1);
	stdoutput.printf("\n");

	uint64_t	row=0;
	bool		found=false;
	for (uint64_t i=0; i<cur->rowCount(); i++) {
		if (!charstring::compare(cur->getField(i,(uint32_t)8),
							"sqlrcmd cstat")) {
			found=true;
			row=i;
			break;
		}
	}
	checkSuccess(found,true);
	checkSuccess(cur->getField(row,(uint32_t)1),"*");
	checkSuccess(cur->getField(row,(uint32_t)4),"RETURN_RESULT_SET");
	// 127.0.0.1 on Windows
	//checkSuccess(cur->getField(row,(uint32_t)6),"UNIX");
	checkSuccess(cur->getField(row,(uint32_t)7),"extensionstest");
	checkSuccess(cur->getField(row,(uint32_t)8),"sqlrcmd cstat");
	stdoutput.printf("\n\n");


	stdoutput.printf("SQLRCMD GSTAT: \n");
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
	stdoutput.printf("\n\n");


	stdoutput.printf("SESSION QUERIES: Date Format\n");
	checkSuccess(cur->sendQuery("select sysdate from dual"),1);
	datetime	dt;
	dt.initFromSystemDateTime();
	const char	*field=cur->getField(0,(uint32_t)0);
	char	*day=charstring::getSubString(field,0,1);
	char	*month=charstring::getSubString(field,3,4);
	char	*year=charstring::getSubString(field,6,9);
	char	*hour=charstring::getSubString(field,11,12);
	char	*minute=charstring::getSubString(field,14,15);
	checkSuccess((int)charstring::convertToInteger(day),(int)dt.getDayOfMonth());
	checkSuccess((int)charstring::convertToInteger(month),(int)dt.getMonth());
	checkSuccess((int)charstring::convertToInteger(year),(int)dt.getYear());
	checkSuccess((int)charstring::convertToInteger(hour),(int)dt.getHour());
	int	dbmin=(int)charstring::convertToInteger(minute);
	int	min=(int)dt.getMinute();
	bool	success=((dbmin==min) || (dbmin==min-1) || (dbmin-1==min));
	checkSuccess(success,1);
	delete[] year;
	delete[] day;
	delete[] month;
	delete[] hour;
	delete[] minute;
	stdoutput.printf("\n\n");


	stdoutput.printf("FILTERS:\n");
	checkSuccess(cur->sendQuery("select * from badstring"),0);
	checkSuccess(cur->errorMessage(),"badstring encountered");
	checkSuccess(cur->sendQuery("select * from badregex"),0);
	checkSuccess(cur->errorMessage(),"badregex encountered");
	checkSuccess(cur->errorNumber(),100);
	checkSuccess(cur->sendQuery("select * from badpattern"),0);
	stdoutput.printf("\n\n");

	delete cur;
	delete con;

	stdoutput.printf("PWDENCS:\n");
	const char	*usrpwds[]={
		"test",
		"rot16test",
		"rot13test",
		"rot10test",
		"md5test",
		"sha1test",
		"sha256test",
		"crypttest",
		"aes128test",
		NULL
	};
	for (const char **usrpwd=usrpwds; *usrpwd; usrpwd++) {
		con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							*usrpwd,*usrpwd,0,1);
		cur=new sqlrcursor(con);
		checkSuccess(cur->sendQuery("select 1 from dual"),1);
		stdoutput.printf("\n");
		delete cur;
		delete con;
	}
	stdoutput.printf("\n");

	stdoutput.printf("UPSERT:\n");
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);
	secondcur=new sqlrcursor(con);
	cur->sendQuery("drop table student");
	cur->sendQuery("drop sequence student_id");
	checkSuccess(cur->sendQuery("create sequence student_id"),1);
	checkSuccess(cur->sendQuery("create table student ("
					"id int, "
					"firstname varchar(20), "
					"lastname varchar(20), "
					"year varchar(20), "
					"major varchar(20), "
					"gpa varchar(20), "
					"primary key (id), "
					"unique (firstname,lastname) "
					")"),1);
	stdoutput.printf("\n");
	// initial insert
	checkSuccess(cur->sendQuery("insert into student values "
				"(student_id.nextval,"
				"'David','Muse','Freshman','ME','4.0')"),1);
	checkSuccess(secondcur->sendQuery("select count(*) from student"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"1");
	checkSuccess(secondcur->sendQuery("select * from student"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"1");
	checkSuccess(secondcur->getField(0,1),"David");
	checkSuccess(secondcur->getField(0,2),"Muse");
	checkSuccess(secondcur->getField(0,3),"Freshman");
	checkSuccess(secondcur->getField(0,4),"ME");
	checkSuccess(secondcur->getField(0,5),"4.0");
	stdoutput.printf("\n");
	// should be converted to an update
	checkSuccess(cur->sendQuery("insert into student values "
				"(student_id.nextval,"
				"'David','Muse','Sophomore','ME','3.5')"),1);
	checkSuccess(secondcur->sendQuery("select count(*) from student"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"1");
	checkSuccess(secondcur->sendQuery("select * from student"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"1");
	checkSuccess(secondcur->getField(0,1),"David");
	checkSuccess(secondcur->getField(0,2),"Muse");
	checkSuccess(secondcur->getField(0,3),"Sophomore");
	checkSuccess(secondcur->getField(0,4),"ME");
	checkSuccess(secondcur->getField(0,5),"3.5");
	stdoutput.printf("\n");
	// with bind variables, should also be converted to an update
	cur->prepareQuery("insert into student values "
				"(student_id.nextval,"
				":firstname,:lastname,:year,:major,:gpa)");
	cur->inputBind("firstname","David");
	cur->inputBind("lastname","Muse");
	cur->inputBind("year","Junior");
	cur->inputBind("major","CS");
	cur->inputBind("gpa","3.0");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(secondcur->sendQuery("select count(*) from student"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"1");
	checkSuccess(secondcur->sendQuery("select * from student"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"1");
	checkSuccess(secondcur->getField(0,1),"David");
	checkSuccess(secondcur->getField(0,2),"Muse");
	checkSuccess(secondcur->getField(0,3),"Junior");
	checkSuccess(secondcur->getField(0,4),"CS");
	checkSuccess(secondcur->getField(0,5),"3.0");
	stdoutput.printf("\n");
	// reexecute with bind variables, should also be converted to an update
	cur->inputBind("firstname","David");
	cur->inputBind("lastname","Muse");
	cur->inputBind("year","Senior");
	cur->inputBind("major","CS");
	cur->inputBind("gpa","2.5");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(secondcur->sendQuery("select count(*) from student"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"1");
	checkSuccess(secondcur->sendQuery("select * from student"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"1");
	checkSuccess(secondcur->getField(0,1),"David");
	checkSuccess(secondcur->getField(0,2),"Muse");
	checkSuccess(secondcur->getField(0,3),"Senior");
	checkSuccess(secondcur->getField(0,4),"CS");
	checkSuccess(secondcur->getField(0,5),"2.5");
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("drop table student"),1);
	checkSuccess(cur->sendQuery("drop sequence student_id"),1);
	delete secondcur;
	stdoutput.printf("\n\n");

	stdoutput.printf("ERROR TRANSLATION:\n");
	checkSuccess(cur->sendQuery("select 1"),0);
	checkSuccess(cur->errorNumber(),10923);
	checkSuccess(cur->errorMessage(),
			"ORA-10923: fRoM kEyWoRd nOt fOuNd wHeRe eXpEcTeD");
	stdoutput.printf("\n\n");

	stdoutput.printf("done\n");
	delete cur;
	delete con;
	stdoutput.printf("\n\n");

	return 0;
}
