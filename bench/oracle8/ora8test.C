// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"
#include <rudiments/environment.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

#define FETCH_AT_ONCE		10
#define MAX_ITEM_BUFFER_SIZE	2048
#define MAX_SELECT_LIST_SIZE	256

extern "C" {
	#include <oci.h>
}

struct describe {
	OCIParam	*paramd;
	sb4	dbsize;
	sb2	dbtype;
	text	*buf;
	sb4	buflen;
};
	
sword		ncols;

describe	desc[MAX_SELECT_LIST_SIZE];

OCIDefine	*def[MAX_SELECT_LIST_SIZE];
ub1		def_buf[MAX_SELECT_LIST_SIZE]
			[FETCH_AT_ONCE][MAX_ITEM_BUFFER_SIZE];
sb2		def_indp[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];
ub2		def_col_retlen[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];
ub2		def_col_retcode[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];

OCIEnv		*env;
OCIServer	*srv;
OCIError	*err;
OCISvcCtx	*svc;
OCISession	*session;
OCITrans	*trans;
OCIStmt		*stmt;
int		fetchatonce=FETCH_AT_ONCE;

int main(int argc, char **argv) {

	if (argc<7) {
		printf("usage: ora8test user password sid query iterations queriesperiteration\n");
		exit(0);
	}

	char	*user=argv[1];
	char	*password=argv[2];
	char	*sid=argv[3];
	char	*query=argv[4];
	int	iterations=atoi(argv[5]);
	int	queriesperiteration=atoi(argv[6]);

	environment	envr;
	envr.setValue("ORACLE_SID",sid);
	envr.setValue("TWO_TASK",sid);

	// init the timer
	time_t	starttime=time(NULL);
	printf("oratest running, please wait...\n");
	clock();

	for (int count=0; count<iterations; count++) {

#ifdef HAVE_ORACLE_8i
		OCIEnvCreate((OCIEnv **)&env,OCI_DEFAULT,(dvoid *)0,
				(dvoid *(*)(dvoid *, size_t))0,
				(dvoid *(*)(dvoid *, dvoid *, size_t))0,
				(void (*)(dvoid *, dvoid *))0,
				(size_t)0,(dvoid **)0);
#else
		// initialize OCI
		OCIInitialize(OCI_DEFAULT,(dvoid *)0, 
			(dvoid*(*)(dvoid *,size_t))0,
			(dvoid*(*)(dvoid *,dvoid *,size_t))0, 
			(void(*)(dvoid *,dvoid *))0);

		// init the environment
		OCIEnvInit((OCIEnv **)&env,OCI_DEFAULT,(size_t)0,(dvoid **)0);
#endif

		// allocate an error handle
		OCIHandleAlloc((dvoid *)env,(dvoid **)&err,OCI_HTYPE_ERROR,
				(size_t)0,(dvoid **)0);
		// allocate a server handle
		OCIHandleAlloc((dvoid *)env,(dvoid **)&srv,OCI_HTYPE_SERVER,
				(size_t)0,(dvoid **)0);
		// allocate a service context handle
		OCIHandleAlloc((dvoid *)env,(dvoid **)&svc,OCI_HTYPE_SVCCTX,
				(size_t)0,(dvoid **)0);



		// attach to the server
		OCIServerAttach(srv,err,(text *)sid,strlen(sid),0);

		// attach the server to the service
		OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)srv,(ub4)0,
				OCI_ATTR_SERVER,(OCIError *)err);

		// allocate a session handle
		OCIHandleAlloc((dvoid *)env,
				(dvoid **)&session,(ub4)OCI_HTYPE_SESSION,
				(size_t)0,(dvoid **)0);

		// set username and password
		OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)user,(ub4)strlen(user),
				(ub4)OCI_ATTR_USERNAME,err);
		OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)password,(ub4)strlen(password),
				(ub4)OCI_ATTR_PASSWORD,err);

		// start a session
		OCISessionBegin(svc,err,session,
				OCI_CRED_RDBMS,(ub4)OCI_DEFAULT);

		// attach the session to the service
		OCIAttrSet((dvoid *)svc,(ub4)OCI_HTYPE_SVCCTX,
					(dvoid *)session,(ub4)0,
					(ub4)OCI_ATTR_SESSION,err);
	
		// allocate a transaction handle
		OCIHandleAlloc((dvoid *)env,
				(dvoid **)&trans,OCI_HTYPE_TRANS,0,0);

		// attach the transaction to the service
		OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)trans,(ub4)0,
				(ub4)OCI_ATTR_TRANS,err);

		for (int qcount=0; qcount<queriesperiteration; qcount++) {

			// initialize the column count
			ncols=0;

			// allocate a statement handle
			OCIHandleAlloc((dvoid *)env,(dvoid **)&stmt,
					OCI_HTYPE_STMT,
					(size_t)0,(dvoid **)0);

			// set the number of rows to prefetch
			OCIAttrSet((dvoid *)stmt,OCI_HTYPE_STMT,
					(dvoid *)&fetchatonce,(ub4)0,
					OCI_ATTR_PREFETCH_ROWS,(OCIError *)err);

			// prepare the query
			OCIStmtPrepare(stmt,err,(text *)query,
					(ub4)strlen(query),
					(ub4)OCI_NTV_SYNTAX,
					(ub4)OCI_DEFAULT);

			ub2	stmttype;
			OCIAttrGet(stmt,OCI_HTYPE_STMT,
					(dvoid *)&stmttype,(ub4 *)NULL,
					OCI_ATTR_STMT_TYPE,err);

			ub4	iters=(stmttype!=OCI_STMT_SELECT);

			// execute the query
			OCIStmtExecute(svc,stmt,err,iters,(ub4)0,NULL,NULL,
								OCI_DEFAULT);


			OCIAttrGet((dvoid *)stmt,OCI_HTYPE_STMT,
					(dvoid *)&ncols,(ub4 *)0,
					OCI_ATTR_PARAM_COUNT,err);

			// run through the columns...
			for (int i=0; i<ncols; i++) {

				// get the entire column definition
				OCIParamGet(stmt,OCI_HTYPE_STMT,err,
					(dvoid **)&desc[i].paramd,i+1);

				// get the column name
				OCIAttrGet((dvoid *)desc[i].paramd,
					OCI_DTYPE_PARAM,
					(dvoid **)&desc[i].buf,
					(ub4 *)&desc[i].buflen,
					(ub4)OCI_ATTR_NAME,err);

				// get the column type
				OCIAttrGet((dvoid *)desc[i].paramd,
					OCI_DTYPE_PARAM,
					(dvoid *)&desc[i].dbtype,(ub4 *)0,
					(ub4)OCI_ATTR_DATA_TYPE,err);

				// get the column size
				OCIAttrGet((dvoid *)desc[i].paramd,
					OCI_DTYPE_PARAM,
					(dvoid *)&desc[i].dbsize,(ub4 *)0,
					(ub4)OCI_ATTR_DATA_SIZE,err);

				// translate to a NULL terminated string
				OCIDefineByPos(stmt,&def[i],err,
					i+1,
					(dvoid *)def_buf[i],
					(sb4)MAX_ITEM_BUFFER_SIZE,
					SQLT_STR,
					(dvoid *)def_indp[i],
					(ub2 *)def_col_retlen[i],
					def_col_retcode[i],
					OCI_DEFAULT);

			}

			// go fetch all rows and columns
			if (ncols) {
				ub4	newpos;
				ub4	oldpos=0;
				for (;;) {
					OCIStmtFetch(stmt,err,fetchatonce,
						OCI_FETCH_NEXT,OCI_DEFAULT);
					OCIAttrGet(stmt,OCI_HTYPE_STMT,
						(dvoid *)&newpos,(ub4 *)0,
						OCI_ATTR_ROW_COUNT,err);
					if (newpos==oldpos) {
						break;
					}
					for (ub4 j=0; j<newpos-oldpos; j++) {
						for (int i=0; i<ncols; i++) {
							//printf("\"%s\",",
							//	def_buf[i][j]);
						}
						//printf("\n");
					}
					oldpos=newpos;
				}
			}

			// free resources
			OCIHandleFree(stmt,OCI_HTYPE_STMT);
			for (int i=0; i<ncols; i++) {
				if (def[i]) {
					OCIHandleFree(def[i],OCI_HTYPE_DEFINE);
				}
			}
		}
	
		// log off
		OCIHandleFree(trans,OCI_HTYPE_TRANS);
		OCISessionEnd(svc,err,session,OCI_DEFAULT);
		OCIHandleFree(session,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);

		// clean up
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
	}

	printf("total system time used: %ld\n",clock());
	printf("total real time: %ld\n",time(NULL)-starttime);
}
