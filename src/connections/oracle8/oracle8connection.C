// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <oracle8connection.h>
#include <oracle8sqltranslator.h>
#include <oracle8sqlwriter.h>
#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/character.h>
#include <rudiments/environment.h>

#include <datatypes.h>

#define MAX_BYTES_PER_CHAR	4

oracle8connection::oracle8connection() : sqlrconnection_svr() {
	statementmode=OCI_DEFAULT;
#ifdef OCI_ATTR_PROXY_CREDENTIALS
	newsession=NULL;
#endif
}

oracle8connection::~oracle8connection() {
}

uint16_t oracle8connection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void oracle8connection::handleConnectString() {
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	sid=connectStringValue("oracle_sid");
	home=connectStringValue("oracle_home");
	nlslang=connectStringValue("nls_lang");
	const char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));
	fetchatonce=charstring::toUnsignedInteger(
				connectStringValue("fetchatonce"));
	if (!fetchatonce) {
		fetchatonce=FETCH_AT_ONCE;
	}
	maxselectlistsize=charstring::toUnsignedInteger(
				connectStringValue("maxselectlistsize"));
	if (!maxselectlistsize) {
		maxselectlistsize=MAX_SELECT_LIST_SIZE;
	}
	maxitembuffersize=charstring::toUnsignedInteger(
				connectStringValue("maxitembuffersize"));
	if (!maxitembuffersize) {
		maxitembuffersize=MAX_ITEM_BUFFER_SIZE;
	}
	if (maxitembuffersize<MAX_BYTES_PER_CHAR) {
		maxitembuffersize=MAX_BYTES_PER_CHAR;
	}
	setFakeTransactionBlocksBehavior(
		!charstring::compare(
			connectStringValue("faketransactionblocks"),"yes"));
	setTranslateBindVariablesBehavior(
		!charstring::compare(
			connectStringValue("translatebindvariables"),"yes"));

#ifdef HAVE_ORACLE_8i
	droptemptables=!charstring::compare(
				connectStringValue("droptemptables"),"yes");
#endif
}

void oracle8connection::dropTempTables(sqlrcursor_svr *cursor,
					stringlist *tablelist) {

	// When dropping temporary tables at the end of the session,
	// if any of those tables were created with "on commit preserve rows"
	// then the session has to exit before the table can be dropped or
	// oracle will return the following error:
	// ORA-14452: attempt to create, alter or drop an index on temporary
	// table already in use
	// It's not really clear why, but that's the case.  We'll accomplish
	// this by re-logging in, then dropping the tables
	if (tablelist==&sessiontemptablesfordrop &&
					tablelist->getLength() &&
					droptemptables) {
		// FIXME: this is only necessary if a table was created with
		// "on commit preserve rows", we should have a flag set for
		// that.
		reLogIn();
	}

	sqlrconnection_svr::dropTempTables(cursor,tablelist);
}

