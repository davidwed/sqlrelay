// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <oracle8connection.h>
#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>

#include <config.h>
#include <datatypes.h>

#include <stdio.h>
#include <stdlib.h>

oracle8connection::oracle8connection() {
	statementmode=OCI_DEFAULT;
#ifdef OCI_ATTR_PROXY_CREDENTIALS
	newsession=NULL;
#endif
	environ=new environment();
}

oracle8connection::~oracle8connection() {
	delete environ;
}

uint16_t oracle8connection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void oracle8connection::handleConnectString() {
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	sid=connectStringValue("oracle_sid");
	home=connectStringValue("oracle_home");
	const char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));
}

bool oracle8connection::logIn() {


	// handle ORACLE_HOME
	if (home) {
		if (!environ->setValue("ORACLE_HOME",home)) {
			fprintf(stderr,"Failed to set ORACLE_HOME environment variable.\n");
			return false;
		}
	} else {
		if (!environ->getValue("ORACLE_HOME")) {
			fprintf(stderr,"No ORACLE_HOME environment variable set or specified in connect string.\n");
			return false;
		}
	}

	// handle ORACLE_SID
	if (sid) {
		if (!environ->setValue("ORACLE_SID",sid)) {
			fprintf(stderr,"Failed to set ORACLE_SID environment variable.\n");
			return false;
		}
	} else {
		if (!environ->getValue("ORACLE_SID")) {
			fprintf(stderr,"No ORACLE_SID environment variable set or specified in connect string.\n");
			return false;
		}
	}

	// handle TWO_TASK
	if (sid) {
		if (!environ->setValue("TWO_TASK",sid)) {
			fprintf(stderr,"Failed to set TWO_TASK environment variable.\n");
			return false;
		}
	} else {
		if (!environ->getValue("TWO_TASK")) {
			fprintf(stderr,"No TWO_TASK environment variable set or specified in connect string.\n");
			return false;
		}
	}

	// init OCI
#ifdef HAVE_ORACLE_8i
	if (OCIEnvCreate((OCIEnv **)&env,OCI_DEFAULT,(dvoid *)0,
				(dvoid *(*)(dvoid *, size_t))0,
				(dvoid *(*)(dvoid *, dvoid *, size_t))0,
				(void (*)(dvoid *, dvoid *))0,
				(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		logInError("OCIEnvCreate() failed.");
		return false;
	}
#else
	if (OCIInitialize(OCI_DEFAULT,NULL,NULL,NULL,NULL)!=OCI_SUCCESS) {
		logInError("OCIInitialize() failed.\n");
		return false;
	}
	if (OCIEnvInit((OCIEnv **)&env,OCI_DEFAULT,
					0,(dvoid **)0)!=OCI_SUCCESS) {
		logInError("OCIEnvInit() failed.\n");
		return false;
	}
#endif

	// allocate an error handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&err,
				OCI_HTYPE_ERROR,0,NULL)!=OCI_SUCCESS) {
		logInError("OCIHandleAlloc(OCI_HTYPE_ERROR) failed.\n");
		return false;
	}

	// allocate a server handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&srv,
				OCI_HTYPE_SERVER,0,NULL)!=OCI_SUCCESS) {
		logInError("OCIHandleAlloc(OCI_HTYPE_SERVER) failed.\n");
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return false;
	}

	// allocate a service context handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&svc,
				OCI_HTYPE_SVCCTX,0,NULL)!=OCI_SUCCESS) {
		logInError("OCIHandleAlloc(OCI_HTYPE_SVCCTX) failed.\n");
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return false;
	}

	// attach to the server
	if (OCIServerAttach(srv,err,(text *)sid,
				charstring::length(sid),0)!=OCI_SUCCESS) {
		logInError("OCIServerAttach() failed.\n");
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return false;
	}

	// attach the server to the service
	if (OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)srv,(ub4)0,
				OCI_ATTR_SERVER,(OCIError *)err)!=OCI_SUCCESS) {
		logInError("Attach server to service failed.\n");
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return false;
	}

	// allocate a session handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&session,
				(ub4)OCI_HTYPE_SESSION,0,NULL)!=OCI_SUCCESS) {
		logInError("OCIHandleAlloc(OCI_HTYPE_SESSION) failed.\n");
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return false;
	}

	// set username and password
	const char	*user=getUser();
	const char	*password=getPassword();
	if (OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)user,
				(ub4)charstring::length(user),
				(ub4)OCI_ATTR_USERNAME,err)!=OCI_SUCCESS) {
		logInError("Set username failed.\n");
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		return false;
	}
	if (OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)password,
				(ub4)charstring::length(password),
				(ub4)OCI_ATTR_PASSWORD,err)!=OCI_SUCCESS) {
		logInError("Set password failed.\n");
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		return false;
	}

	// start a session
	if (OCISessionBegin(svc,err,session,
				OCI_CRED_RDBMS,(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
		logInError("OCISessionBegin() failed.\n");
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		return false;
	}

	// attach the session to the service
	if (OCIAttrSet((dvoid *)svc,(ub4)OCI_HTYPE_SVCCTX,
				(dvoid *)session,(ub4)0,
				(ub4)OCI_ATTR_SESSION,err)!=OCI_SUCCESS) {
		logInError("Attach session to service failed.\n");
		OCISessionEnd(svc,err,session,OCI_DEFAULT);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return false;
	}

	// allocate a transaction handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&trans,
				OCI_HTYPE_TRANS,0,0)!=OCI_SUCCESS) {
		logInError("OCIHandleAlloc(OCI_HTYPE_TRANS) failed.\n");
		OCISessionEnd(svc,err,session,OCI_DEFAULT);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return false;
	}

	// attach the transaction to the service
	if (OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)trans,(ub4)0,
				(ub4)OCI_ATTR_TRANS,err)!=OCI_SUCCESS) {
		OCIHandleFree(err,OCI_HTYPE_TRANS);
		OCISessionEnd(svc,err,session,OCI_DEFAULT);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return false;
	}

