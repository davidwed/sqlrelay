// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <sqlwriter.h>

#include <rudiments/regularexpression.h>

#include <mysql.h>

#ifdef HAVE_MYSQL_STMT_PREPARE
	#define MAX_SELECT_LIST_SIZE	256
	#define MAX_ITEM_BUFFER_SIZE	32768
#endif

class mysqlconnection;

class mysqlcursor : public sqlrcursor_svr {
	friend class mysqlconnection;
	private:
				mysqlcursor(sqlrconnection_svr *conn);
				~mysqlcursor();
#ifdef HAVE_MYSQL_STMT_PREPARE
		bool		open(uint16_t id);
		bool		close();
		bool		prepareQuery(const char *query,
						uint32_t length);
#endif
		bool		supportsNativeBinds();
#ifdef HAVE_MYSQL_STMT_PREPARE
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
		bool		inputBind(const char *variable,
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
						uint32_t length);
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		bool		knowsRowCount();
		uint64_t	rowCount();
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		uint16_t	getColumnIsNullable(uint32_t col);
		uint16_t	getColumnIsPrimaryKey(uint32_t col);
		uint16_t	getColumnIsUnique(uint32_t col);
		uint16_t	getColumnIsPartOfKey(uint32_t col);
		uint16_t	getColumnIsUnsigned(uint32_t col);
		uint16_t	getColumnIsZeroFilled(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
		uint16_t	getColumnIsAutoIncrement(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		cleanUpData(bool freeresult, bool freebinds);

		MYSQL_RES	*mysqlresult;
		MYSQL_FIELD	*mysqlfields[MAX_SELECT_LIST_SIZE];
		uint32_t	mysqlfieldindex;
		unsigned int	ncols;
		my_ulonglong	nrows;
		my_ulonglong	affectedrows;
		int		queryresult;

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
		MYSQL_BIND	*bind;
		unsigned long	*bindvaluesize;

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
				mysqlconnection(sqlrcontroller_svr *cont);
				~mysqlconnection();
	private:
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
		bool		getLastInsertId(uint64_t *id);
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
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

		static const my_bool	mytrue;
		static const my_bool	myfalse;

		bool		firstquery;
};

#endif
