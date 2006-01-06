// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef ROUTERCONNECTION_H
#define ROUTERCONNECTION_H

#define FETCH_AT_ONCE	10

#include <sqlrconnection.h>

#include <sqlrelay/sqlrclient.h>

class routerconnection;

class routercursor : public sqlrcursor_svr {
	friend class routerconnection;
	private:
				routercursor(sqlrconnection_svr *conn);
				~routercursor();
		bool		openCursor(uint16_t id);
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
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
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

		sqlrcursor	*cur;

		uint64_t	currentrow;
};

class routerconnection : public sqlrconnection_svr {
	friend class routercursor;
	public:
			routerconnection();
			~routerconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		bool		supportsNativeBinds();
		void		handleConnectString();
		bool		logIn();
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		const char	*identify();

		void		endSession();
		void		dropTable(const char *table);

		sqlrconnection	**cons;
		uint16_t	concount;

		sqlrconfigfile	*cfgfile;
};

#endif
