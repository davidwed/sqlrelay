// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#include "../../config.h"

#include <rudiments/charstring.h>
#include <rudiments/environment.h>

extern "C" {
	#include <oci.h>
}

#include "sqlrbench.h"

class oraclebench : public sqlrbench {
	public:
		oraclebench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

#define ORACLE_FETCH_AT_ONCE		10
#define ORACLE_MAX_ITEM_BUFFER_SIZE	2048
#define ORACLE_MAX_SELECT_LIST_SIZE	256

struct describe {
	OCIParam	*paramd;
	sb4	dbsize;
	sb2	dbtype;
	text	*buf;
	sb4	buflen;
};

class oraclebenchconnection : public sqlrbenchconnection {
	friend class oraclebenchcursor;
	public:
			oraclebenchconnection(const char *connectstring,
						const char *dbtype);

		bool	connect();
		bool	disconnect();

	private:
		const char	*sid;
		const char	*user;
		const char	*password;

		OCIEnv		*env;
		OCIServer	*srv;
		OCIError	*err;
		OCISvcCtx	*svc;
		OCISession	*session;
		OCITrans	*trans;
};

class oraclebenchcursor : public sqlrbenchcursor {
	public:
			oraclebenchcursor(sqlrbenchconnection *con);

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		oraclebenchconnection	*orabcon;

		OCIStmt		*stmt;
		int32_t		fetchatonce;

		describe	desc[ORACLE_MAX_SELECT_LIST_SIZE];

		OCIDefine	*def[ORACLE_MAX_SELECT_LIST_SIZE];
		ub1		def_buf[ORACLE_MAX_SELECT_LIST_SIZE]
						[ORACLE_FETCH_AT_ONCE]
						[ORACLE_MAX_ITEM_BUFFER_SIZE];
		sb2		def_indp[ORACLE_MAX_SELECT_LIST_SIZE]
							[ORACLE_FETCH_AT_ONCE];
		ub2		def_col_retlen[ORACLE_MAX_SELECT_LIST_SIZE]
							[ORACLE_FETCH_AT_ONCE];
		ub2		def_col_retcode[ORACLE_MAX_SELECT_LIST_SIZE]
							[ORACLE_FETCH_AT_ONCE];
};

oraclebench::oraclebench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug) :
					sqlrbench(connectstring,db,
						queries,rows,cols,colsize,
						samples,rsbs,debug) {
	con=new oraclebenchconnection(connectstring,db);
	cur=new oraclebenchcursor(con);
}


oraclebenchconnection::oraclebenchconnection(
				const char *connectstring,
				const char *db) :
				sqlrbenchconnection(connectstring,db) {
	sid=getParam("sid");
	user=getParam("user");
	password=getParam("password");

	environment::setValue("ORACLE_SID",sid);
	environment::setValue("TWO_TASK",sid);
}

