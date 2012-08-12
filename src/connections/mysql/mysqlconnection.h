// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H


#include <sqlrconnection.h>
#include <sqlwriter.h>

#include <rudiments/regularexpression.h>

#include <mysql.h>

#ifdef HAVE_MYSQL_STMT_PREPARE
	#define NUM_CONNECT_STRING_VARS 7
	#define MAX_SELECT_LIST_SIZE	256
	#define MAX_ITEM_BUFFER_SIZE	32768
#else
	#define NUM_CONNECT_STRING_VARS 6
#endif

class mysqlconnection;

class mysqlcursor : public sqlrcursor_svr {
	friend class mysqlconnection;
	private:
				mysqlcursor(sqlrconnection_svr *conn);
				~mysqlcursor();
#ifdef HAVE_MYSQL_STMT_PREPARE
		bool		openCursor(uint16_t id);
		bool		closeCursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
#endif
		bool		supportsNativeBinds();
#ifdef HAVE_MYSQL_STMT_PREPARE
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
		bool		inputBindDate(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
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
		void		errorMessage(const char **errorstring,
						int64_t	*errorcode,
						bool *liveconnection);
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

		MYSQL_RES	*mysqlresult;
		MYSQL_FIELD	*mysqlfield;
		unsigned int	ncols;
		my_ulonglong	nrows;
		my_ulonglong	affectedrows;
		int		queryresult;
		char		**columnnames;

#ifdef HAVE_MYSQL_STMT_PREPARE
		MYSQL_STMT	*stmt;
		bool		stmtfreeresult;

		MYSQL_BIND	fieldbind[MAX_SELECT_LIST_SIZE];
		char		field[MAX_SELECT_LIST_SIZE]
					[MAX_ITEM_BUFFER_SIZE];
		my_bool		isnull[MAX_SELECT_LIST_SIZE];
		unsigned long	fieldlength[MAX_SELECT_LIST_SIZE];

		int		bindcount;
		int		bindcounter;
		MYSQL_BIND	bind[MAXVAR];
		unsigned long	bindvaluesize[MAXVAR];

		bool		usestmtprepare;

		regularexpression	unsupportedbystmt;
#endif
		MYSQL_ROW	mysqlrow;
		unsigned long	*mysqlrowlengths;

		mysqlconnection	*mysqlconn;
};

class mysqlconnection : public sqlrconnection_svr {
	friend class mysqlcursor;
	public:
				mysqlconnection();
				~mysqlconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn(bool printerrors);
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
		const char	*dbVersion();
		const char	*bindFormat();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		bool		getLastInsertId(uint64_t *id, char **error);
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		sqlwriter	*getSqlWriter();
#ifdef HAVE_MYSQL_STMT_PREPARE
		short		nonNullBindValue();
		short		nullBindValue();
#endif
		void		endSession();

		MYSQL	mysql;
		bool	connected;

		const char	*db;
		const char	*host;
		const char	*port;
		const char	*socket;
		const char	*charset;

		char	*dbversion;

#ifdef HAVE_MYSQL_OPT_RECONNECT
		static const my_bool	mytrue;
#endif

		bool		firstquery;
};

#endif
