// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <msql.h>

int		msql;
m_result	*msqlresult;
m_row		msqlrow;

int main(int argc, char **argv) {

	if (argc<6) {
		printf("usage: msqltest host db query iterations queriesperiteration\n");
		exit(0);
	}

	char	*host=(argv[1][0])?argv[1]:NULL;
	char	*db=argv[2];
	char	*query=argv[3];
	int	iterations=atoi(argv[4]);
	int	queriesperiteration=atoi(argv[5]);

	// init the timer
	time_t	starttime=time(NULL);
	clock();

	for (int count=0; count<iterations; count++) {

		// log in
		msql=msqlConnect((char *)host);
		msqlSelectDB(msql,db);

		for (int qcount=0; qcount<queriesperiteration; qcount++) {

			// execute the query
			msqlQuery(msql,query);

			// get the result set
			msqlresult=msqlStoreResult();

			// run through the rows
			int	cols=msqlNumFields(msqlresult);
			while(msqlrow=msqlFetchRow(msqlresult)) {
				for (int i=0; i<cols; i++) {
					//printf("\"%s\",",msqlrow[i]);
				}
				//printf("\n");
			}

			// free the result set
			msqlFreeResult(msqlresult);
		}

		// log off
		msqlClose(msql);
	}

	printf("total system time used: %d\n",clock());
	printf("total real time: %d\n",time(NULL)-starttime);
}
