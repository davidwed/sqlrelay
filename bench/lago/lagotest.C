// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <lago.h>

LCTX		lagocontext;
LRST		lagoresult;

int main(int argc, char **argv) {

	if (argc<4) {
		printf("usage: lagotest db user password query iterations\n");
		exit(0);
	}

	char	*db=argv[1];
	char	*user=argv[2];
	char	*password=argv[3];
	char	*query=argv[4];
	int	iterations=atoi(argv[5]);

	// init the timer
	time_t	starttime=time(NULL);
	clock();

	for (int count=0; count<iterations; count++) {

		// init
		lagocontext=Lnewctx();

		// log in
		Lconnect(lagocontext,"localhost","7412",db,user,password);

		// execute the query
		lagoresult=Lquery(lagocontext,query);

		// run through the rows
		int	cols=Lgetncols(lagoresult);
		while(Lfetch(lagoresult)!=LFETCH_END) {
			for (int i=1; i<cols+1; i++) {
				printf("\"");
				printf("%d",Lgetasstr(lagoresult,i));
				printf("\",");
			}
			printf("\n");
		}

		// log off
		Ldisconnect(lagocontext);
		Ldelctx(lagocontext);
	}

	printf("total system time used: %d\n",clock());
	printf("total real time: %d\n",time(NULL)-starttime);
}
