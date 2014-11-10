// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#ifndef SQLRSERVERCONTROLLER_H
#define SQLRSERVERCONTROLLER_H

#include <sqlrelay/private/sqlrservercontrollerincludes.h>

class SQLRSERVER_DLLSPEC sqlrservercontroller : public listener {
	public:
			sqlrservercontroller();
		virtual	~sqlrservercontroller();

		bool	init(int argc, const char **argv);
		bool	listen();


		// connection api...

		// connect string 
		const char	*getConnectStringValue(const char *variable);

		// environment
		const char	*getId();
		const char	*getLogDir();
		const char	*getDebugDir();

		// re-login to the database
		void	reLogIn();

		// client authentication
		void		setUser(const char *user);
		void		setPassword(const char *password);
		const char	*getUser();
		const char	*getPassword();
		bool		authenticate(const char *userbuffer,
						const char *passwordbuffer);
		bool		changeUser(const char *newuser,
						const char *newpassword);

		// close client connection
		void	closeClientConnection(uint32_t bytes);

		// session management
		void	beginSession();
		void	suspendSession(const char **unixsocket,
						uint16_t *inetport);
		void	endSession();

		// ping
		bool	ping();

		// database info
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		const char	*dbIpAddress();

		// bind variables
		const char	*bindFormat();
		char		bindVariablePrefix();
		int16_t		nonNullBindValue();
		int16_t		nullBindValue();
		bool		bindValueIsNull(int16_t isnull);
		void		fakeInputBinds();
		bool		getFakeInputBinds();
		memorypool	*getBindMappingsPool();

		// db selection
		bool	selectDatabase(const char *db);
		void	dbHasChanged();
		char	*getCurrentDatabase();

		// column names
		bool	getColumnNames(const char *query, stringbuffer *output);

		// last insert id
		bool	getLastInsertId(uint64_t *id);

		// transactions
		bool	begin();
		bool	commit();
		bool	rollback();
		bool	autoCommitOn();
		bool	autoCommitOff();
		void	commitOrRollbackIsNeeded();
		void	commitOrRollbackIsNotNeeded();
		bool	setIsolationLevel(const char *isolevel);
		void	setFakeTransactionBlocksBehavior(bool ftb);
		void	setAutoCommitBehavior(bool ac);

		// errors
		void		errorMessage(char *errorbuffer,
					uint32_t errorbuffersize,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection);
		void		clearError();
		void		setError(const char *err,
					int64_t errn, bool liveconn);
		char		*getErrorBuffer();
		uint32_t	getErrorLength();
		void		setErrorLength(uint32_t errorlength);
		uint32_t	getErrorNumber();
		void		setErrorNumber(uint32_t errnum);
		bool		getLiveConnection();
		void		setLiveConnection(bool liveconnection);

		// connection state
		void	updateState(enum sqlrconnectionstate_t state);
		void	updateCurrentQuery(const char *query,
						uint32_t querylen);
		void	updateClientInfo(const char *info,
						uint32_t infolen);


		// statistics api...
		void	incrementOpenDatabaseConnections();
		void	decrementOpenDatabaseConnections();
		void	incrementOpenClientConnections();
		void	decrementOpenClientConnections();
		void	incrementOpenDatabaseCursors();
		void	decrementOpenDatabaseCursors();
		void	incrementTimesNewCursorUsed();
		void	incrementTimesCursorReused();
		void	incrementQueryCounts(sqlrquerytype_t querytype);
		void	incrementTotalErrors();
		void	incrementAuthenticateCount();
		void	incrementSuspendSessionCount();
		void	incrementEndSessionCount();
		void	incrementPingCount();
		void	incrementIdentifyCount();
		void	incrementAutocommitCount();
		void	incrementBeginCount();
		void	incrementCommitCount();
		void	incrementRollbackCount();
		void	incrementDbVersionCount();
		void	incrementBindFormatCount();
		void	incrementServerVersionCount();
		void	incrementSelectDatabaseCount();
		void	incrementGetCurrentDatabaseCount();
		void	incrementGetLastInsertIdCount();
		void	incrementDbHostNameCount();
		void	incrementDbIpAddressCount();
		void	incrementNewQueryCount();
		void	incrementReexecuteQueryCount();
		void	incrementFetchFromBindCursorCount();
		void	incrementFetchResultSetCount();
		void	incrementAbortResultSetCount();
		void	incrementSuspendResultSetCount();
		void	incrementResumeResultSetCount();
		void	incrementGetDbListCount();
		void	incrementGetTableListCount();
		void	incrementGetColumnListCount();
		void	incrementGetQueryTreeCount();
		void	incrementReLogInCount();


