// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

extern "C" {
	#include <sqlite.h>
}


sqlite		*sqliteptr;
char		*errmesg;

int	handleRow(void *parg, int argc, char **argv, char **columnnames) {

	for (int i=0; i<argc; i++) {
		printf("\"%s\",",argv[i]);
	}
	printf("\n");
	return 0;
}

int main(int argc, char **argv) {

	if (argc<4) {
		printf("usage: sqlitetest db query iterations\n");
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
		sqliteptr=sqlite_open(db,666,&errmesg);

		// execute the query
		sqlite_exec(sqliteptr,query,handleRow,NULL,&errmesg);

		// log off
		sqlite_close(sqliteptr);
	}

	printf("total system time used: %d\n",clock());
	printf("total real time: %d\n",time(NULL)-starttime);
}
