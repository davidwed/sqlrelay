// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef ORACLESTATEMENT_H
#define ORACLESTATEMENT_H

#define STATEMENTS		1
#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	2048

#define NUM_CONNECT_STRING_VARS	6

#include <rudiments/environment.h>
#include <sqlrconnection.h>

#include <config.h>

extern "C" {
	#ifdef __CYGWIN__
		#define _int64 long long
	#endif
	#ifdef HAVE_OCI_H
		#include <oci.h>
	#else
		#include <oratypes.h>
		#include <ocidfn.h>
		#include <ociapr.h>
	#endif

	#define PARSE_DEFER 1
	#define PARSE_V7_LNG 2

	#define SELECT_QUERY 4
	#define COMMIT_QUERY 54
	#define ROLLBACK_QUERY 55

	#define NOT_IN_LIST 1007
	#define NO_DATA_FOUND 1403

	#define VARCHAR2_TYPE 1
	#define	NUMBER_TYPE 2
	#define	LONG_TYPE 8
	#define ROWID_TYPE 11
	#define DATE_TYPE 12
	#define RAW_TYPE 23
	#define LONG_RAW_TYPE 24
	#define CHAR_TYPE 96
	#define MLSLABEL_TYPE 105

	#define VARCHAR2_BIND_TYPE 1
	#define LONG_BIND_TYPE 3
	#define DOUBLE_BIND_TYPE 4
	#define NULL_TERMINATED_STRING 5
}

struct describe {
	sb4	dbsize;
	sb2	dbtype;
	sb1	buf[MAX_ITEM_BUFFER_SIZE];
	sb4	buflen;
	sb4	dsize;
	sb2	precision;
	sb2	scale;
	sb2	nullok;
};

class oracle7connection;

class oracle7cursor : public sqlrcursor_svr {
	friend class oracle7connection;
	private:
				oracle7cursor(sqlrconnection_svr *conn);
				~oracle7cursor();
		bool		openCursor(uint16_t id);
		bool		closeCursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						short *isnull);
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
						short *isnull);
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
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
		bool		queryIsNotSelect();
		bool		queryIsCommitOrRollback();
		const char	*getErrorMessage(bool *liveconnection);
		void		returnRowCounts();
		void		returnColumnCount();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		returnRow();
		void		cleanUpData(bool freeresult, bool freebinds);
	private:

		Cda_Def		cda;
		stringbuffer	*errormessage;
		sword		ncols;

		describe	desc[MAX_SELECT_LIST_SIZE];
		ub1		def_buf[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE][MAX_ITEM_BUFFER_SIZE];
		sb2		def_indp[MAX_SELECT_LIST_SIZE][FETCH_AT_ONCE];
		ub2		def_col_retlen[MAX_SELECT_LIST_SIZE]
							[FETCH_AT_ONCE];
		ub2		def_col_retcode[MAX_SELECT_LIST_SIZE]
							[FETCH_AT_ONCE];
	
		uint64_t	row;
		uint64_t	maxrow;
		uint64_t	totalrows;

		uint16_t	inputbindcount;
		char		*intbindstring[MAXVAR];

		char		*outintbindstring[MAXVAR];
		int64_t		*outintbind[MAXVAR];
		uint16_t	outbindcount;

		oracle7connection	*oracle7conn;
};


class oracle7connection : public sqlrconnection_svr {
	friend class oracle7cursor;
	public:
			oracle7connection();
			~oracle7connection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn();
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		const char	*pingQuery();
		const char	*identify();

		Lda_Def		lda;
		ub4		hda[256/sizeof(ub4)];

		const char	*home;
		const char	*sid;
		const char	*nlslang;

		environment	*env;
};

#endif
