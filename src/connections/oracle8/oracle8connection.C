// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <oracle8connection.h>

#include <config.h>
#include <datatypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

oracle8connection::oracle8connection() {

	oraclehomeenv=NULL;
	oraclesidenv=NULL;
	twotaskenv=NULL;

	statementmode=OCI_DEFAULT;

#ifdef OCI_ATTR_PROXY_CREDENTIALS
	newsession=NULL;
#endif
}

int	oracle8connection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	oracle8connection::handleConnectString() {
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	sid=connectStringValue("oracle_sid");
	home=connectStringValue("oracle_home");
	char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom && !strcasecmp(autocom,"yes")));
}

int	oracle8connection::logIn() {


	// handle ORACLE_HOME
	if (home) {
		oraclehomeenv=new char[strlen(home)+13];
		sprintf(oraclehomeenv,"ORACLE_HOME=%s",home);
		if (!setEnv("ORACLE_HOME",home,oraclehomeenv)) {
			fprintf(stderr,"Failed to set ORACLE_HOME environment variable.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			return 0;
		}
	} else {
		if (!getenv("ORACLE_HOME")) {
			fprintf(stderr,"No ORACLE_HOME environment variable set or specified in connect string.\n");
			return 0;
		}
	}

	// handle ORACLE_SID
	if (sid) {
		oraclesidenv=new char[strlen(sid)+12];
		sprintf(oraclesidenv,"ORACLE_SID=%s",sid);
		if (!setEnv("ORACLE_SID",sid,oraclesidenv)) {
			fprintf(stderr,"Failed to set ORACLE_SID environment variable.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			delete[] oraclesidenv;
			oraclesidenv=NULL;
			return 0;
		}
	} else {
		if (!getenv("ORACLE_SID")) {
			fprintf(stderr,"No ORACLE_SID environment variable set or specified in connect string.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			return 0;
		}
	}

	// handle TWO_TASK
	if (sid) {
		twotaskenv=new char[strlen(sid)+10];
		sprintf(twotaskenv,"TWO_TASK=%s",sid);
		if (!setEnv("TWO_TASK",sid,twotaskenv)) {
			fprintf(stderr,"Failed to set TWO_TASK environment variable.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			delete[] oraclesidenv;
			oraclesidenv=NULL;
			delete[] twotaskenv;
			twotaskenv=NULL;
			return 0;
		}
	} else {
		if (!getenv("TWO_TASK")) {
			fprintf(stderr,"No TWO_TASK environment variable set or specified in connect string.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			delete[] oraclesidenv;
			oraclesidenv=NULL;
			return 0;
		}
	}

	// init OCI
	OCIInitialize(OCI_DEFAULT,(dvoid *)0,
			(dvoid*(*)(dvoid *,size_t))0,
			(dvoid*(*)(dvoid *,dvoid *,size_t))0,
			(void(*)(dvoid *,dvoid *))0);

	// init the environment
	OCIEnvInit((OCIEnv **)&env,OCI_DEFAULT,(size_t)0,(dvoid **)0); 

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
	if (OCIServerAttach(srv,err,(text *)sid,strlen(sid),0)!=OCI_SUCCESS) {
		delete[] oraclehomeenv;
		oraclehomeenv=NULL;
		delete[] oraclesidenv;
		oraclesidenv=NULL;
		delete[] twotaskenv;
		twotaskenv=NULL;
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return 0;
	}

	// attach the server to the service
	OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)srv,(ub4)0,
				OCI_ATTR_SERVER,(OCIError *)err);

	// allocate a session handle
	OCIHandleAlloc((dvoid *)env,(dvoid **)&session,(ub4)OCI_HTYPE_SESSION,
				(size_t)0,(dvoid **)0);

	// set username and password
	char	*user=getUser();
	char	*password=getPassword();
	OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)user,(ub4)strlen(user),
				(ub4)OCI_ATTR_USERNAME,err);
	OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)password,(ub4)strlen(password),
				(ub4)OCI_ATTR_PASSWORD,err);

	// start a session
	if (OCISessionBegin(svc,err,session,
			OCI_CRED_RDBMS,(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {

		// get the error message from oracle
		text	message[512];
		for (int i=0; i<512; i++) {
			message[i]=(char)NULL;
		}
		sb4	errcode;
		OCIErrorGet((dvoid *)err,1,(text *)0,&errcode,
				message,sizeof(message),OCI_HTYPE_ERROR);
		message[511]=(char)NULL;
		fprintf(stderr,"%s\n",message);

		OCIServerDetach(srv,err,OCI_DEFAULT);
		delete[] oraclehomeenv;
		oraclehomeenv=NULL;
		delete[] oraclesidenv;
		oraclesidenv=NULL;
		delete[] twotaskenv;
		twotaskenv=NULL;
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		return 0;
	}

	// attach the session to the service
	OCIAttrSet((dvoid *)svc,(ub4)OCI_HTYPE_SVCCTX,
				(dvoid *)session,(ub4)0,
				(ub4)OCI_ATTR_SESSION,err);

	// allocate a transaction handle
	OCIHandleAlloc((dvoid *)env,(dvoid **)&trans,OCI_HTYPE_TRANS,0,0);

	// attach the transaction to the service
	OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)trans,(ub4)0,
				(ub4)OCI_ATTR_TRANS,err);
	return 1;
}

