// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <stdlib.h>
#include <rudiments/environment.h>
#include <time.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	#include <ctpublic.h>
}

#ifndef HAVE_FREETDS_FUNCTION_DEFINITIONS
	#include <ctfunctions.h>
#endif

#define FETCH_AT_ONCE 1
#define MAX_SELECT_LIST_SIZE 256
#define MAX_ITEM_BUFFER_SIZE 2048

CS_CONTEXT	*context;
CS_LOCALE	*locale;
CS_CONNECTION	*conn;
CS_COMMAND	*cmd;
CS_INT		results_type;
CS_INT		ncols;
CS_DATAFMT	column[MAX_SELECT_LIST_SIZE];
char		data[MAX_SELECT_LIST_SIZE]
			[FETCH_AT_ONCE][MAX_ITEM_BUFFER_SIZE];
CS_INT		datalength[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];
CS_SMALLINT	nullindicator[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];
CS_INT		rowcount;

int main(int argc, char **argv) {

	if (argc<8) {
		printf("usage: freetdstest server port user password query iterations queriesperiteration\n");
		exit(0);
	}

	char	*server=argv[1];
	char	*port=argv[2];
	char	*user=argv[3];
	char	*password=argv[4];
	char	*query=argv[5];
	int	iterations=atoi(argv[6]);
	int	queriesperiteration=atoi(argv[7]);

	environment	env;
	env.setValue("DSQUERY",server);
	env.setValue("TDSPORT",port);
	env.setValue("DSLIB_PORT",port);

	// init the timer
	time_t	starttime=time(NULL);
	clock();

	for (int count=0; count<iterations; count++) {

		// allocate a context
		context=(CS_CONTEXT *)NULL;
		cs_ctx_alloc(CS_VERSION_100,&context);

		// init the context
		ct_init(context,CS_VERSION_100);

		// allocate a connection
		ct_con_alloc(context,&conn);

		// set the user/password to use
		ct_con_props(conn,CS_SET,CS_USERNAME,user,
				CS_NULLTERM,(CS_INT *)NULL);
		ct_con_props(conn,CS_SET,CS_PASSWORD,password,
				CS_NULLTERM,(CS_INT *)NULL);

		// connect to the database
		if (ct_connect(conn,(CS_CHAR *)NULL,(CS_INT)0)!=CS_SUCCEED) {
			printf("ct_connect failed...\n");
			exit(0);
		}

		for (int qcount=0; qcount<queriesperiteration; qcount++) {

			// allocate a command structure
			ct_cmd_alloc(conn,&cmd);

			// initialize number of columns
			ncols=0;

			// initiate a language command
			ct_command(cmd,CS_LANG_CMD,query,CS_NULLTERM,CS_UNUSED);

			// send the command
			ct_send(cmd);

			// get the results, sybase is weird, a query can return 
			// multiple result sets.  We're only interested in the
			// first one though, the rest will be cancelled
			ct_results(cmd,&results_type);

			// get the number of columns
			ct_res_info(cmd,CS_NUMDATA,(CS_VOID *)&ncols,
					CS_UNUSED,(CS_INT *)NULL);

			// for each column...
			for (int i=0; i<(int)ncols; i++) {
	
				// get the column description
				ct_describe(cmd,i+1,&column[i]);
	
				column[i].datatype=CS_CHAR_TYPE;
				column[i].format=CS_FMT_NULLTERM;
				column[i].maxlength=MAX_ITEM_BUFFER_SIZE;
				column[i].scale=CS_UNUSED;
				column[i].precision=CS_UNUSED;
				column[i].status=CS_UNUSED;
				column[i].count=FETCH_AT_ONCE;
				column[i].usertype=CS_UNUSED;
				column[i].locale=NULL;

				// bind the column for the fetches
				ct_bind(cmd,i+1,&column[i],(CS_VOID *)data[i],
						datalength[i],nullindicator[i]);
			}
	
			// go fetch all rows and columns
			for (;;) {
				ct_fetch(cmd,CS_UNUSED,CS_UNUSED,
						CS_UNUSED,&rowcount);
				if (rowcount==0) {
					break;
				}

				// print row
				for (int row=0; row<(int)rowcount; row++) {
					for (int col=0; col<(int)ncols; col++) {
						if (nullindicator[col][row]>-1
							&& 
							datalength[col][row]) {
							printf("%s,",
								data[col][row]);
						} else {
							printf("NULL,");
						}
					}
					printf("\n");
				}
			}


			// cancel any extra result sets
			ct_cancel(NULL,cmd,CS_CANCEL_ALL);

			// clean up
			ct_cmd_drop(cmd);
			cs_loc_drop(context,locale);
		}

		// clean up
		ct_close(conn,CS_UNUSED);
		ct_con_drop(conn);
		ct_exit(context,CS_UNUSED);
		cs_ctx_drop(context);
	}

	printf("total system time used: %d\n",clock());
	printf("total real time: %d\n",time(NULL)-starttime);
}
