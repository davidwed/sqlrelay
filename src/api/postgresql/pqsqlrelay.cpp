/*
Portions Copyright (c) 1996-2002, The PostgreSQL Global Development Group
 
Portions Copyright (c) 1994, The Regents of the University of California

Portions Copyright (c) 2003-2004, David Muse

Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without a written agreement is hereby granted, provided that the above copyright notice and this paragraph and the following two paragraphs appear in all copies.
 
IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <rudiments/parameterstring.h>
#include <rudiments/environment.h>
#include <rudiments/character.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#define NEED_DATATYPESTRING	1
#include <datatypes.h>

// for fprintf
#include <stdio.h>

// for malloc/calloc/free
#include <stdlib.h>

extern "C" {

#define TRUE	1
#define FALSE	0

typedef unsigned int Oid;
typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;
typedef struct pg_cancel PGcancel;
typedef void (*PQnoticeProcessor) (void *arg, const char *message);

typedef enum {
	PGRES_EMPTY_QUERY = 0,
	PGRES_COMMAND_OK,
	PGRES_TUPLES_OK,
	PGRES_COPY_OUT,
	PGRES_COPY_IN,
	PGRES_BAD_RESPONSE,
	PGRES_NONFATAL_ERROR,
	PGRES_FATAL_ERROR
} ExecStatusType;

typedef enum {
	PQTRANS_IDLE,
	PQTRANS_ACTIVE,
	PQTRANS_INTRANS,
	PQTRANS_INERROR,
	PQTRANS_UNKNOWN
} PGTransactionStatusType;

typedef enum {
	PQERRORS_TERSE,
	PQERRORS_DEFAULT,
	PQERRORS_VERBOSE
} PGVerbosity;

struct pg_conn;

struct pg_result {
	sqlrcursor	*sqlrcur;
	ExecStatusType	execstatus;
	pg_conn		*conn;
	int		previousnonblockingmode;
	int		queryisnotselect;
};

struct pg_conn {

	sqlrconnection	*sqlrcon;
	sqlrcursor	*sqlrcur;

	parameterstring	*connstr;

	char		*conninfo;

	char		*host;
	char		*port;
	char		*options;
	char		*tty;
	char		*db;
	char		*user;
	char		*password;

	int		clientencoding;

	pg_result	*currentresult;
	int		nonblockingmode;

	PQnoticeProcessor	noticeprocessor;
	void			*noticeprocessorarg;

	char		*error;

	int		removetrailingsemicolons;

	PGVerbosity	errorverbosity;
};

struct pg_cancel {
	void	*extension;
};

// encodings
typedef enum pg_enc {
	PG_SQL_ASCII = 0,			/* SQL/ASCII */
	PG_EUC_JP,				/* EUC for Japanese */
	PG_EUC_CN,				/* EUC for Chinese */
	PG_EUC_KR,				/* EUC for Korean */
	PG_EUC_TW,				/* EUC for Taiwan */
	PG_EUC_JIS_2004,			/* EUC-JIS-2004 */
	PG_UTF8,				/* Unicode UTF8 */
	PG_MULE_INTERNAL,			/* Mule internal code */
	PG_LATIN1,				/* ISO-8859-1 Latin 1 */
	PG_LATIN2,				/* ISO-8859-2 Latin 2 */
	PG_LATIN3,				/* ISO-8859-3 Latin 3 */
	PG_LATIN4,				/* ISO-8859-4 Latin 4 */
	PG_LATIN5,				/* ISO-8859-9 Latin 5 */
	PG_LATIN6,				/* ISO-8859-10 Latin6 */
	PG_LATIN7,				/* ISO-8859-13 Latin7 */
	PG_LATIN8,				/* ISO-8859-14 Latin8 */
	PG_LATIN9,				/* ISO-8859-15 Latin9 */
	PG_LATIN10,				/* ISO-8859-16 Latin10 */
	PG_WIN1256,				/* windows-1256 */
	PG_WIN1258,				/* Windows-1258 */
	PG_WIN866,				/* (MS-DOS CP866) */
	PG_WIN874,				/* windows-874 */
	PG_KOI8R,				/* KOI8-R */
	PG_WIN1251,				/* windows-1251 */
	PG_WIN1252,				/* windows-1252 */
	PG_ISO_8859_5,				/* ISO-8859-5 */
	PG_ISO_8859_6,				/* ISO-8859-6 */
	PG_ISO_8859_7,				/* ISO-8859-7 */
	PG_ISO_8859_8,				/* ISO-8859-8 */
	PG_WIN1250,				/* windows-1250 */
	PG_WIN1253,				/* windows-1253 */
	PG_WIN1254,				/* windows-1254 */
	PG_WIN1255,				/* windows-1255 */
	PG_WIN1257,				/* windows-1257 */
	PG_KOI8U,				/* KOI8-U */
	/* PG_ENCODING_BE_LAST points to the above entry */

	/* followings are for client encoding only */
	PG_SJIS,				/* Shift JIS (Windows-932) */
	PG_BIG5,				/* Big5 (Windows-950) */
	PG_GBK,					/* GBK (Windows-936) */
	PG_UHC,					/* UHC (Windows-949) */
	PG_GB18030,				/* GB18030 */
	PG_JOHAB,				/* EUC for Korean JOHAB */
	PG_SHIFT_JIS_2004,			/* Shift-JIS-2004 */
	_PG_LAST_ENCODING_			/* mark only */
} pg_enc;

#define PG_ENCODING_BE_LAST PG_KOI8U

static const char *pg_enc_str[]={
	"SQL_ASCII",
	"EUC_JP",
	"EUC_CN",
	"EUC_KR",
	"EUC_TW",
	"EUC_JIS_2004",
	"UTF8",
	"MULE_INTERNAL",
	"LATIN1",
	"LATIN2",
	"LATIN3",
	"LATIN4",
	"LATIN5",
	"LATIN6",
	"LATIN7",
	"LATIN8",
	"LATIN9",
	"LATIN10",
	"WIN1256",
	"WIN1258",
	"WIN866",
	"WIN874",
	"KOI8R",
	"WIN1251",
	"WIN1252",
	"ISO_8859_5",
	"ISO_8859_6",
	"ISO_8859_7",
	"ISO_8859_8",
	"WIN1250",
	"WIN1253",
	"WIN1254",
	"WIN1255",
	"WIN1257",
	"KOI8U",
	"SJIS",
	"BIG5",
	"GBK",
	"UHC",
	"GB18030",
	"JOHAB",
	"SHIFT_JIS_2004",
	NULL
};

// object id's
#define InvalidOid	0


typedef enum {
	CONNECTION_OK,
	CONNECTION_BAD,
	CONNECTION_STARTED,
	CONNECTION_MADE,
	CONNECTION_AWAITING_RESPONSE,
	CONNECTION_AUTH_OK,
	CONNECTION_SETENV
} ConnStatusType;

static void defaultNoticeProcessor(void *arg, const char *message) {
	debugFunction();
	stderror.printf("%s",message);
}