#ifdef OCI_ATTR_PROXY_CREDENTIALS
	// figure out what version database we're connected to...
	supportsproxycredentials=false;
	char	versionbuf[512];
	if (OCIServerVersion((dvoid *)svc,err,
				(text *)versionbuf,sizeof(versionbuf),
				OCI_HTYPE_SVCCTX)==OCI_SUCCESS &&
			(!charstring::compare(versionbuf,"Oracle8i ",9) ||
			!charstring::compare(versionbuf,"Oracle9i ",9) ||
			!charstring::compare(versionbuf,"Oracle10g ",10))) {
		supportsproxycredentials=true;
	}
#endif
	return true;
}

void oracle8connection::logInError(const char *errmsg) {

	fprintf(stderr,"%s\n\n",errmsg);

	// get the error message from oracle
	text	message[1024];
	rawbuffer::zero((void *)message,sizeof(message));
	sb4	errcode;
	OCIErrorGet((dvoid *)err,1,(text *)0,&errcode,
			message,sizeof(message),OCI_HTYPE_ERROR);
	message[1023]=(char)NULL;
	fprintf(stderr,"%s\n",message);
}

sqlrcursor *oracle8connection::initCursor() {
	return (sqlrcursor *)new oracle8cursor((sqlrconnection *)this);
}

void oracle8connection::deleteCursor(sqlrcursor *curs) {
	delete (oracle8cursor *)curs;
}

void oracle8connection::logOut() {

#ifdef OCI_ATTR_PROXY_CREDENTIALS
	if (newsession) {
		OCISessionEnd(svc,err,newsession,OCI_DEFAULT);
		OCIHandleFree(newsession,OCI_HTYPE_SESSION);
	}
#endif
	OCIHandleFree(trans,OCI_HTYPE_TRANS);
	OCISessionEnd(svc,err,session,OCI_DEFAULT);
	OCIHandleFree(session,OCI_HTYPE_SESSION);
	OCIServerDetach(srv,err,OCI_DEFAULT);

	// free the service, server and error handles
	OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
	OCIHandleFree(srv,OCI_HTYPE_SERVER);
	OCIHandleFree(err,OCI_HTYPE_ERROR);
}

