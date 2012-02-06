// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef MDBTOOLSCONNECTION_H
#define MDBTOOLSCONNECTION_H

#define NUM_CONNECT_STRING_VARS 1

#include <sqlrconnection.h>

extern "C" {
	#include <mdbsql.h>
}

class mdbtoolsconnection;

class mdbtoolscursor : public sqlrcursor_svr {
	friend class mdbtoolsconnection;
	private:
				mdbtoolscursor(sqlrconnection_svr *conn);
				~mdbtoolscursor();
		bool		openCursor(uint16_t id);
		bool		closeCursor();
		bool		supportsNativeBinds();
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
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
		void		cleanUpData(bool freeresult, bool freebinds);

		mdbtoolsconnection	*mdbtoolsconn;

		char	**columnnames;

		MdbSQL	mdbsql;
};

class mdbtoolsconnection : public sqlrconnection_svr {
	friend class mdbtoolscursor;
	public:
			mdbtoolsconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void	handleConnectString();
		bool	logIn(bool printerrors);
		sqlrcursor_svr	*initCursor();
		void	deleteCursor(sqlrcursor_svr *curs);
		void	logOut();
		bool	isTransactional();
		bool	ping();
		const char	*identify();
		const char	*dbVersion();
		bool		getDatabaseList(sqlrcursor_svr *cursor,
						const char *wild,
						char ***cols,
						uint32_t *colcount,
						char ****rows,
						uint64_t *rowcount);
		bool		getTableList(sqlrcursor_svr *cursor,
						const char *wild,
						char ***cols,
						uint32_t *colcount,
						char ****rows,
						uint64_t *rowcount);
		bool		getColumnList(sqlrcursor_svr *cursor,
						const char *table,
						const char *wild,
						char ***cols,
						uint32_t *colcount,
						char ****rows,
						uint64_t *rowcount);
		bool	setIsolationLevel(const char *isolevel);
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	commit();
		bool	rollback();

		const char	*db;
};

#endif