PGconn *allocatePGconn(const char *conninfo,
				const char *host, const char *port,
				const char *options, const char *tty,
				const char *db, const char *user,
				const char *password) {
	debugFunction();

	PGconn	*conn=new PGconn;

	conn->sqlrcon=NULL;
	conn->sqlrcur=NULL;

	conn->conninfo=(char *)conninfo;
	if (conninfo) {

		conn->connstr=new parameterstring();
		conn->connstr->setDelimiter(' ');
		conn->connstr->parse(conninfo);

		const char	*tmphost=conn->connstr->getValue("host");
		conn->host=charstring::duplicate((tmphost)?tmphost:"");
		const char	*tmpport=conn->connstr->getValue("port");
		// if port is passed in via conninfo, an empty port is NOT
		// translated to 5432
		conn->port=charstring::duplicate((tmpport)?tmpport:"");
		const char	*tmpoptions=conn->connstr->getValue("options");
		conn->options=charstring::duplicate((tmpoptions)?tmpoptions:"");
		const char	*tmptty=conn->connstr->getValue("tty");
		conn->tty=charstring::duplicate((tmptty)?tmptty:"");
		const char	*tmpdbname=conn->connstr->getValue("dbname");
		conn->db=charstring::duplicate((tmpdbname)?tmpdbname:"");
		const char	*tmpuser=conn->connstr->getValue("user");
		conn->user=charstring::duplicate((tmpuser)?tmpuser:"");
		const char	*tmppassword=conn->connstr->
						getValue("password");
		conn->password=charstring::duplicate((tmppassword)?
							tmppassword:"");

	} else {

		conn->connstr=NULL;

		conn->host=charstring::duplicate((host)?host:"");
		// if port is passed in via parameters, an empty port is
		// translated to 5432
		conn->port=charstring::duplicate((port)?port:"5432");
		conn->options=charstring::duplicate((options)?options:"");
		conn->tty=charstring::duplicate((tty)?tty:"");
		conn->db=charstring::duplicate((db)?db:"");
		conn->user=charstring::duplicate((user)?user:"");
		conn->password=charstring::duplicate((password)?password:"");
	}

	conn->clientencoding=PG_UTF8;

	conn->currentresult=NULL;
	conn->nonblockingmode=0;

	conn->noticeprocessor=defaultNoticeProcessor;
	conn->noticeprocessorarg=(void *)NULL;

	conn->error=NULL;

	int	portnumber=charstring::toInteger(conn->port);

	conn->sqlrcon=new sqlrconnection((portnumber)?conn->host:"",
					portnumber,
					(!portnumber)?conn->port:"",
					conn->user,conn->password,0,1,true);

	conn->removetrailingsemicolons=-1;

	conn->errorverbosity=PQERRORS_DEFAULT;

	conn->sqlrcon->selectDatabase(conn->db);

	return conn;
}

void freePGconn(PGconn *conn) {
	debugFunction();

	if (!conn) {
		return;
	}

	delete conn->sqlrcon;
	conn->sqlrcon=NULL;
	conn->sqlrcur=NULL;

	if (conn->conninfo) {
		delete conn->connstr;
		conn->connstr=NULL;
		conn->conninfo=NULL;
	}
	delete[] conn->host;
	delete[] conn->port;
	delete[] conn->options;
	delete[] conn->tty;
	delete[] conn->db;
	delete[] conn->user;
	delete[] conn->password;

	conn->currentresult=NULL;
	conn->nonblockingmode=0;

	delete[] conn->error;

	delete conn;
}

PGconn *PQsetdbLogin(const char *host, const char *port,
			 const char *options, const char *tty,
			 const char *db,
			 const char *user, const char *password) {
	debugFunction();
	return allocatePGconn(NULL,host,port,options,tty,db,user,password);
}

PGconn *PQsetdb(const char *host, const char *port,
			 const char *options, const char *tty,
			 const char *db) {
	debugFunction();
	return PQsetdbLogin(host,port,options,tty,db,NULL,NULL);
}

PGconn *PQconnectdb(const char *conninfo) {
	debugFunction();
	return allocatePGconn(conninfo,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

PGconn *PQconnectdbParams(const char **keywords,
				const char **values,
				int expanddbname) {
	debugFunction();

	const char *host=NULL;
	const char *port=NULL;
	const char *options=NULL;
	const char *tty=NULL;
	const char *db=NULL;
	const char *user=NULL;
	const char *password=NULL;

	for (uint16_t i=0; keywords[i]; i++) {
		debugPrintf("%s=%s\n",keywords[i],values[i]);
		if (!charstring::compare(keywords[i],"host")) {
			host=values[i];
		} else if (!charstring::compare(keywords[i],"hostaddr")) {
			host=values[i];
		} else if (!charstring::compare(keywords[i],"port")) {
			port=values[i];
		} else if (!charstring::compare(keywords[i],"options")) {
			options=values[i];
		} else if (!charstring::compare(keywords[i],"tty")) {
			tty=values[i];
		} else if (!charstring::compare(keywords[i],"dbname")) {
			db=values[i];
		} else if (!charstring::compare(keywords[i],"user")) {
			user=values[i];
		} else if (!charstring::compare(keywords[i],"password")) {
			password=values[i];
		}
	}

	return allocatePGconn(NULL,host,port,options,tty,db,user,password);
}

void PQfinish(PGconn *conn) {
	debugFunction();
	freePGconn(conn);
}

void PQreset(PGconn *conn) {
	debugFunction();
	conn->sqlrcon->endSession();
}

char *PQdb(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->db);
	return conn->db;
}

char *PQuser(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->user);
	return conn->user;
}

char *PQpass(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->password);
	return conn->password;
}

char *PQhost(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->host);
	return conn->host;
}

char *PQport(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->port);
	return conn->port;
}

char *PQtty(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->tty);
	return conn->tty;
}

char *PQoptions(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->options);
	return conn->options;
}

ConnStatusType PQstatus(const PGconn *conn) {
	debugFunction();
	return CONNECTION_OK;
}

PGVerbosity PQsetErrorVerbosity(PGconn *conn, PGVerbosity verbosity) {
	PGVerbosity	retval=conn->errorverbosity;
	conn->errorverbosity=verbosity;
	return retval;
}

char *PQerrorMessage(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->error);
	return (conn->error)?conn->error:(char *)"";
}

int PQsocket(const PGconn *conn) {
	debugFunction();
	return -1;
}

int PQbackendPID(const PGconn *conn) {
	debugFunction();
	return -1;
}

unsigned long PQgetssl(PGconn *conn) {
	debugFunction();
	return 0;
}

int PQclientEncoding(const PGconn *conn) {
	debugFunction();
	return conn->clientencoding;
}

int pg_char_to_encoding(const char *encoding) {
	debugFunction();
	uint32_t i=0; 
	while (pg_enc_str[i]) {
		if (!charstring::compare(encoding,pg_enc_str[i])) {
			return i;
		}
		i++;
	}
	return -1;
}

const char *pg_encoding_to_char(int encoding) {
	debugFunction();
	return (encoding<(int)_PG_LAST_ENCODING_)?pg_enc_str[encoding]:"";
}

int pg_valid_server_encoding_id(int encoding) {
	debugFunction();
	return (encoding<=PG_ENCODING_BE_LAST)?1:0;
}

int PQsetClientEncoding(PGconn *conn, const char *encoding) {
	debugFunction();
	int	enc=pg_char_to_encoding(encoding);
	if (enc>-1) {
		conn->clientencoding=enc;
		return 0;
	}
	return -1;
}

PQnoticeProcessor PQsetNoticeProcessor(PGconn *conn,
					 PQnoticeProcessor proc,
					 void *arg) {
	debugFunction();

	PQnoticeProcessor	oldprocessor=conn->noticeprocessor;

	conn->noticeprocessor=proc;
	conn->noticeprocessorarg=arg;

	return oldprocessor;
}

const char *PQparameterStatus(const PGconn *conn, const char *paramname) {
	debugFunction();

	if (!conn) {
		return NULL;
	}

	// Ideally we'd return the correct values but for now,
	// just return safe-ish values
	if (!charstring::compare(paramname,"server_version")) {
		return "80100";
	} else if (!charstring::compare(paramname,"server_encoding")) {
		return NULL;
	} else if (!charstring::compare(paramname,"client_encoding")) {
		return pg_encoding_to_char(conn->clientencoding);
	} else if (!charstring::compare(paramname,"is_superuser")) {
		return NULL;
	} else if (!charstring::compare(paramname,"session_authorization")) {
		return NULL;
	} else if (!charstring::compare(paramname,"DateStyle")) {
		// psycopg2 will run "SET DATETYPE TO 'ISO'" if this
		// doesn't return ISO, which causes problems for
		// non-postgresql databases
		return "ISO";
	} else if (!charstring::compare(paramname,"IntervalStyle")) {
		return NULL;
	} else if (!charstring::compare(paramname,"TimeZone")) {
		return NULL;
	} else if (!charstring::compare(paramname,"integer_datetimes")) {
		return NULL;
	} else if (!charstring::compare(paramname,
					"standard_conforming_strings")) {
		return NULL;
	}
	return NULL;
}

int PQserverVersion() {
	return 80100;
}

typedef struct _PQconninfoOption {
	char	*keyword;
	char	*envvar;
	char	*compiled;
	char	*val;
	char	*label;
	char	*dispchar;
	int	dispsize;
} PQconninfoOption;

PQconninfoOption *PQconninfoParse(const char *conninfo, char **errmsg) {
	debugFunction();
	return NULL;
}