bool oracle8connection::logIn(bool printerrors) {

	// get user/password
	const char	*user=getUser();
	const char	*password=getPassword();

	// handle ORACLE_SID
	if (sid) {
		if (!environment::setValue("ORACLE_SID",sid)) {
			if (printerrors) {
				fprintf(stderr,"Failed to set ORACLE_SID environment variable.\n");
			}
			return false;
		}
	} else {
		if (!environment::getValue("ORACLE_SID")) {
			if (printerrors) {
				fprintf(stderr,"No ORACLE_SID environment variable set or specified in connect string.\n");
			}
			return false;
		}
	}

	// handle TWO_TASK
	if (sid) {
		if (!environment::setValue("TWO_TASK",sid)) {
			if (printerrors) {
				fprintf(stderr,"Failed to set TWO_TASK environment variable.\n");
			}
			return false;
		}
	} else {
		// allow empty TWO_TASK when using OS-authentication
		// allow it in all cases?
		if (!environment::getValue("TWO_TASK") &&
				charstring::length(user) &&
				charstring::length(password)) {
			if (printerrors) {
				fprintf(stderr,"No TWO_TASK environment variable set or specified in connect string.\n");
			}
			return false;
		}
	}

	// see if the specified sid is in tnsnames.ora format
	bool	sidtnsnameformat=(charstring::length(sid) &&
					sid[charstring::length(sid)-1]==')');

	// handle ORACLE_HOME
	if (home) {
		if (!environment::setValue("ORACLE_HOME",home)) {
			if (printerrors) {
				fprintf(stderr,"Failed to set ORACLE_HOME environment variable.\n");
			}
			return false;
		}
	} else {
		if (!sidtnsnameformat &&
				!environment::getValue("ORACLE_HOME")) {
			if (printerrors) {
				fprintf(stderr,"No ORACLE_HOME environment variable set or specified in connect string.\n");
			}
			return false;
		}
	}

	// handle NLS_LANG
	if (nlslang) {
		if (!environment::setValue("NLS_LANG",nlslang)) {
			if (printerrors) {
				fprintf(stderr,"Failed to set NLS_LANG environment variable.\n");
			}
			return false;
		}
	}

	// init OCI
#ifdef HAVE_ORACLE_8i
	if (OCIEnvCreate((OCIEnv **)&env,OCI_DEFAULT|OCI_OBJECT,(dvoid *)0,
				(dvoid *(*)(dvoid *, size_t))0,
				(dvoid *(*)(dvoid *, dvoid *, size_t))0,
				(void (*)(dvoid *, dvoid *))0,
				(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCIEnvCreate() failed.");
		}
		return false;
	}
#else
	if (OCIInitialize(OCI_DEFAULT,NULL,NULL,NULL,NULL)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCIInitialize() failed.\n");
		}
		return false;
	}
	if (OCIEnvInit((OCIEnv **)&env,OCI_DEFAULT,
					0,(dvoid **)0)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCIEnvInit() failed.\n");
		}
		return false;
	}
#endif

	// allocate an error handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&err,
				OCI_HTYPE_ERROR,0,NULL)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCIHandleAlloc(OCI_HTYPE_ERROR) failed.\n");
		}
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// allocate a server handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&srv,
				OCI_HTYPE_SERVER,0,NULL)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCIHandleAlloc(OCI_HTYPE_SERVER) failed.\n");
		}
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// allocate a service context handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&svc,
				OCI_HTYPE_SVCCTX,0,NULL)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCIHandleAlloc(OCI_HTYPE_SVCCTX) failed.\n");
		}
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// attach to the server
	if (OCIServerAttach(srv,err,(text *)sid,
				charstring::length(sid),0)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCIServerAttach() failed.\n");
		}
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// attach the server to the service
	if (OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)srv,(ub4)0,
				OCI_ATTR_SERVER,(OCIError *)err)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("Attach server to service failed.\n");
		}
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// allocate a session handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&session,
				(ub4)OCI_HTYPE_SESSION,0,NULL)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCIHandleAlloc(OCI_HTYPE_SESSION) failed.\n");
		}
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// set username and password
	if (charstring::length(user) &&
		OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)user,
				(ub4)charstring::length(user),
				(ub4)OCI_ATTR_USERNAME,err)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("Set username failed.\n");
		}
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}
	if (charstring::length(password) &&
		OCIAttrSet((dvoid *)session,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)password,
				(ub4)charstring::length(password),
				(ub4)OCI_ATTR_PASSWORD,err)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("Set password failed.\n");
		}
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// start a session
	sword	cred=OCI_CRED_RDBMS;
	if (!charstring::length(user) && !charstring::length(password)) {
		cred=OCI_CRED_EXT;
	}
	if (OCISessionBegin(svc,err,session,
				cred,(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCISessionBegin() failed.\n");
		}
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// attach the session to the service
	if (OCIAttrSet((dvoid *)svc,(ub4)OCI_HTYPE_SVCCTX,
				(dvoid *)session,(ub4)0,
				(ub4)OCI_ATTR_SESSION,err)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("Attach session to service failed.\n");
		}
		OCISessionEnd(svc,err,session,OCI_DEFAULT);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// allocate a transaction handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&trans,
				OCI_HTYPE_TRANS,0,0)!=OCI_SUCCESS) {
		if (printerrors) {
			logInError("OCIHandleAlloc(OCI_HTYPE_TRANS) failed.\n");
		}
		OCISessionEnd(svc,err,session,OCI_DEFAULT);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
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
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

#ifdef OCI_ATTR_PROXY_CREDENTIALS
	// figure out what version database we're connected to...
	supportsproxycredentials=false;
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
	message[1023]='\0';
	fprintf(stderr,"error: %s\n",message);
}

