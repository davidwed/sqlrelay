// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#define NUM_CONNECT_STRING_VARS 1

#include <sqlrconnection.h>
#include <rudiments/regularexpression.h>

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
/*#ifdef HAVE_SQLITE3_BIND_INT
		bool		prepareQuery(const char *query,
						uint32_t length);
#endif*/
		bool		supportsNativeBinds();

/*#ifdef HAVE_SQLITE3_BIND_INT
		bool		inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		bool		inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
#endif*/
		bool		executeQuery(const char *query,
						uint32_t length);
		int		runQuery(const char *query);
		void		selectLastInsertRowId();
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
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		cleanUpData(bool freeresult, bool freebinds);

		char		**result;
		char		**columnnames;
		int		nrow;
		int		ncolumn;
		int		rowindex;
		bool		lastinsertrowid;

#ifdef HAVE_SQLITE3_BIND_INT
#endif

		regularexpression	selectlastinsertrowid;

		sqliteconnection	*sqliteconn;
};

class sqliteconnection : public sqlrconnection_svr {
	friend class sqlitecursor;
	public:
				sqliteconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn(bool printerrors);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		bool		ping();
		const char	*identify();
		const char	*dbVersion();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(bool wild);
#ifdef SQLITE_TRANSACTIONAL
		const char	*setIsolationLevelQuery();
#endif
		bool		getLastInsertId(uint64_t *id, char **error);
#ifdef SQLITE3
		char		*duplicate(const char *str);
#endif
#ifndef SQLITE_TRANSACTIONAL
		bool		isTransactional();
		bool		commit();
		bool		rollback();
#endif
		void		errorMessage(const char **errorstring,
						int64_t	*errorcode,
						bool *liveconnection);

		const char	*db;

#ifdef SQLITE3
		sqlite3	*sqliteptr;
#else
		sqlite	*sqliteptr;
#endif
		char	*errmesg;
		int64_t	errcode;
};

#endif