PQconninfoOption *PQconndefaults(void) {
	debugFunction();
	return NULL;
}

void PQconninfoFree(PQconninfoOption *connOptions) {
	debugFunction();
}

// lame... I stole this code from sqlrservercontroller
char *skipWhitespaceAndComments(const char *querybuffer) {
	debugFunction();

	// scan the query, bypassing whitespace and comments.
	char	*ptr=(char *)querybuffer;
	while (*ptr && 
		(*ptr==' ' || *ptr=='\n' || *ptr=='	' || *ptr=='-')) {

		// skip any comments
		if (*ptr=='-') {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
		}
		ptr++;
	}
	return ptr;
}

// lame... I stole this code from sqlrservercursor
int queryIsNotSelect(const char *querybuffer) {
	debugFunction();

	// scan the query, bypassing whitespace and comments.
	char	*ptr=skipWhitespaceAndComments(querybuffer);

	// if the query is a select but not a select into then return false,
	// otherwise return true
	if (!charstring::compareIgnoringCase(ptr,"select",6) && 
		charstring::compareIgnoringCase(ptr,"select into ",12)) {
		return 0;
	}
	return 1;
}

PGresult *PQmakeEmptyPGresult(PGconn *conn, ExecStatusType status) {
	debugFunction();
	// Make an empty PGresult with given status (some apps find this
	// useful). If conn is not NULL and status indicates an error, the
	// conn's errorMessage is copied.
	PGresult	*result=new PGresult;
	result->sqlrcur=NULL;
	result->execstatus=status;
	result->conn=conn;
	result->previousnonblockingmode=conn->nonblockingmode;
	result->queryisnotselect=1;
	return result;
}

void PQclear(PGresult *res) {
	debugFunction();
	if (res) {
		res->conn->nonblockingmode=res->previousnonblockingmode;
		delete res->sqlrcur;
		delete res;
	}
}

PGresult *PQprepare(PGconn *conn,
			const char *stmtname,
			const char *query,
			int paramcount,
			const Oid *paramtypes) {
	debugFunction();
	debugPrintf("%s\n",query);

	PGresult	*result=PQmakeEmptyPGresult(conn,PGRES_EMPTY_QUERY);

	delete[] conn->error;
	conn->error=NULL;

	if (!charstring::isNullOrEmpty(query)) {

		conn->sqlrcur=new sqlrcursor(conn->sqlrcon,true);

		if (conn->removetrailingsemicolons==-1) {

			const char	*dbtype=conn->sqlrcon->identify();
			if (!dbtype) {
				charstring::printf(&conn->error,"%s\n",
						conn->sqlrcur->errorMessage());
				PQclear(result);
				return NULL;
			}

			// FIXME: it's possible that other non-postgresql
			// databases allow trailing semicolons
			conn->removetrailingsemicolons=
				(!charstring::compare(dbtype,"postgresql"))?0:1;
		}

		size_t	length=charstring::length(query);
		if (conn->removetrailingsemicolons==1) {
			while (character::inSet(query[length-1]," \t\n\r;")) {
				length--;
			}
		}

		conn->sqlrcur->prepareQuery(query,length);

		if (queryIsNotSelect(query)) {
			result->execstatus=PGRES_COMMAND_OK;
		} else {
			result->execstatus=PGRES_TUPLES_OK;
			result->queryisnotselect=0;
		}
	}

	// Copy the result so it can be passed on to PQexecPrepared.
	// We need to copy it because the app might delete the result
	// returned from this method.
	if (conn->currentresult) {
		PQclear(conn->currentresult);
	}
	conn->currentresult=new PGresult(*result);

	return result;
}

PGresult *PQexecPrepared(PGconn *conn,
				const char *stmtname,
				int paramcount,
				const char * const *paramvalues,
				const int *paramlengths,
				const int *paramformats,
				int resultformat) {
	debugFunction();

	PGresult	*result=conn->currentresult;
	conn->currentresult=NULL;

	delete[] conn->error;
	conn->error=NULL;

	if (result->execstatus!=PGRES_EMPTY_QUERY) {

		result->sqlrcur=conn->sqlrcur;

		if (paramvalues) {
			for (int32_t i=0; i<paramcount; i++) {
				// postgresql only supports binary and
				// non-binary binds, all non-binary binds are
				// strings and the type is inferred on the
				// backend
				char	*varname=charstring::parseNumber(i+1);
				if (paramformats && paramformats[i]) {
					result->sqlrcur->inputBindBlob(
						varname,paramvalues[i],
						(paramlengths)?
						paramlengths[i]:0);
				} else {
					// paramlengths should be ignored
					// for text format values
					result->sqlrcur->inputBind(
						varname,paramvalues[i]);
				}
				delete[] varname;
			}
		}

		if (result->sqlrcur->executeQuery()) {
			if (result->queryisnotselect) {
				result->execstatus=PGRES_COMMAND_OK;
			} else {
				result->execstatus=PGRES_TUPLES_OK;
			}
		} else {
			charstring::printf(&conn->error,"%s\n",
					result->sqlrcur->errorMessage());
			PQclear(result);
			result=NULL;
		}
	}

	return result;
}

PGresult *PQexecParams(PGconn *conn, const char *query,
				int paramcount,
				const Oid *paramtypes,
				const char * const *paramvalues,
				const int *paramlengths,
				const int *paramformats,
				int resultformat) {
	debugFunction();
	PGresult	*result=
			PQprepare(conn,NULL,query,paramcount,paramtypes);
	if (!result || result->execstatus==PGRES_EMPTY_QUERY) {
		return result;
	}
	PQclear(result);
	return PQexecPrepared(conn,NULL,paramcount,
				paramvalues,paramlengths,paramformats,
				resultformat);
}

PGresult *PQexec(PGconn *conn, const char *query) {
	debugFunction();
	return PQexecParams(conn,query,0,NULL,NULL,NULL,NULL,0);
}

PGresult *PQdescribePrepared(PGconn *conn, const char *stmtname) {
	debugFunction();
	// FIXME: this could be sort-of implemented, at least enough
	// to make PQnparams work.
	return NULL;
}

PGresult *PQdescribePortal(PGconn *conn, const char *portalname) {
	debugFunction();
	// SQL Relay doesn't support anything like this, 
	return NULL;
}

ExecStatusType PQresultStatus(const PGresult *res) {
	debugFunction();
	// FIXME: I'm not sure I should return PGRES_FATAL_ERROR if res is NULL
	return (res)?res->execstatus:PGRES_FATAL_ERROR;
}

char *PQresStatus(ExecStatusType status) {
	debugFunction();
	if (status==PGRES_EMPTY_QUERY) {
		return (char *)"PGRES_EMPTY_QUERY";
	} else if (status==PGRES_COMMAND_OK) {
		return (char *)"PGRES_COMMAND_OK";
	} else if (status==PGRES_TUPLES_OK) {
		return (char *)"PGRES_TUPLES_OK";
	} else if (status==PGRES_COPY_OUT) {
		return (char *)"PGRES_COPY_OUT";
	} else if (status==PGRES_COPY_IN) {
		return (char *)"PGRES_COPY_IN";
	} else if (status==PGRES_BAD_RESPONSE) {
		return (char *)"PGRES_BAD_RESPONSE";
	} else if (status==PGRES_NONFATAL_ERROR) {
		return (char *)"PGRES_NONFATAL_ERROR";
	} else if (status==PGRES_FATAL_ERROR) {
		return (char *)"PGRES_FATAL_ERROR";
	}
	return NULL;
}

char *PQresultErrorMessage(const PGresult *res) {
	debugFunction();
	return const_cast<char *>(res->sqlrcur->errorMessage());
}

char *PQresultErrorField(const PGresult *res, int fieldcode) {
	debugFunction();
	// FIXME: SQL Relay doesn't have error fields, so for now,
	// I guess we'll return the error message for every field
	return const_cast<char *>(res->sqlrcur->errorMessage());
}


int PQntuples(const PGresult *res) {
	debugFunction();
	return res->sqlrcur->rowCount();
}

int PQnfields(const PGresult *res) {
	debugFunction();
	return res->sqlrcur->colCount();
}

