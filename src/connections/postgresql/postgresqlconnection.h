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
		bool	executeQuery(const char *query,
					long length,
					bool execute);
		char	*getErrorMessage(bool *liveconnection);
		void	returnRowCounts();
		void	returnColumnCount();
		void	returnColumnInfo();
		bool	noRowsToReturn();
		bool	skipRow();
		bool	fetchRow();
		void	returnRow();
		void	cleanUpData(bool freerows, bool freecols,
							bool freebinds);

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
		bool	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		bool	ping();
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

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	public:
			postgresqlconnection();
			~postgresqlconnection();
	private:
		int	devnull;
#endif
};

#endif
