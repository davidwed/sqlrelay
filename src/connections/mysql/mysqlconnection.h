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
		bool		executeQuery(const char *query,
						long length,
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
		int		getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn();
#ifdef HAVE_MYSQL_CHANGE_USER
		bool		changeUser(const char *newuser,
						const char *newpassword);
#endif
		sqlrcursor	*initCursor();
		void		deleteCursor(sqlrcursor *curs);
		void		logOut();
		bool		isTransactional();
#ifdef HAVE_MYSQL_PING
		bool		ping();
#endif
		const char	*identify();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();

		MYSQL	mysql;
		int	connected;

		const char	*db;
		const char	*host;
		const char	*port;
		const char	*socket;
};

#endif
