// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRSERVER_H
#define SQLRSERVER_H

#include <sqlrelay/private/sqlrserverincludes.h>

class SQLRSERVER_DLLSPEC sqlrlistener {
	public:
		sqlrlistener();
		~sqlrlistener();

		bool	init(int argc, const char **argv);
		void	listen();

		const char	*getId();
		const char	*getLogDir();
		const char	*getDebugDir();

		#include <sqlrelay/private/sqlrlistener.h>
};

enum sqlrcursorstate_t {
	SQLRCURSORSTATE_AVAILABLE=0,
	SQLRCURSORSTATE_BUSY,
	SQLRCURSORSTATE_SUSPENDED
};

enum sqlrquerytype_t {
	SQLRQUERYTYPE_SELECT=0,
	SQLRQUERYTYPE_INSERT,
	SQLRQUERYTYPE_UPDATE,
	SQLRQUERYTYPE_DELETE,
	SQLRQUERYTYPE_CREATE,
	SQLRQUERYTYPE_DROP,
	SQLRQUERYTYPE_ALTER,
	SQLRQUERYTYPE_CUSTOM,
	SQLRQUERYTYPE_ETC,
	SQLRQUERYTYPE_BEGIN,
	SQLRQUERYTYPE_COMMIT,
	SQLRQUERYTYPE_ROLLBACK,
	SQLRQUERYTYPE_AUTOCOMMIT_ON,
	SQLRQUERYTYPE_AUTOCOMMIT_OFF,
	SQLRQUERYTYPE_SET_INCLUDING_AUTOCOMMIT_ON,
	SQLRQUERYTYPE_SET_INCLUDING_AUTOCOMMIT_OFF
};

enum sqlrquerystatus_t {
	SQLRQUERYSTATUS_SUCCESS=0,
	SQLRQUERYSTATUS_ERROR,
	SQLRQUERYSTATUS_FILTER_VIOLATION
};

enum sqlrserverbindvartype_t {
	SQLRSERVERBINDVARTYPE_NULL=0,
	SQLRSERVERBINDVARTYPE_STRING,
	SQLRSERVERBINDVARTYPE_INTEGER,
	SQLRSERVERBINDVARTYPE_DOUBLE,
	SQLRSERVERBINDVARTYPE_BLOB,
	SQLRSERVERBINDVARTYPE_CLOB,
	SQLRSERVERBINDVARTYPE_CURSOR,
	SQLRSERVERBINDVARTYPE_DATE,

	// special types for bulk load
	SQLRSERVERBINDVARTYPE_DELIMITER,
	SQLRSERVERBINDVARTYPE_NEWLINE
};

enum sqlrserverlistformat_t {
	SQLRSERVERLISTFORMAT_NULL=0,
	SQLRSERVERLISTFORMAT_MYSQL,
	SQLRSERVERLISTFORMAT_ODBC
};

class SQLRSERVER_DLLSPEC sqlrserverbindvar {
	public:
		char	*variable;
		int16_t	variablesize;
		union {
			char	*stringval;
			int64_t	integerval;
			struct	{
				double		value;
				uint32_t	precision;
				uint32_t	scale;
			} doubleval;
			struct {
				int16_t		year;
				int16_t		month;
				int16_t		day;
				int16_t		hour;
				int16_t		minute;
				int16_t		second;
				int32_t		microsecond;
				char		*tz;
				bool		isnegative;
				char		*buffer;
				uint16_t	buffersize;
			} dateval;
			uint16_t	cursorid;
		} value;
		uint32_t		valuesize;
		uint32_t		resultvaluesize;
		sqlrserverbindvartype_t	type;
		unsigned char		nativetype;
		int16_t			isnull;
};

class SQLRSERVER_DLLSPEC sqlrservercontroller {
	public:
		sqlrservercontroller();
		~sqlrservercontroller();

		bool	init(int argc, const char **argv);
		bool	listen();


		// connection api...

		// connect string 
		const char	*getConnectStringValue(const char *variable);
		void		setConnectTimeout(uint64_t connecttimeout);
		uint64_t	getConnectTimeout();
		void		setQueryTimeout(uint64_t querytimeout);
		uint64_t	getQueryTimeout();
		void		setExecuteDirect(bool executedirect);
		bool		getExecuteDirect();

		// environment
		const char	*getId();
		const char	*getConnectionId();
		const char	*getLogDir();
		const char	*getDebugDir();

		// passthrough
		bool	send(unsigned char *data, size_t size);
		bool	recv(unsigned char **data, size_t *size);

		// re-login to the database
		void	reLogIn();

		// backend auth
		void		setUser(const char *user);
		void		setPassword(const char *password);
		const char	*getUser();
		const char	*getPassword();