#ifdef OCI_ATTR_PROXY_CREDENTIALS
bool oracle8connection::changeUser(const char *newuser,
					const char *newpassword) {

	// if the database we're connected to doesn't
	// support proxy credentials, use the sqlrconnection
	// class's default changeUser() method
	if (!supportsproxycredentials) {
		return sqlrconnection::changeUser(newuser,newpassword);
	}

	// delete any previously existing "newsessions"
	if (newsession) {
		OCISessionEnd(svc,err,newsession,OCI_DEFAULT);
		OCIHandleFree(newsession,OCI_HTYPE_SESSION);
		newsession=NULL;
	}

	// create a session handle for the new user
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&newsession,
				(ub4)OCI_HTYPE_SESSION,
				(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		return false;
	}

	// set the user name
	OCIAttrSet((dvoid *)newsession,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)newuser,
				(ub4)charstring::length(newuser),
				(ub4)OCI_ATTR_USERNAME,err);

	// don't set the password, use the proxy
	OCIAttrSet((dvoid *)newsession,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)session,(ub4)0,
				(ub4)OCI_ATTR_PROXY_CREDENTIALS,err);

	// start the session
	if (OCISessionBegin(svc,err,newsession,
			OCI_CRED_PROXY,(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
		return false;
	}

	// switch to the new session
	if (OCIAttrSet((dvoid *)svc,(ub4)OCI_HTYPE_SVCCTX,
				(dvoid *)newsession,(ub4)0,
				(ub4)OCI_ATTR_SESSION,err)!=OCI_SUCCESS) {
		return false;
	}
	return true;
}
#endif

bool oracle8connection::autoCommitOn() {
	statementmode=OCI_COMMIT_ON_SUCCESS;
	return true;
}

bool oracle8connection::autoCommitOff() {
	statementmode=OCI_DEFAULT;
	return true;
}

bool oracle8connection::commit() {
	return (OCITransCommit(svc,err,OCI_DEFAULT)==OCI_SUCCESS);
}

bool oracle8connection::rollback() {
	return (OCITransRollback(svc,err,OCI_DEFAULT)==OCI_SUCCESS);
}

const char *oracle8connection::pingQuery() {
	return "select 1 from dual";
}

const char *oracle8connection::identify() {
	return "oracle8";
}

oracle8cursor::oracle8cursor(sqlrconnection *conn) : sqlrcursor(conn) {
	prepared=false;
	errormessage=NULL;
	oracle8conn=(oracle8connection *)conn;
	fetchatonce=FETCH_AT_ONCE;
#ifdef HAVE_ORACLE_8i
	inbindlobcount=0;
	outbindlobcount=0;
#endif
	inbindcount=0;
	outbindcount=0;
	curbindcount=0;
	for (uint16_t i=0; i<MAXVAR; i++) {
		inbindpp[i]=NULL;
		outbindpp[i]=NULL;
		curbindpp[i]=NULL;
	}
	for (uint16_t i=0; i<MAX_SELECT_LIST_SIZE; i++) {
		for (uint32_t j=0; j<FETCH_AT_ONCE; j++) {
			def_lob[i][j]=NULL;
		}
		def[i]=NULL;
	}
#ifdef HAVE_ORACLE_8i
	createtemp.compile("(create|CREATE)[ \\t\\n\\r]+(global|GLOBAL)[ \\t\\n\\r]+(temporary|TEMPORARY)[ \\t\\n\\r]+(table|TABLE)[ \\t\\n\\r]+");
	preserverows.compile("(on|ON)[ \\t\\n\\r]+(commit|COMMIT)[ \\t\\n\\r]+(preserve|PRESERVE)[ \\t\\n\\r]+(rows|ROWS)");
#endif
}

oracle8cursor::~oracle8cursor() {
	delete errormessage;
}

bool oracle8cursor::openCursor(uint16_t id) {

	// allocate a cursor handle
	stmt=NULL;
	if (OCIHandleAlloc((dvoid *)oracle8conn->env,(dvoid **)&stmt,
				OCI_HTYPE_STMT,(size_t)0,
				(dvoid **)0)!=OCI_SUCCESS) {
		return false;
	}

	// set the number of rows to prefetch
	if (OCIAttrSet((dvoid *)stmt,OCI_HTYPE_STMT,
				(dvoid *)&fetchatonce,(ub4)0,
				OCI_ATTR_PREFETCH_ROWS,
				(OCIError *)oracle8conn->err)!=OCI_SUCCESS) {
		return false;
	}
	return sqlrcursor::openCursor(id);
}

bool oracle8cursor::closeCursor() {
	return (OCIHandleFree(stmt,OCI_HTYPE_STMT)==OCI_SUCCESS);
}

bool oracle8cursor::prepareQuery(const char *query, uint32_t length) {

	// keep a pointer to the query and length in case it needs to be 
	// reprepared later
	this->query=(char *)query;
	this->length=length;

	// reset the statement type
	stmttype=0;

	// prepare the query
	return (OCIStmtPrepare(stmt,oracle8conn->err,
				(text *)query,(ub4)length,
				(ub4)OCI_NTV_SYNTAX,
				(ub4)OCI_DEFAULT)==OCI_SUCCESS);
}

void oracle8cursor::checkRePrepare() {

	// Oracle8 appears to have a bug.
	// You can prepare, bind, execute, rebind, re-execute, etc. with
	// selects, but not with DML, it has to be re-prepared.
	// What a drag.
	if (!prepared && stmttype && stmttype!=OCI_STMT_SELECT) {
		cleanUpData(true,true);
		prepareQuery(query,length);
		prepared=true;
	}
}

bool oracle8cursor::inputBindString(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint16_t valuesize,
						int16_t *isnull) {
	checkRePrepare();

	// the size of the value must include the terminating NULL
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)value,(sb4)valuesize+1,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	} else {
		if (OCIBindByName(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,(sb4)valuesize+1,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	inbindcount++;
	return true;
}


bool oracle8cursor::inputBindLong(const char *variable,
						uint16_t variablesize,
						uint32_t *value) {
	checkRePrepare();

	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)value,(sb4)sizeof(int32_t),
				SQLT_INT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	} else {
		if (OCIBindByName(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,(sb4)sizeof(int32_t),
				SQLT_INT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	inbindcount++;
	return true;
}


bool oracle8cursor::inputBindDouble(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale) {
	checkRePrepare();

	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	} else {
		if (OCIBindByName(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	inbindcount++;
	return true;
}

bool oracle8cursor::outputBindString(const char *variable,
						uint16_t variablesize,
						char *value,
						uint16_t valuesize,
						int16_t *isnull) {
	checkRePrepare();

	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[outbindcount],
				oracle8conn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)value,
				(sb4)valuesize,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	} else {
		if (OCIBindByName(stmt,&outbindpp[outbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,
				(sb4)valuesize,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	outbindcount++;
	return true;
}

#ifdef HAVE_ORACLE_8i
bool oracle8cursor::inputBindBlob(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint32_t valuesize,
						int16_t *isnull) {
	return inputBindGenericLob(variable,variablesize,
					value,valuesize,isnull,
					OCI_TEMP_BLOB,SQLT_BLOB);
}

bool oracle8cursor::inputBindClob(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint32_t valuesize,
						int16_t *isnull) {
	return inputBindGenericLob(variable,variablesize,
					value,valuesize,isnull,
					OCI_TEMP_CLOB,SQLT_CLOB);
}

bool oracle8cursor::inputBindGenericLob(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint32_t valuesize,
						int16_t *isnull,
						ub1 temptype,
						ub2 type) {

	checkRePrepare();

	// create a temporary lob, write the value to it
	if (OCIDescriptorAlloc((dvoid *)oracle8conn->env,
			(dvoid **)&inbind_lob[inbindlobcount],
			(ub4)OCI_DTYPE_LOB,
			(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		return false;
	}

	if (OCILobCreateTemporary(oracle8conn->svc,oracle8conn->err,
			inbind_lob[inbindlobcount],
			//(ub2)0,SQLCS_IMPLICIT,
			(ub2)OCI_DEFAULT,OCI_DEFAULT,
			temptype,OCI_ATTR_NOCACHE,
			OCI_DURATION_SESSION)!=OCI_SUCCESS) {
		OCIDescriptorFree(inbind_lob[inbindlobcount],OCI_DTYPE_LOB);
		return false;
	}

	if (OCILobOpen(oracle8conn->svc,oracle8conn->err,
			inbind_lob[inbindlobcount],
			OCI_LOB_READWRITE)!=OCI_SUCCESS) {
		OCILobFreeTemporary(oracle8conn->svc,oracle8conn->err,
					inbind_lob[inbindlobcount]);
		OCIDescriptorFree(inbind_lob[inbindlobcount],OCI_DTYPE_LOB);
		return false;
	}

	ub4	size=valuesize;
	if (OCILobWrite(oracle8conn->svc,oracle8conn->err,
			inbind_lob[inbindlobcount],&size,1,
			(void *)value,valuesize,
			OCI_ONE_PIECE,(dvoid *)0,
			(sb4 (*)(dvoid*,dvoid*,ub4*,ub1 *))0,
			0,SQLCS_IMPLICIT)!=OCI_SUCCESS) {

		OCILobClose(oracle8conn->svc,oracle8conn->err,
					inbind_lob[inbindlobcount]);
		OCILobFreeTemporary(oracle8conn->svc,oracle8conn->err,
					inbind_lob[inbindlobcount]);
		OCIDescriptorFree(inbind_lob[inbindlobcount],OCI_DTYPE_LOB);
		return false;
	}

	// bind the temporary lob
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)&inbind_lob[inbindlobcount],(sb4)0,
				type,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	} else {
		if (OCIBindByName(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)&inbind_lob[inbindlobcount],(sb4)0,
				type,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	inbindlobcount++;
	inbindcount++;
	return true;
}

bool oracle8cursor::outputBindBlob(const char *variable,
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull) {
	return outputBindGenericLob(variable,variablesize,index,
						isnull,SQLT_BLOB);
}

bool oracle8cursor::outputBindClob(const char *variable,
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull) {
	return outputBindGenericLob(variable,variablesize,index,
						isnull,SQLT_CLOB);
}

bool oracle8cursor::outputBindGenericLob(const char *variable,
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull,
						ub2 type) {

	checkRePrepare();

	// allocate a lob descriptor
	if (OCIDescriptorAlloc((dvoid *)oracle8conn->env,
		(dvoid **)&outbind_lob[index],(ub4)OCI_DTYPE_LOB,
		(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		return false;
	}
	outbindlobcount=index+1;

	// bind the lob descriptor
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[outbindcount],
			oracle8conn->err,
			(ub4)charstring::toInteger(variable+1),
			(dvoid *)&outbind_lob[index],
			//(sb4)-1,
			(sb4)sizeof(OCILobLocator *),
			type,
			(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
			OCI_DEFAULT)!=OCI_SUCCESS) {
				return false;
		}
	} else {
		if (OCIBindByName(stmt,&outbindpp[outbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)&outbind_lob[index],
				//(sb4)-1,
				(sb4)sizeof(OCILobLocator *),
				type,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	outbindcount++;
	return true;
}

bool oracle8cursor::outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrcursor *cursor) {

	checkRePrepare();

	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&curbindpp[curbindcount],
				oracle8conn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)&(((oracle8cursor *)cursor)->stmt),
				(sb4)0,
				SQLT_RSET,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	} else {
		if (OCIBindByName(stmt,&curbindpp[curbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)&(((oracle8cursor *)cursor)->stmt),
				(sb4)0,
				SQLT_RSET,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	curbindcount++;
	return true;
}

void oracle8cursor::returnOutputBindBlob(uint16_t index) {
	returnOutputBindGenericLob(index);
}

void oracle8cursor::returnOutputBindClob(uint16_t index) {
	returnOutputBindGenericLob(index);
}

void oracle8cursor::returnOutputBindGenericLob(uint16_t index) {

	// handle lob datatypes
	char	buf[MAX_ITEM_BUFFER_SIZE+1];
	ub4	retlen=MAX_ITEM_BUFFER_SIZE;
	ub4	offset=1;
	bool	start=true;

	// Get the length of the lob. If we fail to read the
	// length, send a NULL field.  Unfortunately, there
	// is OCILobGetLength has no way to express that a
	// LOB is NULL, the result is "undefined" in that case.
	ub4	loblength=0;
	if (OCILobGetLength(oracle8conn->svc,
			oracle8conn->err,
			outbind_lob[index],
			&loblength)!=OCI_SUCCESS) {
		conn->sendNullField();
		return;
	}

	// We should be able to call OCILobRead over and over,
	// as long as it returns OCI_NEED_DATA, but OCILobRead
	// fails to return OCI_NEED_DATA (at least in version
	// 8.1.7 for Linux), so we have to do this instead.
	// OCILobRead appears to return OCI_INVALID_HANDLE when
	// the LOB is NULL, but this is not documented anywhere.
	while (offset<=loblength) {

		// read a segment from the lob
		sword	retval=OCILobRead(oracle8conn->svc,
				oracle8conn->err,
				outbind_lob[index],
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
			conn->sendNullField();
			break;
		} else {
			if (start) {
				conn->startSendingLong(loblength);
				start=false;
			}
			conn->sendLongSegment((char *)buf,(int32_t)retlen);
			offset=offset+retlen;
			retlen=MAX_ITEM_BUFFER_SIZE;
		}
	}

	// if we ever started sending a LOB,
	// finish sending it now
	if (!start) {
		conn->endSendingLong();
	}

	// handle empty lob's
	if (!loblength) {
		conn->startSendingLong(0);
		conn->sendLongSegment("",0);
		conn->endSendingLong();
	}
}

void oracle8cursor::checkForTempTable(const char *query, uint32_t length) {

	char	*ptr=(char *)query;
	char	*endptr=(char *)query+length;

	// skip any leading comments
	if (!skipWhitespace(&ptr,endptr) || !skipComment(&ptr,endptr) ||
		!skipWhitespace(&ptr,endptr)) {
		return;
	}

	// look for "create global temporary table "
	if (createtemp.match(ptr)) {
		ptr=createtemp.getSubstringEnd(0);
	} else {
		return;
	}

	// get the table name
	stringbuffer	tablename;
	while (*ptr!=' ' && *ptr!='\n' && *ptr!='	' && ptr<endptr) {
		tablename.append(*ptr);
		ptr++;
	}

	// append to list of temp tables
	// check for "on commit preserve rows" otherwise assume
	// "on commit delete rows"
	if (preserverows.match(ptr)) {
		conn->addSessionTempTableForTrunc(tablename.getString());
	}
}
#endif

bool oracle8cursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

	// initialize the column count
	ncols=0;

	// get the type of the query (select, insert, update, etc...)
	if (OCIAttrGet(stmt,OCI_HTYPE_STMT,
			(dvoid *)&stmttype,(ub4 *)NULL,
			OCI_ATTR_STMT_TYPE,oracle8conn->err)!=OCI_SUCCESS) {
		return false;
	}
#ifdef HAVE_ORACLE_8i
	if (stmttype==OCI_STMT_CREATE) {
		checkForTempTable(query,length);
	}
#endif

	// set up how many times to iterate;
	// 0 for selects, 1 for non-selects
	ub4	iters=1;
	if (stmttype==OCI_STMT_SELECT) {
		iters=0;
	}

	// initialize row counters
	row=0;
	maxrow=0;
	totalrows=0;

	// execute the query
	if (execute) {
		if (OCIStmtExecute(oracle8conn->svc,stmt,
				oracle8conn->err,iters,
				(ub4)0,NULL,NULL,
				oracle8conn->statementmode)!=OCI_SUCCESS) {
			return false;
		}

		// reset the prepared flag
		prepared=false;
	}

	// if the query is a select, describe/define it
	if (stmttype==OCI_STMT_SELECT) {

		// get the column count
		if (OCIAttrGet((dvoid *)stmt,OCI_HTYPE_STMT,
				(dvoid *)&ncols,(ub4 *)NULL,
				OCI_ATTR_PARAM_COUNT,
				oracle8conn->err)!=OCI_SUCCESS) {
			return false;
		}

		// run through the columns...
		for (sword i=0; i<ncols; i++) {

			// get the entire column definition
			if (OCIParamGet(stmt,OCI_HTYPE_STMT,
				oracle8conn->err,
				(dvoid **)&desc[i].paramd,
				i+1)!=OCI_SUCCESS) {
				return false;
			}

			// get the column name
			if (OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid **)&desc[i].buf,
				(ub4 *)&desc[i].buflen,
				(ub4)OCI_ATTR_NAME,
				oracle8conn->err)!=OCI_SUCCESS) {
				return false;
			}

			// get the column type
			if (OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].dbtype,(ub4 *)NULL,
				(ub4)OCI_ATTR_DATA_TYPE,
				oracle8conn->err)!=OCI_SUCCESS) {
				return false;
			}

			// get the column precision
			if (OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].precision,(ub4 *)NULL,
				(ub4)OCI_ATTR_PRECISION,
				oracle8conn->err)!=OCI_SUCCESS) {
				return false;
			}

			// get the column scale
			if (OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].scale,(ub4 *)NULL,
				(ub4)OCI_ATTR_SCALE,
				oracle8conn->err)!=OCI_SUCCESS) {
				return false;
			}

			// get whether the column is nullable
			if (OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].nullok,(ub4 *)NULL,
				(ub4)OCI_ATTR_IS_NULL,
				oracle8conn->err)!=OCI_SUCCESS) {
				return false;
			}

			// is the column a LOB?
			if (desc[i].dbtype==BLOB_TYPE ||
				desc[i].dbtype==CLOB_TYPE ||
				desc[i].dbtype==BFILE_TYPE) {

				// return 0 for the size of lobs
				desc[i].dbsize=0;

				// allocate a lob descriptor
				for (uint32_t j=0; j<FETCH_AT_ONCE; j++) {
					if (OCIDescriptorAlloc(
						(void *)oracle8conn->env,
						(void **)&def_lob[i][j],
						OCI_DTYPE_LOB,0,0)) {
						return false;
					}
				}

				// define the column as a lob
				if (OCIDefineByPos(stmt,&def[i],
					oracle8conn->err,
					i+1,
					(dvoid *)def_lob[i],
					(sb4)-1,
					desc[i].dbtype,
					(dvoid *)0,
					0,
					(ub2 *)0,
					OCI_DEFAULT)!=OCI_SUCCESS) {
					return false;
				}

			} else {

				// get the column size
				if (OCIAttrGet((dvoid *)desc[i].paramd,
					OCI_DTYPE_PARAM,
					(dvoid *)&desc[i].dbsize,
					(ub4 *)NULL,
					(ub4)OCI_ATTR_DATA_SIZE,
					oracle8conn->err)!=OCI_SUCCESS) {
					return false;
				}

				// if the column is not a LOB, define it,
				// translated to a NULL terminated string
				if (OCIDefineByPos(stmt,&def[i],
					oracle8conn->err,
					i+1,
					(dvoid *)def_buf[i],
					(sb4)MAX_ITEM_BUFFER_SIZE,
					SQLT_STR,
					(dvoid *)def_indp[i],
					(ub2 *)def_col_retlen[i],
					def_col_retcode[i],
					OCI_DEFAULT)!=OCI_SUCCESS) {
					return false;
				}

				// set the lob member to NULL
				for (uint32_t j=0; j<FETCH_AT_ONCE; j++) {
					def_lob[i][j]=NULL;
				}
			}
		}
	}

	return true;
}

bool oracle8cursor::queryIsNotSelect() {
	return (stmttype!=OCI_STMT_SELECT);
}

bool oracle8cursor::queryIsCommitOrRollback() {
	// apparantly in OCI8, the cursor type gets 
	// set to 0 for both commits and rollbacks
	return (!stmttype);
}

const char *oracle8cursor::getErrorMessage(bool *liveconnection) {

	// get the message from oracle
	text	message[1024];
	rawbuffer::zero((void *)message,sizeof(message));
	sb4	errcode;
	OCIErrorGet((dvoid *)oracle8conn->err,1,
			(text *)0,&errcode,
			message,sizeof(message),
			OCI_HTYPE_ERROR);
	message[1023]=(char)NULL;

	// check for dead connection
	if (errcode==3114 || errcode==3113) {
		*liveconnection=false;
	} else {
		*liveconnection=true;
	}

	// only return an error message if the error wasn't a dead database
	delete errormessage;
	errormessage=new stringbuffer();
	if (*liveconnection) {
		errormessage->append((const char *)message);
	}

	return errormessage->getString();
}

void oracle8cursor::returnRowCounts() {

	// get the row count
	ub4	rows;
	OCIAttrGet(stmt,OCI_HTYPE_STMT,
			(dvoid *)&rows,(ub4 *)NULL,
			OCI_ATTR_ROW_COUNT,oracle8conn->err);

	// don't know how to get affected row count in OCI8
	conn->sendRowCounts(false,0,true,rows);
}

void oracle8cursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void oracle8cursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// a useful variable
	uint16_t	type;

	// for each column...
	for (sword i=0; i<ncols; i++) {

		// set column type
		uint16_t	binary=0;
		if (desc[i].dbtype==VARCHAR2_TYPE) {
			type=VARCHAR2_DATATYPE;
		} else if (desc[i].dbtype==NUMBER_TYPE) {
			type=NUMBER_DATATYPE;
		} else if (desc[i].dbtype==LONG_TYPE) {
			type=LONG_DATATYPE;
		} else if (desc[i].dbtype==ROWID_TYPE) {
			type=ROWID_DATATYPE;
		} else if (desc[i].dbtype==DATE_TYPE) {
			type=DATE_DATATYPE;
		} else if (desc[i].dbtype==RAW_TYPE) {
			type=RAW_DATATYPE;
			binary=1;
		} else if (desc[i].dbtype==LONG_RAW_TYPE) {
			type=LONG_RAW_DATATYPE;
			binary=1;
		} else if (desc[i].dbtype==CHAR_TYPE) {
			type=CHAR_DATATYPE;
		} else if (desc[i].dbtype==MLSLABEL_TYPE) {
			type=MLSLABEL_DATATYPE;
		} else if (desc[i].dbtype==BLOB_TYPE) {
			type=BLOB_DATATYPE;
			binary=1;
		} else if (desc[i].dbtype==CLOB_TYPE) {
			type=CLOB_DATATYPE;
		} else if (desc[i].dbtype==BFILE_TYPE) {
			type=BFILE_DATATYPE;
			binary=1;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send the column definition
		conn->sendColumnDefinition((char *)desc[i].buf,
					(uint16_t)desc[i].buflen,
					type,
					(uint32_t)desc[i].dbsize,
					(uint32_t)desc[i].precision,
					(uint32_t)desc[i].scale,
					(uint16_t)desc[i].nullok,0,0,
					0,0,0,binary,0);
	}
}

bool oracle8cursor::noRowsToReturn() {
	return (stmttype!=OCI_STMT_SELECT);
}

bool oracle8cursor::skipRow() {
	if (fetchRow()) {
		row++;
		return true;
	}
	return false;
}

bool oracle8cursor::fetchRow() {
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return false;
	}
	if (!row) {
		OCIStmtFetch(stmt,oracle8conn->err,fetchatonce,
					OCI_FETCH_NEXT,OCI_DEFAULT);
		ub4	currentrow;
		OCIAttrGet(stmt,OCI_HTYPE_STMT,
				(dvoid *)&currentrow,(ub4 *)NULL,
				OCI_ATTR_ROW_COUNT,oracle8conn->err);
		if (currentrow==totalrows) {
			return false;
		}
		maxrow=currentrow-totalrows;
		totalrows=currentrow;
	}
	return true;
}

