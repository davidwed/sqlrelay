#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/bytestring.h>
#include <rudiments/environment.h>
#include <rudiments/stdio.h>
#include <config.h>

extern "C" {
	#include <oci.h>
}

OCIError	*err;

void error() {
	text	message[1024];
	bytestring::zero(message,sizeof(message));
	sb4	errcode;
	OCIErrorGet(err,1,NULL,&errcode,
			message,sizeof(message),OCI_HTYPE_ERROR);
	message[1023]='\0';
	stdoutput.printf("\n%s\n",message);
}

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
			stdoutput.printf("failure ");
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
		stdoutput.printf("failure ");
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%d\"!=\"%d\"\n",value,success);
		stdoutput.printf("failure ");
error();
		process::exit(1);
	}
}

int	main(int argc, char **argv) {

	const char	*sid;
	const char	*user;
	const char	*password;

	OCIEnv		*env;
	OCIServer	*srv;
	OCISvcCtx	*svc;
	OCISession	*session;
	OCITrans	*trans;
	OCIStmt		*stmt;
	const char	*query;
	ub2		stmttype;
	sword		ncols;
	OCIParam	*param;
	text		*colname;
	ub4		colnamelen;
	OCIDefine	*def;
	ub1		field[1024];
	ub2		indp;
	ub2		retlen;
	ub2		retcode;
	ub4		currentrow;


	// to run against a real instance, provide an sid
	// eg: ./oracle ora1
	if (argc==2) {
		sid=argv[1];
	} else {
		sid="sqlrelay";
	}
	user="scott";
	password="tiger";

	environment::setValue("ORACLE_SID",sid);
	environment::setValue("TWO_TASK",sid);

	stdoutput.printf("\n================ Connect ==============\n");
	#ifdef HAVE_OREACLE_8i
		checkSuccess(
			OCIEnvCreate((OCIEnv **)&env,
					OCI_DEFAULT|OCI_OBJECT,
					NULL,NULL,NULL,NULL,0),
			OCI_SUCCESS);
	#else
		checkSuccess(
			OCIInitialize(OCI_DEFAULT,NULL,NULL,NULL,NULL),
			OCI_SUCCESS);
		checkSuccess(
			OCIEnvInit((OCIEnv **)&env,OCI_DEFAULT,0,NULL),
			OCI_SUCCESS);
	#endif
	checkSuccess(
		OCIHandleAlloc(env,(void **)&err,OCI_HTYPE_ERROR,0,NULL),
		OCI_SUCCESS);
	checkSuccess(
		OCIHandleAlloc(env,(void **)&srv,OCI_HTYPE_SERVER,0,NULL),
		OCI_SUCCESS);
	checkSuccess(
		OCIHandleAlloc(env,(void **)&svc,OCI_HTYPE_SVCCTX,0,NULL),
		OCI_SUCCESS);
	checkSuccess(
		OCIServerAttach(srv,err,(text *)sid,charstring::length(sid),0),
		OCI_SUCCESS);
	checkSuccess(
		OCIAttrSet(svc,OCI_HTYPE_SVCCTX,srv,0,OCI_ATTR_SERVER,err),
		OCI_SUCCESS);
	checkSuccess(
		OCIHandleAlloc(env,(void **)&session,OCI_HTYPE_SESSION,0,NULL),
		OCI_SUCCESS);
	checkSuccess(
		OCIAttrSet(session,OCI_HTYPE_SESSION,
				(void *)user,charstring::length(user),
				OCI_ATTR_USERNAME,err),
		OCI_SUCCESS);
	checkSuccess(
		OCIAttrSet(session,OCI_HTYPE_SESSION,
				(void *)password,charstring::length(password),
				OCI_ATTR_PASSWORD,err),
		OCI_SUCCESS);
	checkSuccess(
		OCISessionBegin(svc,err,session,OCI_CRED_RDBMS,OCI_DEFAULT),
		OCI_SUCCESS);
	checkSuccess(
		OCIAttrSet(svc,OCI_HTYPE_SVCCTX,session,0,OCI_ATTR_SESSION,err),
		OCI_SUCCESS);
	checkSuccess(
		OCIHandleAlloc(env,(void **)&trans,OCI_HTYPE_TRANS,0,NULL),
		OCI_SUCCESS);
	checkSuccess(
		OCIAttrSet(svc,OCI_HTYPE_SVCCTX,trans,0,OCI_ATTR_TRANS,err),
		OCI_SUCCESS);
	stdoutput.printf("\n\n");


	stdoutput.printf("\n================ Server Version =======\n");
	char	versionbuf[512];
	checkSuccess(
		OCIServerVersion(svc,err,
				(text *)versionbuf,sizeof(versionbuf),
				OCI_HTYPE_SVCCTX),
		OCI_SUCCESS);
	stdoutput.printf("\n%s\n",versionbuf);
	stdoutput.printf("\n");


	stdoutput.printf("\n================ Open Cursor ==========\n");
	checkSuccess(
		OCIHandleAlloc(env,(void **)&stmt,OCI_HTYPE_STMT,0,NULL),
		OCI_SUCCESS);
	stdoutput.printf("\n\n");


	stdoutput.printf("\n================ Prepare ==============\n");
	query="select 'hello' as hello from dual";
	checkSuccess(
		OCIStmtPrepare(stmt,err,
				(text *)query,charstring::length(query),
				OCI_NTV_SYNTAX,
				//OCI_V7_SYNTAX,
				OCI_DEFAULT),
		OCI_SUCCESS);
	checkSuccess(
		OCIStmtExecute(svc,stmt,err,0,0,NULL,NULL,OCI_PARSE_ONLY),
		OCI_SUCCESS);
	checkSuccess(
		OCIAttrGet(stmt,OCI_HTYPE_STMT,
				&stmttype,NULL,OCI_ATTR_STMT_TYPE,err),
		OCI_SUCCESS);
	checkSuccess(stmttype,OCI_STMT_SELECT);
	stdoutput.printf("\n\n");


	stdoutput.printf("\n================ Describe =============\n");
	checkSuccess(
		OCIStmtExecute(svc,stmt,err,0,0,NULL,NULL,OCI_DESCRIBE_ONLY),
		OCI_SUCCESS);
	checkSuccess(
		OCIAttrGet(stmt,OCI_HTYPE_STMT,
				&ncols,NULL,
				OCI_ATTR_PARAM_COUNT,err),
		OCI_SUCCESS);
	checkSuccess(ncols,1);
	checkSuccess(
		OCIParamGet(stmt,OCI_HTYPE_STMT,err,(void **)&param,1),
		OCI_SUCCESS);
	checkSuccess(
		OCIAttrGet(param,OCI_DTYPE_PARAM,
				&colname,&colnamelen,
				OCI_ATTR_NAME,err),
		OCI_SUCCESS);
	checkSuccess((const char *)colname,"HELLO");
	checkSuccess(colnamelen,5);
	stdoutput.printf("\n\n");


	stdoutput.printf("\n================ Execute ==============\n");
	checkSuccess(
		OCIStmtExecute(svc,stmt,err,0,0,NULL,NULL,OCI_DEFAULT),
		OCI_SUCCESS);
	stdoutput.printf("\n\n");


	stdoutput.printf("\n================ Define ===============\n");
	checkSuccess(
		OCIDefineByPos(stmt,&def,err,1,
				field,sizeof(field),
				SQLT_STR,
				&indp,&retlen,&retcode,
				OCI_DEFAULT),
		OCI_SUCCESS);
	stdoutput.printf("\n\n");


	stdoutput.printf("\n================ Fetch ================\n");
	checkSuccess(
		OCIStmtFetch(stmt,err,1,OCI_FETCH_NEXT,OCI_DEFAULT),
		OCI_SUCCESS);
	checkSuccess(
		OCIAttrGet(stmt,OCI_HTYPE_STMT,
				&currentrow,NULL,
				OCI_ATTR_ROW_COUNT,err),
		OCI_SUCCESS);
	checkSuccess(currentrow,1);
	checkSuccess((const char *)field,"hello");
	stdoutput.printf("\n\n");
}