		// client auth
		sqlrcredentials	*getCredentials(const char *user,
						const char *password,
						bool usegss,
						bool usetls);
		bool		auth(sqlrcredentials *cred);
		bool		changeUser(const char *newuser,
						const char *newpassword);
		bool		changeProxiedUser(const char *newuser,
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
		int16_t		nonNullBindValue();
		int16_t		nullBindValue();
		bool		bindValueIsNull(int16_t isnull);
		void		setFakeInputBinds(bool fake);
		bool		getFakeInputBinds();

		// fetch info
		void		setFetchAtOnce(uint32_t fao);
		void		setMaxColumnCount(uint32_t mcc);
		void		setMaxFieldLength(uint32_t mfl);
		uint32_t	getFetchAtOnce();
		uint32_t	getMaxColumnCount();
		uint32_t	getMaxFieldLength();

		// db selection
		bool	selectDatabase(const char *db);
		void	dbHasChanged();
		char	*getCurrentDatabase();
		char	*getCurrentSchema();

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
		void	setNeedsCommitOrRollback(bool needed);
		bool	getNeedsCommitOrRollback();
		bool	setIsolationLevel(const char *isolevel);
		void	setFakeTransactionBlocks(bool ftb);
		bool	getFakeTransactionBlocks();
		void	setFakeAutoCommit(bool fac);
		bool	getFakeAutoCommit();
		void	setInitialAutoCommit(bool iac);
		bool	getInitialAutoCommit();
		bool	inTransaction();

		// errors
		void		saveError();
		void		saveErrorFromCursor(sqlrservercursor *cursor);
		void		errorMessage(const char **errorbuffer,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection);
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
		void	setState(enum sqlrconnectionstate_t state);
		enum sqlrconnectionstate_t	getState();

		// current client info
		void	setCurrentUser(const char *user, uint32_t userlen);
		void	setCurrentQuery(const char *query, uint32_t querylen);
		void	setClientInfo(const char *info, uint32_t infolen);
		const char	*getCurrentUser();
		const char	*getCurrentQuery();
		const char	*getClientInfo();
		const char	*getClientAddr();

		// instance state
		void	setInstanceDisabled(bool disabled);
		bool	getInstanceDisabled();


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
		void	incrementAuthCount();
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
		void	incrementNextResultSetCount();
		void	incrementNextResultSetAvailableCount();
		uint32_t	getStatisticsIndex();


		// event api...
		bool	logEnabled();
		bool	notificationsEnabled();
		void	raiseDebugMessageEvent(const char *info);
		void	raiseClientConnectedEvent();
		void	raiseClientConnectionRefusedEvent(const char *info);
		void	raiseClientDisconnectedEvent(const char *info);
		void	raiseClientProtocolErrorEvent(sqlrservercursor *cursor,
							const char *info,
							ssize_t result);
		void	raiseDbLogInEvent();
		void	raiseDbLogOutEvent();
		void	raiseDbErrorEvent(sqlrservercursor *cursor,
							const char *info);
		void	raiseDbWarningEvent(sqlrservercursor *cursor,
							const char *info);
		void	raiseQueryEvent(sqlrservercursor *cursor);
		void	raiseFilterViolationEvent(sqlrservercursor *cursor);
		void	raiseInternalErrorEvent(sqlrservercursor *cursor,
							const char *info);
		void	raiseInternalWarningEvent(sqlrservercursor *cursor,
							const char *info);
		void	raiseScheduleViolationEvent(const char *info);
		void	raiseIntegrityViolationEvent(const char *info);
		void	raiseTranslationFailureEvent(sqlrservercursor *cursor,
							const char *info);
		void	raiseParseFailureEvent(sqlrservercursor *cursor,
							const char *info);
		void	raiseCursorOpenEvent(sqlrservercursor *cursor);
		void	raiseCursorCloseEvent(sqlrservercursor *cursor);
		void	raiseBeginTransactionEvent();
		void	raiseCommitEvent();
		void	raiseRollbackEvent();


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

		// query status
		sqlrquerystatus_t	getQueryStatus(
						sqlrservercursor *cursor);

		// query translations
		xmldom		*getQueryTree(sqlrservercursor *cursor);
		const char	*getTranslatedQuery(sqlrservercursor *cursor);

		// running queries
		bool	prepareQuery(sqlrservercursor *cursor,
						const char *query,
						uint32_t length);
		bool	prepareQuery(sqlrservercursor *cursor,
						const char *query,
						uint32_t length,
						bool enabledirectives,
						bool enabletranslations,
						bool enablefilters);
		bool	executeQuery(sqlrservercursor *cursor);
		bool	executeQuery(sqlrservercursor *cursor,
						bool enabledirectives,
						bool enabletranslations,
						bool enablefilters,
						bool enabletriggers);
		bool	fetchFromBindCursor(sqlrservercursor *cursor);
		bool	nextResultSet(sqlrservercursor *cursor,
						bool *nextresultsetavailable);

		// bind variables
		memorypool	*getBindPool(sqlrservercursor *cursor);
		memorypool	*getBindMappingsPool(sqlrservercursor *cursor);
		namevaluepairs	*getBindMappings(sqlrservercursor *cursor);

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

		// input/output bind variables
		void		setInputOutputBindCount(
						sqlrservercursor *cursor,
						uint16_t inoutbindcount);
		uint16_t	getInputOutputBindCount(
						sqlrservercursor *cursor);
		sqlrserverbindvar	*getInputOutputBinds(
						sqlrservercursor *cursor);
		bool		getLobInputOutputBindLength(
						sqlrservercursor *cursor,
						uint16_t index,
						uint64_t *length);
		bool		getLobInputOutputBindSegment(
						sqlrservercursor *cursor,
						uint16_t index,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread);
		void		closeLobInputOutputBind(
						sqlrservercursor *cursor,
						uint16_t index);

		// custom queries
		bool		isCustomQuery(sqlrservercursor *cursor);
		sqlrservercursor	*useCustomQueryCursor(
						sqlrservercursor *cursor);

		// temp tables
		void	addGlobalTempTables(const char *gtts);
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

		// db, table, column, procedure bind/column lists
		bool		getListsByApiCalls();
		bool		fakePrepareAndExecuteForApiCall(
						sqlrservercursor *cursor);
		bool		getDatabaseList(sqlrservercursor *cursor,
						const char *wild);
		bool		getSchemaList(sqlrservercursor *cursor,
						const char *wild);
		bool		getTableList(sqlrservercursor *cursor,
						const char *wild,
						uint16_t objecttypes);
		bool		getTableTypeList(sqlrservercursor *cursor,
						const char *wild);
		bool		getColumnList(sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		bool		getPrimaryKeyList(sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		bool		getKeyAndIndexList(sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		bool		getProcedureBindAndColumnList(
						sqlrservercursor *cursor,
						const char *proc,
						const char *wild);
		bool		getTypeInfoList(sqlrservercursor *cursor,
						const char *type,
						const char *wild);
		bool		getProcedureList(sqlrservercursor *cursor,
						const char *wild);
		const char	*getDatabaseListQuery(bool wild);
		const char	*getSchemaListQuery(bool wild);
		const char	*getTableListQuery(bool wild,
						uint16_t objecttypes);
		const char	*getTableTypeListQuery(bool wild);
		const char	*getGlobalTempTableListQuery();
		const char	*getColumnListQuery(const char *table,
							bool wild);
		const char	*getPrimaryKeyListQuery(const char *table,
							bool wild);
		const char	*getKeyAndIndexListQuery(const char *table,
							bool wild);
		const char	*getProcedureBindAndColumnListQuery(
							const char *proc,
							bool wild);
		const char	*getTypeInfoListQuery(const char *type,
							bool wild);
		const char	*getProcedureListQuery(bool wild);

		// column info
		bool		columnInfoIsValidAfterPrepare(
						sqlrservercursor *cursor);
		uint16_t	getSendColumnInfo();
		void		setSendColumnInfo(uint16_t sendcolumninfo);
		uint32_t	colCount(sqlrservercursor *cursor);
		uint16_t	columnTypeFormat(sqlrservercursor *cursor);
		void		setDatabaseListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setSchemaListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setTableListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setTableTypeListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setColumnListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setPrimaryKeyListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setKeyAndIndexListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setProcedureBindAndColumnListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setTypeInfoListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setProcedureListColumnMap(
					sqlrserverlistformat_t listformat);
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
		const char	*getColumnTable(sqlrservercursor *cursor,
							uint32_t col);
		uint16_t	getColumnTableLength(sqlrservercursor *cursor,
							uint32_t col);
		void		getColumnNameList(sqlrservercursor *cursor,
							stringbuffer *output);
		bool		handleResultSetHeader(sqlrservercursor *cursor);

		// result set navigation
		bool		knowsRowCount(sqlrservercursor *cursor);
		uint64_t	rowCount(sqlrservercursor *cursor);
		bool		knowsAffectedRows(sqlrservercursor *cursor);
		uint64_t	affectedRows(sqlrservercursor *cursor);
		bool		noRowsToReturn(sqlrservercursor *cursor);
		bool		skipRow(sqlrservercursor *cursor,
							bool *error);
		bool		skipRows(sqlrservercursor *cursor,
							uint64_t rows,
							bool *error);
		bool		fetchRow(sqlrservercursor *cursor, bool *error);
		void		nextRow(sqlrservercursor *cursor);
		uint64_t	getTotalRowsFetched(sqlrservercursor *cursor);
		void		closeResultSet(sqlrservercursor *cursor);
		void		closeAllResultSets();

		// fields
		bool	getField(sqlrservercursor *cursor,
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
		bool	reformatField(sqlrservercursor *cursor,
						const char *name,
						uint32_t index,
						const char **field,
						uint64_t *fieldlength);
		bool	reformatRow(sqlrservercursor *cursor,
						uint32_t colcount,
						const char * const *names,
						const char ***fields,
						uint64_t **fieldlengths);
		bool	reformatDateTimes(sqlrservercursor *cursor,
						uint32_t index,
						const char *field,
						uint64_t fieldlength,
						const char **newfield,
						uint64_t *newfieldlength,
						bool ddmm, bool yyyyddmm,
						bool ignorenondatetime,
						const char *datedelimiters,
						const char *datetimeformat,
						const char *dateformat,
						const char *timeformat);

		// errors
		void		saveError(sqlrservercursor *cursor);
		void		errorMessage(sqlrservercursor *cursor,
						const char **errorbuffer,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection);
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

		// bulk load
		bool	bulkLoadBegin(const char *id,
					const char *errorfieldtable,
					const char *errorrowtable,
					uint64_t maxerrorcount,
					bool droperrortables);
		bool	bulkLoadCheckpoint(const char *id);
		bool	bulkLoadPrepareQuery(const char *query,
						uint64_t querylen,
						uint16_t inbindcount,
						sqlrserverbindvar *inbinds);
		bool	bulkLoadCreateErrorTables(const char *query,
						uint64_t querylen,
						const char *errorfieldtable,
						const char *errorrowtable);
		bool	bulkLoadCreateErrorTable1(sqlrservercursor *cursor,
						const char *query,
						uint64_t querylen,
						const char *errorfieldtable);
		bool	bulkLoadCreateErrorTable2(sqlrservercursor *cursor,
						const char *query,
						uint64_t querylen,
						const char *errorrowtable);
		bool	bulkLoadJoin(const char *id);
		bool	bulkLoadInputBind(const unsigned char *data,
						uint64_t datalen);
		void	bulkLoadParseInsert(const char *query,
						uint64_t querylen,
						char **table,
                                                linkedlist<char *> *cols,
                                                linkedlist<char *> *binds);
		bool	bulkLoadExecuteQuery();
		void	bulkLoadInitBinds();
		void	bulkLoadBindRow(const unsigned char *data,
						uint64_t datalen);
		void	bulkLoadError();
		bool	bulkLoadStoreError(int64_t errorcode,
						const char *error,
						uint32_t errorlength,
						const char *errorfieldtable,
						const char *errorrowtable);
		bool	bulkLoadEnd();
		bool	bulkLoadDropErrorTables(const char *errorfieldtable,
						const char *errorrowtable);

		// cursor state
		void			setState(sqlrservercursor *cursor,
						sqlrcursorstate_t state);
		sqlrcursorstate_t	getState(sqlrservercursor *cursor);

		// memory pools
		memorypool	*getPerTransactionMemoryPool();
		memorypool	*getPerSessionMemoryPool();

		// query parser
		sqlrparser	*getParser();

		// gss
		gsscontext	*getGSSContext();

		// tls
		tlscontext	*getTLSContext();

		// configuration
		sqlrconfig	*getConfig();
		sqlrpaths	*getPaths();

		// shared memory
		sqlrshm		*getShm();

		// module data
		sqlrmoduledata	*getModuleData(const char *id);

		// utilities
		bool		skipComment(const char **ptr,
						const char *endptr);
		bool		skipWhitespace(const char **ptr,
						const char *endptr);
		const char	*skipWhitespaceAndComments(const char *query);

		bool	parseDateTime(const char *datetime,
				bool ddmm, bool yyyyddmm,
				const char *datedelimiters,
				int16_t *year, int16_t *month, int16_t *day,
				int16_t *hour, int16_t *minute, int16_t *second,
				int32_t *fraction, bool *isnegative);

		char	*convertDateTime(const char *format,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t fraction, bool isnegative);

		const char	*asciiToHex(unsigned char ch);
		const char	*asciiToOctal(unsigned char ch);

		bool		hasBindVariables(const char *query,
							uint32_t querylen);
		uint16_t	countBindVariables(const char *query,
							uint32_t querylen);

		bool	isBitType(const char *type);
		bool	isBitType(int16_t type);
		bool	isBoolType(const char *type);
		bool	isBoolType(int16_t type);
		bool	isFloatType(const char *type);
		bool	isFloatType(int16_t type);
		bool	isNumberType(const char *type);
		bool	isNumberType(int16_t type);
		bool	isBlobType(const char *type);
		bool	isBlobType(int16_t type);
		bool	isUnsignedType(const char *type);
		bool	isUnsignedType(int16_t type);
		bool	isBinaryType(const char *type);
		bool	isBinaryType(int16_t type);
		bool	isDateTimeType(const char *type);
		bool	isDateTimeType(int16_t type);

		const char * const	*dataTypeStrings();

	#include <sqlrelay/private/sqlrservercontroller.h>
};

class SQLRSERVER_DLLSPEC sqlrserverconnection {
	public:
		sqlrserverconnection(sqlrservercontroller *cont);
		virtual	~sqlrserverconnection();

		virtual bool	mustDetachBeforeLogIn();

		virtual bool	supportsAuthOnDatabase();
		virtual	void	handleConnectString();

		virtual	bool	send(unsigned char *data, size_t size);
		virtual	bool	recv(unsigned char **data, size_t *size);

		virtual	bool	logIn(const char **error,
					const char **warning)=0;
		virtual	void	logOut()=0;

		virtual	bool	changeUser(const char *newuser,
						const char *newpassword);
		virtual	bool	changeProxiedUser(const char *newuser,
						const char *newpassword);

		virtual bool	autoCommitOn();
		virtual bool	autoCommitOff();

		virtual bool	isTransactional();
		virtual bool	supportsTransactionBlocks();
		virtual bool	supportsAutoCommit();

		virtual bool		begin();
		virtual const char	*beginTransactionQuery();

		virtual bool	commit();
		virtual bool	rollback();

		virtual	void	errorMessage(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection)=0;

		virtual bool		selectDatabase(const char *database);
		virtual const char	*selectDatabaseQuery();

		virtual char		*getCurrentDatabase();
		virtual const char	*getCurrentDatabaseQuery();

		virtual char		*getCurrentSchema();
		virtual const char	*getCurrentSchemaQuery();

		virtual bool		getLastInsertId(uint64_t *id);
		virtual const char	*getLastInsertIdQuery();
		virtual const char	*noopQuery();

		virtual bool		setIsolationLevel(const char *isolevel);
		virtual const char	*setIsolationLevelQuery();

		virtual bool		ping();
		virtual const char	*pingQuery();

		virtual const char	*identify()=0;

		virtual	const char	*dbVersion()=0;

		virtual const char	*dbHostNameQuery();
		virtual const char	*dbIpAddressQuery();
		virtual const char	*dbHostName();
		virtual const char	*dbIpAddress();
		virtual bool		cacheDbHostInfo();

		virtual bool		getListsByApiCalls();
		virtual bool		getDatabaseList(
						sqlrservercursor *cursor,
						const char *wild);
		virtual bool		getSchemaList(
						sqlrservercursor *cursor,
						const char *wild);
		virtual bool		getTableList(
						sqlrservercursor *cursor,
						const char *wild,
						uint16_t objecttypes);
		virtual bool		getTableTypeList(
						sqlrservercursor *cursor,
						const char *wild);
		virtual bool		getColumnList(
						sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		virtual bool		getPrimaryKeyList(
						sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		virtual bool		getKeyAndIndexList(
						sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		virtual bool		getProcedureBindAndColumnList(
						sqlrservercursor *cursor,
						const char *procedure,
						const char *wild);
		virtual bool		getTypeInfoList(
						sqlrservercursor *cursor,
						const char *type,
						const char *wild);
		virtual bool		getProcedureList(
						sqlrservercursor *cursor,
						const char *wild);
		virtual const char	*getDatabaseListQuery(bool wild);
		virtual const char	*getSchemaListQuery(bool wild);
		virtual const char	*getTableListQuery(bool wild,
						uint16_t objecttypes);
		virtual const char	*getTableListQuery(bool wild,
						uint16_t objecttypes,
						const char *extrawhere);
		virtual const char	*getTableTypeListQuery(bool wild);
		virtual const char	*getGlobalTempTableListQuery();
		virtual const char	*getColumnListQuery(
						const char *table,
						bool wild);
		virtual const char	*getPrimaryKeyListQuery(
						const char *table,
						bool wild);
		virtual const char	*getKeyAndIndexListQuery(
						const char *table,
						bool wild);
		virtual const char	*getProcedureBindAndColumnListQuery(
						const char *procedure,
						bool wild);
		virtual const char	*getTypeInfoListQuery(
						const char *type,
						bool wild);
		virtual const char	*getProcedureListQuery(
						bool wild);
		virtual bool		isSynonym(const char *table);
		virtual const char	*isSynonymQuery();

		virtual sqlrservercursor	*newCursor(uint16_t id)=0;
		virtual void			deleteCursor(
						sqlrservercursor *curs)=0;

		virtual	const char	*bindFormat();
		virtual	int16_t		nonNullBindValue();
		virtual	int16_t		nullBindValue();
		virtual bool		bindValueIsNull(int16_t isnull);

		virtual const char	*tempTableDropPrefix();
		virtual bool		tempTableTruncateBeforeDrop();

		virtual void		endSession();

		char		*getErrorBuffer();
		uint32_t	getErrorLength();
		void		setErrorLength(uint32_t errorlength);
		uint32_t	getErrorNumber();
		void		setErrorNumber(uint32_t errnum);
		bool		getLiveConnection();
		void		setLiveConnection(bool liveconnection);

		sqlrservercontroller	*cont;

	#include <sqlrelay/private/sqlrserverconnection.h>
};

class SQLRSERVER_DLLSPEC sqlrservercursor {
	public:
		sqlrservercursor(sqlrserverconnection *conn, uint16_t id);
		virtual	~sqlrservercursor();

		virtual	bool	open();
		virtual	bool	close();

		virtual sqlrquerytype_t	queryType(const char *query,
							uint32_t length);
		virtual	bool	isCustomQuery();
		virtual	bool	prepareQuery(const char *query,
							uint32_t length);
		virtual	bool	supportsNativeBinds(const char *query,
							uint32_t length);
		virtual	bool	inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		virtual	bool	inputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		virtual void	dateToString(char *buffer,
						uint16_t buffersize,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative);
		virtual bool	inputBind(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		virtual	bool	inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	outputBind(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	outputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		virtual	bool	outputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		virtual bool	outputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						bool *isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		virtual	bool	outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		virtual	bool	outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		virtual	bool	outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrservercursor *cursor);
		virtual bool	getLobOutputBindLength(uint16_t index,
							uint64_t *length);
		virtual bool	getLobOutputBindSegment(uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);
		virtual void	closeLobOutputBind(uint16_t index);
		virtual	bool	inputOutputBind(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	inputOutputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		virtual	bool	inputOutputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		virtual bool	inputOutputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						bool *isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		virtual	bool	inputOutputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		virtual	bool	inputOutputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		virtual bool	getLobInputOutputBindLength(uint16_t index,
							uint64_t *length);
		virtual bool	getLobInputOutputBindSegment(uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);
		virtual void	closeLobInputOutputBind(uint16_t index);
		virtual void	checkForTempTable(const char *query,
							uint32_t length);
		virtual	const char	*truncateTableQuery();
		virtual	bool		executeQuery(const char *query,
							uint32_t length);
		virtual bool	fetchFromBindCursor();
		virtual	bool	nextResultSet(bool *nextresultsetavailable);
		virtual	bool	queryIsNotSelect();
		virtual	bool	queryIsCommitOrRollback();
		virtual	void	errorMessage(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection);
		virtual bool		knowsRowCount();
		virtual uint64_t	rowCount();
		virtual bool		knowsAffectedRows();
		virtual uint64_t	affectedRows();
		virtual	uint32_t	colCount();
		virtual uint16_t	columnTypeFormat();
		virtual const char	*getColumnName(uint32_t col);
		virtual uint16_t	getColumnNameLength(uint32_t col);
		virtual uint16_t	getColumnType(uint32_t col);
		virtual const char	*getColumnTypeName(uint32_t col);
		virtual uint16_t	getColumnTypeNameLength(uint32_t col);
		virtual uint32_t	getColumnLength(uint32_t col);
		virtual uint32_t	getColumnPrecision(uint32_t col);
		virtual uint32_t	getColumnScale(uint32_t col);
		virtual uint16_t	getColumnIsNullable(uint32_t col);
		virtual uint16_t	getColumnIsPrimaryKey(uint32_t col);
		virtual uint16_t	getColumnIsUnique(uint32_t col);
		virtual uint16_t	getColumnIsPartOfKey(uint32_t col);
		virtual uint16_t	getColumnIsUnsigned(uint32_t col);
		virtual uint16_t	getColumnIsZeroFilled(uint32_t col);
		virtual uint16_t	getColumnIsBinary(uint32_t col);
		virtual uint16_t	getColumnIsAutoIncrement(uint32_t col);
		virtual const char	*getColumnTable(uint32_t col);
		virtual uint16_t	getColumnTableLength(uint32_t col);
		virtual bool		ignoreDateDdMmParameter(uint32_t col,
							const char *data,
							uint32_t size);
		virtual	bool	noRowsToReturn();
		virtual	bool	skipRow(bool *error);
		virtual	bool	fetchRow(bool *error);
		virtual	void	nextRow();
		virtual void	getField(uint32_t col,
						const char **field,
						uint64_t *fieldlength,
						bool *blob,
						bool *null);
		virtual bool	getLobFieldLength(uint32_t col,
						uint64_t *length);
		virtual bool	getLobFieldSegment(uint32_t col,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread);
		virtual void	closeLobField(uint32_t col);
		virtual	void	closeResultSet();

		virtual void	encodeBlob(stringbuffer *buffer,
					const char *data, uint32_t datasize);

		virtual bool	columnInfoIsValidAfterPrepare();


		uint16_t	getId();

		bool		fakeInputBinds();

		memorypool	*getBindPool();
		memorypool	*getBindMappingsPool();
		namevaluepairs	*getBindMappings();

		void		setInputBindCount(uint16_t inbindcount);
		uint16_t	getInputBindCount();
		sqlrserverbindvar	*getInputBinds();

		void		setOutputBindCount(uint16_t outbindcount);
		uint16_t	getOutputBindCount();
		sqlrserverbindvar	*getOutputBinds();

		void		setInputOutputBindCount(
					uint16_t inoutbindcount);
		uint16_t	getInputOutputBindCount();
		sqlrserverbindvar	*getInputOutputBinds();

		void	performSubstitution(stringbuffer *buffer,
							int16_t index);
		void	abort();

		char		*getQueryBuffer();
		uint32_t 	getQueryLength();
		void		setQueryLength(uint32_t querylength);

		void		setQueryStatus(sqlrquerystatus_t status);
		sqlrquerystatus_t	getQueryStatus();

		void		setQueryTree(xmldom *tree);
		xmldom		*getQueryTree();
		void		clearQueryTree();

		stringbuffer	*getTranslatedQueryBuffer();

		void		setCommandStart(uint64_t sec, uint64_t usec);
		uint64_t	getCommandStartSec();
		uint64_t	getCommandStartUSec();

		void		setCommandEnd(uint64_t sec, uint64_t usec);
		uint64_t	getCommandEndSec();
		uint64_t	getCommandEndUSec();

		void		setQueryStart(uint64_t sec, uint64_t usec);
		uint64_t	getQueryStartSec();
		uint64_t	getQueryStartUSec();

		void		setQueryEnd(uint64_t sec, uint64_t usec);
		uint64_t	getQueryEndSec();
		uint64_t	getQueryEndUSec();

		void		setFetchStart(uint64_t sec, uint64_t usec);
		uint64_t	getFetchStartSec();
		uint64_t	getFetchStartUSec();

		void		setFetchEnd(uint64_t sec, uint64_t usec);
		uint64_t	getFetchEndSec();
		uint64_t	getFetchEndUSec();

		void		resetFetchTime();
		void		tallyFetchTime();
		uint64_t	getFetchUSec();

		void			setState(sqlrcursorstate_t state);
		sqlrcursorstate_t	getState();

		void		setCustomQueryCursor(sqlrquerycursor *cur);
		sqlrquerycursor	*getCustomQueryCursor();
		void		clearCustomQueryCursor();

		void		clearTotalRowsFetched();
		uint64_t	getTotalRowsFetched();
		void		incrementTotalRowsFetched();

		void		setCurrentRowReformatted(bool crr);
		bool		getCurrentRowReformatted();

		char		*getErrorBuffer();
		uint32_t	getErrorLength();
		void		setErrorLength(uint32_t errorlength);
		uint32_t	getErrorNumber();
		void		setErrorNumber(uint32_t errnum);
		bool		getLiveConnection();
		void		setLiveConnection(bool liveconnection);

		void		setCreateTempTablePattern(
						const char *createtemp);
		const char	*skipCreateTempTableClause(
						const char *query);

		void	setColumnInfoIsValid(bool valid);
		bool	getColumnInfoIsValid();

		void	setQueryHasBeenPreProcessed(bool preprocessed);
		bool	getQueryHasBeenPreProcessed();

		void	setQueryHasBeenPrepared(bool prepared);
		bool	getQueryHasBeenPrepared();

		void	setQueryHasBeenExecuted(bool executed);
		bool	getQueryHasBeenExecuted();

		void	setQueryNeedsIntercept(bool intercept);
		bool	getQueryNeedsIntercept();

		void	setQueryWasIntercepted(bool intercepted);
		bool	getQueryWasIntercepted();

		void	setBindsWereFaked(bool faked);
		bool	getBindsWereFaked();

		void	setFakeInputBindsForThisQuery(bool fake);
		bool	getFakeInputBindsForThisQuery();

		void		setQueryType(sqlrquerytype_t querytype);
		sqlrquerytype_t	getQueryType();

		stringbuffer	*getQueryWithFakeInputBindsBuffer();

		void	allocateColumnPointers(uint32_t colcount);
		void	deallocateColumnPointers();
		void	getColumnPointers(const char ***columnnames,
					uint16_t **columnnamelengths,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamelengths,
					uint32_t **columnlengths,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablelengths);
		const char	*getColumnNameFromBuffer(uint32_t col);
		uint16_t	getColumnNameLengthFromBuffer(uint32_t col);
		uint16_t	getColumnTypeFromBuffer(uint32_t col);
		const char	*getColumnTypeNameFromBuffer(uint32_t col);
		uint16_t	getColumnTypeNameLengthFromBuffer(uint32_t col);
		uint32_t	getColumnLengthFromBuffer(uint32_t col);
		uint32_t	getColumnPrecisionFromBuffer(uint32_t col);
		uint32_t	getColumnScaleFromBuffer(uint32_t col);
		uint16_t	getColumnIsNullableFromBuffer(uint32_t col);
		uint16_t	getColumnIsPrimaryKeyFromBuffer(uint32_t col);
		uint16_t	getColumnIsUniqueFromBuffer(uint32_t col);
		uint16_t	getColumnIsPartOfKeyFromBuffer(uint32_t col);
		uint16_t	getColumnIsUnsignedFromBuffer(uint32_t col);
		uint16_t	getColumnIsZeroFilledFromBuffer(uint32_t col);
		uint16_t	getColumnIsBinaryFromBuffer(uint32_t col);
		uint16_t	getColumnIsAutoIncrementFromBuffer(
							uint32_t col);
		const char	*getColumnTableFromBuffer(uint32_t col);
		uint16_t	getColumnTableLengthFromBuffer(uint32_t col);

		void	allocateFieldPointers(uint32_t colcount);
		void	deallocateFieldPointers();
		void	getFieldPointers(const char ***fieldnames,
					const char ***fields,
					uint64_t **fieldlengths,
					bool **blob,
					bool **null);

		void		setQueryTimeout(uint64_t querytimeout);
		uint64_t	getQueryTimeout();
		void		setExecuteDirect(bool executedirect);
		bool		getExecuteDirect();
		void		setExecuteRpc(bool executerpc);
		bool		getExecuteRpc();

		void		setResultSetHeaderHasBeenHandled(
					bool resultsetheaderhasbeenhandled);
		bool		getResultSetHeaderHasBeenHandled();

		unsigned char	*getModuleData();

		sqlrserverconnection	*conn;

	#include <sqlrelay/private/sqlrservercursor.h>
};

enum clientsessionexitstatus_t {
	CLIENTSESSIONEXITSTATUS_ERROR=0,
	CLIENTSESSIONEXITSTATUS_CLOSED_CONNECTION,
	CLIENTSESSIONEXITSTATUS_ENDED_SESSION,
	CLIENTSESSIONEXITSTATUS_SUSPENDED_SESSION
};

class SQLRSERVER_DLLSPEC sqlrprotocol {
	public:
		sqlrprotocol(sqlrservercontroller *cont,
					sqlrprotocols *ps,
					domnode *parameters);
		virtual	~sqlrprotocol();

		virtual clientsessionexitstatus_t
				clientSession(filedescriptor *clientsock)=0;

		virtual gsscontext	*getGSSContext();
		virtual tlscontext	*getTLSContext();

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrprotocols		*getProtocols();
		domnode			*getParameters();

		void	setProtocolIsBigEndian(bool bigendian);
		bool	getProtocolIsBigEndian();

		void	read(const unsigned char *rp,
					char *value,
					const unsigned char **rpout);
		bool	read(const unsigned char *rp,
					char *value,
					const char *name,
					char expected,
					const unsigned char **rpout);
		void	read(const unsigned char *rp,
					unsigned char *value,
					const unsigned char **rpout);
		bool	read(const unsigned char *rp,
					unsigned char *value,
					const char *name,
					unsigned char expected,
					const unsigned char **rpout);
		void	read(const unsigned char *rp,
					char *value,
					size_t length,
					const unsigned char **rpout);
		void	read(const unsigned char *rp,
					unsigned char *value,
					size_t length,
					const unsigned char **rpout);
		void	read(const unsigned char *rp,
					char16_t *value,
					size_t length,
					const unsigned char **rpout);
		void	read(const unsigned char *rp,
					float *value,
					const unsigned char **rpout);
		void	read(const unsigned char *rp,
					double *value,
					const unsigned char **rpout);
		void	read(const unsigned char *rp,
					uint16_t *value,
					const unsigned char **rpout);
		void	readLE(const unsigned char *rp,
					uint16_t *value,
					const unsigned char **rpout);
		bool	readLE(const unsigned char *rp,
					uint16_t *value,
					const char *name,
					uint16_t expected,
					const unsigned char **rpout);
		void	readBE(const unsigned char *rp,
					uint16_t *value,
					const unsigned char **rpout);
		bool	readBE(const unsigned char *rp,
					uint16_t *value,
					const char *name,
					uint16_t expected,
					const unsigned char **rpout);
		void	read(const unsigned char *rp,
					uint32_t *value,
					const unsigned char **rpout);
		void	readLE(const unsigned char *rp,
					uint32_t *value,
					const unsigned char **rpout);
		bool	readLE(const unsigned char *rp,
					uint32_t *value,
					const char *name,
					uint32_t expected,
					const unsigned char **rpout);
		void	readBE(const unsigned char *rp,
					uint32_t *value,
					const unsigned char **rpout);
		bool	readBE(const unsigned char *rp,
					uint32_t *value,
					const char *name,
					uint32_t expected,
					const unsigned char **rpout);
		void	read(const unsigned char *rp,
					uint64_t *value,
					const unsigned char **rpout);
		void	readLE(const unsigned char *rp,
					uint64_t *value,
					const unsigned char **rpout);
		bool	readLE(const unsigned char *rp,
					uint64_t *value,
					const char *name,
					uint64_t expected,
					const unsigned char **rpout);
		void	readBE(const unsigned char *rp,
					uint64_t *value,
					const unsigned char **rpout);
		bool	readBE(const unsigned char *rp,
					uint64_t *value,
					const char *name,
					uint64_t expected,
					const unsigned char **rpout);
		uint64_t	readLenEncInt(const unsigned char *in,
						const unsigned char **out);

		void	write(bytebuffer *buffer, char value);
		void	write(bytebuffer *buffer, unsigned char value);
		void	write(bytebuffer *buffer, const char *value);
		void	write(bytebuffer *buffer, const char *value,
								size_t length);
		void	write(bytebuffer *buffer, const unsigned char *value,
								size_t length);
		void	write(bytebuffer *buffer, char16_t *str, size_t length);
		void	write(bytebuffer *buffer, float value);
		void	write(bytebuffer *buffer, double value);
		void	write(bytebuffer *buffer, uint16_t value);
		void	writeLE(bytebuffer *buffer, uint16_t value);
		void	writeBE(bytebuffer *buffer, uint16_t value);
		void	write(bytebuffer *buffer, uint32_t value);
		void	writeLE(bytebuffer *buffer, uint32_t value);
		void	writeBE(bytebuffer *buffer, uint32_t value);
		void	write(bytebuffer *buffer, uint64_t value);
		void	writeLE(bytebuffer *buffer, uint64_t value);
		void	writeBE(bytebuffer *buffer, uint64_t value);
		void	writeLenEncInt(bytebuffer *buffer,
						uint64_t value);
		void	writeLenEncStr(bytebuffer *buffer,
						const char *string);
		void	writeLenEncStr(bytebuffer *buffer,
						const char *string,
						uint64_t length);
		void	writeTriplet(bytebuffer *buffer, uint32_t value);

		uint16_t	toHost(uint16_t value);
		uint32_t	toHost(uint32_t value);
		uint64_t	toHost(uint64_t value);
		uint16_t	leToHost(uint16_t value);
		uint32_t	leToHost(uint32_t value);
		uint64_t	leToHost(uint64_t value);
		uint16_t	beToHost(uint16_t value);
		uint32_t	beToHost(uint32_t value);
		uint64_t	beToHost(uint64_t value);

		uint16_t	hostTo(uint16_t value);
		uint32_t	hostTo(uint32_t value);
		uint64_t	hostTo(uint64_t value);
		uint16_t	hostToLE(uint16_t value);
		uint32_t	hostToLE(uint32_t value);
		uint64_t	hostToLE(uint64_t value);
		uint16_t	hostToBE(uint16_t value);
		uint32_t	hostToBE(uint32_t value);
		uint64_t	hostToBE(uint64_t value);

		bool	getDebug();

		void	debugStart(const char *title);
		void	debugStart(const char *title, uint16_t indent);
		void	debugEnd();
		void	debugEnd(uint16_t indent);

		void	debugHexDump(const unsigned char *data,
						uint64_t size);
		void	debugHexDump(const unsigned char *data,
						uint64_t size,
						uint16_t indent);

		sqlrservercontroller	*cont;

	#include <sqlrelay/private/sqlrprotocol.h>
};

class SQLRSERVER_DLLSPEC sqlrprotocols {
	public:
		sqlrprotocols(sqlrservercontroller *cont);
		~sqlrprotocols();

		bool		load(domnode *listeners);
		sqlrprotocol	*getProtocol(uint16_t port);

		void	endTransaction(bool commit);
		void	endSession();

	#include <sqlrelay/private/sqlrprotocols.h>
};

class SQLRSERVER_DLLSPEC sqlrcredentials {
	public:
		sqlrcredentials();
		virtual	~sqlrcredentials();
		virtual const char	*getType()=0;
};

class SQLRSERVER_DLLSPEC sqlruserpasswordcredentials : public sqlrcredentials {
	public:
		sqlruserpasswordcredentials();
		~sqlruserpasswordcredentials();
		const char	*getType();

		void	setUser(const char *user);
		void	setPassword(const char *password);

		const char	*getUser();
		const char	*getPassword();

	#include <sqlrelay/private/sqlruserpasswordcredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrgsscredentials : public sqlrcredentials {
	public:
		sqlrgsscredentials();
		~sqlrgsscredentials();
		const char	*getType();

		void		setInitiator(const char *initiator);
		const char	*getInitiator();

	#include <sqlrelay/private/sqlrgsscredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrtlscredentials : public sqlrcredentials {
	public:
		sqlrtlscredentials();
		~sqlrtlscredentials();
		const char	*getType();

		void	setCommonName(const char *commonname);
		void	setSubjectAlternateNames(
				linkedlist < char * > *subjectalternatenames);

		const char		*getCommonName();
		linkedlist< char * >	*getSubjectAlternateNames();

	#include <sqlrelay/private/sqlrtlscredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrmysqlcredentials : public sqlrcredentials {
	public:
		sqlrmysqlcredentials();
		~sqlrmysqlcredentials();
		const char	*getType();

		void	setUser(const char *user);
		void	setPassword(const char *password);
		void	setPasswordLength(uint64_t passwordlength);
		void	setMethod(const char *method);
		void	setExtra(const char *extra);

		const char	*getUser();
		const char	*getPassword();
		uint64_t	getPasswordLength();
		const char	*getMethod();
		const char	*getExtra();

	#include <sqlrelay/private/sqlrmysqlcredentials.h>
};

class SQLRSERVER_DLLSPEC sqlrauth {
	public:
		sqlrauth(sqlrservercontroller *cont,
					sqlrauths *auths,
					sqlrpwdencs *sqlrpe,
					domnode *parameters);
		virtual	~sqlrauth();
		virtual	const char	*auth(sqlrcredentials *cred);

	protected:
		sqlrauths	*getAuths();
		sqlrpwdencs	*getPasswordEncryptions();
		domnode	*getParameters();

		sqlrservercontroller	*cont;

	#include <sqlrelay/private/sqlrauth.h>
};

class SQLRSERVER_DLLSPEC sqlrauths {
	public:
		sqlrauths(sqlrservercontroller *cont);
		~sqlrauths();

		bool		load(domnode *parameters,
					sqlrpwdencs *sqlrpe);
		const char	*auth(sqlrcredentials *cred);

		void	endSession();

	#include <sqlrelay/private/sqlrauths.h>
};

class SQLRSERVER_DLLSPEC sqlrpwdenc {
	public:
		sqlrpwdenc(domnode *parameters, bool debug);
		virtual	~sqlrpwdenc();
		virtual const char	*getId();
		virtual	bool	oneWay();
		virtual	char	*encrypt(const char *value);
		virtual	char	*decrypt(const char *value);

	protected:
		domnode	*getParameters();
		bool		getDebug();

	#include <sqlrelay/private/sqlrpwdenc.h>
};

class SQLRSERVER_DLLSPEC sqlrpwdencs {
	public:
		sqlrpwdencs(sqlrpaths *sqlrpth, bool debug);
		~sqlrpwdencs();

		bool		load(domnode *parameters);
		sqlrpwdenc	*getPasswordEncryptionById(const char *id);

	#include <sqlrelay/private/sqlrpwdencs.h>
};

enum sqlrevent_t {
	SQLREVENT_CLIENT_CONNECTED=0,
	SQLREVENT_CLIENT_CONNECTION_REFUSED,
	SQLREVENT_CLIENT_DISCONNECTED,
	SQLREVENT_CLIENT_PROTOCOL_ERROR,
	SQLREVENT_DB_LOGIN,
	SQLREVENT_DB_LOGOUT,
	SQLREVENT_DB_ERROR,
	SQLREVENT_DB_WARNING,
	SQLREVENT_QUERY,
	SQLREVENT_FILTER_VIOLATION,
	SQLREVENT_INTERNAL_ERROR,
	SQLREVENT_INTERNAL_WARNING,
	SQLREVENT_DEBUG_MESSAGE,
	SQLREVENT_SCHEDULE_VIOLATION,
	SQLREVENT_INTEGRITY_VIOLATION,
	SQLREVENT_TRANSLATION_FAILURE,
	SQLREVENT_PARSE_FAILURE,
	SQLREVENT_CURSOR_OPEN,
	SQLREVENT_CURSOR_CLOSE,
	SQLREVENT_BEGIN_TRANSACTION,
	SQLREVENT_COMMIT,
	SQLREVENT_ROLLBACK,
	SQLREVENT_INVALID_EVENT
};

enum sqlrlogger_loglevel_t {
	SQLRLOGGER_LOGLEVEL_DEBUG=0,
	SQLRLOGGER_LOGLEVEL_INFO,
	SQLRLOGGER_LOGLEVEL_WARNING,
	SQLRLOGGER_LOGLEVEL_ERROR
};

class SQLRSERVER_DLLSPEC sqlrlogger {
	public:
		sqlrlogger(sqlrloggers *ls, domnode *parameters);
		virtual	~sqlrlogger();

		virtual bool	init(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon);
		virtual bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrevent_t event,
					const char *info);
		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrloggers	*getLoggers();
		domnode	*getParameters();

	#include <sqlrelay/private/sqlrlogger.h>
};

class SQLRSERVER_DLLSPEC sqlrloggers {
	public:
		sqlrloggers(sqlrpaths *sqlrpth);
		~sqlrloggers();

		bool	load(domnode *parameters);
		void	init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon);
		void	run(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrlogger_loglevel_t level,
				sqlrevent_t event,
				const char *info);

		void	endTransaction(bool commit);
		void	endSession();

		const char	*logLevel(sqlrlogger_loglevel_t level);
		sqlrlogger_loglevel_t	logLevel(const char *level);

		const char	*eventType(sqlrevent_t event);
		sqlrevent_t	eventType(const char *event);

	#include <sqlrelay/private/sqlrloggers.h>
};

class SQLRSERVER_DLLSPEC sqlrnotification {
	public:
		sqlrnotification(sqlrnotifications *ns,
					domnode *parameters);
		virtual	~sqlrnotification();

		virtual bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);
		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrnotifications	*getNotifications();
		domnode		*getParameters();

	#include <sqlrelay/private/sqlrnotification.h>
};

class SQLRSERVER_DLLSPEC sqlrnotifications {
	public:
		sqlrnotifications(sqlrpaths *sqlrpth);
		~sqlrnotifications();

		bool	load(domnode *parameters);
		void	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);

		void	endTransaction(bool commit);
		void	endSession();

		const char	*eventType(sqlrevent_t event);
		sqlrevent_t	eventType(const char *event);

		bool	sendNotification(sqlrlistener *sqlrl,
						sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *address,
						const char *transportid,
						const char *subject,
						const char *templatefile,
						sqlrevent_t event,
						const char *info);

		domnode	*getTransport(const char *transportid);

	#include <sqlrelay/private/sqlrnotifications.h>
};

class SQLRSERVER_DLLSPEC sqlrscheduleperiod {
	public:
		uint16_t	start;
		uint16_t	end;
};

class SQLRSERVER_DLLSPEC sqlrscheduledaypart {
	public:
		uint16_t	starthour;
		uint16_t	startminute;
		uint16_t	endhour;
		uint16_t	endminute;
};

class SQLRSERVER_DLLSPEC sqlrschedulerule {
	public:
		sqlrschedulerule(bool allow, const char *when);
		sqlrschedulerule(bool allow,
			const char *years,
			const char *months,
			const char *daysofmonth,
			const char *daysofweek,
			const char *dayparts);
		~sqlrschedulerule();

		bool	allowed(datetime *dt, bool currentlyallowed);
		
	#include <sqlrelay/private/sqlrschedulerule.h>
};

class SQLRSERVER_DLLSPEC sqlrschedule {
	public:
		sqlrschedule(sqlrservercontroller *cont,
					sqlrschedules *ss,
					domnode *parameters);
		virtual	~sqlrschedule();

		virtual bool	allowed(sqlrserverconnection *sqlrcon,
							const char *user);

		virtual	void	addRule(bool allow, const char *when);
		virtual	void	addRule(bool allow,
					const char *years,
					const char *months,
					const char *daysofmonth,
					const char *daysofweek,
					const char *dayparts);

		virtual	bool	rulesAllow(datetime *dt, bool currentlyallowed);

	protected:
		sqlrschedules	*getSchedules();
		domnode	*getParameters();

	#include <sqlrelay/private/sqlrschedule.h>
};

class SQLRSERVER_DLLSPEC sqlrschedules {
	public:
		sqlrschedules(sqlrservercontroller *cont);
		~sqlrschedules();

		bool	load(domnode *parameters);
		bool	allowed(sqlrserverconnection *sqlrcon,
						const char *user);

		void	endSession();

	#include <sqlrelay/private/sqlrschedules.h>
};

class SQLRSERVER_DLLSPEC sqlrrouter {
	public:
		sqlrrouter(sqlrservercontroller *cont,
				sqlrrouters *rs,
				domnode *parameters);
		virtual	~sqlrrouter();

		virtual const char *route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);

		virtual	bool	routeEntireSession();

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrrouters	*getRouters();
		domnode	*getParameters();
		const char 	**getConnectionIds();
		sqlrconnection 	**getConnections();
		uint16_t	getConnectionCount();

	#include <sqlrelay/private/sqlrrouter.h>
};

class SQLRSERVER_DLLSPEC sqlrrouters {
	public:
		sqlrrouters(sqlrservercontroller *cont,
				const char **connectionids,
				sqlrconnection **connections,
				uint16_t connectioncount);
		~sqlrrouters();

		bool		load(domnode *parameters);
		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);
		bool	routeEntireSession();

		void	endTransaction(bool commit);
		void	endSession();

		const char	*getCurrentConnectionId();
		const char 	**getConnectionIds();
		sqlrconnection 	**getConnections();
		uint16_t	getConnectionCount();

	#include <sqlrelay/private/sqlrrouters.h>
};

class SQLRSERVER_DLLSPEC sqlrparser {
	public:
		sqlrparser(sqlrservercontroller *cont,
				domnode *parameters);
		virtual	~sqlrparser();

		virtual	bool	parse(const char *query);
		virtual	void	useTree(xmldom *tree);
		virtual	xmldom	*getTree();
		virtual	xmldom	*detachTree();

		virtual	bool	write(stringbuffer *output);
		virtual	bool	write(domnode *node,
					stringbuffer *output,
					bool omitsiblings);
		virtual	bool	write(domnode *node, 
					stringbuffer *output);

		virtual void	getMetaData(domnode *node);

		virtual void	endSession();

	protected:
		domnode	*getParameters();

	#include <sqlrelay/private/sqlrparser.h>
};

class SQLRSERVER_DLLSPEC sqlrdirective {
	public:
		sqlrdirective(sqlrservercontroller *cont,
					sqlrdirectives *sqlts,
					domnode *parameters);
		virtual	~sqlrdirective();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
	protected:
		sqlrdirectives	*getDirectives();
		domnode	*getParameters();
		bool		getDirective(const char *line,
						const char **directivestart,
						uint32_t *directivelength,
						const char **newline);

	#include <sqlrelay/private/sqlrdirective.h>
};

class SQLRSERVER_DLLSPEC sqlrdirectives {
	public:
		sqlrdirectives(sqlrservercontroller *cont);
		~sqlrdirectives();

		bool	load(domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);

	#include <sqlrelay/private/sqlrdirectives.h>
};

class SQLRSERVER_DLLSPEC sqlrtranslation {
	public:
		sqlrtranslation(sqlrservercontroller *cont,
					sqlrtranslations *sqlts,
					domnode *parameters);
		virtual	~sqlrtranslation();

		virtual bool	usesTree();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					uint32_t querylength,
					stringbuffer *translatedquery);

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);

		virtual const char	*getError();

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrtranslations	*getTranslations();
		domnode			*getParameters();

	#include <sqlrelay/private/sqlrtranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrtranslations {
	public:
		sqlrtranslations(sqlrservercontroller *cont);
		~sqlrtranslations();

		bool	load(domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						sqlrparser *sqlrp,
						const char *query,
						uint32_t querylength,
						stringbuffer *translatedquery);

		const char	*getError();

		void	endTransaction(bool commit);
		void	endSession();

		void	setReplacementTableName(const char *database,
						const char *schema,
						const char *oldtable,
						const char *newtable);
		void	setReplacementIndexName(const char *database,
						const char *schema,
						const char *oldindex,
						const char *newindex,
						const char *table);

		bool	getReplacementTableName(const char *database,
						const char *schema,
						const char *oldtable,
						const char **newtable);
		bool	getReplacementIndexName(const char *database,
						const char *schema,
						const char *oldtable,
						const char **newtable);

		bool	removeReplacementTable(const char *database,
						const char *schema,
						const char *table);
		bool	removeReplacementIndex(const char *database,
						const char *schema,
						const char *index);

		bool	getUseOriginalOnError();

	#include <sqlrelay/private/sqlrtranslations.h>
};

class SQLRSERVER_DLLSPEC sqlrfilter {
	public:
		sqlrfilter(sqlrservercontroller *cont,
					sqlrfilters *fs,
					domnode *parameters);
		virtual	~sqlrfilter();

		virtual bool	usesTree();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);

		virtual void	getError(const char **err, int64_t *errn);

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrfilters	*getFilters();
		domnode	*getParameters();

	#include <sqlrelay/private/sqlrfilter.h>
};

class SQLRSERVER_DLLSPEC sqlrfilters {
	public:
		sqlrfilters(sqlrservercontroller *cont);
		~sqlrfilters();

