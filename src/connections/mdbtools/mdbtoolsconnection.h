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
		bool	logIn();
		sqlrcursor_svr	*initCursor();
		void	deleteCursor(sqlrcursor_svr *curs);
		void	logOut();
		bool	isTransactional();
		bool	ping();
		const char	*identify();
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	commit();
		bool	rollback();

		const char	*db;
};

#endif
