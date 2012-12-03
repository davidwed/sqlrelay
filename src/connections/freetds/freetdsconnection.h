// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef SYBASECONNECTION_H
#define SYBASECONNECTION_H

// this is here in case freetds ever supports array fetches
//#define FETCH_AT_ONCE		10
#define FETCH_AT_ONCE		1
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096

//#define FREETDS_SUPPORTS_CURSORS

#include <rudiments/environment.h>
#include <sqlrcontroller.h>
#include <sqlrconnection.h>

extern "C" {
	#include <ctpublic.h>
}

struct datebind {
        int16_t         *year;
        int16_t         *month;
        int16_t         *day;
        int16_t         *hour;
        int16_t         *minute;
        int16_t         *second;
        int32_t         *microsecond;
        const char      **tz;
};

class freetdsconnection;

class freetdscursor : public sqlrcursor_svr {
	friend class freetdsconnection;
	private:
				freetdscursor(sqlrconnection_svr *conn);
				~freetdscursor();
		bool		open(uint16_t id);
		bool		close();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		supportsNativeBinds();
#ifdef FREETDS_SUPPORTS_CURSORS
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
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						char *value, 
						uint16_t valuesize, 
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
#endif
		bool		executeQuery(const char *query,
						uint32_t length);
		bool		knowsAffectedRows();
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		uint16_t	getColumnIsNullable(uint32_t col);
		uint16_t	getColumnIsPartOfKey(uint32_t col);
		uint16_t	getColumnIsUnsigned(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
		uint16_t	getColumnIsAutoIncrement(uint32_t col);
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		nextRow();
		void		cleanUpData();
		void		discardResults();
		void		discardCursor();

		char		*cursorname;
		size_t		cursornamelength;

		void		checkRePrepare();

		uint32_t	majorversion;
		uint32_t	minorversion;
		uint32_t	patchlevel;

		CS_COMMAND	*languagecmd;
		CS_COMMAND	*cursorcmd;
		CS_COMMAND	*cmd;
		CS_INT		results;
		CS_INT		resultstype;
		CS_INT		ncols;
		bool		knowsaffectedrows;
		CS_INT		affectedrows;

		CS_INT		rowsread;
		CS_INT		row;
		CS_INT		maxrow;
		CS_INT		totalrows;

		CS_DATAFMT	*parameter;
		uint16_t	paramindex;
		CS_INT		*outbindtype;
		char		**outbindstrings;
		uint16_t	*outbindstringlengths;
		int64_t		**outbindints;
		double		**outbinddoubles;
		datebind	*outbinddates;
		uint16_t	outbindindex;

		CS_DATAFMT	column[MAX_SELECT_LIST_SIZE];
		char		data[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE]
					[MAX_ITEM_BUFFER_SIZE];
		CS_INT		datalength[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE];
		CS_SMALLINT	nullindicator[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE];

		char		*query;
		uint32_t	length;
		bool		prepared;
		bool		clean;

		regularexpression	cursorquery;
		regularexpression	rpcquery;
		bool			isrpcquery;

		freetdsconnection	*freetdsconn;

		unsigned short		opencount;
};


class freetdsconnection : public sqlrconnection_svr {
	friend class freetdscursor;
	public:
			freetdsconnection(sqlrcontroller_svr *cont);
			~freetdsconnection();
	private:
		void	handleConnectString();
		bool	logIn(bool printerrors);
		void	logInError(const char *error, uint16_t stage);
		sqlrcursor_svr	*initCursor();
		void	deleteCursor(sqlrcursor_svr *curs);
		void	logOut();
		const char	*identify();
		const char	*dbVersion();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		const char	*getLastInsertIdQuery();
		const char	*bindFormat();
		const char	*beginTransactionQuery();
		char	bindVariablePrefix();
		const char	*tempTableDropPrefix();
		bool		commit();
		bool		rollback();
		void		errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t	*errorcode,
					bool *liveconnection);

		CS_CONTEXT	*context;
		CS_LOCALE	*locale;
		CS_CONNECTION	*dbconn;

		const char	*sybase;
		const char	*lang;
		const char	*server;
		const char	*db;
		const char	*charset;
		const char	*language;
		const char	*hostname;
		const char	*packetsize;

		bool		dbused;

		char		*dbversion;

		static	stringbuffer	errorstring;
		static	int64_t		errorcode;
		static	bool		liveconnection;

		static	CS_RETCODE	csMessageCallback(CS_CONTEXT *ctxt,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	clientMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	serverMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp);

		freetdscursor	*singlecursor;
		unsigned short	singlecursorrefcount;
};

#endif
