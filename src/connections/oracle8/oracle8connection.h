// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef ORACLECONNECTION_H
#define ORACLECONNECTION_H

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	32768

#define NUM_CONNECT_STRING_VARS 6

#include <sqlrconnection.h>
#ifdef HAVE_ORACLE_8i
	#include <rudiments/regularexpression.h>
#endif
#include <rudiments/environment.h>

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
	sb4		dbsize;
	sb2		dbtype;
	text		*buf;
	sb4		buflen;
	ub1		precision;
	ub1		scale;
	ub1		nullok;
};

class oracle8connection;

class oracle8cursor : public sqlrcursor_svr {
	friend class oracle8connection;
	private:
				oracle8cursor(sqlrconnection_svr *conn);
				~oracle8cursor();
		bool		openCursor(uint16_t id);
		bool		closeCursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						int16_t *isnull);
		bool		inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
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
		void		returnOutputBindBlob(uint16_t index);
		void		returnOutputBindClob(uint16_t index);
		void		sendLob(OCILobLocator *lob, ub1 *buf);
#endif
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
#ifdef HAVE_ORACLE_8i
		void		checkForTempTable(const char *query,
							uint32_t length);
#endif
		bool		queryIsNotSelect();
		bool		queryIsCommitOrRollback();
		const char	*errorMessage(bool *liveconnection);
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
		void		returnRow();
		void		cleanUpData(bool freeresult, bool freebinds);

		void		checkRePrepare();

		OCIStmt		*stmt;
		ub2		stmttype;
		sword		ncols;
		stringbuffer	*errormessage;

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
		char		*outintbindstring[MAXVAR];
		int64_t		*outintbind[MAXVAR];
		uint16_t	inbindcount;
		uint16_t	outbindcount;
		uint16_t	curbindcount;

#ifdef HAVE_ORACLE_8i
		OCILobLocator	*inbind_lob[MAXVAR];
		OCILobLocator	*outbind_lob[MAXVAR];
		uint16_t	inbindlobcount;
		uint16_t	outbindlobcount;
#endif

		uint64_t	row;
		uint64_t	maxrow;
		uint64_t	totalrows;

		char		*query;
		uint32_t	length;
		bool		prepared;

		oracle8connection	*oracle8conn;

#ifdef HAVE_ORACLE_8i
		regularexpression	preserverows;
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
		bool		logIn();
		void		logInError(const char *errmsg);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
#ifdef OCI_ATTR_PROXY_CREDENTIALS
		bool		changeUser(const char *newuser,
						const char *newpassword);
#endif
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		const char	*pingQuery();
		const char	*identify();

		ub4		statementmode;

		OCIEnv		*env;
		OCIServer	*srv;
		OCIError	*err;
		OCISvcCtx	*svc;
		OCISession	*session;
		OCITrans	*trans;

#ifdef OCI_ATTR_PROXY_CREDENTIALS
		OCISession	*newsession;
		bool		supportsproxycredentials;
#endif

		const char	*home;
		const char	*sid;
		const char	*nlslang;

		ub4		fetchatonce;
		ub4		maxselectlistsize;
		ub4		maxitembuffersize;

		environment	*envr;
};

#endif
