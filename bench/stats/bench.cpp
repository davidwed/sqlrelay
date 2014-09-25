// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include <rudiments/stringbuffer.h>

#include "bench.h"

benchmarks::benchmarks(const char *connectstring,
				const char *db,
				uint64_t cons,
				uint64_t queries,
				uint64_t rows,
				uint32_t cols,
				uint32_t colsize,
				bool debug) {

	this->connectstring=connectstring;
	this->db=db;
	this->cons=cons;
	this->queries=queries;
	this->rows=rows;
	this->cols=cols;
	this->colsize=colsize;
	this->debug=debug;
	this->con=NULL;
	this->cur=NULL;

	rnd.setSeed(randomnumber::getSeed());

	// create query
	stringbuffer	createquerystr;
	createquerystr.append("create table testtable (");
	createquerystr.append("keycol int");
	for (uint32_t i=0; i<cols; i++) {
		createquerystr.append(",");
		createquerystr.append("col")->append(i)->append(" ");
		if (!charstring::compare(db,"oracle")) {
			createquerystr.append("varchar2");
		}
		createquerystr.append("(")->append(colsize)->append(")");
	}
	createquerystr.append(")");
	createquery=createquerystr.detachString();

	// drop query
	dropquery=charstring::duplicate(
			"drop table testtable");

	// insert query
	stringbuffer	insertquerystr;
	insertquerystr.append("insert into testtable values (%lld");
	for (uint32_t i=0; i<cols; i++) {
		insertquerystr.append(",'");
		appendRandomString(&insertquerystr);
		insertquerystr.append("'");
	}
	insertquerystr.append(")");
	insertquery=insertquerystr.detachString();

	// update query
	stringbuffer	updatequerystr;
	updatequerystr.append("update testtable set ");
	for (uint32_t i=0; i<cols; i++) {
		if (i) {
			updatequerystr.append(",");
		}
		updatequerystr.append("col")->append(i);
		updatequerystr.append("='");
		appendRandomString(&updatequerystr);
		updatequerystr.append("'");
	}
	updatequerystr.append(" where keycol=%lld");
	updatequery=updatequerystr.detachString();

	// select query
	selectquery=charstring::duplicate("select * from testtable");

	// delete query
	stringbuffer	deletequerystr;
	deletequerystr.append("delete from testtable where keycol=%lld");
	deletequery=deletequerystr.detachString();
}

benchmarks::~benchmarks() {
	delete[] createquery;
	delete[] dropquery;
	delete[] insertquery;
	delete[] updatequery;
	delete[] selectquery;
	delete[] deletequery;
	delete cur;
	delete con;
}

void benchmarks::run() {

	// connect
	if (debug) {
		stdoutput.printf("connecting\n");
	}
	if (!con->connect()) {
		stdoutput.printf("error connecting\n");
	}

	// create the table
	if (debug) {
		stdoutput.printf("creating table:\n%s\n",createquery);
	}
	if (!cur->query(createquery)) {
		stdoutput.printf("error creating table\n");
	}

	// insert rows
	if (debug) {
		stdoutput.printf("inserting %lld rows:\n%s\n",rows,insertquery);
	}
	for (uint64_t i=0; i<rows; i++) {
		if (debug) {
			stdoutput.printf("  row %lld\n",i);
		}
		char	*newq=buildQuery(insertquery,i);
		bool	result=cur->query(newq);
		delete[] newq;
		if (!result) {
			stdoutput.printf("error inserting rows\n");
		}
	}

	// update rows
	if (debug) {
		stdoutput.printf("updating %lld rows:\n%s\n",rows,updatequery);
	}
	for (uint64_t i=0; i<rows; i++) {
		if (debug) {
			stdoutput.printf("  row %lld\n",i);
		}
		char	*newq=buildQuery(updatequery,i);
		bool	result=cur->query(newq);
		delete[] newq;
		if (!result) {
			stdoutput.printf("error updating rows\n");
		}
	}

	// disconnect
	if (!con->disconnect()) {
		stdoutput.printf("error disconnecting\n");
	}

	// select rows
	if (debug) {
		stdoutput.printf("selecting %lld rows, %ld columns:\n%s\n",
							rows,cols,selectquery);
	}
	for (uint64_t i=0; i<cons; i++) {
		if (debug) {
			stdoutput.printf("  connection %lld\n",i);
		}
		if (!con->connect()) {
			stdoutput.printf("error connecting\n");
		}
		for (uint64_t j=0; j<queries; j++) {
			if (debug) {
				stdoutput.printf("    query %lld\n",j);
			}
			if (!cur->query(selectquery)) {
				stdoutput.printf("error selecting rows\n");
			}
		}
		if (!con->disconnect()) {
			stdoutput.printf("error disconnecting\n");
		}
	}

	// re-connect
	if (!con->connect()) {
		stdoutput.printf("error disconnecting\n");
	}

	// delete rows
	if (debug) {
		stdoutput.printf("deleting %lld rows:\n%s\n",rows,deletequery);
	}
	for (uint64_t i=0; i<rows; i++) {
		if (debug) {
			stdoutput.printf("  row %lld\n",i);
		}
		char	*newq=buildQuery(deletequery,i);
		bool	result=cur->query(newq);
		delete[] newq;
		if (!result) {
			stdoutput.printf("error deleting rows\n");
		}
	}

	// drop the table
	if (debug) {
		stdoutput.printf("dropping table:\n%s\n",dropquery);
	}
	if (!cur->query(dropquery)) {
		stdoutput.printf("error dropping table\n");
	}

	// disconnect
	if (debug) {
		stdoutput.printf("disconnecting\n");
	}
	if (!con->disconnect()) {
		stdoutput.printf("error disconnecting\n");
	}
}

void benchmarks::appendRandomString(stringbuffer *str) {
	for (uint32_t j=0; j<colsize; j++) {
		int32_t	result;
		rnd.generateScaledNumber('a','z',&result);
		str->append((char)result);
	}
}

char *benchmarks::buildQuery(const char *query, uint64_t key) {

	// build the query (inserting the key)
	const char	*q=query;
	size_t	newqlen=charstring::length(q)+22;
	char	*newq=new char[newqlen];
	charstring::printf(newq,newqlen,q,key);
	return newq;
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
