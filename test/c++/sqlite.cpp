// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include "../../config.h"
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
			stdoutput.printf("failure %s!=%s\n",value,success);
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("failure %s!=%s\n",value,success);
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("failure %d!=%d\n",value,success);
		delete cur;
		delete con;
		process::exit(1);
	}
}

int	main(int argc, char **argv) {

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
	checkSuccess(con->identify(),"sqlite");
	stdoutput.printf("\n");

	// ping
	stdoutput.printf("PING: \n");
	checkSuccess(con->ping(),1);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("begin transaction");
	cur->sendQuery("drop table testtable");
	con->commit();

	// create a new table
	stdoutput.printf("CREATE TEMPTABLE: \n");
	cur->sendQuery("begin transaction");
	checkSuccess(cur->sendQuery("create table testtable (testint int, testfloat float, testchar char(40), testvarchar varchar(40), testclob clob, testblob blob)"),1);
	con->commit();
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	cur->sendQuery("begin transaction");
	checkSuccess(cur->sendQuery("insert into testtable values (1,1.1,'testchar1','testvarchar1','testclob1','testblob1')"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (2,2.2,'testchar2','testvarchar2','testclob2','testblob2')"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (3,3.3,'testchar3','testvarchar3','testclob3','testblob3')"),1);
	checkSuccess(cur->sendQuery("insert into testtable values (4,4.4,'testchar4','testvarchar4','testclob4','testblob4')"),1);
	stdoutput.printf("\n");

	stdoutput.printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),0);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY NAME: \n");
	cur->prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6)");
	checkSuccess(cur->countBindVariables(),6);
	cur->inputBind("var1",5);
	cur->inputBind("var2",5.5,4,1);
	cur->inputBind("var3","testchar5");
	cur->inputBind("var4","testvarchar5");
	cur->inputBindClob("var5","testclob5",9);
	cur->inputBindBlob("var6","testblob5",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("var1",6);
	cur->inputBind("var2",6.6,4,1);
	cur->inputBind("var3","testchar6");
	cur->inputBind("var4","testvarchar6");
	cur->inputBindClob("var5","testclob6",9);
	cur->inputBindBlob("var6","testblob6",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("var1",7);
	cur->inputBind("var2",7.7,4,1);
	cur->inputBind("var3","testchar7");
	cur->inputBind("var4","testvarchar7");
	cur->inputBindClob("var5","testclob7",9);
	cur->inputBindBlob("var6","testblob7",9);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("var1",8);
	cur->inputBind("var2",8.8,4,1);
	cur->inputBind("var3","testchar8");
	cur->inputBind("var4","testvarchar8");
	cur->inputBindClob("var5","testclob8",9);
	cur->inputBindBlob("var6","testblob8",9);
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),6);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES: \n");
	checkSuccess(cur->getColumnName(0),"testint");
	checkSuccess(cur->getColumnName(1),"testfloat");
	checkSuccess(cur->getColumnName(2),"testchar");
	checkSuccess(cur->getColumnName(3),"testvarchar");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testint");
	checkSuccess(cols[1],"testfloat");
	checkSuccess(cols[2],"testchar");
	checkSuccess(cols[3],"testvarchar");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN TYPES: \n");
	#ifdef HAVE_SQLITE3_STMT
	checkSuccess(cur->getColumnType((uint32_t)0),"INTEGER");
	checkSuccess(cur->getColumnType("testint"),"INTEGER");
	checkSuccess(cur->getColumnType(1),"FLOAT");
	checkSuccess(cur->getColumnType("testfloat"),"FLOAT");
	checkSuccess(cur->getColumnType(2),"STRING");
	checkSuccess(cur->getColumnType("testchar"),"STRING");
	checkSuccess(cur->getColumnType(3),"STRING");
	checkSuccess(cur->getColumnType("testvarchar"),"STRING");
	checkSuccess(cur->getColumnType(4),"STRING");
	checkSuccess(cur->getColumnType("testclob"),"STRING");
	checkSuccess(cur->getColumnType(5),"STRING");
	checkSuccess(cur->getColumnType("testblob"),"STRING");
	#else
	checkSuccess(cur->getColumnType((uint32_t)0),"UNKNOWN");
	checkSuccess(cur->getColumnType("testint"),"UNKNOWN");
	checkSuccess(cur->getColumnType(1),"UNKNOWN");
	checkSuccess(cur->getColumnType("testfloat"),"UNKNOWN");
	checkSuccess(cur->getColumnType(2),"UNKNOWN");
	checkSuccess(cur->getColumnType("testchar"),"UNKNOWN");
	checkSuccess(cur->getColumnType(3),"UNKNOWN");
	checkSuccess(cur->getColumnType("testvarchar"),"UNKNOWN");
	checkSuccess(cur->getColumnType(4),"UNKNOWN");
	checkSuccess(cur->getColumnType("testclob"),"UNKNOWN");
	checkSuccess(cur->getColumnType(5),"UNKNOWN");
	checkSuccess(cur->getColumnType("testblob"),"UNKNOWN");
	#endif
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),0);
	checkSuccess(cur->getColumnLength("testint"),0);
	checkSuccess(cur->getColumnLength(1),0);
	checkSuccess(cur->getColumnLength("testfloat"),0);
	checkSuccess(cur->getColumnLength(2),0);
	checkSuccess(cur->getColumnLength("testchar"),0);
	checkSuccess(cur->getColumnLength(3),0);
	checkSuccess(cur->getColumnLength("testvarchar"),0);
	checkSuccess(cur->getColumnLength(4),0);
	checkSuccess(cur->getColumnLength("testclob"),0);
	checkSuccess(cur->getColumnLength(5),0);
	checkSuccess(cur->getColumnLength("testblob"),0);
	stdoutput.printf("\n");

	stdoutput.printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest("testint"),1);
	checkSuccess(cur->getLongest(1),3);
	checkSuccess(cur->getLongest("testfloat"),3);
	checkSuccess(cur->getLongest(2),9);
	checkSuccess(cur->getLongest("testchar"),9);
	checkSuccess(cur->getLongest(3),12);
	checkSuccess(cur->getLongest("testvarchar"),12);
	checkSuccess(cur->getLongest(4),9);
	checkSuccess(cur->getLongest("testclob"),9);
	checkSuccess(cur->getLongest(5),9);
	checkSuccess(cur->getLongest("testblob"),9);
	stdoutput.printf("\n");

	stdoutput.printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("TOTAL ROWS: \n");
	#ifdef HAVE_SQLITE3_STMT
	checkSuccess(cur->totalRows(),0);
	#else
	checkSuccess(cur->totalRows(),8);
	#endif
	stdoutput.printf("\n");

	stdoutput.printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	stdoutput.printf("\n");

	stdoutput.printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"1.1");
	checkSuccess(cur->getField(0,2),"testchar1");
	checkSuccess(cur->getField(0,3),"testvarchar1");
	checkSuccess(cur->getField(0,4),"testclob1");
	checkSuccess(cur->getField(0,5),"testblob1");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(7,1),"8.8");
	checkSuccess(cur->getField(7,2),"testchar8");
	checkSuccess(cur->getField(7,3),"testvarchar8");
	checkSuccess(cur->getField(7,4),"testclob8");
	checkSuccess(cur->getField(7,5),"testblob8");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(0,1),3);
	checkSuccess(cur->getFieldLength(0,2),9);
	checkSuccess(cur->getFieldLength(0,3),12);
	checkSuccess(cur->getFieldLength(0,4),9);
	checkSuccess(cur->getFieldLength(0,5),9);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(7,1),3);
	checkSuccess(cur->getFieldLength(7,2),9);
	checkSuccess(cur->getFieldLength(7,3),12);
	checkSuccess(cur->getFieldLength(7,4),9);
	checkSuccess(cur->getFieldLength(7,5),9);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"testint"),"1");
	checkSuccess(cur->getField(0,"testfloat"),"1.1");
	checkSuccess(cur->getField(0,"testchar"),"testchar1");
	checkSuccess(cur->getField(0,"testvarchar"),"testvarchar1");
	checkSuccess(cur->getField(0,"testclob"),"testclob1");
	checkSuccess(cur->getField(0,"testblob"),"testblob1");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,"testint"),"8");
	checkSuccess(cur->getField(7,"testfloat"),"8.8");
	checkSuccess(cur->getField(7,"testchar"),"testchar8");
	checkSuccess(cur->getField(7,"testvarchar"),"testvarchar8");
	checkSuccess(cur->getField(7,"testclob"),"testclob8");
	checkSuccess(cur->getField(7,"testblob"),"testblob8");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"testint"),1);
	checkSuccess(cur->getFieldLength(0,"testfloat"),3);
	checkSuccess(cur->getFieldLength(0,"testchar"),9);
	checkSuccess(cur->getFieldLength(0,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(0,"testclob"),9);
	checkSuccess(cur->getFieldLength(0,"testblob"),9);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,"testint"),1);
	checkSuccess(cur->getFieldLength(7,"testfloat"),3);
	checkSuccess(cur->getFieldLength(7,"testchar"),9);
	checkSuccess(cur->getFieldLength(7,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(7,"testclob"),9);
	checkSuccess(cur->getFieldLength(7,"testblob"),9);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	checkSuccess(fields[1],"1.1");
	checkSuccess(fields[2],"testchar1");
	checkSuccess(fields[3],"testvarchar1");
	checkSuccess(fields[4],"testclob1");
	checkSuccess(fields[5],"testblob1");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	checkSuccess(fieldlens[1],3);
	checkSuccess(fieldlens[2],9);
	checkSuccess(fieldlens[3],12);
	checkSuccess(fieldlens[4],9);
	checkSuccess(fieldlens[5],9);
	stdoutput.printf("\n");

	stdoutput.printf("INDIVIDUAL SUBSTITUTIONS: \n");
	cur->sendQuery("drop table testtable1");
	checkSuccess(cur->sendQuery("create table testtable1 (col1 int, col2 char, col3 float)"),1);
	cur->prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))");
	cur->substitution("var1",1);
	cur->substitution("var2","hello");
	cur->substitution("var3",10.5556,6,4);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->sendQuery("select * from testtable1"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"10.5556");
	checkSuccess(cur->sendQuery("delete from testtable1"),1);
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("insert into testtable1 values ('$(var1)','$(var2)','$(var3)')");
	cur->substitutions(subvars,subvalstrings);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->sendQuery("select * from testtable1"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"hi");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"bye");
	checkSuccess(cur->sendQuery("delete from testtable1"),1);
	stdoutput.printf("\n");


	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))");
	cur->substitutions(subvars,subvallongs);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->sendQuery("select * from testtable1"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"2");
	checkSuccess(cur->getField(0,2),"3.0");
	checkSuccess(cur->sendQuery("delete from testtable1"),1);
	stdoutput.printf("\n");


	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))");
	cur->substitutions(subvars,subvaldoubles,precs,scales);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->sendQuery("select * from testtable1"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"10.55");
	checkSuccess(cur->getField(0,1),"10.556");
	checkSuccess(cur->getField(0,2),"10.5556");
	checkSuccess(cur->sendQuery("delete from testtable1"),1);
	stdoutput.printf("\n");


	stdoutput.printf("NULLS as Nulls: \n");
	cur->getNullsAsNulls();
	checkSuccess(cur->sendQuery("insert into testtable1 values (1,NULL,NULL)"),1);
	checkSuccess(cur->sendQuery("select * from testtable1"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),NULL);
	checkSuccess(cur->getField(0,2),NULL);
	cur->getNullsAsEmptyStrings();
	checkSuccess(cur->sendQuery("select * from testtable1"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"");
	checkSuccess(cur->getField(0,2),"");
	cur->getNullsAsNulls();
	stdoutput.printf("\n");

	stdoutput.printf("RESULT SET BUFFER SIZE: \n");
	checkSuccess(cur->getResultSetBufferSize(),0);
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getColumnName(0),NULL);
	checkSuccess(cur->getColumnLength((uint32_t)0),0);
	checkSuccess(cur->getColumnType((uint32_t)0),NULL);
	cur->getColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	checkSuccess(cur->getColumnName(0),"testint");
	checkSuccess(cur->getColumnLength((uint32_t)0),0);
	#ifdef HAVE_SQLITE3_STMT
	checkSuccess(cur->getColumnType((uint32_t)0),"INTEGER");
	#else
	checkSuccess(cur->getColumnType((uint32_t)0),"UNKNOWN");
	#endif
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(cur->colCount(),6);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(cur->getColumnName(0),"testint");
	checkSuccess(cur->getColumnName(1),"testfloat");
	checkSuccess(cur->getColumnName(2),"testchar");
	checkSuccess(cur->getColumnName(3),"testvarchar");
	checkSuccess(cur->getColumnName(4),"testclob");
	checkSuccess(cur->getColumnName(5),"testblob");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"testint");
	checkSuccess(cols[1],"testfloat");
	checkSuccess(cols[2],"testchar");
	checkSuccess(cols[3],"testvarchar");
	checkSuccess(cols[4],"testclob");
	checkSuccess(cols[5],"testblob");
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testint"),1);
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
	secondcon=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							"test","test",0,1);
	secondcur=new sqlrcursor(secondcon);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"0");
	checkSuccess(con->commit(),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"8");
	checkSuccess(cur->sendQuery("insert into testtable values (10,10.1,'testchar10','testvarchar10','testclob10','testblob10')"),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"9");
	stdoutput.printf("\n");

	stdoutput.printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable"),1);
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

	// drop existing table
	cur->sendQuery("drop table testtable");

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

	// last insert row id
	stdoutput.printf("LAST INSERT ROW ID: \n");
	checkSuccess(cur->sendQuery("select last insert rowid"),1);
	checkSuccess(cur->colCount(),1);
	checkSuccess(cur->rowCount(),1);
	checkSuccess(cur->getColumnName(0),"LASTINSERTROWID");
	checkSuccess(!charstring::length(cur->getField(0,(uint32_t)0)),0);
	stdoutput.printf("\n");

	// invalid queries...
	stdoutput.printf("INVALID QUERIES: \n");
	checkSuccess(cur->sendQuery("select * from testtable"),0);
	checkSuccess(cur->sendQuery("select * from testtable"),0);
	checkSuccess(cur->sendQuery("select * from testtable"),0);
	checkSuccess(cur->sendQuery("select * from testtable"),0);
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

	return 0;
}
