// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <libpq-fe.h>

PGconn		*pgconn;
PGresult	*pgresult;

int main(int argc, char **argv) {

	if (argc<9) {
		printf("usage: postgresqltest host port user password db query iterations queriesperiteration\n");
		exit(0);
	}

	char	*host=argv[1];
	char	*port=argv[2];
	char	*user=argv[3];
	char	*password=argv[4];
	char	*db=argv[5];
	char	*query=argv[6];
	int	iterations=atoi(argv[7]);
	int	queriesperiteration=atoi(argv[8]);

	// init the timer
	time_t	starttime=time(NULL);
	clock();

	for (int count=0; count<iterations; count++) {

		// log in
		pgconn=PQsetdbLogin(host,port,NULL,NULL,db,user,password);

		for (int qcount=0; qcount<queriesperiteration; qcount++) {

			// execute the query
			pgresult=PQexec(pgconn,query);

			// run through the rows
			int	cols=PQnfields(pgresult);
			int	rows=PQntuples(pgresult);
			for (int i=0; i<rows; i++) {
				for (int j=0; j<cols; j++) {
					//printf("%s,",PQgetvalue(pgresult,i,j));
					PQgetvalue(pgresult,i,j);
				}
				//printf("\n");
			}
			PQclear(pgresult);
		}

		// log off
		PQfinish(pgconn);
	}

	printf("total system time used: %ld\n",clock());
	printf("total real time: %ld\n",time(NULL)-starttime);
}
