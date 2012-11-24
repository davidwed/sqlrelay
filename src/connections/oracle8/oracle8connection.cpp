// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <oracle8connection.h>
#include <oracle8sqlwriter.h>
#include <parsedatetime.h>
#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/character.h>
#include <rudiments/environment.h>

#include <datatypes.h>

#define MAX_BYTES_PER_CHAR	4

#ifdef OCI_STMT_CACHE
	// 9i calls OCI_STRLS_CACHE_DELETE something else
	#ifndef OCI_STRLS_CACHE_DELETE
		#define OCI_STRLS_CACHE_DELETE OCI_STMTCACHE_DELETE;
	#endif
#endif

oracle8connection::oracle8connection(sqlrcontroller_svr *cont) :
						sqlrconnection_svr(cont) {
	stmtmode=OCI_DEFAULT;

	env=NULL;
	srv=NULL;
	err=NULL;
	svc=NULL;
	session=NULL;
	trans=NULL;

#ifdef OCI_ATTR_PROXY_CREDENTIALS
	newsession=NULL;
	supportsproxycredentials=false;
#endif
	supportssyscontext=false;
	requiresreprepare=false;

	home=NULL;
	sid=NULL;
	nlslang=NULL;

	lastinsertidquery=NULL;

	fetchatonce=FETCH_AT_ONCE;
	maxselectlistsize=MAX_SELECT_LIST_SIZE;
	maxitembuffersize=MAX_ITEM_BUFFER_SIZE;
#ifdef OCI_STMT_CACHE
	stmtcachesize=STMT_CACHE_SIZE;
#endif
#ifdef HAVE_ORACLE_8i
	droptemptables=false;
#endif
	rejectduplicatebinds=false;
}

oracle8connection::~oracle8connection() {
	delete[] lastinsertidquery;
}

void oracle8connection::handleConnectString() {
	cont->setUser(cont->connectStringValue("user"));
	cont->setPassword(cont->connectStringValue("password"));
	sid=cont->connectStringValue("oracle_sid");
	home=cont->connectStringValue("oracle_home");
	nlslang=cont->connectStringValue("nls_lang");
	const char	*autocom=cont->connectStringValue("autocommit");
	cont->setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));

	fetchatonce=charstring::toUnsignedInteger(
				cont->connectStringValue("fetchatonce"));
	if (!fetchatonce) {
		fetchatonce=FETCH_AT_ONCE;
	}

	maxselectlistsize=charstring::toInteger(
				cont->connectStringValue("maxselectlistsize"));
	if (!maxselectlistsize) {
		maxselectlistsize=MAX_SELECT_LIST_SIZE;
	}

	maxitembuffersize=charstring::toInteger(
				cont->connectStringValue("maxitembuffersize"));
	if (!maxitembuffersize) {
		maxitembuffersize=MAX_ITEM_BUFFER_SIZE;
	}
	if (maxitembuffersize<MAX_BYTES_PER_CHAR) {
		maxitembuffersize=MAX_BYTES_PER_CHAR;
	}

#ifdef OCI_STMT_CACHE
	stmtcachesize=charstring::toUnsignedInteger(
				cont->connectStringValue("stmtcachesize"));
	if (!stmtcachesize) {
		stmtcachesize=STMT_CACHE_SIZE;
	}
#endif

	cont->setFakeTransactionBlocksBehavior(
		!charstring::compare(
			cont->connectStringValue("faketransactionblocks"),
			"yes"));

#ifdef HAVE_ORACLE_8i
	droptemptables=!charstring::compare(
			cont->connectStringValue("droptemptables"),"yes");
#endif

	rejectduplicatebinds=!charstring::compare(
			cont->connectStringValue("rejectduplicatebinds"),
			"yes");

	const char	*lastinsertidfunc=
			cont->connectStringValue("lastinsertidfunction");
	if (lastinsertidfunc) {
		stringbuffer	liiquery;
		liiquery.append("select ");
		liiquery.append(lastinsertidfunc);
		liiquery.append(" from dual");
		lastinsertidquery=liiquery.detachString();
	}

	cont->fakeinputbinds=
		!charstring::compare(
			cont->connectStringValue("fakebinds"),
			"yes");
}

#ifdef HAVE_ORACLE_8i
bool oracle8connection::tempTableDropReLogIn() {
	// When dropping temporary tables, if any of those tables were created
	// with "on commit preserve rows" then the session has to exit before
	// the table can be dropped or oracle will return the following error:
	// ORA-14452: attempt to create, alter or drop an index on temporary
	// table already in use
	// It's not really clear why, but that's the case.
	return true;
}
#endif

bool oracle8connection::logIn(bool printerrors) {

	// get user/password
	const char	*user=cont->getUser();
	const char	*password=cont->getPassword();

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

	// handle error reading tnsnames.ora
	if (!sidtnsnameformat) {
		if (!charstring::length(home)) {
			home=environment::getValue("ORACLE_HOME");
		}
		char	*tnsnamesora=new char[charstring::length(home)+28];
		charstring::copy(tnsnamesora,home);
		charstring::append(tnsnamesora,"/network/admin/tnsnames.ora");
		if (!file::readable(tnsnamesora)) {
			fprintf(stderr,"Warning: %s/tnsnames.ora is not readable by %s:%s\n",home,cont->cfgfl->getRunAsUser(),cont->cfgfl->getRunAsGroup());
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

	// decide what credentials to use
	sword	cred=OCI_CRED_RDBMS;
	if (!charstring::length(user) && !charstring::length(password)) {
		cred=OCI_CRED_EXT;
	}

	// use statement caching if available
	ub4	mode=OCI_DEFAULT;
#ifdef OCI_STMT_CACHE
	if (stmtcachesize) {
		mode=OCI_STMT_CACHE;
	}
#endif

	// begin the session
	if (OCISessionBegin(svc,err,session,cred,mode)!=OCI_SUCCESS) {
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

#ifdef OCI_STMT_CACHE
	// set the statement cache size
	if (OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)&stmtcachesize,(ub4)0,
				(ub4)OCI_ATTR_STMTCACHESIZE,
				(OCIError *)err)!=OCI_SUCCESS) {
		logInError("Set statement cache size failed.\n");
		OCISessionEnd(svc,err,session,OCI_DEFAULT);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}
	if (cont->dbgfile.debugEnabled()) {
		if (OCIAttrGet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)&stmtcachesize,(ub4)0,
				(ub4)OCI_ATTR_STMTCACHESIZE,
				(OCIError *)err)==OCI_SUCCESS) {
			stringbuffer	debugstr;
			debugstr.append("cache size ");
			debugstr.append(stmtcachesize);
			cont->dbgfile.debugPrint("connection",1,
						debugstr.getString());
		}
	}