sqlrcursor_svr *oracle8connection::initCursor() {
	return (sqlrcursor_svr *)new oracle8cursor((sqlrconnection_svr *)this);
}

void oracle8connection::deleteCursor(sqlrcursor_svr *curs) {
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
	OCIHandleFree(env,OCI_HTYPE_ENV);
}

#ifdef OCI_ATTR_PROXY_CREDENTIALS
bool oracle8connection::changeUser(const char *newuser,
					const char *newpassword) {

	// if the database we're connected to doesn't
	// support proxy credentials, use the sqlrconnection
	// class's default changeUser() method
	if (!supportsproxycredentials) {
		return sqlrconnection_svr::changeUser(newuser,newpassword);
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

bool oracle8connection::supportsTransactionBlocks() {
	return false;
}

bool oracle8connection::commit() {
	return (OCITransCommit(svc,err,OCI_DEFAULT)==OCI_SUCCESS);
}

bool oracle8connection::rollback() {
	return (OCITransRollback(svc,err,OCI_DEFAULT)==OCI_SUCCESS);
}

sqlwriter *oracle8connection::getSqlWriter() {
	return new oracle8sqlwriter;
}

sqltranslator *oracle8connection::getSqlTranslator() {
	return new oracle8sqltranslator;
}

const char *oracle8connection::pingQuery() {
	return "select 1 from dual";
}

const char *oracle8connection::identify() {
	return "oracle8";
}

const char *oracle8connection::dbVersion() {
	if (OCIServerVersion((dvoid *)svc,err,
				(text *)versionbuf,sizeof(versionbuf),
				OCI_HTYPE_SVCCTX)==OCI_SUCCESS) {
		return versionbuf;
	}
	return NULL;
}

const char *oracle8connection::getDatabaseListQuery(bool wild) {
	return (wild)?"select username from all_users "
			"where username like upper('%s') order by username":
			"select username from all_users order by username";
}

const char *oracle8connection::getTableListQuery(bool wild) {
	return (wild)?"select table_name from user_tables "
			"where table_name like upper('%s') "
				"order by table_name":
			"select table_name from user_tables "
				"order by table_name";
}

const char *oracle8connection::getColumnListQuery(bool wild) {
	return (wild)? "select "
			"	column_name, "
			"	data_type, "
			"	data_length, "
			"	data_precision, "
			"	data_scale, "
			"	nullable, "
			"	'' as key, "
			"	data_default, "
			"	'' as extra "
			"from "
			"	all_tab_columns "
			"where "
			"	table_name=upper('%s') "
			"	and "
			"	column_name like upper('%s') "
			"order by "
			"	column_id":

			"select "
			"	column_name, "
			"	data_type, "
			"	data_length, "
			"	data_precision, "
			"	data_scale, "
			"	nullable, "
			"	'' as key, "
			"	data_default, "
			"	'' as extra "
			"from "
			"	all_tab_columns "
			"where "
			"	table_name=upper('%s') "
			"order by "
			"	column_id";
}

const char *oracle8connection::selectDatabaseQuery() {
	return "alter session set current_schema=%s";
}

const char *oracle8connection::getCurrentDatabaseQuery() {
	return "select sys_context('userenv','current_schema') from dual";
}

oracle8cursor::oracle8cursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {

	stmt=NULL;
	stmttype=0;
	ncols=0;

	prepared=false;
	errormessage=NULL;
	oracle8conn=(oracle8connection *)conn;
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
		inintbindstring[i]=NULL;
		outintbindstring[i]=NULL;
		outintbind[i]=NULL;
	}

	desc=new describe[oracle8conn->maxselectlistsize];
	columnnames=new char *[oracle8conn->maxselectlistsize];
	def=new OCIDefine *[oracle8conn->maxselectlistsize];
	def_lob=new OCILobLocator **[oracle8conn->maxselectlistsize];
	def_buf=new ub1 *[oracle8conn->maxselectlistsize];
	def_indp=new sb2 *[oracle8conn->maxselectlistsize];
	def_col_retlen=new ub2 *[oracle8conn->maxselectlistsize];
	def_col_retcode=new ub2 *[oracle8conn->maxselectlistsize];
	for (uint16_t i=0; i<oracle8conn->maxselectlistsize; i++) {
		def_lob[i]=new OCILobLocator *[oracle8conn->fetchatonce];
		for (uint32_t j=0; j<oracle8conn->fetchatonce; j++) {
			def_lob[i][j]=NULL;
		}
		def_buf[i]=new ub1[oracle8conn->fetchatonce*
					oracle8conn->maxitembuffersize];
		def_indp[i]=new sb2[oracle8conn->fetchatonce];
		def_col_retlen[i]=new ub2[oracle8conn->fetchatonce];
		def_col_retcode[i]=new ub2[oracle8conn->fetchatonce];
		def[i]=NULL;
	}

#ifdef HAVE_ORACLE_8i
	createtemp.compile("(create|CREATE)[ \\t\\n\\r]+(global|GLOBAL)[ \\t\\n\\r]+(temporary|TEMPORARY)[ \\t\\n\\r]+(table|TABLE)[ \\t\\n\\r]+");
	createtemp.study();
	deleterows.compile("(on|ON)[ \\t\\n\\r]+(commit|COMMIT)[ \\t\\n\\r]+(delete|DELETE)[ \\t\\n\\r]+(rows|ROWS)");
	deleterows.study();
	preserverows.compile("(on|ON)[ \\t\\n\\r]+(commit|COMMIT)[ \\t\\n\\r]+(preserve|PRESERVE)[ \\t\\n\\r]+(rows|ROWS)");
	preserverows.study();
	asselect.compile("(as|AS)[ \\t\\n\\r]+\\(?[ \\t\\n\\r]?(select|SELECT)[ \\t\\n\\r]+");
	asselect.study();
#endif
}

oracle8cursor::~oracle8cursor() {

	delete errormessage;

	for (uint16_t i=0; i<oracle8conn->maxselectlistsize; i++) {
		delete[] def_col_retcode[i];
		delete[] def_col_retlen[i];
		delete[] def_indp[i];
		delete[] def_lob[i];
		delete[] def_buf[i];
	}
	for (uint16_t i=0; i<inbindcount; i++) {
		delete[] inintbindstring[i];
	}
	for (uint16_t i=0; i<outbindcount; i++) {
		delete[] outintbindstring[i];
	}
	delete[] def_col_retcode;
	delete[] def_col_retlen;
	delete[] def_indp;
	delete[] def_lob;
	delete[] def_buf;
	delete[] def;
	delete[] desc;
	delete[] columnnames;
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
				(dvoid *)&(oracle8conn->fetchatonce),(ub4)0,
				OCI_ATTR_PREFETCH_ROWS,
				(OCIError *)oracle8conn->err)!=OCI_SUCCESS) {
		return false;
	}
	return true;
}

