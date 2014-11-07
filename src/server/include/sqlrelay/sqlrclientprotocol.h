// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#ifndef SQLRCLIENTPROTOCOL_H
#define SQLRCLIENTPROTOCOL_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <sqlrelay/sqlrprotocol.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/memorypool.h>

#include <sqlrelay/private/sqlrshmdata.h>

class sqlrservercontroller;
class sqlrserverconnection;
class sqlrservercursor;
class sqlrserverbindvar;

class SQLRSERVER_DLLSPEC sqlrclientprotocol : public sqlrprotocol {
	public:
			sqlrclientprotocol(sqlrservercontroller *cont,
						sqlrserverconnection *conn);
		virtual	~sqlrclientprotocol();

		sqlrclientexitstatus_t	clientSession();

	private:
		bool	getCommand(uint16_t *command);
		sqlrservercursor	*getCursor(uint16_t command);
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
		bool	newQueryCommand(sqlrservercursor *cursor);
		bool	reExecuteQueryCommand(sqlrservercursor *cursor);
		bool	fetchFromBindCursorCommand(sqlrservercursor *cursor);
		bool	processQueryOrBindCursor(sqlrservercursor *cursor,
							bool reexecute,
							bool bindcursor);
		bool	getClientInfo(sqlrservercursor *cursor);
		bool	getQuery(sqlrservercursor *cursor);
		bool	getInputBinds(sqlrservercursor *cursor);
		bool	getOutputBinds(sqlrservercursor *cursor);
		bool	getBindVarCount(sqlrservercursor *cursor,
						uint16_t *count);
		bool	getBindVarName(sqlrservercursor *cursor,
						sqlrserverbindvar *bv);
		bool	getBindVarType(sqlrserverbindvar *bv);
		bool	getBindSize(sqlrservercursor *cursor,
						sqlrserverbindvar *bv,
						uint32_t *maxsize);
		void	getNullBind(sqlrserverbindvar *bv);
		bool	getStringBind(sqlrservercursor *cursor,
						sqlrserverbindvar *bv);
		bool	getIntegerBind(sqlrserverbindvar *bv);
		bool	getDoubleBind(sqlrserverbindvar *bv);
		bool	getDateBind(sqlrserverbindvar *bv);
		bool	getLobBind(sqlrservercursor *cursor,
						sqlrserverbindvar *bv);
		bool	getSendColumnInfo();
		bool	getSkipAndFetch(sqlrservercursor *cursor);
		void	returnResultSetHeader(sqlrservercursor *cursor);
		void	returnColumnInfo(sqlrservercursor *cursor,
							uint16_t format);
		void	sendRowCounts(bool knowsactual, uint64_t actual,
					bool knowsaffected, uint64_t affected);
		void	returnOutputBindValues(sqlrservercursor *cursor);
		void	returnOutputBindBlob(sqlrservercursor *cursor,
							uint16_t index);
		void	returnOutputBindClob(sqlrservercursor *cursor,
							uint16_t index);
		void	sendLobOutputBind(sqlrservercursor *cursor,
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
		bool	returnResultSetData(sqlrservercursor *cursor,
						bool getskipandfetch);
		void	returnRow(sqlrservercursor *cursor);
		void	sendField(const char *data, uint32_t size);
		void	sendNullField();
		void	sendLobField(sqlrservercursor *cursor, uint32_t col);
		void	startSendingLong(uint64_t longlength);
		void	sendLongSegment(const char *data, uint32_t size);
		void	endSendingLong();
		void	returnError(bool disconnect);
		void	returnError(sqlrservercursor *cursor, bool disconnect);
		bool	fetchResultSetCommand(sqlrservercursor *cursor);
		void	abortResultSetCommand(sqlrservercursor *cursor);
		void	suspendResultSetCommand(sqlrservercursor *cursor);
		bool	resumeResultSetCommand(sqlrservercursor *cursor);
		bool	getDatabaseListCommand(sqlrservercursor *cursor);
		bool	getTableListCommand(sqlrservercursor *cursor);
		bool	getColumnListCommand(sqlrservercursor *cursor);
		bool	getListCommand(sqlrservercursor *cursor,
						int which, bool gettable);
		bool	getListByApiCall(sqlrservercursor *cursor,
						int which,
						const char *table,
						const char *wild);
		bool	getListByQuery(sqlrservercursor *cursor,
						int which,
						const char *table,
						const char *wild);
		bool	buildListQuery(sqlrservercursor *cursor,
						const char *query,
						const char *wild,
						const char *table);
		void	escapeParameter(stringbuffer *buffer,
						const char *parameter);
		bool	getQueryTreeCommand(sqlrservercursor *cursor);

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
