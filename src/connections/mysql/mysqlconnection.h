// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#define NUM_CONNECT_STRING_VARS 6

#include <sqlrconnection.h>

#include <mysql.h>

class mysqlconnection;

class mysqlcursor : public sqlrcursor_svr {
	friend class mysqlconnection;
	private:
				mysqlcursor(sqlrconnection_svr *conn);
				~mysqlcursor();
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
		const char	*errorMessage(bool *liveconnection);
		bool		knowsRowCount();
		uint64_t	rowCount();
		bool		knowsAffectedRows();
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char * const * columnNames();
		uint16_t	columnTypeFormat();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		returnRow();
		void		cleanUpData(bool freeresult, bool freebinds);

		MYSQL_RES	*mysqlresult;
		MYSQL_FIELD	*mysqlfield;
		MYSQL_ROW	mysqlrow;
		unsigned int	ncols;
		my_ulonglong	nrows;
		my_ulonglong	affectedrows;
		int		queryresult;
		char		**columnnames;

		mysqlconnection	*mysqlconn;
};

class mysqlconnection : public sqlrconnection_svr {
	friend class mysqlcursor;
	public:
				mysqlconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		bool		supportsNativeBinds();
		void		handleConnectString();
		bool		logIn();
#ifdef HAVE_MYSQL_CHANGE_USER
		bool		changeUser(const char *newuser,
						const char *newpassword);
#endif
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
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
		bool	connected;

		const char	*db;
		const char	*host;
		const char	*port;
		const char	*socket;
};

#endif
