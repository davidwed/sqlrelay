// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <rudiments/randomnumber.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>
#include <rudiments/snooze.h>
#include <rudiments/inetsocketclient.h>


int main(int argc, const char **argv) {

	commandline	cmdl(argc,argv);

	if (!cmdl.found("host") ||
			!cmdl.found("port") ||
			!cmdl.found("colcount") ||
			!cmdl.found("rowcount")) {
		stdoutput.printf("usage: testtable -host host -port port -socket socket [-user user] [-password password] [-table tablename] -colcount count -rowcount count\n");
		process::exit(1);
	}

	const char	*host=cmdl.getValue("host");
	uint16_t	port=charstring::toUnsignedInteger(
					cmdl.getValue("port"));
	const char	*sock=cmdl.getValue("socket");
	const char	*user=cmdl.getValue("user");
	const char	*password=cmdl.getValue("password");
	int32_t		colcount=charstring::toInteger(
					cmdl.getValue("colcount"));
	int32_t		rowcount=charstring::toInteger(
					cmdl.getValue("rowcount"));
	const char	*table=cmdl.getValue("table");
	if (charstring::isNullOrEmpty(table)) {
		table="test";
	}

	sqlrconnection	sqlrcon(host,port,sock,user,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	stringbuffer	query;

	// drop the table (just in case)
	query.append("drop table ")->append(table);

	if (!sqlrcur.sendQuery(query.getString())) {
		// bail if we couldn't connect to the listener
		if (charstring::contains(sqlrcur.errorMessage(),
				"Couldn't connect to the listener.")) {
			process::exit(1);
		}
	}

	// create a table with a random number of fields
	stdoutput.printf("creating table with %d cols\n",colcount);
	query.clear();
	query.append("create table ")->append(table)->append(" (");
	for (int32_t i=0; i<colcount; i++) {
		if (i) {
			query.append(", ");
		}
		query.append("col")->append(i)->append(" int");
	}
	query.append(")");
	if (!sqlrcur.sendQuery(query.getString())) {
		stdoutput.printf("create table - %s\n",sqlrcur.errorMessage());
	}

	// populate it with a random number of rows
	uint32_t	seed=randomnumber::getSeed();
	stdoutput.printf("populating with %d rows\n",rowcount);
	for (int32_t i=0; i<rowcount; i++) {
		query.clear();
		query.append("insert into ");
		query.append(table);
		query.append(" values (");
		for (int32_t j=0; j<colcount; j++) {
			seed=randomnumber::generate(seed);
			if (j) {
				query.append(", ");
			}
			query.append(randomnumber::scale(seed,1,100000));
		}
		query.append(")");
		if (!sqlrcur.sendQuery(query.getString())) {
			stdoutput.printf("insert - %s\n",
					sqlrcur.errorMessage());
		}
	}

	process::exit(0);
}
