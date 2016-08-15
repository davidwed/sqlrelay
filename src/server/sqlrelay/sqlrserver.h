// Copyright (c) 1999-2016  David Muse
// See the file COPYING for more information

#ifndef SQLRSERVER_H
#define SQLRSERVER_H

#include <sqlrelay/private/sqlrserverincludes.h>

class sqlrserverconnection;
class sqlrservercursor;
class sqlrprotocol;
class sqlrprotocols;
class sqlrcredentials;
class sqlrauth;
class sqlrauths;
class sqlrpwdenc;
class sqlrpwdencs;
class sqlrlogger;
class sqlrloggers;
class sqlrnotification;
class sqlrnotifications;
class sqlrscheduleperiod;
class sqlrscheduledaypart;
class sqlrschedulerule;
class sqlrschedule;
class sqlrschedules;
class sqlrparser;
class sqlrtranslation;
class sqlrtranslations;
class sqlrfilter;
class sqlrfilters;
class sqlrresultsettranslation;
class sqlrresultsettranslations;
class sqlrtrigger;
class sqlrtriggers;
class sqlrquery;
class sqlrquerycursor;
class sqlrqueries;

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
	SQLRQUERYTYPE_ETC
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
	SQLRSERVERBINDVARTYPE_DATE
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
				char		*buffer;
				uint16_t	buffersize;
			} dateval;
			uint16_t	cursorid;
		} value;
		uint32_t		valuesize;
		uint32_t		resultvaluesize;
		sqlrserverbindvartype_t	type;
		int16_t			isnull;
};

class SQLRSERVER_DLLSPEC sqlrservercontroller {
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

		// client auth
		void		setUser(const char *user);
		void		setPassword(const char *password);
		const char	*getUser();
		const char	*getPassword();
		bool		auth(const char *userbuffer,
						const char *passwordbuffer);
		bool		auth(const char *userbuffer,
						const char *passwordbuffer,
						const char *method,
						const char *extra);
		bool		auth(sqlrcredentials *cred);
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
		void		dontFakeInputBinds();
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
		void	setInterceptTransactionQueriesBehavior(bool itxqb);
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
		void	disableInstance();
		void	enableInstance();
		bool	disabledInstance();


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

		// query tree
		xmldom		*getQueryTree(sqlrservercursor *cursor);

		// running queries
		bool	prepareQuery(sqlrservercursor *cursor,
						const char *query,
						uint32_t length);
		bool	executeQuery(sqlrservercursor *cursor);
		bool	executeQuery(sqlrservercursor *cursor,
						bool enabletranslations,
						bool enablefilters,
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
		const char	*getGlobalTempTableListQuery();
		const char	*getColumnListQuery(const char *table,
								bool wild);

		// column info
		uint16_t	getSendColumnInfo();
		void		setSendColumnInfo(uint16_t sendcolumninfo);
		uint32_t	colCount(sqlrservercursor *cursor);
		uint16_t	columnTypeFormat(sqlrservercursor *cursor);
		void		setDatabaseListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setTableListColumnMap(
					sqlrserverlistformat_t listformat);
		void		setColumnListColumnMap(
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
						const char *name,
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
						bool ignorenondatetime,
						const char *datedelimiters,
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

		// query parser
		sqlrparser	*getParser();

		// gss
		gsscontext	*getGSSContext();

		// tls
		tlscontext	*getTLSContext();

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
				int16_t *fraction);

		char	*convertDateTime(const char *format,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int16_t fraction);

		const char	*asciiToHex(unsigned char ch);
		const char	*asciiToOctal(unsigned char ch);

		uint16_t	countBindVariables(const char *query);

		bool	isBitType(const char *type);
		bool	isBoolType(const char *type);
		bool	isFloatType(const char *type);
		bool	isNumberType(const char *type);
		bool	isNumberType(int16_t type);
		bool	isBlobType(const char *type);
		bool	isUnsignedType(const char *type);
		bool	isBinaryType(const char *type);
		bool	isDateTimeType(const char *type);
		bool	isDateTimeType(int16_t type);

		const char * const	*dataTypeStrings();

		// listener
		listener		lsnr;

		// connection
		sqlrserverconnection	*conn;

		// configuration
		sqlrconfigs		*sqlrcfgs;
		sqlrconfig		*cfg;
		sqlrpaths		*pth;

		// statistics
		shmdata			*shm;
		sqlrconnstatistics	*connstats;

	#include <sqlrelay/private/sqlrservercontroller.h>
};


class SQLRSERVER_DLLSPEC sqlrserverconnection {
	public:
			sqlrserverconnection(sqlrservercontroller *cont);
		virtual	~sqlrserverconnection();

