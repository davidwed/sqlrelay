// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#define NUM_CONNECT_STRING_VARS 6

#include <sqlrconnection.h>

#include <mysql.h>

class mysqlconnection;

class mysqlcursor : public sqlrcursor {
	friend class mysqlconnection;
	private:
			mysqlcursor(sqlrconnection *conn);
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
		void	cleanUpData(bool freerows, bool freecols,
							bool freebinds);

		MYSQL_RES	*mysqlresult;
		MYSQL_FIELD	*mysqlfield;
		MYSQL_ROW	mysqlrow;
		int		ncols;
		int		nrows;
		int		affectedrows;
		int		queryresult;

		mysqlconnection	*mysqlconn;
};

class mysqlconnection : public sqlrconnection {
	friend class mysqlcursor;
	public:
			mysqlconnection();
	private:
		int	getNumberOfConnectStringVars();
		void	handleConnectString();
		int	logIn();
#ifdef HAVE_MYSQL_CHANGE_USER
		int	changeUser(const char *newuser,
					const char *newpassword);
#endif
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		int	isTransactional();
#ifdef HAVE_MYSQL_PING
		int	ping();
#endif
		char	*identify();
		unsigned short	autoCommitOn();
		unsigned short	autoCommitOff();
		int	commit();
		int	rollback();

		MYSQL	mysql;
		int	connected;

		char	*db;
		char	*host;
		char	*port;
		char	*socket;
};

#endif
