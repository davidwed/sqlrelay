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

class postgresqlcursor : public sqlrcursor_svr {
	friend class postgresqlconnection;
	private:
				postgresqlcursor(sqlrconnection_svr *conn);
#ifdef HAVE_POSTGRESQL_PQEXECPARAMS
				~postgresqlcursor();
		bool		openCursor(uint16_t id);
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
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
#endif
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

		PGresult	*pgresult;
		ExecStatusType	pgstatus;
		int		ncols;
		int		nrows;
		uint64_t	affectedrows;
		int		currentrow;

		postgresqlconnection	*postgresqlconn;

#ifdef HAVE_POSTGRESQL_PQEXECPARAMS
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
	private:
		uint16_t	getNumberOfConnectStringVars();
		bool		supportsNativeBinds();
		void		handleConnectString();
		bool		logIn();
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
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
		const char	*db;
		uint16_t	typemangling;
#ifdef HAVE_POSTGRESQL_PQEXECPARAMS
		bool		fakebinds;
#endif

#ifndef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR
	public:
			postgresqlconnection();
			~postgresqlconnection();
	private:
		file	devnull;
#endif
};

#endif
