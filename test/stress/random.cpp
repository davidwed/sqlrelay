#include <sqlrelay/sqlrclient.h>
#include <rudiments/randomnumber.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

const char	*host;
uint16_t	port;
const  char	*sock;
const char	*login;
const char	*password;

int main(int argc, char **argv) {

	//host="sqlrserver";
	host="192.168.123.13";
	port=9000;
	sock="/tmp/test.socket";
	login="test";
	password="test";

	sqlrconnection	sqlrcon(host,port,sock,login,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	//sqlrcon.debugOn();

	stringbuffer	query;
	uint32_t	seed=randomnumber::getSeed();
	int32_t		colcount=0;
	int32_t		rowcount=0;
	int32_t		value=0;
	int32_t		times=0;

	// drop the table (just in case)
	query.clear();
	query.append("drop table test");
	sqlrcur.sendQuery(query.getString());

	for (;;) {

		// create a table with a random number of fields
		seed=randomnumber::generateNumber(seed);
		colcount=randomnumber::scaleNumber(seed,1,15);
		stdoutput.printf("creating table with %d cols\n",colcount);
		query.clear();
		query.append("create table test (");
		for (int32_t i=0; i<colcount; i++) {
			if (i) {
				query.append(", ");
			}
			query.append("col")->append(i)->append(" int");
		}
		query.append(")");
		if (!sqlrcur.sendQuery(query.getString())) {
			stdoutput.printf("%s\n",sqlrcur.errorMessage());
			process::exit(1);
		}

		// populate it with a random number of rows
		seed=randomnumber::generateNumber(seed);
		rowcount=randomnumber::scaleNumber(seed,1,100);
		stdoutput.printf("populating with %d rows\n",rowcount);
		for (int32_t i=0; i<rowcount; i++) {
			seed=randomnumber::generateNumber(seed);
			value=randomnumber::scaleNumber(seed,1,100000);
			query.clear();
			query.append("insert into test values (");
			for (int32_t j=0; j<colcount; j++) {
				if (j) {
					query.append(", ");
				}
				query.append(value);
			}
			query.append(")");
			if (!sqlrcur.sendQuery(query.getString())) {
				stdoutput.printf("%s\n",sqlrcur.errorMessage());
				process::exit(1);
			}
		}

		// select those rows a random number of times,
		// use a new cursor for each time
		seed=randomnumber::generateNumber(seed);
		times=randomnumber::scaleNumber(seed,1,4);
		stdoutput.printf("selecting %d times\n",times);
		sqlrcursor	**cursors=new sqlrcursor *[times];
		for (int32_t i=0; i<times; i++) {
			cursors[i]=new sqlrcursor(&sqlrcon);
			query.clear();
			query.append("select * from test");
			if (!cursors[i]->sendQuery(query.getString())) {
				stdoutput.printf("%s\n",
						cursors[i]->errorMessage());
				stdoutput.printf(
					"NOTE: this test requires 5 cursors\n");
				process::exit(1);
			}
		}
		for (int32_t i=0; i<times; i++) {
			delete cursors[i];
		}
		delete cursors;

		// drop the table
		query.clear();
		query.append("drop table test");
		sqlrcur.sendQuery(query.getString());
	}
}
