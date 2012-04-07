// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"
#include <rudiments/environment.h>
#include <rudiments/stringbuffer.h>
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

OCIEnv		*env;
OCIServer	*srv;
OCIError	*err;
OCISvcCtx	*svc;
OCISession	*session;
OCITrans	*trans;
OCIStmt		*stmt;

int main(int argc, char **argv) {

	if (argc<3) {
		printf("usage: ora8test user password sid\n");
		exit(0);
	}

	char	*user=argv[1];
	char	*password=argv[2];
	char	*sid=argv[3];

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

	// allocate a statement handle
	OCIHandleAlloc((dvoid *)env,(dvoid **)&stmt,
			OCI_HTYPE_STMT,
			(size_t)0,(dvoid **)0);

	// drop the table
	const char	*q="drop table vis_acesso_usuario_programa";
	OCIStmtPrepare(stmt,err,
			(text *)q,
			(ub4)strlen(q),
			(ub4)OCI_NTV_SYNTAX,
			(ub4)OCI_DEFAULT);
	OCIStmtExecute(svc,stmt,err,1,(ub4)0,NULL,NULL,OCI_DEFAULT);

	// create the table
	q="create table vis_acesso_usuario_programa (nom_usuario char(8) not null , num_programa char(8) not null , opcao char(2) not null , ies_permite char(1), primary key (nom_usuario,num_programa,opcao))";
	OCIStmtPrepare(stmt,err,
			(text *)q,
			(ub4)strlen(q),
			(ub4)OCI_NTV_SYNTAX,
			(ub4)OCI_DEFAULT);
	OCIStmtExecute(svc,stmt,err,1,(ub4)0,NULL,NULL,OCI_DEFAULT);

	stringbuffer	query;
	for (int qcount=0; qcount<1000; qcount++) {

		query.clear();
		query.append("insert into vis_acesso_usuario_programa values ('")->append(qcount)->append("','hello','he','h')");

		// prepare and execute the query
		OCIStmtPrepare(stmt,err,(text *)query.getString(),
				(ub4)query.getStringLength(),
				(ub4)OCI_NTV_SYNTAX,
				(ub4)OCI_DEFAULT);
		//OCIStmtExecute(svc,stmt,err,1,(ub4)0,NULL,NULL,OCI_DEFAULT);
		OCIStmtExecute(svc,stmt,err,1,(ub4)0,NULL,NULL,OCI_COMMIT_ON_SUCCESS);
	}

	// drop the table
	q="drop table vis_acesso_usuario_programa";
	OCIStmtPrepare(stmt,err,
			(text *)q,
			(ub4)strlen(q),
			(ub4)OCI_NTV_SYNTAX,
			(ub4)OCI_DEFAULT);
	OCIStmtExecute(svc,stmt,err,1,(ub4)0,NULL,NULL,OCI_DEFAULT);

	// free resources
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
