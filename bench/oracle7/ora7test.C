// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <rudiments/environment.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	2048

extern "C" {

	#ifdef HAVE_OCI_H
		#include <oci.h>
	#else
		#include <oratypes.h>
		#include <ocidfn.h>
		#include <ociapr.h>
	#endif
	#include <ocidem.h>

	#define NULL_TERMINATED_STRING	5

	#define PARSE_DEFER		1
	#define PARSE_V7_LNG		2
}

struct describe {
	sb4	dbsize;
	sb2	dbtype;
	sb1	buf[MAX_ITEM_BUFFER_SIZE];
	sb4	buflen;
	sb4	dsize;
	sb2	precision;
	sb2	scale;
	sb2	nullok;
};
	
Lda_Def		lda;
Cda_Def		cda;
ub4		hda[256/sizeof(ub4)];
sword		ncols;

describe	desc[MAX_SELECT_LIST_SIZE];
ub1		def_buf[MAX_SELECT_LIST_SIZE]
			[FETCH_AT_ONCE][MAX_ITEM_BUFFER_SIZE];
sb2		def_indp[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];
ub2		def_col_retlen[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];
ub2		def_col_retcode[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];

int		row;

int main(int argc, char **argv) {

	if (argc<7) {
		printf("usage: ora7test user password sid query iterations queriesperiteration\n");
		exit(0);
	}

	char	*user=argv[1];
	char	*password=argv[2];
	char	*sid=argv[3];
	char	*query=argv[4];
	int	iterations=atoi(argv[5]);
	int	queriesperiteration=atoi(argv[6]);

	environment	env;
	env.setValue("ORACLE_SID",sid);
	env.setValue("TWO_TASK",sid);

	// init the timer
	time_t	starttime=time(NULL);
	printf("oratest running, please wait...\n");
	clock();

	for (int count=0; count<iterations; count++) {

		// log in
		olog(&lda,(ub1 *)hda,(text *)user,-1,(text *)password,-1,
			(text *)0,-1,OCI_LM_DEF);

		for (int qcount=0; qcount<queriesperiteration; qcount++) {

			// allocate a cursor
			oopen(&cda,&lda,(text *)0,-1,-1,(text *)0,-1);

			// parse the query
			oparse(&cda,(text *)query,
				(sb4)-1,(sword)PARSE_DEFER,(ub4)PARSE_V7_LNG);

			// describe/define the query
			ncols=0;
			if (cda.ft==FT_SELECT) {

				sword	col=0;

				// run through the columns...
				for (;;) {

					desc[col].buflen=MAX_ITEM_BUFFER_SIZE;

					// describe the column
					if (!odescr(&cda,col+1,
						&desc[col].dbsize,
						&desc[col].dbtype,
						&desc[col].buf[0],
						&desc[col].buflen,
						&desc[col].dsize,
						&desc[col].precision,
						&desc[col].scale,
						&desc[col].nullok)) {
	
						// define the column
						odefin(&cda,col+1,
							*def_buf[col],
							MAX_ITEM_BUFFER_SIZE,
							NULL_TERMINATED_STRING,
							-1,
							def_indp[col],
							(text *)0,
							-1,
							-1,
							def_col_retlen[col],
							def_col_retcode[col]);
					} else {
						ncols=col;
						break;
					}
					col++;
				}
			}

			// execute the query
			oexec(&cda);

			// go fetch all rows and columns
			int	oldrpc=0;
			for (;;) {
				ofen(&cda,FETCH_AT_ONCE);
				if (cda.rpc==oldrpc) {
					break;
				}
				for (int j=0; j<cda.rpc-oldrpc; j++) {
					for (int i=0; i<ncols; i++) {
						//printf("\"%s\",",def_buf[i][j]);
					}
					//printf("\n");
				}
				oldrpc=cda.rpc;
			}

			oclose(&cda);
		}
	
		// log off
		ologof(&lda);
	}

	printf("total system time used: %ld\n",clock());
	printf("total real time: %ld\n",time(NULL)-starttime);
}
