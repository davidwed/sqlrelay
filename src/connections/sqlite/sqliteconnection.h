// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#define NUM_CONNECT_STRING_VARS 1

#include <pthread.h>

#include <sqlrconnection.h>

extern "C" {
#ifdef SQLITE3
	#include <sqlite3.h>
#else
	#include <sqlite.h>
#endif
}

#include <config.h>


class sqliteconnection;

class sqlitecursor : public sqlrcursor {
	friend class sqliteconnection;
	private:
				sqlitecursor(sqlrconnection *conn);
				~sqlitecursor();
		bool		executeQuery(const char *query,
						long length,
						bool execute);
		int		runQuery(stringbuffer *newquery,
						const char *query);
		const char	*getErrorMessage(bool *liveconnection);
		void		returnRowCounts();
		void		returnColumnCount();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		returnRow();
		void		cleanUpData(bool freeresult, bool freebinds);

		stringbuffer	*newquery;
		char		**result;
		char		**columnnames;
		int		nrow;
		int		ncolumn;

		int		rowindex;

		sqliteconnection	*sqliteconn;
};

class sqliteconnection : public sqlrconnection {
	friend class sqlitecursor;
	public:
				sqliteconnection();
	private:
		int		getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn();
		sqlrcursor	*initCursor();
		void		deleteCursor(sqlrcursor *curs);
		void		logOut();
		bool		ping();
		const char	*identify();
#ifndef SQLITE_TRANSACTIONAL
		bool		isTransactional();
		bool		commit();
		bool		rollback();
#endif

		const char	*db;

#ifdef SQLITE3
		sqlite3	*sqliteptr;
#else
		sqlite	*sqliteptr;
#endif
		char	*errmesg;
};

#endif