		bool	load(domnode *parameters);
		bool	runBeforeFilters(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						sqlrparser *sqlrp,
						const char *query,
						const char **err,
						int64_t *errn);
		bool	runAfterFilters(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						sqlrparser *sqlrp,
						const char *query,
						const char **err,
						int64_t *errn);

		void	endTransaction(bool commit);
		void	endSession();

	#include <sqlrelay/private/sqlrfilters.h>
};

class SQLRSERVER_DLLSPEC sqlrbindvariabletranslation {
	public:
		sqlrbindvariabletranslation(sqlrservercontroller *cont,
					sqlrbindvariabletranslations *bvts,
					domnode *parameters);
		virtual	~sqlrbindvariabletranslation();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur);

		virtual const char	*getError();

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrbindvariabletranslations	*getBindVariableTranslations();
		domnode				*getParameters();

	#include <sqlrelay/private/sqlrbindvariabletranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrbindvariabletranslations {
	public:
		sqlrbindvariabletranslations(sqlrservercontroller *cont);
		~sqlrbindvariabletranslations();

		bool	load(domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur);

		const char	*getError();

		void	endTransaction(bool commit);
		void	endSession();

	#include <sqlrelay/private/sqlrbindvariabletranslations.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsettranslation {
	public:
		sqlrresultsettranslation(sqlrservercontroller *cont,
						sqlrresultsettranslations *rs,
						domnode *parameters);
		virtual	~sqlrresultsettranslation();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint32_t fieldindex,
					const char **field,
					uint64_t *fieldlength);

		virtual const char	*getError();

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrresultsettranslations	*getResultSetTranslations();
		domnode			*getParameters();

	#include <sqlrelay/private/sqlrresultsettranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsettranslations {
	public:
		sqlrresultsettranslations(sqlrservercontroller *cont);
		~sqlrresultsettranslations();

		bool	load(domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *fieldname,
						uint32_t fieldindex,
						const char **field,
						uint64_t *fieldlength);

		const char	*getError();

		void	endTransaction(bool commit);
		void	endSession();

	#include <sqlrelay/private/sqlrresultsettranslations.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsetrowtranslation {
	public:
		sqlrresultsetrowtranslation(
					sqlrservercontroller *cont,
					sqlrresultsetrowtranslations *rs,
					domnode *parameters);
		virtual	~sqlrresultsetrowtranslation();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char ***fields,
					uint64_t **fieldlengths);

		virtual const char	*getError();

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrresultsetrowtranslations	*getResultSetRowTranslations();
		domnode			*getParameters();

	#include <sqlrelay/private/sqlrresultsetrowtranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsetrowtranslations {
	public:
		sqlrresultsetrowtranslations(sqlrservercontroller *cont);
		~sqlrresultsetrowtranslations();

		bool	load(domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char ***fields,
						uint64_t **fieldlengths);

		const char	*getError();

		void	endTransaction(bool commit);
		void	endSession();

	#include <sqlrelay/private/sqlrresultsetrowtranslations.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsetrowblocktranslation {
	public:
		sqlrresultsetrowblocktranslation(
					sqlrservercontroller *cont,
					sqlrresultsetrowblocktranslations *rs,
					domnode *parameters);
		virtual	~sqlrresultsetrowblocktranslation();

		virtual bool	setRow(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char * const *fields,
					uint64_t *fieldlengths,
					bool *blobs,
					bool *nulls);
		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames);
		virtual bool	getRow(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***fields,
					uint64_t **fieldlengths,
					bool **blobs,
					bool **nulls);

		virtual const char	*getError();

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrresultsetrowblocktranslations
					*getResultSetRowBlockTranslations();
		domnode			*getParameters();

	#include <sqlrelay/private/sqlrresultsetrowblocktranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsetrowblocktranslations {
	public:
		sqlrresultsetrowblocktranslations(sqlrservercontroller *cont);
		~sqlrresultsetrowblocktranslations();

		bool	load(domnode *parameters);

		uint64_t	getRowBlockSize();

		bool	setRow(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char * const *fields,
					uint64_t *fieldlengths,
					bool *blobs,
					bool *nulls);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames);
		bool	getRow(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***fields,
					uint64_t **fieldlengths,
					bool **blobs,
					bool **nulls);

		const char	*getError();

		void	endTransaction(bool commit);
		void	endSession();

	#include <sqlrelay/private/sqlrresultsetrowblocktranslations.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsetheadertranslation {
	public:
		sqlrresultsetheadertranslation(
					sqlrservercontroller *cont,
					sqlrresultsetheadertranslations *rs,
					domnode *parameters);
		virtual	~sqlrresultsetheadertranslation();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***columnnames,
					uint16_t **columnnamelengths,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamelengths,
					uint32_t **columnlengths,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablelengths);

		virtual const char	*getError();

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrresultsetheadertranslations
					*getResultSetHeaderTranslations();
		domnode		*getParameters();

	#include <sqlrelay/private/sqlrresultsetheadertranslation.h>
};

