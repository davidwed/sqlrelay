// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/randomnumber.h>
#include <rudiments/thread.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>

const char	*host;
int		port;
const char	*sock;
const char	*login;
const char	*password;
const char	*query;
int		threadcount;

void runQuery(void *id) {

	uint32_t	seed=(int64_t)id;

	for (;;) {

		sqlrconnection	sqlrcon(host,port,sock,login,password,0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		seed=randomnumber::generateNumber(seed);
		int32_t	count=randomnumber::scaleNumber(seed,1,20);
		//count=10;
								
		stdoutput.printf("%lld: looping %d times\n",(uint64_t)id,count);
		int32_t	successcount=0;
		for (int32_t i=0; i<count; i++) {
			if (!sqlrcur.sendQuery(query)) {
				stdoutput.printf("error: %s\n",
						sqlrcur.errorMessage());
				//return;
			} else {
				successcount++;
			}
		}
		stdoutput.printf("%d: succeeded\n",successcount);
	}
}

int main(int argc, char **argv) {

	if (argc<3) {
		stdoutput.printf("usage: threadtest \"query\" threadcount\n");
		process::exit(1);
	}

	host="sqlrserver";
	//host="192.168.123.13";
	port=9000;
	sock="/tmp/test.socket";
	login="test";
	password="test";
	query=argv[1];
	threadcount=charstring::toInteger(argv[2]);

	thread	th[threadcount];

	for (int64_t i=0; i<threadcount; i++) {
		th[i].setFunction((void *(*)(void *))runQuery,(void *)i);
		th[i].create();
	}

	for (int64_t i=0; i<threadcount; i++) {
		th[i].join(NULL);
	}
	process::exit(1);
}