int PQbinaryTuples(const PGresult *res) {
	debugFunction();
	// return 1 if result set contains binary data, 0 otherwise
	for (uint32_t i=0; i<res->sqlrcur->colCount(); i++) {
		if (res->sqlrcur->getColumnIsBinary(i)) {
			return 1;
		}
	}
	return 0;
}

char *PQfname(const PGresult *res, int field_num) {
	debugFunction();
	return const_cast<char *>(res->sqlrcur->getColumnName(field_num));
}

int PQfnumber(const PGresult *res, const char *field_name) {
	debugFunction();
	for (uint32_t i=0; i<res->sqlrcur->colCount(); i++) {
		if (!charstring::compare(field_name,
					res->sqlrcur->getColumnName(i))) {
			return i;
		}
	}
	return -1;
}

int PQftable(const PGresult *res, int column_number) {
	debugFunction();
	// SQL Relay doesn't know this bit of information
	// so we'll return an invalid oid.
	return InvalidOid;
}

int PQftablecol(const PGresult *res, int column_number) {
	debugFunction();
	// SQL Relay doesn't know this bit of information
	// so we'll return an invalid oid.
	return InvalidOid;
}

int PQfformat(const PGresult *res, int column_number) {
	debugFunction();
	if (res->sqlrcur->getColumnIsBinary(column_number)) {
		return 1;
	}
	return 0;
}

static Oid postgresqltypemap[]={
	// "UNKNOWN"
	705,
	// addded by freetds
	// "CHAR"
	18,
	// "INT"
	23,
	// "SMALLINT"
	21,
	// "TINYINT"
	21,
	// "MONEY"
	790,
	// "DATETIME"
	1082,
	// "NUMERIC"
	1700,
	// "DECIMAL"
	701,
	// "SMALLDATETIME"
	1082,
	// "SMALLMONEY"
	790,
	// "IMAGE"
	26,
	// "BINARY"
	26,
	// "BIT"
	1560,
	// "REAL"
	700,
	// "FLOAT"
	701,
	// "TEXT"
	25,
	// "VARCHAR"
	1043,
	// "VARBINARY"
	26,
	// "LONGCHAR"
	26,
	// "LONGBINARY"
	26,
	// "LONG"
	26,
	// "ILLEGAL"
	705,
	// "SENSITIVITY"
	705,
	// "BOUNDARY"
	705,
	// "VOID"
	2278,
	// "USHORT"
	21,
	// added by lago
	// "UNDEFINED"
	705,
	// "DOUBLE"
	708,
	// "DATE"
	1082,
	// "TIME"
	1083,
	// "TIMESTAMP"
	1114,
	// added by msql
	// "UINT"
	23,
	// "LASTREAL"
	700,
	// added by mysql
	// "STRING"
	1042,
	// "VARSTRING"
	1043,
	// "LONGLONG"
	20,
	// "MEDIUMINT"
	23,
	// "YEAR"
	23,
	// "NEWDATE"
	1082,
	// "NULL"
	705,
	// "ENUM"
	32,
	// "SET"
	32,
	// "TINYBLOB"
	26,
	// "MEDIUMBLOB"
	26,
	// "LONGBLOB"
	26,
	// "BLOB"
	26,
	// added by oracle
	// "VARCHAR2"
	1043,
	// "NUMBER"
	1700,
	// "ROWID"
	20,
	// "RAW"
	26,
	// "LONG_RAW"
	26,
	// "MLSLABEL"
	26,
	// "CLOB"
	26,
	// "BFILE"
	26,
	// added by odbc
	// "BIGINT"
	20,
	// "INTEGER"
	23,
	// "LONGVARBINARY"
	26,
	// "LONGVARCHAR"
	26,
	// added by db2
	// "GRAPHIC"
	26,
	// "VARGRAPHIC"
	26,
	// "LONGVARGRAPHIC"
	26,
	// "DBCLOB"
	26,
	// "DATALINK"
	26,
	// "USER_DEFINED_TYPE"
	705,
	// "SHORT_DATATYPE"
	21,
	// "TINY_DATATYPE"
	21,
	// added by firebird
	// "D_FLOAT"
	708,
	// "ARRAY"
	2277,
	// "QUAD"
	2277,
	// "INT64"
	20,
	// "DOUBLE PRECISION"
	708,
	// added by postgresql
	// "BOOL"
	16,
	// "BYTEA"
	17,
	// "NAME"
	19,
	// "INT8"
	20,
	// "INT2"
	21,
	// "INT2VECTOR"
	22,
	// "INT4"
	23,
	// "REGPROC"
	24,
	// "OID"
	26,
	// "TID"
	27,
	// "XID"
	28,
	// "CID"
	29,
	// "OIDVECTOR"
	30,
	// "SMGR"
	210,
	// "POINT"
	600,
	// "LSEG"
	601,
	// "PATH"
	602,
	// "BOX"
	603,
	// "POLYGON"
	604,
	// "LINE"
	628,
	// "_LINE"
	629,
	// "FLOAT4"
	700,
	// "FLOAT8"
	701,
	// "ABSTIME"
	702,
	// "RELTIME"
	703,
	// "TINTERVAL"
	704,
	// "CIRCLE"
	718,
	// "_CIRCLE"
	719,
	// "_MONEY"
	791,
	// "MACADDR"
	829,
	// "INET"
	869,
	// "CIDR"
	650,
	// "_BOOL"
	1000,
	// "_BYTEA"
	1001,
	// "_CHAR"
	1002,
	// "_NAME"
	1003,
	// "_INT2"
	1005,
	// "_INT2VECTOR"
	1006,
	// "_INT4"
	1007,
	// "_REGPROC"
	1008,
	// "_TEXT"
	1009,
	// "_oid"
	1028,
	// "_TID"
	1010,
	// "_XID"
	1011,
	// "_CID"
	1012,
	// "_OIDVECTOR"
	1013,
	// "_BPCHAR"
	1014,
	// "_VARCHAR"
	1015,
	// "_INT8"
	1016,
	// "_POINT"
	1017,
	// "_LSEG"
	1018,
	// "_PATH"
	1019,
	// "_BOX"
	1020,
	// "_FLOAT4"
	1021,
	// "_FLOAT8"
	1022,
	// "_ABSTIME"
	1023,
	// "_RELTIME"
	1024,
	// "_TINTERVAL"
	1025,
	// "_POLYGON"
	1027,
	// "ACLITEM"
	1033,
	// "_ACLITEM"
	1034,
	// "_MACADDR"
	1040,
	// "_INET"
	1041,
	// "_CIDR"
	651,
	// "BPCHAR"
	1042,
	// "_TIMESTAMP"
	1114,
	// "_DATE"
	1182,
	// "_TIME"
	1183,
	// "TIMESTAMPTZ"
	1184,
	// "_TIMESTAMPTZ"
	1185,
	// "INTERVAL"
	1186,
	// "_INTERVAL"
	1187,
	// "_NUMERIC"
	1231,
	// "TIMETZ"
	1266,
	// "_TIMETZ"
	1270,
	// "_BIT"
	1561,
	// "VARBIT"
	1562,
	// "_VARBIT"
	1563,
	// "REFCURSOR"
	1790,
	// "_REFCURSOR"
	2201,
	// "REGPROCEDURE"
	2202,
	// "REGOPER"
	2203,
	// "REGOPERATOR"
	2204,
	// "REGCLASS"
	2205,
	// "REGTYPE"
	2206,
	// "_REGPROCEDURE"
	2207,
	// "_REGOPER"
	2208,
	// "_REGOPERATOR"
	2209,
	// "_REGCLASS"
	2210,
	// "_REGTYPE"
	2211,
	// "RECORD"
	2249,
	// "CSTRING"
	2275,
	// "ANY"
	2276,
	// "ANYARRAY"
	2277,
	// "TRIGGER"
	2279,
	// "LANGUAGE_HANDLER"
	2280,
	// "INTERNAL"
	2281,
	// "OPAQUE"
	2282,
	// none added by sqlite
	// added by sqlserver
	// "UBIGINT"
	20,
	// "UNIQUEIDENTIFIER"
	26,
	// added by informix
	// "SMALLFLOAT"
	700,
	// "BYTE"
	26,
	// "BOOLEAN"
	16,
	// "TINY_TEXT"
	26,
	// "MEDIUM_TEXT"
	26,
	// "LONG_TEXT"
	26
};

