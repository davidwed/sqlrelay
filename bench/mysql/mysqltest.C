// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mysql.h>

MYSQL		mysql;
MYSQL_RES	*mysqlresult;
MYSQL_ROW	mysqlrow;

int main(int argc, char **argv) {

	if (argc<4) {
		printf("usage: mysqltest host user password query iterations\n");
		exit(0);
	}

	char	*host=argv[1];
	char	*user=argv[2];
	char	*password=argv[3];
	char	*query=argv[4];
	int	iterations=atoi(argv[5]);

	// init the timer
	time_t	starttime=time(NULL);
	clock();

	for (int count=0; count<iterations; count++) {

		// init
#ifdef MYSQL_VERSION
	#if MYSQL_VERSION_ID>=32200
		mysql_init(&mysql);
	
		// log in
		mysql_real_connect(&mysql,host,user,password,"",0,NULL,0);
	#else
		mysql_real_connect(&mysql,host,user,password,0,NULL,0);
	#endif
#else
		mysql_connect(&mysql,host,user,password);
#endif

		// execute the query
		mysql_real_query(&mysql,query,strlen(query));

		// get the result set
		mysqlresult=mysql_store_result(&mysql);

		// run through the rows
		int	cols=mysql_num_fields(mysqlresult);
		while(mysqlrow=mysql_fetch_row(mysqlresult)) {
			for (int i=0; i<cols; i++) {
				printf("\"%s\",",mysqlrow[i]);
			}
			printf("\n");
		}

			// free the result set
		mysql_free_result(mysqlresult);

		// log off
		mysql_close(&mysql);
	}

	printf("total system time used: %d\n",clock());
	printf("total real time: %d\n",time(NULL)-starttime);
}
