// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/randomnumber.h>
#include <rudiments/thread.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>

const char	*host;
uint16_t	port;
const char	*sock;
const char	*login;
const char	*password;
int64_t		threadcount;
int64_t		cursorcount;

void runQuery(void *id) {

	uint64_t	threadid=(uint64_t)id;
	uint32_t	seed=threadid;

	for (;;) {
								
		sqlrconnection	sqlrcon(host,port,sock,login,password,0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		stringbuffer	query;
		int32_t		colcount=0;
		int32_t		rowcount=0;
		int32_t		value=0;
		int32_t		times=0;

		// drop the table (just in case)
		query.clear();
		query.append("drop table test")->append(threadid);
		sqlrcur.sendQuery(query.getString());

		seed=randomnumber::generateNumber(seed);

		int32_t	loopcount=randomnumber::scaleNumber(seed,1,20);
		stdoutput.printf("%lld: looping %d times\n",id,loopcount);

		for (int32_t l=0; l<loopcount; l++) {

			// create a table with a random number of fields
			seed=randomnumber::generateNumber(seed);
			colcount=randomnumber::scaleNumber(seed,1,15);
			stdoutput.printf("%lld: creating table with %d cols\n",
								id,colcount);
			query.clear();
			query.append("create table test");
			query.append(threadid)->append(" (");
			for (int32_t i=0; i<colcount; i++) {
				if (i) {
					query.append(", ");
				}
				query.append("col")->append(i)->append(" int");
			}
			query.append(")");
			if (!sqlrcur.sendQuery(query.getString())) {
				stdoutput.printf("%lld: %s\n",
						id,sqlrcur.errorMessage());
			}

			// populate it with a random number of rows
			seed=randomnumber::generateNumber(seed);
			rowcount=randomnumber::scaleNumber(seed,1,100);
			stdoutput.printf("%lld: populating with %d rows\n",
								id,rowcount);
			for (int32_t i=0; i<rowcount; i++) {
				seed=randomnumber::generateNumber(seed);
				value=randomnumber::scaleNumber(seed,1,100000);
				query.clear();
				query.append("insert into test");
				query.append(threadid)->append(" values (");
				for (int32_t j=0; j<colcount; j++) {
					if (j) {
						query.append(", ");
					}
					query.append(value);
				}
				query.append(")");
				if (!sqlrcur.sendQuery(query.getString())) {
					stdoutput.printf("%lld: %s\n",
						id,sqlrcur.errorMessage());
				}
			}

			// select those rows a random number of times,
			// use a new cursor for each time
			seed=randomnumber::generateNumber(seed);
			times=randomnumber::scaleNumber(seed,1,4);
			stdoutput.printf("%lld: selecting %d times with "
					"%lld nested cursors\n",
					id,times,cursorcount);
			for (int64_t i=0; i<times; i++) {
				sqlrcursor	**cursors=
						new sqlrcursor *[cursorcount];
				cursors[0]=&sqlrcur;
				for (int64_t j=0; j<cursorcount; j++) {
					if (j) {
						cursors[j]=
						new sqlrcursor(&sqlrcon);
					}
					query.clear();
					query.append("select * from test");
					query.append(threadid);
					if (!cursors[j]->sendQuery(
							query.getString())) {
						stdoutput.printf("%s\n",
						cursors[j]->errorMessage());
					}
				}
				for (int32_t j=1; j<cursorcount; j++) {
					delete cursors[j];
				}
				delete cursors;
			}
	
			// drop the table
			query.clear();
			query.append("drop table test")->append(threadid);
			sqlrcur.sendQuery(query.getString());
		}
	}
}

int main(int argc, char **argv) {

	if (argc<3) {
		stdoutput.printf("usage: stress connectioncount cursorcount\n");
		process::exit(1);
	}

	host="sqlrserver";
	port=9000;
	sock="/tmp/test.socket";
	login="test";
	password="test";
	threadcount=charstring::toInteger(argv[1]);
	cursorcount=charstring::toInteger(argv[2]);

	thread	*th=new thread[threadcount];

	for (int64_t i=0; i<threadcount; i++) {
		th[i].setFunction((void *(*)(void *))runQuery,(void *)i);
		th[i].create();
	}

	for (int64_t i=0; i<threadcount; i++) {
		th[i].join(NULL);
	}
	delete[] th;
	process::exit(1);
}