		virtual bool	mustDetachBeforeLogIn();

		virtual bool	supportsAuthOnDatabase();
		virtual	void	handleConnectString()=0;

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

		virtual bool		getLastInsertId(uint64_t *id);
		virtual const char	*getLastInsertIdQuery();

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

		virtual bool		getListsByApiCalls();
		virtual bool		getDatabaseList(
						sqlrservercursor *cursor,
						const char *wild);
		virtual bool		getTableList(
						sqlrservercursor *cursor,
						const char *wild);
		virtual bool		getColumnList(
						sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		virtual const char	*getDatabaseListQuery(bool wild);
		virtual const char	*getTableListQuery(bool wild);
		virtual const char	*getGlobalTempTableListQuery();
		virtual const char	*getColumnListQuery(
						const char *table,
						bool wild);
		virtual bool		isSynonym(const char *table);
		virtual const char	*isSynonymQuery();

		virtual sqlrservercursor	*newCursor(uint16_t id)=0;
		virtual void		deleteCursor(sqlrservercursor *curs)=0;

		virtual	const char	*bindFormat();
		virtual	int16_t		nonNullBindValue();
		virtual	int16_t		nullBindValue();
		virtual char		bindVariablePrefix();
		virtual bool		bindValueIsNull(int16_t isnull);

		virtual const char	*tempTableDropPrefix();
		virtual bool		tempTableTruncateBeforeDrop();

		virtual void		endSession();

		bool	getAutoCommit();
		void	setAutoCommit(bool autocommit);
		bool	getFakeAutoCommit();

		void	clearError();
		void	setError(const char *err, int64_t errn, bool liveconn);

		char		*getErrorBuffer();
		uint32_t	getErrorLength();
		void		setErrorLength(uint32_t errorlength);
		uint32_t	getErrorNumber();
		void		setErrorNumber(uint32_t errnum);
		bool		getLiveConnection();
		void		setLiveConnection(bool liveconnection);
		bool		getErrorSetManually();
		void		setErrorSetManually(bool errorsetmanually);

		sqlrservercontroller	*cont;

	#include <sqlrelay/private/sqlrserverconnection.h>
};


class SQLRSERVER_DLLSPEC sqlrservercursor {
	public:
			sqlrservercursor(sqlrserverconnection *conn,
							uint16_t id);
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
						const char *tz);
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
						uint16_t valuesize,
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
		virtual void	checkForTempTable(const char *query,
							uint32_t length);
		virtual	const char	*truncateTableQuery();
		virtual	bool		executeQuery(const char *query,
							uint32_t length);
		virtual bool	fetchFromBindCursor();
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
		virtual bool		ignoreDateDdMmParameter(uint32_t col,
							const char *data,
							uint32_t size);
		virtual	bool	noRowsToReturn();
		virtual	bool	skipRow();
		virtual	bool	fetchRow();
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
		virtual bool	getColumnNameList(stringbuffer *output);


		uint16_t	getId();

		bool		fakeInputBinds();

		void		setInputBindCount(uint16_t inbindcount);
		uint16_t	getInputBindCount();
		sqlrserverbindvar	*getInputBinds();

		void		setOutputBindCount(uint16_t outbindcount);
		uint16_t	getOutputBindCount();
		sqlrserverbindvar	*getOutputBinds();

		void	performSubstitution(stringbuffer *buffer,
							int16_t index);
		virtual void	encodeBlob(stringbuffer *buffer,
					const char *data, uint32_t datasize);
		void	abort();

		char		*getQueryBuffer();
		uint32_t 	getQueryLength();
		void		setQueryLength(uint32_t querylength);

		void		setQueryStatus(sqlrquerystatus_t status);
		sqlrquerystatus_t	getQueryStatus();

		void		setQueryTree(xmldom *tree);
		xmldom		*getQueryTree();
		void		clearQueryTree();

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

		void			setState(sqlrcursorstate_t state);
		sqlrcursorstate_t	getState();

		void		setCustomQueryCursor(sqlrquerycursor *cur);
		sqlrquerycursor	*getCustomQueryCursor();
		void		clearCustomQueryCursor();

		void		clearTotalRowsFetched();
		uint64_t	getTotalRowsFetched();
		void		incrementTotalRowsFetched();

