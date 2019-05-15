// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <rudiments/dynamiclib.h>
#include <rudiments/file.h>


// FIXME: for now, assume version 8i+
#undef HAVE_ORACLE_8i
#define HAVE_ORACLE_8i	1


// types...
#define CONST const
#define dvoid void
typedef signed int sword;
typedef unsigned char ub1;
typedef unsigned short ub2;
typedef short sb2;
typedef unsigned int ub4;
typedef int sb4;
typedef int boolean;
typedef unsigned char text;
typedef unsigned char OraText;
typedef unsigned short OCIDuration;


// structs...
struct OCIParam;
struct OCIEnv;
struct OCIServer;
struct OCIError;
struct OCISvcCtx;
struct OCISession;
struct OCITrans;
struct OCISession;
struct OCIStmt;
struct OCIDefine;
struct OCILobLocator;
struct OCIBind;
struct OCISnapshot;


// date/time structs need to be defined...
struct OCITime {
	ub1 OCITimeHH;
	ub1 OCITimeMI;
	ub1 OCITimeSS;
};

struct OCIDate {
	sb2 OCIDateYYYY;
	ub1 OCIDateMM;
	ub1 OCIDateDD;
	OCITime OCIDateTime;
};


// function pointers...
sword (*OCIEnvCreate)(OCIEnv **,
				ub4,
				void *,
				void *(*)(void *,size_t),
				void *(*)(void *,void *,size_t),
				void (*)(void *,void *),
				size_t,
				void **);
sword (*OCIInitialize)(ub4,
				void *,
				void *(*)(void *,size_t),
				void *(*)(void *,void *,size_t),
				void (*)(void *,void *));
sword (*OCIHandleAlloc)(const void *,
				void **,
				const ub4,
				const size_t,
				void **);
sword (*OCIServerAttach)(OCIServer *,
				OCIError *,
				const OraText *,
				sb4,
				ub4);
sword (*OCIServerDetach)(OCIServer *,
				OCIError *,
				ub4);
sword (*OCISessionBegin)(OCISvcCtx *,
				OCIError *,
				OCISession *,
				ub4,
				ub4);
sword (*OCISessionEnd)(OCISvcCtx *,
				OCIError *,
				OCISession *,
				ub4);
sword (*OCIServerVersion)(void *,
				OCIError *,
				OraText *,
				ub4,
				ub1);
sword (*OCIStmtPrepare2)(OCISvcCtx *,
				OCIStmt **,
				OCIError *,
				const OraText *,
				ub4,
				const OraText *,
				ub4,
				ub4,
				ub4);
sword (*OCIStmtExecute)(OCISvcCtx *,
				OCIStmt *,
				OCIError *,
				ub4,
				ub4,
				const OCISnapshot *,
				OCISnapshot *,
				ub4);
sword (*OCIStmtRelease)(OCIStmt *,
				OCIError *,
				const OraText *,
				ub4,
				ub4);
sword (*OCIErrorGet)(void *,
				ub4,
				OraText *,
				sb4 *,
				OraText *,
				ub4,
				ub4);
sword (*OCIStmtPrepare)(OCIStmt *,
				OCIError *,
				const OraText *,
				ub4,
				ub4,
				ub4);
sword (*OCIBindByPos)(OCIStmt *,
				OCIBind **,
				OCIError *,
				ub4,
				void *,
				sb4,
				ub2,
				void *,
				ub2 *,
				ub2 *,
				ub4,
				ub4 *,
				ub4);
sword (*OCIBindByName)(OCIStmt *,
				OCIBind **,
				OCIError *,
				const OraText *,
				sb4,
				void *,
				sb4,
				ub2,
				void *,
				ub2 *,
				ub2 *,
				ub4,
				ub4 *,
				ub4);
sword (*OCIDescriptorAlloc)(const void *,
				void **,
				const ub4,
				const size_t,
				void **);
sword (*OCILobCreateTemporary)(OCISvcCtx *,
				OCIError *,
				OCILobLocator *,
				ub2,
				ub1,
				ub1,
				boolean,
				OCIDuration);
