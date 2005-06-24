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

class mdbtoolscursor : public sqlrcursor {
	friend class mdbtoolsconnection;
	private:
				mdbtoolscursor(sqlrconnection *conn);
		bool		openCursor(uint16_t id);
		bool		closeCursor();
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
		const char	*getErrorMessage(bool *liveconnection);
		void		returnRowCounts();
		void		returnColumnCount();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		returnRow();
		void		cleanUpData(bool freeresult, bool freebinds);

		mdbtoolsconnection	*mdbtoolsconn;

		MdbSQL	mdbsql;
};

class mdbtoolsconnection : public sqlrconnection {
	friend class mdbtoolscursor;
	public:
			mdbtoolsconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void	handleConnectString();
		bool	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
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
