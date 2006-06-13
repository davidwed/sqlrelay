// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("failure ");
			delete cur;
			delete con;
			exit(0);
		}
	}

	if (!strcmp(value,success)) {
		printf("success ");
	} else {
		printf("failure ");
		delete cur;
		delete con;
		exit(0);
	}
}

void checkSuccess(const char *value, const char *success, size_t length) {

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("failure ");
			delete cur;
			delete con;
			exit(0);
		}
	}

	if (!strncmp(value,success,length)) {
		printf("success ");
	} else {
		printf("failure ");
		delete cur;
		delete con;
		exit(0);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("failure ");
		delete cur;
		delete con;
		exit(0);
	}
}

void checkSuccess(double value, double success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("failure ");
		delete cur;
		delete con;
		exit(0);
	}
}

int	main(int argc, char **argv) {

	const char	*subvars[4]={"var1","var2","var3",NULL};
	const char	*subvalstrings[3]={"hi","hello","bye"};
	int64_t		subvallongs[3]={1,2,3};
	double		subvaldoubles[3]={10.55,10.556,10.5556};
	uint32_t	precs[3]={4,5,6};
	uint32_t	scales[3]={2,3,4};
	int64_t		numvar;
	const char	*clobvar;
	uint32_t	clobvarlength;
	const char	*blobvar;
	uint32_t	blobvarlength;
	const char	*stringvar;
	double		floatvar;	
	const char * const *cols;
	const char * const *fields;
	uint16_t	port;
	char		*socket;
	uint16_t	id;
	char		*filename;
	uint32_t	*fieldlens;

	// usage...
	if (argc<5) {
		printf("usage: postgresql host port socket user password\n");
		exit(0);
	}


	// instantiation
	con=new sqlrconnection(argv[1],atoi(argv[2]), 
					argv[3],argv[4],argv[5],0,1);
	cur=new sqlrcursor(con);

	printf("IDENTIFY: \n");
	checkSuccess(con->identify(),"router");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccess(con->ping(),1);
	printf("\n");

	printf("FILTERED-OUT QUERIES: \n");
	checkSuccess(cur->sendQuery("create table junktable (col1 int)"),0);
	checkSuccess(cur->sendQuery("insert into junktable values (1)"),0);
	checkSuccess(cur->sendQuery("update junktable set col1=2"),0);
	checkSuccess(cur->sendQuery("delete from junktable"),0);
	checkSuccess(cur->sendQuery("drop table junktable (col1 int)"),0);
	printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable1");
	cur->sendQuery("drop table testtable2");

	printf("CREATE TESTTABLES: \n");
	checkSuccess(cur->sendQuery("create table testtable1 (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
	checkSuccess(cur->sendQuery("create table testtable2 (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
	printf("\n");

	printf("BEGIN TRANSCTION: \n");
	checkSuccess(cur->sendQuery("begin"),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable1 values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable1 values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable1 values (3,3.3,3.3,3,'testchar3','testvarchar3','01/01/2003','03:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable1 values (4,4.4,4.4,4,'testchar4','testvarchar4','01/01/2004','04:00:00',NULL)"),1);
	printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable2 values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable2 values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable2 values (3,3.3,3.3,3,'testchar3','testvarchar3','01/01/2003','03:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable2 values (4,4.4,4.4,4,'testchar4','testvarchar4','01/01/2004','04:00:00',NULL)"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),1);
	printf("\n");

	printf("BIND BY NAME: \n");
	cur->prepareQuery("insert into testtable1 values ($1,$2,$3,$4,$5,$6,$7,$8)");
	checkSuccess(cur->countBindVariables(),8);
	cur->inputBind("1",5);
	cur->inputBind("2",5.5,4,2);
	cur->inputBind("3",5.5,4,2);
	cur->inputBind("4",5);
	cur->inputBind("5","testchar5");
	cur->inputBind("6","testvarchar5");
	cur->inputBind("7","01/01/2005");
	cur->inputBind("8","05:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",6);
	cur->inputBind("2",6.6,4,2);
	cur->inputBind("3",6.6,4,2);
	cur->inputBind("4",6);
	cur->inputBind("5","testchar6");
	cur->inputBind("6","testvarchar6");
	cur->inputBind("7","01/01/2006");
	cur->inputBind("8","06:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",7);
	cur->inputBind("2",7.7,4,2);
	cur->inputBind("3",7.7,4,2);
	cur->inputBind("4",7);
	cur->inputBind("5","testchar7");
	cur->inputBind("6","testvarchar7");
	cur->inputBind("7","01/01/2007");
	cur->inputBind("8","07:00:00");
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("1",8);
	cur->inputBind("2",8.8,4,2);
	cur->inputBind("3",8.8,4,2);
	cur->inputBind("4",8);
	cur->inputBind("5","testchar8");
	cur->inputBind("6","testvarchar8");
	cur->inputBind("7","01/01/2008");
	cur->inputBind("8","08:00:00");
	cur->inputBind("9","junkvalue");
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("BIND BY NAME: \n");
	cur->prepareQuery("insert into testtable2 values ($1,$2,$3,$4,$5,$6,$7,$8)");
	checkSuccess(cur->countBindVariables(),8);
	cur->inputBind("1",5);
	cur->inputBind("2",5.5,4,2);
	cur->inputBind("3",5.5,4,2);
	cur->inputBind("4",5);
	cur->inputBind("5","testchar5");
	cur->inputBind("6","testvarchar5");
	cur->inputBind("7","01/01/2005");
	cur->inputBind("8","05:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",6);
	cur->inputBind("2",6.6,4,2);
	cur->inputBind("3",6.6,4,2);
	cur->inputBind("4",6);
	cur->inputBind("5","testchar6");
	cur->inputBind("6","testvarchar6");
	cur->inputBind("7","01/01/2006");
	cur->inputBind("8","06:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",7);
	cur->inputBind("2",7.7,4,2);
	cur->inputBind("3",7.7,4,2);
	cur->inputBind("4",7);
	cur->inputBind("5","testchar7");
	cur->inputBind("6","testvarchar7");
	cur->inputBind("7","01/01/2007");
	cur->inputBind("8","07:00:00");
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("1",8);
	cur->inputBind("2",8.8,4,2);
	cur->inputBind("3",8.8,4,2);
	cur->inputBind("4",8);
	cur->inputBind("5","testchar8");
	cur->inputBind("6","testvarchar8");
	cur->inputBind("7","01/01/2008");
	cur->inputBind("8","08:00:00");
	cur->inputBind("9","junkvalue");
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable1 order by testint"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),9);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccess(cur->getColumnName(0),"testint");
	checkSuccess(cur->getColumnName(1),"testfloat");
	checkSuccess(cur->getColumnName(2),"testreal");
	checkSuccess(cur->getColumnName(3),"testsmallint");
	checkSuccess(cur->getColumnName(4),"testchar");
	checkSuccess(cur->getColumnName(5),"testvarchar");
	checkSuccess(cur->getColumnName(6),"testdate");
	checkSuccess(cur->getColumnName(7),"testtime");
	checkSuccess(cur->getColumnName(8),"testtimestamp");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testint");
	checkSuccess(cols[1],"testfloat");
	checkSuccess(cols[2],"testreal");
	checkSuccess(cols[3],"testsmallint");
	checkSuccess(cols[4],"testchar");
	checkSuccess(cols[5],"testvarchar");
	checkSuccess(cols[6],"testdate");
	checkSuccess(cols[7],"testtime");
	checkSuccess(cols[8],"testtimestamp");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType((uint32_t)0),"int4");
	checkSuccess(cur->getColumnType("testint"),"int4");
	checkSuccess(cur->getColumnType(1),"float8");
	checkSuccess(cur->getColumnType("testfloat"),"float8");
	checkSuccess(cur->getColumnType(2),"float4");
	checkSuccess(cur->getColumnType("testreal"),"float4");
	checkSuccess(cur->getColumnType(3),"int2");
	checkSuccess(cur->getColumnType("testsmallint"),"int2");
	checkSuccess(cur->getColumnType(4),"bpchar");
	checkSuccess(cur->getColumnType("testchar"),"bpchar");
	checkSuccess(cur->getColumnType(5),"varchar");
	checkSuccess(cur->getColumnType("testvarchar"),"varchar");
	checkSuccess(cur->getColumnType(6),"date");
	checkSuccess(cur->getColumnType("testdate"),"date");
	checkSuccess(cur->getColumnType(7),"time");
	checkSuccess(cur->getColumnType("testtime"),"time");
	checkSuccess(cur->getColumnType(8),"timestamp");
	checkSuccess(cur->getColumnType("testtimestamp"),"timestamp");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),4);
	checkSuccess(cur->getColumnLength("testint"),4);
	checkSuccess(cur->getColumnLength(1),8);
	checkSuccess(cur->getColumnLength("testfloat"),8);
	checkSuccess(cur->getColumnLength(2),4);
	checkSuccess(cur->getColumnLength("testreal"),4);
	checkSuccess(cur->getColumnLength(3),2);
	checkSuccess(cur->getColumnLength("testsmallint"),2);
	checkSuccess(cur->getColumnLength(4),44);
	checkSuccess(cur->getColumnLength("testchar"),44);
	checkSuccess(cur->getColumnLength(5),44);
	checkSuccess(cur->getColumnLength("testvarchar"),44);
	checkSuccess(cur->getColumnLength(6),4);
	checkSuccess(cur->getColumnLength("testdate"),4);
	checkSuccess(cur->getColumnLength(7),8);
	checkSuccess(cur->getColumnLength("testtime"),8);
	checkSuccess(cur->getColumnLength(8),8);
	checkSuccess(cur->getColumnLength("testtimestamp"),8);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest("testint"),1);
	checkSuccess(cur->getLongest(1),3);
	checkSuccess(cur->getLongest("testfloat"),3);
	checkSuccess(cur->getLongest(2),3);
	checkSuccess(cur->getLongest("testreal"),3);
	checkSuccess(cur->getLongest(3),1);
	checkSuccess(cur->getLongest("testsmallint"),1);
	checkSuccess(cur->getLongest(4),40);
	checkSuccess(cur->getLongest("testchar"),40);
	checkSuccess(cur->getLongest(5),12);
	checkSuccess(cur->getLongest("testvarchar"),12);
	checkSuccess(cur->getLongest(6),10);
	checkSuccess(cur->getLongest("testdate"),10);
	checkSuccess(cur->getLongest(7),8);
	checkSuccess(cur->getLongest("testtime"),8);
	printf("\n");

	printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	printf("\n");

	printf("TOTAL ROWS: \n");
	checkSuccess(cur->totalRows(),8);
	printf("\n");

	printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	printf("\n");

	printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	printf("\n");

	printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"1.1");
	checkSuccess(cur->getField(0,2),"1.1");
	checkSuccess(cur->getField(0,3),"1");
	checkSuccess(cur->getField(0,4),"testchar1                               ");
	checkSuccess(cur->getField(0,5),"testvarchar1");
	checkSuccess(cur->getField(0,6),"2001-01-01");
	checkSuccess(cur->getField(0,7),"01:00:00");
	printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(7,1),"8.8");
	checkSuccess(cur->getField(7,2),"8.8");
	checkSuccess(cur->getField(7,3),"8");
	checkSuccess(cur->getField(7,4),"testchar8                               ");
	checkSuccess(cur->getField(7,5),"testvarchar8");
	checkSuccess(cur->getField(7,6),"2008-01-01");
	checkSuccess(cur->getField(7,7),"08:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(0,1),3);
	checkSuccess(cur->getFieldLength(0,2),3);
	checkSuccess(cur->getFieldLength(0,3),1);
	checkSuccess(cur->getFieldLength(0,4),40);
	checkSuccess(cur->getFieldLength(0,5),12);
	checkSuccess(cur->getFieldLength(0,6),10);
	checkSuccess(cur->getFieldLength(0,7),8);
	printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(7,1),3);
	checkSuccess(cur->getFieldLength(7,2),3);
	checkSuccess(cur->getFieldLength(7,3),1);
	checkSuccess(cur->getFieldLength(7,4),40);
	checkSuccess(cur->getFieldLength(7,5),12);
	checkSuccess(cur->getFieldLength(7,6),10);
	checkSuccess(cur->getFieldLength(7,7),8);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"testint"),"1");
	checkSuccess(cur->getField(0,"testfloat"),"1.1");
	checkSuccess(cur->getField(0,"testreal"),"1.1");
	checkSuccess(cur->getField(0,"testsmallint"),"1");
	checkSuccess(cur->getField(0,"testchar"),"testchar1                               ");
	checkSuccess(cur->getField(0,"testvarchar"),"testvarchar1");
	checkSuccess(cur->getField(0,"testdate"),"2001-01-01");
	checkSuccess(cur->getField(0,"testtime"),"01:00:00");
	printf("\n");
	checkSuccess(cur->getField(7,"testint"),"8");
	checkSuccess(cur->getField(7,"testfloat"),"8.8");
	checkSuccess(cur->getField(7,"testreal"),"8.8");
	checkSuccess(cur->getField(7,"testsmallint"),"8");
	checkSuccess(cur->getField(7,"testchar"),"testchar8                               ");
	checkSuccess(cur->getField(7,"testvarchar"),"testvarchar8");
	checkSuccess(cur->getField(7,"testdate"),"2008-01-01");
	checkSuccess(cur->getField(7,"testtime"),"08:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"testint"),1);
	checkSuccess(cur->getFieldLength(0,"testfloat"),3);
	checkSuccess(cur->getFieldLength(0,"testreal"),3);
	checkSuccess(cur->getFieldLength(0,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(0,"testchar"),40);
	checkSuccess(cur->getFieldLength(0,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(0,"testdate"),10);
	checkSuccess(cur->getFieldLength(0,"testtime"),8);
	printf("\n");
	checkSuccess(cur->getFieldLength(7,"testint"),1);
	checkSuccess(cur->getFieldLength(7,"testfloat"),3);
	checkSuccess(cur->getFieldLength(7,"testreal"),3);
	checkSuccess(cur->getFieldLength(7,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(7,"testchar"),40);
	checkSuccess(cur->getFieldLength(7,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(7,"testdate"),10);
	checkSuccess(cur->getFieldLength(7,"testtime"),8);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	checkSuccess(fields[1],"1.1");
	checkSuccess(fields[2],"1.1");
	checkSuccess(fields[3],"1");
	checkSuccess(fields[4],"testchar1                               ");
	checkSuccess(fields[5],"testvarchar1");
	checkSuccess(fields[6],"2001-01-01");
	checkSuccess(fields[7],"01:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	checkSuccess(fieldlens[1],3);
	checkSuccess(fieldlens[2],3);
	checkSuccess(fieldlens[3],1);
	checkSuccess(fieldlens[4],40);
	checkSuccess(fieldlens[5],12);
	checkSuccess(fieldlens[6],10);
	checkSuccess(fieldlens[7],8);
	printf("\n");

	printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable1 order by testint"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),9);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccess(cur->getColumnName(0),"testint");
	checkSuccess(cur->getColumnName(1),"testfloat");
	checkSuccess(cur->getColumnName(2),"testreal");
	checkSuccess(cur->getColumnName(3),"testsmallint");
	checkSuccess(cur->getColumnName(4),"testchar");
	checkSuccess(cur->getColumnName(5),"testvarchar");
	checkSuccess(cur->getColumnName(6),"testdate");
	checkSuccess(cur->getColumnName(7),"testtime");
	checkSuccess(cur->getColumnName(8),"testtimestamp");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testint");
	checkSuccess(cols[1],"testfloat");
	checkSuccess(cols[2],"testreal");
	checkSuccess(cols[3],"testsmallint");
	checkSuccess(cols[4],"testchar");
	checkSuccess(cols[5],"testvarchar");
	checkSuccess(cols[6],"testdate");
	checkSuccess(cols[7],"testtime");
	checkSuccess(cols[8],"testtimestamp");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType((uint32_t)0),"int4");
	checkSuccess(cur->getColumnType("testint"),"int4");
	checkSuccess(cur->getColumnType(1),"float8");
	checkSuccess(cur->getColumnType("testfloat"),"float8");
	checkSuccess(cur->getColumnType(2),"float4");
	checkSuccess(cur->getColumnType("testreal"),"float4");
	checkSuccess(cur->getColumnType(3),"int2");
	checkSuccess(cur->getColumnType("testsmallint"),"int2");
	checkSuccess(cur->getColumnType(4),"bpchar");
	checkSuccess(cur->getColumnType("testchar"),"bpchar");
	checkSuccess(cur->getColumnType(5),"varchar");
	checkSuccess(cur->getColumnType("testvarchar"),"varchar");
	checkSuccess(cur->getColumnType(6),"date");
	checkSuccess(cur->getColumnType("testdate"),"date");
	checkSuccess(cur->getColumnType(7),"time");
	checkSuccess(cur->getColumnType("testtime"),"time");
	checkSuccess(cur->getColumnType(8),"timestamp");
	checkSuccess(cur->getColumnType("testtimestamp"),"timestamp");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),4);
	checkSuccess(cur->getColumnLength("testint"),4);
	checkSuccess(cur->getColumnLength(1),8);
	checkSuccess(cur->getColumnLength("testfloat"),8);
	checkSuccess(cur->getColumnLength(2),4);
	checkSuccess(cur->getColumnLength("testreal"),4);
	checkSuccess(cur->getColumnLength(3),2);
	checkSuccess(cur->getColumnLength("testsmallint"),2);
	checkSuccess(cur->getColumnLength(4),44);
	checkSuccess(cur->getColumnLength("testchar"),44);
	checkSuccess(cur->getColumnLength(5),44);
	checkSuccess(cur->getColumnLength("testvarchar"),44);
	checkSuccess(cur->getColumnLength(6),4);
	checkSuccess(cur->getColumnLength("testdate"),4);
	checkSuccess(cur->getColumnLength(7),8);
	checkSuccess(cur->getColumnLength("testtime"),8);
	checkSuccess(cur->getColumnLength(8),8);
	checkSuccess(cur->getColumnLength("testtimestamp"),8);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest("testint"),1);
	checkSuccess(cur->getLongest(1),3);
	checkSuccess(cur->getLongest("testfloat"),3);
	checkSuccess(cur->getLongest(2),3);
	checkSuccess(cur->getLongest("testreal"),3);
	checkSuccess(cur->getLongest(3),1);
	checkSuccess(cur->getLongest("testsmallint"),1);
	checkSuccess(cur->getLongest(4),40);
	checkSuccess(cur->getLongest("testchar"),40);
	checkSuccess(cur->getLongest(5),12);
	checkSuccess(cur->getLongest("testvarchar"),12);
	checkSuccess(cur->getLongest(6),10);
	checkSuccess(cur->getLongest("testdate"),10);
	checkSuccess(cur->getLongest(7),8);
	checkSuccess(cur->getLongest("testtime"),8);
	printf("\n");

	printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	printf("\n");

	printf("TOTAL ROWS: \n");
	checkSuccess(cur->totalRows(),8);
	printf("\n");

	printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	printf("\n");

	printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	printf("\n");

	printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"1.1");
	checkSuccess(cur->getField(0,2),"1.1");
	checkSuccess(cur->getField(0,3),"1");
	checkSuccess(cur->getField(0,4),"testchar1                               ");
	checkSuccess(cur->getField(0,5),"testvarchar1");
	checkSuccess(cur->getField(0,6),"2001-01-01");
	checkSuccess(cur->getField(0,7),"01:00:00");
	printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(7,1),"8.8");
	checkSuccess(cur->getField(7,2),"8.8");
	checkSuccess(cur->getField(7,3),"8");
	checkSuccess(cur->getField(7,4),"testchar8                               ");
	checkSuccess(cur->getField(7,5),"testvarchar8");
	checkSuccess(cur->getField(7,6),"2008-01-01");
	checkSuccess(cur->getField(7,7),"08:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(0,1),3);
	checkSuccess(cur->getFieldLength(0,2),3);
	checkSuccess(cur->getFieldLength(0,3),1);
	checkSuccess(cur->getFieldLength(0,4),40);
	checkSuccess(cur->getFieldLength(0,5),12);
	checkSuccess(cur->getFieldLength(0,6),10);
	checkSuccess(cur->getFieldLength(0,7),8);
	printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(7,1),3);
	checkSuccess(cur->getFieldLength(7,2),3);
	checkSuccess(cur->getFieldLength(7,3),1);
	checkSuccess(cur->getFieldLength(7,4),40);
	checkSuccess(cur->getFieldLength(7,5),12);
	checkSuccess(cur->getFieldLength(7,6),10);
	checkSuccess(cur->getFieldLength(7,7),8);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"testint"),"1");
	checkSuccess(cur->getField(0,"testfloat"),"1.1");
	checkSuccess(cur->getField(0,"testreal"),"1.1");
	checkSuccess(cur->getField(0,"testsmallint"),"1");
	checkSuccess(cur->getField(0,"testchar"),"testchar1                               ");
	checkSuccess(cur->getField(0,"testvarchar"),"testvarchar1");
	checkSuccess(cur->getField(0,"testdate"),"2001-01-01");
	checkSuccess(cur->getField(0,"testtime"),"01:00:00");
	printf("\n");
	checkSuccess(cur->getField(7,"testint"),"8");
	checkSuccess(cur->getField(7,"testfloat"),"8.8");
	checkSuccess(cur->getField(7,"testreal"),"8.8");
	checkSuccess(cur->getField(7,"testsmallint"),"8");
	checkSuccess(cur->getField(7,"testchar"),"testchar8                               ");
	checkSuccess(cur->getField(7,"testvarchar"),"testvarchar8");
	checkSuccess(cur->getField(7,"testdate"),"2008-01-01");
	checkSuccess(cur->getField(7,"testtime"),"08:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"testint"),1);
	checkSuccess(cur->getFieldLength(0,"testfloat"),3);
	checkSuccess(cur->getFieldLength(0,"testreal"),3);
	checkSuccess(cur->getFieldLength(0,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(0,"testchar"),40);
	checkSuccess(cur->getFieldLength(0,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(0,"testdate"),10);
	checkSuccess(cur->getFieldLength(0,"testtime"),8);
	printf("\n");
	checkSuccess(cur->getFieldLength(7,"testint"),1);
	checkSuccess(cur->getFieldLength(7,"testfloat"),3);
	checkSuccess(cur->getFieldLength(7,"testreal"),3);
	checkSuccess(cur->getFieldLength(7,"testsmallint"),1);
	checkSuccess(cur->getFieldLength(7,"testchar"),40);
	checkSuccess(cur->getFieldLength(7,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(7,"testdate"),10);
	checkSuccess(cur->getFieldLength(7,"testtime"),8);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	checkSuccess(fields[1],"1.1");
	checkSuccess(fields[2],"1.1");
	checkSuccess(fields[3],"1");
	checkSuccess(fields[4],"testchar1                               ");
	checkSuccess(fields[5],"testvarchar1");
	checkSuccess(fields[6],"2001-01-01");
	checkSuccess(fields[7],"01:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	checkSuccess(fieldlens[1],3);
	checkSuccess(fieldlens[2],3);
	checkSuccess(fieldlens[3],1);
	checkSuccess(fieldlens[4],40);
	checkSuccess(fieldlens[5],12);
	checkSuccess(fieldlens[6],10);
	checkSuccess(fieldlens[7],8);
	printf("\n");

	printf("COMMIT AND ROLLBACK: \n");
	secondcon=new sqlrconnection(argv[1],
				atoi(argv[2]), 
				argv[3],argv[4],argv[5],0,1);
	secondcur=new sqlrcursor(secondcon);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable1"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"0");
	checkSuccess(secondcur->sendQuery("select count(*) from testtable2"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"0");
	checkSuccess(con->commit(),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable1"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"8");
	checkSuccess(secondcur->sendQuery("select count(*) from testtable2"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"8");
	//checkSuccess(con->autoCommitOn(),1);
	checkSuccess(cur->sendQuery("insert into testtable1 values (10,10.1,10.1,10,'testchar10','testvarchar10','01/01/2010','10:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable2 values (10,10.1,10.1,10,'testchar10','testvarchar10','01/01/2010','10:00:00',NULL)"),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable1"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"9");
	checkSuccess(secondcur->sendQuery("select count(*) from testtable2"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"9");
	//checkSuccess(con->autoCommitOff(),1);
	checkSuccess(cur->sendQuery("begin"),1);
	printf("\n");

	printf("OUTPUT BIND BY POSITION: \n");
	cur->prepareQuery("begin  :1:=1; :2:='hello'; :3:=2.5; end;");
	cur->defineOutputBindInteger("1");
	cur->defineOutputBindString("2",10);
	cur->defineOutputBindDouble("3");
	checkSuccess(cur->executeQuery(),1);
	numvar=cur->getOutputBindInteger("1");
	stringvar=cur->getOutputBindString("2");
	floatvar=cur->getOutputBindDouble("3");
	checkSuccess(numvar,1);
	checkSuccess(stringvar,"hello");
	checkSuccess(floatvar,2.5);
	printf("\n");

	printf("OUTPUT BIND BY NAME: \n");
	cur->prepareQuery("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;");
	cur->defineOutputBindInteger("numvar");
	cur->defineOutputBindString("stringvar",10);
	cur->defineOutputBindDouble("floatvar");
	checkSuccess(cur->executeQuery(),1);
	numvar=cur->getOutputBindInteger("numvar");
	stringvar=cur->getOutputBindString("stringvar");
	floatvar=cur->getOutputBindDouble("floatvar");
	checkSuccess(numvar,1);
	checkSuccess(stringvar,"hello");
	checkSuccess(floatvar,2.5);
	printf("\n");

	printf("OUTPUT BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->defineOutputBindInteger("numvar");
	cur->defineOutputBindString("stringvar",10);
	cur->defineOutputBindDouble("floatvar");
	cur->defineOutputBindString("dummyvar",10);
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	numvar=cur->getOutputBindInteger("numvar");
	stringvar=cur->getOutputBindString("stringvar");
	floatvar=cur->getOutputBindDouble("floatvar");
	checkSuccess(numvar,1);
	checkSuccess(stringvar,"hello");
	checkSuccess(floatvar,2.5);
	printf("\n");

	printf("CLOB AND BLOB OUTPUT BIND: \n");
	cur->sendQuery("drop table testtable3");
	checkSuccess(cur->sendQuery("create table testtable3 (testclob clob, testblob blob)"),1);
	cur->prepareQuery("insert into testtable3 values ('hello',:var1)");
	cur->inputBindBlob("var1","hello",5);
	checkSuccess(cur->executeQuery(),1);
	cur->prepareQuery("begin select testclob into :clobvar from testtable3;  select testblob into :blobvar from testtable3; end;");
	cur->defineOutputBindClob("clobvar");
	cur->defineOutputBindBlob("blobvar");
	checkSuccess(cur->executeQuery(),1);
	clobvar=cur->getOutputBindClob("clobvar");
	clobvarlength=cur->getOutputBindLength("clobvar");
	blobvar=cur->getOutputBindBlob("blobvar");
	blobvarlength=cur->getOutputBindLength("blobvar");
	checkSuccess(clobvar,"hello",5);
	checkSuccess(clobvarlength,5);
	checkSuccess(blobvar,"hello",5);
	checkSuccess(blobvarlength,5);
	cur->sendQuery("drop table testtable3");
	printf("\n");

	printf("NULL AND EMPTY CLOBS AND CLOBS: \n");
	cur->getNullsAsNulls();
	cur->sendQuery("create table testtable3 (testclob1 clob, testclob2 clob, testblob1 blob, testblob2 blob)");
	cur->prepareQuery("insert into testtable3 values (:var1,:var2,:var3,:var4)");
	cur->inputBindClob("var1","",0);
	cur->inputBindClob("var2",NULL,0);
	cur->inputBindBlob("var3","",0);
	cur->inputBindBlob("var4",NULL,0);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select * from testtable3");
	checkSuccess(cur->getField(0,(uint32_t)0),NULL);
	checkSuccess(cur->getField(0,1),NULL);
	checkSuccess(cur->getField(0,2),NULL);
	checkSuccess(cur->getField(0,3),NULL);
	cur->sendQuery("drop table testtable3");
	printf("\n");


	printf("LONG CLOB: \n");
	cur->sendQuery("drop table testtable3");
	cur->sendQuery("create table testtable3 (testclob clob)");
	cur->prepareQuery("insert into testtable3 values (:clobval)");
	char	clobval[8*1024+1];
	for (int i=0; i<8*1024; i++) {
		clobval[i]='C';
	}
	clobval[8*1024]=(char)NULL;
	cur->inputBindClob("clobval",clobval,8*1024);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testclob from testtable3");
	checkSuccess(clobval,cur->getField(0,"testclob"));
	cur->prepareQuery("begin select testclob into :clobbindval from testtable3; end;");
	cur->defineOutputBindClob("clobbindval");
	checkSuccess(cur->executeQuery(),1);
	const char	*clobbindvar=cur->getOutputBindClob("clobbindval");
	checkSuccess(cur->getOutputBindLength("clobbindval"),8*1024);
	checkSuccess(clobval,clobbindvar);
	cur->sendQuery("drop table testtable3");
	printf("\n");


	printf("LONG OUTPUT BIND\n");
	cur->sendQuery("drop table testtable3");
	cur->sendQuery("create table testtable3 (testval varchar2(4000))");
	char	testval[4001];
	testval[4000]=(char)NULL;
	cur->prepareQuery("insert into testtable3 values (:testval)");
	for (int i=0; i<4000; i++) {
		testval[i]='C';
	}
	cur->inputBind("testval",testval);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testval from testtable3");
	checkSuccess(testval,cur->getField(0,"testval"));
	char	query[4000+25];
	sprintf(query,"begin :bindval:='%s'; end;",testval);
	cur->prepareQuery(query);
	cur->defineOutputBindString("bindval",4000);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindLength("bindval"),4000);
	checkSuccess(cur->getOutputBindString("bindval"),testval);
	cur->sendQuery("drop table testtable3");
	printf("\n");

	printf("NEGATIVE INPUT BIND\n");
	cur->sendQuery("drop table testtable3");
	cur->sendQuery("create table testtable3 (testval number)");
	cur->prepareQuery("insert into testtable3 values (:testval)");
	cur->inputBind("testval",-1);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testval from testtable3");
	checkSuccess(cur->getField(0,"testval"),"-1");
	cur->sendQuery("drop table testtable3");
	printf("\n");


	printf("CURSOR BINDS: \n");
	checkSuccess(cur->sendQuery("create table testtable3 (testnumber number)"),1);
	checkSuccess(cur->sendQuery("insert into testtable3 values (1)"),1);
	checkSuccess(cur->sendQuery("insert into testtable3 values (2)"),1);
	checkSuccess(cur->sendQuery("insert into testtable3 values (3)"),1);
	checkSuccess(cur->sendQuery("insert into testtable3 values (4)"),1);
	checkSuccess(cur->sendQuery("insert into testtable3 values (5)"),1);
	checkSuccess(cur->sendQuery("insert into testtable3 values (6)"),1);
	checkSuccess(cur->sendQuery("insert into testtable3 values (7)"),1);
	checkSuccess(cur->sendQuery("insert into testtable3 values (8)"),1);
	printf("\n");
	checkSuccess(cur->sendQuery("create or replace package types is type cursorType is ref cursor; end;"),1);
	checkSuccess(cur->sendQuery("create or replace function sp_testtable(value in number) return types.cursortype is l_cursor    types.cursorType; begin open l_cursor for select * from testtable3 where testnumber>value; return l_cursor; end;"),1);
	cur->prepareQuery("begin  :curs1:=sp_testtable(5);  :curs2:=sp_testtable(0); end;");
	cur->defineOutputBindCursor("curs1");
	cur->defineOutputBindCursor("curs2");
	checkSuccess(cur->executeQuery(),1);
	sqlrcursor	*bindcur1=cur->getOutputBindCursor("curs1");
	checkSuccess(bindcur1->fetchFromBindCursor(),1);
	checkSuccess(bindcur1->getField(0,(uint32_t)0),"6");
	checkSuccess(bindcur1->getField(1,(uint32_t)0),"7");
	checkSuccess(bindcur1->getField(2,(uint32_t)0),"8");
	delete bindcur1;
	sqlrcursor	*bindcur2=cur->getOutputBindCursor("curs2");
	checkSuccess(bindcur2->fetchFromBindCursor(),1);
	checkSuccess(bindcur2->getField(0,(uint32_t)0),"1");
	checkSuccess(bindcur2->getField(1,(uint32_t)0),"2");
	checkSuccess(bindcur2->getField(2,(uint32_t)0),"3");
	delete bindcur2;
	checkSuccess(cur->sendQuery("drop package types"),1);
	cur->sendQuery("drop table testtable3");
	printf("\n");


	// temporary tables
	printf("TEMPORARY TABLES: \n");
	cur->sendQuery("drop table temptabledelete\n");
	cur->sendQuery("create global temporary table temptabledelete (col1 number) on commit delete rows");
	checkSuccess(cur->sendQuery("insert into temptabledelete values (1)"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptabledelete"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(con->commit(),1);
	checkSuccess(cur->sendQuery("begin"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptabledelete"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"0");
	cur->sendQuery("drop table temptabledelete\n");
	printf("\n");
	cur->sendQuery("drop table temptablepreserve\n");
	cur->sendQuery("create global temporary table temptablepreserve (col1 number) on commit preserve rows");
	checkSuccess(cur->sendQuery("insert into temptablepreserve values (1)"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(con->commit(),1);
	checkSuccess(cur->sendQuery("begin"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	con->endSession();
	printf("\n");
	checkSuccess(cur->sendQuery("begin"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"0");
	cur->sendQuery("drop table temptablepreserve\n");
	printf("\n");

	printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable1 order by testint"),1);
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=strdup(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeResultSet(id),1);
	checkSuccess(cur->getField(4,(uint32_t)0),NULL);
	checkSuccess(cur->getField(5,(uint32_t)0),NULL);
	checkSuccess(cur->getField(6,(uint32_t)0),NULL);
	checkSuccess(cur->getField(7,(uint32_t)0),NULL);
	printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable1");
	cur->sendQuery("drop table testtable2");

	printf("\n");
}