		void	clearError();
		void	setError(const char *err, int64_t errn, bool liveconn);

		char		*getErrorBuffer();
		uint32_t	getErrorLength();
		void		setErrorLength(uint32_t errorlength);
		uint32_t	getErrorNumber();
		void		setErrorNumber(uint32_t errnum);
		bool		getLiveConnection();
		void		setLiveConnection(bool liveconnection);
		bool		getErrorSetManually();
		void		setErrorSetManually(bool errorsetmanually);

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
					xmldomnode *parameters,
					bool debug);
		virtual	~sqlrprotocol();

		void	setClientSocket(filedescriptor *clientsock);

		virtual gsscontext	*getGSSContext();
		virtual tlscontext	*getTLSContext();

		virtual clientsessionexitstatus_t	clientSession()=0;

	protected:
		sqlrservercontroller	*cont;
		xmldomnode		*parameters;
		bool			debug;

		filedescriptor		*clientsock;

		gsscredentials	gcred;
		gssmechanism	gmech;
		gsscontext	gctx;
		tlscontext	tctx;
};


class SQLRSERVER_DLLSPEC sqlrprotocolplugin {
	public:
		sqlrprotocol	*pr;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrprotocols {
	public:
			sqlrprotocols(sqlrservercontroller *cont,
						sqlrpaths *sqlrpth,
						bool debug);
			~sqlrprotocols();

		bool		load(xmldomnode *listeners);
		sqlrprotocol	*getProtocol(uint16_t port);
	private:
		void	unload();
		void	loadProtocol(uint16_t index, xmldomnode *listener);

		sqlrservercontroller	*cont;
		const char		*libexecdir;
		bool			debug;

		dictionary< uint16_t , sqlrprotocolplugin * >	protos;
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
		virtual	~sqlruserpasswordcredentials();
		const char	*getType();

		void	setUser(const char *user);
		void	setPassword(const char *password);

		const char	*getUser();
		const char	*getPassword();
	private:
		const char	*user;
		const char	*password;
};

class SQLRSERVER_DLLSPEC sqlrgsscredentials : public sqlrcredentials {
	public:
			sqlrgsscredentials();
		virtual	~sqlrgsscredentials();
		const char	*getType();

		void	setInitiator(const char *initiator);

		const char	*getInitiator();
	private:
		const char	*initiator;
};

class SQLRSERVER_DLLSPEC sqlrtlscredentials : public sqlrcredentials {
	public:
			sqlrtlscredentials();
		virtual	~sqlrtlscredentials();
		const char	*getType();

		void	setCommonName(const char *commonname);
		void	setSubjectAlternateNames(
				linkedlist < char * > *subjectalternatenames);

		const char		*getCommonName();
		linkedlist< char * >	*getSubjectAlternateNames();
	private:
		const char		*commonname;
		linkedlist< char * >	*subjectalternatenames;
};


class SQLRSERVER_DLLSPEC sqlrauth {
	public:
			sqlrauth(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe,
					bool debug);
		virtual	~sqlrauth();
		virtual void	setParameter(const char *name,
							const char *value);
		virtual	const char	*auth(sqlrserverconnection *sqlrcon,
							sqlrcredentials *cred);
		virtual	bool	auth(sqlrserverconnection *sqlrcon,
							const char *user,
							const char *password,
							const char *method,
							const char *extra);
	protected:
		xmldomnode		*parameters;
		sqlrpwdencs		*sqlrpe;
		bool			debug;
};


class SQLRSERVER_DLLSPEC sqlrauthplugin {
	public:
		sqlrauth	*au;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrauths {
	public:
			sqlrauths(sqlrpaths *sqlrpth, bool debug);
			~sqlrauths();

		bool	load(xmldomnode *parameters, sqlrpwdencs *sqlrpe);
		bool	auth(sqlrserverconnection *sqlrcon,
					const char *user, const char *password);
		bool	auth(sqlrserverconnection *sqlrcon,
					const char *user, const char *password,
					const char *method, const char *extra);
		const char	*auth(sqlrserverconnection *sqlrcon,
						sqlrcredentials *cred);
	private:
		void	unload();
		void	loadAuth(xmldomnode *auth, sqlrpwdencs *sqlrpe);

		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrauthplugin * >	llist;
};


class SQLRSERVER_DLLSPEC sqlrpwdenc {
	public:
			sqlrpwdenc(xmldomnode *parameters);
		virtual	~sqlrpwdenc();
		virtual const char	*getId();
		virtual	bool	oneWay();
		virtual	char	*encrypt(const char *value);
		virtual	char	*decrypt(const char *value);
	protected:
		xmldomnode	*parameters;
};


class SQLRSERVER_DLLSPEC sqlrpwdencplugin {
	public:
		sqlrpwdenc	*pe;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrpwdencs {
	public:
			sqlrpwdencs(sqlrpaths *sqlrpth);
			~sqlrpwdencs();

		bool		load(xmldomnode *parameters);
		sqlrpwdenc	*getPasswordEncryptionById(const char *id);
	private:
		void	unload();
		void	loadPasswordEncryption(xmldomnode *pwdenc);

		const char	*libexecdir;

		singlylinkedlist< sqlrpwdencplugin * >	llist;
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
			sqlrlogger(xmldomnode *parameters);
		virtual	~sqlrlogger();

		virtual bool	init(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon);
		virtual bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrevent_t event,
					const char *info);
	protected:
		const char	*logLevel(sqlrlogger_loglevel_t level);
		const char	*eventType(sqlrevent_t event);
		xmldomnode	*parameters;
};


class SQLRSERVER_DLLSPEC sqlrloggerplugin {
	public:
		sqlrlogger	*lg;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrloggers {
	public:
			sqlrloggers(sqlrpaths *sqlrpth);
			~sqlrloggers();

		bool	load(xmldomnode *parameters);
		void	init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon);
		void	run(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrlogger_loglevel_t level,
				sqlrevent_t event,
				const char *info);
	private:
		void		unload();
		void		loadLogger(xmldomnode *logger);