class SQLRSERVER_DLLSPEC sqlrresultsetheadertranslations {
	public:
		sqlrresultsetheadertranslations(sqlrservercontroller *cont);
		~sqlrresultsetheadertranslations();

		bool	load(domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***columnnames,
					uint16_t **columnnamelengths,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamelengths,
					uint32_t **columnlengths,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablelengths);

		const char	*getError();

		void	endTransaction(bool commit);
		void	endSession();

	#include <sqlrelay/private/sqlrresultsetheadertranslations.h>
};

class SQLRSERVER_DLLSPEC sqlrtrigger {
	public:
		sqlrtrigger(sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters);
		virtual	~sqlrtrigger();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					bool before,
					bool *success);

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrtriggers	*getTriggers();
		domnode	*getParameters();

	#include <sqlrelay/private/sqlrtrigger.h>
};

class SQLRSERVER_DLLSPEC sqlrtriggers {
	public:
		sqlrtriggers(sqlrservercontroller *cont);
		~sqlrtriggers();

		bool	load(domnode *parameters);
		void	runBeforeTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
		void	runAfterTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool *success);

		void	endTransaction(bool commit);
		void	endSession();

	#include <sqlrelay/private/sqlrtriggers.h>
};

class SQLRSERVER_DLLSPEC sqlrquery {
	public:
		sqlrquery(sqlrservercontroller *cont,
				sqlrqueries *qs,
				domnode *parameters);
		virtual	~sqlrquery();