Oid PQftype(const PGresult *res, int field_num) {
	debugFunction();

	// if the type is numeric then we're using a postgresql database and
	// typemangling is turned off, so we'll just return the type
	const char	*columntype=res->sqlrcur->getColumnType(field_num);
	Oid	oid=charstring::toInteger(columntype);
	if (oid) {
		return oid;
	}
	
	// if the type is not numeric, then we need to translate to a type
	// number
	for (int index=0; datatypestring[index]; index++) {
		if (!charstring::compareIgnoringCase(datatypestring[index],
								columntype)) {
			return postgresqltypemap[index];
		}
	}

	// if it wasn't found, return 705 for unknown type
	return 705;
}

int PQfsize(const PGresult *res, int field_num) {
	// for char/varchar fields, return -1,
	// otherwise, return the column length
	Oid	oid=PQftype(res,field_num);
	return (oid==1042 || oid==1043)?
		-1:(int)res->sqlrcur->getColumnLength(field_num);
}

int PQfmod(const PGresult *res, int field_num) {
	debugFunction();
	// for char/varchar fields, return the column length,
	// otherwise, return -1
	Oid	oid=PQftype(res,field_num);
	return (oid==1042 || oid==1043)?
		(int)res->sqlrcur->getColumnLength(field_num):-1;
}

char *PQcmdStatus(PGresult *res) {
	debugFunction();
	// should return a string represeting the "command type" like:
	//	SELECT, INSERT, UPDATE, DROP, etc.
	return const_cast<char *>((res->queryisnotselect)?"":"SELECT");
}

char *PQoidStatus(const PGresult *res) {
	debugFunction();
	// return OID of tuple if query was an insert,
	// otherwise return InvalidOid
	return (char *)"InvalidOid";
}

Oid PQoidValue(const PGresult *res) {
	debugFunction();
	// return OID of tuple if query was an insert,
	// otherwise return InvalidOid
	return InvalidOid;
}

char *PQcmdTuples(PGresult *res) {
	debugFunction();
	return charstring::parseNumber(res->sqlrcur->affectedRows());
}

char *PQgetvalue(const PGresult *res, int tup_num, int field_num) {
	debugFunction();
	return const_cast<char *>(res->sqlrcur->getField(tup_num,field_num));
}

int PQgetlength(const PGresult *res, int tup_num, int field_num) {
	debugFunction();
	return res->sqlrcur->getFieldLength(tup_num,field_num);
}

int PQgetisnull(const PGresult *res, int tup_num, int field_num) {
	debugFunction();
	return (res->sqlrcur->getField(tup_num,field_num)==(char *)NULL);
}

int PQnparams(const PGresult *res) {
	debugFunction();
	// Supposed to return 0 if it's not inspecting the result of
	// PQdescribePrepared.  We don't currently support PQdescribePrepared
	// so we'll always return 0.  SQL Relay does support this though,
	// with res->sqlrcur->countBindVariables();
	return 0;
}

Oid PQparamtype(const PGresult *res, int param_number) {
	debugFunction();
	// Supposed to return 0 if it's not inspecting the result of
	// PQdescribePrepared.  We don't currently support PQdescribePrepared
	// so we'll always return 0.
	return 0;
}

int PQmblen(const unsigned char *s, int encoding) {
	debugFunction();
	debugPrintf("%c\n",*s);
	// determine length of multibyte encoded char at *s
	return 1;
}

int PQenv2encoding(void) {
	debugFunction();
	return pg_char_to_encoding(environment::getValue("PGCLIENTENCODING"));
}

// Haha!  I stole these straight out of the postgresql source
//		(and reformatted them a bit)

size_t PQescapeString(char *to, const char *from, size_t length) {
	debugFunction();

	const char	*source=from;
	char		*target=to;
	unsigned int	remaining=length;

	while (remaining>0) {
		switch (*source) {
			case '\\':
				*target='\\';
				target++;
				*target='\\';
				// target and remaining are updated below.
				break;

			case '\'':
				*target='\'';
				target++;
				*target='\'';
				// target and remaining are updated below.
				break;

			default:
				*target=*source;
				// target and remaining are updated below.
		}
		source++;
		target++;
		remaining--;
	}

	// Write the terminating NUL character.
	*target='\0';

	return target-to;
}

unsigned char *PQescapeByteaConn(PGconn *conn,
					unsigned char *bintext,
					size_t binlen,
			  		size_t *bytealen) {
	unsigned char	*vp;
	unsigned char	*rp;
	unsigned char	*result;
	size_t		i;
	size_t		len;

	// empty string has 1 char ('\0')
	len=1;

	vp=bintext;
	for (i=binlen; i>0; i--, vp++) {
		if (*vp==0 || *vp>=0x80) {
			len+=5;			// '5' is for '\\ooo'
		} else if (*vp=='\'') {
			len+=2;
		} else if (*vp=='\\') {
			len+=4;
		} else {
			len++;
		}
	}

	rp=result=(unsigned char *)malloc(len);
	if (rp==NULL) {
		return NULL;
	}

	vp=bintext;
	*bytealen=len;

	for (i=binlen; i>0; i--, vp++) {
		if (*vp==0 || *vp>=0x80) {
			(void)charstring::printf((char *)rp,len,"\\\\%03o",*vp);
			rp+=5;
		} else if (*vp=='\'') {
			rp[0]='\\';
			rp[1]='\'';
			rp+=2;
		} else if (*vp=='\\') {
			rp[0]='\\';
			rp[1]='\\';
			rp[2]='\\';
			rp[3]='\\';
			rp+=4;
		} else {
			*rp++=*vp;
		}
	}
	*rp='\0';

	return result;
}

unsigned char *PQescapeBytea(unsigned char *bintext,
					size_t binlen,
			  		size_t *bytealen) {
	debugFunction();
	return PQescapeByteaConn(NULL,bintext,binlen,bytealen);
}

unsigned char *PQunescapeBytea(unsigned char *strtext, size_t *retbuflen) {
	debugFunction();

	size_t		buflen;
	unsigned char	*buffer, *sp, *bp;
	unsigned int	state=0;

	if (strtext==NULL) {
		return NULL;
	}

	// will shrink, also we discover if strtext isn't NULL terminated
	buflen=charstring::length((char *)strtext);
	buffer=(unsigned char *)malloc(buflen);

	if (buffer==NULL) {
		return NULL;
	}

	for (bp=buffer, sp=strtext; *sp!='\0'; bp++, sp++) {
		switch (state) {
			case 0:
				if (*sp=='\\') {
					state=1;
				}
				*bp=*sp;
				break;
			case 1:
				if (*sp=='\'') {
					// state=5
					// replace \' with 39
					bp--;
					*bp='\'';
					buflen--;
					state=0;
				} else if (*sp=='\\')	{
					// state=6
					// replace \\ with 92
					bp--;
					*bp='\\';
					buflen--;
					state=0;
				} else {
					if (character::isDigit(*sp)) {
						state=2;
					} else {
						state=0;
					}
					*bp=*sp;
				}
				break;
			case 2:
				if (character::isDigit(*sp)) {
					state=3;
				} else {
					state=0;
				}
				*bp=*sp;
				break;
			case 3:
				if (character::isDigit(*sp)) {
					// state=4
					int	v;
					bp-=3;
					sscanf((char *)(sp-2),
						"%03o",&v);
					*bp=v;
					buflen-=3;
					state=0;
				} else {
					*bp=*sp;
					state=0;
				}
				break;
		}
	}
	buffer=(unsigned char *)realloc(buffer, buflen);
	if (buffer==NULL) {
		return NULL;
	}

	*retbuflen=buflen;
	return buffer;
}

void PQfreemem(void *ptr) {
	debugFunction();
	free(ptr);
}

int lo_open(PGconn *conn, Oid lobjId, int mode) {
	debugFunction();
	return -1;
}

int lo_close(PGconn *conn, int fd) {
	debugFunction();
	return -1;
}

int lo_read(PGconn *conn, int fd, char *buf, size_t len) {
	debugFunction();
	return -1;
}

int lo_write(PGconn *conn, int fd, char *buf, size_t len) {
	debugFunction();
	return -1;
}