sword (*OCILobOpen)(OCISvcCtx *,
				OCIError *,
				OCILobLocator *,
				ub1);
sword (*OCILobFreeTemporary)(OCISvcCtx *,
				OCIError *,
				OCILobLocator *);
sword (*OCILobWrite)(OCISvcCtx *,
				OCIError *,
				OCILobLocator *,
				ub4 *,
				ub4,
				void *,
				ub4,
				ub1,
				void *,
				sb4 (*)(dvoid*,dvoid*,ub4*,ub1 *),
				ub2,
				ub1);
sword (*OCILobClose)(OCISvcCtx *,
				OCIError *,
				OCILobLocator *);
sword (*OCILobGetLength)(OCISvcCtx *,
				OCIError *,
				OCILobLocator *,
				ub4 *);
sword (*OCILobRead)(OCISvcCtx *,
				OCIError *,
				OCILobLocator *,
				ub4 *,
				ub4,
				void *,
				ub4,
				void *,
				sb4 (*)(dvoid *,CONST dvoid *,ub4,ub1),
				ub2,
				ub1);
sword (*OCIAttrSet)(void *,
				ub4,
				void *,
				ub4,
				ub4,
				OCIError *);
sword (*OCIAttrGet)(const void *,
				ub4,
				void *,
				ub4 *,
				ub4,
				OCIError *);
sword (*OCIDefineByPos)(OCIStmt *,
				OCIDefine **,
				OCIError *,
				ub4,
				void *,
				sb4,
				ub2,
				void *,
				ub2 *,
				ub2 *,
				ub4);
sword (*OCIStmtGetBindInfo)(OCIStmt *,
				OCIError *,
				ub4,
				ub4,
				sb4 *,
				OraText *[],
				ub1[],
				OraText *[],
				ub1[],
				ub1[],
				OCIBind **);
sword (*OCIStmtFetch)(OCIStmt *,
				OCIError *,
				ub4,
				ub2,
				ub4);
sword (*OCILobIsTemporary)(OCIEnv *,
				OCIError *,
				OCILobLocator *locp,
				boolean *);
sword (*OCIDescriptorFree)(void *descp,
				const ub4);
sword (*OCIHandleFree)(void *,
				const ub4);
sword (*OCIParamGet)(const void *,
				ub4,
				OCIError *,
				void **,
				ub4);
sword (*OCITransCommit)(OCISvcCtx *,
				OCIError *,
				ub4);
sword (*OCITransRollback)(OCISvcCtx *,
				OCIError *,
				ub4);


// date/time macros...
#define OCIDateSetDate(date,year,month,day) { \
	(date)->OCIDateYYYY=year; \
	(date)->OCIDateMM=month; \
	(date)->OCIDateDD=day; \
}
#define OCIDateSetTime(date,hour,min,sec) { \
	(date)->OCIDateTime.OCITimeHH=hour; \
	(date)->OCIDateTime.OCITimeMI=min; \
	(date)->OCIDateTime.OCITimeSS=sec; \
}
#define OCIDateGetDate(date,year,month,day) { \
	*year=(date)->OCIDateYYYY; \
	*month=(date)->OCIDateMM; \
	*day=(date)->OCIDateDD; \
}
#define OCIDateGetTime(date,hour,min,sec) { \
	*hour=(date)->OCIDateTime.OCITimeHH; \
	*min=(date)->OCIDateTime.OCITimeMI; \
	*sec=(date)->OCIDateTime.OCITimeSS; \
}


// constants...
#define OCI_DEFAULT		0x00000000
#define OCI_SUCCESS		0
#define OCI_SUCCESS_WITH_INFO	1
#define OCI_ERROR		-1
#define OCI_INVALID_HANDLE	-2

#define OCI_OBJECT		0x00000002

