// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <sqlrelay/sqlrclient.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {

	if (argc<6) {
		printf("usage: sqlrtest host port socket user password query iterations\n");
		exit(0);
	}

	char	*host=argv[1];
	char	*port=argv[2];
	char	*socket=argv[3];
	char	*user=argv[4];
	char	*password=argv[5];
	char	*query=argv[6];
	int	iterations=atoi(argv[7]);

	time_t	starttime=time(NULL);
	printf("sqlrtest running, please wait...\n");
	clock();

	sqlrconnection	*sqlrcon=new sqlrconnection(host,atoi(port),
						socket,user,password,0,1);
	sqlrcursor	*sqlrcur=new sqlrcursor(sqlrcon);
	for (int count=0; count<iterations; count++) {
		sqlrcur->sendQuery(query);
		for (int i=0; i<sqlrcur->rowCount(); i++) {
			for (int j=0; j<sqlrcur->colCount(); j++) {
				printf("\"%s\"",sqlrcur->getField(i,j));
			}
			printf("\n");
		}
	}
	sqlrcon->endSession();
	delete sqlrcur;
	delete sqlrcon;

	printf("total system time used : %d\n",clock());
	printf("total real time : %d\n",time(NULL)-starttime);
}
