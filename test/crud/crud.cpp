// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrcrud.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrcrud	*crud;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("%s!=%s\n",value,success);
			stdoutput.printf("failure\n");
			stdoutput.printf("%lld: %s\n",
					crud->getErrorCode(),
					crud->getErrorMessage());
			delete crud;
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%s!=%s\n",value,success);
		stdoutput.printf("failure\n");
		stdoutput.printf("%lld: %s\n",
				crud->getErrorCode(),
				crud->getErrorMessage());
		delete crud;
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(const char *value, const char *success, size_t length) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("%s!=%s\n",value,success);
			stdoutput.printf("failure\n");
			stdoutput.printf("%lld: %s\n",
					crud->getErrorCode(),
					crud->getErrorMessage());
			delete crud;
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success,length)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%s!=%s\n",value,success);
		stdoutput.printf("failure\n");
		stdoutput.printf("%lld: %s\n",
				crud->getErrorCode(),
				crud->getErrorMessage());
		delete crud;
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%d!=%d\n",value,success);
		stdoutput.printf("failure\n");
		stdoutput.printf("%lld: %s\n",
				crud->getErrorCode(),
				crud->getErrorMessage());
		delete crud;
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(double value, double success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%f!=%f\n",value,success);
		stdoutput.printf("failure\n");
		stdoutput.printf("%lld: %s\n",
				crud->getErrorCode(),
				crud->getErrorMessage());
		delete crud;
		delete cur;
		delete con;
		process::exit(1);
	}
}

int main(int argc, char **argv) {

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);
	crud=new sqlrcrud;
	crud->setSqlrConnection(con);
	crud->setSqlrCursor(cur);
	crud->setTable("testtable");
	crud->buildQueries();

	// drop existing table and sequence
	cur->sendQuery("drop table testtable");
	cur->sendQuery("drop sequence testtable_ids");

	stdoutput.printf("CREATE TESTTABLE and TESTTABLE_IDS: \n");
	checkSuccess(cur->sendQuery("create table testtable "
					"(testtable_id int primary key, "
					"col1 varchar(128), "
					"col2 int, "
					"col3 date)"),true);
	checkSuccess(cur->sendQuery("create sequence testtable_ids"),true);
	stdoutput.printf("\n");

	// set up columns and values
	const char	*cols[]={
		"testtable_id","col1","col2","col3",NULL
	};
	const char	*vals[][4]={
		{"val1","1","01-JAN-2000",NULL},
		{"val2","2","02-JAN-2000",NULL},
		{"val3","3","03-JAN-2000",NULL},
		{"val4","4","04-JAN-2000",NULL},
		{"val5","5","05-JAN-2000",NULL},
	};

	// create (insert)
	stdoutput.printf("CREATE (insert): \n");
	for (uint16_t i=0; i<5; i++) {
		const char	**v=new const char *[5];
		v[0]="";
		v[1]=vals[i][0];
		v[2]=vals[i][1];
		v[3]=vals[i][2];
		v[4]=NULL;
		checkSuccess(crud->doCreate(cols,v),true);
		delete[] v;
	}
	stdoutput.printf("\n");

	// read (select)
	stdoutput.printf("READ (select): \n");
	stringbuffer	criteria;
	stringbuffer	sort;
	for (uint16_t i=0; i<5; i++) {

		// for each iteration, select one row:
		//	1) row 0
		// 	2) row 1
		// 	3) row 2
		// 	etc.

		for (uint16_t j=1; j<4; j++) {

			// for each row, run three selects, each of which
			// contains more columns in the where/sort clause:
			// 	1) col1=val1
			// 	2) col1=val1 and col2=1
			// 	3) col1=val1 and col2=1 and col3=01-JAN-2000
			// 	etc.


			// build criteria/sort json
			criteria.append("{\n");
			sort.append("{\n");
			for (uint16_t k=1; k<=j; k++) {
				criteria.appendFormatted(
					"	\"field\": \"%s\",\n"
					"	\"operator\": \"=\",\n"
					"	\"value\": \"%s\"",
					cols[k],vals[i][k-1]);
				sort.appendFormatted(
					"	\"field\": \"%s\",\n"
					"	\"order\": \"asc\"",
					cols[k]);
				if (k<j) {
					criteria.append(",\n"
					"	\"boolean\": \"and\",\n");
					sort.append(",\n");
				} else {
					criteria.append('\n');
					sort.append('\n');
				}
			}
			criteria.append("}\n");
			sort.append("}\n");

			// run the query
			checkSuccess(crud->doRead(criteria.getString(),
						sort.getString(),0),true);

			// check col/row counts
			checkSuccess(cur->colCount(),4);
			checkSuccess(cur->rowCount(),1);

			// check results
			for (uint16_t k=1; k<j; k++) {
				checkSuccess(!charstring::compare(
							cur->getField(0,k),
							vals[i][k-1]),true);
			}
			stdoutput.printf("\n");

			// clean up
			criteria.clear();
			sort.clear();
		}
	}
	stdoutput.printf("\n");

	// update
	// replace each field with a null, one by one
	stdoutput.printf("UPDATE: \n");
	criteria.clear();
	const char	**c=new const char *[4];
	c[0]=cols[1];
	c[1]=cols[2];
	c[2]=cols[3];
	c[3]=NULL;
	const char	**v=new const char *[4];
	for (uint16_t i=0; i<5; i++) {

		// build criteria:
		// 	1) testtable_id=1
		// 	2) testtable_id=2
		// 	3) testtable_id=3
		// 	etc.
		criteria.appendFormatted(
			"{\n"
			"	\"field\": \"testtable_id\",\n"
			"	\"operator\": \"=\",\n"
			"	\"value\": \"%d\"\n"
			"}\n",
			i+1);

		for (uint16_t j=1; j<=3; j++) {

			// for each row, run 3 updates, each of which replaces
			// an additional column with '0':
			// 	1) col1=null, col2=...value..., col3=...value...
			// 	2) col1=null, col2=null, col3=...value...
			// 	3) col1=null, col2=null, col3=null
			// 	etc.
			v[0]=(j>0)?"0":vals[i][0];
			v[1]=(j>1)?"0":vals[i][1];
			v[2]=(j>2)?"12-DEC-2000":vals[i][2];
			v[3]=NULL;

			// run the query
			checkSuccess(crud->doUpdate(
					c,v,criteria.getString()),true);

			// check affected rows
			checkSuccess(cur->affectedRows(),1);
			stdoutput.printf("\n");

			// validate updates to the row
			checkSuccess(crud->doRead(
					criteria.getString(),NULL,0),true);
			checkSuccess(cur->colCount(),4);
			checkSuccess(cur->rowCount(),1);
			stdoutput.printf("\n");
			for (uint16_t k=1; k<=3; k++) {
				checkSuccess(cur->getField(0,k),v[k-1]);
			}
			stdoutput.printf("\n");
		}

		// clean up
		criteria.clear();
	}

	// clean up
	delete[] c;
	delete[] v;

	// delete

	// drop table and sequence
	cur->sendQuery("drop table testtable");
	cur->sendQuery("drop sequence testtable_ids");

	// clean up
	delete cur;
	delete con;

	return 0;
}
