// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("%s!=%s\n",value,success);
			stdoutput.printf("failure: %s\n",cur->errorMessage());
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%s!=%s\n",value,success);
		stdoutput.printf("failure: %s\n",cur->errorMessage());
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
		stdoutput.printf("failure: %s\n",cur->errorMessage());
		delete cur;
		delete con;
		process::exit(1);
	}
}

// utf-8
const unsigned char yo8[]={'y','o','\0'};
const unsigned char grin8[]={0xF0,0x9F,0x98,0x84,'\0'};
const unsigned char horn8[]={0xF0,0x9F,0x91,0xBF,'\0'};
const unsigned char cool8[]={0xF0,0x9F,0x98,0x8E,'\0'};
const unsigned char *emoji8[]={yo8,grin8,horn8,cool8,NULL};

// utf-16
const unsigned char yo16[]={'y','\0','o','\0','\0','\0'};
const unsigned char grin16[]={0x3D,0xD8,0x04,0xDE,'\0','\0'};
const unsigned char horn16[]={0x3D,0xD8,0x7F,0xDC,'\0','\0'};
const unsigned char cool16[]={0x3D,0xD8,0x0E,0xDE,'\0','\0'};
const unsigned char *emoji16[]={yo16,grin16,horn16,cool16,NULL};


int	main(int argc, char **argv) {

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);

	cur->sendQuery("drop table testtable");

	stdoutput.printf("CREATE TEMPTABLE: \n");
	checkSuccess(cur->sendQuery("create table testtable (i int identity, emojidirect nvarchar(64), emojifrombase64 nvarchar(64), base64 varchar(64))"),1);
	stdoutput.printf("\n");
	stdoutput.printf("\n");


	stdoutput.printf("INSERT: \n");
	cur->prepareQuery("insert into testtable values (:1,null,:2)");
	const unsigned char **e16=emoji16;
	const unsigned char **e8=emoji8;
	for (; *e16; e16++,e8++) {

		cur->inputBind("1",(const char *)*e8);

		// FIXME: should use an encoding-aware len()
		// function instead of hardcoding to 6
		char	*b64e=charstring::base64Encode(*e16,6);
		cur->inputBind("2",b64e);
		checkSuccess(cur->executeQuery(),1);
		delete[] b64e;
	}
	stdoutput.printf("\n");

	stdoutput.printf("UPDATE: \n");
	checkSuccess(cur->sendQuery("update testtable set emojifrombase64=cast(cast(N'' as xml).value('xs:base64Binary(sql:column(\"base64\"))','VARBINARY(MAX)') AS NVARCHAR(MAX))"),1);
	stdoutput.printf("\n");


	stdoutput.printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable"),1);
	stdoutput.printf("\n");
	uint64_t	row=0;
	for (const unsigned char **e=emoji8; *e; e++) {
		checkSuccess(cur->getField(row,"emojidirect"),
							(const char *)*e);
		checkSuccess(cur->getField(row,"emojifrombase64"),
							(const char *)*e);
		row++;
	}
	stdoutput.printf("\n");

	stdoutput.printf("OUTPUT BIND: \n");
	row=1;
	for (const unsigned char **e=emoji8; *e; e++) {

		cur->prepareQuery("set :output=(select emojidirect from testtable where i=$(row))");
		cur->substitution("row",row);
		cur->defineOutputBindString("output",100);
		checkSuccess(cur->executeQuery(),1);
		checkSuccess(cur->getOutputBindString("output"),
						(const char *)*e);

		cur->prepareQuery("set :output=(select emojifrombase64 from testtable where i=$(row))");
		cur->substitution("row",row);
		cur->defineOutputBindString("output",100);
		checkSuccess(cur->executeQuery(),1);
		checkSuccess(cur->getOutputBindString("output"),
						(const char *)*e);
		row++;
	}
	stdoutput.printf("\n");
	stdoutput.printf("\n");

	delete cur;
	delete con;

	return 0;
}
