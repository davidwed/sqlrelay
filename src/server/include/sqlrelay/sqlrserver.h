// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#ifndef SQLRSERVER_H
#define SQLRSERVER_H

#include <sqlrelay/private/sqlrserverincludes.h>

class sqlrserverconnection;
class sqlrservercursor;
class sqlrprotocol;
class sqlrprotocols;
class sqlrauth;
class sqlrauths;
class sqlrpwdenc;
class sqlrpwdencs;
class sqlrlogger;
class sqlrloggers;
class sqlrparser;
class sqlrtranslation;
class sqlrtranslations;
class sqlrresultsettranslation;
class sqlrresultsettranslations;
class sqlrtrigger;
class sqlrtriggers;
class sqlrquery;
class sqlrquerycursor;
class sqlrqueries;

class SQLRSERVER_DLLSPEC sqlrlistener : public listener {
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
		uint32_t	valuesize;
		uint32_t	resultvaluesize;
		uint16_t	type;
		int16_t		isnull;
};

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


class SQLRSERVER_DLLSPEC sqlrserverconnection {
	public:
			sqlrserverconnection(sqlrservercontroller *cont);
		virtual	~sqlrserverconnection();

		virtual bool	mustDetachBeforeLogIn();

		virtual bool	supportsAuthOnDatabase();
		virtual	void	handleConnectString()=0;

		virtual	bool	logIn(const char **error)=0;
		virtual	void	logOut()=0;

