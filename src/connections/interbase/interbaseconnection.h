// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef INTERBASECCONNECTION_H
#define INTERBASECCONNECTION_H

#define NUM_CONNECT_STRING_VARS 6

#include <rudiments/environment.h>
#include <sqlrconnection.h>

#include <ibase.h>

#define MAX_ITEM_BUFFER_SIZE 4096
#define MAX_SELECT_LIST_SIZE 256
#define MAX_BIND_VARS 512

struct fieldstruct {
	int		sqlrtype;
	short		type;

	short		shortbuffer;
	long		longbuffer;
	float		floatbuffer;
	double		doublebuffer;
	ISC_QUAD	quadbuffer;
	ISC_DATE	datebuffer;
	ISC_TIME	timebuffer;
	ISC_TIMESTAMP	timestampbuffer;
	ISC_INT64	int64buffer;
	char		textbuffer[MAX_ITEM_BUFFER_SIZE+1];

	short		nullindicator;
};

static char tpb[] = {
	isc_tpb_version3,
	isc_tpb_write,
	isc_tpb_read_committed,
	isc_tpb_rec_version,
	// FIXME: vladimir changed this to isc_tpb_nowait.  why?
	isc_tpb_wait
};

class interbaseconnection;

class interbasecursor : public sqlrcursor {
	friend class interbaseconnection;
	private:
			interbasecursor(sqlrconnection *conn);
			~interbasecursor();
		int	prepareQuery(const char *query, long length);
		int	inputBindString(const char *variable, 
					unsigned short variablesize,
					const char *value, 
					unsigned short valuesize,
					short *isnull);
		int	inputBindLong(const char *variable, 
					unsigned short variablesize,
					unsigned long *value);
		int	inputBindDouble(const char *variable, 
					unsigned short variablesize,
					double *value, 
					unsigned short precision,
					unsigned short scale);
		int	outputBindString(const char *variable, 
					unsigned short variablesize,
					char *value, 
					unsigned short valuesize,
					short *isnull);
		int	executeQuery(const char *query, long length,
					unsigned short execute);
		int	queryIsNotSelect();
		int	queryIsCommitOrRollback();
		char	*getErrorMessage(int *liveconnection);
		void	returnRowCounts();
		void	returnColumnCount();
		void	returnColumnInfo();
		int	noRowsToReturn();
		int	skipRow();
		int	fetchRow();
		void	returnRow();

		isc_stmt_handle	stmt;
		XSQLDA	ISC_FAR	*outsqlda;
		XSQLDA	ISC_FAR	*insqlda;
		ISC_BLOB_DESC	to_desc;

		int		querytype;

		fieldstruct	field[MAX_SELECT_LIST_SIZE];

		stringbuffer	*errormsg;

		interbaseconnection	*interbaseconn;

		int		queryIsExecSP;
};

class interbaseconnection : public sqlrconnection {
	friend class interbasecursor;
	public:
			interbaseconnection();
			~interbaseconnection();
	private:
		int	getNumberOfConnectStringVars();
		void	handleConnectString();
		int	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		int	commit();
		int	rollback();
		int	ping();
		char	*identify();

		char		dpb[256];
		short		dpblength;
		isc_db_handle	db;
		isc_tr_handle	tr;

		char		*database;
		int		dialect;

		environment	*env;

		ISC_STATUS	error[20];
};

#endif
