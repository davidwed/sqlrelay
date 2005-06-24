// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef SYBASECONNECTION_H
#define SYBASECONNECTION_H

// this is here in case freetds ever supports array fetches
//#define FETCH_AT_ONCE		10
#define FETCH_AT_ONCE		1
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096
#define MAX_BIND_VARS		512

#define NUM_CONNECT_STRING_VARS 11

#include <rudiments/environment.h>
#include <sqlrconnection.h>

extern "C" {
	#include <ctpublic.h>
}

class freetdsconnection;

class freetdscursor : public sqlrcursor {
	friend class freetdsconnection;
	private:
				freetdscursor(sqlrconnection *conn);
				~freetdscursor();
		bool		openCursor(uint16_t id);
		bool		closeCursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		/*bool		inputBindString(const char *variable,
						uint16_t variablesize,
						const char *value,
						uint16_t valuesize,
						int16_t *isnull);
		bool		inputBindLong(const char *variable,
						uint16_t variablesize,
						uint32_t *value);
		bool		inputBindDouble(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		bool		outputBindString(const char *variable, 
						uint16_t variablesize,
						char *value, 
						uint16_t valuesize, 
						int16_t *isnull);*/
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
		const char	*getErrorMessage(bool *liveconnection);
		void		returnRowCounts();
		void		returnColumnCount();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		returnRow();
		void		cleanUpData(bool freeresult, bool freebinds);
		void		discardResults();
		void		discardCursor();

		char	*cursorname;

		void	checkRePrepare();

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

		CS_DATAFMT	parameter[MAX_BIND_VARS];
		uint16_t	paramindex;
		char		*outbindvalues[MAX_BIND_VARS];
		uint16_t	outbindvaluelengths[MAX_BIND_VARS];
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
};


class freetdsconnection : public sqlrconnection {
	friend class freetdscursor;
	public:
			freetdsconnection();
			~freetdsconnection();
	private:
		uint16_t	getNumberOfConnectStringVars();
		void	handleConnectString();
		bool	logIn();
		void	logInError(const char *error, uint16_t stage);
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		const char	*identify();
		char	bindVariablePrefix();
		void	dropTempTable(sqlrcursor *cursor,
					const char *tablename);

		CS_CONTEXT	*context;
		CS_LOCALE	*locale;
		CS_CONNECTION	*dbconn;

		const char	*sybase;
		const char	*lang;
		const char	*server;
		const char	*db;
		const char	*charset;
		const char	*language;
		const char	*encryption;
		const char	*hostname;
		const char	*packetsize;

		environment	*env;

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