int lo_lseek(PGconn *conn, int fd, int offset, int whence) {
	debugFunction();
	return -1;
}

Oid lo_creat(PGconn *conn, int mode) {
	debugFunction();
	return InvalidOid;
}

int lo_tell(PGconn *conn, int fd) {
	debugFunction();
	return -1;
}

int lo_unlink(PGconn *conn, Oid lobjId) {
	debugFunction();
	return -1;
}

Oid lo_import(PGconn *conn, const char *filename) {
	debugFunction();
	return InvalidOid;
}

int lo_export(PGconn *conn, Oid lobjId, const char *filename) {
	debugFunction();
	return -1;
}

int PQgetline(PGconn *conn, char *string, int length) {
	debugFunction();
	// return EOF if we're at the end of input
	// 0 if an entire line has been read
	// 1 if more data is left to be read
	string[0]='\0';
	return EOF;
}

int PQputline(PGconn *conn, const char *string) {
	debugFunction();
	return EOF;
}

int PQgetlineAsync(PGconn *conn, char *buffer, int bufsize) {
	debugFunction();
	buffer[0]='\0';
	return EOF;
}

int PQputnbytes(PGconn *conn, const char *buffer, int nbytes) {
	debugFunction();
	return EOF;
}

int PQendcopy(PGconn *conn) {
	debugFunction();
	// return 0 on success, nonzero otherwise
	return -1;
}

typedef struct {
	int	len;
	int	isint;
	union {
		int	*ptr;
		int	integer;
	} u;
} PQArgBlock;

PGresult *PQfn(PGconn *conn, int fnid, int *result_buf, int *result_len,
	 	int result_is_int, const PQArgBlock *args, int nargs) {
	debugFunction();
	return NULL;
}

typedef struct pgNotify {
	char	*relname;
	int	be_pid;
} PGnotify;

PGnotify *PQnotifies(PGconn *conn) {
	debugFunction();
	return NULL;
}

void PQfreeNotify(PGnotify *notify) {
	debugFunction();
}

// for isatty()
#ifdef RUDIMENTS_HAVE_UNISTD_H
	#include <unistd.h>
#endif

typedef struct _PQprintOpt {
	char	header;	
	char	align;
	char	standard;
	char	html3;
	char	expanded;
	char	pager;
	char	*fieldSep;
	char	*tableOpt;
	char	*caption;
	char	**fieldName;
} PQprintOpt;

// Haha!  I stole these straight out of the postgresql source

static void do_field(const PQprintOpt *po, const PGresult *res,
		 const int i, const int j, const int fs_len,
		 char **fields,
		 const int nFields, const char **fieldNames,
		 unsigned char *fieldNotNum, int *fieldMax,
		 const int fieldMaxLen, FILE *fout);
static char *do_header(FILE *fout, const PQprintOpt *po, const int nFields,
	  int *fieldMax, const char **fieldNames, unsigned char *fieldNotNum,
		  const int fs_len, const PGresult *res);
static void output_row(FILE *fout, const PQprintOpt *po, const int nFields, char **fields,
		   unsigned char *fieldNotNum, int *fieldMax, char *border,
		   const int row_index);
static void fill(int length, int max, char filler, FILE *fp);

/*
 * PQprint()
 *
 * Format results of a query for printing.
 *
 * PQprintOpt is a typedef (structure) that containes
 * various flags and options. consult libpq-fe.h for
 * details
 *
 * This function should probably be removed sometime since psql
 * doesn't use it anymore. It is unclear to what extend this is used
 * by external clients, however.
 */

void
PQprint(FILE *fout,
		const PGresult *res,
		const PQprintOpt *po)
{
	debugFunction();
	int			nFields;

	nFields = PQnfields(res);

	if (nFields > 0)
	{							/* only print rows with at least 1 field.  */
		int			i,
					j;
		int			nTups;
		int		   *fieldMax = NULL;	/* in case we don't use them */
		unsigned char *fieldNotNum = NULL;
		char	   *border = NULL;
		char	  **fields = NULL;
		const char **fieldNames;
		int			fieldMaxLen = 0;
		int			numFieldName;
		int			fs_len = charstring::length(po->fieldSep);
		int			total_line_length = 0;
		int			usePipe = 0;
		//pqsigfunc	oldsigpipehandler = NULL;
		char	   *pagerenv;

#ifdef TIOCGWINSZ
		struct winsize screen_size;

#else
		struct winsize
		{
			int			ws_row;
			int			ws_col;
		}			screen_size;
#endif

		nTups = PQntuples(res);
		if (!(fieldNames = (const char **) calloc(nFields, sizeof(char *))))
		{
			perror("calloc");
			process::exit(1);
		}
		if (!(fieldNotNum = (unsigned char *) calloc(nFields, 1)))
		{
			perror("calloc");
			process::exit(1);
		}
		if (!(fieldMax = (int *) calloc(nFields, sizeof(int))))
		{
			perror("calloc");
			process::exit(1);
		}
		for (numFieldName = 0;
			 po->fieldName && po->fieldName[numFieldName];
			 numFieldName++)
			;
		for (j = 0; j < nFields; j++)
		{
			int			len;
			const char *s = (j < numFieldName && po->fieldName[j][0]) ?
			po->fieldName[j] : PQfname(res, j);

			fieldNames[j] = s;
			len = s ? charstring::length(s) : 0;
			fieldMax[j] = len;
			len += fs_len;
			if (len > fieldMaxLen)
				fieldMaxLen = len;
			total_line_length += len;
		}

		total_line_length += nFields * charstring::length(po->fieldSep) + 1;

		if (fout == NULL)
			fout = stdout;
		if (po->pager && fout == stdout
#ifndef WIN32
			&&
			isatty(fileno(stdin)) &&
			isatty(fileno(stdout))
#endif
			)
		{
			/*
			 * If we think there'll be more than one screen of output, try
			 * to pipe to the pager program.
			 */
/*#ifdef TIOCGWINSZ
			if (ioctl(fileno(stdout), TIOCGWINSZ, &screen_size) == -1 ||
				screen_size.ws_col == 0 ||
				screen_size.ws_row == 0)
			{
				screen_size.ws_row = 24;
				screen_size.ws_col = 80;
			}
#else*/
			screen_size.ws_row = 24;
			screen_size.ws_col = 80;
//#endif
			pagerenv = getenv("PAGER");
			if (pagerenv != NULL &&
				pagerenv[0] != '\0' &&
				!po->html3 &&
				((po->expanded &&
				  nTups * (nFields + 1) >= screen_size.ws_row) ||
				 (!po->expanded &&
				  nTups * (total_line_length / screen_size.ws_col + 1) *
				  (1 + (po->standard != 0)) >= screen_size.ws_row -
				  (po->header != 0) *
				  (total_line_length / screen_size.ws_col + 1) * 2
				  - (po->header != 0) * 2		/* row count and newline */
				  )))
			{
#ifdef WIN32
				fout = _popen(pagerenv, "w");
#else
				fout = popen(pagerenv, "w");
#endif
				if (fout)
				{
					usePipe = 1;
#ifndef WIN32
					//oldsigpipehandler = pqsignal(SIGPIPE, SIG_IGN);
#endif
				}
				else
					fout = stdout;
			}
		}

		if (!po->expanded && (po->align || po->html3))
		{
			if (!(fields = (char **) calloc(nFields * (nTups + 1), sizeof(char *))))
			{
				perror("calloc");
				process::exit(1);
			}
		}
		else if (po->header && !po->html3)
		{
			if (po->expanded)
			{
				if (po->align)
					fprintf(fout, "%-*s%s Value\n",
							fieldMaxLen - fs_len, "Field", po->fieldSep);
				else
					fprintf(fout, "%s%sValue\n", "Field", po->fieldSep);
			}
			else
			{
				int			len = 0;

				for (j = 0; j < nFields; j++)
				{
					const char *s = fieldNames[j];

					fputs(s, fout);
					len += charstring::length(s) + fs_len;
					if ((j + 1) < nFields)
						fputs(po->fieldSep, fout);
				}
				fputc('\n', fout);
				for (len -= fs_len; len--; fputc('-', fout));
				fputc('\n', fout);
			}
		}
		if (po->expanded && po->html3)
		{
			if (po->caption)
				fprintf(fout, "<centre><h2>%s</h2></centre>\n", po->caption);
			else
				fprintf(fout,
						"<centre><h2>"
						"Query retrieved %d rows * %d fields"
						"</h2></centre>\n",
						nTups, nFields);
		}
		for (i = 0; i < nTups; i++)
		{
			if (po->expanded)
			{
				if (po->html3)
					fprintf(fout,
						  "<table %s><caption align=high>%d</caption>\n",
							po->tableOpt ? po->tableOpt : "", i);
				else
					fprintf(fout, "-- RECORD %d --\n", i);
			}
			for (j = 0; j < nFields; j++)
				do_field(po, res, i, j, fs_len, fields, nFields,
						 fieldNames, fieldNotNum,
						 fieldMax, fieldMaxLen, fout);
			if (po->html3 && po->expanded)
				fputs("</table>\n", fout);
		}
		if (!po->expanded && (po->align || po->html3))
		{
			if (po->html3)
			{
				if (po->header)
				{
					if (po->caption)
						fprintf(fout,
						  "<table %s><caption align=high>%s</caption>\n",
								po->tableOpt ? po->tableOpt : "",
								po->caption);
					else
						fprintf(fout,
								"<table %s><caption align=high>"
								"Retrieved %d rows * %d fields"
								"</caption>\n",
						po->tableOpt ? po->tableOpt : "", nTups, nFields);
				}
				else
					fprintf(fout, "<table %s>", po->tableOpt ? po->tableOpt : "");
			}
			if (po->header)
				border = do_header(fout, po, nFields, fieldMax, fieldNames,
								   fieldNotNum, fs_len, res);
			for (i = 0; i < nTups; i++)
				output_row(fout, po, nFields, fields,
						   fieldNotNum, fieldMax, border, i);
			free(fields);
			if (border)
				free(border);
		}
		if (po->header && !po->html3)
			fprintf(fout, "(%d row%s)\n\n", PQntuples(res),
					(PQntuples(res) == 1) ? "" : "s");
		free(fieldMax);
		free(fieldNotNum);
		free((void *) fieldNames);
		if (usePipe)
		{
#ifdef WIN32
			_pclose(fout);
#else
			pclose(fout);
			//pqsignal(SIGPIPE, oldsigpipehandler);
#endif
		}
		if (po->html3 && !po->expanded)
			fputs("</table>\n", fout);
	}
}


