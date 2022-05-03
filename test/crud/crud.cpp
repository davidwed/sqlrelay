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

	stdoutput.printf("CREATE TEMPTABLE and TEMPTABLE_IDS: \n");
	checkSuccess(cur->sendQuery("create table testtable (testtable_id int primary key, col1 varchar(128), col2 int, col3 date)"),true);
	checkSuccess(cur->sendQuery("create sequence testtable_ids"),true);
	stdoutput.printf("\n");

	// create (insert)
	stdoutput.printf("CREATE (insert): \n");
	const char	*cols[]={"testtable_id","col1","col2","col3",NULL};
	const char	*vals1[]={"","val1","1","01-JAN-2000",NULL};
	const char	*vals2[]={"","val2","2","02-FEB-2000",NULL};
	const char	*vals3[]={"","val3","3","03-MAR-2000",NULL};
	const char	*vals4[]={"","val4","4","04-APR-2000",NULL};
	const char	*vals5[]={"","val5","5","05-MAY-2000",NULL};
	checkSuccess(crud->doCreate(cols,vals1),true);
	checkSuccess(crud->doCreate(cols,vals2),true);
	checkSuccess(crud->doCreate(cols,vals3),true);
	checkSuccess(crud->doCreate(cols,vals4),true);
	checkSuccess(crud->doCreate(cols,vals5),true);
	stdoutput.printf("\n");

	// read (select)
	stdoutput.printf("READ (select): \n");
	const char	*criteria=
	"{\n"
	"	\"field\": \"col1\",\n"
	"	\"operator\": \"=\",\n"
	"	\"value\": \"val11\",\n"
	"	\"boolean\": \"and\",\n"
	"	\"field\": \"col2\",\n"
	"	\"operator\": \"=\",\n"
	"	\"value\": \"val12\",\n"
	"	\"boolean\": \"and\",\n"
	"	\"field\": \"col3\",\n"
	"	\"operator\": \"=\",\n"
	"	\"value\": \"val13\"\n"
	"}\n";
	const char	*sort=
	"{\n"
	"	\"field\": \"col1\",\n"
	"	\"field\": \"col2\",\n"
	"	\"order\": \"asc\",\n"
	"	\"field\": \"col3\",\n"
	"	\"order\": \"desc\"\n"
	"}\n";
	checkSuccess(crud->doRead(criteria,sort,0),true);
	stdoutput.printf("\n");

	// drop table and sequence
	cur->sendQuery("drop table testtable");
	cur->sendQuery("drop sequence testtable_ids");

	// clean up
	delete cur;
	delete con;

	return 0;
}
