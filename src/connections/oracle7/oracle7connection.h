// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef ORACLESTATEMENT_H
#define ORACLESTATEMENT_H

#define STATEMENTS		1
#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	2048

#define NUM_CONNECT_STRING_VARS	5

#include <rudiments/environment.h>
#include <sqlrconnection.h>

#include <config.h>

extern "C" {
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

class oracle7cursor : public sqlrcursor {
	friend class oracle7connection;
	private:
			oracle7cursor(sqlrconnection *conn);
			~oracle7cursor();
		bool	openCursor(int id);
		bool	closeCursor();
		bool	prepareQuery(const char *query, long length);
		bool	inputBindString(const char *variable, 
					unsigned short variablesize,
					const char *value, 
					unsigned short valuesize,
					short *isnull);
		bool	inputBindLong(const char *variable, 
					unsigned short variablesize,
					unsigned long *value);
		bool	inputBindDouble(const char *variable, 
					unsigned short variablesize,
					double *value,
					unsigned short precision,
					unsigned short scale);
		bool	outputBindString(const char *variable, 
					unsigned short variablesize,
					char *value,
					unsigned short valuesize,
					short *isnull);
		bool	executeQuery(const char *query,
					long length,
					bool execute);
		bool	queryIsNotSelect();
		bool	queryIsCommitOrRollback();
		char	*getErrorMessage(bool *liveconnection);
		void	returnRowCounts();
		void	returnColumnCount();
		void	returnColumnInfo();
		bool	noRowsToReturn();
		bool	skipRow();
		bool	fetchRow();
		void	returnRow();
		void	cleanUpData(bool freerows, bool freecols,
							bool freebinds);
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
	
		int		row;
		int		maxrow;
		unsigned int	totalrows;

		oracle7connection	*oracle7conn;
};


class oracle7connection : public sqlrconnection {
	friend class oracle7cursor;
	public:
			oracle7connection();
			~oracle7connection();
	private:
		int	getNumberOfConnectStringVars();
		void	handleConnectString();
		bool	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	commit();
		bool	rollback();
		char	*pingQuery();
		char	*identify();

		Lda_Def		lda;
		ub4		hda[256/sizeof(ub4)];

		char		*home;
		char		*sid;

		environment	*env;
};

#endif