#define OCI_HTYPE_ENV		1
#define OCI_HTYPE_ERROR		2
#define OCI_HTYPE_SVCCTX	3
#define OCI_HTYPE_STMT		4
#define OCI_HTYPE_SERVER	8
#define OCI_HTYPE_SESSION	9
#define OCI_HTYPE_TRANS		10

#define OCI_ATTR_DATA_SIZE	1
#define OCI_ATTR_DATA_TYPE	2
#define OCI_ATTR_NAME		4
#define OCI_ATTR_PRECISION	5
#define OCI_ATTR_SCALE		6
#define OCI_ATTR_IS_NULL	7
#define OCI_ATTR_TRANS		8
#define OCI_ATTR_ROW_COUNT	9
#define OCI_ATTR_PREFETCH_ROWS	11
#define OCI_ATTR_PARAM_COUNT	18
#define OCI_ATTR_USERNAME	22
#define OCI_ATTR_PASSWORD	23
#define OCI_ATTR_NOCACHE	87
#define OCI_ATTR_STMTCACHESIZE	176

#define OCI_ATTR_SERVER		6
#define OCI_ATTR_SESSION	7
#define OCI_ATTR_STMT_TYPE	24

#define OCI_CRED_RDBMS	1
#define OCI_CRED_EXT	2
#define OCI_CRED_PROXY	3

// FIXME: for now, assume this is supported...
#define OCI_ATTR_PROXY_CREDENTIALS	99

#define OCI_NTV_SYNTAX	1

#define OCI_COMMIT_ON_SUCCESS	0x00000020

#define OCI_STRLS_CACHE_DELETE	0x0010

// FIXME: for now, assume this is supported...
#define OCI_STMT_CACHE	0x00000040

#define OCI_STMT_SELECT	1
#define OCI_STMT_CREATE	5
#define OCI_STMT_DROP	6
#define OCI_STMT_ALTER	7

#define OCI_PREP2_CACHE_SEARCHONLY	0x0010

#define OCI_TEMP_BLOB	1
#define OCI_TEMP_CLOB	2

#define OCI_DTYPE_LOB	50
#define OCI_DTYPE_PARAM	53

#define OCI_DURATION_SESSION	10

#define OCI_LOB_READWRITE	2

#define OCI_ONE_PIECE	0

#define OCI_NO_DATA	100

#define OCI_STRLS_CACHE_DELETE	0x0010

#define OCI_FETCH_NEXT	0x00000002

#define SQLT_FLT	4
#define SQLT_STR	5
#define SQLT_CLOB	112
#define SQLT_BLOB	113
#define SQLT_RSET	116
#define SQLT_ODT	156

#define SQLCS_IMPLICIT	1


// dlopen infrastructure...
static bool		alreadyopen=false;
static dynamiclib	lib;

