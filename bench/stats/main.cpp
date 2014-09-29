// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include <rudiments/commandline.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
#include <rudiments/signalclasses.h>

#include "bench.h"

#include "oraclebench.h"
#include "mysqlbench.h"
#include "sqlrelaybench.h"

#define ORACLE_SID "(DESCRIPTION = (ADDRESS = (PROTOCOL = TCP)(HOST = db64.firstworks.com)(PORT = 1521)) (CONNECT_DATA = (SERVER = DEDICATED) (SERVICE_NAME = ora1)))"

benchmarks	*bm;

void shutDown(int32_t signum) {
	if (bm) {
		bm->shutDown();
	}
}

int main(int argc, const char **argv) {

	// process the command line
	commandline	cmdl(argc,argv);

	// usage info
	if (cmdl.found("help") || cmdl.found("h")) {
		stdoutput.printf(
			"usage: sqlr-bench \\\n"
			"	-db [db] \\\n"
			"	-queries [total-query-count] \\\n"
			"	-rows [rows-per-query] \\\n"
			"	-cols [columns-per-row] \\\n"
			"	-colsize [characters-per-column] \\\n"
			"	-iterations [iterations-per-test] \\\n"
			"	-dbonly|-sqlrelayonly \\n"
			"	-debug\n");
		process::exit(1);
	}

	// default parameters
	const char	*db="oracle";
	uint64_t	queries=20;
	uint64_t	rows=256;
	uint32_t	cols=16;
	uint32_t	colsize=32;
	uint16_t	iterations=10;
	bool		dbonly=false;
	bool		sqlrelayonly=false;
	bool		debug=false;

	// override defaults with command line parameters
	if (cmdl.found("db")) {
		db=cmdl.getValue("db");
	}
	if (cmdl.found("queries")) {
		queries=charstring::toInteger(cmdl.getValue("queries"));
	}
	if (cmdl.found("rows")) {
		rows=charstring::toInteger(cmdl.getValue("rows"));
	}
	if (cmdl.found("cols")) {
		cols=charstring::toInteger(cmdl.getValue("cols"));
	}
	if (cmdl.found("colsize")) {
		colsize=charstring::toInteger(cmdl.getValue("colsize"));
	}
	if (cmdl.found("iterations")) {
		iterations=charstring::toInteger(cmdl.getValue("iterations"));
	}
	if (cmdl.found("dbonly")) {
		dbonly=true;
	}
	if (cmdl.found("sqlrelayonly")) {
		sqlrelayonly=true;
	}
	if (cmdl.found("debug")) {
		debug=true;
	}

	// handle signals
	bm=NULL;
	process::handleShutDown(shutDown);
	process::handleCrash(shutDown);

	// for each database...
	bool	error=false;

	// first time for the real db, second time for sqlrelay...
	uint16_t	start=(sqlrelayonly)?1:0;
	uint16_t	end=(dbonly)?1:2;
	for (uint16_t which=start; which<end && !error; which++) {

		if (!which) {
			stdoutput.printf("benchmarking %s\n",db);
		} else {
			stdoutput.printf("benchmarking sqlrelay-%s\n",db);
		}

		// init benchmarks
		delete bm;
		if (which) {
			bm=new sqlrelaybenchmarks(
					"host=localhost;port=9000;"
					"socket=/tmp/test.socket;"
					"user=test;password=test;"
					"debug=no",
					db,queries,rows,
					cols,colsize,iterations,debug);
		} else if (!charstring::compare(db,"db2")) {
		} else if (!charstring::compare(db,"firebird")) {
		} else if (!charstring::compare(db,"freetds")) {
		} else if (!charstring::compare(db,"mdbtools")) {
		} else if (!charstring::compare(db,"mysql")) {
			bm=new mysqlbenchmarks(
					"host=db64;db=testdb;"
					"user=testuser;password=testpassword;",
					db,queries,rows,
					cols,colsize,iterations,debug);
		} else if (!charstring::compare(db,"odbc")) {
		} else if (!charstring::compare(db,"oracle")) {
			bm=new oraclebenchmarks(
					"sid="ORACLE_SID";"
					"user=testuser;password=testpassword;",
					db,queries,rows,
					cols,colsize,iterations,debug);
		} else if (!charstring::compare(db,"postgresql")) {
		} else if (!charstring::compare(db,"sqlite")) {
		} else if (!charstring::compare(db,"sybase")) {
		}
		if (!bm) {
			stdoutput.printf("error creating benchmarks\n");
			continue;
		}

		// run the benchmarks
		bm->run();
	}

	// exit
	process::exit(0);
}
