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
		int	executeQuery(const char *query, long length,
					unsigned short execute);
		char	*getErrorMessage(int *liveconnection);
		void	returnRowCounts();
		void	returnColumnCount();
		void	returnColumnInfo();
		int	noRowsToReturn();
		int	skipRow();
		int	fetchRow();
		void	returnRow();
		void	cleanUpData();

		LRST	lagoresult;
		int	ncols;
		int	nrows;
		int	affectedrows;

		lagoconnection	*lagoconn;
};

class lagoconnection : public sqlrconnection {
	friend class lagocursor;
	private:
		int	getNumberOfConnectStringVars();
		void	handleConnectString();
		int	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		int	ping();
		char	*identify();
		int	isTransactional();
		unsigned short	autoCommitOn();
		unsigned short	autoCommitOff();
		int	commit();
		int	rollback();

		LCTX	lagocontext;

		char	*host;
		char	*port;
		char	*db;
};

#endif
