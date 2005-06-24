// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef LAGOCONNECTION_H
#define LAGOCONNECTION_H

#define NUM_CONNECT_STRING_VARS 5

#include <sqlrconnection.h>

#include <lago.h>

class lagoconnection;

class lagocursor : public sqlrcursor {
	friend class lagoconnection;
	private:
				lagocursor(sqlrconnection *conn);
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

		LRST		lagoresult;
		uint32_t	ncols;
		uint64_t	nrows;

		lagoconnection	*lagoconn;
};

class lagoconnection : public sqlrconnection {
	friend class lagocursor;
	private:
		uint16_t	getNumberOfConnectStringVars();
		void	handleConnectString();
		bool	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		bool	ping();
		const char	*identify();
		bool	isTransactional();
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	commit();
		bool	rollback();

		LCTX	lagocontext;

		const char	*host;
		const char	*port;
		const char	*db;
};

#endif
