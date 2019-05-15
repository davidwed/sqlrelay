// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

int	main(int argc, char **argv) {

	semaphoreset	sem;

	// create the key file
	file::remove("semkey");
	file	fd;
	if (!fd.create("semkey",permissions::evalPermString("rw-------"))) {
		stdoutput.printf("failed to create semkey file\n");
		process::exit(1);
	}
	fd.close();

	// create the semaphore
	int32_t	vals[3]={0,0,0};
	if (!sem.create(file::generateKey("semkey",1),
			permissions::evalPermString("rw-------"),
			3,vals)) {
		stdoutput.printf("failed to create semaphoreset\n");
		process::exit(1);
	}

	pid_t	pid1=process::fork();
	if (!pid1) {

		// attach to the semaphore
		if (!sem.attach(file::generateKey("semkey",1),3)) {
			stdoutput.printf("failed to attach to "
						"the semaphoreset\n");
			process::exit(1);
		}

		// connect to relay
		sqlrconnection	sqlrcon("sqlrelay",9000,
				"/tmp/test.socket","test","test",0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		stdoutput.printf("tx 1...\n");

		sqlrcur.sendQuery("drop table testtable");
		sqlrcur.sendQuery("create table testtable "
			"(col1 int primary key auto_increment, col2 int)");
		sqlrcur.sendQuery("insert into testtable values (1,1)");
		sqlrcur.sendQuery("insert into testtable values (2,1)");

		// done
		process::exit(0);
	}

	pid_t	pid2=process::fork();
	if (!pid2) {

		// attach to the semaphore
		if (!sem.attach(file::generateKey("semkey",1),3)) {
			stdoutput.printf("failed to attach to "
						"the semaphoreset\n");
			process::exit(1);
		}

		// connect to relay
		sqlrconnection	sqlrcon("sqlrelay",9000,
				"/tmp/test.socket","test","test",0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		stdoutput.printf("tx 2...\n");

		// done
		process::exit(0);
	}

	// wait for children to exit
	process::wait(pid1);
	process::wait(pid2);

	// clean up
	file::remove("semkey");

	// done
	process::exit(0);
}
