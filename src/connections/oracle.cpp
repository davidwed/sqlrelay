// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/bytestring.h>
#include <rudiments/character.h>
#include <rudiments/environment.h>
#include <rudiments/file.h>
#include <rudiments/sys.h>
#include <rudiments/stdio.h>

#include <parsedatetime.h>

#include <datatypes.h>
#include <defines.h>
#include <config.h>

#ifdef ORACLE_AT_RUNTIME
	#include "oracleatruntime.cpp"
#endif

#ifdef HAVE_ORACLE_8i
	#include <rudiments/regularexpression.h>
#endif

#define MAX_BYTES_PER_CHAR	4

#ifdef OCI_STMT_CACHE
	// 9i calls OCI_STRLS_CACHE_DELETE something else
	#ifndef OCI_STRLS_CACHE_DELETE
		#define OCI_STRLS_CACHE_DELETE OCI_STMTCACHE_DELETE;
	#endif
#endif

#define STMT_CACHE_SIZE		0

extern "C" {
	#ifdef __CYGWIN__
		#define _int64 long long
	#endif
	#ifndef ORACLE_AT_RUNTIME
		#include <oci.h>
	#endif

	#define VARCHAR2_TYPE 1
	#define	NUMBER_TYPE 2
	#define	LONG_TYPE 8
	#define ROWID_TYPE 11
	#define DATE_TYPE 12
	#define RAW_TYPE 23
	#define LONG_RAW_TYPE 24
	#define CHAR_TYPE 96
	#define MLSLABEL_TYPE 105
	#define CLOB_TYPE 112
	#define BLOB_TYPE 113
	#define BFILE_TYPE 114

	#define LONG_BIND_TYPE 3
	#define DOUBLE_BIND_TYPE 4
	#define NULL_TERMINATED_STRING 5
}

struct describe {
	OCIParam	*paramd;
	// Hmm, oracle's docs say to use a ub4 for dbsize but it doesn't work...
	// I had used an sb4 before and it worked in the past, but not on
	// x86_64.
	ub2		dbsize;
	sb2		dbtype;
	text		*buf;
	sb4		buflen;
	ub1		precision;
	ub1		scale;
	ub1		nullok;
};

struct datebind {
	int16_t		*year;
	int16_t		*month;
	int16_t		*day;
	int16_t		*hour;
	int16_t		*minute;
	int16_t		*second;
	const char	**tz;
	bool		*isnegative;
	OCIDate		*ocidate;
};

class SQLRSERVER_DLLSPEC oracleconnection : public sqlrserverconnection {
	friend class oraclecursor;
	public:
				oracleconnection(sqlrservercontroller *cont);
				~oracleconnection();
	private:
		void		handleConnectString();
		#ifdef HAVE_ORACLE_8i
		bool		tempTableTruncateBeforeDrop();
		#endif
		bool		logIn(const char **error, const char **warning);
		const char	*logInError(const char *errmsg);
		sqlrservercursor	*newCursor(uint16_t id);
		void		deleteCursor(sqlrservercursor *curs);
		void		logOut();
		#ifdef OCI_ATTR_PROXY_CREDENTIALS
		bool		changeProxiedUser(const char *newuser,
						const char *newpassword);
		#endif
		bool		supportsTransactionBlocks();
		bool		supportsAutoCommit();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		const char	*pingQuery();
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostNameQuery();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getSchemaListQuery(bool wild);
		const char	*getTableListQuery(bool wild,
						uint16_t objecttypes);
		const char	*getGlobalTempTableListQuery();
		const char	*getColumnListQuery(
						const char *table,
						bool wild);
		const char	*getColumnListQueryWithoutKeys(
						const char *table,
						bool wild);
		const char	*getColumnListQueryWithKeys(
						const char *table,
						bool wild);
		const char	*getTypeInfoListQuery(
						const char *type,
						bool wild);
		const char	*isSynonymQuery();
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		const char	*getLastInsertIdQuery();
		const char	*noopQuery();

		ub4		stmtmode;

		OCIEnv		*env;
		OCIServer	*srv;
		OCIError	*err;
		OCISvcCtx	*svc;
		OCISession	*session;
		OCITrans	*trans;

		char		versionbuf[512];

		#ifdef OCI_ATTR_PROXY_CREDENTIALS
		OCISession	*newsession;
		bool		supportsproxycredentials;
		#endif
		bool		supportssyscontext;
		bool		requiresreprepare;

		const char	*home;
		const char	*sid;
		const char	*nlslang;

		char		*lastinsertidquery;

		stringbuffer	errormessage;

		#ifdef OCI_STMT_CACHE
		uint32_t	stmtcachesize;
		#endif
		#ifdef HAVE_ORACLE_8i
		bool		droptemptables;
		bool		temptabletruncatebeforedrop;
		#endif
		bool		rejectduplicatebinds;
		bool		disablekeylookup;

		const char	*identity;

		stringbuffer	alltypeinfoquery;
};

class SQLRSERVER_DLLSPEC oraclecursor : public sqlrservercursor {
	friend class oracleconnection;
	private:
				oraclecursor(sqlrserverconnection *conn,
								uint16_t id);
				~oraclecursor();
		void		allocateResultSetBuffers(int32_t columncount);
		void		deallocateResultSetBuffers();
		bool		open();
		bool		close();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		bool		inputBind(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint32_t valuesize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						bool *isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrservercursor *cursor);
		#ifdef HAVE_ORACLE_8i
		bool		inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBindGenericLob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull,
						ub1 temptype,
						ub2 type);
		bool		outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindGenericLob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull,
						ub2 type);
		bool		getLobOutputBindLength(uint16_t index,
							uint64_t *length);
		bool		getLobOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread);
		#endif
		bool		executeQuery(const char *query,
						uint32_t length);
		bool		fetchFromBindCursor();
		bool		executeQueryOrFetchFromBindCursor(
						const char *query,
						uint32_t length,
						bool execute);
		bool		validBinds();
		#ifdef HAVE_ORACLE_8i
		void		checkForTempTable(const char *query,
							uint32_t length);
		const char	*truncateTableQuery();
		#endif
		bool		queryIsNotSelect();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnNameLength(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		uint16_t	getColumnIsNullable(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
		bool		noRowsToReturn();
		bool		skipRow(bool *error);
		bool		fetchRow(bool *error);
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		nextRow();
		bool		getLobFieldLength(uint32_t col,
							uint64_t *length);
		bool		getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread);
		void		closeLobField(uint32_t col);
		void		closeResultSet();

		void		checkRePrepare();

		void		dateToString(char *buffer,
						uint16_t buffersize,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative);

		void		encodeBlob(stringbuffer *buffer,
						const char *data,
						uint32_t datasize);

		OCIStmt		*stmt;
		ub2		stmttype;
		#ifdef OCI_STMT_CACHE
		ub4		stmtreleasemode;
		#endif
		sword		ncols;

		int32_t		columncount;
		describe	*desc;
		OCIDefine	**def;
		OCILobLocator	***def_lob;
		ub1		**def_buf;
		sb2		**def_indp;
		ub2		**def_col_retlen;
		ub2		**def_col_retcode;

		uint16_t	maxbindcount;
		OCIBind		**inbindpp;
		OCIBind		**outbindpp;
		OCIBind		**curbindpp;
		char		**inintbindstring;
		OCIDate		**indatebind;
		char		**outintbindstring;
		datebind	**outdatebind;
		int64_t		**outintbind;
		uint16_t	orainbindcount;
		uint16_t	oraoutbindcount;
		uint16_t	oracurbindcount;
		const char	**bindvarname;
		bool		*boundbypos;
		uint16_t	bindvarcount;
		text		**bvnp;
		text		**invp;
		ub1		*inpl;
		ub1		*dupl;
		ub1		*bvnl;
		OCIBind		**hndl;

		#ifdef HAVE_ORACLE_8i
		OCILobLocator	**inbind_lob;
		OCILobLocator	**outbind_lob;
		uint16_t	orainbindlobcount;
		uint16_t	oraoutbindlobcount;
		#endif

		bool		bindformaterror;

		uint64_t	row;
		uint64_t	maxrow;
		uint64_t	totalrows;

		char		*query;
		uint32_t	length;
		bool		prepared;

		bool		bound;

		bool		resultfreed;

		oracleconnection	*oracleconn;

		#ifdef HAVE_ORACLE_8i
		regularexpression	preserverows;
		#endif
};

oracleconnection::oracleconnection(sqlrservercontroller *cont) :
						sqlrserverconnection(cont) {
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

	#ifdef OCI_STMT_CACHE
	stmtcachesize=STMT_CACHE_SIZE;
	#endif
	#ifdef HAVE_ORACLE_8i
	droptemptables=false;
	temptabletruncatebeforedrop=false;
	#endif
	rejectduplicatebinds=false;
	disablekeylookup=false;
	identity=NULL;
}

oracleconnection::~oracleconnection() {
	delete[] lastinsertidquery;
}

void oracleconnection::handleConnectString() {

	sqlrserverconnection::handleConnectString();

	sid=cont->getConnectStringValue("oracle_sid");
	home=cont->getConnectStringValue("oracle_home");

	nlslang=cont->getConnectStringValue("nls_lang");

	// override max field length if it was set too small
	if (cont->getMaxFieldLength()<MAX_BYTES_PER_CHAR) {
		cont->setMaxFieldLength(MAX_BYTES_PER_CHAR);
	}

	// When using OCI from 8.0, if the fetch buffer is bigger than 32767,
	// and you fetch a date, OCIStmtFetch will fail with
	// ORA-01801: date format is too long for internal buffer
	// at least with 8.0.5 on Redhat 5.2.
	#ifndef HAVE_ORACLE_8i
		if (cont->getMaxFieldLength()>32767) {
			cont->setMaxFieldLength(32767);
		}
	#endif

	#ifdef OCI_STMT_CACHE
	stmtcachesize=charstring::toUnsignedInteger(
				cont->getConnectStringValue("stmtcachesize"));
	if (!stmtcachesize) {
		stmtcachesize=STMT_CACHE_SIZE;
	}
	#endif

	#ifdef HAVE_ORACLE_8i
	droptemptables=charstring::isYes(
			cont->getConnectStringValue("droptemptables"));

	cont->addGlobalTempTables(
			cont->getConnectStringValue("globaltemptables"));
	#endif

	rejectduplicatebinds=charstring::isYes(
			cont->getConnectStringValue("rejectduplicatebinds"));

	disablekeylookup=charstring::isYes(
			cont->getConnectStringValue("disablekeylookup"));

	const char	*lastinsertidfunc=
			cont->getConnectStringValue("lastinsertidfunction");
	if (lastinsertidfunc) {
		stringbuffer	liiquery;
		liiquery.append("select ");
		liiquery.append(lastinsertidfunc);
		liiquery.append(" from dual");
		lastinsertidquery=liiquery.detachString();
	}

	identity=cont->getConnectStringValue("identity");
}

