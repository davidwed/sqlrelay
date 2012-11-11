// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef MDBTOOLSCONNECTION_H
#define MDBTOOLSCONNECTION_H

#define NUM_CONNECT_STRING_VARS 1

#include <sqlrconnection.h>
#ifndef MAIN
extern "C" {
	#include <mdbsql.h>
}
#endif

#include <rudiments/regularexpression.h>

class mdbtoolsconnection;

#ifndef MAIN
enum cursortype_t {
	QUERY_CURSORTYPE=0,
	DB_LIST_CURSORTYPE,
	TABLE_LIST_CURSORTYPE,
	COLUMN_LIST_CURSORTYPE
};

class mdbtoolscursor : public sqlrcursor_svr {
	friend class mdbtoolsconnection;
	private:
				mdbtoolscursor(sqlrconnection_svr *conn);
				~mdbtoolscursor();
		bool		openCursor(uint16_t id);
		bool		closeCursor();
		bool		supportsNativeBinds();
		bool		executeQuery(const char *query,
						uint32_t length);
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
		void		getField(uint32_t col,
						const char **field,
						uint64_t *fieldlength,
						bool *blob,
						bool *null);
		void		cleanUpData(bool freeresult, bool freebinds);
		bool		getDatabaseList(const char *wild);
		bool		getTableList(const char *wild);
		bool		getColumnList(const char *table,
						const char *wild);
		void		resetListValues(const char *wild);
		bool		matchCurrentWild(const char *value);

		mdbtoolsconnection	*mdbtoolsconn;

		char	**columnnames;

		void		*mdbsql;

		MdbHandle	*mdb;
		uint32_t	currentlistindex;
		MdbCatalogEntry	*currenttable;
		MdbTableDef	*currenttabledef;
		MdbColumn	*currentcolumn;
		char		*currentcolumnsize;
		char		*currentcolumnprec;
		char		*currentcolumnscale;
		regularexpression	*currentwild;

		cursortype_t	cursortype;
};
#endif

class mdbtoolsconnection : public sqlrconnection_svr {
	friend class mdbtoolscursor;
	public:
			mdbtoolsconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void	handleConnectString();
		bool	logIn(bool printerrors);
		sqlrcursor_svr	*initCursor();
		void	deleteCursor(sqlrcursor_svr *curs);
		void	logOut();
		bool	isTransactional();
		bool	ping();
		const char	*identify();
		const char	*dbVersion();
		bool	getListsByApiCalls();
		bool	getDatabaseList(sqlrcursor_svr *cursor,
						const char *wild);
		bool	getTableList(sqlrcursor_svr *cursor,
						const char *wild);
		bool	getColumnList(sqlrcursor_svr *cursor,
						const char *table,
						const char *wild);
		bool	setIsolationLevel(const char *isolevel);
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	commit();
		bool	rollback();
		void	errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t	*errorcode,
					bool *liveconnection);

		const char	*db;
};

#endif
