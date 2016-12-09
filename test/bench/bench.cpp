// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include <rudiments/datetime.h>

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

	shutdown=false;
}

benchmarks::~benchmarks() {
	delete cur;
	delete con;
}

void benchmarks::shutDown() {
	stdoutput.printf("shutting down, please wait...\n");
	shutdown=true;
}

bool benchmarks::run(dictionary< float, linkedlist< float > *> *stats) {

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
	stdoutput.printf("  dropping table...\n");
	const char	*dropquery="drop table testtable";
	if (debug) {
		stdoutput.printf("%s\n",dropquery);
	}
	cur->query(dropquery,false);

	// handle shutdown
	if (shutdown) {
		return false;
	}

	const char	*selectquery="select * from testtable";

	// create
	stdoutput.printf("  creating table with %d columns...\n",cols);
	char	*createquery=createQuery(cols,colsize);
	if (debug) {
		stdoutput.printf("%s\n",createquery);
	}
	if (!cur->query(createquery,false)) {
		stdoutput.printf("error creating table\n");
	}
	delete[] createquery;

	// insert
	stdoutput.printf("  inserting %lld rows...\n",rows);
	for (uint64_t i=0; i<rows && !shutdown; i++) {
		char	*insertquery=
			insertQuery(cols,colsize);
		if (debug) {
			stdoutput.printf("  row %lld\n%s\n",i,insertquery);
		}
		bool	result=cur->query(insertquery,false);
		if (!result) {
			stdoutput.printf("error inserting rows\n");
			shutdown=true;
		}
		delete[] insertquery;
	}

	// select
	stdoutput.printf("  selecting...\n",rows,cols);
	if (debug) {
		stdoutput.printf("%s\n",rows,cols,selectquery);
	}
	if (!shutdown) {
		benchSelect(selectquery,queries,rows,cols,
					colsize,iterations,stats);
	}

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

	// drop
	stdoutput.printf("  dropping table...\n");
	if (debug) {
		stdoutput.printf("%s\n",dropquery);
	}
	if (!cur->query(dropquery,false)) {
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

	return !shutdown;
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
		} else {
			createquerystr.append("varchar");
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

void benchmarks::benchSelect(
			const char *selectquery,
			uint64_t queries, uint64_t rows,
			uint32_t cols, uint32_t colsize,
			uint16_t iterations,
			dictionary< float, linkedlist< float > *> *stats) {

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

	// handle shutdown
	if (shutdown) {
		return;
	}

	// display stats
	stdoutput.printf("\nqueries rows cols colsize iterations\n");
	stdoutput.printf("   % 4lld % 4lld % 4d    % 4d       % 4d\n",
					queries,rows,cols,colsize,iterations);
	stdoutput.printf("queries-per-cx  queries-per-second\n");

	// run selects
	for (uint64_t qcount=1; qcount<=queries && !shutdown; qcount++) {

		// get start time
		datetime	start;
		start.getSystemDateAndTime();

		// run all of this some number of times and average the results
		for (uint16_t iter=0; iter<iterations && !shutdown; iter++) {

			for (uint64_t i=0; i<iterations && !shutdown; i++) {

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
				for (uint64_t j=0; j<qcount && !shutdown; j++) {

					if (debug) {
						stdoutput.printf(
							"    query %lld\n",j);
					}
					if (!cur->query(selectquery,true)) {
						stdoutput.printf(
						"error selecting rows\n");
						shutdown=true;
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

			// progress
			if (iterations>=100) {
				if (!((iter+1)%(iterations/10))) {
					if (iter/(iterations/10)) {
						stdoutput.printf(".");
					}
					stdoutput.printf("%d",
						(iter+1)/(iterations/10));
					stdoutput.flush();
					if (iter+1==iterations) {
						stdoutput.printf("\n");
					}
				}
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

		// total seconds
		float	totalsec=(float)sec+(((float)usec)/1000000.0);

		// average number of seconds per query
		float	spq=totalsec/(iterations*iterations*qcount);

		// average number of queries per second
		float	qps=1.0/spq;
		
		// calculate queries per connection
		float	qpc=(float)qcount;

		// display stats
		stdoutput.printf("          % 4lld             % 4.2f\n",
								qcount,qps);

		// update stats
		linkedlist< float >	*d=stats->getValue(qpc);
		if (!d) {
			d=new linkedlist< float >();
			stats->setValue(qpc,d);
		}
		d->append(qps);
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
