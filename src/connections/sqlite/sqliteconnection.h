// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#define NUM_CONNECT_STRING_VARS 1

#include <sqlrconnection.h>
#include <rudiments/semaphoreset.h>

extern "C" {
	#include <sqlite.h>
}

#include <pthread.h>

#include <config.h>


class sqliteconnection;

class sqlitecursor : public sqlrcursor {
	friend class sqliteconnection;
	private:
				sqlitecursor(sqlrconnection *conn);
				~sqlitecursor();
			int	executeQuery(const char *query, long length,
						unsigned short execute);
			int	runQuery(stringbuffer *newquery,
						const char *query);
			char	*getErrorMessage(int *liveconnection);
			void	returnRowCounts();
			void	returnColumnCount();
			void	returnColumnInfo();
			int	noRowsToReturn();
			int	skipRow();
			int	fetchRow();
			void	returnRow();
			void	cleanUpData();

			stringbuffer	*newquery;
			char		**result;
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
		int	getNumberOfConnectStringVars();
		void	handleConnectString();
		int	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		int	ping();
		char	*identify();
#ifndef SQLITE_TRANSACTIONAL
		int	isTransactional();
		int	commit();
		int	rollback();
#endif

		char	*db;

		sqlite	*sqliteptr;
		char	*errmesg;
};

#endif