static bool loadLibraries(stringbuffer *errormessage) {

	// don't open multiple times...
	if (alreadyopen) {
		return true;
	}
	alreadyopen=true;

	// build lib names
	const char	**libnames=new const char *[11];
	uint16_t	p=0;
	stringbuffer	libdir;
	const char	*oraclehome=environment::getValue("ORACLE_HOME");
	if (!charstring::isNullOrEmpty(oraclehome)) {
		libdir.append(oraclehome)->append("/lib/libclntsh.so");
		libnames[p++]=libdir.getString();
	}

	if (sizeof(long)==8) {
		libnames[p++]="/usr/lib/oracle/12.1/client64/lib/libclntsh.so";
	} else {
		libnames[p++]="/usr/lib/oracle/12.1/client/lib/libclntsh.so";
	}
	libnames[p++]="/opt/instantclient_12_1/libclntsh.so.12.1";
	libnames[p++]="/usr/local/instantclient_12_1/libclntsh.so.12.1";

	if (sizeof(long)==8) {
		libnames[p++]="/usr/lib/oracle/11.2/client64/lib/libclntsh.so";
	} else {
		libnames[p++]="/usr/lib/oracle/11.2/client/lib/libclntsh.so";
	}
	libnames[p++]="/opt/instantclient_11_2/libclntsh.so.11.2";
	libnames[p++]="/usr/local/instantclient_11_2/libclntsh.so.11.2";

	if (sizeof(long)==8) {
		libnames[p++]="/usr/lib/oracle/10.2/client64/lib/libclntsh.so";
	} else {
		libnames[p++]="/usr/lib/oracle/10.2/client/lib/libclntsh.so";
	}
	libnames[p++]="/opt/instantclient_10_2/libclntsh.so.10.2";
	libnames[p++]="/usr/local/instantclient_10_2/libclntsh.so.10.2";

	libnames[p++]=NULL;

	// look for the library
	const char	**libname=libnames;
	while (*libname) {
		if (file::readable(*libname)) {
			break;
		}
		libname++;
	}
	if (!*libname) {
		errormessage->clear();
		errormessage->append("\nFailed to load Oracle libraries.\n");
		if (charstring::isNullOrEmpty(oraclehome)) {
			errormessage->append("ORACLE_HOME not set and none");
		} else {
			errormessage->append("None");
		}
		errormessage->append(" of these libraries were found:\n");
		libname=libnames;
		while (*libname) {
			errormessage->append('	');
			errormessage->append(*libname)->append('\n');
			libname++;
		}
		return false;
	}

	// open the library
	if (!lib.open(*libname,true,true)) {
		goto error;
	}

	// get the functions we need
	OCIEnvCreate=(sword (*)(OCIEnv **,
					ub4,
					void *,
					void *(*)(void *,size_t),
					void *(*)(void *,void *,size_t),
					void (*)(void *,void *),
					size_t,
					void **))
				lib.getSymbol("OCIEnvCreate");
	if (!OCIEnvCreate) {
		goto error;
	}

	OCIInitialize=(sword (*)(ub4,
					void *,
					void *(*)(void *,size_t),
					void *(*ralocfp)(void *,void *,size_t),
					void (*)(void *,void *)))
				lib.getSymbol("OCIInitialize");
	if (!OCIInitialize) {
		goto error;
	}

	OCIHandleAlloc=(sword (*)(const void *,
					void **,
					const ub4,
					const size_t,
					void **))
				lib.getSymbol("OCIHandleAlloc");
	if (!OCIHandleAlloc) {
		goto error;
	}

	OCIServerAttach=(sword (*)(OCIServer *,
					OCIError *,
					const OraText *,
					sb4,
					ub4))
				lib.getSymbol("OCIServerAttach");
	if (!OCIServerAttach) {
		goto error;
	}

	OCIServerDetach=(sword (*)(OCIServer *,
					OCIError *,
					ub4))
				lib.getSymbol("OCIServerDetach");
	if (!OCIServerDetach) {
		goto error;
	}

	OCISessionBegin=(sword (*)(OCISvcCtx *,
					OCIError *,
					OCISession *,
					ub4,
					ub4))
				lib.getSymbol("OCISessionBegin");
	if (!OCISessionBegin) {
		goto error;
	}

	OCISessionEnd=(sword (*)(OCISvcCtx *,
					OCIError *,
					OCISession *,
					ub4))
				lib.getSymbol("OCISessionEnd");
	if (!OCISessionEnd) {
		goto error;
	}

	OCIServerVersion=(sword (*)(void *,
					OCIError *,
					OraText *,
					ub4,
					ub1))
				lib.getSymbol("OCIServerVersion");
	if (!OCIServerVersion) {
		goto error;
	}

	OCIStmtPrepare2=(sword (*)(OCISvcCtx *,
					OCIStmt **,
					OCIError *,
					const OraText *,
					ub4,
					const OraText *,
					ub4,
					ub4,
					ub4))
				lib.getSymbol("OCIStmtPrepare2");
	if (!OCIStmtPrepare2) {
		goto error;
	}

	OCIStmtExecute=(sword (*)(OCISvcCtx *,
					OCIStmt *,
					OCIError *,
					ub4,
					ub4,
					const OCISnapshot *,
					OCISnapshot *,
					ub4))
				lib.getSymbol("OCIStmtExecute");
	if (!OCIStmtExecute) {
		goto error;
	}

	OCIStmtRelease=(sword (*)(OCIStmt *,
					OCIError *,
					const OraText *,
					ub4,
					ub4))
				lib.getSymbol("OCIStmtRelease");
	if (!OCIStmtRelease) {
		goto error;
	}

	OCIErrorGet=(sword (*)(void *,
					ub4,
					OraText *,
					sb4 *,
					OraText *,
					ub4,
					ub4))
				lib.getSymbol("OCIErrorGet");
	if (!OCIErrorGet) {
		goto error;
	}

	OCIStmtPrepare=(sword (*)(OCIStmt *,
					OCIError *,
					const OraText *,
					ub4,
					ub4,
					ub4))
				lib.getSymbol("OCIStmtPrepare");
	if (!OCIStmtPrepare) {
		goto error;
	}

	OCIBindByPos=(sword (*)(OCIStmt *,
					OCIBind **,
					OCIError *,
					ub4,
					void *,
					sb4,
					ub2,
					void *,
					ub2 *,
					ub2 *,
					ub4,
					ub4 *,
					ub4))
				lib.getSymbol("OCIBindByPos");
	if (!OCIBindByPos) {
		goto error;
	}

	OCIBindByName=(sword (*)(OCIStmt *,
					OCIBind **,
					OCIError *,
					const OraText *,
					sb4,
					void *,
					sb4,
					ub2,
					void *p,
					ub2 *,
					ub2 *,
					ub4,
					ub4 *,
					ub4))
				lib.getSymbol("OCIBindByName");
	if (!OCIBindByName) {
		goto error;
	}

	OCIDescriptorAlloc=(sword (*)(const void *,
					void **,
					const ub4,
					const size_t,
					void **))
				lib.getSymbol("OCIDescriptorAlloc");
	if (!OCIDescriptorAlloc) {
		goto error;
	}

	OCILobCreateTemporary=(sword (*)(OCISvcCtx *,
					OCIError *,
					OCILobLocator *,
					ub2,
					ub1,
					ub1,
					boolean,
					OCIDuration))
				lib.getSymbol("OCILobCreateTemporary");
	if (!OCILobCreateTemporary) {
		goto error;
	}

	OCILobOpen=(sword (*)(OCISvcCtx *,
					OCIError *,
					OCILobLocator *,
					ub1))
				lib.getSymbol("OCILobOpen");
	if (!OCILobOpen) {
		goto error;
	}

	OCILobFreeTemporary=(sword (*)(OCISvcCtx *,
					OCIError *,
					OCILobLocator *))
				lib.getSymbol("OCILobFreeTemporary");
	if (!OCILobFreeTemporary) {
		goto error;
	}

	OCILobWrite=(sword (*)(OCISvcCtx *,
					OCIError *,
					OCILobLocator *,
					ub4 *,
					ub4,
					void *,
					ub4,
					ub1,
					void *,
					sb4 (*)(dvoid*,dvoid*,ub4*,ub1 *),
					ub2,
					ub1))
				lib.getSymbol("OCILobWrite");
	if (!OCILobWrite) {
		goto error;
	}

	OCILobClose=(sword (*)(OCISvcCtx *,
					OCIError *,
					OCILobLocator *))
				lib.getSymbol("OCILobClose");
	if (!OCILobClose) {
		goto error;
	}

	OCILobGetLength=(sword (*)(OCISvcCtx *,
					OCIError *,
					OCILobLocator *,
					ub4 *))
				lib.getSymbol("OCILobGetLength");
	if (!OCILobGetLength) {
		goto error;
	}

	OCILobRead=(sword (*)(OCISvcCtx *,
					OCIError *,
					OCILobLocator *,
					ub4 *,
					ub4,
					void *,
					ub4,
					void *,
					sb4 (*)(dvoid *,CONST dvoid *,ub4,ub1),
					ub2,
					ub1))
				lib.getSymbol("OCILobRead");
	if (!OCILobRead) {
		goto error;
	}

	OCIAttrSet=(sword (*)(void *,
					ub4,
					void *,
					ub4,
					ub4,
					OCIError *))
				lib.getSymbol("OCIAttrSet");
	if (!OCIAttrSet) {
		goto error;
	}

	OCIAttrGet=(sword (*)(const void *,
					ub4,
					void *,
					ub4 *,
					ub4,
					OCIError *))
				lib.getSymbol("OCIAttrGet");
	if (!OCIAttrGet) {
		goto error;
	}

	OCIDefineByPos=(sword (*)(OCIStmt *,
					OCIDefine **,
					OCIError *,
					ub4,
					void *,
					sb4,
					ub2,
					void *,
					ub2 *,
					ub2 *,
					ub4))
				lib.getSymbol("OCIDefineByPos");
	if (!OCIDefineByPos) {
		goto error;
	}

	OCIStmtGetBindInfo=(sword (*)(OCIStmt *,
					OCIError *,
					ub4,
					ub4,
					sb4 *,
					OraText *[],
					ub1[],
					OraText *[],
					ub1[],
					ub1[],
					OCIBind **))
				lib.getSymbol("OCIStmtGetBindInfo");
	if (!OCIStmtGetBindInfo) {
		goto error;
	}

	OCIStmtFetch=(sword (*)(OCIStmt *,
					OCIError *,
					ub4,
					ub2,
					ub4))
				lib.getSymbol("OCIStmtFetch");
	if (!OCIStmtFetch) {
		goto error;
	}

	OCILobIsTemporary=(sword (*)(OCIEnv *,
					OCIError *,
					OCILobLocator *,
					boolean *))
				lib.getSymbol("OCILobIsTemporary");
	if (!OCILobIsTemporary) {
		goto error;
	}

	OCIDescriptorFree=(sword (*)(void *,
					const ub4))
				lib.getSymbol("OCIDescriptorFree");
	if (!OCIDescriptorFree) {
		goto error;
	}

	OCIHandleFree=(sword (*)(void *,
					const ub4))
				lib.getSymbol("OCIHandleFree");
	if (!OCIHandleFree) {
		goto error;
	}

	OCIParamGet=(sword (*)(const void *,
					ub4,
					OCIError *,
					void **,
					ub4))
				lib.getSymbol("OCIParamGet");
	if (!OCIParamGet) {
		goto error;
	}

	OCITransCommit=(sword (*)(OCISvcCtx *,
					OCIError *,
					ub4))
				lib.getSymbol("OCITransCommit");
	if (!OCITransCommit) {
		goto error;
	}

	OCITransRollback=(sword (*)(OCISvcCtx *,
					OCIError *,
					ub4))
				lib.getSymbol("OCITransRollback");
	if (!OCITransRollback) {
		goto error;
	}

	// success
	return true;

	// error
error:
	char	*error=lib.getError();
	errormessage->clear();
	errormessage->append("\nFailed to load Oracle libraries.\n");
	errormessage->append(error)->append('\n');
	#ifndef _WIN32
	if (charstring::contains(error,"No such file or directory")) {
		char		*path=file::dirname(*libname);
		const char	*lib=(*libname)+charstring::length(path)+1;
		errormessage->append("\n(NOTE: The error message above may "
					"be misleading.  Most likely it means "
					"that a library that ");
		errormessage->append(lib);
		errormessage->append(" depends on could not be located.  ");
		errormessage->append(lib)->append(" was found in ");
		errormessage->append(path)->append(".  Verify that ");
		errormessage->append(path);
		errormessage->append(" and directories containing each of "
					"the libraries that ");
		errormessage->append(lib);
		errormessage->append(" depends on are included in your "
					"LD_LIBRARY_PATH, /etc/ld.so.conf, "
					"or /etc/ld.so.conf.d.  Try using "
					"ldd to show ");
		errormessage->append(path)->append('/')->append(lib);
		errormessage->append("'s dependencies.)\n");
		delete[] path;
	}
	#endif
	delete[] error;
	return false;
}