		const char	*libexecdir;

		singlylinkedlist< sqlrloggerplugin * >	llist;
};


class SQLRSERVER_DLLSPEC sqlrnotification {
	public:
			sqlrnotification(sqlrnotifications *ns,
						xmldomnode *parameters,
						bool debug);
		virtual	~sqlrnotification();

		virtual bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);
	protected:
		sqlrnotifications	*ns;
		xmldomnode		*parameters;
		xmldomnode		*transports;
		xmldomnode		*recipients;
		bool			debug;
};


class SQLRSERVER_DLLSPEC sqlrnotificationplugin {
	public:
		sqlrnotification	*n;
		dynamiclib		*dl;
};

class SQLRSERVER_DLLSPEC sqlrnotifications {
	public:
			sqlrnotifications(sqlrpaths *sqlrpth, bool debug);
			~sqlrnotifications();

		bool	load(xmldomnode *parameters);
		void	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);

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

		xmldomnode	*getTransport(const char *transportid);
	private:
		void		unload();
		void		loadNotification(xmldomnode *notification);

		char		*substitutions(sqlrlistener *sqlrl,
						sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *str,
						const char *event,
						const char *info);

		const char	*libexecdir;
		const char	*tmpdir;
		char		*tmpfilename;
		bool		debug;

		xmldomnode	*transports;

		singlylinkedlist< sqlrnotificationplugin * >	llist;
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
		
	private:
		void	init(bool allow,
				const char *years,
				const char *months,
				const char *daysofmonth,
				const char *daysofweek,
				const char *dayparts);
		void	splitTimePart(
				linkedlist< sqlrscheduleperiod * > *periods,
				const char *timepartlist);
		void	splitDayParts(const char *daypartlist);
		bool	inPeriods(
				linkedlist< sqlrscheduleperiod * > *periods,
				int32_t timepart);
		bool	inDayParts(int32_t hour, int32_t minute);

		bool			allow;
		linkedlist< sqlrscheduleperiod * >	years;
		linkedlist< sqlrscheduleperiod * >	months;
		linkedlist< sqlrscheduleperiod * >	daysofmonth;
		linkedlist< sqlrscheduleperiod * >	daysofweek;
		linkedlist< sqlrscheduledaypart * >	dayparts;
};


class SQLRSERVER_DLLSPEC sqlrschedule {
	public:
			sqlrschedule(xmldomnode *parameters, bool debug);
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
		xmldomnode	*parameters;
		bool		debug;

	private:
		linkedlist< sqlrschedulerule * >	rules;
};


