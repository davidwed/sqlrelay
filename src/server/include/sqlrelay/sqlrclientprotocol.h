// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#ifndef SQLRCLIENTPROTOCOL_H
#define SQLRCLIENTPROTOCOL_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <sqlrelay/sqlrprotocol.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/memorypool.h>

#include <sqlrelay/private/sqlrshmdata.h>

class sqlrcontroller_svr;
class sqlrconnection_svr;
class sqlrcursor_svr;
class bindvar_svr;

class SQLRSERVER_DLLSPEC sqlrclientprotocol : public sqlrprotocol {
	public:
			sqlrclientprotocol(sqlrcontroller_svr *cont,
						sqlrconnection_svr *conn);
		virtual	~sqlrclientprotocol();

		sqlrclientexitstatus_t	clientSession();
	private:
		bool	getCommand(uint16_t *command);
		sqlrcursor_svr	*getCursor(uint16_t command);
		void	noAvailableCursors(uint16_t command);
		bool	authenticateCommand();
		bool	getUserFromClient();
		bool	getPasswordFromClient();
		void	suspendSessionCommand();
		void	pingCommand();
		void	identifyCommand();
		void	autoCommitCommand();
		void	beginCommand();
		void	commitCommand();
		void	rollbackCommand();
		void	dbVersionCommand();
		void	bindFormatCommand();
		void	serverVersionCommand();
		void	selectDatabaseCommand();
		void	getCurrentDatabaseCommand();
		void	getLastInsertIdCommand();
		void	dbHostNameCommand();
		void	dbIpAddressCommand();
		bool	newQueryCommand(sqlrcursor_svr *cursor);
		bool	reExecuteQueryCommand(sqlrcursor_svr *cursor);
		bool	fetchFromBindCursorCommand(sqlrcursor_svr *cursor);
		bool	processNewQuery(sqlrcursor_svr *cursor);
		bool	processReExecuteQuery(sqlrcursor_svr *cursor);
		bool	processBindCursor(sqlrcursor_svr *cursor);
		bool	processQueryOrBindCursor(sqlrcursor_svr *cursor,
							bool reexecute,
							bool bindcursor);
		bool	getClientInfo(sqlrcursor_svr *cursor);
		bool	getQuery(sqlrcursor_svr *cursor);
		bool	getInputBinds(sqlrcursor_svr *cursor);
		bool	getOutputBinds(sqlrcursor_svr *cursor);
		bool	getBindVarCount(sqlrcursor_svr *cursor,
						uint16_t *count);
		bool	getBindVarName(sqlrcursor_svr *cursor,
						bindvar_svr *bv);
		bool	getBindVarType(bindvar_svr *bv);
		bool	getBindSize(sqlrcursor_svr *cursor,
					bindvar_svr *bv, uint32_t *maxsize);
		void	getNullBind(bindvar_svr *bv);
		bool	getStringBind(sqlrcursor_svr *cursor,
						bindvar_svr *bv);
		bool	getIntegerBind(bindvar_svr *bv);
		bool	getDoubleBind(bindvar_svr *bv);
		bool	getDateBind(bindvar_svr *bv);
		bool	getLobBind(sqlrcursor_svr *cursor, bindvar_svr *bv);
		bool	getSendColumnInfo();
		bool	getSkipAndFetch(sqlrcursor_svr *cursor);
		void	returnResultSetHeader(sqlrcursor_svr *cursor);
		void	returnColumnInfo(sqlrcursor_svr *cursor,
							uint16_t format);
		void	sendRowCounts(bool knowsactual, uint64_t actual,
					bool knowsaffected, uint64_t affected);
		void	returnOutputBindValues(sqlrcursor_svr *cursor);
		void	returnOutputBindBlob(sqlrcursor_svr *cursor,
							uint16_t index);
		void	returnOutputBindClob(sqlrcursor_svr *cursor,
							uint16_t index);
		void	sendLobOutputBind(sqlrcursor_svr *cursor,
							uint16_t index);
		void	sendColumnDefinition(const char *name,
						uint16_t namelen,
						uint16_t type, 
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement);
		void	sendColumnDefinitionString(const char *name,
						uint16_t namelen,
						const char *type, 
						uint16_t typelen,
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement);
		bool	returnResultSetData(sqlrcursor_svr *cursor,
						bool getskipandfetch);
		void	returnRow(sqlrcursor_svr *cursor);
		void	sendField(const char *data, uint32_t size);
		void	sendNullField();
		void	sendLobField(sqlrcursor_svr *cursor, uint32_t col);
		void	startSendingLong(uint64_t longlength);
		void	sendLongSegment(const char *data, uint32_t size);
		void	endSendingLong();
		void	returnError(bool disconnect);
		void	returnError(sqlrcursor_svr *cursor, bool disconnect);
		bool	fetchResultSetCommand(sqlrcursor_svr *cursor);
		void	abortResultSetCommand(sqlrcursor_svr *cursor);
		void	suspendResultSetCommand(sqlrcursor_svr *cursor);
		bool	resumeResultSetCommand(sqlrcursor_svr *cursor);
		bool	getDatabaseListCommand(sqlrcursor_svr *cursor);
		bool	getTableListCommand(sqlrcursor_svr *cursor);
		bool	getColumnListCommand(sqlrcursor_svr *cursor);
		bool	getListCommand(sqlrcursor_svr *cursor,
						int which, bool gettable);
		bool	getListByApiCall(sqlrcursor_svr *cursor,
						int which,
						const char *table,
						const char *wild);
		bool	getListByQuery(sqlrcursor_svr *cursor,
						int which,
						const char *table,
						const char *wild);
		bool	buildListQuery(sqlrcursor_svr *cursor,
						const char *query,
						const char *wild,
						const char *table);
		void	escapeParameter(stringbuffer *buffer,
						const char *parameter);
		bool	getQueryTreeCommand(sqlrcursor_svr *cursor);

		stringbuffer	debugstr;

		int32_t		idleclienttimeout;

		uint64_t	maxclientinfolength;
		uint32_t	maxquerysize;
		uint16_t	maxbindcount;
		uint16_t	maxbindnamelength;
		uint32_t	maxstringbindvaluelength;
		uint32_t	maxlobbindvaluelength;
		uint32_t	maxerrorlength;
		bool		waitfordowndb;

		char		userbuffer[USERSIZE];
		char		passwordbuffer[USERSIZE];

		char		*clientinfo;
		uint64_t	clientinfolen;

		memorypool	*bindpool;

		uint64_t	skip;
		uint64_t	fetch;

		char		lobbuffer[32768];
};

#endif
