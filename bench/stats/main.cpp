// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include <rudiments/commandline.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

#include "bench.h"

#include "sqlrelaybench.h"

bool benchmarks(benchconnection *bcon, benchcursor *bcur,
			uint64_t connections, uint64_t queries) {

	// reconnect some number of times
	for (uint64_t c=0; c<connections; c++) {

		// connect to db
		bcon->connect();

		// run some number of queries per connection
		for (uint64_t q=0; q<queries; q++) {

			// run query
			if (!bcur->selectQuery()) {
				return false;
			}
		}

		// disconnect from the db
		bcon->disconnect();
	}
	return true;
}

int main(int argc, const char **argv) {

	// process the command line
	commandline	cmdl(argc,argv);

	uint64_t	connections=charstring::toInteger(
					cmdl.getValue("connections"));
	uint64_t	queries=charstring::toInteger(
					cmdl.getValue("queries"));
	uint64_t	rows=charstring::toInteger(
					cmdl.getValue("rows"));
	uint64_t	cols=charstring::toInteger(
					cmdl.getValue("cols"));

	// sanity check
	if (!connections || !queries || !rows || !cols) {
		stdoutput.printf(
			"usage: bench \\\n"
			"	-connections [connection-count] \\\n"
			"	-queries [query-per-connection-count] \\\n"
			"	-rows [rows-per-query] \\\n"
			"	-cols [columns-per-query]\n");
		process::exit(1);
	}

	const char	*dbs[]={
		/*"db2",
		"firebird",
		"freetds",
		"mdbtools",
		"mysql",
		"odbc",*/
		"oracle",
		/*"postgresql",
		"sqlite",
		"sybase",*/
		NULL
	};

	// for each database...
	bool	error=false;
	for (const char * const *db=dbs; *db && !error; db++) {

		// first time for the real db, second time for sqlrelay...
		for (uint16_t which=0; which<2 && !error; which++) {

			// init connection and cursor
			benchconnection	*bcon=NULL;
			benchcursor	*bcur=NULL;
			if (!which) {
				stdoutput.printf("benchmarking %s\n",*db);
				if (!charstring::compare(*db,"db2")) {
				} else if (!charstring::compare(
							*db,"firebird")) {
				} else if (!charstring::compare(
							*db,"freetds")) {
				} else if (!charstring::compare(
							*db,"mdbtools")) {
				} else if (!charstring::compare(
							*db,"mysql")) {
				} else if (!charstring::compare(
							*db,"odbc")) {
				} else if (!charstring::compare(
							*db,"oracle")) {
				} else if (!charstring::compare(
							*db,"postgresql")) {
				} else if (!charstring::compare(
							*db,"sqlite")) {
				} else if (!charstring::compare(
							*db,"sybase")) {
				}
continue;
			} else {
				stdoutput.printf("benchmarking "
						"sqlrelay-%s\n",*db);
				bcon=new sqlrelaybenchconnection(
						"host=localhost;port=9000;"
						"socket=/tmp/test.socket;"
						"user=test;password=test",
						*db);
				bcur=new sqlrelaybenchcursor(bcon);
			}

			// set up everything...
			bcon->setRowCount(rows);
			bcon->setColumnCount(cols);
			bcon->buildQueries();
			bcur->createTable();

			// run the benchmarks
			error=benchmarks(bcon,bcur,connections,queries);

			// handle errors
			if (error) {
				stdoutput.printf("error running query\n");
			}

			// clean up
			bcur->dropTable();
			delete bcon;
			delete bcur;
		}
	}

	// exit
	process::exit(0);
}