		virtual	bool	changeUser(const char *newuser,
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
		virtual bool		tempTableDropReLogIn();

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
		virtual	bool	executeQuery(const char *query,
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
		void	abort();

		char		*getQueryBuffer();
		uint32_t 	getQueryLength();
		void		setQueryLength(uint32_t querylength);

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

		sqlrserverconnection	*conn;

	#include <sqlrelay/private/sqlrservercursor.h>
};


enum sqlrclientexitstatus_t {
	SQLRCLIENTEXITSTATUS_ERROR=0,
	SQLRCLIENTEXITSTATUS_CLOSED_CONNECTION,
	SQLRCLIENTEXITSTATUS_ENDED_SESSION,
	SQLRCLIENTEXITSTATUS_SUSPENDED_SESSION
};

class SQLRSERVER_DLLSPEC sqlrprotocol {
	public:
			sqlrprotocol(sqlrservercontroller *cont);
		virtual	~sqlrprotocol();

		void	setClientSocket(filedescriptor *clientsock);

		virtual sqlrclientexitstatus_t	clientSession()=0;

	protected:
		sqlrservercontroller	*cont;
		filedescriptor		*clientsock;
};


class SQLRSERVER_DLLSPEC sqlrprotocolplugin {
	public:
		sqlrprotocol	*pr;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrprotocols {
	public:
			sqlrprotocols(sqlrservercontroller *cont);
			~sqlrprotocols();

		bool		loadProtocols();
		sqlrprotocol	*getProtocol(const char *module);
	private:
		void	unloadProtocols();
		void	loadProtocol(const char *module);

		dictionary< const char *, sqlrprotocolplugin * >	protos;

		sqlrservercontroller	*cont;
};


class SQLRSERVER_DLLSPEC sqlrauth {
	public:
			sqlrauth(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe);
		virtual	~sqlrauth();
		virtual	bool	authenticate(const char *user,
						const char *password);
	protected:
		xmldomnode		*parameters;
		sqlrpwdencs		*sqlrpe;
};


class SQLRSERVER_DLLSPEC sqlrauthplugin {
	public:
		sqlrauth	*au;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrauths {
	public:
			sqlrauths();
			~sqlrauths();

		bool	loadAuthenticators(const char *auths,
						sqlrpwdencs *sqlrpe);
		bool	authenticate(const char *user, const char *password);
	private:
		void	unloadAuthenticators();
		void	loadAuthenticator(xmldomnode *auth,
						sqlrpwdencs *sqlrpe);

		xmldom					*xmld;
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
			sqlrpwdencs();
			~sqlrpwdencs();

		bool		loadPasswordEncryptions(const char *pwdencs);
		sqlrpwdenc	*getPasswordEncryptionById(const char *id);
	private:
		void	unloadPasswordEncryptions();
		void	loadPasswordEncryption(xmldomnode *pwdenc);

		xmldom					*xmld;
		singlylinkedlist< sqlrpwdencplugin * >	llist;
};


enum sqlrlogger_loglevel_t {
	SQLRLOGGER_LOGLEVEL_DEBUG=0,
	SQLRLOGGER_LOGLEVEL_INFO,
	SQLRLOGGER_LOGLEVEL_WARNING,
	SQLRLOGGER_LOGLEVEL_ERROR
};

enum sqlrlogger_eventtype_t {
	SQLRLOGGER_EVENTTYPE_CLIENT_CONNECTED=0,
	SQLRLOGGER_EVENTTYPE_CLIENT_CONNECTION_REFUSED,
	SQLRLOGGER_EVENTTYPE_CLIENT_DISCONNECTED,
	SQLRLOGGER_EVENTTYPE_CLIENT_PROTOCOL_ERROR,
	SQLRLOGGER_EVENTTYPE_DB_LOGIN,
	SQLRLOGGER_EVENTTYPE_DB_LOGOUT,
	SQLRLOGGER_EVENTTYPE_DB_ERROR,
	SQLRLOGGER_EVENTTYPE_QUERY,
	SQLRLOGGER_EVENTTYPE_INTERNAL_ERROR,
	SQLRLOGGER_EVENTTYPE_DEBUG_MESSAGE
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
					sqlrlogger_eventtype_t event,
					const char *info);
	protected:
		const char	*logLevel(sqlrlogger_loglevel_t level);
		const char	*eventType(sqlrlogger_eventtype_t event);
		xmldomnode	*parameters;
};


class SQLRSERVER_DLLSPEC sqlrloggerplugin {
	public:
		sqlrlogger	*lg;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrloggers {
	public:
			sqlrloggers();
			~sqlrloggers();

		bool	loadLoggers(const char *loggers);
		void	initLoggers(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon);
		void	runLoggers(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrlogger_eventtype_t event,
					const char *info);
	private:
		void		unloadLoggers();
		void		loadLogger(xmldomnode *logger);

		xmldom					*xmld;
		singlylinkedlist< sqlrloggerplugin * >	llist;
};


class SQLRSERVER_DLLSPEC sqlrparser {
	public:
			sqlrparser();
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
			sqlrtranslations(bool debug);
			~sqlrtranslations();

		bool	loadTranslations(const char *translations);
		bool	runTranslations(sqlrserverconnection *sqlrcon,
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
		void	unloadTranslations();
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
		
		xmldom	*xmld;
		xmldom	*tree;
		bool	debug;

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


class SQLRSERVER_DLLSPEC sqlrresultsettranslation {
	public:
			sqlrresultsettranslation(
					sqlrresultsettranslations *sqlrrsts,
					xmldomnode *parameters);
		virtual	~sqlrresultsettranslation();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint16_t fieldindex,
					const char *field,
					uint32_t fieldlength,
					const char **newfield,
					uint32_t *newfieldlength);
	protected:
		sqlrresultsettranslations	*sqlrrsts;
		xmldomnode			*parameters;
};


class SQLRSERVER_DLLSPEC sqlrresultsettranslationplugin {
	public:
		sqlrresultsettranslation	*rstr;
		dynamiclib			*dl;
};

class SQLRSERVER_DLLSPEC sqlrresultsettranslations {
	public:
			sqlrresultsettranslations();
			~sqlrresultsettranslations();

		bool	loadResultSetTranslations(
					const char *resultsettranslations);
		bool	runResultSetTranslations(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint16_t fieldindex,
						const char *field,
						uint32_t fieldlength,
						const char **newfield,
						uint32_t *newfieldlength);
	private:
		void	unloadResultSetTranslations();
		void	loadResultSetTranslation(
					xmldomnode *resultsettranslation);
		
		xmldom	*xmld;

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
			sqlrtriggers(bool debug);
			~sqlrtriggers();

		bool	loadTriggers(const char *triggers);
		void	runBeforeTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree);
		void	runAfterTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree,
						bool success);
	private:
		void		unloadTriggers();
		void		loadTrigger(xmldomnode *trigger,
					singlylinkedlist< sqlrtriggerplugin *>
					*list);
		void		runTriggers(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					singlylinkedlist< sqlrtriggerplugin * >
					*list,
					bool before,
					bool success);

		xmldom					*xmld;
		bool					debug;
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
			sqlrqueries();
			~sqlrqueries();

		bool		loadQueries(const char *queries);
		sqlrquerycursor	*match(sqlrserverconnection *sqlrcon,
						const char *querystring,
						uint32_t querylength,
						uint16_t id);
	private:
		void		unloadQueries();
		void		loadQuery(xmldomnode *logger);

		xmldom					*xmld;
		singlylinkedlist< sqlrqueryplugin * >	llist;
};

#endif