#endif

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
		if (printerrors) {
			logInError("OCIAttrSet(OCI_ATTR_TRANS) failed.\n");
		}
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

	// figure out what version of the database we're connected to...
	#ifdef OCI_ATTR_PROXY_CREDENTIALS
	supportsproxycredentials=false;
	#endif
	supportssyscontext=false;
	requiresreprepare=false;
	if (OCIServerVersion((dvoid *)svc,err,
				(text *)versionbuf,sizeof(versionbuf),
				OCI_HTYPE_SVCCTX)==OCI_SUCCESS) {

		// get the major and minor versions
		const char	*release=
				charstring::findFirst(versionbuf,"Release ");
		const char	*majorstr=NULL;
		const char	*minorstr=NULL;
		if (release) {
			majorstr=release+8;
			minorstr=charstring::findFirst(majorstr,".");
			if (minorstr) {
				minorstr=minorstr+1;
			}
		}
		int64_t	major=charstring::toInteger(majorstr);
		int64_t	minor=charstring::toInteger(minorstr);
	
		// 8.1 and up supports proxy credentials and syscontext
		if (major>=8 || (major==8 && minor>0)) {
			#ifdef OCI_ATTR_PROXY_CREDENTIALS
			supportsproxycredentials=true;
			#endif
			supportssyscontext=true;
		}

		// anything below 9 requires reprepare
		if (major<9) {
			requiresreprepare=true;
		}
	}

	// reprepare is required when using OCI 8 (not 8i or higher)
#ifndef HAVE_ORACLE_8i
	requiresreprepare=true;
#endif

