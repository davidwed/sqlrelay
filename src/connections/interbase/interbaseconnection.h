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

class interbaseconnection;

class interbasecursor : public sqlrcursor {
	friend class interbaseconnection;
	private:
				interbasecursor(sqlrconnection *conn);
				~interbasecursor();
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

		isc_stmt_handle	stmt;
		XSQLDA	ISC_FAR	*outsqlda;
		XSQLDA	ISC_FAR	*insqlda;
		ISC_BLOB_DESC	to_desc;

		ISC_LONG	querytype;

		fieldstruct	field[MAX_SELECT_LIST_SIZE];

		stringbuffer	*errormsg;

		interbaseconnection	*interbaseconn;

		bool		queryIsExecSP;
};

class interbaseconnection : public sqlrconnection {
	friend class interbasecursor;
	public:
			interbaseconnection();
			~interbaseconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void	handleConnectString();
		bool	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		bool	commit();
		bool	rollback();
		bool	ping();
		const char	*identify();

		char		dpb[256];
		short		dpblength;
		isc_db_handle	db;
		isc_tr_handle	tr;

		const char	*database;
		unsigned short	dialect;

		environment	*env;

		ISC_STATUS	error[20];
};

#endif