static void
do_field(const PQprintOpt *po, const PGresult *res,
		 const int i, const int j, const int fs_len,
		 char **fields,
		 const int nFields, char const ** fieldNames,
		 unsigned char *fieldNotNum, int *fieldMax,
		 const int fieldMaxLen, FILE *fout)
{
	debugFunction();

	const char *pval,
			   *p;
	int			plen;
	bool		skipit;

	plen = PQgetlength(res, i, j);
	pval = PQgetvalue(res, i, j);

	if (plen < 1 || !pval || !*pval)
	{
		if (po->align || po->expanded)
			skipit = true;
		else
		{
			skipit = false;
			goto efield;
		}
	}
	else
		skipit = false;

	if (!skipit)
	{
		if (po->align && !fieldNotNum[j])
		{
			/* Detect whether field contains non-numeric data */
			char		ch = '0';

			for (p = pval; *p; p += PQmblen((unsigned char *)p, PQclientEncoding(res->conn)))
			{
				ch = *p;
				if (!((ch >= '0' && ch <= '9') ||
					  ch == '.' ||
					  ch == 'E' ||
					  ch == 'e' ||
					  ch == ' ' ||
					  ch == '-'))
				{
					fieldNotNum[j] = 1;
					break;
				}
			}

			/*
			 * Above loop will believe E in first column is numeric; also,
			 * we insist on a digit in the last column for a numeric. This
			 * test is still not bulletproof but it handles most cases.
			 */
			if (*pval == 'E' || *pval == 'e' ||
				!(ch >= '0' && ch <= '9'))
				fieldNotNum[j] = 1;
		}

		if (!po->expanded && (po->align || po->html3))
		{
			if (plen > fieldMax[j])
				fieldMax[j] = plen;
			if (!(fields[i * nFields + j] = (char *) malloc(plen + 1)))
			{
				perror("malloc");
				process::exit(1);
			}
			charstring::copy(fields[i * nFields + j], pval);
		}
		else
		{
			if (po->expanded)
			{
				if (po->html3)
					fprintf(fout,
							"<tr><td align=left><b>%s</b></td>"
							"<td align=%s>%s</td></tr>\n",
							fieldNames[j],
							fieldNotNum[j] ? "left" : "right",
							pval);
				else
				{
					if (po->align)
						fprintf(fout,
								"%-*s%s %s\n",
								fieldMaxLen - fs_len, fieldNames[j],
								po->fieldSep,
								pval);
					else
						fprintf(fout,
								"%s%s%s\n",
								fieldNames[j], po->fieldSep, pval);
				}
			}
			else
			{
				if (!po->html3)
				{
					fputs(pval, fout);
			efield:
					if ((j + 1) < nFields)
						fputs(po->fieldSep, fout);
					else
						fputc('\n', fout);
				}
			}
		}
	}
}


static char *
do_header(FILE *fout, const PQprintOpt *po, const int nFields, int *fieldMax,
		  const char **fieldNames, unsigned char *fieldNotNum,
		  const int fs_len, const PGresult *res)
{
	debugFunction();

	int			j;				/* for loop index */
	char	   *border = NULL;

	if (po->html3)
		fputs("<tr>", fout);
	else
	{
		int			tot = 0;
		int			n = 0;
		char	   *p = NULL;

		for (; n < nFields; n++)
			tot += fieldMax[n] + fs_len + (po->standard ? 2 : 0);
		if (po->standard)
			tot += fs_len * 2 + 2;
		border = (char *)malloc(tot + 1);
		if (!border)
		{
			perror("malloc");
			process::exit(1);
		}
		p = border;
		if (po->standard)
		{
			char	   *fs = po->fieldSep;

			while (*fs++)
				*p++ = '+';
		}
		for (j = 0; j < nFields; j++)
		{
			int			len;

			for (len = fieldMax[j] + (po->standard ? 2 : 0); len--; *p++ = '-');
			if (po->standard || (j + 1) < nFields)
			{
				char	   *fs = po->fieldSep;

				while (*fs++)
					*p++ = '+';
			}
		}
		*p = '\0';
		if (po->standard)
			fprintf(fout, "%s\n", border);
	}
	if (po->standard)
		fputs(po->fieldSep, fout);
	for (j = 0; j < nFields; j++)
	{
		const char *s = PQfname(res, j);

		if (po->html3)
		{
			fprintf(fout, "<th align=%s>%s</th>",
					fieldNotNum[j] ? "left" : "right", fieldNames[j]);
		}
		else
		{
			int			n = charstring::length(s);

			if (n > fieldMax[j])
				fieldMax[j] = n;
			if (po->standard)
				fprintf(fout,
						fieldNotNum[j] ? " %-*s " : " %*s ",
						fieldMax[j], s);
			else
				fprintf(fout, fieldNotNum[j] ? "%-*s" : "%*s", fieldMax[j], s);
			if (po->standard || (j + 1) < nFields)
				fputs(po->fieldSep, fout);
		}
	}
	if (po->html3)
		fputs("</tr>\n", fout);
	else
		fprintf(fout, "\n%s\n", border);
	return border;
}


static void
output_row(FILE *fout, const PQprintOpt *po, const int nFields, char **fields,
		   unsigned char *fieldNotNum, int *fieldMax, char *border,
		   const int row_index)
{
	debugFunction();

	int			field_index;	/* for loop index */

	if (po->html3)
		fputs("<tr>", fout);
	else if (po->standard)
		fputs(po->fieldSep, fout);
	for (field_index = 0; field_index < nFields; field_index++)
	{
		char	   *p = fields[row_index * nFields + field_index];

		if (po->html3)
			fprintf(fout, "<td align=%s>%s</td>",
				fieldNotNum[field_index] ? "left" : "right", p ? p : "");
		else
		{
			fprintf(fout,
					fieldNotNum[field_index] ?
					(po->standard ? " %-*s " : "%-*s") :
					(po->standard ? " %*s " : "%*s"),
					fieldMax[field_index],
					p ? p : "");
			if (po->standard || field_index + 1 < nFields)
				fputs(po->fieldSep, fout);
		}
		if (p)
			free(p);
	}
	if (po->html3)
		fputs("</tr>", fout);
	else if (po->standard)
		fprintf(fout, "\n%s", border);
	fputc('\n', fout);
}



