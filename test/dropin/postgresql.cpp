#include <libpq-fe.h>
#include <rudiments/charstring.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

void checkSuccess(const char *value, const char *success) {
	//printf("\"%s\"=\"%s\"\n",value,success);

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("failure ");
			process::exit(0);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("failure ");
		process::exit(0);
	}
}

void checkSuccess(int value, int success) {
	//printf("\"%d\"=\"%d\"\n",value,success);

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("failure ");
		process::exit(0);
	}
}

int	main(int argc, char **argv) {

	const char	*host="localhost";
	const char	*port="9000";
	const char	*user="test";
	const char	*password="test";
	const char	*db="testdb";

	stdoutput.printf("PQresStatus:\n");
	checkSuccess(PQresStatus(PGRES_EMPTY_QUERY),"PGRES_EMPTY_QUERY");
	checkSuccess(PQresStatus(PGRES_COMMAND_OK),"PGRES_COMMAND_OK");
	checkSuccess(PQresStatus(PGRES_TUPLES_OK),"PGRES_TUPLES_OK");
	checkSuccess(PQresStatus(PGRES_COPY_OUT),"PGRES_COPY_OUT");
	checkSuccess(PQresStatus(PGRES_COPY_IN),"PGRES_COPY_IN");
	checkSuccess(PQresStatus(PGRES_BAD_RESPONSE),"PGRES_BAD_RESPONSE");
	checkSuccess(PQresStatus(PGRES_NONFATAL_ERROR),"PGRES_NONFATAL_ERROR");
	checkSuccess(PQresStatus(PGRES_FATAL_ERROR),"PGRES_FATAL_ERROR");

	stdoutput.printf("PQstatus:\n");
	PGconn	*pgconn=PQsetdbLogin(host,port,NULL,NULL,db,user,password);
	checkSuccess(PQstatus(pgconn),CONNECTION_OK);
	stdoutput.printf("\n");

	stdoutput.printf("PQdb:\n");
	checkSuccess(PQdb(pgconn),db);
	stdoutput.printf("\n");

	stdoutput.printf("PQuser:\n");
	checkSuccess(PQuser(pgconn),user);
	stdoutput.printf("\n");

	stdoutput.printf("PQpass:\n");
	checkSuccess(PQpass(pgconn),password);
	stdoutput.printf("\n");

	stdoutput.printf("PQhost:\n");
	checkSuccess(PQhost(pgconn),host);
	stdoutput.printf("\n");

	stdoutput.printf("PQport:\n");
	checkSuccess(PQport(pgconn),
			(charstring::length(port)?port:(char *)"5432"));
	stdoutput.printf("\n");

	stdoutput.printf("PQtty:\n");
	checkSuccess(PQtty(pgconn),"");
	stdoutput.printf("\n");

	stdoutput.printf("PQoptions:\n");
	checkSuccess(PQoptions(pgconn),"");
	stdoutput.printf("\n");

	stdoutput.printf("PQstatus:\n");
	PQfinish(pgconn);
	char	conninfo[1024];
	charstring::printf(conninfo,sizeof(conninfo),
		"host='%s' port='%s' user='%s' password='%s' dbname='%s'",
						host,port,user,password,db);
	pgconn=PQconnectdb(conninfo);
	checkSuccess(PQstatus(pgconn),CONNECTION_OK);
	stdoutput.printf("\n");

	stdoutput.printf("PQdb:\n");
	checkSuccess(PQdb(pgconn),db);
	stdoutput.printf("\n");

	stdoutput.printf("PQuser:\n");
	checkSuccess(PQuser(pgconn),user);
	stdoutput.printf("\n");

	stdoutput.printf("PQpass:\n");
	checkSuccess(PQpass(pgconn),password);
	stdoutput.printf("\n");

	stdoutput.printf("PQhost:\n");
	checkSuccess(PQhost(pgconn),host);
	stdoutput.printf("\n");

	stdoutput.printf("PQport:\n");
	checkSuccess(PQport(pgconn),port);
	stdoutput.printf("\n");

	stdoutput.printf("PQtty:\n");
	checkSuccess(PQtty(pgconn),"");
	stdoutput.printf("\n");

	stdoutput.printf("PQoptions:\n");
	checkSuccess(PQoptions(pgconn),"");
	stdoutput.printf("\n");

	stdoutput.printf("PQresetStart:\n");
	PQresetStart(pgconn);
	pgconn=PQconnectdb(conninfo);
	checkSuccess(PQstatus(pgconn),CONNECTION_OK);
	stdoutput.printf("\n");

	const char	*query="drop table testtable";
	PGresult	*pgresult=PQexec(pgconn,query);
	PQclear(pgresult);

	stdoutput.printf("PQexec: create\n");
	query="create table testtable (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)";
	pgresult=PQexec(pgconn,query);
	checkSuccess(PQresultStatus(pgresult),PGRES_COMMAND_OK);
	PQclear(pgresult);
	stdoutput.printf("\n");

	stdoutput.printf("PQexec: insert\n");
	query="insert into testtable values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)";
	pgresult=PQexec(pgconn,query);
	checkSuccess(PQresultStatus(pgresult),PGRES_COMMAND_OK);
	checkSuccess(PQcmdTuples(pgresult),"1");
	PQclear(pgresult);
	stdoutput.printf("\n");

	stdoutput.printf("PQprepare/PQexecPrepared: insert\n");
	//query="insert into testtable values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',NULL)";
	query="insert into testtable values ($1,$2,$3,$4,$5,$6,$7,$8,$9)";
	pgresult=PQprepare(pgconn,NULL,query,9,NULL);
	checkSuccess(PQresultStatus(pgresult),PGRES_COMMAND_OK);
	PQclear(pgresult);
	const char * const paramvalues[]={"2","2.2","2.2","2","testchar2","testvarchar2","01/01/2002","02:00:00",NULL};
	pgresult=PQexecPrepared(pgconn,NULL,9,paramvalues,NULL,NULL,0);
	checkSuccess(PQresultStatus(pgresult),PGRES_COMMAND_OK);
	checkSuccess(PQcmdTuples(pgresult),"1");
	PQclear(pgresult);
	stdoutput.printf("\n");

	stdoutput.printf("PQexec: select\n");
	query="select * from testtable";
	pgresult=PQexec(pgconn,query);
	checkSuccess(PQresultStatus(pgresult),PGRES_TUPLES_OK);
	stdoutput.printf("\n");

	stdoutput.printf("PQnfields:\n");
	checkSuccess(PQnfields(pgresult),9);
	stdoutput.printf("\n");

	stdoutput.printf("PQntuples:\n");
	checkSuccess(PQntuples(pgresult),2);
	stdoutput.printf("\n");
	
	stdoutput.printf("PQfname:\n");
	checkSuccess(PQfname(pgresult,0),"testint");
	checkSuccess(PQfname(pgresult,1),"testfloat");
	checkSuccess(PQfname(pgresult,2),"testreal");
	checkSuccess(PQfname(pgresult,3),"testsmallint");
	checkSuccess(PQfname(pgresult,4),"testchar");
	checkSuccess(PQfname(pgresult,5),"testvarchar");
	checkSuccess(PQfname(pgresult,6),"testdate");
	checkSuccess(PQfname(pgresult,7),"testtime");
	checkSuccess(PQfname(pgresult,8),"testtimestamp");
	stdoutput.printf("\n");
	
	stdoutput.printf("PQftype:\n");
	checkSuccess(PQftype(pgresult,0),23);
	checkSuccess(PQftype(pgresult,1),701);
	checkSuccess(PQftype(pgresult,2),700);
	checkSuccess(PQftype(pgresult,3),21);
	checkSuccess(PQftype(pgresult,4),1042);
	checkSuccess(PQftype(pgresult,5),1043);
	checkSuccess(PQftype(pgresult,6),1082);
	checkSuccess(PQftype(pgresult,7),1083);
	checkSuccess(PQftype(pgresult,8),1114);
	stdoutput.printf("\n");
	
	stdoutput.printf("PQfsize:\n");
	checkSuccess(PQfsize(pgresult,0),4);
	checkSuccess(PQfsize(pgresult,1),8);
	checkSuccess(PQfsize(pgresult,2),4);
	checkSuccess(PQfsize(pgresult,3),2);
	checkSuccess(PQfsize(pgresult,4),-1);
	checkSuccess(PQfsize(pgresult,5),-1);
	checkSuccess(PQfsize(pgresult,6),4);
	checkSuccess(PQfsize(pgresult,7),8);
	checkSuccess(PQfsize(pgresult,8),8);
	stdoutput.printf("\n");
	
	stdoutput.printf("PQfmod:\n");
	checkSuccess(PQfmod(pgresult,0),-1);
	checkSuccess(PQfmod(pgresult,1),-1);
	checkSuccess(PQfmod(pgresult,2),-1);
	checkSuccess(PQfmod(pgresult,3),-1);
	checkSuccess(PQfmod(pgresult,4),44);
	checkSuccess(PQfmod(pgresult,5),44);
	checkSuccess(PQfmod(pgresult,6),-1);
	checkSuccess(PQfmod(pgresult,7),-1);
	checkSuccess(PQfmod(pgresult,8),-1);
	stdoutput.printf("\n");
	
	stdoutput.printf("PQbinaryTuples:\n");
	checkSuccess(PQbinaryTuples(pgresult),0);
	stdoutput.printf("\n");

	stdoutput.printf("PQgetisnull:\n");
	checkSuccess(PQgetisnull(pgresult,0,0),0);
	checkSuccess(PQgetisnull(pgresult,0,1),0);
	checkSuccess(PQgetisnull(pgresult,0,2),0);
	checkSuccess(PQgetisnull(pgresult,0,3),0);
	checkSuccess(PQgetisnull(pgresult,0,4),0);
	checkSuccess(PQgetisnull(pgresult,0,5),0);
	checkSuccess(PQgetisnull(pgresult,0,6),0);
	checkSuccess(PQgetisnull(pgresult,0,7),0);
	checkSuccess(PQgetisnull(pgresult,1,0),0);
	checkSuccess(PQgetisnull(pgresult,1,1),0);
	checkSuccess(PQgetisnull(pgresult,1,2),0);
	checkSuccess(PQgetisnull(pgresult,1,3),0);
	checkSuccess(PQgetisnull(pgresult,1,4),0);
	checkSuccess(PQgetisnull(pgresult,1,5),0);
	checkSuccess(PQgetisnull(pgresult,1,6),0);
	checkSuccess(PQgetisnull(pgresult,1,7),0);
	stdoutput.printf("\n");

	stdoutput.printf("PQgetvalue:\n");
	checkSuccess(PQgetvalue(pgresult,0,0),"1");
	checkSuccess(PQgetvalue(pgresult,0,1),"1.1");
	checkSuccess(PQgetvalue(pgresult,0,2),"1.1");
	checkSuccess(PQgetvalue(pgresult,0,3),"1");
	checkSuccess(PQgetvalue(pgresult,0,4),"testchar1                               ");
	checkSuccess(PQgetvalue(pgresult,0,5),"testvarchar1");
	checkSuccess(PQgetvalue(pgresult,0,6),"2001-01-01");
	checkSuccess(PQgetvalue(pgresult,0,7),"01:00:00");
	checkSuccess(PQgetvalue(pgresult,1,0),"2");
	checkSuccess(PQgetvalue(pgresult,1,1),"2.2");
	checkSuccess(PQgetvalue(pgresult,1,2),"2.2");
	checkSuccess(PQgetvalue(pgresult,1,3),"2");
	checkSuccess(PQgetvalue(pgresult,1,4),"testchar2                               ");
	checkSuccess(PQgetvalue(pgresult,1,5),"testvarchar2");
	checkSuccess(PQgetvalue(pgresult,1,6),"2002-01-01");
	checkSuccess(PQgetvalue(pgresult,1,7),"02:00:00");
	stdoutput.printf("\n");

	stdoutput.printf("PQgetlength:\n");
	checkSuccess(PQgetlength(pgresult,0,0),1);
	checkSuccess(PQgetlength(pgresult,0,1),3);
	checkSuccess(PQgetlength(pgresult,0,2),3);
	checkSuccess(PQgetlength(pgresult,0,3),1);
	checkSuccess(PQgetlength(pgresult,0,4),40);
	checkSuccess(PQgetlength(pgresult,0,5),12);
	checkSuccess(PQgetlength(pgresult,0,6),10);
	checkSuccess(PQgetlength(pgresult,0,7),8);
	checkSuccess(PQgetlength(pgresult,1,0),1);
	checkSuccess(PQgetlength(pgresult,1,1),3);
	checkSuccess(PQgetlength(pgresult,1,2),3);
	checkSuccess(PQgetlength(pgresult,1,3),1);
	checkSuccess(PQgetlength(pgresult,1,4),40);
	checkSuccess(PQgetlength(pgresult,1,5),12);
	checkSuccess(PQgetlength(pgresult,1,6),10);
	checkSuccess(PQgetlength(pgresult,1,7),8);
	stdoutput.printf("\n");

	stdoutput.printf("PQescapeString:\n");
	char	to[1024];
	const char	*from=" \\ ' ";
	checkSuccess(PQescapeString(to,from,charstring::length(from)),7);
	checkSuccess(to," \\\\ '' ");
	stdoutput.printf("\n");

	//PQescapeBytea
	// PQunescapeBytea

	PQclear(pgresult);

	query="drop table testtable";
	pgresult=PQexec(pgconn,query);
	PQclear(pgresult);

	PQfinish(pgconn);
}