bool oraclebenchconnection::connect() {

#ifdef HAVE_ORACLE_8i
	// initialize OCI
	if (OCIEnvCreate((OCIEnv **)&env,OCI_DEFAULT,(dvoid *)0,
				(dvoid *(*)(dvoid *, size_t))0,
				(dvoid *(*)(dvoid *, dvoid *, size_t))0,
				(void (*)(dvoid *, dvoid *))0,
				(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIEnvCreate Failed\n");
		return false;
	}
#else
	// initialize OCI
	if (OCIInitialize(OCI_DEFAULT,(dvoid *)0, 
				(dvoid*(*)(dvoid *,size_t))0,
				(dvoid*(*)(dvoid *,dvoid *,size_t))0, 
				(void(*)(dvoid *,dvoid *))0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIInitialize Failed\n");
		return false;
	}

	// init the environment
	if (OCIEnvInit((OCIEnv **)&env,OCI_DEFAULT,
				(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIInitialize Failed\n");
		return false;
	}
#endif

	// allocate an error handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&err,OCI_HTYPE_ERROR,
					(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIHandleAlloc (error) Failed\n");
		return false;
	}

	// allocate a server handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&srv,OCI_HTYPE_SERVER,
					(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIHandleAlloc (server) Failed\n");
		return false;
	}

	// allocate a service context handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&svc,OCI_HTYPE_SVCCTX,
					(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIHandleAlloc (svc) Failed\n");
		return false;
	}

	// attach to the server
	if (OCIServerAttach(srv,err,
			(text *)sid,charstring::length(sid),0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIServerAttach Failed\n");
		return false;
	}

	// attach the server to the service
	if (OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)srv,(ub4)0,
				OCI_ATTR_SERVER,(OCIError *)err)!=OCI_SUCCESS) {
		stdoutput.printf("OCIAttrSet (srv-svc) Failed\n");
		return false;
	}

	// allocate a session handle
	if (OCIHandleAlloc((dvoid *)env,
				(dvoid **)&session,(ub4)OCI_HTYPE_SESSION,
				(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIHandleAlloc (session) Failed\n");
		return false;
	}

	// set username and password
	if (OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)user,
				(ub4)charstring::length(user),
				(ub4)OCI_ATTR_USERNAME,err)!=OCI_SUCCESS) {
		stdoutput.printf("OCIAttrSet (user) Failed\n");
		return false;
	}
	if (OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)password,
				(ub4)charstring::length(password),
				(ub4)OCI_ATTR_PASSWORD,err)!=OCI_SUCCESS) {
		stdoutput.printf("OCIAttrSet (password) Failed\n");
		return false;
	}

	// start a session
	if (OCISessionBegin(svc,err,session,
				OCI_CRED_RDBMS,
				(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
		stdoutput.printf("OCISessionBegin Failed\n");
		return false;
	}

	// attach the session to the service
	if (OCIAttrSet((dvoid *)svc,(ub4)OCI_HTYPE_SVCCTX,
				(dvoid *)session,(ub4)0,
				(ub4)OCI_ATTR_SESSION,err)!=OCI_SUCCESS) {
		stdoutput.printf("OCIAttrSet (svc-session) Failed\n");
		return false;
	}
	
	// allocate a transaction handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&trans,
				OCI_HTYPE_TRANS,0,0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIAttrSet (session-svc) Failed\n");
		return false;
	}

	// attach the transaction to the service
	if (OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)trans,(ub4)0,
				(ub4)OCI_ATTR_TRANS,err)!=OCI_SUCCESS) {
		stdoutput.printf("OCIAttrSet (trans-svc) Failed\n");
		return false;
	}

	return true;
}

bool oraclebenchconnection::disconnect() {

	// log off
	OCIHandleFree(trans,OCI_HTYPE_TRANS);
	OCISessionEnd(svc,err,session,OCI_DEFAULT);
	OCIHandleFree(session,OCI_HTYPE_SESSION);
	OCIServerDetach(srv,err,OCI_DEFAULT);

	// clean up
	OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
	OCIHandleFree(srv,OCI_HTYPE_SERVER);
	OCIHandleFree(err,OCI_HTYPE_ERROR);
	OCIHandleFree(env,OCI_HTYPE_ENV);
	return true;
}

oraclebenchcursor::oraclebenchcursor(sqlrbenchconnection *con) :
						sqlrbenchcursor(con) {
	orabcon=(oraclebenchconnection *)con;
}

bool oraclebenchcursor::open() {

	// allocate a statement handle
	if (OCIHandleAlloc((dvoid *)orabcon->env,
			(dvoid **)&stmt,OCI_HTYPE_STMT,
			(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		stdoutput.printf("OCIHandleAlloc (stmt) Failed\n");
		return false;
	}

	// set the number of rows to prefetch
	fetchatonce=ORACLE_FETCH_AT_ONCE;
	if (OCIAttrSet((dvoid *)stmt,OCI_HTYPE_STMT,
			(dvoid *)&fetchatonce,(ub4)0,
			OCI_ATTR_PREFETCH_ROWS,
			(OCIError *)orabcon->err)!=OCI_SUCCESS) {
		stdoutput.printf("OCIAttrSet (prefetch) Failed\n");
		OCIHandleFree(stmt,OCI_HTYPE_STMT);
		return false;
	}

	return true;
}

bool oraclebenchcursor::query(const char *query, bool getcolumns) {

	// prepare the query
	if (OCIStmtPrepare(stmt,orabcon->err,(text *)query,
				(ub4)charstring::length(query),
				(ub4)OCI_NTV_SYNTAX,
				(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
		//stdoutput.printf("OCIStmtPrepare Failed\n");
		return false;
	}

	// get the statement type
	ub2	stmttype;
	if (OCIAttrGet(stmt,OCI_HTYPE_STMT,
				(dvoid *)&stmttype,(ub4 *)NULL,
				OCI_ATTR_STMT_TYPE,
				orabcon->err)!=OCI_SUCCESS) {
		stdoutput.printf("OCIAttrGet (stmt-type) Failed\n");
		return false;
	}

	// execute the query
	if (OCIStmtExecute(orabcon->svc,stmt,orabcon->err,
				(stmttype==OCI_STMT_SELECT)?0:1,
				(ub4)0,NULL,NULL,OCI_DEFAULT)!=OCI_SUCCESS) {
		stdoutput.printf("OCIStmtExecute Failed\n");
		return false;
	}

	// get the column count
	sword	ncols=0;
	if (getcolumns) {
		if (OCIAttrGet((dvoid *)stmt,OCI_HTYPE_STMT,
					(dvoid *)&ncols,(ub4 *)0,
					OCI_ATTR_PARAM_COUNT,
					orabcon->err)!=OCI_SUCCESS) {
			stdoutput.printf("OCIAttrGet (ncols) Failed\n");
			return false;
		}
	}

	// run through the columns...
	for (sword i=0; i<ncols; i++) {

		// get the entire column definition
		OCIParamGet(stmt,OCI_HTYPE_STMT,orabcon->err,
				(dvoid **)&desc[i].paramd,i+1);

		// get the column name
		OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid **)&desc[i].buf,
				(ub4 *)&desc[i].buflen,
				(ub4)OCI_ATTR_NAME,orabcon->err);

		// get the column type
		OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].dbtype,(ub4 *)0,
				(ub4)OCI_ATTR_DATA_TYPE,orabcon->err);

		// get the column size
		OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].dbsize,(ub4 *)0,
				(ub4)OCI_ATTR_DATA_SIZE,orabcon->err);

		// translate to a NULL terminated string
		OCIDefineByPos(stmt,&def[i],orabcon->err,
				i+1,
				(dvoid *)def_buf[i],
				(sb4)ORACLE_MAX_ITEM_BUFFER_SIZE,
				SQLT_STR,
				(dvoid *)def_indp[i],
				(ub2 *)def_col_retlen[i],
				def_col_retcode[i],
				OCI_DEFAULT);

	}

	// fetch all rows and columns
	if (ncols) {
		ub4	newpos;
		ub4	oldpos=0;
		for (;;) {
			OCIStmtFetch(stmt,orabcon->err,fetchatonce,
					OCI_FETCH_NEXT,OCI_DEFAULT);
			OCIAttrGet(stmt,OCI_HTYPE_STMT,
					(dvoid *)&newpos,(ub4 *)0,
					OCI_ATTR_ROW_COUNT,orabcon->err);
			if (newpos==oldpos) {
				break;
			}
			/*for (ub4 j=0; j<newpos-oldpos; j++) {
				for (int i=0; i<ncols; i++) {
					stdoutput.printf("\"%s\",",
							def_buf[i][j]);
				}
				stdoutput.printf("\n");
			}*/
			oldpos=newpos;
		}
	}

	// clean up
	for (uint16_t i=0; i<ncols; i++) {
		OCIDescriptorFree((dvoid *)desc[i].paramd,OCI_DTYPE_PARAM);
	}

	return true;
}

bool oraclebenchcursor::close() {

	OCIHandleFree(stmt,OCI_HTYPE_STMT);
	return true;
}

extern "C" {
	sqlrbench *new_oraclebench(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new oraclebench(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}
