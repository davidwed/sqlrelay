// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/process.h>
#include <rudiments/bytestring.h>
#include <rudiments/stdio.h>

//#define PROFILING 1

#ifdef PROFILING
	class noio {
		public:
			void printf(const char * str, ...) {};
			void flush() {};
	};
	static noio nooutput;
	#define stdoutput nooutput
#endif

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

#ifdef PROFILING
for (uint16_t a=0; a<50; a++) {
#endif

	const char	*subvars[4]={"var1","var2","var3",NULL};
	const char	*subvalstrings[3]={"hi","hello","bye"};
	int64_t		subvallongs[3]={1,2,3};
	double		subvaldoubles[3]={10.55,10.556,10.5556};
	uint32_t	precs[3]={4,5,6};
	uint32_t	scales[3]={2,3,4};
	const char * const *cols;
	const char * const *fields;
	uint16_t	port;
	char		*socket;
	uint16_t	id;
	char		*filename;
	uint32_t	*fieldlens;

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	cur=new sqlrcursor(con);

	// get database type
	stdoutput.printf("IDENTIFY: \n");
	checkSuccess(con->identify(),"mysql");

	// ping
	stdoutput.printf("PING: \n");
	checkSuccess(con->ping(),1);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	// create a new table
	stdoutput.printf("CREATE TEMPTABLE: \n");
	checkSuccess(cur->sendQuery("create table testdb.testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(2,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testvarchar varchar(40), testtext text, testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testblob blob, testtinyblob tinyblob, testmediumblob mediumblob, testlongblob longblob, testtimestamp timestamp)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testdb.testtable values (1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','varchar1','text1','tinytext1','mediumtext1','longtext1','blob1','tinyblob1','mediumblob1','longblob1',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testdb.testtable values (2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','varchar2','text2','tinytext2','mediumtext2','longtext2','blob2','tinyblob2','mediumblob2','longblob2',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testdb.testtable values (3,3,3,3,3,3.1,3.1,3.1,'2003-01-01','03:00:00','2003-01-01 03:00:00','2003','char3','varchar3','text3','tinytext3','mediumtext3','longtext3','blob3','tinyblob3','mediumblob3','longblob3',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testdb.testtable values (4,4,4,4,4,4.1,4.1,4.1,'2004-01-01','04:00:00','2004-01-01 04:00:00','2004','char4','varchar4','text4','tinytext4','mediumtext4','longtext4','blob4','tinyblob4','mediumblob4','longblob4',NULL)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY POSITION: \n");
	cur->prepareQuery("insert into testdb.testtable values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,NULL)");
	checkSuccess(cur->countBindVariables(),22);
	cur->inputBind("1",5);
	cur->inputBind("2",5);
	cur->inputBind("3",5);
	cur->inputBind("4",5);
	cur->inputBind("5",5);
	cur->inputBind("6",5.1,2,1);
	cur->inputBind("7",5.1,2,1);
	cur->inputBind("8",5.1,2,1);
	cur->inputBind("9","2005-01-01");
	cur->inputBind("10","05:00:00");
	cur->inputBind("11",2005,1,1,5,0,0,0,NULL,false);
	cur->inputBind("12","2005");
	cur->inputBind("13","char5");
	cur->inputBind("14","varchar5");
	cur->inputBindClob("15","text5",5);
	cur->inputBindClob("16","tinytext5",9);
	cur->inputBindClob("17","mediumtext5",11);
	cur->inputBindClob("18","longtext5",9);
	cur->inputBindBlob("19","blob5",5);
	cur->inputBindBlob("20","tinyblob5",9);
	cur->inputBindBlob("21","mediumblob5",11);
	cur->inputBindBlob("22","longblob5",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",6);
	cur->inputBind("2",6);
	cur->inputBind("3",6);
	cur->inputBind("4",6);
	cur->inputBind("5",6);
	cur->inputBind("6",6.1,2,1);
	cur->inputBind("7",6.1,2,1);
	cur->inputBind("8",6.1,2,1);
	cur->inputBind("9","2006-01-01");
	cur->inputBind("10","06:00:00");
	cur->inputBind("11",2006,1,1,6,0,0,0,NULL,false);
	cur->inputBind("12","2006");
	cur->inputBind("13","char6");
	cur->inputBind("14","varchar6");
	cur->inputBindClob("15","text6",5);
	cur->inputBindClob("16","tinytext6",9);
	cur->inputBindClob("17","mediumtext6",11);
	cur->inputBindClob("18","longtext6",9);
	cur->inputBindBlob("19","blob6",5);
	cur->inputBindBlob("20","tinyblob6",9);
	cur->inputBindBlob("21","mediumblob6",11);
	cur->inputBindBlob("22","longblob6",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",7);
	cur->inputBind("2",7);
	cur->inputBind("3",7);
	cur->inputBind("4",7);
	cur->inputBind("5",7);
	cur->inputBind("6",7.1,2,1);
	cur->inputBind("7",7.1,2,1);
	cur->inputBind("8",7.1,2,1);
	cur->inputBind("9","2007-01-01");
	cur->inputBind("10","07:00:00");
	cur->inputBind("11",2007,1,1,7,0,0,0,NULL,false);
	cur->inputBind("12","2007");
	cur->inputBind("13","char7");
	cur->inputBind("14","varchar7");
	cur->inputBindClob("15","text7",5);
	cur->inputBindClob("16","tinytext7",9);
	cur->inputBindClob("17","mediumtext7",11);
	cur->inputBindClob("18","longtext7",9);
	cur->inputBindBlob("19","blob7",5);
	cur->inputBindBlob("20","tinyblob7",9);
	cur->inputBindBlob("21","mediumblob7",11);
	cur->inputBindBlob("22","longblob7",9);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY POSITION WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("1",8);
	cur->inputBind("2",8);
	cur->inputBind("3",8);
	cur->inputBind("4",8);
	cur->inputBind("5",8);
	cur->inputBind("6",8.1,2,1);
	cur->inputBind("7",8.1,2,1);
	cur->inputBind("8",8.1,2,1);
	cur->inputBind("9","2008-01-01");
	cur->inputBind("10","08:00:00");
	cur->inputBind("11",2008,1,1,8,0,0,0,NULL,false);
	cur->inputBind("12","2008");
	cur->inputBind("13","char8");
	cur->inputBind("14","varchar8");
	cur->inputBindClob("15","text8",5);
	cur->inputBindClob("16","tinytext8",9);
	cur->inputBindClob("17","mediumtext8",11);
	cur->inputBindClob("18","longtext8",9);
	cur->inputBindBlob("19","blob8",5);
	cur->inputBindBlob("20","tinyblob8",9);
	cur->inputBindBlob("21","mediumblob8",11);
	cur->inputBindBlob("22","longblob8",9);
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),23);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES: \n");
	checkSuccess(cur->getColumnName(0),"testtinyint");
	checkSuccess(cur->getColumnName(1),"testsmallint");
	checkSuccess(cur->getColumnName(2),"testmediumint");
	checkSuccess(cur->getColumnName(3),"testint");
	checkSuccess(cur->getColumnName(4),"testbigint");
	checkSuccess(cur->getColumnName(5),"testfloat");
	checkSuccess(cur->getColumnName(6),"testreal");
	checkSuccess(cur->getColumnName(7),"testdecimal");
	checkSuccess(cur->getColumnName(8),"testdate");
	checkSuccess(cur->getColumnName(9),"testtime");
	checkSuccess(cur->getColumnName(10),"testdatetime");
	checkSuccess(cur->getColumnName(11),"testyear");
	checkSuccess(cur->getColumnName(12),"testchar");
	checkSuccess(cur->getColumnName(13),"testvarchar");
	checkSuccess(cur->getColumnName(14),"testtext");
	checkSuccess(cur->getColumnName(15),"testtinytext");
	checkSuccess(cur->getColumnName(16),"testmediumtext");
	checkSuccess(cur->getColumnName(17),"testlongtext");
	checkSuccess(cur->getColumnName(18),"testblob");
	checkSuccess(cur->getColumnName(19),"testtinyblob");
	checkSuccess(cur->getColumnName(20),"testmediumblob");
	checkSuccess(cur->getColumnName(21),"testlongblob");
	checkSuccess(cur->getColumnName(22),"testtimestamp");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testtinyint");
	checkSuccess(cols[1],"testsmallint");
	checkSuccess(cols[2],"testmediumint");
	checkSuccess(cols[3],"testint");
	checkSuccess(cols[4],"testbigint");
	checkSuccess(cols[5],"testfloat");
	checkSuccess(cols[6],"testreal");
	checkSuccess(cols[7],"testdecimal");
	checkSuccess(cols[8],"testdate");
	checkSuccess(cols[9],"testtime");
	checkSuccess(cols[10],"testdatetime");
	checkSuccess(cols[11],"testyear");
	checkSuccess(cols[12],"testchar");
	checkSuccess(cols[13],"testvarchar");
	checkSuccess(cols[14],"testtext");
	checkSuccess(cols[15],"testtinytext");
	checkSuccess(cols[16],"testmediumtext");
	checkSuccess(cols[17],"testlongtext");
	checkSuccess(cols[18],"testblob");
	checkSuccess(cols[19],"testtinyblob");
	checkSuccess(cols[20],"testmediumblob");
	checkSuccess(cols[21],"testlongblob");
	checkSuccess(cols[22],"testtimestamp");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType((uint32_t)0),"TINYINT");
	checkSuccess(cur->getColumnType(1),"SMALLINT");
	checkSuccess(cur->getColumnType(2),"MEDIUMINT");
	checkSuccess(cur->getColumnType(3),"INT");
	checkSuccess(cur->getColumnType(4),"BIGINT");
	checkSuccess(cur->getColumnType(5),"FLOAT");
	checkSuccess(cur->getColumnType(6),"REAL");
	checkSuccess(cur->getColumnType(7),"DECIMAL");
	checkSuccess(cur->getColumnType(8),"DATE");
	checkSuccess(cur->getColumnType(9),"TIME");
	checkSuccess(cur->getColumnType(10),"DATETIME");
	checkSuccess(cur->getColumnType(11),"YEAR");
	checkSuccess(cur->getColumnType(12),"STRING");
	checkSuccess(cur->getColumnType(13),"VARSTRING");
	checkSuccess(cur->getColumnType(14),"BLOB");
	checkSuccess(cur->getColumnType(15),"TINYBLOB");
	checkSuccess(cur->getColumnType(16),"MEDIUMBLOB");
	checkSuccess(cur->getColumnType(17),"LONGBLOB");
	checkSuccess(cur->getColumnType(18),"BLOB");
	checkSuccess(cur->getColumnType(19),"TINYBLOB");
	checkSuccess(cur->getColumnType(20),"MEDIUMBLOB");
	checkSuccess(cur->getColumnType(21),"LONGBLOB");
	checkSuccess(cur->getColumnType(22),"TIMESTAMP");
	checkSuccess(cur->getColumnType("testtinyint"),"TINYINT");
	checkSuccess(cur->getColumnType("testsmallint"),"SMALLINT");
	checkSuccess(cur->getColumnType("testmediumint"),"MEDIUMINT");
	checkSuccess(cur->getColumnType("testint"),"INT");
	checkSuccess(cur->getColumnType("testbigint"),"BIGINT");
	checkSuccess(cur->getColumnType("testfloat"),"FLOAT");
	checkSuccess(cur->getColumnType("testreal"),"REAL");
	checkSuccess(cur->getColumnType("testdecimal"),"DECIMAL");
	checkSuccess(cur->getColumnType("testdate"),"DATE");
	checkSuccess(cur->getColumnType("testtime"),"TIME");
	checkSuccess(cur->getColumnType("testdatetime"),"DATETIME");
	checkSuccess(cur->getColumnType("testyear"),"YEAR");
	checkSuccess(cur->getColumnType("testchar"),"STRING");
	checkSuccess(cur->getColumnType("testvarchar"),"VARSTRING");
	checkSuccess(cur->getColumnType("testtext"),"BLOB");
	checkSuccess(cur->getColumnType("testtinytext"),"TINYBLOB");
	checkSuccess(cur->getColumnType("testmediumtext"),"MEDIUMBLOB");
	checkSuccess(cur->getColumnType("testlongtext"),"LONGBLOB");
	checkSuccess(cur->getColumnType("testblob"),"BLOB");
	checkSuccess(cur->getColumnType("testtinyblob"),"TINYBLOB");
	checkSuccess(cur->getColumnType("testmediumblob"),"MEDIUMBLOB");
	checkSuccess(cur->getColumnType("testlongblob"),"LONGBLOB");
	checkSuccess(cur->getColumnType("testtimestamp"),"TIMESTAMP");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),1);
	checkSuccess(cur->getColumnLength(1),2);
	checkSuccess(cur->getColumnLength(2),3);
	checkSuccess(cur->getColumnLength(3),4);
	checkSuccess(cur->getColumnLength(4),8);
	checkSuccess(cur->getColumnLength(5),4);
	checkSuccess(cur->getColumnLength(6),8);
	checkSuccess(cur->getColumnLength(7),6);
	checkSuccess(cur->getColumnLength(8),3);
	checkSuccess(cur->getColumnLength(9),3);
	checkSuccess(cur->getColumnLength(10),8);
	checkSuccess(cur->getColumnLength(11),1);
	// these can be 120/121 if the db charset is utf8
	//checkSuccess(cur->getColumnLength(12),40);
	//checkSuccess(cur->getColumnLength(13),41);
	checkSuccess(cur->getColumnLength(14),65535);
	checkSuccess(cur->getColumnLength(15),255);
	checkSuccess(cur->getColumnLength(16),16777215);
	checkSuccess(cur->getColumnLength(17),2147483647);
	checkSuccess(cur->getColumnLength(18),65535);
	checkSuccess(cur->getColumnLength(19),255);
	checkSuccess(cur->getColumnLength(20),16777215);
	checkSuccess(cur->getColumnLength(21),2147483647);
	checkSuccess(cur->getColumnLength(22),4);
	checkSuccess(cur->getColumnLength("testtinyint"),1);
	checkSuccess(cur->getColumnLength("testsmallint"),2);
	checkSuccess(cur->getColumnLength("testmediumint"),3);
	checkSuccess(cur->getColumnLength("testint"),4);
	checkSuccess(cur->getColumnLength("testbigint"),8);
	checkSuccess(cur->getColumnLength("testfloat"),4);
	checkSuccess(cur->getColumnLength("testreal"),8);
	checkSuccess(cur->getColumnLength("testdecimal"),6);
	checkSuccess(cur->getColumnLength("testdate"),3);
	checkSuccess(cur->getColumnLength("testtime"),3);
	checkSuccess(cur->getColumnLength("testdatetime"),8);
	checkSuccess(cur->getColumnLength("testyear"),1);
	// these can be 120/121 if the db charset is utf8
	//checkSuccess(cur->getColumnLength("testchar"),40);
	//checkSuccess(cur->getColumnLength("testvarchar"),41);
	checkSuccess(cur->getColumnLength("testtext"),65535);
	checkSuccess(cur->getColumnLength("testtinytext"),255);
	checkSuccess(cur->getColumnLength("testmediumtext"),16777215);
	checkSuccess(cur->getColumnLength("testlongtext"),2147483647);
	checkSuccess(cur->getColumnLength("testblob"),65535);
	checkSuccess(cur->getColumnLength("testtinyblob"),255);
	checkSuccess(cur->getColumnLength("testmediumblob"),16777215);
	checkSuccess(cur->getColumnLength("testlongblob"),2147483647);
	checkSuccess(cur->getColumnLength("testtimestamp"),4);
	stdoutput.printf("\n");

	stdoutput.printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest(1),1);
	checkSuccess(cur->getLongest(2),1);
	checkSuccess(cur->getLongest(3),1);
	checkSuccess(cur->getLongest(4),1);
	//checkSuccess(cur->getLongest(5),3);
	checkSuccess(cur->getLongest(6),3);
	checkSuccess(cur->getLongest(7),3);
	checkSuccess(cur->getLongest(8),10);
	checkSuccess(cur->getLongest(9),8);
	checkSuccess(cur->getLongest(10),19);
	checkSuccess(cur->getLongest(11),4);
	checkSuccess(cur->getLongest(12),5);
	checkSuccess(cur->getLongest(13),8);
	checkSuccess(cur->getLongest(14),5);
	checkSuccess(cur->getLongest(15),9);
	checkSuccess(cur->getLongest(16),11);
	checkSuccess(cur->getLongest(17),9);
	checkSuccess(cur->getLongest(18),5);
	checkSuccess(cur->getLongest(19),9);
	checkSuccess(cur->getLongest(20),11);
	checkSuccess(cur->getLongest(21),9);
	checkSuccess(cur->getLongest(22),19);
	checkSuccess(cur->getLongest("testtinyint"),1);
	checkSuccess(cur->getLongest("testsmallint"),1);
	checkSuccess(cur->getLongest("testmediumint"),1);
	checkSuccess(cur->getLongest("testint"),1);
	checkSuccess(cur->getLongest("testbigint"),1);
	//checkSuccess(cur->getLongest("testfloat"),3);
	checkSuccess(cur->getLongest("testreal"),3);
	checkSuccess(cur->getLongest("testdecimal"),3);
	checkSuccess(cur->getLongest("testdate"),10);
	checkSuccess(cur->getLongest("testtime"),8);
	checkSuccess(cur->getLongest("testdatetime"),19);
	checkSuccess(cur->getLongest("testyear"),4);
	checkSuccess(cur->getLongest("testchar"),5);
	checkSuccess(cur->getLongest("testvarchar"),8);
	checkSuccess(cur->getLongest("testtext"),5);
	checkSuccess(cur->getLongest("testtinytext"),9);
	checkSuccess(cur->getLongest("testmediumtext"),11);
	checkSuccess(cur->getLongest("testlongtext"),9);
	checkSuccess(cur->getLongest("testblob"),5);
	checkSuccess(cur->getLongest("testtinyblob"),9);
	checkSuccess(cur->getLongest("testmediumblob"),11);
	checkSuccess(cur->getLongest("testlongblob"),9);
	checkSuccess(cur->getLongest("testtimestamp"),19);
	stdoutput.printf("\n");

	stdoutput.printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("TOTAL ROWS: \n");
	checkSuccess(cur->totalRows(),0);
	stdoutput.printf("\n");

	stdoutput.printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	stdoutput.printf("\n");

	stdoutput.printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),"1");
	checkSuccess(cur->getField(0,3),"1");
	checkSuccess(cur->getField(0,4),"1");
	//checkSuccess(cur->getField(0,5),"1.1");
	checkSuccess(cur->getField(0,6),"1.1");
	checkSuccess(cur->getField(0,7),"1.1");
	checkSuccess(cur->getField(0,8),"2001-01-01");
	checkSuccess(cur->getField(0,9),"01:00:00");
	checkSuccess(cur->getField(0,10),"2001-01-01 01:00:00");
	checkSuccess(cur->getField(0,11),"2001");
	checkSuccess(cur->getField(0,12),"char1");
	checkSuccess(cur->getField(0,13),"varchar1");
	checkSuccess(cur->getField(0,14),"text1");
	checkSuccess(cur->getField(0,15),"tinytext1");
	checkSuccess(cur->getField(0,16),"mediumtext1");
	checkSuccess(cur->getField(0,17),"longtext1");
	checkSuccess(cur->getField(0,18),"blob1");
	checkSuccess(cur->getField(0,19),"tinyblob1");
	checkSuccess(cur->getField(0,20),"mediumblob1");
	checkSuccess(cur->getField(0,21),"longblob1");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(7,1),"8");
	checkSuccess(cur->getField(7,2),"8");
	checkSuccess(cur->getField(7,3),"8");
	checkSuccess(cur->getField(7,4),"8");
	//checkSuccess(cur->getField(7,5),"8.1");
	checkSuccess(cur->getField(7,6),"8.1");
	checkSuccess(cur->getField(7,7),"8.1");
	checkSuccess(cur->getField(7,8),"2008-01-01");
	checkSuccess(cur->getField(7,9),"08:00:00");
	checkSuccess(cur->getField(7,10),"2008-01-01 08:00:00");
	checkSuccess(cur->getField(7,11),"2008");
	checkSuccess(cur->getField(7,12),"char8");
	checkSuccess(cur->getField(7,13),"varchar8");
	checkSuccess(cur->getField(7,14),"text8");
	checkSuccess(cur->getField(7,15),"tinytext8");
	checkSuccess(cur->getField(7,16),"mediumtext8");
	checkSuccess(cur->getField(7,17),"longtext8");
	checkSuccess(cur->getField(7,18),"blob8");
	checkSuccess(cur->getField(7,19),"tinyblob8");
	checkSuccess(cur->getField(7,20),"mediumblob8");
	checkSuccess(cur->getField(7,21),"longblob8");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(0,1),1);
	checkSuccess(cur->getFieldLength(0,2),1);
	checkSuccess(cur->getFieldLength(0,3),1);
	checkSuccess(cur->getFieldLength(0,4),1);
	//checkSuccess(cur->getFieldLength(0,5),3);
	checkSuccess(cur->getFieldLength(0,6),3);
	checkSuccess(cur->getFieldLength(0,7),3);
	checkSuccess(cur->getFieldLength(0,8),10);
	checkSuccess(cur->getFieldLength(0,9),8);
	checkSuccess(cur->getFieldLength(0,10),19);
	checkSuccess(cur->getFieldLength(0,11),4);
	checkSuccess(cur->getFieldLength(0,12),5);
	checkSuccess(cur->getFieldLength(0,13),8);
	checkSuccess(cur->getFieldLength(0,14),5);
	checkSuccess(cur->getFieldLength(0,15),9);
	checkSuccess(cur->getFieldLength(0,16),11);
	checkSuccess(cur->getFieldLength(0,17),9);
	checkSuccess(cur->getFieldLength(0,18),5);
	checkSuccess(cur->getFieldLength(0,19),9);
	checkSuccess(cur->getFieldLength(0,20),11);
	checkSuccess(cur->getFieldLength(0,21),9);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(7,1),1);
	checkSuccess(cur->getFieldLength(7,2),1);
	checkSuccess(cur->getFieldLength(7,3),1);
	checkSuccess(cur->getFieldLength(7,4),1);
	//checkSuccess(cur->getFieldLength(7,5),3);
	checkSuccess(cur->getFieldLength(7,6),3);
	checkSuccess(cur->getFieldLength(7,7),3);
	checkSuccess(cur->getFieldLength(7,8),10);
	checkSuccess(cur->getFieldLength(7,9),8);
	checkSuccess(cur->getFieldLength(7,10),19);
	checkSuccess(cur->getFieldLength(7,11),4);
	checkSuccess(cur->getFieldLength(7,12),5);
	checkSuccess(cur->getFieldLength(7,13),8);
	checkSuccess(cur->getFieldLength(7,14),5);
	checkSuccess(cur->getFieldLength(7,15),9);
	checkSuccess(cur->getFieldLength(7,16),11);
	checkSuccess(cur->getFieldLength(7,17),9);
	checkSuccess(cur->getFieldLength(7,18),5);
	checkSuccess(cur->getFieldLength(7,19),9);
	checkSuccess(cur->getFieldLength(7,20),11);
	checkSuccess(cur->getFieldLength(7,21),9);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"testtinyint"),"1");
	checkSuccess(cur->getField(0,"testsmallint"),"1");
	checkSuccess(cur->getField(0,"testmediumint"),"1");
	checkSuccess(cur->getField(0,"testint"),"1");
	checkSuccess(cur->getField(0,"testbigint"),"1");
	//checkSuccess(cur->getField(0,"testfloat"),"1.1");
	checkSuccess(cur->getField(0,"testreal"),"1.1");
	checkSuccess(cur->getField(0,"testdecimal"),"1.1");
	checkSuccess(cur->getField(0,"testdate"),"2001-01-01");
	checkSuccess(cur->getField(0,"testtime"),"01:00:00");
	checkSuccess(cur->getField(0,"testdatetime"),"2001-01-01 01:00:00");
	checkSuccess(cur->getField(0,"testyear"),"2001");
	checkSuccess(cur->getField(0,"testchar"),"char1");
	checkSuccess(cur->getField(0,"testvarchar"),"varchar1");
	checkSuccess(cur->getField(0,"testtext"),"text1");
	checkSuccess(cur->getField(0,"testtinytext"),"tinytext1");
	checkSuccess(cur->getField(0,"testmediumtext"),"mediumtext1");
	checkSuccess(cur->getField(0,"testlongtext"),"longtext1");
	checkSuccess(cur->getField(0,"testblob"),"blob1");
	checkSuccess(cur->getField(0,"testlongblob"),"longblob1");
	checkSuccess(cur->getField(0,"testtinyblob"),"tinyblob1");
	checkSuccess(cur->getField(0,"testmediumblob"),"mediumblob1");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,"testtinyint"),"8");
	checkSuccess(cur->getField(7,"testsmallint"),"8");
	checkSuccess(cur->getField(7,"testmediumint"),"8");
	checkSuccess(cur->getField(7,"testint"),"8");
	checkSuccess(cur->getField(7,"testbigint"),"8");
	//checkSuccess(cur->getField(7,"testfloat"),"8.1");
	checkSuccess(cur->getField(7,"testreal"),"8.1");
	checkSuccess(cur->getField(7,"testdecimal"),"8.1");
	checkSuccess(cur->getField(7,"testdate"),"2008-01-01");
	checkSuccess(cur->getField(7,"testtime"),"08:00:00");
	checkSuccess(cur->getField(7,"testdatetime"),"2008-01-01 08:00:00");
	checkSuccess(cur->getField(7,"testyear"),"2008");
	checkSuccess(cur->getField(7,"testchar"),"char8");
	checkSuccess(cur->getField(7,"testvarchar"),"varchar8");
	checkSuccess(cur->getField(7,"testtext"),"text8");
	checkSuccess(cur->getField(7,"testtinytext"),"tinytext8");
	checkSuccess(cur->getField(7,"testmediumtext"),"mediumtext8");
	checkSuccess(cur->getField(7,"testlongtext"),"longtext8");
	checkSuccess(cur->getField(7,"testblob"),"blob8");
	checkSuccess(cur->getField(7,"testlongblob"),"longblob8");
	checkSuccess(cur->getField(7,"testtinyblob"),"tinyblob8");
	checkSuccess(cur->getField(7,"testmediumblob"),"mediumblob8");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"testtinyint"),1);
	checkSuccess(cur->getFieldLength(0,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(0,"testmediumint"),1);
	checkSuccess(cur->getFieldLength(0,"testint"),1);
	checkSuccess(cur->getFieldLength(0,"testbigint"),1);
	//checkSuccess(cur->getFieldLength(0,"testfloat"),3);
	checkSuccess(cur->getFieldLength(0,"testreal"),3);
	checkSuccess(cur->getFieldLength(0,"testdecimal"),3);
	checkSuccess(cur->getFieldLength(0,"testdate"),10);
	checkSuccess(cur->getFieldLength(0,"testtime"),8);
	checkSuccess(cur->getFieldLength(0,"testdatetime"),19);
	checkSuccess(cur->getFieldLength(0,"testyear"),4);
	checkSuccess(cur->getFieldLength(0,"testchar"),5);
	checkSuccess(cur->getFieldLength(0,"testvarchar"),8);
	checkSuccess(cur->getFieldLength(0,"testtext"),5);
	checkSuccess(cur->getFieldLength(0,"testtinytext"),9);
	checkSuccess(cur->getFieldLength(0,"testmediumtext"),11);
	checkSuccess(cur->getFieldLength(0,"testlongtext"),9);
	checkSuccess(cur->getFieldLength(0,"testblob"),5);
	checkSuccess(cur->getFieldLength(0,"testtinyblob"),9);
	checkSuccess(cur->getFieldLength(0,"testmediumblob"),11);
	checkSuccess(cur->getFieldLength(0,"testlongblob"),9);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,"testtinyint"),1);
	checkSuccess(cur->getFieldLength(7,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(7,"testmediumint"),1);
	checkSuccess(cur->getFieldLength(7,"testint"),1);
	checkSuccess(cur->getFieldLength(7,"testbigint"),1);
	//checkSuccess(cur->getFieldLength(7,"testfloat"),3);
	checkSuccess(cur->getFieldLength(7,"testreal"),3);
	checkSuccess(cur->getFieldLength(7,"testdecimal"),3);
	checkSuccess(cur->getFieldLength(7,"testdate"),10);
	checkSuccess(cur->getFieldLength(7,"testtime"),8);
	checkSuccess(cur->getFieldLength(7,"testdatetime"),19);
	checkSuccess(cur->getFieldLength(7,"testyear"),4);
	checkSuccess(cur->getFieldLength(7,"testchar"),5);
	checkSuccess(cur->getFieldLength(7,"testvarchar"),8);
	checkSuccess(cur->getFieldLength(7,"testtext"),5);
	checkSuccess(cur->getFieldLength(7,"testtinytext"),9);
	checkSuccess(cur->getFieldLength(7,"testmediumtext"),11);
	checkSuccess(cur->getFieldLength(7,"testlongtext"),9);
	checkSuccess(cur->getFieldLength(7,"testblob"),5);
	checkSuccess(cur->getFieldLength(7,"testtinyblob"),9);
	checkSuccess(cur->getFieldLength(7,"testmediumblob"),11);
	checkSuccess(cur->getFieldLength(7,"testlongblob"),9);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	checkSuccess(fields[1],"1");
	checkSuccess(fields[2],"1");
	checkSuccess(fields[3],"1");
	checkSuccess(fields[4],"1");
	//checkSuccess(fields[5],"1.1");
	checkSuccess(fields[6],"1.1");
	checkSuccess(fields[7],"1.1");
	checkSuccess(fields[8],"2001-01-01");
	checkSuccess(fields[9],"01:00:00");
	checkSuccess(fields[10],"2001-01-01 01:00:00");
	checkSuccess(fields[11],"2001");
	checkSuccess(fields[12],"char1");
	checkSuccess(fields[13],"varchar1");
	checkSuccess(fields[14],"text1");
	checkSuccess(fields[15],"tinytext1");
	checkSuccess(fields[16],"mediumtext1");
	checkSuccess(fields[17],"longtext1");
	checkSuccess(fields[18],"blob1");
	checkSuccess(fields[19],"tinyblob1");
	checkSuccess(fields[20],"mediumblob1");
	checkSuccess(fields[21],"longblob1");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	checkSuccess(fieldlens[1],1);
	checkSuccess(fieldlens[2],1);
	checkSuccess(fieldlens[3],1);
	checkSuccess(fieldlens[4],1);
	//checkSuccess(fieldlens[5],3);
	checkSuccess(fieldlens[6],3);
	checkSuccess(fieldlens[7],3);
	checkSuccess(fieldlens[8],10);
	checkSuccess(fieldlens[9],8);
	checkSuccess(fieldlens[10],19);
	checkSuccess(fieldlens[11],4);
	checkSuccess(fieldlens[12],5);
	checkSuccess(fieldlens[13],8);
	checkSuccess(fieldlens[14],5);
	checkSuccess(fieldlens[15],9);
	checkSuccess(fieldlens[16],11);
	checkSuccess(fieldlens[17],9);
	checkSuccess(fieldlens[18],5);
	checkSuccess(fieldlens[19],9);
	checkSuccess(fieldlens[20],11);
	checkSuccess(fieldlens[21],9);
	stdoutput.printf("\n");

	stdoutput.printf("INDIVIDUAL SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),'$(var2)',$(var3)");
	cur->substitution("var1",1);
	cur->substitution("var2","hello");
	cur->substitution("var3",10.5556,6,4);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"10.5556");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3)");
	cur->substitutions(subvars,subvallongs);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");
	
	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"2");
	checkSuccess(cur->getField(0,2),"3");
	stdoutput.printf("\n");
	
	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select '$(var1)','$(var2)','$(var3)'");
	cur->substitutions(subvars,subvalstrings);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"hi");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"bye");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3)");
	cur->substitutions(subvars,subvaldoubles,precs,scales);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"10.55");
	checkSuccess(cur->getField(0,1),"10.556");
	checkSuccess(cur->getField(0,2),"10.5556");
	stdoutput.printf("\n");

	stdoutput.printf("NULLS as Nulls: \n");
	cur->getNullsAsNulls();
	checkSuccess(cur->sendQuery("select NULL,1,NULL"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),NULL);
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),NULL);
	cur->getNullsAsEmptyStrings();
	checkSuccess(cur->sendQuery("select NULL,1,NULL"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"");
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),"");
	cur->getNullsAsNulls();
	stdoutput.printf("\n");

	stdoutput.printf("RESULT SET BUFFER SIZE: \n");
	checkSuccess(cur->getResultSetBufferSize(),0);
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	checkSuccess(cur->getResultSetBufferSize(),2);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),0);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),2);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),2);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),4);
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("DONT GET COLUMN INFO: \n");
	cur->dontGetColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	checkSuccess(cur->getColumnName(0),NULL);
	checkSuccess(cur->getColumnLength((uint32_t)0),0);
	checkSuccess(cur->getColumnType((uint32_t)0),NULL);
	stdoutput.printf("\n");
	cur->getColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	checkSuccess(cur->getColumnName(0),"testtinyint");
	checkSuccess(cur->getColumnLength((uint32_t)0),1);
	checkSuccess(cur->getColumnType((uint32_t)0),"TINYINT");
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->getField(3,(uint32_t)0),"4");
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->getField(3,(uint32_t)0),"4");
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->getField(3,(uint32_t)0),"4");
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED RESULT SET: \n");
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeResultSet(id),1);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),4);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),6);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	cur->setResultSetBufferSize(0);
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET: \n");
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(cur->colCount(),23);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(cur->getColumnName(0),"testtinyint");
	checkSuccess(cur->getColumnName(1),"testsmallint");
	checkSuccess(cur->getColumnName(2),"testmediumint");
	checkSuccess(cur->getColumnName(3),"testint");
	checkSuccess(cur->getColumnName(4),"testbigint");
	checkSuccess(cur->getColumnName(5),"testfloat");
	checkSuccess(cur->getColumnName(6),"testreal");
	checkSuccess(cur->getColumnName(7),"testdecimal");
	checkSuccess(cur->getColumnName(8),"testdate");
	checkSuccess(cur->getColumnName(9),"testtime");
	checkSuccess(cur->getColumnName(10),"testdatetime");
	checkSuccess(cur->getColumnName(11),"testyear");
	checkSuccess(cur->getColumnName(12),"testchar");
	checkSuccess(cur->getColumnName(13),"testvarchar");
	checkSuccess(cur->getColumnName(14),"testtext");
	checkSuccess(cur->getColumnName(15),"testtinytext");
	checkSuccess(cur->getColumnName(16),"testmediumtext");
	checkSuccess(cur->getColumnName(17),"testlongtext");
	checkSuccess(cur->getColumnName(18),"testblob");
	checkSuccess(cur->getColumnName(19),"testtinyblob");
	checkSuccess(cur->getColumnName(20),"testmediumblob");
	checkSuccess(cur->getColumnName(21),"testlongblob");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testtinyint");
	checkSuccess(cols[1],"testsmallint");
	checkSuccess(cols[2],"testmediumint");
	checkSuccess(cols[3],"testint");
	checkSuccess(cols[4],"testbigint");
	checkSuccess(cols[5],"testfloat");
	checkSuccess(cols[6],"testreal");
	checkSuccess(cols[7],"testdecimal");
	checkSuccess(cols[8],"testdate");
	checkSuccess(cols[9],"testtime");
	checkSuccess(cols[10],"testdatetime");
	checkSuccess(cols[11],"testyear");
	checkSuccess(cols[12],"testchar");
	checkSuccess(cols[13],"testvarchar");
	checkSuccess(cols[14],"testtext");
	checkSuccess(cols[15],"testtinytext");
	checkSuccess(cols[16],"testmediumtext");
	checkSuccess(cols[17],"testlongtext");
	checkSuccess(cols[18],"testblob");
	checkSuccess(cols[19],"testtinyblob");
	checkSuccess(cols[20],"testmediumblob");
	checkSuccess(cols[21],"testlongblob");
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("FROM ONE CACHE FILE TO ANOTHER: \n");
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");

	stdoutput.printf("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	cur->setResultSetBufferSize(0);
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),1);
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	stdoutput.printf("\n");
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeCachedResultSet(id,filename),1);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),4);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),6);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	cur->cacheOff();
	stdoutput.printf("\n");
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("COMMIT AND ROLLBACK: \n");
	// Note: Mysql's default isolation level is repeatable-read,
	// not read-committed like most other db's.  Both sessions must
	// commit to see the changes that each other has made.
	secondcon=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	secondcur=new sqlrcursor(secondcon);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"0");
	checkSuccess(con->commit(),1);
	checkSuccess(secondcon->commit(),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"8");
	checkSuccess(con->autoCommitOn(),1);
	checkSuccess(cur->sendQuery("insert into testdb.testtable values (10,10,10,10,10,10.1,10.1,10.1,'2010-01-01','10:00:00','2010-01-01 10:00:00','2010','char10','varchar10','text10','tinytext10','mediumtext10','longtext10','blob10','tinyblob10','mediumblob10','longblob10',NULL)"),1);
	checkSuccess(secondcon->commit(),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"9");
	checkSuccess(con->autoCommitOff(),1);
	secondcon->commit();
	stdoutput.printf("\n");

	stdoutput.printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeResultSet(id),1);
	checkSuccess(cur->getField(4,(uint32_t)0),NULL);
	checkSuccess(cur->getField(5,(uint32_t)0),NULL);
	checkSuccess(cur->getField(6,(uint32_t)0),NULL);
	checkSuccess(cur->getField(7,(uint32_t)0),NULL);
	stdoutput.printf("\n");

	// temporary tables
	stdoutput.printf("TEMPORARY TABLES: \n");
	cur->sendQuery("drop table temptable\n");
	cur->sendQuery("create temporary table temptable (col1 int)");
	checkSuccess(cur->sendQuery("insert into temptable values (1)"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptable"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	con->endSession();
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select count(*) from temptable"),0);
	cur->sendQuery("drop table temptable\n");
	stdoutput.printf("\n");

	// stored functions
	stdoutput.printf("FUNCTIONS: \n");
	cur->sendQuery("drop function if exists testfunc");
	checkSuccess(cur->sendQuery("create function testfunc(in1 int, in2 int) returns int return in1+in2;"),1);
	cur->prepareQuery("select testfunc(?,?)");
	cur->inputBind("1",10);
	cur->inputBind("2",20);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"30");
	cur->sendQuery("drop function if exists testfunc");
	stdoutput.printf("\n");

	// stored procedures
	stdoutput.printf("STORED PROCEDURES: \n");
	// return no values
	cur->sendQuery("drop procedure if exists testproc");
	checkSuccess(cur->sendQuery("create procedure testproc(in in1 int, in in2 float, in in3 char(20)) begin select in1, in2, in3; end;"),1);
	cur->prepareQuery("call testproc(:in1,:in2,:in3)");
	cur->inputBind("in1",1);
	cur->inputBind("in2",1.1,4,2);
	cur->inputBind("in3","hello");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,(uint32_t)1),"1.1");
	checkSuccess(cur->getField(0,(uint32_t)2),"hello");
	cur->sendQuery("drop procedure testproc");
	stdoutput.printf("\n");
	// return values
	checkSuccess(cur->sendQuery("create procedure testproc(out out1 int, out out2 float, out out3 char(20)) begin select 1, 1.1, 'hello' into out1, out2, out3; end;"),1);
	checkSuccess(cur->sendQuery("set @out1=0, @out2=0.0, @out3=''"),1);
	checkSuccess(cur->sendQuery("call testproc(@out1,@out2,@out3)"),1);
	checkSuccess(cur->sendQuery("select @out1, @out2, @out3"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getFieldAsDouble(0,(uint32_t)1),1.1);
	checkSuccess(cur->getField(0,(uint32_t)2),"hello");
	cur->sendQuery("drop procedure testproc");
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	// long lobs
	stdoutput.printf("LONG LOBS: \n");
	cur->sendQuery("drop table testtable1");
	cur->sendQuery("create table testtable1 (testtext longtext, testblob longblob)");
	cur->prepareQuery("insert into testtable1 values (?,?)");
	char	clobval[8*1024+1];
	char	blobval[8*1024+1];
	for (uint32_t i=0; i<8*1024; i++) {
		clobval[i]='C';
		blobval[i]='C';
	}
	clobval[8*1024]='\0';
	blobval[8*1024]='\0';
	cur->inputBindClob("1",clobval,8*1024);
	cur->inputBindBlob("2",blobval,8*1024);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select * from testtable1");
	checkSuccess(cur->getField(0,"testtext"),clobval);
	checkSuccess(cur->getField(0,"testblob"),clobval);
	cur->sendQuery("drop table testtable1");
	stdoutput.printf("\n");

	// binary data
	stdoutput.printf("BINARY DATA: \n");
	checkSuccess(cur->sendQuery("create table testtable (col1 longblob)"),true);
	unsigned char	buffer[256];
	for (uint16_t i=0; i<256; i++) {
		buffer[i]=i;
	}
	stringbuffer	query;
	query.append("insert into testtable values (_binary'");
	for (uint64_t i=0; i<sizeof(buffer); i++) {
		if (buffer[i]=='\'') {
			query.append('\'');
		}
		if (buffer[i]=='\\') {
			query.append('\\');
		}
		query.append(buffer[i]);
	}
	query.append("')");
	checkSuccess(cur->sendQuery(query.getString(),query.getSize()),true);
	checkSuccess(cur->sendQuery("select col1 from testtable"),true);
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),sizeof(buffer));
	checkSuccess(bytestring::compare(cur->getField(0,(uint32_t)0),
						buffer,sizeof(buffer)),0);
	checkSuccess(cur->sendQuery("delete from testtable"),true);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable values (_binary'''''')"),true);
	checkSuccess(cur->sendQuery("select col1 from testtable"),true);
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),2);
	checkSuccess(charstring::compare(cur->getField(0,(uint32_t)0),"''"),0);
	checkSuccess(cur->sendQuery("delete from testtable"),true);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable values (_binary'\"\"')"),true);
	checkSuccess(cur->sendQuery("select col1 from testtable"),true);
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),2);
	checkSuccess(charstring::compare(cur->getField(0,(uint32_t)0),"\"\""),0);
	checkSuccess(cur->sendQuery("delete from testtable"),true);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable values (_binary'\0\"\"')",43),true);
	checkSuccess(cur->sendQuery("select col1 from testtable"),true);
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),3);
	checkSuccess(bytestring::compare(cur->getField(0,(uint32_t)0),"\0\"\"",3),0);
	checkSuccess(cur->sendQuery("delete from testtable"),true);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable values (_binary'\\\0\\\"\\\"')",46),true);
	checkSuccess(cur->sendQuery("select col1 from testtable"),true);
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),3);
	checkSuccess(bytestring::compare(cur->getField(0,(uint32_t)0),"\0\"\"",3),0);
	checkSuccess(cur->sendQuery("delete from testtable"),true);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable values (_binary'\\\\\\'')",44),true);
	checkSuccess(cur->sendQuery("select col1 from testtable"),true);
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),2);
	checkSuccess(bytestring::compare(cur->getField(0,(uint32_t)0),"\\\'",2),0);
	checkSuccess(cur->sendQuery("delete from testtable"),true);
	cur->sendQuery("drop table testtable");
	stdoutput.printf("\n");

	// invalid queries...
	stdoutput.printf("INVALID QUERIES: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testtinyint"),0);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	stdoutput.printf("\n");

	delete secondcur;
	delete secondcon;
	delete cur;
	delete con;

#ifdef PROFILING
}
#endif

	return 0;
}