		virtual bool	match(const char *querystring,
						uint32_t querylength);
		virtual sqlrquerycursor	*newCursor(	
						sqlrserverconnection *sqlrcon,
						uint16_t id);

		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		sqlrqueries	*getQueries();
		domnode	*getParameters();

	#include <sqlrelay/private/sqlrquery.h>
};

class SQLRSERVER_DLLSPEC sqlrquerycursor : public sqlrservercursor {
	public:
		sqlrquerycursor(sqlrserverconnection *conn,
					sqlrquery *q,
					domnode *parameters,
					uint16_t id);
		virtual	~sqlrquerycursor();
		virtual sqlrquerytype_t	queryType(const char *query,
							uint32_t length);
		bool	isCustomQuery();

	protected:
		sqlrquery	*getQuery();
		sqlrqueries	*getQueries();
		domnode	*getParameters();

	#include <sqlrelay/private/sqlrquerycursor.h>
};

class SQLRSERVER_DLLSPEC sqlrqueries {
	public:
		sqlrqueries(sqlrservercontroller *cont);
		~sqlrqueries();

		bool		load(domnode *parameters);
		sqlrquerycursor	*match(sqlrserverconnection *sqlrcon,
						const char *querystring,
						uint32_t querylength,
						uint16_t id);

		void	endTransaction(bool commit);
		void	endSession();

	#include <sqlrelay/private/sqlrqueries.h>
};

class SQLRSERVER_DLLSPEC sqlrmoduledata {
	public:
		sqlrmoduledata(domnode *parameters);
		virtual	~sqlrmoduledata();

		const char	*getModuleType();
		const char	*getId();

		domnode		*getParameters();

	#include <sqlrelay/private/sqlrmoduledata.h>
};

class SQLRSERVER_DLLSPEC sqlrmoduledatas {
	public:
		sqlrmoduledatas(sqlrservercontroller *cont);
		~sqlrmoduledatas();

		bool	load(domnode *parameters);

		sqlrmoduledata	*getModuleData(const char *id);

	#include <sqlrelay/private/sqlrmoduledatas.h>
};

#endif
