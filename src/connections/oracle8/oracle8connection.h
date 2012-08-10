// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef ORACLECONNECTION_H
#define ORACLECONNECTION_H

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	32768

#define NUM_CONNECT_STRING_VARS 6

#include <sqlrconnection.h>
#include <sqlwriter.h>
#ifdef HAVE_ORACLE_8i
	#include <rudiments/regularexpression.h>
#endif

extern "C" {
	#ifdef __CYGWIN__
		#define _int64 long long
	#endif
	#include <oci.h>

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
	OCIDate		*ocidate;
};

class oracle8connection;

class oracle8cursor : public sqlrcursor_svr {
	friend class oracle8connection;
	private:
				oracle8cursor(sqlrconnection_svr *conn);
				~oracle8cursor();
		void		allocateResultSetBuffers(uint32_t fetchatonce,
							int32_t selectlistsize,
							int32_t itembuffersize);
		void		deallocateResultSetBuffers();
		bool		openCursor(uint16_t id);
		bool		closeCursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		bool		inputBindDate(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						const char *tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBindString(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint16_t valuesize,
						int16_t *isnull);
		bool		outputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		outputBindDate(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						const char **tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
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
		bool		outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrcursor_svr *cursor);
		bool		getLobOutputBindLength(uint16_t index,
							uint64_t *length);
		bool		getLobOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread);
#endif
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
#ifdef HAVE_ORACLE_8i
		void		checkForTempTable(const char *query,
							uint32_t length);
#endif
		bool		queryIsNotSelect();
		void		errorMessage(const char **errorstring,
						int64_t	*errorcode,
						bool *liveconnection);
		bool		knowsRowCount();
		uint64_t	rowCount();
		bool		knowsAffectedRows();
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char * const * columnNames();
		uint16_t	columnTypeFormat();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
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
		void		cleanUpLobField(uint32_t col);
		void		cleanUpData(bool freeresult, bool freebinds);
		bool		getColumnNameList(stringbuffer *output);

		void		checkRePrepare();

		void		dateToString(char *buffer,
						uint16_t buffersize,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						const char *tz);

		OCIStmt		*stmt;
		ub2		stmttype;
		sword		ncols;
		stringbuffer	*errormessage;

		int32_t		resultsetbuffercount;
		describe	*desc;
		char		**columnnames;
		OCIDefine	**def;
		OCILobLocator	***def_lob;
		ub1		**def_buf;
		sb2		**def_indp;
		ub2		**def_col_retlen;
		ub2		**def_col_retcode;

		OCIBind		*inbindpp[MAXVAR];
		OCIBind		*outbindpp[MAXVAR];
		OCIBind		*curbindpp[MAXVAR];
		char		*inintbindstring[MAXVAR];
		OCIDate		*indatebind[MAXVAR];
		char		*outintbindstring[MAXVAR];
		datebind	*outdatebind[MAXVAR];
		int64_t		*outintbind[MAXVAR];
		uint16_t	orainbindcount;
		uint16_t	oraoutbindcount;
		uint16_t	oracurbindcount;

#ifdef HAVE_ORACLE_8i
		OCILobLocator	*inbind_lob[MAXVAR];
		OCILobLocator	*outbind_lob[MAXVAR];
		uint16_t	orainbindlobcount;
		uint16_t	oraoutbindlobcount;
#endif

		uint64_t	row;
		uint64_t	maxrow;
		uint64_t	totalrows;

		char		*query;
		uint32_t	length;
		bool		prepared;

		bool		resultfreed;

		oracle8connection	*oracle8conn;

#ifdef HAVE_ORACLE_8i
		regularexpression	preserverows;
		regularexpression	deleterows;
		regularexpression	asselect;
#endif
};
	
class oracle8connection : public sqlrconnection_svr {
	friend class oracle8cursor;
	public:
				oracle8connection();
				~oracle8connection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		void		dropTempTables(sqlrcursor_svr *cursor,
						stringlist *tablelist);
		bool		logIn(bool printerrors);
		void		logInError(const char *errmsg);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
#ifdef OCI_ATTR_PROXY_CREDENTIALS
		bool		changeUser(const char *newuser,
						const char *newpassword);
#endif
		bool		supportsTransactionBlocks();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		const char	*pingQuery();
		const char	*identify();
		const char	*dbVersion();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		const char	*getLastInsertIdQuery();

		sqlwriter	*getSqlWriter();

		ub4		statementmode;

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

		const char	*home;
		const char	*sid;
		const char	*nlslang;

		char		*lastinsertidquery;

		uint32_t	fetchatonce;
		int32_t		maxselectlistsize;
		int32_t		maxitembuffersize;

#ifdef HAVE_ORACLE_8i
		bool		droptemptables;
#endif
};

#endif
