// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <rudiments/randomnumber.h>
#include <rudiments/thread.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>
#include <rudiments/snooze.h>
#include <rudiments/inetsocketclient.h>

const char	*host;
uint16_t	port;
const char	*sock;
const char	*user;
const char	*password;
int64_t		concount;
int64_t		curcount;
bool		terminated;
uint32_t	garbage;

void shutDown(int32_t signum) {
	terminated=true;
}

void queriesTest(void *id) {

	uint64_t	threadid=(uint64_t)id;
	uint32_t	seed=threadid;

	stdoutput.printf("%lld: starting\n",threadid);

	while (!terminated) {

		sqlrconnection	sqlrcon(host,port,sock,user,password,0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		stringbuffer	query;
		int32_t		colcount=0;
		int32_t		rowcount=0;
		int32_t		value=0;
		int32_t		times=0;

		// drop the table (just in case)
		query.append("drop table test")->append(threadid);

		if (!sqlrcur.sendQuery(query.getString())) {
			// loop back if we couldn't connect to the listener
			if (charstring::contains(sqlrcur.errorMessage(),
					"Couldn't connect to the listener.")) {
				snooze::macrosnooze(1);
				continue;
			}
		}

		seed=randomnumber::generate(seed);

		int32_t	loopcount=randomnumber::scale(seed,1,20);
		stdoutput.printf("%lld: looping %d times\n",threadid,loopcount);

		for (int32_t l=0; l<loopcount; l++) {

			// create a table with a random number of fields
			seed=randomnumber::generate(seed);
			colcount=randomnumber::scale(seed,1,15);
			stdoutput.printf("%lld: creating table with %d cols\n",
							threadid,colcount);
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
			if (!terminated &&
				!sqlrcur.sendQuery(query.getString())) {
				stdoutput.printf("%lld: create table - %s\n",
					threadid,sqlrcur.errorMessage());
			}

			// populate it with a random number of rows
			seed=randomnumber::generate(seed);
			rowcount=randomnumber::scale(seed,1,100);
			stdoutput.printf("%lld: populating with %d rows\n",
							threadid,rowcount);
			for (int32_t i=0; i<rowcount; i++) {
				seed=randomnumber::generate(seed);
				value=randomnumber::scale(seed,1,100000);
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
				if (!terminated &&
					!sqlrcur.sendQuery(query.getString())) {
					stdoutput.printf("%lld: insert - %s\n",
					threadid,sqlrcur.errorMessage());
				}
			}

			// select those rows a random number of times,
			// use a new cursor for each time
			seed=randomnumber::generate(seed);
			times=randomnumber::scale(seed,1,4);
			stdoutput.printf("%lld: selecting %d times with "
					"%lld nested cursors\n",
					threadid,times,curcount);
			for (int64_t i=0; i<times; i++) {
				sqlrcursor	**cursors=
						new sqlrcursor *[curcount];
				cursors[0]=&sqlrcur;
				for (int64_t j=0; j<curcount; j++) {
					if (j) {
						cursors[j]=
						new sqlrcursor(&sqlrcon);
					}
					query.clear();
					query.append("select * from test");
					query.append(threadid);
					if (!terminated &&
						!cursors[j]->sendQuery(
							query.getString())) {
						stdoutput.printf(
						"%lld: select - %s\n",threadid,
						cursors[j]->errorMessage());
					}
				}
				for (int32_t j=1; j<curcount; j++) {
					delete cursors[j];
				}
				delete[] cursors;
			}
	
			// drop the table
			stdoutput.printf("%lld: dropping table\n",threadid);
			query.clear();
			query.append("drop table test")->append(threadid);
			sqlrcur.sendQuery(query.getString());
		}

		stdoutput.printf("%lld: disconnecting\n",threadid);
	}
}

void heartbeatTest(void *id) {

	uint64_t	threadid=(uint64_t)id;
	uint32_t	seed=threadid;

	inetsocketclient	isc;
	isc.setHost(host);
	isc.setPort(port);

	uint64_t	i=0;
	while (!terminated) {

		if (isc.connect()==RESULT_SUCCESS) {

			stdoutput.printf(
				"%lld: %d: connect success\n",threadid,i);

			if (garbage) {

				seed=randomnumber::generate(seed);
				int32_t	garbagesize=
					randomnumber::scale(seed,1,garbage);

				char	*gbg=new char[garbagesize];
				for (int32_t i=0; i<garbagesize; i++) {
					seed=randomnumber::generate(seed);
					gbg[i]=randomnumber::scale(seed,0,255);
				}

				stdoutput.printf("%lld: sending %d "
						"bytes of garbage - ",
						threadid,garbagesize);
				stdoutput.safePrint(gbg,
						(garbagesize<=4)?garbagesize:4);
				if (garbagesize>4) {
					stdoutput.write("...");
				}
				stdoutput.write('\n');
				isc.write(gbg,garbagesize);
			}

		} else {
			stdoutput.printf(
				"%lld: %d: connect failed\n",threadid,i);
		}

		isc.close();
		i++;
	}
}

int main(int argc, const char **argv) {

	terminated=false;

	process::setShutDownHandler(shutDown);

	commandline	cmdl(argc,argv);

	if (!cmdl.isFound("host") ||
			!cmdl.isFound("port") ||
			!cmdl.isFound("concount")) {
		stdoutput.printf("usage: stress -host host -port port -socket socket [-user user] [-password password] -concount concount -curcount curcount [-heartbeat|-garbage [size]]\n");
		process::exit(1);
	}

	host=cmdl.getValue("host");
	port=charstring::convertToUnsignedInteger(cmdl.getValue("port"));
	sock=cmdl.getValue("socket");
	user=cmdl.getValue("user");
	password=cmdl.getValue("password");
	concount=charstring::convertToUnsignedInteger(cmdl.getValue("concount"));
	curcount=charstring::convertToUnsignedInteger(cmdl.getValue("curcount"));
	garbage=0;
	if (cmdl.isFound("garbage")) {
		garbage=charstring::convertToUnsignedInteger(cmdl.getValue("garbage"));
		if (garbage==0) {
			garbage=100;
		}
	}
	bool	heartbeat=(garbage || cmdl.isFound("heartbeat"));

	thread	*th=new thread[concount];

	stdoutput.printf("starting %lld threads\n",concount);
	for (int64_t i=0; i<concount; i++) {
		if (!th[i].spawn((heartbeat)?
					((void *(*)(void *))heartbeatTest):
					((void *(*)(void *))queriesTest),
					(void *)i,false)) {
			stdoutput.printf("%lld: failed to start\n",i);
		}
	}

	for (int64_t i=0; i<concount; i++) {
		th[i].wait(NULL);
	}
	delete[] th;
	process::exit(1);
}