#ifdef OCI_STMT_CACHE
	if (stmtcachesize) {

		// disable cursor sharing when statement caching is used...

		OCIStmt	*stmt=NULL;

		const char	*alter="alter session set cursor_sharing=exact";
		if (OCIStmtPrepare2(svc,&stmt,err,
				(text *)alter,charstring::length(alter),
				NULL,0,
				(ub4)OCI_NTV_SYNTAX,
				(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
			if (printerrors) {
				logInError("Prepare alter session failed.\n");
			}
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

		if (OCIStmtExecute(svc,stmt,err,1,(ub4)0,
				NULL,NULL,stmtmode)!=OCI_SUCCESS) {
			if (printerrors) {
				logInError("Execute alter session failed.\n");
			}
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

		if (OCIStmtRelease(stmt,err,NULL,0,
				OCI_STRLS_CACHE_DELETE)!=OCI_SUCCESS) {
			if (printerrors) {
				logInError("Statement release failed.\n");
			}
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

	// use statement caching if available
	ub4	mode=OCI_DEFAULT;
#ifdef OCI_STMT_CACHE
	if (stmtcachesize) {
		mode=OCI_STMT_CACHE;
	}
#endif

	// start the session
	if (OCISessionBegin(svc,err,newsession,
				OCI_CRED_PROXY,mode)!=OCI_SUCCESS) {
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
	stmtmode=OCI_COMMIT_ON_SUCCESS;
	return true;
}

bool oracle8connection::autoCommitOff() {
	stmtmode=OCI_DEFAULT;
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

void oracle8connection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {

	// get the message from oracle
	rawbuffer::zero(errorbuffer,errorbufferlength);
	sb4	errcode=0;
	OCIErrorGet((dvoid *)err,1,(text *)0,&errcode,
			(text *)errorbuffer,errorbufferlength,OCI_HTYPE_ERROR);
	errorbuffer[errorbufferlength-1]='\0';

	// truncate the trailing \n
	*errorlength=charstring::length((char *)errorbuffer);
	char	*last=errorbuffer+(*errorlength)-1;
	if (*last=='\n') {
		*last='\0';
	}

	// set return values
	*errorcode=errcode;

	// check for dead connection or shutdown in progress
	switch (errcode) {
		case 22: // invalid session ID; access denied
		case 28: // your session has been killed
		case 604: // error occurred at recursive SQL level ...
		case 1012: // not logged on
		case 1033: // oracle init/shutdown in progress
		case 1041: // internal error. hostdef extension doesn't exist
		case 1089: // immediate shutdown in progress -
				// no operations are permitted
		case 2067: // transaction or savepoint rollback required
		case 3114: // not connected to ORACLE
		case 3113: // end-of-file on communication channel
		case 3135: // connection lost contact
		case 14452: // attempt to create, alter or drop an index on
				// temporary table already in use
				// (see note below)
			*liveconnection=false;
			break;
		default:
			*liveconnection=true;
			break;
	}

	// Note:
	// When dropping temporary tables, if any of those tables were created
	// with "on commit preserve rows" then the session has to exit before
	// the table can be dropped or oracle will return the following error:
	// ORA-14452: attempt to create, alter or drop an index on temporary
	// table already in use
	// It's not really clear why, but that's the case.  If we encounter
	// this error, then we'll declare the db down.  SQL Relay will then
	// relogin and reexecute the query when it comes back up.
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
	if (supportssyscontext) {
		return (wild)?
			"select "
			"	table_name "
			"from "
			"	all_tables "
			"where "
			"	table_name like upper('%s') "
			"	and "
			"	owner=sys_context('userenv','current_schema') "
			"order by "
			"	table_name":

			"select "
			"	table_name "
			"from "
			"	all_tables "
			"where "
			"	owner=sys_context('userenv','current_schema') "
			"order by "
			"	table_name";
	} else {
		return (wild)?
			"select "
			"	table_name "
			"from "
			"	user_tables "
			"where "
			"	table_name like upper('%s') "
			"order by "
			"	table_name":

			"select "
			"	table_name "
			"from "
			"	user_tables "
			"order by "
			"	table_name";
	}
}

const char *oracle8connection::getColumnListQuery(bool wild) {
	if (supportssyscontext) {
		return (wild)?
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
			"	and "
			"	column_name like upper('%s') "
			"	and "
			"	(owner=sys_context('userenv','current_schema') "
			"	or "
			"	owner='SYS' "
			"	or "
			"	owner='SYSTEM') "
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
			"	and "
			"	(owner=sys_context('userenv','current_schema') "
			"	or "
			"	owner='SYS' "
			"	or "
			"	owner='SYSTEM') "
			"order by "
			"	column_id";
	} else {
		return (wild)?
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
			"	user_tab_columns "
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
			"	user_tab_columns "
			"where "
			"	table_name=upper('%s') "
			"order by "
			"	column_id";
	}
}

const char *oracle8connection::selectDatabaseQuery() {
	return "alter session set current_schema=%s";
}

const char *oracle8connection::getCurrentDatabaseQuery() {
	return "select sys_context('userenv','current_schema') from dual";
}

const char *oracle8connection::getLastInsertIdQuery() {
	return lastinsertidquery;
}

oracle8cursor::oracle8cursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {

	stmt=NULL;
	stmttype=0;
#ifdef OCI_STMT_CACHE
	stmtreleasemode=OCI_DEFAULT;
#endif
	ncols=0;

	oracle8conn=(oracle8connection *)conn;
	allocateResultSetBuffers(oracle8conn->fetchatonce,
					oracle8conn->maxselectlistsize,
					oracle8conn->maxitembuffersize);

	inbindpp=new OCIBind *[conn->cont->maxbindcount];
	outbindpp=new OCIBind *[conn->cont->maxbindcount];
	curbindpp=new OCIBind *[conn->cont->maxbindcount];
	inintbindstring=new char *[conn->cont->maxbindcount];
	indatebind=new OCIDate *[conn->cont->maxbindcount];
	outintbindstring=new char *[conn->cont->maxbindcount];
	outdatebind=new datebind *[conn->cont->maxbindcount];
	outintbind=new int64_t *[conn->cont->maxbindcount];
	bindvarname=new const char *[conn->cont->maxbindcount];
	boundbypos=new bool[conn->cont->maxbindcount];
	bvnp=new text *[conn->cont->maxbindcount];
	invp=new text *[conn->cont->maxbindcount];
	inpl=new ub1[conn->cont->maxbindcount];
	dupl=new ub1[conn->cont->maxbindcount];
	bvnl=new ub1[conn->cont->maxbindcount];
	hndl=new OCIBind *[conn->cont->maxbindcount];
	for (uint16_t i=0; i<conn->cont->maxbindcount; i++) {
		inbindpp[i]=NULL;
		outbindpp[i]=NULL;
		curbindpp[i]=NULL;
		inintbindstring[i]=NULL;
		indatebind[i]=NULL;
		outintbindstring[i]=NULL;
		outdatebind[i]=NULL;
		outintbind[i]=NULL;
		bindvarname[i]=NULL;
		boundbypos[i]=false;
	}
	orainbindcount=0;
	oraoutbindcount=0;
	oracurbindcount=0;
	bindvarcount=0;

#ifdef HAVE_ORACLE_8i
	inbind_lob=new OCILobLocator *[conn->cont->maxbindcount];
	outbind_lob=new OCILobLocator *[conn->cont->maxbindcount];
	for (uint16_t i=0; i<conn->cont->maxbindcount; i++) {
		inbind_lob[i]=NULL;
		outbind_lob[i]=NULL;
	}
	orainbindlobcount=0;
	oraoutbindlobcount=0;
#endif

	row=0;
	maxrow=0;
	totalrows=0;

	query=NULL;
	length=0;
	prepared=false;
	bound=false;

	resultfreed=true;

#ifdef HAVE_ORACLE_8i
	createtemp.compile("(create|CREATE)[ \\t\\n\\r]+(global|GLOBAL)[ \\t\\n\\r]+(temporary|TEMPORARY)[ \\t\\n\\r]+(table|TABLE)[ \\t\\n\\r]+");
	createtemp.study();
	deleterows.compile("(on|ON)[ \\t\\n\\r]+(commit|COMMIT)[ \\t\\n\\r]+(delete|DELETE)[ \\t\\n\\r]+(rows|ROWS)");
	deleterows.study();
	preserverows.compile("(on|ON)[ \\t\\n\\r]+(commit|COMMIT)[ \\t\\n\\r]+(preserve|PRESERVE)[ \\t\\n\\r]+(rows|ROWS)");
	preserverows.study();
#endif
}

oracle8cursor::~oracle8cursor() {

	for (uint16_t i=0; i<orainbindcount; i++) {
		delete[] inintbindstring[i];
		delete indatebind[i];
	}
	for (uint16_t i=0; i<oraoutbindcount; i++) {
		delete[] outintbindstring[i];
		if (outdatebind[i]) {
			delete outdatebind[i]->ocidate;
		}
		delete outdatebind[i];
	}

	delete[] inbindpp;
	delete[] outbindpp;
	delete[] curbindpp;
	delete[] inintbindstring;
	delete[] indatebind;
	delete[] outintbindstring;
	delete[] outdatebind;
	delete[] outintbind;
	delete[] bindvarname;
	delete[] boundbypos;
	delete[] bvnp;
	delete[] invp;
	delete[] inpl;
	delete[] dupl;
	delete[] bvnl;
	delete[] hndl;

#ifdef HAVE_ORACLE_8i
	delete[] inbind_lob;
	delete[] outbind_lob;
#endif

	deallocateResultSetBuffers();
}

void oracle8cursor::allocateResultSetBuffers(uint32_t fetchatonce,
						int32_t selectlistsize,
						int32_t itembuffersize) {

	if (selectlistsize==-1) {
		resultsetbuffercount=0;
		desc=NULL;
		columnnames=NULL;
		def=NULL;
		def_lob=NULL;
		def_buf=NULL;
		def_indp=NULL;
		def_col_retlen=NULL;
		def_col_retcode=NULL;
	} else {
		resultsetbuffercount=selectlistsize;
		desc=new describe[resultsetbuffercount];
		columnnames=new char *[resultsetbuffercount];
		def=new OCIDefine *[resultsetbuffercount];
		def_lob=new OCILobLocator **[resultsetbuffercount];
		def_buf=new ub1 *[resultsetbuffercount];
		def_indp=new sb2 *[resultsetbuffercount];
		def_col_retlen=new ub2 *[resultsetbuffercount];
		def_col_retcode=new ub2 *[resultsetbuffercount];
		for (int32_t i=0; i<resultsetbuffercount; i++) {
			def_lob[i]=new OCILobLocator *[fetchatonce];
			for (uint32_t j=0; j<fetchatonce; j++) {
				def_lob[i][j]=NULL;
			}
			def_buf[i]=new ub1[fetchatonce*itembuffersize];
			def_indp[i]=new sb2[fetchatonce];
			def_col_retlen[i]=new ub2[fetchatonce];
			def_col_retcode[i]=new ub2[fetchatonce];
			def[i]=NULL;
			desc[i].paramd=NULL;
		}
	}
}

void oracle8cursor::deallocateResultSetBuffers() {
	if (resultsetbuffercount) {
		for (int32_t i=0; i<resultsetbuffercount; i++) {
			delete[] def_col_retcode[i];
			delete[] def_col_retlen[i];
			delete[] def_indp[i];
			delete[] def_lob[i];
			delete[] def_buf[i];
		}
		delete[] def_col_retcode;
		delete[] def_col_retlen;
		delete[] def_indp;
		delete[] def_lob;
		delete[] def_buf;
		delete[] def;
		delete[] desc;
		delete[] columnnames;
		resultsetbuffercount=0;
	}
}

bool oracle8cursor::open(uint16_t id) {

	stmt=NULL;

#ifdef OCI_STMT_CACHE
	// If statement caching is available then we don't need to allocate
	// a cursor handle here, as it will be allocated by the call to
	// OCIStmtPrepare2 later.
	//
	// If statement caching isn't available then we need to allocate a
	// cursor handle here and set the number of rows to prefetch.
	stmtreleasemode=OCI_DEFAULT;
	if (oracle8conn->stmtcachesize) {
		return true;
	}
#endif

	// allocate a cursor handle
	if (OCIHandleAlloc((dvoid *)oracle8conn->env,
				(dvoid **)&stmt,
				OCI_HTYPE_STMT,(size_t)0,
				(dvoid **)0)!=OCI_SUCCESS) {
		return false;
	}

	// set the number of rows to prefetch
	return (OCIAttrSet((dvoid *)stmt,OCI_HTYPE_STMT,
				(dvoid *)&(oracle8conn->fetchatonce),
				(ub4)0,OCI_ATTR_PREFETCH_ROWS,
				(OCIError *)oracle8conn->err)==OCI_SUCCESS);
}

bool oracle8cursor::close() {

	cleanUpData(true,true);

#ifdef OCI_STMT_CACHE
	if (oracle8conn->stmtcachesize && stmt) {
		return OCIStmtRelease(stmt,oracle8conn->err,
				NULL,0,OCI_STRLS_CACHE_DELETE)==OCI_SUCCESS;
	}
#endif

	return (OCIHandleFree(stmt,OCI_HTYPE_STMT)==OCI_SUCCESS);
}

bool oracle8cursor::prepareQuery(const char *query, uint32_t length) {

	// keep a pointer to the query and length in case it needs to be 
	// reprepared later
	this->query=(char *)query;
	this->length=length;

	// if the query is being prepared then apparently this isn't an
	// output bind cursor
	bound=false;

	// If statement caching is available then use OCIStmtPrepare2,
	// otherwise just use OCIStmtPrepare.

#ifdef OCI_STMT_CACHE
	if (oracle8conn->stmtcachesize) {

		// release any prior-allocated statement...
		if (stmt) {

			// delete DML statements from the cache
			if (stmttype==OCI_STMT_DROP ||
					stmttype==OCI_STMT_CREATE ||
					stmttype==OCI_STMT_ALTER) {
				stmtreleasemode=OCI_STRLS_CACHE_DELETE;
			}

			if (OCIStmtRelease(stmt,oracle8conn->err,
					NULL,0,stmtreleasemode)!=OCI_SUCCESS) {
				return false;
			}

			stmt=NULL;
			stmtreleasemode=OCI_DEFAULT;
		}

		// reset the statement type
		stmttype=0;

		// prepare the query...
		bool	prepare=true;
		if (oracle8conn->cont->dbgfile.debugEnabled()) {
			// check for a statment cache hit
			// and report our findings
			if (OCIStmtPrepare2(oracle8conn->svc,&stmt,
					oracle8conn->err,
					(text *)query,(ub4)length,
					NULL,0,
					(ub4)OCI_NTV_SYNTAX,
					(ub4)OCI_PREP2_CACHE_SEARCHONLY)==
					OCI_SUCCESS) {
				// we got a hit and don't
				// need to do anything else
				oracle8conn->cont->dbgfile.debugPrint(
							"connection",1,
							"statement cache hit");
				prepare=false;
			} else {
				// we didn't get a hit and
				// need to prepare the query
				oracle8conn->cont->dbgfile.debugPrint(
							"connection",1,
							"statement cache miss");
			}
		}
		if (prepare) {
			// prepare the query
			if (OCIStmtPrepare2(oracle8conn->svc,&stmt,
					oracle8conn->err,
					(text *)query,(ub4)length,
					NULL,0,
					(ub4)OCI_NTV_SYNTAX,
					(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
				return false;
			}
		}

		// set the number of rows to prefetch
		return (OCIAttrSet((dvoid *)stmt,OCI_HTYPE_STMT,
				(dvoid *)&(oracle8conn->fetchatonce),(ub4)0,
				OCI_ATTR_PREFETCH_ROWS,
				(OCIError *)oracle8conn->err)==OCI_SUCCESS);

	}
#endif

	// reset the statement type
	stmttype=0;

	// prepare the query
	return (OCIStmtPrepare(stmt,oracle8conn->err,
				(text *)query,(ub4)length,
				(ub4)OCI_NTV_SYNTAX,
				(ub4)OCI_DEFAULT)==OCI_SUCCESS);
}

void oracle8cursor::checkRePrepare() {

	// I once thought that it was necessary to re-prepare every time we
	// re-execute.  It turns out that actually, when using OCI lower than
	// 8i or against DB's lower than 9i, if we bind one type (say, a date),
	// execute, then re-bind a different type (say, a string representation
	// of a date), then re-execute, then it will throw ORA-01475.  Keeping
	// track of the type of each bind variable and deciding whether to
	// reprepare or not is a bit of work.  Maybe I'll do that at some point.
	// For now, with those versions, we'll reprepare if we rebind at all.

	if (oracle8conn->requiresreprepare && !prepared &&
			stmttype && stmttype!=OCI_STMT_SELECT) {
		cleanUpData(true,true);
		prepareQuery(query,length);
		prepared=true;
	}
}

void oracle8cursor::dateToString(char *buffer, uint16_t buffersize,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t microsecond, const char *tz) {
	// typically oracle just wants DD-MON-YYYY but if hour,
	// minute and second are non-zero then use them too
	if (hour && minute && second) {
		snprintf(buffer,buffersize,"%02d-%s-%04d %02d:%02d:%02d",
				day,shortmonths[month-1],year,
				hour,minute,second);
	} else {
		snprintf(buffer,buffersize,"%02d-%s-%04d",
				day,shortmonths[month-1],year);
	}
}

bool oracle8cursor::inputBind(const char *variable,
				uint16_t variablesize,
				const char *value,
				uint32_t valuesize,
				int16_t *isnull) {
	checkRePrepare();

	// the size of the value must include the terminating NULL
	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracle8conn->err,pos,
				(dvoid *)value,(sb4)valuesize+1,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&inbindpp[orainbindcount],
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
	orainbindcount++;
	bindvarname[bindvarcount++]=variable+1;
	return true;
}


bool oracle8cursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value) {
	checkRePrepare();

	inintbindstring[orainbindcount]=charstring::parseNumber(*value);

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracle8conn->err,pos,
				(dvoid *)inintbindstring[orainbindcount],
				(sb4)charstring::length(
					inintbindstring[orainbindcount])+1,
				SQLT_STR,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&inbindpp[orainbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)inintbindstring[orainbindcount],
				(sb4)charstring::length(
					inintbindstring[orainbindcount])+1,
				SQLT_STR,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	orainbindcount++;
	bindvarname[bindvarcount++]=variable+1;
	return true;
}


bool oracle8cursor::inputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {
	checkRePrepare();

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracle8conn->err,pos,
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&inbindpp[orainbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	orainbindcount++;
	bindvarname[bindvarcount++]=variable+1;
	return true;
}


bool oracle8cursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t year,
				int16_t month,
				int16_t day,
				int16_t hour,
				int16_t minute,
				int16_t second,
				int32_t microsecond,
				const char *tz,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {
	checkRePrepare();

	indatebind[orainbindcount]=new OCIDate;
	OCIDateSetDate(indatebind[orainbindcount],year,month,day);
	OCIDateSetTime(indatebind[orainbindcount],hour,minute,second);

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracle8conn->err,pos,
				(dvoid *)indatebind[orainbindcount],
				(sb4)sizeof(OCIDate),
				SQLT_ODT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&inbindpp[orainbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)indatebind[orainbindcount],
				(sb4)sizeof(OCIDate),
				SQLT_ODT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	orainbindcount++;
	bindvarname[bindvarcount++]=variable+1;
	return true;
}


bool oracle8cursor::outputBind(const char *variable,
				uint16_t variablesize,
				char *value,
				uint16_t valuesize,
				int16_t *isnull) {
	checkRePrepare();

	outintbindstring[oraoutbindcount]=NULL;
	outdatebind[oraoutbindcount]=NULL;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
				oracle8conn->err,pos,
				(dvoid *)value,
				(sb4)valuesize,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&outbindpp[oraoutbindcount],
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
	oraoutbindcount++;
	bindvarname[bindvarcount++]=variable+1;
	return true;
}

bool oracle8cursor::outputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {
	checkRePrepare();

	outintbindstring[oraoutbindcount]=new char[21];
	rawbuffer::zero(outintbindstring[oraoutbindcount],21);
	outintbind[oraoutbindcount]=value;
	outdatebind[oraoutbindcount]=NULL;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
				oracle8conn->err,pos,
				(dvoid *)outintbindstring[oraoutbindcount],
				(sb4)21,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&outbindpp[oraoutbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)outintbindstring[oraoutbindcount],
				(sb4)21,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	oraoutbindcount++;
	bindvarname[bindvarcount++]=variable+1;
	return true;
}

bool oracle8cursor::outputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t *precision,
				uint32_t *scale,
				int16_t *isnull) {
	checkRePrepare();

	outintbindstring[oraoutbindcount]=NULL;
	outdatebind[oraoutbindcount]=NULL;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
				oracle8conn->err,pos,
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&outbindpp[oraoutbindcount],
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
	oraoutbindcount++;
	bindvarname[bindvarcount++]=variable+1;
	return true;
}

bool oracle8cursor::outputBind(const char *variable,
				uint16_t variablesize,
				int16_t *year,
				int16_t *month,
				int16_t *day,
				int16_t *hour,
				int16_t *minute,
				int16_t *second,
				int32_t *microsecond,
				const char **tz,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {
	checkRePrepare();

	outintbindstring[oraoutbindcount]=NULL;
	datebind	*db=new datebind;
	db->year=year;
	db->month=month;
	db->day=day;
	db->hour=hour;
	db->minute=minute;
	db->second=second;
	db->tz=tz;
	db->ocidate=new OCIDate;
	outdatebind[oraoutbindcount]=db;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
				oracle8conn->err,pos,
				(dvoid *)db->ocidate,
				(sb4)sizeof(OCIDate),
				SQLT_ODT,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&outbindpp[oraoutbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)db->ocidate,
				(sb4)sizeof(OCIDate),
				SQLT_ODT,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	oraoutbindcount++;
	bindvarname[bindvarcount++]=variable+1;
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
			(dvoid **)&inbind_lob[orainbindlobcount],
			(ub4)OCI_DTYPE_LOB,
			(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		return false;
	}

	if (OCILobCreateTemporary(oracle8conn->svc,oracle8conn->err,
			inbind_lob[orainbindlobcount],
			//(ub2)0,SQLCS_IMPLICIT,
			(ub2)OCI_DEFAULT,OCI_DEFAULT,
			temptype,OCI_ATTR_NOCACHE,
			OCI_DURATION_SESSION)!=OCI_SUCCESS) {
		OCIDescriptorFree(inbind_lob[orainbindlobcount],OCI_DTYPE_LOB);
		return false;
	}

	if (OCILobOpen(oracle8conn->svc,oracle8conn->err,
			inbind_lob[orainbindlobcount],
			OCI_LOB_READWRITE)!=OCI_SUCCESS) {
		OCILobFreeTemporary(oracle8conn->svc,oracle8conn->err,
					inbind_lob[orainbindlobcount]);
		OCIDescriptorFree(inbind_lob[orainbindlobcount],OCI_DTYPE_LOB);
		return false;
	}

	ub4	size=valuesize;
	if (OCILobWrite(oracle8conn->svc,oracle8conn->err,
			inbind_lob[orainbindlobcount],&size,1,
			(void *)value,valuesize,
			OCI_ONE_PIECE,(dvoid *)0,
			(sb4 (*)(dvoid*,dvoid*,ub4*,ub1 *))0,
			0,SQLCS_IMPLICIT)!=OCI_SUCCESS) {

		OCILobClose(oracle8conn->svc,oracle8conn->err,
					inbind_lob[orainbindlobcount]);
		OCILobFreeTemporary(oracle8conn->svc,oracle8conn->err,
					inbind_lob[orainbindlobcount]);
		OCIDescriptorFree(inbind_lob[orainbindlobcount],OCI_DTYPE_LOB);
		return false;
	}

	// bind the temporary lob
	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracle8conn->err,pos,
				(dvoid *)&inbind_lob[orainbindlobcount],(sb4)0,
				type,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&inbindpp[orainbindcount],
				oracle8conn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)&inbind_lob[orainbindlobcount],(sb4)0,
				type,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
	}
	orainbindlobcount++;
	orainbindcount++;
	bindvarname[bindvarcount++]=variable+1;
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
	oraoutbindlobcount=index+1;

	// bind the lob descriptor
	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
			oracle8conn->err,pos,
			(dvoid *)&outbind_lob[index],
			(sb4)sizeof(OCILobLocator *),
			type,
			(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
			OCI_DEFAULT)!=OCI_SUCCESS) {
				return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&outbindpp[oraoutbindcount],
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
	oraoutbindcount++;
	bindvarname[bindvarcount++]=variable+1;
	return true;
}

bool oracle8cursor::outputBindCursor(const char *variable,
					uint16_t variablesize,
					sqlrcursor_svr *cursor) {

#ifdef OCI_STMT_CACHE
	// If the statement cache is in use then OCIStmtExecute will crash
	// if the query includes cursor binds.  I'm not sure if this is an OCI
	// bug or a problem caused by SQL Relay somehow, but until I discover
	// a solution, we'll return an error here.  Ideally, I'd set an error
	// message too...
	if (oracle8conn->stmtcachesize) {
		return false;
	}
#endif

	checkRePrepare();

	((oracle8cursor *)cursor)->bound=true;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			return false;
		}
		if (OCIBindByPos(stmt,&curbindpp[oracurbindcount],
				oracle8conn->err,pos,
				(dvoid *)&(((oracle8cursor *)cursor)->stmt),
				(sb4)0,
				SQLT_RSET,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&curbindpp[oracurbindcount],
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
	oracurbindcount++;
	bindvarname[bindvarcount++]=variable+1;

	// initialize values as if a statement has been prepared and executed
	((oracle8cursor *)cursor)->stmttype=0;
	((oracle8cursor *)cursor)->ncols=0;
	((oracle8cursor *)cursor)->row=0;
	((oracle8cursor *)cursor)->maxrow=0;
	((oracle8cursor *)cursor)->totalrows=0;
	((oracle8cursor *)cursor)->bound=true;
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
		conn->cont->addSessionTempTableForDrop(tablename.getString());
	} else if (preserverows.match(ptr)) {
		// If "on commit preserve rows" was specified, then when
		// the commit/rollback is executed at the end of the
		// session, the data won't be truncated.  It needs to
		// be though, so we'll set it up to be truncated manually.
		conn->cont->addSessionTempTableForTrunc(tablename.getString());
	}
}
#endif

bool oracle8cursor::executeQuery(const char *query, uint32_t length) {
	return executeQueryOrFetchFromBindCursor(query,length,true);
}

bool oracle8cursor::fetchFromBindCursor() {
	return executeQueryOrFetchFromBindCursor(NULL,0,false);
}

bool oracle8cursor::executeQueryOrFetchFromBindCursor(const char *query,
							uint32_t length,
							bool execute) {

	// initialize the row and column counters
	row=0;
	maxrow=0;
	totalrows=0;
	ncols=0;

	// get the type of the query (select, insert, update, etc...)
	if (OCIAttrGet(stmt,OCI_HTYPE_STMT,
			(dvoid *)&stmttype,(ub4 *)NULL,
			OCI_ATTR_STMT_TYPE,oracle8conn->err)!=OCI_SUCCESS) {
		return false;
	}

	// execute the query
	if (execute) {

#ifdef HAVE_ORACLE_8i
		// check for create temp table query
		if (stmttype==OCI_STMT_CREATE) {
			checkForTempTable(query,length);
		}
#endif

		// validate binds
		if (!validBinds()) {
			return false;
		}

		// loop to handle retries...
		// If the query fails with ora-04068 then we need to retry
		// the query.  Only retry once though.
		bool	first=true;
		for (;;) {
			if (OCIStmtExecute(oracle8conn->svc,stmt,
					oracle8conn->err,
					(stmttype==OCI_STMT_SELECT)?0:1,
					(ub4)0,NULL,NULL,
					oracle8conn->stmtmode)==OCI_SUCCESS) {
				break;
			}
			if (!first) {
				return false;
			}
			sb4	errcode=0;
			OCIErrorGet((dvoid *)oracle8conn->err,
					1,(text *)0,&errcode,
					NULL,0,OCI_HTYPE_ERROR);
			if (!first || errcode!=4068) {
				return false;
			}
			first=false;
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

		// validate column count
		if (oracle8conn->maxselectlistsize!=-1 &&
			ncols>oracle8conn->maxselectlistsize) {
			stringbuffer	err;
			err.append(SQLR_ERROR_MAXSELECTLIST_STRING);
			err.append(" (")->append(ncols)->append('>');
			err.append(oracle8conn->maxselectlistsize)->append(')');
			setError(err.getString(),
					SQLR_ERROR_MAXSELECTLIST,true);
			return false;
		}

		// allocate buffers, if necessary
		if (oracle8conn->maxselectlistsize==-1) {
			allocateResultSetBuffers(oracle8conn->fetchatonce,
					ncols,oracle8conn->maxitembuffersize);
		}

		// indicate that the result needs to be freed
		resultfreed=false;

		// run through the columns...
		for (sword i=0; i<ncols; i++) {

			// FIXME: neowiz bzero's desc[i] here
			//rawbuffer::zero(&desc[i],sizeof(describe));

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
	for (uint16_t i=0; i<oraoutbindcount; i++) {
		if (outintbindstring[i]) {
			*outintbind[i]=charstring::
					toInteger(outintbindstring[i]);
		}
	}

	// convert date output binds
	for (uint16_t i=0; i<oraoutbindcount; i++) {
		if (outdatebind[i]) {
			datebind	*db=outdatebind[i];
			sb2	year;
			ub1	month;
			ub1	day;
			ub1	hour;
			ub1	minute;
			ub1	second;
			OCIDateGetDate(db->ocidate,&year,&month,&day);
			OCIDateGetTime(db->ocidate,&hour,&minute,&second);
			*db->year=year;
			*db->month=month;
			*db->day=day;
			*db->hour=hour;
			*db->minute=minute;
			*db->second=second;
			*db->tz=NULL;
		}
	}

	return true;
}

bool oracle8cursor::validBinds() {

	// NOTE: If we're using the statement cache, then it is vital to
	// verify that all variables are bound.  Without the statement cache,
	// OCIStmtExecute would just fail with "ORA-01008: not all variables
	// bound", but when using the statement cache, it will segfault if we
	// get a cache hit.


	// if we're not using the statement cache and we're not rejecting
	// duplicate binds then we can just return
	bool	usingstmtcache=false;
	#ifdef OCI_STMT_CACHE
	if (oracle8conn->stmtcachesize) {
		usingstmtcache=true;
	}
	#endif
	if (!usingstmtcache && !oracle8conn->rejectduplicatebinds) {
		return true;
	}

	// otherwise we need to validate the binds...

	// get the bind info from the query
	sb4	found;
	sword	ret=OCIStmtGetBindInfo(stmt,oracle8conn->err,
					conn->cont->maxbindcount,
					1,&found,bvnp,bvnl,invp,inpl,dupl,hndl);

	// there were no bind variables
	if (ret==OCI_NO_DATA) {
		return true;
	}

	// error of some kind
	if (ret==OCI_ERROR) {
		return false;
	}

	// loop through the variables...
	for (sb4 i=0; i<found; i++) {

		// Using PL/SQL and binding by position with duplicate bind
		// variables, doesn't work correctly.  Detecting PL/SQL is
		// tricky so we'll just prevent duplicate bind names outright.
		if (oracle8conn->rejectduplicatebinds && dupl[i]) {
			stringbuffer	err;
			err.append(SQLR_ERROR_DUPLICATE_BINDNAME_STRING);
			err.append(" (")->append(bvnp[i])->append(')');
			setError(err.getString(),
					SQLR_ERROR_DUPLICATE_BINDNAME,true);
			return false;
		}

		// verify that the variable was bound,
		// first check by position, then by name
		if (usingstmtcache) {
			bool	foundvar=boundbypos[i];
			for (uint16_t j=0; j<bindvarcount && !foundvar; j++) {
				foundvar=!charstring::compareIgnoringCase(
							bindvarname[j],
							(char *)bvnp[i]);
			}
			if (!foundvar) {
				setError("ORA-01008: not all variables bound",
								1008,true);
				return false;
			}
		}
	}
	return true;
}

bool oracle8cursor::queryIsNotSelect() {
	return (stmttype!=OCI_STMT_SELECT);
}

void oracle8cursor::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {

	oracle8conn->errorMessage(errorbuffer,errorbufferlength,
					errorlength,errorcode,liveconnection);

#ifdef OCI_STMT_CACHE
	// set the statement release mode such that this query will be
	// removed from the statement cache on the next iteration
	if (charstring::length(errorbuffer)) {
		stmtreleasemode=OCI_STRLS_CACHE_DELETE;
	}
#endif
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

const char *oracle8cursor::getColumnName(uint32_t col) {
	return (const char *)desc[col].buf;
}

uint16_t oracle8cursor::getColumnNameLength(uint32_t col) {
	return (uint16_t)desc[col].buflen;
}

uint16_t oracle8cursor::getColumnType(uint32_t col) {
	switch (desc[col].dbtype) {
		case VARCHAR2_TYPE:
			return VARCHAR2_DATATYPE;
		case NUMBER_TYPE:
			return NUMBER_DATATYPE;
		case LONG_TYPE:
			return LONG_DATATYPE;
		case ROWID_TYPE:
			return ROWID_DATATYPE;
		case DATE_TYPE:
			return DATE_DATATYPE;
		case RAW_TYPE:
			return RAW_DATATYPE;
		case LONG_RAW_TYPE:
			return LONG_RAW_DATATYPE;
		case CHAR_TYPE:
			return CHAR_DATATYPE;
		case MLSLABEL_TYPE:
			return MLSLABEL_DATATYPE;
		case BLOB_TYPE:
			return BLOB_DATATYPE;
		case CLOB_TYPE:
			return CLOB_DATATYPE;
		case BFILE_TYPE:
			return BFILE_DATATYPE;
		default:
			return UNKNOWN_DATATYPE;
	}
}

uint32_t oracle8cursor::getColumnLength(uint32_t col) {
	return (uint32_t)desc[col].dbsize;
}

uint32_t oracle8cursor::getColumnPrecision(uint32_t col) {
	return (uint32_t)desc[col].precision;
}

uint32_t oracle8cursor::getColumnScale(uint32_t col) {
	return (uint32_t)desc[col].scale;
}

uint16_t oracle8cursor::getColumnIsNullable(uint32_t col) {
	return (uint16_t)desc[col].nullok;
}

uint16_t oracle8cursor::getColumnIsBinary(uint32_t col) {
	switch (getColumnType(col)) {
		case RAW_DATATYPE:
		case LONG_RAW_DATATYPE:
		case BLOB_DATATYPE:
		case BFILE_DATATYPE:
			return 1;
		default:
			return 0;
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
		OCIStmtFetch(stmt,oracle8conn->err,
				(ub4)oracle8conn->fetchatonce,
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
	if (freeresult && !resultfreed) {

		int32_t	selectlistsize=(oracle8conn->maxselectlistsize==-1)?
					ncols:oracle8conn->maxselectlistsize;

		for (int32_t i=0; i<selectlistsize; i++) {

			// free lob resources
			for (uint32_t j=0; j<oracle8conn->fetchatonce; j++) {
				if (def_lob[i][j]) {
					OCIDescriptorFree(
						def_lob[i][j],OCI_DTYPE_LOB);
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

			// free column resources...
			// For some reason, it's not always safe to do this for
			// a cursor that was bound to a result set from a
			// stored procedure.  Sometimes it works but other
			// times it crashes.  This could be an OCI bug.
			// I'm not sure.  It wish I knew more about exactly
			// when it succeeds or fails, as this leaks memory.
			if (desc[i].paramd && !bound) {
				OCIDescriptorFree(
					(dvoid *)desc[i].paramd,
					OCI_DTYPE_PARAM);
				desc[i].paramd=NULL;
			}
		}

		// deallocate buffers, if necessary
		if (stmttype==OCI_STMT_SELECT &&
			oracle8conn->maxselectlistsize==-1) {
			deallocateResultSetBuffers();
		}

		resultfreed=true;
	}

	if (freebinds) {
		// free lob bind resources
#ifdef HAVE_ORACLE_8i
		for (uint16_t i=0; i<orainbindlobcount; i++) {
			OCILobFreeTemporary(oracle8conn->svc,
							oracle8conn->err,
							inbind_lob[i]);
			OCILobClose(oracle8conn->svc,oracle8conn->err,
							inbind_lob[i]);
			OCIDescriptorFree(inbind_lob[i],OCI_DTYPE_LOB);
		}
		for (uint16_t i=0; i<oraoutbindlobcount; i++) {
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
		orainbindlobcount=0;
		oraoutbindlobcount=0;
#endif

		// free regular bind resources
		for (uint16_t i=0; i<orainbindcount; i++) {
			delete[] inintbindstring[i];
			inintbindstring[i]=NULL;
			delete indatebind[i];
			indatebind[i]=NULL;
		}
		for (uint16_t i=0; i<oraoutbindcount; i++) {
			delete[] outintbindstring[i];
			outintbindstring[i]=NULL;
			outintbind[i]=NULL;
			if (outdatebind[i]) {
				delete outdatebind[i]->ocidate;
			}
			delete outdatebind[i];
			outdatebind[i]=NULL;
		}
		orainbindcount=0;
		oraoutbindcount=0;
		oracurbindcount=0;
		for (uint16_t i=0; i<bindvarcount; i++) {
			bindvarname[i]=NULL;
			boundbypos[i]=false;
		}
	}
}
