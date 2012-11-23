// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef POSTGRESQLCONNECTION_H
#define POSTGRESQLCONNECTION_H

#include <sqlrconnection.h>

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	#include <rudiments/file.h>
#endif

#include <libpq-fe.h>

class postgresqlconnection;

class postgresqlcursor : public sqlrcursor_svr {
	friend class postgresqlconnection;
	private:
				postgresqlcursor(sqlrconnection_svr *conn);
				~postgresqlcursor();
#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
		bool		open(uint16_t id);
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		deallocateStatement();
#endif
		bool		supportsNativeBinds();
#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBind(const char *variable, 
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
#endif
		bool		executeQuery(const char *query,
						uint32_t length);
		bool		knowsRowCount();
		uint64_t	rowCount();
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char * const * columnNames();
		uint16_t	columnTypeFormat();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		const char	*getColumnTypeName(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		cleanUpData(bool freeresult, bool freebinds);

		PGresult	*pgresult;
		ExecStatusType	pgstatus;
		int		ncols;
		int		nrows;
		uint64_t	affectedrows;
		int		currentrow;

		char		typenamebuffer[6];

		postgresqlconnection	*postgresqlconn;

#if defined(HAVE_POSTGRESQL_PQEXECPREPARED) && \
		defined(HAVE_POSTGRESQL_PQPREPARE)
		bool		deallocatestatement;
		int		bindcount;
		int		bindcounter;
		char		**bindvalues;
		int		*bindlengths;
		int		*bindformats;
		char		*cursorname;
#endif

		char		**columnnames;
};

class postgresqlconnection : public sqlrconnection_svr {
	friend class postgresqlcursor;
	public:
			postgresqlconnection();
			~postgresqlconnection();
	private:
		void		handleConnectString();
		bool		logIn(bool printerrors);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		const char	*identify();
		const char	*dbVersion();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		bool		getLastInsertId(uint64_t *id);
		const char	*getLastInsertIdQuery();
		const char	*bindFormat();

		int	datatypecount;
		int32_t	*datatypeids;
		char	**datatypenames;

		PGconn	*pgconn;

		const char	*host;
		const char	*port;
		const char	*options;
		const char	*db;
		uint16_t	typemangling;
		const char	*charset;
		char		*dbversion;

#ifdef HAVE_POSTGRESQL_PQOIDVALUE
		Oid	currentoid;
#endif
		char	*lastinsertidquery;

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	private:
		file	devnull;
#endif
};

#endif