#ifdef HAVE_ORACLE_8i
bool oracleconnection::tempTableTruncateBeforeDrop() {
	// When dropping temporary tables, if any of those tables were created
	// with "on commit preserve rows" then the table has to be truncated
	// before it can be dropped or oracle will return the following error:
	// ORA-14452: attempt to create, alter or drop an index on
	// temporary table already in use
	//
	// It's not really clear why, but that's the case.
	if (temptabletruncatebeforedrop) {
		temptabletruncatebeforedrop=false;
		return true;
	}
	return false;
}
#endif

bool oracleconnection::logIn(const char **error, const char **warning) {

	// get user/password
	const char	*user=cont->getUser();
	const char	*password=cont->getPassword();

	// handle ORACLE_SID
	if (sid) {
		if (!environment::setValue("ORACLE_SID",sid)) {
			*error="Failed to set ORACLE_SID environment variable.";
			return false;
		}
	} else {
		if (!environment::getValue("ORACLE_SID")) {
			*error="No ORACLE_SID environment variable set or specified in connect string.";
			return false;
		}
	}

	// handle TWO_TASK
	if (sid) {
		if (!environment::setValue("TWO_TASK",sid)) {
			*error="Failed to set TWO_TASK environment variable.";
			return false;
		}
	} else {
		// allow empty TWO_TASK when using OS-authentication
		// allow it in all cases?
		if (!environment::getValue("TWO_TASK") &&
				charstring::length(user) &&
				charstring::length(password)) {
			*error="No TWO_TASK environment variable set or specified in connect string.";
			return false;
		}
	}

	// see if the specified sid is in tnsnames.ora format
	bool	sidtnsnameformat=(charstring::length(sid) &&
					sid[charstring::length(sid)-1]==')');

	// handle ORACLE_HOME
	if (home) {
		if (!environment::setValue("ORACLE_HOME",home)) {
			*error="Failed to set ORACLE_HOME environment variable.";
			return false;
		}
	} else {
		if (!sidtnsnameformat &&
			!environment::getValue("ORACLE_HOME")) {
			*error="No ORACLE_HOME environment variable set or specified in connect string.";
			return false;
		}
	}

	// handle error reading tnsnames.ora
	if (!sidtnsnameformat) {
		if (!charstring::length(home)) {
			home=environment::getValue("ORACLE_HOME");
		}
		char	*tnsnamesora=NULL;
		charstring::printf(&tnsnamesora,
				"%s%cnetwork%cadmin%ctnsnames.ora",
				home,
				sys::getDirectorySeparator(),
				sys::getDirectorySeparator(),
				sys::getDirectorySeparator());
		if (!file::readable(tnsnamesora)) {
			stderror.printf(
				"Warning: %s is not readable by %s:%s\n",
				tnsnamesora,
				cont->getConfig()->getRunAsUser(),
				cont->getConfig()->getRunAsGroup());
		}
	}

	// handle NLS_LANG
	if (nlslang) {
		if (!environment::setValue("NLS_LANG",nlslang)) {
			*error="Failed to set NLS_LANG environment variable.";
			return false;
		}
	}

	// load libraries on demand, if necessary
	#ifdef ORACLE_AT_RUNTIME
	if (!loadLibraries(&errormessage)) {
		*error=errormessage.getString();
		return false;
	}
	#endif

	// init OCI
	#ifdef HAVE_ORACLE_8i
	if (OCIEnvCreate((OCIEnv **)&env,OCI_DEFAULT|OCI_OBJECT,(dvoid *)0,
				(dvoid *(*)(dvoid *, size_t))0,
				(dvoid *(*)(dvoid *, dvoid *, size_t))0,
				(void (*)(dvoid *, dvoid *))0,
				(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		*error=logInError("OCIEnvCreate() failed");
		return false;
	}
	#else
	if (OCIInitialize(OCI_DEFAULT,NULL,NULL,NULL,NULL)!=OCI_SUCCESS) {
		*error=logInError("OCIInitialize() failed");
		return false;
	}
	if (OCIEnvInit((OCIEnv **)&env,OCI_DEFAULT,
					0,(dvoid **)0)!=OCI_SUCCESS) {
		*error=logInError("OCIEnvInit() failed");
		return false;
	}
	#endif

	// allocate an error handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&err,
				OCI_HTYPE_ERROR,0,NULL)!=OCI_SUCCESS) {
		*error=logInError("OCIHandleAlloc(OCI_HTYPE_ERROR) failed");
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// allocate a server handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&srv,
				OCI_HTYPE_SERVER,0,NULL)!=OCI_SUCCESS) {
		*error=logInError("OCIHandleAlloc(OCI_HTYPE_SERVER) failed");
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// allocate a service context handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&svc,
				OCI_HTYPE_SVCCTX,0,NULL)!=OCI_SUCCESS) {
		*error=logInError("OCIHandleAlloc(OCI_HTYPE_SVCCTX) failed");
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// attach to the server
	if (OCIServerAttach(srv,err,(text *)sid,
				charstring::length(sid),0)!=OCI_SUCCESS) {
		*error=logInError("OCIServerAttach() failed");
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
		*error=logInError("Attach server to service failed");
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}

	// allocate a session handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&session,
				(ub4)OCI_HTYPE_SESSION,0,NULL)!=OCI_SUCCESS) {
		*error=logInError("OCIHandleAlloc(OCI_HTYPE_SESSION) failed");
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
		*error=logInError("Set username failed");
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
		*error=logInError("Set password failed");
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
	sword	result=OCISessionBegin(svc,err,session,cred,mode);
	if (result==OCI_SUCCESS_WITH_INFO) {
		// This can happen if the password will expire soon,
		// or possibly for other reasons.
		*warning=logInError(NULL); 
	} else if (result!=OCI_SUCCESS) {
		*error=logInError("OCISessionBegin() failed");
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
		*error=logInError("Attach session to service failed");
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
		*error=logInError("Set statement cache size failed");
		OCISessionEnd(svc,err,session,OCI_DEFAULT);
		OCIHandleFree(err,OCI_HTYPE_SESSION);
		OCIServerDetach(srv,err,OCI_DEFAULT);
		OCIHandleFree(svc,OCI_HTYPE_SVCCTX);
		OCIHandleFree(srv,OCI_HTYPE_SERVER);
		OCIHandleFree(err,OCI_HTYPE_ERROR);
		OCIHandleFree(env,OCI_HTYPE_ENV);
		return false;
	}
	if (cont->logEnabled() || cont->notificationsEnabled()) {
		if (OCIAttrGet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)&stmtcachesize,(ub4)0,
				(ub4)OCI_ATTR_STMTCACHESIZE,
				(OCIError *)err)==OCI_SUCCESS) {
			stringbuffer	debugstr;
			debugstr.append("cache size ");
			debugstr.append(stmtcachesize);
			cont->raiseDebugMessageEvent(debugstr.getString());
		}
	}
	#endif

	// allocate a transaction handle
	if (OCIHandleAlloc((dvoid *)env,(dvoid **)&trans,
				OCI_HTYPE_TRANS,0,0)!=OCI_SUCCESS) {
		*error=logInError("OCIHandleAlloc(OCI_HTYPE_TRANS) failed");
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
		*error=logInError("OCIAttrSet(OCI_ATTR_TRANS) failed");
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
		if (major>8 || (major==8 && minor>0)) {
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
			*error=logInError("Prepare alter session failed.");
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
			*error=logInError("Execute alter session failed.");
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
			*error=logInError("Statement release failed.");
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

const char *oracleconnection::logInError(const char *errmsg) {

	errormessage.clear();
	if (errmsg) {
		errormessage.append(errmsg)->append(": ");
	}

	// get the error message from oracle
	text	message[1024];
	bytestring::zero((void *)message,sizeof(message));
	sb4	errcode;
	OCIErrorGet((dvoid *)err,1,(text *)0,&errcode,
			message,sizeof(message),OCI_HTYPE_ERROR);
	message[1023]='\0';
	errormessage.append(message);
	return errormessage.getString();
}

sqlrservercursor *oracleconnection::newCursor(uint16_t id) {
	return (sqlrservercursor *)new oraclecursor(
					(sqlrserverconnection *)this,id);
}

void oracleconnection::deleteCursor(sqlrservercursor *curs) {
	delete (oraclecursor *)curs;
}

void oracleconnection::logOut() {

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
bool oracleconnection::changeProxiedUser(const char *newuser,
					const char *newpassword) {

	// fail if the database we're connected to
	// doesn't support proxy credentials
	if (!supportsproxycredentials) {
		return false;
	}

	// delete any previously existing "newsessions"
	if (newsession) {
		OCISessionEnd(svc,err,newsession,OCI_DEFAULT);
		OCIHandleFree(newsession,OCI_HTYPE_SESSION);
		newsession=NULL;
	}

	// clean up
	OCISessionEnd(svc,err,newsession,OCI_DEFAULT);
	OCIHandleFree(newsession,OCI_HTYPE_SESSION);
	newsession=NULL;

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

	// set the password
	// (this isn't usually necessary, as proxy users are trusted to switch
	// between the accounts they're configured to proxy, but we specifically
	// want to validate the password, so we'll include it)
	OCIAttrSet((dvoid *)newsession,(ub4)OCI_HTYPE_SESSION,
				(dvoid *)newpassword,
				(ub4)charstring::length(newpassword),
				(ub4)OCI_ATTR_PASSWORD,err);

	// use the proxy
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

bool oracleconnection::autoCommitOn() {
	stmtmode=OCI_COMMIT_ON_SUCCESS;
	return true;
}

bool oracleconnection::autoCommitOff() {
	stmtmode=OCI_DEFAULT;
	return true;
}

bool oracleconnection::supportsTransactionBlocks() {
	return false;
}

bool oracleconnection::supportsAutoCommit() {
	return true;
}

bool oracleconnection::commit() {
	return (OCITransCommit(svc,err,OCI_DEFAULT)==OCI_SUCCESS);
}

bool oracleconnection::rollback() {
	return (OCITransRollback(svc,err,OCI_DEFAULT)==OCI_SUCCESS);
}

void oracleconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {

	// get the message from oracle
	bytestring::zero(errorbuffer,errorbufferlength);
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
		case 20: // maximum number of processes is exceeded
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
			*liveconnection=false;
			break;
		default:
			*liveconnection=true;
			break;
	}
}

const char *oracleconnection::pingQuery() {
	return "select 1 from dual";
}

const char *oracleconnection::identify() {
	return (identity)?identity:"oracle";
}

const char *oracleconnection::dbVersion() {
	if (OCIServerVersion((dvoid *)svc,err,
				(text *)versionbuf,sizeof(versionbuf),
				OCI_HTYPE_SVCCTX)==OCI_SUCCESS) {
		return versionbuf;
	}
	return NULL;
}

const char *oracleconnection::dbHostNameQuery() {
	if (supportssyscontext) {
		return "select sys_context('USERENV','SERVER_HOST') from dual";
	}
	return NULL;
}

const char *oracleconnection::getDatabaseListQuery(bool wild) {
	return (wild)?"select "
			"	username, "
			"	NULL "
			"from "
			"	all_users "
			"where "
			"	username like upper('%s') "
			"order by username"
			:
			"select "
			"	username, "
			"	NULL "
			"from "
			"	all_users "
			"order by username";
}

const char *oracleconnection::getSchemaListQuery(bool wild) {
	return "select test from dual";
}

const char *oracleconnection::getTableListQuery(bool wild,
						uint16_t objecttypes) {
	if (supportssyscontext) {
		return (wild)?
			"select "
			"	NULL as table_cat, "
			"	owner as table_schem, "
			"	table_name as table_name, "
			"	'TABLE' as table_type, "
			"	NULL as remarks, "
			"	NULL as extra "
			"from "
			"	all_tables "
			"where "
			"	table_name like upper('%s') "
			"	and "
			"	owner=sys_context('userenv','current_schema') "
			"order by "
			"	owner, "
			"	table_name":

			"select "
			"	NULL as table_cat, "
			"	owner as table_schem, "
			"	table_name as table_name, "
			"	'TABLE' as table_type, "
			"	NULL as remarks, "
			"	NULL as extra "
			"from "
			"	all_tables "
			"where "
			"	owner=sys_context('userenv','current_schema') "
			"order by "
			"	owner, "
			"	table_name";
	} else {
		return (wild)?
			"select "
			"	NULL as table_cat, "
			"	owner as table_schem, "
			"	table_name as table_name, "
			"	'TABLE' as table_type, "
			"	NULL as remarks, "
			"	NULL as extra "
			"from "
			"	user_tables "
			"where "
			"	table_name like upper('%s') "
			"order by "
			"	owner, "
			"	table_name":

			"select "
			"	NULL as table_cat, "
			"	owner as table_schem, "
			"	table_name as table_name, "
			"	'TABLE' as table_type, "
			"	NULL as remarks, "
			"	NULL as extra "
			"from "
			"	user_tables "
			"order by "
			"	owner, "
			"	table_name";
	}
}

const char *oracleconnection::getGlobalTempTableListQuery() {
	if (supportssyscontext) {
		return "select "
			"	table_name "
			"from "
			"	all_tables "
			"where "
			"	owner=sys_context('userenv','current_schema') "
			"	and "
			"	temporary='Y'";
	} else {
		return "select "
			"	table_name, "
			"	NULL "
			"from "
			"	user_tables "
			"	and "
			"	temporary='Y'";
	}
}

const char *oracleconnection::getColumnListQuery(const char *table,
						bool wild) {
	return (disablekeylookup)?
			getColumnListQueryWithoutKeys(table,wild):
			getColumnListQueryWithKeys(table,wild);
}

const char *oracleconnection::getColumnListQueryWithoutKeys(
						const char *table,
						bool wild) {

	// It takes a lot longer to look up synonyms than tables.  It's quick
	// to see if the object is a synonym though, so we'll do that first
	// and only spend the extra time if it is.

	if (isSynonym(table)) {
		if (supportssyscontext) {
			return (wild)?
				"select "
				"	all_tab_columns.column_name, "
				"	all_tab_columns.data_type, "
				"	all_tab_columns.data_length, "
				"	all_tab_columns.data_precision, "
				"	all_tab_columns.data_scale, "
				"	all_tab_columns.nullable, "
				"	'' as key, "
				"	all_tab_columns.data_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	all_synonyms, "
				"	all_tab_columns "
				"where "
				"	all_synonyms.synonym_name=upper('%s') "
				"	and "
				"	(all_synonyms.owner= "
				"		sys_context( "
				"		'userenv', "
				"		'current_schema') "
				"	or "
				"	all_synonyms.owner='SYS' "
				"	or "
				"	all_synonyms.owner='SYSTEM') "
				"	and "
				"	all_tab_columns.table_name= "
				"		all_synonyms.table_name "
				"	and "
				"	all_tab_columns.owner= "
				"		all_synonyms.table_owner "
				"	and "
				"	all_tab_columns.column_name like "
				"			upper('%s') "
				"order by "
				"	all_tab_columns.column_id":
	
				"select "
				"	all_tab_columns.column_name, "
				"	all_tab_columns.data_type, "
				"	all_tab_columns.data_length, "
				"	all_tab_columns.data_precision, "
				"	all_tab_columns.data_scale, "
				"	all_tab_columns.nullable, "
				"	'' as key, "
				"	all_tab_columns.data_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	all_synonyms, "
				"	all_tab_columns "
				"where "
				"	all_synonyms.synonym_name=upper('%s') "
				"	and "
				"	(all_synonyms.owner= "
				"		sys_context( "
				"		'userenv', "
				"		'current_schema') "
				"	or "
				"	all_synonyms.owner='SYS' "
				"	or "
				"	all_synonyms.owner='SYSTEM') "
				"	and "
				"	all_tab_columns.table_name= "
				"		all_synonyms.table_name "
				"	and "
				"	all_tab_columns.owner= "
				"		all_synonyms.table_owner "
				"order by "
				"	all_tab_columns.column_id";
		} else {
			return (wild)?
				"select "
				"	user_tab_columns.column_name, "
				"	user_tab_columns.data_type, "
				"	user_tab_columns.data_length, "
				"	user_tab_columns.data_precision, "
				"	user_tab_columns.data_scale, "
				"	user_tab_columns.nullable, "
				"	'' as key, "
				"	user_tab_columns.data_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	user_synonyms, "
				"	user_tab_columns "
				"where "
				"	user_synonyms.synonym_name=upper('%s') "
				"	and "
				"	user_tab_columns.table_name= "
				"		user_synonyms.table_name "
				"	and "
				"	user_tab_columns.column_name like "
				"			upper('%s') "
				"order by "
				"	user_tab_columns.column_id":
	
				"select "
				"	user_tab_columns.column_name, "
				"	user_tab_columns.data_type, "
				"	user_tab_columns.data_length, "
				"	user_tab_columns.data_precision, "
				"	user_tab_columns.data_scale, "
				"	user_tab_columns.nullable, "
				"	'' as key, "
				"	user_tab_columns.data_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	user_synonyms, "
				"	user_tab_columns "
				"where "
				"	user_synonyms.synonym_name=upper('%s') "
				"	and "
				"	user_tab_columns.table_name= "
				"		user_synonyms.table_name "
				"order by "
				"	user_tab_columns.column_id";
		}
	}

	if (supportssyscontext) {
		return (wild)?
			"select "
			"	all_tab_columns.column_name, "
			"	all_tab_columns.data_type, "
			"	all_tab_columns.data_length, "
			"	all_tab_columns.data_precision, "
			"	all_tab_columns.data_scale, "
			"	all_tab_columns.nullable, "
			"	'' as key, "
			"	all_tab_columns.data_default, "
			"	'' as extra, "
			"	NULL "
			"from "
			"	all_tab_columns "
			"where "
			"	all_tab_columns.table_name=upper('%s') "
			"	and "
			"	all_tab_columns.column_name like upper('%s') "
			"	and "
			"	(all_tab_columns.owner="
			"		sys_context('userenv',"
			"				'current_schema') "
			"	or "
			"	all_tab_columns.owner='SYS' "
			"	or "
			"	all_tab_columns.owner='SYSTEM') "
			"order by "
			"	all_tab_columns.column_id":

			"select "
			"	all_tab_columns.column_name, "
			"	all_tab_columns.data_type, "
			"	all_tab_columns.data_length, "
			"	all_tab_columns.data_precision, "
			"	all_tab_columns.data_scale, "
			"	all_tab_columns.nullable, "
			"	'' as key, "
			"	all_tab_columns.data_default, "
			"	'' as extra, "
			"	NULL "
			"from "
			"	all_tab_columns "
			"where "
			"	all_tab_columns.table_name=upper('%s') "
			"	and "
			"	(all_tab_columns.owner="
			"		sys_context('userenv',"
			"				'current_schema') "
			"	or "
			"	all_tab_columns.owner='SYS' "
			"	or "
			"	all_tab_columns.owner='SYSTEM') "
			"order by "
			"	all_tab_columns.column_id";
	} else {
		return (wild)?
			"select "
			"	user_tab_columns.column_name, "
			"	user_tab_columns.data_type, "
			"	user_tab_columns.data_length, "
			"	user_tab_columns.data_precision, "
			"	user_tab_columns.data_scale, "
			"	user_tab_columns.nullable, "
			"	'' as key, "
			"	user_tab_columns.data_default, "
			"	'' as extra, "
			"	NULL "
			"from "
			"	user_tab_columns "
			"where "
			"	user_tab_columns.table_name=upper('%s') "
			"	and "
			"	user_tab_columns.column_name like upper('%s') "
			"order by "
			"	user_tab_columns.column_id":

			"select "
			"	user_tab_columns.column_name, "
			"	user_tab_columns.data_type, "
			"	user_tab_columns.data_length, "
			"	user_tab_columns.data_precision, "
			"	user_tab_columns.data_scale, "
			"	user_tab_columns.nullable, "
			"	'' as key, "
			"	user_tab_columns.data_default, "
			"	'' as extra, "
			"	NULL "
			"from "
			"	user_tab_columns "
			"where "
			"	user_tab_columns.table_name=upper('%s') "
			"order by "
			"	user_tab_columns.column_id";
	}
}

const char *oracleconnection::getColumnListQueryWithKeys(
						const char *table,
						bool wild) {

	// It takes a lot longer to look up synonyms than tables.  It's quick
	// to see if the object is a synonym though, so we'll do that first
	// and only spend the extra time if it is.

	if (isSynonym(table)) {
		if (supportssyscontext) {
			return (wild)?
				"select "
				"	all_tab_columns.column_name, "
				"	all_tab_columns.data_type, "
				"	all_tab_columns.data_length, "
				"	all_tab_columns.data_precision, "
				"	all_tab_columns.data_scale, "
				"	all_tab_columns.nullable, "
				"	cons.key, "
				"	all_tab_columns.data_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	all_synonyms, "
				"	all_tab_columns "
				"	left outer join "
				"		(select "
				"		all_cons_columns.owner, "
				"		all_cons_columns.table_name, "
				"		all_cons_columns.column_name, "
				"		case "
				"			all_constraints. "
				"			constraint_type "
				"			when 'P' then 'PRI' "
				"			when 'U' then 'UNI' "
				"			when 'R' then 'MUL' "
				"			else NULL "
				"			end as key "
				"		from "
				"			all_cons_columns, "
				"			all_constraints "
				"		where "
				"			all_constraints."
				"			constraint_name="
				"			all_cons_columns."
				"			constraint_name "
				"			and "
				"			all_cons_columns."
				"			position is not null) "
				"			cons "
				"	on ("
				"		cons.owner="
				"		all_tab_columns.owner "
				"		and "
				"		cons.table_name="
				"		all_tab_columns.table_name "
				"		and "
				"		cons.column_name="
				"		all_tab_columns.column_name) "
				"where "
				"	all_synonyms.synonym_name=upper('%s') "
				"	and "
				"	(all_synonyms.owner= "
				"		sys_context( "
				"		'userenv', "
				"		'current_schema') "
				"	or "
				"	all_synonyms.owner='SYS' "
				"	or "
				"	all_synonyms.owner='SYSTEM') "
				"	and "
				"	all_tab_columns.table_name= "
				"		all_synonyms.table_name "
				"	and "
				"	all_tab_columns.owner= "
				"		all_synonyms.table_owner "
				"	and "
				"	all_tab_columns.column_name like "
				"			upper('%s') "
				"order by "
				"	all_tab_columns.column_id":
	
				"select "
				"	all_tab_columns.column_name, "
				"	all_tab_columns.data_type, "
				"	all_tab_columns.data_length, "
				"	all_tab_columns.data_precision, "
				"	all_tab_columns.data_scale, "
				"	all_tab_columns.nullable, "
				"	cons.key, "
				"	all_tab_columns.data_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	all_synonyms, "
				"	all_tab_columns "
				"	left outer join "
				"		(select "
				"		all_cons_columns.owner, "
				"		all_cons_columns.table_name, "
				"		all_cons_columns.column_name, "
				"		case "
				"			all_constraints. "
				"			constraint_type "
				"			when 'P' then 'PRI' "
				"			when 'U' then 'UNI' "
				"			when 'R' then 'MUL' "
				"			else NULL "
				"		end as key "
				"		from "
				"			all_cons_columns, "
				"			all_constraints "
				"		where "
				"			all_constraints."
				"			constraint_name="
				"			all_cons_columns."
				"			constraint_name "
				"			and "
				"			all_cons_columns."
				"			position is not null) "
				"			cons "
				"	on ("
				"		cons.owner="
				"		all_tab_columns.owner "
				"		and "
				"		cons.table_name="
				"		all_tab_columns.table_name "
				"		and "
				"		cons.column_name="
				"		all_tab_columns.column_name) "
				"where "
				"	all_synonyms.synonym_name=upper('%s') "
				"	and "
				"	(all_synonyms.owner= "
				"		sys_context( "
				"		'userenv', "
				"		'current_schema') "
				"	or "
				"	all_synonyms.owner='SYS' "
				"	or "
				"	all_synonyms.owner='SYSTEM') "
				"	and "
				"	all_tab_columns.table_name= "
				"		all_synonyms.table_name "
				"	and "
				"	all_tab_columns.owner= "
				"		all_synonyms.table_owner "
				"order by "
				"	all_tab_columns.column_id";
		} else {
			return (wild)?
				"select "
				"	all_tab_columns.column_name, "
				"	all_tab_columns.data_type, "
				"	all_tab_columns.data_length, "
				"	all_tab_columns.data_precision, "
				"	all_tab_columns.data_scale, "
				"	all_tab_columns.nullable, "
				"	cons.key, "
				"	all_tab_columns.data_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	all_synonyms, "
				"	all_tab_columns "
				"where "
				"	all_synonyms.synonym_name=upper('%s') "
				"	and "
				"	all_tab_columns.table_name= "
				"		all_synonyms.table_name "
				"	and "
				"	all_tab_columns.owner= "
				"		all_synonyms.table_owner "
				"	and "
				"	all_tab_columns.column_name like "
				"			upper('%s') "
				"order by "
				"	all_tab_columns.column_id":
	
				"select "
				"	all_tab_columns.column_name, "
				"	all_tab_columns.data_type, "
				"	all_tab_columns.data_length, "
				"	all_tab_columns.data_precision, "
				"	all_tab_columns.data_scale, "
				"	all_tab_columns.nullable, "
				"	cons.key, "
				"	all_tab_columns.data_default, "
				"	'' as extra, "
				"	NULL "
				"from "
				"	all_synonyms, "
				"	all_tab_columns "
				"where "
				"	all_synonyms.synonym_name=upper('%s') "
				"	and "
				"	all_tab_columns.table_name= "
				"		all_synonyms.table_name "
				"	and "
				"	all_tab_columns.owner= "
				"		all_synonyms.table_owner "
				"order by "
				"	all_tab_columns.column_id";
		}
	}

	if (supportssyscontext) {
		return (wild)?
			"select "
			"	all_tab_columns.column_name, "
			"	all_tab_columns.data_type, "
			"	all_tab_columns.data_length, "
			"	all_tab_columns.data_precision, "
			"	all_tab_columns.data_scale, "
			"	all_tab_columns.nullable, "
			"	cons.key, "
			"	all_tab_columns.data_default, "
			"	'' as extra, "
			"	NULL "
			"from "
			"	all_tab_columns "
			"	left outer join "
			"		(select "
			"			all_cons_columns.owner, "
			"			all_cons_columns.table_name, "
			"			all_cons_columns.column_name, "
			"			case "
			"				all_constraints. "
			"				constraint_type "
			"				when 'P' then 'PRI' "
			"				when 'U' then 'UNI' "
			"				when 'R' then 'MUL' "
			"				else NULL "
			"			end as key "
			"		from "
			"			all_cons_columns, "
			"			all_constraints "
			"		where "
			"			all_constraints."
			"				constraint_name="
			"			all_cons_columns."
			"				constraint_name "
			"			and "
			"			all_cons_columns."
			"				position is not null) "
			"			cons "
			"	on ("
			"		cons.owner="
			"			all_tab_columns.owner "
			"		and "
			"		cons.table_name="
			"			all_tab_columns.table_name "
			"		and "
			"		cons.column_name="
			"			all_tab_columns.column_name) "
			"where "
			"	all_tab_columns.table_name=upper('%s') "
			"	and "
			"	all_tab_columns.column_name like upper('%s') "
			"	and "
			"	(all_tab_columns.owner="
			"		sys_context('userenv',"
			"				'current_schema') "
			"	or "
			"	all_tab_columns.owner='SYS' "
			"	or "
			"	all_tab_columns.owner='SYSTEM') "
			"order by "
			"	all_tab_columns.column_id":

			"select "
			"	all_tab_columns.column_name, "
			"	all_tab_columns.data_type, "
			"	all_tab_columns.data_length, "
			"	all_tab_columns.data_precision, "
			"	all_tab_columns.data_scale, "
			"	all_tab_columns.nullable, "
			"	cons.key, "
			"	all_tab_columns.data_default, "
			"	'' as extra, "
			"	NULL "
			"from "
			"	all_tab_columns "
			"	left outer join "
			"		(select "
			"			all_cons_columns.owner, "
			"			all_cons_columns.table_name, "
			"			all_cons_columns.column_name, "
			"			case "
			"				all_constraints. "
			"				constraint_type "
			"				when 'P' then 'PRI' "
			"				when 'U' then 'UNI' "
			"				when 'R' then 'MUL' "
			"				else NULL "
			"			end as key "
			"		from "
			"			all_cons_columns, "
			"			all_constraints "
			"		where "
			"			all_constraints."
			"				constraint_name="
			"			all_cons_columns."
			"				constraint_name "
			"			and "
			"			all_cons_columns."
			"				position is not null) "
			"			cons "
			"	on ("
			"		cons.owner="
			"			all_tab_columns.owner "
			"		and "
			"		cons.table_name="
			"			all_tab_columns.table_name "
			"		and "
			"		cons.column_name="
			"			all_tab_columns.column_name) "
			"where "
			"	all_tab_columns.table_name=upper('%s') "
			"	and "
			"	(all_tab_columns.owner="
			"		sys_context('userenv',"
			"				'current_schema') "
			"	or "
			"	all_tab_columns.owner='SYS' "
			"	or "
			"	all_tab_columns.owner='SYSTEM') "
			"order by "
			"	all_tab_columns.column_id";
	} else {
		return (wild)?
			"select "
			"	user_tab_columns.column_name, "
			"	user_tab_columns.data_type, "
			"	user_tab_columns.data_length, "
			"	user_tab_columns.data_precision, "
			"	user_tab_columns.data_scale, "
			"	user_tab_columns.nullable, "
			"	cons.key, "
			"	user_tab_columns.data_default, "
			"	'' as extra, "
			"	NULL "
			"from "
			"	user_tab_columns "
			"	left outer join "
			"		(select "
			"			user_cons_columns.owner, "
			"			user_cons_columns.table_name, "
			"			user_cons_columns.column_name, "
			"			case "
			"				user_constraints. "
			"				constraint_type "
			"				when 'P' then 'PRI' "
			"				when 'U' then 'UNI' "
			"				when 'R' then 'MUL' "
			"				else NULL "
			"			end as key "
			"		from "
			"			user_cons_columns, "
			"			user_constraints "
			"		where "
			"			user_constraints."
			"				constraint_name="
			"			user_cons_columns."
			"				constraint_name "
			"			and "
			"			all_cons_columns."
			"				position is not null) "
			"			cons "
			"	on ("
			"		cons.table_name="
			"			user_tab_columns.table_name "
			"		and "
			"		cons.column_name="
			"			user_tab_columns.column_name) "
			"where "
			"	user_tab_columns.table_name=upper('%s') "
			"	and "
			"	user_tab_columns.column_name like upper('%s') "
			"order by "
			"	user_tab_columns.column_id":

			"select "
			"	user_tab_columns.column_name, "
			"	user_tab_columns.data_type, "
			"	user_tab_columns.data_length, "
			"	user_tab_columns.data_precision, "
			"	user_tab_columns.data_scale, "
			"	user_tab_columns.nullable, "
			"	cons.key, "
			"	user_tab_columns.data_default, "
			"	'' as extra, "
			"	NULL "
			"from "
			"	user_tab_columns "
			"	left outer join "
			"		(select "
			"			user_cons_columns.owner, "
			"			user_cons_columns.table_name, "
			"			user_cons_columns.column_name, "
			"			case "
			"				user_constraints. "
			"				constraint_type "
			"				when 'P' then 'PRI' "
			"				when 'U' then 'UNI' "
			"				when 'R' then 'MUL' "
			"				else NULL "
			"			end as key "
			"		from "
			"			user_cons_columns, "
			"			user_constraints "
			"		where "
			"			user_constraints."
			"				constraint_name="
			"			user_cons_columns."
			"				constraint_name "
			"			and "
			"			all_cons_columns."
			"				position is not null) "
			"			cons "
			"	on ("
			"		cons.table_name="
			"			user_tab_columns.table_name "
			"		and "
			"		cons.column_name="
			"			user_tab_columns.column_name) "
			"where "
			"	user_tab_columns.table_name=upper('%s') "
			"order by "
			"	user_tab_columns.column_id";
	}
}

static const char	*chartype=
			"(select "
			"	'CHAR' as type_name, "
			"	1 as data_type, "
			"	2000 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	'length' as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'CHAR' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	1 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";

static const char	*nchartype=
			"(select "
			"	'NCHAR' as type_name, "
			"	-8 as data_type, "
			"	2000 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	'length' as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'NCHAR' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	-8 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";

static const char	*varchar2type=
			"(select "
			"	'VARCHAR2' as type_name, "
			"	12 as data_type, "
			"	4000 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	'length' as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'VARCHAR2' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	12 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";

static const char	*varchartype=
			"(select "
			"	'VARCHAR' as type_name, "
			"	12 as data_type, "
			"	4000 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	'length' as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'VARCHAR' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	12 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";
	
static const char	*nvarchar2type=
			"(select "
			"	'NVARCHAR2' as type_name, "
			"	-9 as data_type, "
			"	4000 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	'length' as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'NVARCHAR2' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	-9 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";
	
static const char	*clobtype=
			"(select "
			"	'CLOB' as type_name, "
			"	-1 as data_type, "
			"	140737488355328 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	null as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'CLOB' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	-1 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";
	
static const char	*nclobtype=
			"(select "
			"	'NCLOB' as type_name, "
			"	-10 as data_type, "
			"	140737488355328 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	null as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'NCLOB' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	-10 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";
		
static const char	*longtype=
			"(select "
			"	'LONG' as type_name, "
			"	-1 as data_type, "
			"	4294967295 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	null as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'LONG' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	-1 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";

static const char	*numbertype=
			"(select "
			"	'NUMBER' as type_name, "
			"	2 as data_type, "
			"	38 as column_size, "
			"	null as literal_prefix, "
			"	null as literal_suffix, "
			"	'precision,scale' as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	0 as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'NUMBER' as local_type_name, "
			"	0 as minimum_scale, "
			"	38 as maximum_scale, "
			"	2 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	10 as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";
	
static const char	*datetype=
			"(select "
			"	'DATE' as type_name, "
			"	9 as data_type, "
			"	26 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	null as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'DATE' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	9 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";

static const char	*blobtype=
			"(select "
			"	'BLOB' as type_name, "
			"	-2 as data_type, "
			"	140737488355328 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	null as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'BLOB' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	-2 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";

static const char	*bfiletype=
			"(select "
			"	'BFILE' as type_name, "
			"	-4 as data_type, "
			"	8589934592 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	null as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'BFILE' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	-4 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";

static const char	*rawtype=
			"(select "
			"	'RAW' as type_name, "
			"	-3 as data_type, "
			"	2000 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	null as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'RAW' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	-3 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";

static const char	*longrawtype=
			"(select "
			"	'LONGRAW' as type_name, "
			"	-4 as data_type, "
			"	4000 as column_size, "
			"	'''' as literal_prefix, "
			"	'''' as literal_suffix, "
			"	null as create_params, "
			"	1 as nullable, "
			"	1 as case_sensitive, "
			"	3 as searchable, "
			"	NULL as unsigned_attribute, "
			"	0 as fixed_prec_scale, "
			"	0 as auto_unique_value, "
			"	'LONGRAW' as local_type_name, "
			"	null as minimum_scale, "
			"	null as maximum_scale, "
			"	-4 as sql_data_type, "
			"	null as sql_datetime_sub, "
			"	null as num_prec_radix, "
			"	null as interval_precision "
			"from "
			"	dual) ";

const char *oracleconnection::getTypeInfoListQuery(const char *type,
							bool wild) {

	if (!charstring::compare(type,"*")) {
		if (!alltypeinfoquery.getStringLength()) {
			alltypeinfoquery.append(chartype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(nchartype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(varchar2type);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(varchartype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(nvarchar2type);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(clobtype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(nclobtype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(longtype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(numbertype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(datetype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(blobtype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(bfiletype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(rawtype);
			alltypeinfoquery.append("union ");
			alltypeinfoquery.append(longrawtype);
		}
		return alltypeinfoquery.getString();
	} else if (!charstring::compareIgnoringCase(type,"char")) {
		return chartype;
	} else if (!charstring::compareIgnoringCase(type,"nchar")) {
		return nchartype;
	} else if (!charstring::compareIgnoringCase(type,"varchar2")) {
		return varchar2type;
	} else if (!charstring::compareIgnoringCase(type,"varchar")) {
		return varchartype;
	} else if (!charstring::compareIgnoringCase(type,"nvarchar2")) {
		return nvarchar2type;
	} else if (!charstring::compareIgnoringCase(type,"clob")) {
		return clobtype;
	} else if (!charstring::compareIgnoringCase(type,"nclob")) {
		return nclobtype;
	} else if (!charstring::compareIgnoringCase(type,"long")) {
		return longtype;
	} else if (!charstring::compareIgnoringCase(type,"number")) {
		return numbertype;
	} else if (!charstring::compareIgnoringCase(type,"date")) {
		return datetype;
	} else if (!charstring::compareIgnoringCase(type,"blob")) {
		return blobtype;
	} else if (!charstring::compareIgnoringCase(type,"bfile")) {
		return bfiletype;
	} else if (!charstring::compareIgnoringCase(type,"raw")) {
		return rawtype;
	}
	return NULL;
}

const char *oracleconnection::isSynonymQuery() {
	if (supportssyscontext) {
		return "select "
			"	object_name "
			"from "
			"	all_objects "
			"where "
			"	object_name=upper('%s') "
			"	and "
			"	object_type='SYNONYM'"
			"	and "
			"	(owner=sys_context('userenv', "
			"			'current_schema') "
			"	or "
			"	owner='SYS' "
			"	or "
			"	owner='SYSTEM')";
	} else {
		return "select "
			"	object_name "
			"from "
			"	all_objects "
			"where "
			"	object_name=upper('%s') "
			"	and "
			"	object_type='SYNONYM'";
	}
}

const char *oracleconnection::selectDatabaseQuery() {
	return "alter session set current_schema=%s";
}

const char *oracleconnection::getCurrentDatabaseQuery() {
	return "select sys_context('userenv','current_schema') from dual";
}

const char *oracleconnection::getLastInsertIdQuery() {
	return lastinsertidquery;
}

const char *oracleconnection::noopQuery() {
	return "begin null; end;";
}

oraclecursor::oraclecursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {

	stmt=NULL;
	stmttype=0;
	#ifdef OCI_STMT_CACHE
	stmtreleasemode=OCI_DEFAULT;
	#endif
	ncols=0;

	oracleconn=(oracleconnection *)conn;
	allocateResultSetBuffers(conn->cont->getMaxColumnCount());

	maxbindcount=conn->cont->getConfig()->getMaxBindCount();
	inbindpp=new OCIBind *[maxbindcount];
	outbindpp=new OCIBind *[maxbindcount];
	curbindpp=new OCIBind *[maxbindcount];
	inintbindstring=new char *[maxbindcount];
	indatebind=new OCIDate *[maxbindcount];
	outintbindstring=new char *[maxbindcount];
	outdatebind=new datebind *[maxbindcount];
	outintbind=new int64_t *[maxbindcount];
	bindvarname=new const char *[maxbindcount];
	boundbypos=new bool[maxbindcount];
	bvnp=new text *[maxbindcount];
	invp=new text *[maxbindcount];
	inpl=new ub1[maxbindcount];
	dupl=new ub1[maxbindcount];
	bvnl=new ub1[maxbindcount];
	hndl=new OCIBind *[maxbindcount];
	for (uint16_t i=0; i<maxbindcount; i++) {
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
	inbind_lob=new OCILobLocator *[maxbindcount];
	outbind_lob=new OCILobLocator *[maxbindcount];
	for (uint16_t i=0; i<maxbindcount; i++) {
		inbind_lob[i]=NULL;
		outbind_lob[i]=NULL;
	}
	orainbindlobcount=0;
	oraoutbindlobcount=0;
	#endif
	bindformaterror=false;

	row=0;
	maxrow=0;
	totalrows=0;

	query=NULL;
	length=0;
	prepared=false;
	bound=false;

	resultfreed=true;

	#ifdef HAVE_ORACLE_8i
	setCreateTempTablePattern("(create|CREATE)[ 	\n\r]+(global|GLOBAL)[ 	\n\r]+(temporary|TEMPORARY)[ 	\n\r]+(table|TABLE)[ 	\n\r]+");
	preserverows.setPattern("(on|ON)[ 	\n\r]+(commit|COMMIT)[ 	\n\r]+(preserve|PRESERVE)[ 	\n\r]+(rows|ROWS)");
	preserverows.study();
	#endif
}

oraclecursor::~oraclecursor() {

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

void oraclecursor::allocateResultSetBuffers(int32_t columncount) {

	if (!columncount) {
		this->columncount=0;
		desc=NULL;
		def=NULL;
		def_lob=NULL;
		def_buf=NULL;
		def_indp=NULL;
		def_col_retlen=NULL;
		def_col_retcode=NULL;
	} else {
		this->columncount=columncount;
		desc=new describe[columncount];
		def=new OCIDefine *[columncount];
		def_lob=new OCILobLocator **[columncount];
		def_buf=new ub1 *[columncount];
		def_indp=new sb2 *[columncount];
		def_col_retlen=new ub2 *[columncount];
		def_col_retcode=new ub2 *[columncount];
		uint32_t	fetchatonce=conn->cont->getFetchAtOnce();
		uint32_t	maxfieldlength=conn->cont->getMaxFieldLength();
		for (int32_t i=0; i<columncount; i++) {
			def_lob[i]=new OCILobLocator *[fetchatonce];
			for (uint32_t j=0; j<fetchatonce; j++) {
				def_lob[i][j]=NULL;
			}
			def_buf[i]=new ub1[fetchatonce*maxfieldlength];
			def_indp[i]=new sb2[fetchatonce];
			def_col_retlen[i]=new ub2[fetchatonce];
			def_col_retcode[i]=new ub2[fetchatonce];
			def[i]=NULL;
			desc[i].paramd=NULL;
		}
	}
}

void oraclecursor::deallocateResultSetBuffers() {
	if (columncount) {
		for (int32_t i=0; i<columncount; i++) {
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
		columncount=0;
	}
}

bool oraclecursor::open() {

	stmt=NULL;

	#ifdef OCI_STMT_CACHE
	// If statement caching is available then we don't need to allocate
	// a cursor handle here, as it will be allocated by the call to
	// OCIStmtPrepare2 later.
	//
	// If statement caching isn't available then we need to allocate a
	// cursor handle here and set the number of rows to prefetch.
	stmtreleasemode=OCI_DEFAULT;
	if (oracleconn->stmtcachesize) {
		return true;
	}
	#endif

	// allocate a cursor handle
	if (OCIHandleAlloc((dvoid *)oracleconn->env,
				(dvoid **)&stmt,
				OCI_HTYPE_STMT,(size_t)0,
				(dvoid **)0)!=OCI_SUCCESS) {
		return false;
	}

	// set the number of rows to prefetch
	uint32_t	fetchatonce=conn->cont->getFetchAtOnce();
	return (OCIAttrSet((dvoid *)stmt,OCI_HTYPE_STMT,
				(dvoid *)&fetchatonce,
				(ub4)0,OCI_ATTR_PREFETCH_ROWS,
				(OCIError *)oracleconn->err)==OCI_SUCCESS);
}

bool oraclecursor::close() {

	closeResultSet();

	#ifdef OCI_STMT_CACHE
	if (oracleconn->stmtcachesize && stmt) {
		return OCIStmtRelease(stmt,oracleconn->err,
				NULL,0,OCI_STRLS_CACHE_DELETE)==OCI_SUCCESS;
	}
	#endif

	return (OCIHandleFree(stmt,OCI_HTYPE_STMT)==OCI_SUCCESS);
}

bool oraclecursor::prepareQuery(const char *query, uint32_t length) {

	// initialize column count
	ncols=0;

	// keep a pointer to the query and length in case it needs to be 
	// reprepared later
	this->query=(char *)query;
	this->length=length;

	// if the query is being prepared then apparently this isn't an
	// output bind cursor
	bound=false;

	// reset the bind format error flag
	bindformaterror=false;

	// If statement caching is available then use OCIStmtPrepare2,
	// otherwise just use OCIStmtPrepare.

	#ifdef OCI_STMT_CACHE
	if (oracleconn->stmtcachesize) {

		// release any prior-allocated statement...
		if (stmt) {

			// delete DML statements from the cache
			if (stmttype==OCI_STMT_DROP ||
					stmttype==OCI_STMT_CREATE ||
					stmttype==OCI_STMT_ALTER) {
				stmtreleasemode=OCI_STRLS_CACHE_DELETE;
			}

			if (OCIStmtRelease(stmt,oracleconn->err,
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
		if (oracleconn->cont->logEnabled() ||
			oracleconn->cont->notificationsEnabled()) {
			// check for a statment cache hit
			// and report our findings
			if (OCIStmtPrepare2(oracleconn->svc,&stmt,
					oracleconn->err,
					(text *)query,(ub4)length,
					NULL,0,
					(ub4)OCI_NTV_SYNTAX,
					(ub4)OCI_PREP2_CACHE_SEARCHONLY)==
					OCI_SUCCESS) {
				// we got a hit and don't
				// need to do anything else
				oracleconn->cont->raiseDebugMessageEvent(
							"statement cache hit");
				prepare=false;
			} else {
				// we didn't get a hit and
				// need to prepare the query
				oracleconn->cont->raiseDebugMessageEvent(
							"statement cache miss");
			}
		}
		if (prepare) {
			// prepare the query
			if (OCIStmtPrepare2(oracleconn->svc,&stmt,
					oracleconn->err,
					(text *)query,(ub4)length,
					NULL,0,
					(ub4)OCI_NTV_SYNTAX,
					(ub4)OCI_DEFAULT)!=OCI_SUCCESS) {
				return false;
			}
		}

		// set the number of rows to prefetch
		// FIXME: we set this in open(), does it need to be reset?
		uint32_t	fetchatonce=conn->cont->getFetchAtOnce();
		return (OCIAttrSet((dvoid *)stmt,OCI_HTYPE_STMT,
				(dvoid *)&fetchatonce,
				(ub4)0,OCI_ATTR_PREFETCH_ROWS,
				(OCIError *)oracleconn->err)==OCI_SUCCESS);

	}
	#endif

	// reset the statement type
	stmttype=0;

	// prepare the query
	return (OCIStmtPrepare(stmt,oracleconn->err,
				(text *)query,(ub4)length,
				(ub4)OCI_NTV_SYNTAX,
				(ub4)OCI_DEFAULT)==OCI_SUCCESS);
}

void oraclecursor::checkRePrepare() {

	// I once thought that it was necessary to re-prepare every time we
	// re-execute.  It turns out that actually, when using OCI lower than
	// 8i or against DB's lower than 9i, if we bind one type (say, a date),
	// execute, then re-bind a different type (say, a string representation
	// of a date), then re-execute, then it will throw ORA-01475.  Keeping
	// track of the type of each bind variable and deciding whether to
	// reprepare or not is a bit of work.  Maybe I'll do that at some point.
	// For now, with those versions, we'll reprepare if we rebind at all.

	if (oracleconn->requiresreprepare && !prepared &&
			stmttype && stmttype!=OCI_STMT_SELECT) {
		closeResultSet();
		prepareQuery(query,length);
		prepared=true;
	}
}

void oraclecursor::dateToString(char *buffer, uint16_t buffersize,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t microsecond, const char *tz,
				bool isnegative) {

	const char	*format=
		conn->cont->getConfig()->getFakeInputBindVariablesDateFormat();
	if (!charstring::isNullOrEmpty(format)) {
		// FIXME: it'd be nice if we could pass buffer/buffersize
		// into convertDateTime
		char	*newdate=conn->cont->convertDateTime(format,
							year,month,day,
							hour,minute,second,
							microsecond,
							false);
		charstring::safeCopy(buffer,buffersize,newdate);
		delete[] newdate;
		return;
	}

	// typically oracle just wants DD-MON-YYYY but if hour,
	// minute and second are non-zero then use them too
	if (hour && minute && second) {
		charstring::printf(buffer,buffersize,
					"%02d-%s-%04d %02d:%02d:%02d",
					day,shortmonths[month-1],year,
					hour,minute,second);
	} else {
		charstring::printf(buffer,buffersize,
					"%02d-%s-%04d",
					day,shortmonths[month-1],year);
	}
}

void oraclecursor::encodeBlob(stringbuffer *buffer,
					const char *data, uint32_t datasize) {

	// oracle wants each byte of blob data to be converted to two
	// hex characters...
	// eg: hello - > 6865656F
	// oracle also wants quotes around a blob

	buffer->append('\'');
	for (uint32_t i=0; i<datasize; i++) {
		buffer->append(conn->cont->asciiToHex(data[i]));
	}
	buffer->append('\'');
}

bool oraclecursor::inputBind(const char *variable,
				uint16_t variablesize,
				const char *value,
				uint32_t valuesize,
				int16_t *isnull) {
	checkRePrepare();

	// the size of the value must include the terminating NULL
	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracleconn->err,pos,
				(dvoid *)value,
				(sb4)valuesize+1,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&inbindpp[orainbindcount],
				oracleconn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,
				(sb4)valuesize+1,
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


bool oraclecursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value) {
	checkRePrepare();

	inintbindstring[orainbindcount]=charstring::parseNumber(*value);

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracleconn->err,pos,
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
				oracleconn->err,
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


bool oraclecursor::inputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {
	checkRePrepare();

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracleconn->err,pos,
				(dvoid *)value,(sb4)sizeof(double),
				SQLT_FLT,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&inbindpp[orainbindcount],
				oracleconn->err,
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


bool oraclecursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t year,
				int16_t month,
				int16_t day,
				int16_t hour,
				int16_t minute,
				int16_t second,
				int32_t microsecond,
				const char *tz,
				bool isnegative,
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
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracleconn->err,pos,
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
				oracleconn->err,
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


bool oraclecursor::outputBind(const char *variable,
				uint16_t variablesize,
				char *value,
				uint32_t valuesize,
				int16_t *isnull) {
	checkRePrepare();

	outintbindstring[oraoutbindcount]=NULL;
	outdatebind[oraoutbindcount]=NULL;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
				oracleconn->err,pos,
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
				oracleconn->err,
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

bool oraclecursor::outputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {
	checkRePrepare();

	outintbindstring[oraoutbindcount]=new char[21];
	bytestring::zero(outintbindstring[oraoutbindcount],21);
	outintbind[oraoutbindcount]=value;
	outdatebind[oraoutbindcount]=NULL;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
				oracleconn->err,pos,
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
				oracleconn->err,
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

bool oraclecursor::outputBind(const char *variable,
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
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
				oracleconn->err,pos,
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
				oracleconn->err,
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

bool oraclecursor::outputBind(const char *variable,
				uint16_t variablesize,
				int16_t *year,
				int16_t *month,
				int16_t *day,
				int16_t *hour,
				int16_t *minute,
				int16_t *second,
				int32_t *microsecond,
				const char **tz,
				bool *isnegative,
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
	db->isnegative=isnegative;
	db->ocidate=new OCIDate;
	outdatebind[oraoutbindcount]=db;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
				oracleconn->err,pos,
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
				oracleconn->err,
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

bool oraclecursor::outputBindCursor(const char *variable,
					uint16_t variablesize,
					sqlrservercursor *cursor) {

	#ifdef OCI_STMT_CACHE
	// If the statement cache is in use then OCIStmtExecute will crash
	// if the query includes cursor binds.  I'm not sure if this is an OCI
	// bug or a problem caused by SQL Relay somehow, but until I discover
	// a solution, we'll return an error here.  Ideally, I'd set an error
	// message too...
	if (oracleconn->stmtcachesize) {
		return false;
	}
	#endif

	checkRePrepare();

	((oraclecursor *)cursor)->bound=true;

	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&curbindpp[oracurbindcount],
				oracleconn->err,pos,
				(dvoid *)&(((oraclecursor *)cursor)->stmt),
				(sb4)0,
				SQLT_RSET,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&curbindpp[oracurbindcount],
				oracleconn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)&(((oraclecursor *)cursor)->stmt),
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
	((oraclecursor *)cursor)->stmttype=0;
	((oraclecursor *)cursor)->ncols=0;
	((oraclecursor *)cursor)->row=0;
	((oraclecursor *)cursor)->maxrow=0;
	((oraclecursor *)cursor)->totalrows=0;
	((oraclecursor *)cursor)->bound=true;
	return true;
}

#ifdef HAVE_ORACLE_8i
bool oraclecursor::inputBindBlob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	return inputBindGenericLob(variable,variablesize,
					value,valuesize,isnull,
					OCI_TEMP_BLOB,SQLT_BLOB);
}

bool oraclecursor::inputBindClob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	return inputBindGenericLob(variable,variablesize,
					value,valuesize,isnull,
					OCI_TEMP_CLOB,SQLT_CLOB);
}

bool oraclecursor::inputBindGenericLob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull,
					ub1 temptype,
					ub2 type) {

	checkRePrepare();

	// create a temporary lob, write the value to it
	if (OCIDescriptorAlloc((dvoid *)oracleconn->env,
			(dvoid **)&inbind_lob[orainbindlobcount],
			(ub4)OCI_DTYPE_LOB,
			(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		return false;
	}

	if (OCILobCreateTemporary(oracleconn->svc,oracleconn->err,
			inbind_lob[orainbindlobcount],
			//(ub2)0,SQLCS_IMPLICIT,
			(ub2)OCI_DEFAULT,OCI_DEFAULT,
			temptype,OCI_ATTR_NOCACHE,
			OCI_DURATION_SESSION)!=OCI_SUCCESS) {
		OCIDescriptorFree(inbind_lob[orainbindlobcount],OCI_DTYPE_LOB);
		return false;
	}

	if (OCILobOpen(oracleconn->svc,oracleconn->err,
			inbind_lob[orainbindlobcount],
			OCI_LOB_READWRITE)!=OCI_SUCCESS) {
		OCILobFreeTemporary(oracleconn->svc,oracleconn->err,
					inbind_lob[orainbindlobcount]);
		OCIDescriptorFree(inbind_lob[orainbindlobcount],OCI_DTYPE_LOB);
		return false;
	}

	ub4	size=valuesize;
	if (OCILobWrite(oracleconn->svc,oracleconn->err,
			inbind_lob[orainbindlobcount],&size,1,
			(void *)value,valuesize,
			OCI_ONE_PIECE,(dvoid *)0,
			(sb4 (*)(dvoid*,dvoid*,ub4*,ub1 *))0,
			0,SQLCS_IMPLICIT)!=OCI_SUCCESS) {

		OCILobClose(oracleconn->svc,oracleconn->err,
					inbind_lob[orainbindlobcount]);
		OCILobFreeTemporary(oracleconn->svc,oracleconn->err,
					inbind_lob[orainbindlobcount]);
		OCIDescriptorFree(inbind_lob[orainbindlobcount],OCI_DTYPE_LOB);
		return false;
	}

	// bind the temporary lob
	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&inbindpp[orainbindcount],
				oracleconn->err,pos,
				(dvoid *)&inbind_lob[orainbindlobcount],(sb4)0,
				type,
				(dvoid *)0,(ub2 *)0,(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT)!=OCI_SUCCESS) {
			return false;
		}
		boundbypos[pos-1]=true;
	} else {
		if (OCIBindByName(stmt,&inbindpp[orainbindcount],
				oracleconn->err,
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

bool oraclecursor::outputBindBlob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	return outputBindGenericLob(variable,variablesize,index,
						isnull,SQLT_BLOB);
}

bool oraclecursor::outputBindClob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	return outputBindGenericLob(variable,variablesize,index,
						isnull,SQLT_CLOB);
}

bool oraclecursor::outputBindGenericLob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull,
					ub2 type) {

	checkRePrepare();

	// allocate a lob descriptor
	if (OCIDescriptorAlloc((dvoid *)oracleconn->env,
		(dvoid **)&outbind_lob[index],(ub4)OCI_DTYPE_LOB,
		(size_t)0,(dvoid **)0)!=OCI_SUCCESS) {
		return false;
	}
	oraoutbindlobcount=index+1;

	// bind the lob descriptor
	if (charstring::isInteger(variable+1,variablesize-1)) {
		ub4	pos=charstring::toInteger(variable+1);
		if (!pos) {
			bindformaterror=true;
			return false;
		}
		if (OCIBindByPos(stmt,&outbindpp[oraoutbindcount],
			oracleconn->err,pos,
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
				oracleconn->err,
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

bool oraclecursor::getLobOutputBindLength(uint16_t index, uint64_t *length) {
	ub4	loblength=0;
	bool	retval=(OCILobGetLength(oracleconn->svc,
				oracleconn->err,
				outbind_lob[index],
				&loblength)==OCI_SUCCESS);
	*length=loblength;
	return retval;
}

bool oraclecursor::getLobOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {

	// initialize the read length to the number of characters to read
	ub4	readlength=charstoread;

	// read from the lob
	// (offset+1 is very important,
	// apparently oracle lob offsets are 1-based)
	sword	result=OCILobRead(oracleconn->svc,
				oracleconn->err,
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

void oraclecursor::checkForTempTable(const char *query, uint32_t length) {

	// see if the query matches the pattern for a temporary query that
	// creates a temporary table
	const char	*ptr=skipCreateTempTableClause(query);
	if (!ptr) {
		return;
	}

	// get the table name
	stringbuffer	tablename;
	const char	*endptr=query+length;
	while (ptr && *ptr && *ptr!=' ' &&
		*ptr!='\n' && *ptr!='	' && ptr<endptr) {
		tablename.append(*ptr);
		ptr++;
	}

	// look for "on commit preserve rows"
	bool	preserverowsoncommit=preserverows.match(ptr);

	if (oracleconn->droptemptables) {

		// When dropping temporary tables, if any of those tables were
		// created with "on commit preserve rows" then the table has to
		// be truncated before it can be dropped or oracle will return
		// the following error:
		// ORA-14452: attempt to create, alter or drop an index on
		// temporary table already in use
		//
		// It's not really clear why, but that's the case.
		// As such, we'll set a flag to cause truncation if the query
		// contained that clause.
		oracleconn->temptabletruncatebeforedrop=preserverowsoncommit;

		// if "droptemptables" was specified...
		conn->cont->addSessionTempTableForDrop(tablename.getString());

	} else if (preserverowsoncommit) {

		// If "on commit preserve rows" was specified, then when
		// the commit/rollback is executed at the end of the
		// session, the data won't be truncated.  It needs to
		// be though, so we'll set it up to be truncated manually.
		conn->cont->addSessionTempTableForTrunc(tablename.getString());
	}
}

const char *oraclecursor::truncateTableQuery() {
	return "truncate table";
}
#endif

bool oraclecursor::executeQuery(const char *query, uint32_t length) {
	return executeQueryOrFetchFromBindCursor(query,length,true);
}

bool oraclecursor::fetchFromBindCursor() {
	return executeQueryOrFetchFromBindCursor(NULL,0,false);
}

bool oraclecursor::executeQueryOrFetchFromBindCursor(const char *query,
							uint32_t length,
							bool execute) {

	// initialize the row and column counters
	row=0;
	maxrow=0;
	totalrows=0;

	// get the type of the query (select, insert, update, etc...)
	if (OCIAttrGet(stmt,OCI_HTYPE_STMT,
			(dvoid *)&stmttype,(ub4 *)NULL,
			OCI_ATTR_STMT_TYPE,oracleconn->err)!=OCI_SUCCESS) {
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
			if (OCIStmtExecute(oracleconn->svc,stmt,
					oracleconn->err,
					(stmttype==OCI_STMT_SELECT)?0:1,
					(ub4)0,NULL,NULL,
					oracleconn->stmtmode)==OCI_SUCCESS) {
				break;
			}
			if (!first) {
				return false;
			}
			sb4	errcode=0;
			OCIErrorGet((dvoid *)oracleconn->err,
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
				oracleconn->err)!=OCI_SUCCESS) {
			return false;
		}

		// validate column count
		uint32_t	maxcolumncount=conn->cont->getMaxColumnCount();
		if (maxcolumncount && (uint32_t)ncols>maxcolumncount) {
			stringbuffer	err;
			err.append(SQLR_ERROR_MAXSELECTLIST_STRING);
			err.append(" (")->append(ncols)->append('>');
			err.append(maxcolumncount);
			err.append(')');
			conn->cont->setError(this,err.getString(),
						SQLR_ERROR_MAXSELECTLIST,true);
			return false;
		}

		// allocate buffers, if necessary
		if (!maxcolumncount) {
			allocateResultSetBuffers(ncols);
		}

		// indicate that the result needs to be freed
		resultfreed=false;

		// run through the columns...
		for (sword i=0; i<ncols; i++) {

			// get the entire column definition
			if (OCIParamGet(stmt,OCI_HTYPE_STMT,
				oracleconn->err,
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
				oracleconn->err)!=OCI_SUCCESS) {
				return false;
			}

			// get the column type
			if (OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].dbtype,(ub4 *)NULL,
				(ub4)OCI_ATTR_DATA_TYPE,
				oracleconn->err)!=OCI_SUCCESS) {
				return false;
			}

			// get the column precision
			if (OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].precision,(ub4 *)NULL,
				(ub4)OCI_ATTR_PRECISION,
				oracleconn->err)!=OCI_SUCCESS) {
				return false;
			}

			// get the column scale
			if (OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].scale,(ub4 *)NULL,
				(ub4)OCI_ATTR_SCALE,
				oracleconn->err)!=OCI_SUCCESS) {
				return false;
			}

			// get whether the column is nullable
			if (OCIAttrGet((dvoid *)desc[i].paramd,
				OCI_DTYPE_PARAM,
				(dvoid *)&desc[i].nullok,(ub4 *)NULL,
				(ub4)OCI_ATTR_IS_NULL,
				oracleconn->err)!=OCI_SUCCESS) {
				return false;
			}

			// is the column a LOB?
			if (desc[i].dbtype==BLOB_TYPE ||
				desc[i].dbtype==CLOB_TYPE ||
				desc[i].dbtype==BFILE_TYPE) {

				// return 0 for the size of lobs
				desc[i].dbsize=0;

				// set the NULL indicators to false
				bytestring::zero(def_indp[i],
						sizeof(sb2)*
						conn->cont->getFetchAtOnce());

				// allocate a lob descriptor
				for (uint32_t j=0;
					j<conn->cont->getFetchAtOnce();
					j++) {
					if (OCIDescriptorAlloc(
						(void *)oracleconn->env,
						(void **)&def_lob[i][j],
						OCI_DTYPE_LOB,0,0)) {
						return false;
					}
				}

				// define the column as a lob
				if (OCIDefineByPos(stmt,&def[i],
					oracleconn->err,
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
					oracleconn->err)!=OCI_SUCCESS) {
					return false;
				}

				// if the column is not a LOB, define it,
				// translated to a NULL terminated string
				if (OCIDefineByPos(stmt,&def[i],
					oracleconn->err,
					i+1,
					(dvoid *)def_buf[i],
					(sb4)conn->cont->getMaxFieldLength(),
					SQLT_STR,
					(dvoid *)def_indp[i],
					(ub2 *)def_col_retlen[i],
					def_col_retcode[i],
					OCI_DEFAULT)!=OCI_SUCCESS) {
					return false;
				}

				// set the lob member to NULL
				for (uint32_t j=0;
					j<conn->cont->getFetchAtOnce();
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
			*db->isnegative=false;
		}
	}

	return true;
}

bool oraclecursor::validBinds() {

	// NOTE: If we're using the statement cache, then it is vital to
	// verify that all variables are bound.  Without the statement cache,
	// OCIStmtExecute would just fail with "ORA-01008: not all variables
	// bound", but when using the statement cache, it will segfault if we
	// get a cache hit.


	// if we're not using the statement cache and we're not rejecting
	// duplicate binds then we can just return
	bool	usingstmtcache=false;
	#ifdef OCI_STMT_CACHE
	if (oracleconn->stmtcachesize) {
		usingstmtcache=true;
	}
	#endif
	if (!usingstmtcache && !oracleconn->rejectduplicatebinds) {
		return true;
	}

	// otherwise we need to validate the binds...

	// get the bind info from the query
	sb4	found;
	sword	ret=OCIStmtGetBindInfo(stmt,oracleconn->err,maxbindcount,
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
		if (oracleconn->rejectduplicatebinds && dupl[i]) {
			stringbuffer	err;
			err.append(SQLR_ERROR_DUPLICATEBINDNAME_STRING);
			err.append(" (")->append(bvnp[i])->append(')');
			conn->cont->setError(this,err.getString(),
					SQLR_ERROR_DUPLICATEBINDNAME,true);
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
				conn->cont->setError(this,
					"ORA-01008: not all variables bound",
								1008,true);
				return false;
			}
		}
	}
	return true;
}

bool oraclecursor::queryIsNotSelect() {
	return (stmttype!=OCI_STMT_SELECT);
}

void oraclecursor::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {

	if (bindformaterror) {

		// handle bind format errors
		*errorlength=charstring::length(
				SQLR_ERROR_INVALIDBINDVARIABLEFORMAT_STRING);
		charstring::safeCopy(errorbuffer,
				errorbufferlength,
				SQLR_ERROR_INVALIDBINDVARIABLEFORMAT_STRING,
				*errorlength);
		*errorcode=SQLR_ERROR_INVALIDBINDVARIABLEFORMAT;
		*liveconnection=true;

	} else {

		// otherwise fall back to default implementation
		sqlrservercursor::errorMessage(errorbuffer,
						errorbufferlength,
						errorlength,
						errorcode,
						liveconnection);
	}

	#ifdef OCI_STMT_CACHE
	// set the statement release mode such that this query will be
	// removed from the statement cache on the next iteration
	if (charstring::length(errorbuffer)) {
		stmtreleasemode=OCI_STRLS_CACHE_DELETE;
	}
	#endif
}

uint64_t oraclecursor::affectedRows() {

	// get the affected row count
	ub4	rows;
	if (OCIAttrGet(stmt,OCI_HTYPE_STMT,
			(dvoid *)&rows,(ub4 *)NULL,
			OCI_ATTR_ROW_COUNT,oracleconn->err)==OCI_SUCCESS) {
		return rows;
	}
	return 0;
}

uint32_t oraclecursor::colCount() {
	return ncols;
}

const char *oraclecursor::getColumnName(uint32_t col) {
	return (const char *)desc[col].buf;
}

uint16_t oraclecursor::getColumnNameLength(uint32_t col) {
	return (uint16_t)desc[col].buflen;
}

uint16_t oraclecursor::getColumnType(uint32_t col) {
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

uint32_t oraclecursor::getColumnLength(uint32_t col) {
	return (uint32_t)desc[col].dbsize;
}

uint32_t oraclecursor::getColumnPrecision(uint32_t col) {
	return (uint32_t)desc[col].precision;
}

uint32_t oraclecursor::getColumnScale(uint32_t col) {
	return (uint32_t)desc[col].scale;
}

uint16_t oraclecursor::getColumnIsNullable(uint32_t col) {
	return (uint16_t)desc[col].nullok;
}

uint16_t oraclecursor::getColumnIsBinary(uint32_t col) {
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

bool oraclecursor::noRowsToReturn() {
	return (stmttype!=OCI_STMT_SELECT);
}

bool oraclecursor::skipRow(bool *error) {
	if (fetchRow(error)) {
		row++;
		return true;
	}
	return false;
}

bool oraclecursor::fetchRow(bool *error) {

	*error=false;

	if (row==conn->cont->getFetchAtOnce()) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return false;
	}
	if (!row) {
		OCIStmtFetch(stmt,oracleconn->err,
				(ub4)conn->cont->getFetchAtOnce(),
				OCI_FETCH_NEXT,OCI_DEFAULT);
		ub4	currentrow;
		OCIAttrGet(stmt,OCI_HTYPE_STMT,
				(dvoid *)&currentrow,(ub4 *)NULL,
				OCI_ATTR_ROW_COUNT,oracleconn->err);
		if (currentrow==totalrows) {
			return false;
		}
		maxrow=currentrow-totalrows;
		totalrows=currentrow;
	}
	return true;
}

void oraclecursor::getField(uint32_t col,
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
	*field=(const char *)&def_buf[col][row*conn->cont->getMaxFieldLength()];
	*fieldlength=def_col_retlen[col][row];
}

void oraclecursor::nextRow() {
	row++;
}

bool oraclecursor::getLobFieldLength(uint32_t col, uint64_t *length) {
	ub4	loblength=0;
	bool	retval=(OCILobGetLength(oracleconn->svc,
				oracleconn->err,
				def_lob[col][row],
				&loblength)==OCI_SUCCESS);
	*length=loblength;
	return retval;
}

bool oraclecursor::getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {

	// initialize the read length to the number of characters to read
	ub4	readlength=charstoread;

	// read from the lob
	// (offset+1 is very important,
	// apparently oracle lob offsets are 1-based)
	sword	result=OCILobRead(oracleconn->svc,
				oracleconn->err,
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

void oraclecursor::closeLobField(uint32_t col) {
	#ifdef HAVE_ORACLE_8i
	// if the lob is temporary, deallocate it
	boolean	templob;
	if (OCILobIsTemporary(oracleconn->env,
				oracleconn->err,
				def_lob[col][row],
				&templob)!=OCI_SUCCESS) {
		return;
	}
	if (templob) {
		OCILobFreeTemporary(oracleconn->svc,
					oracleconn->err,
					def_lob[col][row]);
	}
	#endif
}

void oraclecursor::closeResultSet() {

	// OCI8 version of ocan(), but since it uses OCIStmtFetch we
	// only want to run it if the statement was a select
	if (stmttype==OCI_STMT_SELECT) {
		OCIStmtFetch(stmt,oracleconn->err,0,
				OCI_FETCH_NEXT,OCI_DEFAULT);
	}

	// free row/column resources
	if (!resultfreed) {

		uint32_t	maxcolumncount=conn->cont->getMaxColumnCount();

		int32_t	columncount=(!maxcolumncount)?ncols:maxcolumncount;

		for (int32_t i=0; i<columncount; i++) {

			// free lob resources
			for (uint32_t j=0;
				j<conn->cont->getFetchAtOnce(); j++) {
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
			if (desc[i].paramd) {
				// For some reason, it's not always safe to do
				// this for a cursor that was bound to a result
				// set from a stored procedure.  Sometimes it
				// works but other times it crashes.  This could
				// be an OCI bug.  I'm not sure.  It wish I
				// knew more about exactly when it succeeds or
				// fails, as this leaks memory.
				if (!bound) {
					OCIDescriptorFree(
						(dvoid *)desc[i].paramd,
						OCI_DTYPE_PARAM);
				}
				desc[i].paramd=NULL;
			}
		}

		// deallocate buffers, if necessary
		if (stmttype==OCI_STMT_SELECT && !maxcolumncount) {
			deallocateResultSetBuffers();
		}

		resultfreed=true;
	}

	#ifdef HAVE_ORACLE_8i
	// free lob bind resources
	for (uint16_t i=0; i<orainbindlobcount; i++) {
		OCILobFreeTemporary(oracleconn->svc,
						oracleconn->err,
						inbind_lob[i]);
		OCILobClose(oracleconn->svc,oracleconn->err,
						inbind_lob[i]);
		OCIDescriptorFree(inbind_lob[i],OCI_DTYPE_LOB);
	}
	for (uint16_t i=0; i<oraoutbindlobcount; i++) {
		if (outbind_lob[i]) {
			OCILobFreeTemporary(oracleconn->svc,
						oracleconn->err,
						outbind_lob[i]);
			OCILobClose(oracleconn->svc,
						oracleconn->err,
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
	bindvarcount=0;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_oracleconnection(
						sqlrservercontroller *cont) {
		return new oracleconnection(cont);
	}
}
