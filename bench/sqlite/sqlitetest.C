// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

extern "C" {
	#include <sqlite.h>
}

#ifndef SQLITE3
	#define	sqlite3_open		sqlite_open
	#define	sqlite3_close		sqlite_close
	#define	sqlite3_get_table	sqlite_get_table
	#define	sqlite3_errmsg		sqlite_errmsg
	#define	sqlite3_free_table	sqlite_free_table
	#define	sqlite3_exec		sqlite_exec
	sqlite		*sqliteptr;
#else
	sqlite3		*sqliteptr;
#endif

char		*errmesg;

int	handleRow(void *parg, int argc, char **argv, char **columnnames) {

	for (int i=0; i<argc; i++) {
		//printf("\"%s\",",argv[i]);
	}
	//printf("\n");
	return 0;
}

int main(int argc, char **argv) {

	if (argc<5) {
		printf("usage: sqlitetest db query iterations queriesperiteration\n");
		exit(0);
	}

	char	*db=argv[1];
	char	*query=argv[2];
	int	iterations=atoi(argv[3]);
	int	queriesperiteration=atoi(argv[4]);

	// init the timer
	time_t	starttime=time(NULL);
	clock();

	for (int count=0; count<iterations; count++) {

		// log in
		#ifdef SQLITE3
		sqlite3_open(db,&sqliteptr);
		#else
		sqliteptr=sqlite3_open(db,666,&errmesg);
		#endif

		for (int qcount=0; qcount<queriesperiteration; qcount++) {

			// execute the query
			sqlite3_exec(sqliteptr,query,handleRow,NULL,&errmesg);

		}

		// log off
		sqlite3_close(sqliteptr);
	}

	printf("total system time used: %ld\n",clock());
	printf("total real time: %ld\n",time(NULL)-starttime);
}