class SQLRSERVER_DLLSPEC sqlrscheduleplugin {
	public:
		sqlrschedule	*s;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrschedules {
	public:
			sqlrschedules(sqlrpaths *sqlrpth, bool debug);
			~sqlrschedules();

		bool	load(xmldomnode *parameters);
		bool	allowed(sqlrserverconnection *sqlrcon,
						const char *user);
	private:
		void		unload();
		void		loadSchedule(xmldomnode *schedule);

		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrscheduleplugin * >	llist;
};


class SQLRSERVER_DLLSPEC sqlrrouter {
	public:
			sqlrrouter(xmldomnode *parameters, bool debug);
		virtual	~sqlrrouter();

		virtual const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
	protected:
		xmldomnode	*parameters;
		bool		debug;
};


class SQLRSERVER_DLLSPEC sqlrrouterplugin {
	public:
		sqlrrouter	*r;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrrouters {
	public:
			sqlrrouters(sqlrpaths *sqlrpth, bool debug);
			~sqlrrouters();

		bool	load(xmldomnode *parameters);

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
	private:
		void		unload();
		void		loadRouter(xmldomnode *route);

		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrrouterplugin * >	llist;
};


class SQLRSERVER_DLLSPEC sqlrparser {
	public:
			sqlrparser(xmldomnode *parameters, bool debug);
		virtual	~sqlrparser();

		virtual	bool	parse(const char *query);
		virtual	void	useTree(xmldom *tree);
		virtual	xmldom	*getTree();
		virtual	xmldom	*detachTree();

		virtual	bool	write(stringbuffer *output);
		virtual	bool	write(xmldomnode *node,
					stringbuffer *output,
					bool omitsiblings);
		virtual	bool	write(xmldomnode *node, 
					stringbuffer *output);

		virtual void	getMetaData(xmldomnode *node);
	protected:
		xmldomnode	*parameters;
		bool		debug;
};


class SQLRSERVER_DLLSPEC sqlrtranslation {
	public:
			sqlrtranslation(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug);
		virtual	~sqlrtranslation();

		virtual bool	usesTree();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery);

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	protected:
		sqlrtranslations	*sqlts;
		xmldomnode		*parameters;
		bool			debug;
};


class SQLRSERVER_DLLSPEC databaseobject {
	public:
		const char	*database;
		const char	*schema;
		const char	*object;
		const char	*dependency;
};

class SQLRSERVER_DLLSPEC sqlrtranslationplugin {
	public:
		sqlrtranslation	*tr;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrtranslations {
	public:
			sqlrtranslations(sqlrpaths *sqlrpth, bool debug);
			~sqlrtranslations();

		bool	load(xmldomnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						sqlrparser *sqlrp,
						const char *query,
						stringbuffer *translatedquery);

		bool	getReplacementTableName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname);
		bool	getReplacementIndexName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname);
		databaseobject *createDatabaseObject(
						memorypool *pool,
						const char *database,
						const char *schema,
						const char *object,
						const char *dependency);
		bool	removeReplacementTable(const char *database,
						const char *schema,
						const char *table);
		bool	removeReplacementIndex(const char *database,
						const char *schema,
						const char *index);

		void	endSession();
	private:
		void	unload();
		void	loadTranslation(xmldomnode *translation);

		bool	getReplacementName(
				dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname,
				const char **newname);
		bool	removeReplacement(
				dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname);
		
		const char	*libexecdir;
		xmldom		*tree;
		bool		debug;

		singlylinkedlist< sqlrtranslationplugin * >	tlist;

	public:
		// helper methods
		xmldomnode	*newNode(xmldomnode *parentnode,
							const char *type);
		xmldomnode	*newNode(xmldomnode *parentnode,
							const char *type,
							const char *value);
		xmldomnode	*newNodeAfter(xmldomnode *parentnode,
							xmldomnode *node,
							const char *type);
		xmldomnode	*newNodeAfter(xmldomnode *parentnode,
							xmldomnode *node,
							const char *type,
							const char *value);
		xmldomnode	*newNodeBefore(xmldomnode *parentnode,
							xmldomnode *node,
							const char *type);
		xmldomnode	*newNodeBefore(xmldomnode *parentnode,
							xmldomnode *node,
							const char *type,
							const char *value);
		void		setAttribute(xmldomnode *node,
							const char *name,
							const char *value);
		bool		isString(const char *value);

		memorypool	*temptablepool;
		memorypool	*tempindexpool;
		dictionary< databaseobject *, char * >	temptablemap;
		dictionary< databaseobject *, char * >	tempindexmap;
};


class SQLRSERVER_DLLSPEC sqlrfilter {
	public:
			sqlrfilter(sqlrfilters *sqlrfs,
					xmldomnode *parameters,
					bool debug);
		virtual	~sqlrfilter();

