// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/randomnumber.h>
#include <rudiments/datetime.h>
#include <rudiments/snooze.h>
#include <rudiments/daemonprocess.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char	*host;
int		port;
const  char	*sock;
const char	*login;
const char	*password;
const char	*query;
int		forkcount;

void	runQuery(int seed) {

	for (;;) {

		sqlrconnection	sqlrcon(host,port,sock,login,password,0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		seed=randomnumber::generateNumber(seed);
		int	count=randomnumber::scaleNumber(seed,1,20);
		//count=10;
								
		printf("%d: looping %d times\n",getpid(),count);
		int	successcount=0;
		for (int i=0; i<count; i++) {
			if (!sqlrcur.sendQuery(query)) {
				printf("error: %s\n",sqlrcur.errorMessage());
				//exit(0);
			} else {
				successcount++;
			}
		}
		printf("%d: succeeded\n",successcount);
	}
}

main(int argc, char **argv) {

	if (argc<2) {
		printf("usage: forktest query forkcount\n");
		exit(1);
	}

	host="localhost";
	port=9000;
	sock="/tmp/test.socket";
	login="test";
	password="test";
	query=argv[1];
	forkcount=atoi(argv[2]);

	for (int i=0; i<forkcount; i++) {
		if (!fork()) {
			datetime	dt;
			dt.getSystemDateAndTime();
			runQuery(dt.getEpoch());
			_exit(0);
		}
		//snooze::microsnooze(0,50000);
		//daemonprocess::waitForChildren();
	}
}
