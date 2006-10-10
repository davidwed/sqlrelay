// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef ROUTERCONNECTION_H
#define ROUTERCONNECTION_H

#define FETCH_AT_ONCE	10

#include <sqlrconnection.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/regularexpression.h>

struct outputbindvar {
	const char	*variable;
	union {
		char		*stringvalue;
		int64_t		*intvalue;
		double		*doublevalue;
	} value;
	uint16_t	valuesize;
	bindtype	type;
	int16_t		*isnull;
};

struct cursorbindvar {
	const char	*variable;
	sqlrcursor_svr	*cursor;
};

class routerconnection;

class routercursor : public sqlrcursor_svr {
	friend class routerconnection;
	private:
				routercursor(sqlrconnection_svr *conn);
				~routercursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		supportsNativeBinds();
		bool		begin(const char *query, uint32_t length);
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
		bool		outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrcursor_svr *cursor);
		void		returnOutputBindBlob(uint16_t index);
		void		returnOutputBindClob(uint16_t index);
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
		void		checkForTempTable(const char *query,
							uint32_t length);
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

		routerconnection	*routerconn;

		sqlrconnection	*con;
		sqlrcursor	*cur;
		bool		isbindcur;
		uint16_t	curindex;
		sqlrcursor	**curs;

		uint64_t	nextrow;

		bool		beginquery;

		outputbindvar	obv[MAXVAR];
		uint16_t	obcount;

		cursorbindvar	cbv[MAXVAR];
		uint16_t	cbcount;

		regularexpression	createoratemp;
		regularexpression	preserverows;
};

class routerconnection : public sqlrconnection_svr {
	friend class routercursor;
	public:
			routerconnection();
			~routerconnection();
	private:
		bool		supportsAuthOnDatabase();
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn(bool printerrors);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		const char	*identify();
		bool		ping();
		void		endSession();

		void	autoCommitOnFailed(uint16_t index);
		void	autoCommitOffFailed(uint16_t index);
		void	commitFailed(uint16_t index);
		void	rollbackFailed(uint16_t index);
		void	beginQueryFailed(uint16_t index);

		sqlrconnection	**cons;
		const char	**beginquery;
		bool		anymustbegin;
		uint16_t	concount;

		sqlrconfigfile	*cfgfile;

		bool		justloggedin;

		int16_t		nullbindvalue;
		int16_t		nonnullbindvalue;

		regularexpression	beginregex;
		regularexpression	beginendregex;

		const char	*error;
};

#endif
