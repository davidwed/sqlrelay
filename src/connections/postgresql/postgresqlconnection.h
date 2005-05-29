// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef POSTGRESQLCONNECTION_H
#define POSTGRESQLCONNECTION_H

#define NUM_CONNECT_STRING_VARS 8

#include <sqlrconnection.h>

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	#include <rudiments/file.h>
#endif

#include <libpq-fe.h>

class postgresqlconnection;

class postgresqlcursor : public sqlrcursor {
	friend class postgresqlconnection;
	private:
				postgresqlcursor(sqlrconnection *conn);
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

		PGresult	*pgresult;
		ExecStatusType	pgstatus;
		int		ncols;
		int		nrows;
		uint32_t	affectedrows;
		int		currentrow;

		postgresqlconnection	*postgresqlconn;
};

class postgresqlconnection : public sqlrconnection {
	friend class postgresqlcursor;
	private:
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn();
		sqlrcursor	*initCursor();
		void		deleteCursor(sqlrcursor *curs);
		void		logOut();
		const char	*identify();

		void		endSession();
		void		dropTable(const char *table);

		int	datatypecount;
		int32_t	*datatypeids;
		char	**datatypenames;

		PGconn	*pgconn;

		const char	*host;
		const char	*port;
		const char	*options;
		const char	*tty;
		const char	*db;
		uint16_t	typemangling;

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	public:
			postgresqlconnection();
			~postgresqlconnection();
	private:
		file	devnull;
#endif
};

#endif