		// logging api...
		bool	logEnabled();
		void	logDebugMessage(const char *info);
		void	logClientConnected();
		void	logClientConnectionRefused(const char *info);
		void	logClientDisconnected(const char *info);
		void	logClientProtocolError(sqlrservercursor *cursor,
							const char *info,
							ssize_t result);
		void	logDbLogIn();
		void	logDbLogOut();
		void	logDbError(sqlrservercursor *cursor, const char *info);
		void	logQuery(sqlrservercursor *cursor);
		void	logInternalError(sqlrservercursor *cursor,
							const char *info);


		// cursor api...

		// cursor management
		sqlrservercursor	*newCursor();
		sqlrservercursor	*getCursor();
		sqlrservercursor	*getCursor(uint16_t id);
		uint16_t	getId(sqlrservercursor *cursor);
		bool		open(sqlrservercursor *cursor);
		bool		close(sqlrservercursor *cursor);
		void		suspendResultSet(sqlrservercursor *cursor);
		void		abort(sqlrservercursor *cursor);
		void		deleteCursor(sqlrservercursor *curs);

		// command stats
		void		setCommandStart(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec);
		uint64_t	getCommandStartSec(sqlrservercursor *cursor);
		uint64_t	getCommandStartUSec(sqlrservercursor *cursor);
		void		setCommandEnd(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec);
		uint64_t	getCommandEndSec(sqlrservercursor *cursor);
		uint64_t	getCommandEndUSec(sqlrservercursor *cursor);

		// query stats
		void		setQueryStart(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec);
		uint64_t	getQueryStartSec(sqlrservercursor *cursor);
		uint64_t	getQueryStartUSec(sqlrservercursor *cursor);
		void		setQueryEnd(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec);
		uint64_t	getQueryEndSec(sqlrservercursor *cursor);
		uint64_t	getQueryEndUSec(sqlrservercursor *cursor);

		// query buffer
		char		*getQueryBuffer(sqlrservercursor *cursor);
		uint32_t 	getQueryLength(sqlrservercursor *cursor);
		void		setQueryLength(sqlrservercursor *cursor,
						uint32_t querylength);

		// query tree
		xmldom		*getQueryTree(sqlrservercursor *cursor);

		// running queries
		bool	prepareQuery(sqlrservercursor *cursor,
						const char *query,
						uint32_t length);
		bool	executeQuery(sqlrservercursor *cursor);
		bool	executeQuery(sqlrservercursor *cursor,
						bool enabletranslations,
						bool enabletriggers);
		bool	fetchFromBindCursor(sqlrservercursor *cursor);

		// input bind variables
		void		setFakeInputBindsForThisQuery(
						sqlrservercursor *cursor,
						bool fake);
		bool		getFakeInputBindsForThisQuery(
						sqlrservercursor *cursor);
		void		setInputBindCount(sqlrservercursor *cursor,
						uint16_t inbindcount);
		uint16_t	getInputBindCount(sqlrservercursor *cursor);
		sqlrserverbindvar	*getInputBinds(
						sqlrservercursor *cursor);

		// output bind variables
		void		setOutputBindCount(sqlrservercursor *cursor,
						uint16_t outbindcount);
		uint16_t	getOutputBindCount(sqlrservercursor *cursor);
		sqlrserverbindvar	*getOutputBinds(
						sqlrservercursor *cursor);
		bool		getLobOutputBindLength(
						sqlrservercursor *cursor,
						uint16_t index,
						uint64_t *length);
		bool		getLobOutputBindSegment(
						sqlrservercursor *cursor,
						uint16_t index,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread);
		void		closeLobOutputBind(sqlrservercursor *cursor,
								uint16_t index);

		// custom queries
		bool		isCustomQuery(sqlrservercursor *cursor);
		sqlrservercursor	*useCustomQueryCursor(
						sqlrservercursor *cursor);

		// temp tables
		void	addSessionTempTableForDrop(const char *tablename);
		void	addSessionTempTableForTrunc(const char *tablename);
		void	addTransactionTempTableForDrop(const char *tablename);
		void	addTransactionTempTableForTrunc(const char *tablename);

		// table name remapping
		const char	*translateTableName(const char *table);
		bool		removeReplacementTable(const char *database,
							const char *schema,
							const char *table);
		bool		removeReplacementIndex(const char *database,
							const char *schema,
							const char *table);