sqlrcursor	*oracle8connection::initCursor() {
	return (sqlrcursor *)new oracle8cursor((sqlrconnection *)this);
}

void	oracle8connection::deleteCursor(sqlrcursor *curs) {
	delete (oracle8cursor *)curs;
}

void	oracle8connection::logOut() {

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

	delete[] oraclehomeenv;
	oraclehomeenv=NULL;
	delete[] oraclesidenv;
	oraclesidenv=NULL;
	delete[] twotaskenv;
	twotaskenv=NULL;
}

#ifdef OCI_ATTR_PROXY_CREDENTIALS
int	oracle8connection::changeUser(const char *newuser,
					const char *newpassword) {

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
		return 0;
	}

	// set the user name
	OCIAttrSet((dvoid *)newsession,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)newuser,(ub4)strlen(newuser),
				(ub4)OCI_ATTR_USERNAME,err);

	// don't set the password, use the proxy
	OCIAttrSet((dvoid *)newsession,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)session,(ub4)0,
				(ub4)OCI_ATTR_PROXY_CREDENTIALS,err);

	// start the session
	if (OCISessionBegin(svc,err,newsession,
			OCI_CRED_PROXY,(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
		return 0;
	}

	// switch to the new session
	if (OCIAttrSet((dvoid *)svc,(ub4)OCI_HTYPE_SVCCTX,
				(dvoid *)newsession,(ub4)0,
				(ub4)OCI_ATTR_SESSION,err)!=OCI_SUCCESS) {
		return 0;
	}
	return 1;
}
#endif

unsigned short	oracle8connection::autoCommitOn() {
	statementmode=OCI_COMMIT_ON_SUCCESS;
	return 1;
}

unsigned short	oracle8connection::autoCommitOff() {
	statementmode=OCI_DEFAULT;
	return 1;
}

int	oracle8connection::commit() {
	return (OCITransCommit(svc,err,OCI_DEFAULT)==OCI_SUCCESS);
}

int	oracle8connection::rollback() {
	return (OCITransRollback(svc,err,OCI_DEFAULT)==OCI_SUCCESS);
}

int	oracle8connection::ping() {
	int	retval=0;
	oracle8cursor	*cur=new oracle8cursor(this);
	if (cur->openCursor(-1) && 
		cur->prepareQuery("select 1 from dual",18) && 
		cur->executeQuery("select 1 from dual",18,1)) {
		cur->cleanUpData();
		cur->closeCursor();
		retval=1;
	}
	delete cur;
	return retval;
}

char	*oracle8connection::identify() {
	return "oracle8";
}

oracle8cursor::oracle8cursor(sqlrconnection *conn) : sqlrcursor(conn) {
	prepared=0;
	errormessage=NULL;
	oracle8conn=(oracle8connection *)conn;
	fetchatonce=FETCH_AT_ONCE;
	inbindlobcount=0;
	outbindlobcount=0;
	inbindcount=0;
	outbindcount=0;
	curbindcount=0;
	for (int i=0; i<MAXVAR; i++) {
		inbindpp[i]=NULL;
		outbindpp[i]=NULL;
		curbindpp[i]=NULL;
	}
}

oracle8cursor::~oracle8cursor() {
	delete errormessage;
}

int	oracle8cursor::openCursor(int id) {

	// allocate a cursor handle
	if (OCIHandleAlloc((dvoid *)oracle8conn->env,(dvoid **)&stmt,
			OCI_HTYPE_STMT,(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		return 0;
	}

	// set the number of rows to prefetch
	if (OCIAttrSet((dvoid *)stmt,OCI_HTYPE_STMT,
				(dvoid *)&fetchatonce,(ub4)0,
				OCI_ATTR_PREFETCH_ROWS,
				(OCIError *)oracle8conn->err)) {
		return 0;
	}
	return 1;
}

int	oracle8cursor::closeCursor() {

	if (OCIHandleFree(stmt,OCI_HTYPE_STMT)!=OCI_SUCCESS) {
		return 0;
	}
	return 1;
}

int	oracle8cursor::prepareQuery(const char *query, long length) {

	// keep a pointer to the query and length in case it needs to be 
	// reprepared later
	this->query=(char *)query;
	this->length=length;

	// reset the statement type
	stmttype=0;

	// prepare the query
	if (OCIStmtPrepare(stmt,oracle8conn->err,(text *)query,(ub4)length,
			(ub4)OCI_NTV_SYNTAX,(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
		return 0;
	}
	return 1;
}

void	oracle8cursor::checkRePrepare() {

	// Oracle8 appears to have a bug.
	// You can prepare, bind, execute, rebind, re-execute, etc. with
	// selects, but not with DML, it has to be re-prepared.  What a drag.
	if (!prepared && stmttype && stmttype!=OCI_STMT_SELECT) {
		cleanUpData();
		prepareQuery(query,length);
		prepared=1;
	}
}

int	oracle8cursor::inputBindString(const char *variable,
						unsigned short variablesize,
						const char *value,
						unsigned short valuesize,
						short *isnull) {
	checkRePrepare();

	// the size of the value must include the terminating NULL
	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(ub4)atoi(variable+1),
				(dvoid *)value,(sb4)valuesize+1,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	} else {
		if (OCIBindByName(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,(sb4)valuesize+1,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	}
	inbindcount++;
	return 1;
}


int	oracle8cursor::inputBindLong(const char *variable,
						unsigned short variablesize,
						unsigned long *value) {
	checkRePrepare();

	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(ub4)atoi(variable+1),
				(dvoid *)value,(sb4)sizeof(long),
				SQLT_INT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	} else {
		if (OCIBindByName(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,(sb4)sizeof(long),
				SQLT_INT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	}
	inbindcount++;
	return 1;
}


int	oracle8cursor::inputBindDouble(const char *variable,
						unsigned short variablesize,
						double *value,
						unsigned short precision,
						unsigned short scale) {
	checkRePrepare();

	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(ub4)atoi(variable+1),
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	} else {
		if (OCIBindByName(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	}
	inbindcount++;
	return 1;
}

int	oracle8cursor::inputBindBlob(const char *variable,
						unsigned short variablesize,
						const char *value,
						unsigned long valuesize,
						short *isnull) {
	return inputBindGenericLob(variable,variablesize,
					value,valuesize,isnull,
					OCI_TEMP_BLOB,SQLT_BLOB);
}

int	oracle8cursor::inputBindClob(const char *variable,
						unsigned short variablesize,
						const char *value,
						unsigned long valuesize,
						short *isnull) {
	return inputBindGenericLob(variable,variablesize,
					value,valuesize,isnull,
					OCI_TEMP_CLOB,SQLT_CLOB);
}

int	oracle8cursor::inputBindGenericLob(const char *variable,
						unsigned short variablesize,
						const char *value,
						unsigned long valuesize,
						short *isnull,
						ub1 temptype,
						ub2 type) {

	checkRePrepare();

	// create a temporary lob, write the value to it
	if (OCIDescriptorAlloc((dvoid *)oracle8conn->env,
			(dvoid **)&inbind_lob[inbindlobcount],
			(ub4)OCI_DTYPE_LOB,
			(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		return 0;
	}

	if (OCILobCreateTemporary(oracle8conn->svc,oracle8conn->err,
			inbind_lob[inbindlobcount],
			//(ub2)0,SQLCS_IMPLICIT,
			(ub2)OCI_DEFAULT,OCI_DEFAULT,
			temptype,OCI_ATTR_NOCACHE,
			OCI_DURATION_SESSION)!=OCI_SUCCESS) {
		OCIDescriptorFree(inbind_lob[inbindlobcount],OCI_DTYPE_LOB);
		return 0;
	}

	if (OCILobOpen(oracle8conn->svc,oracle8conn->err,
			inbind_lob[inbindlobcount],
			OCI_LOB_READWRITE)!=OCI_SUCCESS) {
		OCILobFreeTemporary(oracle8conn->svc,oracle8conn->err,
					inbind_lob[inbindlobcount]);
		OCIDescriptorFree(inbind_lob[inbindlobcount],OCI_DTYPE_LOB);
		return 0;
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
		return 0;
	}

	// bind the temporary lob
	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(ub4)atoi(variable+1),
				(dvoid *)&inbind_lob[inbindlobcount],(sb4)0,
				type,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	} else {
		if (OCIBindByName(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)&inbind_lob[inbindlobcount],(sb4)0,
				type,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	}
	inbindlobcount++;
	inbindcount++;
	return 1;
}

int	oracle8cursor::outputBindString(const char *variable,
						unsigned short variablesize,
						char *value,
						unsigned short valuesize,
						short *isnull) {
	checkRePrepare();

	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (OCIBindByPos(stmt,&outbindpp[outbindcount],
				oracle8conn->err,
				(ub4)atoi(variable+1),
				(dvoid *)value,
				(sb4)valuesize,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	} else {
		if (OCIBindByName(stmt,&outbindpp[outbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,
				(sb4)valuesize,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	}
	outbindcount++;
	return 1;
}

int	oracle8cursor::outputBindBlob(const char *variable,
						unsigned short variablesize,
						int index,
						short *isnull) {
	outputBindGenericLob(variable,variablesize,index,isnull,SQLT_BLOB);
}

int	oracle8cursor::outputBindClob(const char *variable,
						unsigned short variablesize,
						int index,
						short *isnull) {
	outputBindGenericLob(variable,variablesize,index,isnull,SQLT_CLOB);
}

int	oracle8cursor::outputBindGenericLob(const char *variable,
						unsigned short variablesize,
						int index,
						short *isnull,
						ub2 type) {

	checkRePrepare();

	// allocate a lob descriptor
	if (OCIDescriptorAlloc((dvoid *)oracle8conn->env,
		(dvoid **)&outbind_lob[index],(ub4)OCI_DTYPE_LOB,
		(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		return 0;
	}
	outbindlobcount=index+1;

	// bind the lob descriptor
	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (OCIBindByPos(stmt,&outbindpp[outbindcount],
			oracle8conn->err,
			(ub4)atoi(variable+1),
			(dvoid *)&outbind_lob[index],(sb4)-1,
			type,
			(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
			OCI_DEFAULT)!=OCI_SUCCESS) {
				return 0;
		}
	} else {
		if (OCIBindByName(stmt,&outbindpp[outbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)&outbind_lob[index],(sb4)-1,
				type,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
		}
	}
	outbindcount++;
	return 1 ;
}

int	oracle8cursor::outputBindCursor(const char *variable,
						unsigned short variablesize,
						sqlrcursor *cursor) {

	checkRePrepare();

	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (OCIBindByPos(stmt,&curbindpp[curbindcount],
				oracle8conn->err,
				(ub4)atoi(variable+1),
				(dvoid *)&(((oracle8cursor *)cursor)->stmt),
				(sb4)0,
				SQLT_RSET,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return 0;
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
			return 0;
		}
	}
	curbindcount++;
	return 1;
}

void    oracle8cursor::returnOutputBindBlob(int index) {
	returnOutputBindGenericLob(index);
}

void    oracle8cursor::returnOutputBindClob(int index) {
	returnOutputBindGenericLob(index);
}

void    oracle8cursor::returnOutputBindGenericLob(int index) {

	// handle lob datatypes
	char	buf[MAX_ITEM_BUFFER_SIZE+1];
	ub4	retlen=MAX_ITEM_BUFFER_SIZE;
	ub4	offset=1;
	int	start=1;

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
				conn->startSendingLong();
				start=0;
			}
			conn->sendLongSegment((char *)buf,(long)retlen);
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
	if (loblength==0) {
		conn->startSendingLong();
		conn->sendLongSegment("",0);
		conn->endSendingLong();
	}
}

int	oracle8cursor::executeQuery(const char *query, long length,
						unsigned short execute) {

	// initialize the column count
	ncols=0;

	// get the type of the query (select, insert, update, etc...)
	if (OCIAttrGet(stmt,OCI_HTYPE_STMT,
			(dvoid *)&stmttype,(ub4 *)NULL,
			OCI_ATTR_STMT_TYPE,oracle8conn->err)!=OCI_SUCCESS) {
		return 0;
	}

	// set up how many times to iterate; 0 for selects, 1 for non-selects
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
		if (OCIStmtExecute(oracle8conn->svc,stmt,oracle8conn->err,iters,
				(ub4)0,NULL,NULL,
				oracle8conn->statementmode)!=OCI_SUCCESS) {
			return 0;
		}

		// reset the prepared flag
		prepared=0;
	}

	// if the query is a select, describe/define it
	if (stmttype==OCI_STMT_SELECT) {

		// get the column count
		if (OCIAttrGet((dvoid *)stmt,OCI_HTYPE_STMT,
				(dvoid *)&ncols,(ub4 *)NULL,
				OCI_ATTR_PARAM_COUNT,
				oracle8conn->err)!=OCI_SUCCESS) {
			return 0;
		}

		// run through the columns...
		for (int i=0; i<ncols; i++) {

			// get the entire column definition
			if (OCIParamGet(stmt,OCI_HTYPE_STMT,
				oracle8conn->err,
				(dvoid **)&desc[i].paramd,i+1)!=OCI_SUCCESS) {
				return 0;
			}

			// get the column name
			if (OCIAttrGet((dvoid *)desc[i].paramd,OCI_DTYPE_PARAM,
				(dvoid **)&desc[i].buf,
				(ub4 *)&desc[i].buflen,
				(ub4)OCI_ATTR_NAME,
				oracle8conn->err)!=OCI_SUCCESS) {
				return 0;
			}

			// get the column type
			if (OCIAttrGet((dvoid *)desc[i].paramd,OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].dbtype,(ub4 *)NULL,
				(ub4)OCI_ATTR_DATA_TYPE,
				oracle8conn->err)!=OCI_SUCCESS) {
				return 0;
			}

			// is the column a LOB?
			if (desc[i].dbtype==BLOB_TYPE ||
				desc[i].dbtype==CLOB_TYPE ||
				desc[i].dbtype==BFILE_TYPE) {

				// return 0 for the size of lobs
				desc[i].dbsize=0;

				// allocate a lob descriptor
				for (int j=0; j<FETCH_AT_ONCE; j++) {
					if (OCIDescriptorAlloc(
						(void *)oracle8conn->env,
						(void **)&def_lob[i][j],
						OCI_DTYPE_LOB,0,0)) {
						return 0;
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
					return 0;
				}

			} else {

				// get the column size
				if (OCIAttrGet((dvoid *)desc[i].paramd,
					OCI_DTYPE_PARAM,
					(dvoid *)&desc[i].dbsize,(ub4 *)NULL,
					(ub4)OCI_ATTR_DATA_SIZE,
					oracle8conn->err)!=OCI_SUCCESS) {
					return 0;
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
					return 0;
				}

				// set the lob member to NULL
				for (int j=0; j<FETCH_AT_ONCE; j++) {
					def_lob[i][j]=NULL;
				}
			}
		}
	}
	return 1;
}

int	oracle8cursor::queryIsNotSelect() {

	if (stmttype==OCI_STMT_SELECT) {
		return 0;
	}
	return 1;
}

int	oracle8cursor::queryIsCommitOrRollback() {

	// apparantly in OCI8, the cursor type gets 
	// set to 0 for both commits and rollbacks
	if (stmttype==0) {
		return 1;
	}
	return 0;
}

char	*oracle8cursor::getErrorMessage(int *liveconnection) {

	// get the message from oracle
	text	message[512];
	for (int i=0; i<512; i++) {
		message[i]=(char)NULL;
	}
	sb4	errcode;
	OCIErrorGet((dvoid *)oracle8conn->err,1,
			(text *)0,&errcode,
			message,sizeof(message),
			OCI_HTYPE_ERROR);
	message[511]=(char)NULL;

	// check for dead connection
	if (errcode==3114 || errcode==3113) {
		*liveconnection=0;
	} else {
		*liveconnection=1;
	}

	// only return an error message if the error wasn't a dead database
	delete errormessage;
	errormessage=new stringbuffer();
	if (*liveconnection) {
		errormessage->append((const char *)message);
	}

	return errormessage->getString();
}

void	oracle8cursor::returnRowCounts() {

	// get the row count
	ub4	rows;
	OCIAttrGet(stmt,OCI_HTYPE_STMT,
			(dvoid *)&rows,(ub4 *)NULL,
			OCI_ATTR_ROW_COUNT,oracle8conn->err);

	// don't know how to get affected row count in OCI8
	conn->sendRowCounts((long)-1,(long)rows);
}

void	oracle8cursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void	oracle8cursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// a useful variable
	int	type;

	// for each column...
	for (int i=0; i<ncols; i++) {

		// set column type
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
		} else if (desc[i].dbtype==LONG_RAW_TYPE) {
			type=LONG_RAW_DATATYPE;
		} else if (desc[i].dbtype==CHAR_TYPE) {
			type=CHAR_DATATYPE;
		} else if (desc[i].dbtype==MLSLABEL_TYPE) {
			type=MLSLABEL_DATATYPE;
		} else if (desc[i].dbtype==BLOB_TYPE) {
			type=BLOB_DATATYPE;
		} else if (desc[i].dbtype==CLOB_TYPE) {
			type=CLOB_DATATYPE;
		} else if (desc[i].dbtype==BFILE_TYPE) {
			type=BFILE_DATATYPE;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send the column definition
		conn->sendColumnDefinition((char *)desc[i].buf,
					(short)desc[i].buflen,
					type,
					(int)desc[i].dbsize);
	}
}

int	oracle8cursor::noRowsToReturn() {
	if (stmttype!=OCI_STMT_SELECT) {
		return 1;
	}
	return 0;
}

int	oracle8cursor::skipRow() {
	if (fetchRow()) {
		row++;
		return 1;
	}
	return 0;
}

int	oracle8cursor::fetchRow() {
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return 0;
	}
	if (row==0) {
		OCIStmtFetch(stmt,oracle8conn->err,fetchatonce,
					OCI_FETCH_NEXT,OCI_DEFAULT);
		ub4	currentrow;
		OCIAttrGet(stmt,OCI_HTYPE_STMT,
				(dvoid *)&currentrow,(ub4 *)NULL,
				OCI_ATTR_ROW_COUNT,oracle8conn->err);
		if (currentrow==totalrows) {
			return 0;
		}
		maxrow=currentrow-totalrows;
		totalrows=currentrow;
	}
	return 1;
}

void	oracle8cursor::returnRow() {

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
			int	start=1;

			// Get the length of the lob. If we fail to read the
			// length, send a NULL field.  Unfortunately, there
			// is OCILobGetLength has no way to express that a
			// LOB is NULL, the result is "undefined" in that case.
			ub4	loblength=0;
			if (OCILobGetLength(oracle8conn->svc,
					oracle8conn->err,
					def_lob[col][row],
					&loblength)!=OCI_SUCCESS) {
				conn->sendNullField();
				continue;
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
				// haven't already), send a segment of the LOB,
				// move to the next segment and reset the
				// amount to read.
				if (retval==OCI_INVALID_HANDLE) {
					conn->sendNullField();
					break;
				} else {
					if (start) {
						conn->startSendingLong();
						start=0;
					}
					conn->sendLongSegment(
						(char *)def_buf[col][row],
						(long)retlen);
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
			if (loblength==0) {
				conn->startSendingLong();
				conn->sendLongSegment("",0);
				conn->endSendingLong();
			}

			// if the lob temporary, deallocate it
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
			continue;
		}

		// handle normal datatypes
		conn->sendField((char *)def_buf[col][row],
					(int)def_col_retlen[col][row]);
	}

	// increment the row counter
	row++;
}

void	oracle8cursor::cleanUpData() {

	// OCI8 version of ocan(), but since it uses OCIStmtFetch we
	// only want to run it if the statement was a select
	if (stmttype==OCI_STMT_SELECT) {
		OCIStmtFetch(stmt,oracle8conn->err,0,
				OCI_FETCH_NEXT,OCI_DEFAULT);
	}

	// free row/column resources
	for (int i=0; i<ncols; i++) {
		for (int j=0; j<FETCH_AT_ONCE; j++) {
			if (def_lob[i][j]) {
				OCIDescriptorFree(def_lob[i][j],OCI_DTYPE_LOB);
			}
		}
		if (def[i]) {
			OCIHandleFree(def[i],OCI_HTYPE_DEFINE);
		}
	}

	// free lob bind resources
	for (i=0; i<inbindlobcount; i++) {
		OCILobFreeTemporary(oracle8conn->svc,oracle8conn->err,
						inbind_lob[i]);
		OCILobClose(oracle8conn->svc,oracle8conn->err,inbind_lob[i]);
		OCIDescriptorFree(inbind_lob[i],OCI_DTYPE_LOB);
	}
	for (i=0; i<outbindlobcount; i++) {
		if (outbind_lob[i]) {
			OCILobFreeTemporary(oracle8conn->svc,oracle8conn->err,
						outbind_lob[i]);
			OCILobClose(oracle8conn->svc,oracle8conn->err,
						outbind_lob[i]);
			OCIDescriptorFree(outbind_lob[i],OCI_DTYPE_LOB);
		}
	}
	inbindlobcount=0;
	outbindlobcount=0;

	// free regular bind resources
	inbindcount=0;
	outbindcount=0;
	curbindcount=0;
}
