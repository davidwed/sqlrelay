// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include <rudiments/datetime.h>

// for pow()
#include <math.h>
// for fflush and stdout
#include <stdio.h>

#include "bench.h"

benchmarks::benchmarks(const char *connectstring,
				const char *db,
				uint64_t queries,
				uint64_t rows,
				uint32_t cols,
				uint32_t colsize,
				uint16_t iterations,
				bool debug) {

	this->connectstring=connectstring;
	this->db=db;
	this->queries=queries;
	this->rows=rows;
	this->cols=cols;
	this->colsize=colsize;
	this->iterations=iterations;
	this->debug=debug;
	this->con=NULL;
	this->cur=NULL;

	rnd.setSeed(randomnumber::getSeed());
}

benchmarks::~benchmarks() {
	delete cur;
	delete con;
}

void benchmarks::run() {

	// connect and open
	if (debug) {
		stdoutput.printf("connecting\n");
	}
	if (!con->connect()) {
		stdoutput.printf("error connecting\n");
	}
	if (debug) {
		stdoutput.printf("opening\n");
	}
	if (!cur->open()) {
		stdoutput.printf("error opening\n");
	}


	// drop table (just in case)
	const char	*dropquery="drop table testtable";
	cur->query(dropquery);


	// create
	char	*createquery=createQuery(cols,colsize);
	if (debug) {
		stdoutput.printf("creating table:\n%s\n",createquery);
	}
	if (!cur->query(createquery)) {
		stdoutput.printf("error creating table\n");
	}
	delete[] createquery;


	// insert
	if (debug) {
		stdoutput.printf("inserting %lld rows:\n",rows);
	}
	for (uint64_t i=0; i<rows; i++) {
		char	*insertquery=insertQuery(cols,colsize);
		if (debug) {
			stdoutput.printf("  row %lld\n%s\n",i,insertquery);
		}
		bool	result=cur->query(insertquery);
		if (!result) {
			stdoutput.printf("error inserting rows\n");
		}
		delete[] insertquery;
	}

	// select
	const char	*selectquery="select * from testtable";
	uint32_t	colfactor=4;
	uint32_t	colcount=pow(2,colfactor);
	while (colcount<=cols) {

		uint32_t	rowfactor=6;
		uint32_t	rowcount=pow(2,rowfactor);
		while (rowcount<=rows) {

			if (debug) {
				stdoutput.printf("selecting %lld rows, "
						"%ld columns:\n%s\n",
						rowcount,colcount,selectquery);
			}
			benchSelect(selectquery,queries,
					rowcount,colcount,colsize,
					iterations);

			// 1, 2, 4, 8, 16...rows
			if (rowcount==rows) {
				rowcount++;
			} else {
				rowfactor++;
				rowcount=pow(2,rowfactor);
				if (rowcount>rows) {
					rowcount=rows;
				}
			}
		}

		// 1, 2, 4, 8, 16...cols
		if (colcount==cols) {
			colcount++;
		} else {
			colfactor++;
			colcount=pow(2,colfactor);
			if (colcount>cols) {
				colcount=cols;
			}
		}
	}


	// re-connect
	if (debug) {
		stdoutput.printf("re-connecting\n");
	}
	if (!con->connect()) {
		stdoutput.printf("error connecting\n");
	}
	if (debug) {
		stdoutput.printf("re-opening\n");
	}
	if (!cur->open()) {
		stdoutput.printf("error opening\n");
	}


	// drop
	if (debug) {
		stdoutput.printf("dropping table:\n%s\n",dropquery);
	}
	if (!cur->query(dropquery)) {
		stdoutput.printf("error dropping table\n");
	}


	// close and disconnect
	if (debug) {
		stdoutput.printf("closing\n");
	}
	if (!cur->close()) {
		stdoutput.printf("error closing\n");
	}
	if (debug) {
		stdoutput.printf("disconnecting\n");
	}
	if (!con->disconnect()) {
		stdoutput.printf("error disconnecting\n");
	}
}

char *benchmarks::createQuery(uint32_t cols, uint32_t colsize) {

	stringbuffer	createquerystr;
	createquerystr.append("create table testtable (");
	for (uint32_t i=0; i<cols; i++) {
		if (i) {
			createquerystr.append(",");
		}
		createquerystr.append("col")->append(i)->append(" ");
		if (!charstring::compare(db,"oracle")) {
			createquerystr.append("varchar2");
		}
		createquerystr.append("(")->append(colsize)->append(")");
	}
	createquerystr.append(")");
	return createquerystr.detachString();
}

char *benchmarks::insertQuery(uint32_t cols, uint32_t colsize) {
	stringbuffer	insertquerystr;
	insertquerystr.append("insert into testtable values ('");
	for (uint32_t i=0; i<cols; i++) {
		if (i) {
			insertquerystr.append(",'");
		}
		appendRandomString(&insertquerystr,colsize);
		insertquerystr.append("'");
	}
	insertquerystr.append(")");
	return insertquerystr.detachString();
}

