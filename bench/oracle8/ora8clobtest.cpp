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
#define MAX_ITEM_BUFFER_SIZE	32768

extern "C" {
	#include <oci.h>
}

sword		ncols;

OCIEnv		*env;
OCIServer	*srv;
OCIError	*err;
OCISvcCtx	*svc;
OCISession	*session;
OCITrans	*trans;
OCIStmt		*stmt;
int		fetchatonce=FETCH_AT_ONCE;

int main(int argc, char **argv) {

	if (argc<4) {
		printf("usage: ora8test user password sid\n");
		exit(0);
	}

	char	*user=argv[1];
	char	*password=argv[2];
	char	*sid=argv[3];
	char	*query="begin select testclob into :clobindval from testtable2; end;";

	environment	envr;
	envr.setValue("ORACLE_SID",sid);
	envr.setValue("TWO_TASK",sid);

	// init the timer
	time_t	starttime=time(NULL);
	printf("oratest running, please wait...\n");
	clock();

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

	// output bind
	OCILobLocator	*outbind_lob;
	OCIDescriptorAlloc((dvoid *)env,
		(dvoid **)&outbind_lob,(ub4)OCI_DTYPE_LOB,
		(size_t)0,(dvoid **)0);

	// bind the lob descriptor
	OCIBind	*outbind;
	OCIBindByName(stmt,&outbind,err,
				(text *)"clobindval",(sb4)10,
				(dvoid *)&outbind_lob,
				(sb4)sizeof(OCILobLocator *),
				SQLT_CLOB,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT);

	// execute the query
	OCIStmtExecute(svc,stmt,err,iters,(ub4)0,NULL,NULL,
						OCI_DEFAULT);

	// fetch the clob
	ub4	loblength;
	OCILobGetLength(svc,err,outbind_lob,&loblength);
	printf("length=%d\n",loblength);

	char	buf[MAX_ITEM_BUFFER_SIZE];
	ub4	retlen=MAX_ITEM_BUFFER_SIZE;
	ub4	offset=1;
int loop=0;
	while (offset<=loblength) {

		// read a segment from the lob
		sword	retval=OCILobRead(svc,
				err,
				outbind_lob,
				&retlen,
				offset,
				(dvoid *)buf,
				MAX_ITEM_BUFFER_SIZE,
				(dvoid *)NULL,
				(sb4(*)(dvoid *,CONST dvoid *,ub4,ub1))NULL,
				(ub2)0,
				(ub1)SQLCS_IMPLICIT);

		// OCILobRead returns OCI_INVALID_HANDLE if
		// the LOB is NULL.  In that case, return a
		// NULL field.
		// Otherwise, start sending the field (if we
		// haven't already), send a segment of the LOB,
		// move to the next segment and reset the
		// amount to read.
		if (retval==OCI_INVALID_HANDLE) {
			break;
		} else {
			offset=offset+retlen;
			retlen=MAX_ITEM_BUFFER_SIZE;
		}
		loop++;
	}
printf("%d loops\n",loop);

	// free resources
	OCILobFreeTemporary(svc,err,outbind_lob);
	OCILobClose(svc,err,outbind_lob);
	OCIDescriptorFree(outbind_lob,OCI_DTYPE_LOB);
	OCIHandleFree(stmt,OCI_HTYPE_STMT);
	
	// log off
	OCIHandleFree(trans,OCI_HTYPE_TRANS);
	OCISessionEnd(svc,err,session,OCI_DEFAULT);
	OCIHandleFree(session,OCI_HTYPE_SESSION);
	OCIServerDetach(srv,err,OCI_DEFAULT);

	// clean up
	OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
	OCIHandleFree(srv,OCI_HTYPE_SERVER);
	OCIHandleFree(err,OCI_HTYPE_ERROR);


	printf("total system time used: %ld\n",clock());
	printf("total real time: %ld\n",time(NULL)-starttime);
}
