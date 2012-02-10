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

	if (argc<10) {
		printf("usage: mysqltest host port socket db user password query iterations queriesperiteration\n");
		exit(0);
	}

	char	*host=argv[1];
	int	port=atoi(argv[2]);
	char	*socket=(argv[3][0])?argv[3]:NULL;
	char	*db=argv[4];
	char	*user=argv[5];
	char	*password=argv[6];
	char	*query=argv[7];
	int	iterations=atoi(argv[8]);
	int	queriesperiteration=atoi(argv[9]);

	// init the timer
	time_t	starttime=time(NULL);
	clock();

	for (int count=0; count<iterations; count++) {

		// init
#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	#if MYSQL_VERSION_ID>=32200
		mysql_init(&mysql);
	
		// log in
		mysql_real_connect(&mysql,host,user,password,db,port,socket,0);
	#else
		mysql_real_connect(&mysql,host,user,password,port,socket,0);
	#endif
#else
		mysql_connect(&mysql,host,user,password);
#endif
#ifdef HAVE_MYSQL_SELECT_DB
		mysql_select_db(&mysql,db);
#endif

		for (int qcount=0; qcount<queriesperiteration; qcount++) {

			// execute the query
			mysql_real_query(&mysql,query,strlen(query));

			// get the result set
			mysqlresult=mysql_store_result(&mysql);

			// run through the rows
			if (mysqlresult) {
				int	cols=mysql_num_fields(mysqlresult);
				while(mysqlrow=mysql_fetch_row(mysqlresult)) {
					for (int i=0; i<cols; i++) {
						//printf("\"%s\",",mysqlrow[i]);
					}
					//printf("\n");
				}

				// free the result set
				mysql_free_result(mysqlresult);
			}
		}

		// log off
		mysql_close(&mysql);
	}

	printf("total system time used: %ld\n",clock());
	printf("total real time: %ld\n",time(NULL)-starttime);
}
