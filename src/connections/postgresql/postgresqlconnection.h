// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef POSTGRESQLCONNECTION_H
#define POSTGRESQLCONNECTION_H

#define NUM_CONNECT_STRING_VARS 8

#include <sqlrconnection.h>

#include <libpq-fe.h>

class postgresqlconnection;

class postgresqlcursor : public sqlrcursor {
	friend class postgresqlconnection;
	private:
			postgresqlcursor(sqlrconnection *conn);

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

		int		ddlquery;
		PGresult	*pgresult;
		ExecStatusType	pgstatus;
		int		ncols;
		int		nrows;
		int		affectedrows;
		int		currentrow;

		postgresqlconnection	*postgresqlconn;
};

class postgresqlconnection : public sqlrconnection {
	friend class postgresqlcursor;
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

		void	endSession();
		void	dropTable(const char *table);

		int	datatypecount;
		long	*datatypeids;
		char	**datatypenames;

		PGconn	*pgconn;

		char	*host;
		char	*port;
		char	*options;
		char	*tty;
		char	*db;
		int	typemangling;
};

#endif