bool oracle8cursor::closeCursor() {
	// Renat says that we should do this here. I'm not sure we need
	// to though.
	cleanUpData(true,true);
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
						uint32_t valuesize,
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


bool oracle8cursor::inputBindInteger(const char *variable,
						uint16_t variablesize,
						int64_t *value) {
	checkRePrepare();

	inintbindstring[inbindcount]=charstring::parseNumber(*value);

	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)inintbindstring[inbindcount],
				(sb4)charstring::length(
					inintbindstring[inbindcount])+1,
				SQLT_STR,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	} else {
		if (OCIBindByName(stmt,&inbindpp[inbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)inintbindstring[inbindcount],
				(sb4)charstring::length(
					inintbindstring[inbindcount])+1,
				SQLT_STR,
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

	outintbindstring[outbindcount]=NULL;

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

bool oracle8cursor::outputBindInteger(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull) {
	checkRePrepare();

	outintbindstring[outbindcount]=new char[21];
	rawbuffer::zero(outintbindstring[outbindcount],21);
	outintbind[outbindcount]=value;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[outbindcount],
				oracle8conn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)outintbindstring[outbindcount],
				(sb4)21,
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
				(dvoid *)outintbindstring[outbindcount],
				(sb4)21,
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

bool oracle8cursor::outputBindDouble(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull) {
	checkRePrepare();

	outintbindstring[outbindcount]=NULL;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[outbindcount],
				oracle8conn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	} else {
		if (OCIBindByName(stmt,&outbindpp[outbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
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
						sqlrcursor_svr *cursor) {

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

	// initialize values as if a statement has been prepared and executed
	((oracle8cursor *)cursor)->stmttype=0;
	((oracle8cursor *)cursor)->ncols=0;
	((oracle8cursor *)cursor)->stmttype=0;
	((oracle8cursor *)cursor)->row=0;
	((oracle8cursor *)cursor)->maxrow=0;
	((oracle8cursor *)cursor)->totalrows=0;
	return true;
}

bool oracle8cursor::getLobOutputBindLength(uint16_t index, uint64_t *length) {
	ub4	loblength=0;
	bool	retval=(OCILobGetLength(oracle8conn->svc,
				oracle8conn->err,
				outbind_lob[index],
				&loblength)==OCI_SUCCESS);
	*length=loblength;
	return retval;
}

bool oracle8cursor::getLobOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {

	// initialize the read length to the number of characters to read
	ub4	readlength=charstoread;

	// read from the lob
	// (offset+1 is very important,
	// apparently oracle lob offsets are 1-based)
	sword	result=OCILobRead(oracle8conn->svc,
				oracle8conn->err,
				outbind_lob[index],
				&readlength,
				offset+1,
				(dvoid *)buffer,
				buffersize,
				NULL,
				(sb4(*)(dvoid *,CONST dvoid *,ub4,ub1))NULL,
				0,
				SQLCS_IMPLICIT);

	// readlength will have been set the number of chars that were read
	// set that on the way out
	*charsread=readlength;

	// return sucess or failure
	return (result!=OCI_INVALID_HANDLE);
}

void oracle8cursor::checkForTempTable(const char *query, uint32_t length) {

	char	*ptr=(char *)query;
	char	*endptr=(char *)query+length;

	// skip any leading whitespace and comments
	ptr=skipWhitespaceAndComments(query);
	if (!(*ptr)) {
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
	while (ptr && *ptr && *ptr!=' ' &&
		*ptr!='\n' && *ptr!='	' && ptr<endptr) {
		tablename.append(*ptr);
		ptr++;
	}

	if (oracle8conn->droptemptables) {
		// if "droptemptables" was specified...
		conn->addSessionTempTableForDrop(tablename.getString());
	} else if (!preserverows.match(ptr)) {
		// if "on commit preserve rows" was not specified...
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

				// set the NULL indicators to false
				rawbuffer::zero(def_indp[i],
						sizeof(sb2)*
						oracle8conn->fetchatonce);

				// allocate a lob descriptor
				for (uint32_t j=0;
					j<oracle8conn->fetchatonce;
					j++) {
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
					(sb4)oracle8conn->maxitembuffersize,
					SQLT_STR,
					(dvoid *)def_indp[i],
					(ub2 *)def_col_retlen[i],
					def_col_retcode[i],
					OCI_DEFAULT)!=OCI_SUCCESS) {
					return false;
				}

				// set the lob member to NULL
				for (uint32_t j=0;
					j<oracle8conn->fetchatonce;
					j++) {
					def_lob[i][j]=NULL;
				}
			}
		}
	}

	// convert integer output binds
	for (uint16_t i=0; i<outbindcount; i++) {
		if (outintbindstring[i]) {
			*outintbind[i]=charstring::
					toInteger(outintbindstring[i]);
		}
	}

	return true;
}

bool oracle8cursor::queryIsNotSelect() {
	return (stmttype!=OCI_STMT_SELECT);
}

const char *oracle8cursor::errorMessage(bool *liveconnection) {

	// get the message from oracle
	text	message[1024];
	rawbuffer::zero((void *)message,sizeof(message));
	sb4	errcode=0;
	OCIErrorGet((dvoid *)oracle8conn->err,1,
			(text *)0,&errcode,
			message,sizeof(message),
			OCI_HTYPE_ERROR);
	message[1023]='\0';

	// check for dead connection or shutdown in progress
	// Might need: 1033 - oracle init/shutdown in progress
	switch (errcode) {
		case 22: // invalid session ID; access denied
		case 28: // your session has been killed
		case 604: // error occurred at recursive SQL level ...
		case 1012: // not logged on
		case 1041: // internal error. hostdef extension doesn't exist
		case 1089: // immediate shutdown in progress -
				// no operations are permitted
		case 3114: // not connected to ORACLE
		case 3113: // end-of-file on communication channel
		case 3135: // connection lost contact
			*liveconnection=false;
			break;
		default:
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

bool oracle8cursor::knowsRowCount() {
	return false;
}

uint64_t oracle8cursor::rowCount() {
	return 0;
}

bool oracle8cursor::knowsAffectedRows() {
	return true;
}

uint64_t oracle8cursor::affectedRows() {

	// get the affected row count
	ub4	rows;
	if (OCIAttrGet(stmt,OCI_HTYPE_STMT,
			(dvoid *)&rows,(ub4 *)NULL,
			OCI_ATTR_ROW_COUNT,oracle8conn->err)==OCI_SUCCESS) {
		return rows;
	}
	return 0;
}

uint32_t oracle8cursor::colCount() {
	return ncols;
}

const char * const * oracle8cursor::columnNames() {
	for (sword i=0; i<ncols; i++) {
		columnnames[i]=(char *)desc[i].buf;
	}
	return columnnames;
}

uint16_t oracle8cursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_IDS;
}

void oracle8cursor::returnColumnInfo() {

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
	if (row==oracle8conn->fetchatonce) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return false;
	}
	if (!row) {
		OCIStmtFetch(stmt,oracle8conn->err,oracle8conn->fetchatonce,
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

void oracle8cursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {

	// handle NULLs
	if (def_indp[col][row]) {
		*null=true;
		return;
	}

	// handle blobs
	if (desc[col].dbtype==BLOB_TYPE ||
		desc[col].dbtype==CLOB_TYPE ||
		desc[col].dbtype==BFILE_TYPE) {
		*blob=true;
		return;
	}

	// handle normal datatypes
	*field=(const char *)&def_buf[col][row*oracle8conn->maxitembuffersize];
	*fieldlength=def_col_retlen[col][row];
}

void oracle8cursor::nextRow() {
	row++;
}

bool oracle8cursor::getLobFieldLength(uint32_t col, uint64_t *length) {
	ub4	loblength=0;
	bool	retval=(OCILobGetLength(oracle8conn->svc,
				oracle8conn->err,
				def_lob[col][row],
				&loblength)==OCI_SUCCESS);
	*length=loblength;
	return retval;
}

bool oracle8cursor::getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {

	// initialize the read length to the number of characters to read
	ub4	readlength=charstoread;

	// read from the lob
	// (offset+1 is very important,
	// apparently oracle lob offsets are 1-based)
	sword	result=OCILobRead(oracle8conn->svc,
				oracle8conn->err,
				def_lob[col][row],
				&readlength,
				offset+1,
				(dvoid *)buffer,
				buffersize,
				NULL,
				(sb4(*)(dvoid *,CONST dvoid *,ub4,ub1))NULL,
				0,
				SQLCS_IMPLICIT);

	// readlength will have been set the number of chars that were read
	// set that on the way out
	*charsread=readlength;

	// return sucess or failure
	return (result!=OCI_INVALID_HANDLE);
}

void oracle8cursor::cleanUpLobField(uint32_t col) {
#ifdef HAVE_ORACLE_8i
	// if the lob is temporary, deallocate it
	boolean	templob;
	if (OCILobIsTemporary(oracle8conn->env,
				oracle8conn->err,
				def_lob[col][row],
				&templob)!=OCI_SUCCESS) {
		return;
	}
	if (templob) {
		OCILobFreeTemporary(oracle8conn->svc,
					oracle8conn->err,
					def_lob[col][row]);
	}
#endif
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
		for (ub4 i=0; i<oracle8conn->maxselectlistsize; i++) {
			for (uint32_t j=0; j<oracle8conn->fetchatonce; j++) {
				if (def_lob[i][j]) {
					OCIDescriptorFree(def_lob[i][j],
								OCI_DTYPE_LOB);
					def_lob[i][j]=NULL;
				}
			}

			// Members of the def[] array should not be freed
			// here using OCIHandleFree as def[] array are just
			// pointers to structures allocated and managed by
			// OCI.  There was once code here that did deallocate
			// using OCIHandleFree, but it led to crashes.  Memory
			// leak detectors may complain, but ultimately the
			// memory will be deallocated.
			def[i]=NULL;
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
		for (uint16_t i=0; i<inbindcount; i++) {
			delete[] inintbindstring[i];
			inintbindstring[i]=NULL;
		}
		for (uint16_t i=0; i<outbindcount; i++) {
			delete[] outintbindstring[i];
			outintbindstring[i]=NULL;
			outintbind[i]=NULL;
		}
		inbindcount=0;
		outbindcount=0;
		curbindcount=0;
	}
}