/*
 * really old printing routines
 */

void
PQdisplayTuples(const PGresult *res,
				FILE *fp,		/* where to send the output */
				int fillAlign,	/* pad the fields with spaces */
				const char *fieldSep,	/* field separator */
				int printHeader,	/* display headers? */
				int quiet
)
{
	debugFunction();
#define DEFAULT_FIELD_SEP " "

	int			i,
				j;
	int			nFields;
	int			nTuples;
	int		   *fLength = NULL;

	if (fieldSep == NULL)
		fieldSep = DEFAULT_FIELD_SEP;

	/* Get some useful info about the results */
	nFields = PQnfields(res);
	nTuples = PQntuples(res);

	if (fp == NULL)
		fp = stdout;

	/* Figure the field lengths to align to */
	/* will be somewhat time consuming for very large results */
	if (fillAlign)
	{
		fLength = (int *) malloc(nFields * sizeof(int));
		for (j = 0; j < nFields; j++)
		{
			fLength[j] = charstring::length(PQfname(res, j));
			for (i = 0; i < nTuples; i++)
			{
				int			flen = PQgetlength(res, i, j);

				if (flen > fLength[j])
					fLength[j] = flen;
			}
		}
	}

	if (printHeader)
	{
		/* first, print out the attribute names */
		for (i = 0; i < nFields; i++)
		{
			fputs(PQfname(res, i), fp);
			if (fillAlign)
				fill(charstring::length(PQfname(res, i)), fLength[i], ' ', fp);
			fputs(fieldSep, fp);
		}
		fprintf(fp, "\n");

		/* Underline the attribute names */
		for (i = 0; i < nFields; i++)
		{
			if (fillAlign)
				fill(0, fLength[i], '-', fp);
			fputs(fieldSep, fp);
		}
		fprintf(fp, "\n");
	}

	/* next, print out the instances */
	for (i = 0; i < nTuples; i++)
	{
		for (j = 0; j < nFields; j++)
		{
			fprintf(fp, "%s", PQgetvalue(res, i, j));
			if (fillAlign)
				fill(charstring::length(PQgetvalue(res, i, j)), fLength[j], ' ', fp);
			fputs(fieldSep, fp);
		}
		fprintf(fp, "\n");
	}

	if (!quiet)
		fprintf(fp, "\nQuery returned %d row%s.\n", PQntuples(res),
				(PQntuples(res) == 1) ? "" : "s");

	fflush(fp);

	if (fLength)
		free(fLength);
}



void
PQprintTuples(const PGresult *res,
		FILE *fout,		/* output stream */
		int PrintAttNames,	/* print attribute names or not */
		int TerseOutput,	/* delimiter bars or not? */
		int colWidth		/* width of column, if 0, use variable
					 * width */
		)
{
	debugFunction();

	int		nFields;
	int		nTups;
	int		i,j;
	char		formatString[80];

	char		*tborder = NULL;

	nFields = PQnfields(res);
	nTups = PQntuples(res);

	if (colWidth > 0)
		charstring::printf(formatString,sizeof(formatString),
						"%%s %%-%ds", colWidth);
	else
		charstring::printf(formatString,sizeof(formatString),"%%s %%s");

	if (nFields > 0)
	{	/* only print rows with at least 1 field.  */

		if (!TerseOutput)
		{
			int			width;

			width = nFields * 14;
			tborder = (char *)malloc(width + 1);
			for (i = 0; i <= width; i++)
				tborder[i] = '-';
			tborder[i] = '\0';
			fprintf(fout, "%s\n", tborder);
		}

		for (i = 0; i < nFields; i++)
		{
			if (PrintAttNames)
			{
				fprintf(fout, formatString,
						TerseOutput ? "" : "|",
						PQfname(res, i));
			}
		}

		if (PrintAttNames)
		{
			if (TerseOutput)
				fprintf(fout, "\n");
			else
				fprintf(fout, "|\n%s\n", tborder);
		}

		for (i = 0; i < nTups; i++)
		{
			for (j = 0; j < nFields; j++)
			{
				const char *pval = PQgetvalue(res, i, j);

				fprintf(fout, formatString,
						TerseOutput ? "" : "|",
						pval ? pval : "");
			}
			if (TerseOutput)
				fprintf(fout, "\n");
			else
				fprintf(fout, "|\n%s\n", tborder);
		}
	}

	if (tborder) {
		free(tborder);
	}
}



/* simply send out max-length number of filler characters to fp */

static void
fill(int length, int max, char filler, FILE *fp)
{
	debugFunction();

	int			count;

	count = max - length;
	while (count-- >= 0)
		putc(filler, fp);
}

void PQtrace(PGconn *conn, FILE *debug_port) {
	debugFunction();
	// FIXME: this should go to a file instead of stdio
	conn->sqlrcon->debugOn();
}

void PQuntrace(PGconn *conn) {
	debugFunction();
	conn->sqlrcon->debugOff();
}

// FIXME: these functions are dummied up to use the synchronous interface

typedef enum {
	PGRES_POLLING_FAILED = 0,
	PGRES_POLLING_READING,
	PGRES_POLLING_WRITING,
	PGRES_POLLING_OK,
	PGRES_POLLING_ACTIVE
} PostgresPollingStatusType;

PGconn *PQconnectStart(const char *conninfo) {
	debugFunction();
	// SQL Relay doesn't connect right away, so just pass through the
	// normal connect process here
	return PQconnectdb(conninfo);
}

PostgresPollingStatusType PQconnectPoll(PGconn *conn) {
	debugFunction();
	return PGRES_POLLING_OK;
}

int PQresetStart(PGconn *conn) {
	debugFunction();
	PQreset(conn);
	return 1;
}

PostgresPollingStatusType PQresetPoll(PGconn *conn) {
	debugFunction();
	return PGRES_POLLING_OK;
}

PGcancel *PQgetCancel(PGconn *conn) {
	debugFunction();
	return new PGcancel;
}

void PQfreeCancel(PGcancel *cancel) {
	debugFunction();
	if (cancel) {
		delete cancel;
	}
}

int PQcancel(PGcancel *cancel, char *errbuf, int errbufsize) {
	debugFunction();
	return 1;
}

int PQrequestCancel(PGconn *conn) {
	debugFunction();
	delete conn->sqlrcon;
	conn->sqlrcon=NULL;
	conn->sqlrcur=NULL;
	return TRUE;
}

int PQsendQuery(PGconn *conn, const char *query) {
	debugFunction();

	// FIXME:
	// "query" could contain multiple queries,
	// parse them out and fork a thread to run each query
	conn->currentresult=PQexec(conn,query);
	return TRUE;
}

PGresult *PQgetResult(PGconn *conn) {
	debugFunction();

	// FIXME:
	// Should poll to see if one of the queries started in
	// PQsendQuery is done, if so, return it's result set.
	// If all queries are done, return NULL
	PGresult	*retval=conn->currentresult;
	conn->currentresult=NULL;
	return retval;
}

PGTransactionStatusType PQtransactionStatus(const PGconn *conn) {
	debugFunction();
	return PQTRANS_IDLE;
}

int PQisBusy(PGconn *conn) {
	debugFunction();
	return FALSE;
}

int PQconsumeInput(PGconn *conn) {
	debugFunction();
	return TRUE;
}

int PQsetnonblocking(PGconn *conn, int arg) {
	debugFunction();
	conn->nonblockingmode=arg;
	return TRUE;
}

int PQisnonblocking(const PGconn *conn) {
	debugFunction();
	return conn->nonblockingmode;
}

int PQflush(PGconn *conn) {
	debugFunction();
	// return 0 on success or EOF on failure
	return 0;
}

int PQsendSome(PGconn *conn) {
	debugFunction();
	// Have no idea what should be returned here
	return 0;
}

int PQprotocolVersion(PGconn *conn) {
	debugFunction();
	// psycopg2 needs this to be 3
	return 3;
}

}
