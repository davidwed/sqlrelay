// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef SYBASECONNECTION_H
#define SYBASECONNECTION_H

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096
#define MAX_BIND_VARS		512

#define NUM_CONNECT_STRING_VARS 10

#include <rudiments/environment.h>
#include <sqlrconnection.h>

extern "C" {
	#include <ctpublic.h>
}

class sybaseconnection;

class sybasecursor : public sqlrcursor_svr {
	friend class sybaseconnection;
	private:
				sybasecursor(sqlrconnection_svr *conn);
				~sybasecursor();
		bool		openCursor(uint16_t id);
		bool		closeCursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
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
		bool		outputBindString(const char *variable, 
						uint16_t variablesize,
						char *value, 
						uint16_t valuesize, 
						int16_t *isnull);
		bool		outputBindInteger(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBindDouble(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
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
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		nextRow();
		void		cleanUpData(bool freeresult, bool freebinds);
		void		discardResults();
		void		discardCursor();

		char		*cursorname;
		size_t		cursornamelength;

		void		checkRePrepare();

		CS_COMMAND	*languagecmd;
		CS_COMMAND	*cursorcmd;
		CS_COMMAND	*cmd;
		CS_INT		results;
		CS_INT		resultstype;
		CS_INT		ncols;
		CS_INT		affectedrows;

		CS_INT		rowsread;
		CS_INT		row;
		CS_INT		maxrow;
		CS_INT		totalrows;

		CS_DATAFMT	parameter[MAX_BIND_VARS];
		uint16_t	paramindex;
		CS_INT		outbindtype[MAX_BIND_VARS];
		char		*outbindstrings[MAX_BIND_VARS];
		uint16_t	outbindstringlengths[MAX_BIND_VARS];
		int64_t		*outbindints[MAX_BIND_VARS];
		double		*outbinddoubles[MAX_BIND_VARS];
		uint16_t	outbindindex;

		CS_DATAFMT	column[MAX_SELECT_LIST_SIZE];
		char		*columnnames[MAX_SELECT_LIST_SIZE];
		char		data[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE]
					[MAX_ITEM_BUFFER_SIZE];
		CS_INT		datalength[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE];
		CS_SMALLINT	nullindicator[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE];

		const char	*query;
		uint32_t	length;
		bool		prepared;
		bool		clean;

		regularexpression	cursorquery;
		regularexpression	rpcquery;

		bool			isrpcquery;

		sybaseconnection	*sybaseconn;
};


class sybaseconnection : public sqlrconnection_svr {
	friend class sybasecursor;
	public:
			sybaseconnection();
			~sybaseconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn(bool printerrors);
		void		logInError(const char *error, uint16_t stage);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		const char	*identify();
		const char	*dbVersion();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(bool wild);
		const char	*bindFormat();
		const char	*beginTransactionQuery();
		char		bindVariablePrefix();
		void		dropTempTable(sqlrcursor_svr *cursor,
						const char *tablename);
		bool		commit();
		bool		rollback();

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

		static	stringbuffer	*errorstring;
		static	bool		deadconnection;

		static	CS_RETCODE	csMessageCallback(CS_CONTEXT *ctxt,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	clientMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	serverMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp);
};

#endif
