// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/process.h>
#include <rudiments/snooze.h>
#include <rudiments/stdio.h>

semaphoreset	*sem;
uint16_t	sessionid;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("%s!=%s\n",value,success);
			stdoutput.printf("failure ");
			if (!sessionid) {
				delete sem;
				file::remove("semkey");
			} else if (sessionid==1) {
				sem->signal(0);
				sem->signal(1);
			}
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%s!=%s\n",value,success);
		stdoutput.printf("failure ");
		if (!sessionid) {
			delete sem;
			file::remove("semkey");
		} else if (sessionid==1) {
			sem->signal(0);
			sem->signal(1);
		}
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%d!=%d\n",value,success);
		stdoutput.printf("failure ");
		if (!sessionid) {
			delete sem;
			file::remove("semkey");
		} else if (sessionid==1) {
			sem->signal(0);
			sem->signal(1);
		}
		process::exit(1);
	}
}

int main(int argc, char **argv) {

	sessionid=0;

	sem=new semaphoreset();

	// create the key file
	file::remove("semkey");
	file	fd;
	if (!fd.create("semkey",permissions::parsePermString("rw-------"))) {
		stdoutput.printf("failed to create semkey file\n");
		process::exit(1);
	}
	fd.close();

	// create the semaphore
	int32_t	vals[3]={0,0,0};
	if (!sem->create(file::generateKey("semkey",1),
			permissions::parsePermString("rw-------"),
			3,vals)) {
		stdoutput.printf("failed to create semaphoreset\n");
		process::exit(1);
	}

	pid_t	pid1=process::fork();
	if (!pid1) {

		sessionid=1;

		// attach to the semaphore
		if (!sem->attach(file::generateKey("semkey",1),3)) {
			stdoutput.printf("failed to attach to "
						"the semaphoreset\n");
			process::exit(1);
		}

		// connect to relay
		sqlrconnection	sqlrcon("sqlrelay",9000,
			"/tmp/test.socket","testuser","testpassword",0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		stdoutput.printf("SESSION 1...\n");

		// set things up
		sqlrcur.sendQuery("drop table testtable");
		checkSuccess(sqlrcur.sendQuery("create table testtable "
			"(col1 int primary key auto_increment, col2 int)"),1);
		checkSuccess(sqlrcur.sendQuery(
			"insert into testtable values (1,1)"),1);
		checkSuccess(sqlrcur.sendQuery(
			"insert into testtable values (2,1)"),1);
		stdoutput.printf("\n");

		// execute the initial update
		checkSuccess(sqlrcon.begin(),1);
		checkSuccess(sqlrcur.sendQuery("update testtable set "
						"col2=col2+1 where col1=1"),1);
		stdoutput.printf("\n");

		// signal the second session to go
		sem->signal(0);

		// wait for the second session to do its update
		snooze::macrosnooze(3);

		stdoutput.printf("SESSION 1...\n");

		// execute the final update
		checkSuccess(sqlrcur.sendQuery("update testtable set "
						"col2=col2+1 where col1=2"),1);
		checkSuccess(sqlrcon.commit(),1);
		stdoutput.printf("\n");

		// signal the second session to go
		sem->signal(1);

		// done
		process::exit(0);
	}

	pid_t	pid2=process::fork();
	if (!pid2) {

		sessionid=2;

		// attach to the semaphore
		if (!sem->attach(file::generateKey("semkey",1),3)) {
			stdoutput.printf("failed to attach to "
						"the semaphoreset\n");
			process::exit(1);
		}

		// connect to relay
		sqlrconnection	sqlrcon("sqlrelay",9000,
			"/tmp/test.socket","testuser","testpassword",0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		// wait for the first session to let us go
		sem->wait(0);

		stdoutput.printf("SESSION 2...\n");

		// execute the conflicting updates
		checkSuccess(sqlrcon.begin(),1);
		checkSuccess(sqlrcur.sendQuery("update testtable set "
						"col2=col2+1 where col1=2"),1);
		stdoutput.printf("\n");


		// this one should hang and wait for session 1 to commit
		bool	qr=sqlrcur.sendQuery("update testtable set "
						"col2=col2+1 where col1=1");

		// commit
		bool	cr=sqlrcon.commit();

		// wait for the first session to let us go
		sem->wait(1);

		stdoutput.printf("SESSION 2...\n");
		checkSuccess(qr,1);
		checkSuccess(cr,1);

		// done
		process::exit(0);
	}

	// wait for children to exit
	process::wait(pid1);
	process::wait(pid2);
	stdoutput.printf("\n");

	// connect to relay
	sqlrconnection	sqlrcon("sqlrelay",9000,
			"/tmp/test.socket","testuser","testpassword",0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	stdoutput.printf("RESULTS: \n");
	sqlrcur.sendQuery("select * from testtable order by col1");
	checkSuccess(sqlrcur.getField(0,"col1"),"1");
	checkSuccess(sqlrcur.getField(0,"col2"),"3");
	checkSuccess(sqlrcur.getField(1,"col1"),"2");
	checkSuccess(sqlrcur.getField(1,"col2"),"3");
	stdoutput.printf("\n");

	// clean up
	delete sem;
	file::remove("semkey");

	// done
	process::exit(0);
}
