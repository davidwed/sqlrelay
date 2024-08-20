// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/process.h>
#include <rudiments/bytestring.h>
#include <rudiments/stdio.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			stdoutput.flush();
			return;
		} else {
			stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
			stdoutput.printf("failure: %s",cur->errorMessage());
			stdoutput.flush();
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
		stdoutput.flush();
	} else {
		stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
		stdoutput.printf("failure: %s",cur->errorMessage());
		stdoutput.flush();
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
		stdoutput.flush();
	} else {
		stdoutput.printf("\"%d\"!=\"%d\"\n",value,success);
		stdoutput.printf("failure: %s",cur->errorMessage());
		stdoutput.flush();
		delete cur;
		delete con;
		process::exit(1);
	}
}

int	main(int argc, char **argv) {

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
						"testuser","testpassword",0,1);
	cur=new sqlrcursor(con);

	// drop existing table
	cur->sendQuery("drop table testtable");

	// create a new table
	stdoutput.printf("CREATE TEMPTABLE: \n");
	checkSuccess(cur->sendQuery("create table testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(2,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testvarchar varchar(40), testtext text, testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testblob blob, testtinyblob tinyblob, testmediumblob mediumblob, testlongblob longblob, testtimestamp timestamp)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	stringbuffer	multiinsert;
	multiinsert.append(
		"insert into testtable values "
		"(1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','varchar1','text1','tinytext1','mediumtext1','longtext1','blob1','tinyblob1','mediumblob1','longblob1',NULL),"
		"(2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','varchar2','text2','tinytext2','mediumtext2','longtext2','blob2','tinyblob2','mediumblob2','longblob2',NULL),"
		"(3,3,3,3,3,3.1,3.1,3.1,'2003-01-01','03:00:00','2003-01-01 03:00:00','2003','char3','varchar3','text3','tinytext3','mediumtext3','longtext3','blob3','tinyblob3','mediumblob3','longblob3',NULL),"
		"(4,4,4,4,4,4.1,4.1,4.1,'2004-01-01','04:00:00','2004-01-01 04:00:00','2004','char4','varchar4','text4','tinytext4','mediumtext4','longtext4','blob4','tinyblob4','mediumblob4','longblob4',NULL)");
	if (charstring::convertToInteger(con->dbVersion())>=5) {
		multiinsert.append(
			" on duplicate key update testtinyint=values(testtinyint)");
	}
	checkSuccess(cur->sendQuery(multiinsert.getString()),1);
	stdoutput.printf("\n");

	stdoutput.printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),4);
	stdoutput.printf("\n");

	stdoutput.printf("ROW COUNT: \n");
	checkSuccess(cur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(cur->getFieldAsInteger(0,(uint32_t)0),4);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT WITH COLUMNS: \n");
	multiinsert.clear();
	multiinsert.append(
		"insert into testtable (testtinyint,testsmallint,testmediumint,testint,testbigint,testfloat,testreal,testdecimal,testdate,testtime,testdatetime,testyear,testchar,testvarchar,testtext,testtinytext,testmediumtext,testlongtext,testblob,testtinyblob,testmediumblob,testlongblob,testtimestamp) values "
		"(1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','varchar1','text1','tinytext1','mediumtext1','longtext1','blob1','tinyblob1','mediumblob1','longblob1',NULL),"
		"(2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','varchar2','text2','tinytext2','mediumtext2','longtext2','blob2','tinyblob2','mediumblob2','longblob2',NULL),"
		"(3,3,3,3,3,3.1,3.1,3.1,'2003-01-01','03:00:00','2003-01-01 03:00:00','2003','char3','varchar3','text3','tinytext3','mediumtext3','longtext3','blob3','tinyblob3','mediumblob3','longblob3',NULL),"
		"(4,4,4,4,4,4.1,4.1,4.1,'2004-01-01','04:00:00','2004-01-01 04:00:00','2004','char4','varchar4','text4','tinytext4','mediumtext4','longtext4','blob4','tinyblob4','mediumblob4','longblob4',NULL)");
	if (charstring::convertToInteger(con->dbVersion())>=5) {
		multiinsert.append(
			" on duplicate key update testtinyint=values(testtinyint)");
	}
	checkSuccess(cur->sendQuery(multiinsert.getString()),1);
	stdoutput.printf("\n");

	stdoutput.printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),4);
	stdoutput.printf("\n");

	stdoutput.printf("ROW COUNT: \n");
	checkSuccess(cur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(cur->getFieldAsInteger(0,(uint32_t)0),8);
	stdoutput.printf("\n");

	cur->sendQuery("drop table testtable");

	delete cur;
	delete con;

	return 0;
}