void oracle8cursor::returnRow() {

	for (sword col=0; col<ncols; col++) {

		// handle NULL's
		if (def_indp[col][row]) {
			conn->sendNullField();
			continue;
		}

		// in OCI8, longs are just like other datatypes, but LOBS
		// are different
		if (desc[col].dbtype==BLOB_TYPE ||
			desc[col].dbtype==CLOB_TYPE ||
			desc[col].dbtype==BFILE_TYPE) {

			// handle lob datatypes
			ub4	retlen=MAX_ITEM_BUFFER_SIZE;
			ub4	offset=1;
			bool	start=true;

			// Get the length of the lob. If we fail to read the
			// length, send a NULL field.  Unfortunately, there
			// is OCILobGetLength has no way to express that a
			// LOB is NULL, the result is "undefined" in that
			// case.
			ub4	loblength=0;
			if (OCILobGetLength(oracle8conn->svc,
					oracle8conn->err,
					def_lob[col][row],
					&loblength)!=OCI_SUCCESS) {
				conn->sendNullField();
				continue;
			}

			// We should be able to call OCILobRead over and
			// over, as long as it returns OCI_NEED_DATA, but
			// OCILobRead fails to return OCI_NEED_DATA (at
			// least in version 8.1.7 for Linux), so we have to
			// do this instead.  OCILobRead appears to return
			// OCI_INVALID_HANDLE when the LOB is NULL, but
			// this is not documented anywhere.
			while (offset<=loblength) {

				// read a segment from the lob
				sword	retval=OCILobRead(oracle8conn->svc,
						oracle8conn->err,
						def_lob[col][row],
						&retlen,
						offset,
						(dvoid *)&def_buf[col][row],
						MAX_ITEM_BUFFER_SIZE,
						(dvoid *)NULL,
				(sb4(*)(dvoid *,CONST dvoid *,ub4,ub1))NULL,
						(ub2)0,
						(ub1)SQLCS_IMPLICIT);

				// OCILobRead returns OCI_INVALID_HANDLE if
				// the LOB is NULL.  In that case, return a
				// NULL field.
				// Otherwise, start sending the field (if we
				// haven't already), send a segment of the
				// LOB, move to the next segment and reset
				// the amount to read.
				if (retval==OCI_INVALID_HANDLE) {
					conn->sendNullField();
					break;
				} else {
					if (start) {
						conn->startSendingLong(
								loblength);
						start=false;
					}
					conn->sendLongSegment(
						(char *)def_buf[col][row],
						(int32_t)retlen);
					offset=offset+retlen;
					retlen=MAX_ITEM_BUFFER_SIZE;
				}
			}

			// if we ever started sending a LOB,
			// finish sending it now
			if (!start) {
				conn->endSendingLong();
			}

			// handle empty lob's
			if (!loblength) {
				conn->startSendingLong(0);
				conn->sendLongSegment("",0);
				conn->endSendingLong();
			}

#ifdef HAVE_ORACLE_8i
			// if the lob is temporary, deallocate it
			boolean	templob;
			if (OCILobIsTemporary(oracle8conn->env,
						oracle8conn->err,
						def_lob[col][row],
						&templob)!=OCI_SUCCESS) {
				continue;
			}
			if (templob) {
				OCILobFreeTemporary(oracle8conn->svc,
							oracle8conn->err,
							def_lob[col][row]);
			}
#endif
			continue;
		}

		// handle normal datatypes
		conn->sendField((char *)def_buf[col][row],
					(int)def_col_retlen[col][row]);
	}

	// increment the row counter
	row++;
}

