// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <sqlrelay/sqlrclient.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {

	if (argc<7) {
		printf("usage: sqlrtest host port socket user password query iterations queriesperiteration\n");
		exit(0);
	}

	char	*host=argv[1];
	char	*port=argv[2];
	char	*socket=argv[3];
	char	*user=argv[4];
	char	*password=argv[5];
	char	*query=argv[6];
	int	iterations=atoi(argv[7]);
	int	queriesperiteration=atoi(argv[8]);

	time_t	starttime=time(NULL);
	printf("sqlrtest running, please wait...\n");
	clock();

	for (int count=0; count<iterations; count++) {
		sqlrconnection	sqlrcon(host,atoi(port),
						socket,user,password,0,1);
		sqlrcursor	sqlrcur(&sqlrcon);
		for (int qcount=0; qcount<queriesperiteration; qcount++) {
			sqlrcur.sendQuery(query);
			for (int i=0; i<sqlrcur.rowCount(); i++) {
				for (int j=0; j<sqlrcur.colCount(); j++) {
					//printf("%s,",sqlrcur.getField(i,j));
					sqlrcur.getField(i,j);
				}
				//printf("\n");
			}
		}
		sqlrcon.endSession();
	}

	printf("total system time used : %ld\n",clock());
	printf("total real time : %ld\n",time(NULL)-starttime);
}