void benchmarks::appendRandomString(stringbuffer *str, uint32_t colsize) {
	for (uint32_t j=0; j<colsize; j++) {
		int32_t	result;
		rnd.generateScaledNumber('a','z',&result);
		str->append((char)result);
	}
}

void benchmarks::benchSelect(const char *selectquery,
				uint64_t queries, uint64_t rows,
				uint32_t cols, uint32_t colsize,
				uint16_t iterations) {

	// close and disconnect
	if (debug) {
		stdoutput.printf("closing\n");
	}
	if (!cur->close()) {
		stdoutput.printf("error closing\n");
	}
	if (debug) {
		stdoutput.printf("disconnecting\n");
	}
	if (!con->disconnect()) {
		stdoutput.printf("error disconnecting\n");
	}

	// display stats
	stdoutput.printf("\nqueries rows cols colsize\n");
	stdoutput.printf("   % 4lld % 4lld % 4d    % 4d\n",
					queries,rows,cols,colsize);
	stdoutput.printf("connections    queries-per-cx     "
				"seconds  queries-per-second\n");

	// run selects
	for (uint64_t concount=1; concount<=queries; concount++) {

		// for this set, figure out how many connections to run
		// and how many queries to run per connection
		uint64_t	actualconcount=concount;
		uint64_t	queriespercon=queries/actualconcount;
		if (queries%actualconcount) {
			actualconcount++;
		}

		// run all of this some number of times and average the results
		float		avgsec=0;
		for (uint16_t iter=0; iter<iterations; iter++) {

			// keep track of how many queries we've actually run
			uint64_t	queriesrun=0;

			// get start time
			datetime	start;
			start.getSystemDateAndTime();

			for (uint64_t i=0; i<actualconcount; i++) {

				// connect and open
				if (debug) {
					stdoutput.printf(
						"  connection %lld\n",i);
				}
				if (debug) {
					stdoutput.printf("connecting\n");
				}
				if (!con->connect()) {
					stdoutput.printf("error connecting\n");
				}
				if (debug) {
					stdoutput.printf("opening\n");
				}
				if (!cur->open()) {
					stdoutput.printf("error opening\n");
				}

				// run some number of queries per connection
				uint64_t	queriestorun=
						queriesrun+queriespercon;
				if (queriestorun>queries) {
					queriestorun=queries;
				}
				for (uint64_t j=queriesrun;
						j<queriestorun; j++) {

					if (debug) {
						stdoutput.printf(
							"    query %lld\n",j);
					}
					if (!cur->query(selectquery)) {
						stdoutput.printf(
						"error selecting rows\n");
					}
				}

				// close and disconnect
				if (debug) {
					stdoutput.printf("closing\n");
				}
				if (!cur->close()) {
					stdoutput.printf("error closing\n");
				}
				if (debug) {
					stdoutput.printf("disconnecting\n");
				}
				if (!con->disconnect()) {
					stdoutput.printf(
						"error disconnecting\n");
				}
			}

			// get end time
			datetime	end;
			end.getSystemDateAndTime();

			// calculate total time
			uint32_t	sec=end.getEpoch()-start.getEpoch();
			int32_t		usec=end.getMicroseconds()-
						start.getMicroseconds();
 			if (usec<0) {
				sec--;
				usec=usec+1000000;
			}

			// tally seconds
			avgsec=avgsec+(float)sec+(((float)usec)/1000000.0);

			// progress
			if (iterations>=100) {
				if (!((iter+1)%(iterations/10))) {
					if (iter/(iterations/10)) {
						stdoutput.printf(".");
					}
					stdoutput.printf("%d",
						(iter+1)/(iterations/10));
					fflush(stdout);
					if (iter+1==iterations) {
						stdoutput.printf("\n");
					}
				}
			}
		}
		
		// average seconds
		avgsec=avgsec/iterations;

		// calcualate queries per second
		float	qps=((float)queries)/avgsec;

		// display stats
		stdoutput.printf(" % 4lld(% 4lld) % 8.2f(% 4lld+% 2lld)"
					" % 4.6f % 4.2f\n",
					concount,actualconcount,
					(float)queries/(float)concount,
					queries/concount,
					queries%concount,
					avgsec,qps);
	}
}

benchconnection::benchconnection(const char *connectstring,
						const char *db) {
	pstring.parse(connectstring);
	this->db=db;
}

benchconnection::~benchconnection() {
}

const char *benchconnection::getParam(const char *param) {
	return pstring.getValue(param);
}

benchcursor::benchcursor(benchconnection *bcon) {
	this->bcon=bcon;
}

benchcursor::~benchcursor() {
}

bool benchcursor::open() {
	return true;
}

bool benchcursor::close() {
	return true;
}
