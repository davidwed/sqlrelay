// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <config.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

int	main(int argc, char **argv) {

	char	*dbtype;
	char	*bindvars[2]={"var1",NULL};
	char	*bindvals[1]={"testchar3"};
	char	*subvars[2]={"col1",NULL};
	char	*subvalstrings[1]={"testchar"};
	char	**cols;
	char	**fields;
	int	port;
	char	*socket;
	int	id;
	char	*filename;
	long	*fieldlens;
	int	counter=1;


	// usage...
	if (argc<5) {
		printf("usage: querytest host port socket user password\n");
		exit(0);
	}

	while (1) {

		// instantiation
		con=new sqlrconnection(argv[1],atoi(argv[2]), 
					argv[3],argv[4],argv[5],0,1);
con->debugOn();
		con->copyReferences();
		cur=new sqlrcursor(con);
		cur->copyReferences();



		// get database type
		con->identify();

		// ping
		con->ping();

		// drop existing table
		cur->sendQuery("drop table testtable");

		// insert
		cur->sendQuery("create table testtable (testchar char(40))");
		cur->sendQuery("insert into testtable values (NULL)");
		cur->sendQuery("insert into testtable values ('testchar1')");

		// affected rows
		cur->affectedRows();

		// binds
		cur->prepareQuery("insert into testtable values (:var1)");
		cur->inputBind("var1","testchar2");
		cur->executeQuery();
		cur->clearBinds();
		cur->inputBinds(bindvars,bindvals);
		cur->executeQuery();
		cur->clearBinds();
		cur->inputBind("var1","testchar4");
		cur->inputBind("var2","testchar4");
		cur->validateBinds();
		cur->executeQuery();

		// select
		cur->sendQuery("select * from testtable");

		// column info
		cur->colCount();
		cur->getColumnName(0);
		cols=cur->getColumnNames();
		cur->getColumnType(0);
		cur->getColumnType("testchar");
		cur->getColumnLength(0);
		cur->getColumnLength("testchar");
		cur->getLongest(0);
		cur->getLongest("testchar");

		// row info
		cur->rowCount();
		cur->totalRows();
		cur->firstRowIndex();
		cur->endOfResultSet();

		// field info
		cur->getField(0,0);
		cur->getFieldLength(0,0);
		cur->getField(0,"testchar");
		cur->getFieldLength(0,"testchar");
		fields=cur->getRow(0);
		fieldlens=cur->getRowLengths(0);

		// substitutions
		cur->prepareQuery("select $(col1) from testtable");
		cur->substitution("col1","testchar");
		cur->executeQuery();

		// array substitutions
		cur->prepareQuery("select $(col1) from testtable");
		cur->substitutions(subvars,subvalstrings);
		cur->executeQuery();
	
		// nulls as nulls/empty strings
		cur->getNullsAsNulls();
		cur->sendQuery("select * from testtable");
		cur->getNullsAsEmptyStrings();
		cur->sendQuery("select * from testtable");

		// result set buffer size
		cur->setResultSetBufferSize(2);
		cur->sendQuery("select * from testtable");
		cur->getResultSetBufferSize();
		cur->setResultSetBufferSize(0);

		// don't get column info
		cur->dontGetColumnInfo();
		cur->sendQuery("select * from testtable");
		cur->getColumnInfo();
		cur->sendQuery("select * from testtable");

		// suspend/resume
		cur->sendQuery("select * from testtable");
		con->suspendSession();
		port=con->getConnectionPort();
		socket=con->getConnectionSocket();
		con->resumeSession(port,socket);
		cur->getField(3,0);

		// suspend/resume with result set buffer size
		cur->setResultSetBufferSize(2);
		cur->sendQuery("select * from testtable");
		cur->getField(2,0);
		id=cur->getResultSetId();
		cur->suspendResultSet();
		con->suspendSession();
		port=con->getConnectionPort();
		socket=con->getConnectionSocket();
		con->resumeSession(port,socket);
		cur->resumeResultSet(id);
		cur->firstRowIndex();
		cur->endOfResultSet();
		cur->rowCount();
		cur->getField(3,0);
		cur->setResultSetBufferSize(0);

		// cache to file
		cur->cacheToFile("cachefile1");
		cur->setCacheTtl(200);
		cur->sendQuery("select * from testtable");
		filename=cur->getCacheFileName();
		if (strcmp(filename,"cachefile1")) {
			printf("%s\n",filename);
			exit(0);
		}
		cur->cacheOff();
		cur->openCachedResultSet(filename);
		cur->getField(3,0);
		cur->colCount();
		cur->getColumnName(0);
		cols=cur->getColumnNames();

		// cache to file with result set buffer size
		cur->setResultSetBufferSize(2);
		cur->cacheToFile("cachefile1");
		cur->setCacheTtl(200);
		cur->sendQuery("select * from testtable");
		filename=cur->getCacheFileName();
		if (strcmp(filename,"cachefile1")) {
			printf("%s\n",filename);
			exit(0);
		}
		cur->cacheOff();
		cur->openCachedResultSet(filename);
		cur->getField(3,0);
		cur->setResultSetBufferSize(0);

		// from 1 cache file to another
		cur->cacheToFile("cachefile2");
		cur->openCachedResultSet("cachefile1");
		cur->cacheOff();
		cur->openCachedResultSet("cachefile2");
		cur->getField(3,0);

		// from 1 cache file to another with result set buffer size
		cur->setResultSetBufferSize(2);
		cur->cacheToFile("cachefile2");
		cur->openCachedResultSet("cachefile1");
		cur->cacheOff();
		cur->openCachedResultSet("cachefile2");
		cur->getField(3,0);
		cur->setResultSetBufferSize(0);

		// suspend/resume with cache file
		cur->setResultSetBufferSize(2);
		cur->cacheToFile("cachefile1");
		cur->setCacheTtl(200);
		cur->sendQuery("select * from testtable");
		cur->getField(2,0);
		filename=cur->getCacheFileName();
		if (strcmp(filename,"cachefile1")) {
			printf("%s\n",filename);
			exit(0);
		}
		id=cur->getResultSetId();
		cur->suspendResultSet();
		con->suspendSession();
		port=con->getConnectionPort();
		socket=con->getConnectionSocket();
		con->resumeSession(port,socket);
		cur->resumeCachedResultSet(id,filename);
		cur->firstRowIndex();
		cur->endOfResultSet();
		cur->rowCount();
		cur->getField(3,0);
		cur->setResultSetBufferSize(0);

		// commit/rollback
		secondcon=new sqlrconnection(argv[1],
				atoi(argv[2]), 
				argv[3],argv[4],argv[5],0,1);
		secondcur=new sqlrcursor(secondcon);
		secondcur->sendQuery("select count(*) from testtable");
		secondcur->getField(0,0);
		con->commit();
		secondcur->sendQuery("select count(*) from testtable");
		secondcur->getField(0,0);
		con->autoCommitOn();
		cur->sendQuery("insert into testtable values ('testchar10')");
		secondcur->sendQuery("select count(*) from testtable");
		secondcur->getField(0,0);
		con->autoCommitOff();

		delete secondcur;
		delete secondcon;

		// drop existing table
		cur->sendQuery("drop table testtable");

		// invalid queries...
		cur->sendQuery("select * from testtable");
		cur->sendQuery("insert into testtable values (1,2,3,4)");
		cur->sendQuery("create table testtable");

		printf("loop %i\n",counter);
		counter++;

		delete cur;
		delete con;
	}
}
