// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

void checkSuccess(char *value, char *success) {

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

int	main(int argc, char **argv) {

	char	*dbtype;
	const char	*subvars[4]={"var1","var2","var3",NULL};
	const char	*subvalstrings[3]={"hi","hello","bye"};
	long	subvallongs[3]={1,2,3};
	double	subvaldoubles[3]={10.55,10.556,10.5556};
	unsigned short	precs[3]={4,5,6};
	unsigned short	scales[3]={2,3,4};
	char	*numvar;
	char	*stringvar;
	char	*floatvar;
	char	**cols;
	char	**fields;
	int	port;
	char	*socket;
	int	id;
	char	*filename;
	long	*fieldlens;

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
	checkSuccess(con->identify(),"postgresql");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccess(con->ping(),1);
	printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	printf("CREATE TEMPTABLE: \n");
	checkSuccess(cur->sendQuery("create table testtable (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
	printf("\n");

	printf("BEGIN TRANSCTION: \n");
	checkSuccess(cur->sendQuery("begin"),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (3,3.3,3.3,3,'testchar3','testvarchar3','01/01/2003','03:00:00',NULL)"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (4,4.4,4.4,4,'testchar4','testvarchar4','01/01/2004','04:00:00',NULL)"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),1);
	printf("\n");

	printf("BIND BY NAME: \n");
	cur->prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8)");
	checkSuccess(cur->countBindVariables(),8);
	cur->inputBind("var1",5);
	cur->inputBind("var2",5.5,4,2);
	cur->inputBind("var3",5.5,4,2);
	cur->inputBind("var4",5);
	cur->inputBind("var5","testchar5");
	cur->inputBind("var6","testvarchar5");
	cur->inputBind("var7","01/01/2005");
	cur->inputBind("var8","05:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("var1",6);
	cur->inputBind("var2",6.6,4,2);
	cur->inputBind("var3",6.6,4,2);
	cur->inputBind("var4",6);
	cur->inputBind("var5","testchar6");
	cur->inputBind("var6","testvarchar6");
	cur->inputBind("var7","01/01/2006");
	cur->inputBind("var8","06:00:00");
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("var1",7);
	cur->inputBind("var2",7.7,4,2);
	cur->inputBind("var3",7.7,4,2);
	cur->inputBind("var4",7);
	cur->inputBind("var5","testchar7");
	cur->inputBind("var6","testvarchar7");
	cur->inputBind("var7","01/01/2007");
	cur->inputBind("var8","07:00:00");
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("var1",8);
	cur->inputBind("var2",8.8,4,2);
	cur->inputBind("var3",8.8,4,2);
	cur->inputBind("var4",8);
	cur->inputBind("var5","testchar8");
	cur->inputBind("var6","testvarchar8");
	cur->inputBind("var7","01/01/2008");
	cur->inputBind("var8","08:00:00");
	cur->inputBind("var9","junkvalue");
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->getColumnType(0),"int4");
	checkSuccess(cur->getColumnType("testint"),"int4");
	checkSuccess(cur->getColumnType(1),"float8");
	checkSuccess(cur->getColumnType("testfloat"),"float8");
	checkSuccess(cur->getColumnType(2),"float8");
	checkSuccess(cur->getColumnType("testreal"),"float8");
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
	checkSuccess(cur->getColumnLength(0),4);
	checkSuccess(cur->getColumnLength("testint"),4);
	checkSuccess(cur->getColumnLength(1),8);
	checkSuccess(cur->getColumnLength("testfloat"),8);
	checkSuccess(cur->getColumnLength(2),8);
	checkSuccess(cur->getColumnLength("testreal"),8);
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
	checkSuccess(cur->getColumnLength(8),4);
	checkSuccess(cur->getColumnLength("testtimestamp"),4);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest(0),1);
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
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(0,1),"1.1");
	checkSuccess(cur->getField(0,2),"1.1");
	checkSuccess(cur->getField(0,3),"1");
	checkSuccess(cur->getField(0,4),"testchar1                               ");
	checkSuccess(cur->getField(0,5),"testvarchar1");
	checkSuccess(cur->getField(0,6),"01-01-2001");
	checkSuccess(cur->getField(0,7),"01:00:00");
	printf("\n");
	checkSuccess(cur->getField(7,0),"8");
	checkSuccess(cur->getField(7,1),"8.8");
	checkSuccess(cur->getField(7,2),"8.8");
	checkSuccess(cur->getField(7,3),"8");
	checkSuccess(cur->getField(7,4),"testchar8                               ");
	checkSuccess(cur->getField(7,5),"testvarchar8");
	checkSuccess(cur->getField(7,6),"01-01-2008");
	checkSuccess(cur->getField(7,7),"08:00:00");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,0),1);
	checkSuccess(cur->getFieldLength(0,1),3);
	checkSuccess(cur->getFieldLength(0,2),3);
	checkSuccess(cur->getFieldLength(0,3),1);
	checkSuccess(cur->getFieldLength(0,4),40);
	checkSuccess(cur->getFieldLength(0,5),12);
	checkSuccess(cur->getFieldLength(0,6),10);
	checkSuccess(cur->getFieldLength(0,7),8);
	printf("\n");
	checkSuccess(cur->getFieldLength(7,0),1);
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
	checkSuccess(cur->getField(0,"testdate"),"01-01-2001");
	checkSuccess(cur->getField(0,"testtime"),"01:00:00");
	printf("\n");
	checkSuccess(cur->getField(7,"testint"),"8");
	checkSuccess(cur->getField(7,"testfloat"),"8.8");
	checkSuccess(cur->getField(7,"testreal"),"8.8");
	checkSuccess(cur->getField(7,"testsmallint"),"8");
	checkSuccess(cur->getField(7,"testchar"),"testchar8                               ");
	checkSuccess(cur->getField(7,"testvarchar"),"testvarchar8");
	checkSuccess(cur->getField(7,"testdate"),"01-01-2008");
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
	checkSuccess(fields[6],"01-01-2001");
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

	printf("INDIVIDUAL SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),'$(var2)',$(var3)");
	cur->substitution("var1",1);
	cur->substitution("var2","hello");
	cur->substitution("var3",10.5556,6,4);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"10.5556");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3)");
	cur->substitutions(subvars,subvallongs);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");
	
	printf("FIELDS: \n");
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(0,1),"2");
	checkSuccess(cur->getField(0,2),"3");
	printf("\n");
	
	printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select '$(var1)','$(var2)','$(var3)'");
	cur->substitutions(subvars,subvalstrings);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccess(cur->getField(0,0),"hi");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"bye");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3)");
	cur->substitutions(subvars,subvaldoubles,precs,scales);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccess(cur->getField(0,0),"10.55");
	checkSuccess(cur->getField(0,1),"10.556");
	checkSuccess(cur->getField(0,2),"10.5556");
	printf("\n");

	printf("NULLS as Nulls: \n");
	cur->getNullsAsNulls();
	checkSuccess(cur->sendQuery("select NULL,1,NULL"),1);
	checkSuccess(cur->getField(0,0),NULL);
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),NULL);
	cur->getNullsAsEmptyStrings();
	checkSuccess(cur->sendQuery("select NULL,1,NULL"),1);
	checkSuccess(cur->getField(0,0),"");
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),"");
	cur->getNullsAsNulls();
	printf("\n");

	printf("RESULT SET BUFFER SIZE: \n");
	checkSuccess(cur->getResultSetBufferSize(),0);
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getResultSetBufferSize(),2);
	printf("\n");
	checkSuccess(cur->firstRowIndex(),0);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),2);
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(1,0),"2");
	checkSuccess(cur->getField(2,0),"3");
	printf("\n");
	checkSuccess(cur->firstRowIndex(),2);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),4);
	checkSuccess(cur->getField(6,0),"7");
	checkSuccess(cur->getField(7,0),"8");
	printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,0),NULL);
	printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	printf("\n");

	printf("DONT GET COLUMN INFO: \n");
	cur->dontGetColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getColumnName(0),NULL);
	checkSuccess(cur->getColumnLength(0),0);
	checkSuccess(cur->getColumnType(0),NULL);
	cur->getColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getColumnName(0),"testint");
	checkSuccess(cur->getColumnLength(0),4);
	checkSuccess(cur->getColumnType(0),"int4");
	printf("\n");

	printf("SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=strdup(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	printf("\n");
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(1,0),"2");
	checkSuccess(cur->getField(2,0),"3");
	checkSuccess(cur->getField(3,0),"4");
	checkSuccess(cur->getField(4,0),"5");
	checkSuccess(cur->getField(5,0),"6");
	checkSuccess(cur->getField(6,0),"7");
	checkSuccess(cur->getField(7,0),"8");
	printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=strdup(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	printf("\n");
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(1,0),"2");
	checkSuccess(cur->getField(2,0),"3");
	checkSuccess(cur->getField(3,0),"4");
	checkSuccess(cur->getField(4,0),"5");
	checkSuccess(cur->getField(5,0),"6");
	checkSuccess(cur->getField(6,0),"7");
	checkSuccess(cur->getField(7,0),"8");
	printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=strdup(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	printf("\n");
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(1,0),"2");
	checkSuccess(cur->getField(2,0),"3");
	checkSuccess(cur->getField(3,0),"4");
	checkSuccess(cur->getField(4,0),"5");
	checkSuccess(cur->getField(5,0),"6");
	checkSuccess(cur->getField(6,0),"7");
	checkSuccess(cur->getField(7,0),"8");
	printf("\n");

	printf("SUSPENDED RESULT SET: \n");
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getField(2,0),"3");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=strdup(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeResultSet(id),1);
	printf("\n");
	checkSuccess(cur->firstRowIndex(),4);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),6);
	checkSuccess(cur->getField(7,0),"8");
	printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,0),NULL);
	printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	cur->setResultSetBufferSize(0);
	printf("\n");

	printf("CACHED RESULT SET: \n");
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	filename=strdup(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,0),"8");
	delete[] filename;
	printf("\n");

	printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(cur->colCount(),9);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
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

	printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	filename=strdup(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,0),"8");
	checkSuccess(cur->getField(8,0),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	printf("\n");

	printf("FROM ONE CACHE FILE TO ANOTHER: \n");
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,0),"8");
	checkSuccess(cur->getField(8,0),NULL);
	printf("\n");

	printf("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,0),"8");
	checkSuccess(cur->getField(8,0),NULL);
	cur->setResultSetBufferSize(0);
	printf("\n");

	printf("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getField(2,0),"3");
	filename=strdup(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=strdup(con->getConnectionSocket());
	printf("\n");
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeCachedResultSet(id,filename),1);
	printf("\n");
	checkSuccess(cur->firstRowIndex(),4);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),6);
	checkSuccess(cur->getField(7,0),"8");
	printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,0),NULL);
	printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	cur->cacheOff();
	printf("\n");
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,0),"8");
	checkSuccess(cur->getField(8,0),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	printf("\n");

	printf("COMMIT AND ROLLBACK: \n");
	secondcon=new sqlrconnection(argv[1],
				atoi(argv[2]), 
				argv[3],argv[4],argv[5],0,1);
	secondcur=new sqlrcursor(secondcon);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,0),"0");
	checkSuccess(con->commit(),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,0),"8");
	//checkSuccess(con->autoCommitOn(),1);
	checkSuccess(cur->sendQuery("insert into testtable values (10,10.1,10.1,10,'testchar10','testvarchar10','01/01/2010','10:00:00',NULL)"),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,0),"9");
	//checkSuccess(con->autoCommitOff(),1);
	printf("\n");

	printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getField(4,0),"5");
	checkSuccess(cur->getField(5,0),"6");
	checkSuccess(cur->getField(6,0),"7");
	checkSuccess(cur->getField(7,0),"8");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=strdup(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	checkSuccess(cur->resumeResultSet(id),1);
	checkSuccess(cur->getField(4,0),NULL);
	checkSuccess(cur->getField(5,0),NULL);
	checkSuccess(cur->getField(6,0),NULL);
	checkSuccess(cur->getField(7,0),NULL);
	printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	// temporary tables
	printf("TEMPORARY TABLES: \n");
	cur->sendQuery("drop table temptable\n");
	cur->sendQuery("create temporary table temptable (col1 int)");
	checkSuccess(cur->sendQuery("insert into temptable values (1)"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptable"),1);
	checkSuccess(cur->getField(0,0),"1");
	con->endSession();
	printf("\n");
	checkSuccess(cur->sendQuery("select count(*) from temptable"),0);
	cur->sendQuery("drop table temptable\n");
	printf("\n");

	printf("STORED PROCEDURES: \n");
	cur->sendQuery("drop function testfunc(int)");
	checkSuccess(cur->sendQuery("create function testfunc(int) returns int as ' begin return $1; end;' language plpgsql"),1);
	cur->prepareQuery("select * from testfunc(:int)");
	cur->inputBind("int",5);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,0),"5");
	cur->sendQuery("drop function testfunc(int)");

	cur->sendQuery("drop function testfunc(int,char(20))");
	checkSuccess(cur->sendQuery("create function testfunc(int, char(20)) returns record as ' declare output record; begin select $1,$2 into output; return output; end;' language plpgsql"),1);
	cur->prepareQuery("select * from testfunc(:int,:char) as (col1 int, col2 char(20))");
	cur->inputBind("int",5);
	cur->inputBind("char","hello");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,0),"5");
	checkSuccess(cur->getField(0,1),"hello");
	cur->sendQuery("drop function testfunc(int,char(20))");
	printf("\n");

	// invalid queries...
	printf("INVALID QUERIES: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),0);
	printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	printf("\n");
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	printf("\n");
}
