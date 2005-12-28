// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#define NUM_CONNECT_STRING_VARS 1

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

class sqlitecursor : public sqlrcursor_svr {
	friend class sqliteconnection;
	private:
				sqlitecursor(sqlrconnection_svr *conn);
				~sqlitecursor();
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
		int		runQuery(const char *query);
		const char	*getErrorMessage(bool *liveconnection);
		bool		knowsRowCount();
		uint64_t	rowCount();
		bool		knowsAffectedRows();
		uint64_t	affectedRows();
		uint32_t	colCount();
		uint16_t	columnTypeFormat();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		returnRow();
		void		cleanUpData(bool freeresult, bool freebinds);

		char		**result;
		char		**columnnames;
		int		nrow;
		int		ncolumn;
		int		rowindex;

		sqliteconnection	*sqliteconn;
};

class sqliteconnection : public sqlrconnection_svr {
	friend class sqlitecursor;
	public:
				sqliteconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		bool		supportsNativeBinds();
		void		handleConnectString();
		bool		logIn();
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
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