		// db, table, column lists
		bool		getListsByApiCalls();
		bool		getDatabaseList(sqlrservercursor *cursor,
							const char *wild);
		bool		getTableList(sqlrservercursor *cursor,
							const char *wild);
		bool		getColumnList(sqlrservercursor *cursor,
							const char *table,
							const char *wild);
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(const char *table,
							bool wild);

		// column info
		uint16_t	getSendColumnInfo();
		void		setSendColumnInfo(uint16_t sendcolumninfo);
		uint32_t	colCount(sqlrservercursor *cursor);
		uint16_t	columnTypeFormat(sqlrservercursor *cursor);
		const char	*getColumnName(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnNameLength(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnType(sqlrservercursor *cursor,
							uint32_t col);
		const char	*getColumnTypeName(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnTypeNameLength(
						sqlrservercursor *cursor,
							uint32_t col);
		uint32_t	getColumnLength(sqlrservercursor *cursor,
							uint32_t col);
		uint32_t	getColumnPrecision(sqlrservercursor *cursor,
							uint32_t col);
		uint32_t	getColumnScale(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnIsNullable(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnIsPrimaryKey(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnIsUnique(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnIsPartOfKey(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnIsUnsigned(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnIsZeroFilled(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnIsBinary(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnIsAutoIncrement(
						sqlrservercursor *cursor,
							uint32_t col);

		// result set navigation
		bool		knowsRowCount(sqlrservercursor *cursor);
		uint64_t	rowCount(sqlrservercursor *cursor);
		bool		knowsAffectedRows(sqlrservercursor *cursor);
		uint64_t	affectedRows(sqlrservercursor *cursor);
		bool		noRowsToReturn(sqlrservercursor *cursor);
		bool		skipRow(sqlrservercursor *cursor);
		bool		skipRows(sqlrservercursor *cursor,
							uint64_t rows);
		bool		fetchRow(sqlrservercursor *cursor);
		void		nextRow(sqlrservercursor *cursor);
		uint64_t	getTotalRowsFetched(sqlrservercursor *cursor);
		void		closeResultSet(sqlrservercursor *cursor);
		void		closeAllResultSets();

		// fields
		void	getField(sqlrservercursor *cursor,
						uint32_t col,
						const char **field,
						uint64_t *fieldlength,
						bool *blob,
						bool *null);
		bool	getLobFieldLength(sqlrservercursor *cursor,
						uint32_t col,
						uint64_t *length);
		bool	getLobFieldSegment(sqlrservercursor *cursor,
						uint32_t col,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread);
		void	closeLobField(sqlrservercursor *cursor,
						uint32_t col);
		void	reformatField(sqlrservercursor *cursor,
						uint16_t index,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength);
		void	reformatDateTimes(sqlrservercursor *cursor,
						uint16_t index,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength,
						bool ddmm, bool yyyyddmm,
						const char *datetimeformat,
						const char *dateformat,
						const char *timeformat);

		// errors
		void		errorMessage(sqlrservercursor *cursor,
						char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection);
		void		clearError(sqlrservercursor *cursor);
		void		setError(sqlrservercursor *cursor,
						const char *err,
						int64_t errn,
						bool liveconn);
		char		*getErrorBuffer(sqlrservercursor *cursor);
		uint32_t	getErrorLength(sqlrservercursor *cursor);
		void		setErrorLength(sqlrservercursor *cursor,
							uint32_t errorlength);
		uint32_t	getErrorNumber(sqlrservercursor *cursor);
		void		setErrorNumber(sqlrservercursor *cursor,
							uint32_t errnum);
		bool		getLiveConnection(sqlrservercursor *cursor);
		void		setLiveConnection(sqlrservercursor *cursor,
							bool liveconnection);

		// cursor state
		void			setState(sqlrservercursor *cursor,
						sqlrcursorstate_t state);
		sqlrcursorstate_t	getState(sqlrservercursor *cursor);

		// utilities
		bool		skipComment(const char **ptr,
						const char *endptr);
		bool		skipWhitespace(const char **ptr,
						const char *endptr);
		const char	*skipWhitespaceAndComments(const char *query);

		// connection
		sqlrserverconnection	*conn;

		// config file
		sqlrconfigfile		*cfgfl;

		// statistics
		shmdata			*shm;
		sqlrconnstatistics	*connstats;

	#include <sqlrelay/private/sqlrservercontroller.h>
};

#endif