		virtual bool	usesTree();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);

		virtual void	getError(const char **err, int64_t *errn);
	protected:
		sqlrfilters		*sqlrfs;
		xmldomnode		*parameters;
		bool			debug;
};


class SQLRSERVER_DLLSPEC sqlrfilterplugin {
	public:
		sqlrfilter	*f;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrfilters {
	public:
			sqlrfilters(sqlrpaths *sqlrpth, bool debug);
			~sqlrfilters();

		bool	loadFilters(xmldomnode *parameters);
		bool	runFilters(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						sqlrparser *sqlrp,
						const char *query,
						const char **err,
						int64_t *errn);
	private:
		void	unloadFilters();
		void	loadFilter(xmldomnode *filter);
		
		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrfilterplugin * >	tlist;
};


class SQLRSERVER_DLLSPEC sqlrresultsettranslation {
	public:
			sqlrresultsettranslation(
					sqlrresultsettranslations *sqlrrsts,
					xmldomnode *parameters,
					bool debug);
		virtual	~sqlrresultsettranslation();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *fieldname,
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength);
	protected:
		sqlrresultsettranslations	*sqlrrsts;
		xmldomnode			*parameters;
		bool				debug;
};


class SQLRSERVER_DLLSPEC sqlrresultsettranslationplugin {
	public:
		sqlrresultsettranslation	*rstr;
		dynamiclib			*dl;
};

class SQLRSERVER_DLLSPEC sqlrresultsettranslations {
	public:
			sqlrresultsettranslations(sqlrpaths *sqlrpth,
							bool debug);
			~sqlrresultsettranslations();

		bool	load(xmldomnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *fieldname,
						uint16_t fieldindex,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength);
	private:
		void	unload();
		void	loadResultSetTranslation(
					xmldomnode *resultsettranslation);
		
		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrresultsettranslationplugin * >	tlist;
};


class SQLRSERVER_DLLSPEC sqlrtrigger {
	public:
			sqlrtrigger(xmldomnode *parameters, bool debug);
		virtual	~sqlrtrigger();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success);
	protected:
		xmldomnode	*parameters;
		bool		debug;
};


class SQLRSERVER_DLLSPEC sqlrtriggerplugin {
	public:
		sqlrtrigger	*tr;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrtriggers {
	public:
			sqlrtriggers(sqlrpaths *sqlrpth, bool debug);
			~sqlrtriggers();

		bool	load(xmldomnode *parameters);
		void	runBeforeTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree);
		void	runAfterTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree,
						bool success);
	private:
		void		unload();
		void		loadTrigger(xmldomnode *trigger,
					singlylinkedlist< sqlrtriggerplugin *>
					*list);
		void		run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					singlylinkedlist< sqlrtriggerplugin * >
					*list,
					bool before,
					bool success);

		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrtriggerplugin * >	beforetriggers;
		singlylinkedlist< sqlrtriggerplugin * >	aftertriggers;
};

class SQLRSERVER_DLLSPEC sqlrquery {
	public:
			sqlrquery(xmldomnode *parameters);
		virtual	~sqlrquery();

		virtual bool	match(const char *querystring,
						uint32_t querylength);
		virtual sqlrquerycursor	*newCursor(	
						sqlrserverconnection *sqlrcon,
						uint16_t id);
	protected:
		xmldomnode	*parameters;
};

class SQLRSERVER_DLLSPEC sqlrquerycursor : public sqlrservercursor {
	public:
			sqlrquerycursor(sqlrserverconnection *conn,
					xmldomnode *parameters,
					uint16_t id);
		virtual	~sqlrquerycursor();
		virtual sqlrquerytype_t	queryType(const char *query,
							uint32_t length);
		bool	isCustomQuery();
	protected:
		xmldomnode	*parameters;
};


class SQLRSERVER_DLLSPEC sqlrqueryplugin {
	public:
		sqlrquery	*qr;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrqueries {
	public:
			sqlrqueries(sqlrpaths *sqlrpth);
			~sqlrqueries();

		bool		load(xmldomnode *parameters);
		sqlrquerycursor	*match(sqlrserverconnection *sqlrcon,
						const char *querystring,
						uint32_t querylength,
						uint16_t id);
	private:
		void		unload();
		void		loadQuery(xmldomnode *logger);

		const char	*libexecdir;

		singlylinkedlist< sqlrqueryplugin * >	llist;
};

#endif
