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

	if (argc<3) {
		printf("usage: postgresqltest db query iterations\n");
		exit(0);
	}

	char	*db=argv[1];
	char	*query=argv[2];
	int	iterations=atoi(argv[3]);

	// init the timer
	time_t	starttime=time(NULL);
	clock();

	for (int count=0; count<iterations; count++) {

		// log in
		pgconn=PQsetdbLogin(NULL,NULL,NULL,NULL,db,NULL,NULL);

		// execute the query
		pgresult=PQexec(pgconn,query);

		// run through the rows
		int	cols=PQnfields(pgresult);
		int	rows=PQntuples(pgresult);
		for (int i=0; i<rows; i++) {
			for (int j=0; j<cols; j++) {
				printf("\"");
				printf("%s",PQgetvalue(pgresult,i,j));
				printf("\",");
			}
			printf("\n");
		}
		PQclear(pgresult);

		// log off
		PQfinish(pgconn);
	}

	printf("total system time used: %d\n",clock());
	printf("total real time: %d\n",time(NULL)-starttime);
}