void oracle8cursor::cleanUpData(bool freeresult, bool freebinds) {

	// OCI8 version of ocan(), but since it uses OCIStmtFetch we
	// only want to run it if the statement was a select
	if (freeresult && stmttype==OCI_STMT_SELECT) {
		OCIStmtFetch(stmt,oracle8conn->err,0,
				OCI_FETCH_NEXT,OCI_DEFAULT);
	}

	// free row/column resources
	if (freeresult) {
		for (sword i=0; i<ncols; i++) {
			for (uint32_t j=0; j<FETCH_AT_ONCE; j++) {
				if (def_lob[i][j]) {
					OCIDescriptorFree(def_lob[i][j],
								OCI_DTYPE_LOB);
					def_lob[i][j]=NULL;
				}
			}
			if (def[i]) {
				OCIHandleFree(def[i],OCI_HTYPE_DEFINE);
				def[i]=NULL;
			}
		}
	}

	if (freebinds) {
		// free lob bind resources
#ifdef HAVE_ORACLE_8i
		for (uint16_t i=0; i<inbindlobcount; i++) {
			OCILobFreeTemporary(oracle8conn->svc,
							oracle8conn->err,
							inbind_lob[i]);
			OCILobClose(oracle8conn->svc,oracle8conn->err,
							inbind_lob[i]);
			OCIDescriptorFree(inbind_lob[i],OCI_DTYPE_LOB);
		}
		for (uint16_t i=0; i<outbindlobcount; i++) {
			if (outbind_lob[i]) {
				OCILobFreeTemporary(oracle8conn->svc,
							oracle8conn->err,
							outbind_lob[i]);
				OCILobClose(oracle8conn->svc,
							oracle8conn->err,
							outbind_lob[i]);
				OCIDescriptorFree(outbind_lob[i],
							OCI_DTYPE_LOB);
			}
		}
		inbindlobcount=0;
		outbindlobcount=0;
#endif

		// free regular bind resources
		inbindcount=0;
		outbindcount=0;
		curbindcount=0;
	}
}
