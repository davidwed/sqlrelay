// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef FREETDSCONNECTION_H
#define FREETDSCONNECTION_H

// FETCH_AT_ONCE must be 1 until freetds supports array fetches
#define FETCH_AT_ONCE		1
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096

// this code is here in case freetds ever supports bind vars
#define MAX_BIND_VARS		512

#define NUM_CONNECT_STRING_VARS 10

#include <rudiments/environment.h>
#include <sqlrconnection.h>

extern "C" {
	#include <ctpublic.h>
	#ifndef HAVE_FREETDS_FUNCTION_DEFINITIONS
		#include <ctfunctions.h>
	#endif
}

class freetdsconnection;

class freetdscursor : public sqlrcursor {
	friend class freetdsconnection;
	private:
			freetdscursor(sqlrconnection *conn);
			~freetdscursor();
		int	openCursor(int id);
		int	prepareQuery(const char *query, long length);

		// this code is here in case freetds ever supports bind vars
		/*int	inputBindString(const char *variable,
						unsigned short variablesize,
						const char *value,
						unsigned short valuesize,
						short *isnull);
		int	inputBindLong(const char *variable,
						unsigned short variablesize,
						unsigned long *value);
		int	inputBindDouble(const char *variable,
						unsigned short variablesize,
						double *value,
						unsigned short precision,
						unsigned short scale);
		int	outputBindString(const char *variable, 
						unsigned short variablesize,
						char *value, 
						unsigned short valuesize, 
						short *isnull);*/

		int	executeQuery(const char *query, long length,
					unsigned short execute);
		char	*getErrorMessage(int *liveconnection);
		void	returnRowCounts();
		void	returnColumnCount();
		void	returnColumnInfo();
		int	noRowsToReturn();
		int	skipRow();
		int	fetchRow();
		void	returnRow();
		void	cleanUpData();

		double		tdsversion;
		int		returnedcolumns;

		CS_COMMAND	*cmd;
		CS_INT		results_type;
		CS_INT		ncols;
		CS_INT		affectedrows;

		CS_INT		rowsread;
		int		row;
		int		maxrow;
		int		totalrows;

		// this code is here in case freetds ever supports bind vars
		/*CS_DATAFMT	parameter[MAX_BIND_VARS];
		int		paramindex;*/

		CS_DATAFMT	column[MAX_SELECT_LIST_SIZE];
		char		data[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE]
					[MAX_ITEM_BUFFER_SIZE];
		CS_INT		datalength[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE];
		CS_SMALLINT	nullindicator[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE];

		freetdsconnection	*freetdsconn;
};

class freetdsconnection : public sqlrconnection {
	friend class freetdscursor;
	public:
			freetdsconnection();
			~freetdsconnection();
	private:
		int	getNumberOfConnectStringVars();
		void	handleConnectString();
		int	logIn();
		void	logInError(const char *error, int stage);
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		char	*identify();
		char	bindVariablePrefix();

		CS_CONTEXT	*context;
		CS_LOCALE	*locale;
		CS_CONNECTION	*dbconn;

		char		*sybase;
		char		*server;
		char		*db;
		char		*charset;
		char		*language;
		char		*encryption;
		int		enc;
		char		*hostname;
		char		*packetsize;

		environment	*env;

		static	stringbuffer	*errorstring;
		static	int		deadconnection;

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
