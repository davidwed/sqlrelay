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

	stdoutput.printf("UPSERT:\n");
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);
	secondcur=new sqlrcursor(con);
	cur->sendQuery("drop table student");
	checkSuccess(cur->sendQuery("create table student ("
					"id int auto_increment, "
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
				"(null,"
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
				"(null,"
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
	cur->prepareQuery("insert into student values (null,?,?,?,?,?)");
	cur->inputBind("1","David");
	cur->inputBind("2","Muse");
	cur->inputBind("3","Junior");
	cur->inputBind("4","CS");
	cur->inputBind("5","3.0");
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
	cur->inputBind("1","David");
	cur->inputBind("2","Muse");
	cur->inputBind("3","Senior");
	cur->inputBind("4","CS");
	cur->inputBind("5","2.5");
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
	delete secondcur;
	delete cur;
	delete con;
	stdoutput.printf("\n\n");

	stdoutput.printf("done\n");
	stdoutput.printf("\n\n");

	return 0;
}
