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

void checkSuccess(char *value, char *success, size_t length) {

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

int	main(int argc, char **argv) {

	char	*dbtype;
	const char	*bindvars[6]={"1","2","3","4","5",NULL};
	const char	*bindvals[5]={"4","testchar4","testvarchar4","01-JAN-2004","testlong4"};
	const char	*subvars[4]={"var1","var2","var3",NULL};
	const char	*subvalstrings[3]={"hi","hello","bye"};
	long	subvallongs[3]={1,2,3};
	double	subvaldoubles[3]={10.55,10.556,10.5556};
	unsigned short	precs[3]={4,5,6};
	unsigned short	scales[3]={2,3,4};
	char	*numvar;
	char	*clobvar;
	long	clobvarlength;
	char	*blobvar;
	long	blobvarlength;
	char	*stringvar;
	char	*floatvar;
	char	**cols;
	char	**fields;
	int	port;
	char	*socket;
	int	id;
	char	*filename;
	const char	*arraybindvars[6]={"var1","var2","var3","var4","var5",NULL};
	const char	*arraybindvals[5]={"7","testchar7","testvarchar7","01-JAN-2007","testlong7"};
	long	*fieldlens;


	// usage...
	if (argc<5) {
		printf("usage: oracle8i host port socket user password\n");
		exit(0);
	}


	// instantiation
	con=new sqlrconnection(argv[1],atoi(argv[2]), 
					argv[3],argv[4],argv[5],0,1);
	cur=new sqlrcursor(con);

	// get database type
	printf("IDENTIFY: \n");
	checkSuccess(con->identify(),"oracle8");
	printf("\n");

	// ping
	printf("PING: \n");
	checkSuccess(con->ping(),1);
	printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	printf("CREATE TEMPTABLE: \n");
	checkSuccess(cur->sendQuery("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long, testclob clob, testblob blob)"),1);
	printf("\n");

	printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1','testclob1',empty_blob())"),1);
	printf("\n");

	printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),1);
	printf("\n");

	printf("BIND BY POSITION: \n");
	cur->prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
	checkSuccess(cur->countBindVariables(),7);
	cur->inputBind("1",2);
	cur->inputBind("2","testchar2");
	cur->inputBind("3","testvarchar2");
	cur->inputBind("4","01-JAN-2002");
	cur->inputBind("5","testlong2");
	cur->inputBindClob("6","testclob2",9);
	cur->inputBindBlob("7","testblob2",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",3);
	cur->inputBind("2","testchar3");
	cur->inputBind("3","testvarchar3");
	cur->inputBind("4","01-JAN-2003");
	cur->inputBind("5","testlong3");
	cur->inputBindClob("6","testclob3",9);
	cur->inputBindBlob("7","testblob3",9);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("ARRAY OF BINDS BY POSITION: \n");
	cur->clearBinds();
	cur->inputBinds(bindvars,bindvals);
	cur->inputBindClob("6","testclob4",9);
	cur->inputBindBlob("7","testblob4",9);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("BIND BY NAME: \n");
	cur->prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
	cur->inputBind("var1",5);
	cur->inputBind("var2","testchar5");
	cur->inputBind("var3","testvarchar5");
	cur->inputBind("var4","01-JAN-2005");
	cur->inputBind("var5","testlong5");
	cur->inputBindClob("var6","testclob5",9);
	cur->inputBindBlob("var7","testblob5",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("var1",6);
	cur->inputBind("var2","testchar6");
	cur->inputBind("var3","testvarchar6");
	cur->inputBind("var4","01-JAN-2006");
	cur->inputBind("var5","testlong6");
	cur->inputBindClob("var6","testclob6",9);
	cur->inputBindBlob("var7","testblob6",9);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("ARRAY OF BINDS BY NAME: \n");
	cur->clearBinds();
	cur->inputBinds(arraybindvars,arraybindvals);
	cur->inputBindClob("var6","testclob7",9);
	cur->inputBindBlob("var7","testblob7",9);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("var1",8);
	cur->inputBind("var2","testchar8");
	cur->inputBind("var3","testvarchar8");
	cur->inputBind("var4","01-JAN-2008");
	cur->inputBind("var5","testlong8");
	cur->inputBindClob("var6","testclob8",9);
	cur->inputBindBlob("var7","testblob8",9);
	cur->inputBind("var9","junkvalue");
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("OUTPUT BIND BY NAME: \n");
	cur->prepareQuery("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;");
	cur->defineOutputBind("numvar",10);
	cur->defineOutputBind("stringvar",10);
	cur->defineOutputBind("floatvar",10);
	checkSuccess(cur->executeQuery(),1);
	numvar=cur->getOutputBind("numvar");
	stringvar=cur->getOutputBind("stringvar");
	floatvar=cur->getOutputBind("floatvar");
	checkSuccess(numvar,"1");
	checkSuccess(stringvar,"hello");
	checkSuccess(floatvar,"2.5");
	printf("\n");

	printf("OUTPUT BIND BY NAME: \n");
	cur->clearBinds();
	cur->defineOutputBind("1",10);
	cur->defineOutputBind("2",10);
	cur->defineOutputBind("3",10);
	checkSuccess(cur->executeQuery(),1);
	numvar=cur->getOutputBind("1");
	stringvar=cur->getOutputBind("2");
	floatvar=cur->getOutputBind("3");
	checkSuccess(numvar,"1");
	checkSuccess(stringvar,"hello");
	checkSuccess(floatvar,"2.5");
	printf("\n");

	printf("OUTPUT BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->defineOutputBind("numvar",10);
	cur->defineOutputBind("stringvar",10);
	cur->defineOutputBind("floatvar",10);
	cur->defineOutputBind("dummyvar",10);
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	numvar=cur->getOutputBind("numvar");
	stringvar=cur->getOutputBind("stringvar");
	floatvar=cur->getOutputBind("floatvar");
	checkSuccess(numvar,"1");
	checkSuccess(stringvar,"hello");
	checkSuccess(floatvar,"2.5");
	printf("\n");

	printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	printf("\n");

	printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),7);
	printf("\n");

	printf("COLUMN NAMES: \n");
	checkSuccess(cur->getColumnName(0),"TESTNUMBER");
	checkSuccess(cur->getColumnName(1),"TESTCHAR");
	checkSuccess(cur->getColumnName(2),"TESTVARCHAR");
	checkSuccess(cur->getColumnName(3),"TESTDATE");
	checkSuccess(cur->getColumnName(4),"TESTLONG");
	checkSuccess(cur->getColumnName(5),"TESTCLOB");
	checkSuccess(cur->getColumnName(6),"TESTBLOB");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"TESTNUMBER");
	checkSuccess(cols[1],"TESTCHAR");
	checkSuccess(cols[2],"TESTVARCHAR");
	checkSuccess(cols[3],"TESTDATE");
	checkSuccess(cols[4],"TESTLONG");
	checkSuccess(cols[5],"TESTCLOB");
	checkSuccess(cols[6],"TESTBLOB");
	printf("\n");

	printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType(0),"NUMBER");
	checkSuccess(cur->getColumnType("testnumber"),"NUMBER");
	checkSuccess(cur->getColumnType(1),"CHAR");
	checkSuccess(cur->getColumnType("testchar"),"CHAR");
	checkSuccess(cur->getColumnType(2),"VARCHAR2");
	checkSuccess(cur->getColumnType("testvarchar"),"VARCHAR2");
	checkSuccess(cur->getColumnType(3),"DATE");
	checkSuccess(cur->getColumnType("testdate"),"DATE");
	checkSuccess(cur->getColumnType(4),"LONG");
	checkSuccess(cur->getColumnType("testlong"),"LONG");
	checkSuccess(cur->getColumnType(5),"CLOB");
	checkSuccess(cur->getColumnType("testclob"),"CLOB");
	checkSuccess(cur->getColumnType(6),"BLOB");
	checkSuccess(cur->getColumnType("testblob"),"BLOB");
	printf("\n");

	printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength(0),22);
	checkSuccess(cur->getColumnLength("testnumber"),22);
	checkSuccess(cur->getColumnLength(1),40);
	checkSuccess(cur->getColumnLength("testchar"),40);
	checkSuccess(cur->getColumnLength(2),40);
	checkSuccess(cur->getColumnLength("testvarchar"),40);
	checkSuccess(cur->getColumnLength(3),7);
	checkSuccess(cur->getColumnLength("testdate"),7);
	checkSuccess(cur->getColumnLength(4),0);
	checkSuccess(cur->getColumnLength("testlong"),0);
	checkSuccess(cur->getColumnLength(5),0);
	checkSuccess(cur->getColumnLength("testclob"),0);
	checkSuccess(cur->getColumnLength(6),0);
	checkSuccess(cur->getColumnLength("testblob"),0);
	printf("\n");

	printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest(0),1);
	checkSuccess(cur->getLongest("testnumber"),1);
	checkSuccess(cur->getLongest(1),40);
	checkSuccess(cur->getLongest("testchar"),40);
	checkSuccess(cur->getLongest(2),12);
	checkSuccess(cur->getLongest("testvarchar"),12);
	checkSuccess(cur->getLongest(3),9);
	checkSuccess(cur->getLongest("testdate"),9);
	checkSuccess(cur->getLongest(4),9);
	checkSuccess(cur->getLongest("testlong"),9);
	checkSuccess(cur->getLongest(5),9);
	checkSuccess(cur->getLongest("testclob"),9);
	checkSuccess(cur->getLongest(6),9);
	checkSuccess(cur->getLongest("testblob"),9);
	printf("\n");

	printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	printf("\n");

	printf("TOTAL ROWS: \n");
	checkSuccess(cur->totalRows(),-1);
	printf("\n");

	printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	printf("\n");

	printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	printf("\n");

	printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(0,1),"testchar1                               ");
	checkSuccess(cur->getField(0,2),"testvarchar1");
	checkSuccess(cur->getField(0,3),"01-JAN-01");
	checkSuccess(cur->getField(0,4),"testlong1");
	checkSuccess(cur->getField(0,5),"testclob1");
	checkSuccess(cur->getField(0,6),"");
	printf("\n");
	checkSuccess(cur->getField(7,0),"8");
	checkSuccess(cur->getField(7,1),"testchar8                               ");
	checkSuccess(cur->getField(7,2),"testvarchar8");
	checkSuccess(cur->getField(7,3),"01-JAN-08");
	checkSuccess(cur->getField(7,4),"testlong8");
	checkSuccess(cur->getField(7,5),"testclob8");
	checkSuccess(cur->getField(7,6),"testblob8");
	printf("\n");

	printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,0),1);
	checkSuccess(cur->getFieldLength(0,1),40);
	checkSuccess(cur->getFieldLength(0,2),12);
	checkSuccess(cur->getFieldLength(0,3),9);
	checkSuccess(cur->getFieldLength(0,4),9);
	checkSuccess(cur->getFieldLength(0,5),9);
	checkSuccess(cur->getFieldLength(0,6),0);
	printf("\n");
	checkSuccess(cur->getFieldLength(7,0),1);
	checkSuccess(cur->getFieldLength(7,1),40);
	checkSuccess(cur->getFieldLength(7,2),12);
	checkSuccess(cur->getFieldLength(7,3),9);
	checkSuccess(cur->getFieldLength(7,4),9);
	checkSuccess(cur->getFieldLength(7,5),9);
	checkSuccess(cur->getFieldLength(7,6),9);
	printf("\n");

	printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"testnumber"),"1");
	checkSuccess(cur->getField(0,"testchar"),"testchar1                               ");
	checkSuccess(cur->getField(0,"testvarchar"),"testvarchar1");
	checkSuccess(cur->getField(0,"testdate"),"01-JAN-01");
	checkSuccess(cur->getField(0,"testlong"),"testlong1");
	checkSuccess(cur->getField(0,"testclob"),"testclob1");
	checkSuccess(cur->getField(0,"testblob"),"");
	printf("\n");
	checkSuccess(cur->getField(7,"testnumber"),"8");
	checkSuccess(cur->getField(7,"testchar"),"testchar8                               ");
	checkSuccess(cur->getField(7,"testvarchar"),"testvarchar8");
	checkSuccess(cur->getField(7,"testdate"),"01-JAN-08");
	checkSuccess(cur->getField(7,"testlong"),"testlong8");
	checkSuccess(cur->getField(7,"testclob"),"testclob8");
	checkSuccess(cur->getField(7,"testblob"),"testblob8");
	printf("\n");

	printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"testnumber"),1);
	checkSuccess(cur->getFieldLength(0,"testchar"),40);
	checkSuccess(cur->getFieldLength(0,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(0,"testdate"),9);
	checkSuccess(cur->getFieldLength(0,"testlong"),9);
	checkSuccess(cur->getFieldLength(0,"testclob"),9);
	checkSuccess(cur->getFieldLength(0,"testblob"),0);
	printf("\n");
	checkSuccess(cur->getFieldLength(7,"testnumber"),1);
	checkSuccess(cur->getFieldLength(7,"testchar"),40);
	checkSuccess(cur->getFieldLength(7,"testvarchar"),12);
	checkSuccess(cur->getFieldLength(7,"testdate"),9);
	checkSuccess(cur->getFieldLength(7,"testlong"),9);
	checkSuccess(cur->getFieldLength(7,"testclob"),9);
	checkSuccess(cur->getFieldLength(7,"testblob"),9);
	printf("\n");

	printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	checkSuccess(fields[1],"testchar1                               ");
	checkSuccess(fields[2],"testvarchar1");
	checkSuccess(fields[3],"01-JAN-01");
	checkSuccess(fields[4],"testlong1");
	checkSuccess(fields[5],"testclob1");
	checkSuccess(fields[6],"");
	printf("\n");

	printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	checkSuccess(fieldlens[1],40);
	checkSuccess(fieldlens[2],12);
	checkSuccess(fieldlens[3],9);
	checkSuccess(fieldlens[4],9);
	checkSuccess(fieldlens[5],9);
	checkSuccess(fieldlens[6],0);
	printf("\n");

	printf("INDIVIDUAL SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),'$(var2)',$(var3) from dual");
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

	printf("OUTPUT BIND: \n");
	cur->prepareQuery("begin :var1:='hello'; end;");
	cur->defineOutputBind("var1",10);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBind("var1"),"hello");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3) from dual");
	cur->substitutions(subvars,subvallongs);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");
	
	printf("FIELDS: \n");
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(0,1),"2");
	checkSuccess(cur->getField(0,2),"3");
	printf("\n");
	
	printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select '$(var1)','$(var2)','$(var3)' from dual");
	cur->substitutions(subvars,subvalstrings);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");

	printf("FIELDS: \n");
	checkSuccess(cur->getField(0,0),"hi");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"bye");
	printf("\n");

	printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3) from dual");
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
	checkSuccess(cur->sendQuery("select NULL,1,NULL from dual"),1);
	checkSuccess(cur->getField(0,0),NULL);
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),NULL);
	cur->getNullsAsEmptyStrings();
	checkSuccess(cur->sendQuery("select NULL,1,NULL from dual"),1);
	checkSuccess(cur->getField(0,0),"");
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),"");
	cur->getNullsAsNulls();
	printf("\n");

	printf("RESULT SET BUFFER SIZE: \n");
	checkSuccess(cur->getResultSetBufferSize(),0);
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getResultSetBufferSize(),2);
	printf("\n");
	checkSuccess(cur->firstRowIndex(),0);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),2);
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(cur->getField(1,0),"2");
	checkSuccess(cur->getField(2,0),"3");
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
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getColumnName(0),NULL);
	checkSuccess(cur->getColumnLength(0),0);
	checkSuccess(cur->getColumnType(0),NULL);
	cur->getColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getColumnName(0),"TESTNUMBER");
	checkSuccess(cur->getColumnLength(0),22);
	checkSuccess(cur->getColumnType(0),"NUMBER");
	printf("\n");

	printf("SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=con->getConnectionSocket();
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
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=con->getConnectionSocket();
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
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=con->getConnectionSocket();
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
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getField(2,0),"3");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=con->getConnectionSocket();
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
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	filename=strdup(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,0),"8");
	delete[] filename;
	printf("\n");

	printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(cur->colCount(),7);
	printf("\n");

	printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(cur->getColumnName(0),"TESTNUMBER");
	checkSuccess(cur->getColumnName(1),"TESTCHAR");
	checkSuccess(cur->getColumnName(2),"TESTVARCHAR");
	checkSuccess(cur->getColumnName(3),"TESTDATE");
	checkSuccess(cur->getColumnName(4),"TESTLONG");
	checkSuccess(cur->getColumnName(5),"TESTCLOB");
	checkSuccess(cur->getColumnName(6),"TESTBLOB");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"TESTNUMBER");
	checkSuccess(cols[1],"TESTCHAR");
	checkSuccess(cols[2],"TESTVARCHAR");
	checkSuccess(cols[3],"TESTDATE");
	checkSuccess(cols[4],"TESTLONG");
	checkSuccess(cols[5],"TESTCLOB");
	checkSuccess(cols[6],"TESTBLOB");
	printf("\n");

	printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
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
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getField(2,0),"3");
	filename=strdup(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=con->getConnectionSocket();
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
	checkSuccess(con->autoCommitOn(),1);
	checkSuccess(cur->sendQuery("insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10','testclob10',NULL)"),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,0),"9");
	checkSuccess(con->autoCommitOff(),1);
	printf("\n");

	printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
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

	printf("CLOB AND BLOB OUTPUT BIND: \n");
	cur->sendQuery("drop table testtable1");
	checkSuccess(cur->sendQuery("create table testtable1 (testclob clob, testblob blob)"),1);
	cur->prepareQuery("insert into testtable1 values ('hello',:var1)");
	cur->inputBindBlob("var1","hello",5);
	checkSuccess(cur->executeQuery(),1);
	cur->prepareQuery("begin select testclob into :clobvar from testtable1;  select testblob into :blobvar from testtable1; end;");
	cur->defineOutputBindClob("clobvar");
	cur->defineOutputBindBlob("blobvar");
	checkSuccess(cur->executeQuery(),1);
	clobvar=cur->getOutputBind("clobvar");
	clobvarlength=cur->getOutputBindLength("clobvar");
	blobvar=cur->getOutputBind("blobvar");
	blobvarlength=cur->getOutputBindLength("blobvar");
	checkSuccess(clobvar,"hello",5);
	checkSuccess(clobvarlength,5);
	checkSuccess(blobvar,"hello",5);
	checkSuccess(blobvarlength,5);
	cur->sendQuery("drop table testtable1");
	printf("\n");

	printf("NULL AND EMPTY CLOBS AND CLOBS: \n");
	cur->getNullsAsNulls();
	cur->sendQuery("create table testtable1 (testclob1 clob, testclob2 clob, testblob1 blob, testblob2 blob)");
	cur->prepareQuery("insert into testtable1 values (:var1,:var2,:var3,:var4)");
	cur->inputBindClob("var1","",0);
	cur->inputBindClob("var2",NULL,0);
	cur->inputBindBlob("var3","",0);
	cur->inputBindBlob("var4",NULL,0);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select * from testtable1");
	checkSuccess(cur->getField(0,0),NULL);
	checkSuccess(cur->getField(0,1),NULL);
	checkSuccess(cur->getField(0,2),NULL);
	checkSuccess(cur->getField(0,3),NULL);
	cur->sendQuery("drop table testtable1");
	printf("\n");

	printf("CURSOR BINDS: \n");
	checkSuccess(cur->sendQuery("create or replace package types as type cursorType is ref cursor; end;"),1);
	checkSuccess(cur->sendQuery("create or replace function sp_testtable return types.cursortype as l_cursor    types.cursorType; begin open l_cursor for select * from testtable; return l_cursor; end;"),1);
	cur->prepareQuery("begin  :curs:=sp_testtable; end;");
	cur->defineOutputBindCursor("curs");
	checkSuccess(cur->executeQuery(),1);
	sqlrcursor	*bindcur=cur->getOutputBindCursor("curs");
	checkSuccess(bindcur->fetchFromBindCursor(),1);
	checkSuccess(bindcur->getField(0,0),"1");
	checkSuccess(bindcur->getField(1,0),"2");
	checkSuccess(bindcur->getField(2,0),"3");
	checkSuccess(bindcur->getField(3,0),"4");
	checkSuccess(bindcur->getField(4,0),"5");
	checkSuccess(bindcur->getField(5,0),"6");
	checkSuccess(bindcur->getField(6,0),"7");
	checkSuccess(bindcur->getField(7,0),"8");
	delete bindcur;
	printf("\n");

	printf("LONG CLOB: \n");
	cur->sendQuery("drop table testtable2");
	cur->sendQuery("create table testtable2 (testclob clob)");
	cur->prepareQuery("insert into testtable2 values (:clobval)");
	char	clobval[8*1024+1];
	for (int i=0; i<8*1024; i++) {
		clobval[i]='C';
	}
	clobval[8*1024]=(char)NULL;
	cur->inputBindClob("clobval",clobval,8*1024);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testclob from testtable2");
	checkSuccess(clobval,cur->getField(0,"testclob"));
	cur->prepareQuery("begin select testclob into :clobbindval from testtable2; end;");
	cur->defineOutputBindClob("clobbindval");
	checkSuccess(cur->executeQuery(),1);
	char	*clobbindvar=cur->getOutputBind("clobbindval");
	checkSuccess(cur->getOutputBindLength("clobbindval"),8*1024);
	checkSuccess(clobval,clobbindvar);
	cur->sendQuery("drop table testtable2");
	printf("\n");


	printf("LONG OUTPUT BIND\n");
	cur->sendQuery("drop table testtable2");
	cur->sendQuery("create table testtable2 (testval varchar2(4000))");
	char	testval[4001];
	testval[4000]=(char)NULL;
	cur->prepareQuery("insert into testtable2 values (:testval)");
	for (int i=0; i<4000; i++) {
		testval[i]='C';
	}
	cur->inputBind("testval",testval);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testval from testtable2");
	checkSuccess(testval,cur->getField(0,"testval"));
	char	query[4000+25];
	sprintf(query,"begin :bindval:='%s'; end;",testval);
	cur->prepareQuery(query);
	cur->defineOutputBind("bindval",4000);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindLength("bindval"),4000);
	checkSuccess(cur->getOutputBind("bindval"),testval);
	cur->sendQuery("drop table testtable2");
	printf("\n");

	printf("NEGATIVE INPUT BIND\n");
	cur->sendQuery("create table testtable2 (testval number)");
	cur->prepareQuery("insert into testtable2 values (:testval)");
	cur->inputBind("testval",-1);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testval from testtable2");
	checkSuccess(cur->getField(0,"testval"),"-1");
	cur->sendQuery("drop table testtable2");
	printf("\n");


	// drop existing table
	cur->sendQuery("drop table testtable");


	// temporary tables
	printf("TEMPORARY TABLES: \n");
	cur->sendQuery("drop table temptabledelete\n");
	cur->sendQuery("create global temporary table temptabledelete (col1 number) on commit delete rows");
	checkSuccess(cur->sendQuery("insert into temptabledelete values (1)"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptabledelete"),1);
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(con->commit(),1);
	checkSuccess(cur->sendQuery("select count(*) from temptabledelete"),1);
	checkSuccess(cur->getField(0,0),"0");
	cur->sendQuery("drop table temptabledelete\n");
	printf("\n");
	cur->sendQuery("drop table temptablepreserve\n");
	cur->sendQuery("create global temporary table temptablepreserve (col1 number) on commit preserve rows");
	checkSuccess(cur->sendQuery("insert into temptablepreserve values (1)"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),1);
	checkSuccess(cur->getField(0,0),"1");
	checkSuccess(con->commit(),1);
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),1);
	checkSuccess(cur->getField(0,0),"1");
	con->endSession();
	printf("\n");
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),1);
	checkSuccess(cur->getField(0,0),"0");
	cur->sendQuery("drop table temptablepreserve\n");
	printf("\n");


	// invalid queries...
	printf("INVALID QUERIES: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),0);
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


	delete cur;
	delete con;

}
