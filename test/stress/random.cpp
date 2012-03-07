#include <sqlrelay/sqlrclient.h>
#include <rudiments/randomnumber.h>
#include <stdio.h>

main() {

	sqlrconnection	sqlrcon("localhost",9000,"","test","test",0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	sqlrcon.debugOn();

	stringbuffer	query;
	int32_t		seed=randomnumber::getSeed();
	int32_t		colcount=0;
	int32_t		rowcount=0;
	int32_t		value=0;
	int32_t		times=0;

	for (;;) {

		// create a table with a random number of fields
		seed=randomnumber::generateNumber(seed);
		colcount=randomnumber::scaleNumber(seed,1,15);
		colcount=1;
		query.clear();
		query.append("create temp table test (");
		for (int32_t i=0; i<colcount; i++) {
			if (i) {
				query.append(", ");
			}
			query.append("col")->append(i)->append(" int");
		}
		query.append(")");
		sqlrcur.sendQuery(query.getString());

		// populate it with a random number of rows
		seed=randomnumber::generateNumber(seed);
		rowcount=randomnumber::scaleNumber(seed,1,100);
		rowcount=1;
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
			sqlrcur.sendQuery(query.getString());
		}

		// select those rows a random number of times,
		// use a new cursor for each time
		seed=randomnumber::generateNumber(seed);
		times=randomnumber::scaleNumber(seed,1,20);
		sqlrcursor	**cursors=new sqlrcursor *[times];
		for (int32_t i=0; i<times; i++) {
			cursors[i]=new sqlrcursor(&sqlrcon);
			query.clear();
			query.append("select * from test");
			cursors[i]->sendQuery(query.getString());
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
