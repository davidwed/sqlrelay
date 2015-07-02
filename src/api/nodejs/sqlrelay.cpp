#include <sqlrelay/sqlrclient.h>
#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

using namespace v8;
using namespace node;



// macros to deal with differences between major versions of node.js
#if NODE_MINOR_VERSION >= 12
	#define RET void
	#define ARGS FunctionCallbackInfo<Value>
	#define newFunctionTemplate(isolate,func) FunctionTemplate::New(isolate,func)
	#define newLocalFunction(isolate,func) Local<Function>::New(isolate,func)
	#define resetConstructor(constructor,isolate,tpl) constructor.Reset(isolate,tpl->GetFunction())
	#define returnObject(object) args.GetReturnValue().Set(object)
	#define returnBoolean(isolate,result) args.GetReturnValue().Set(Boolean::New(isolate,result))
	#define returnString(isolate,result) if (result) { args.GetReturnValue().Set(String::NewFromUtf8(isolate,result)); } else { args.GetReturnValue().Set(Null(isolate)); }
	#define returnInteger(isolate,result) args.GetReturnValue().Set(Integer::New(isolate,result))
	#define returnNumber(isolate,result) args.GetReturnValue().Set(Number::New(isolate,result))
	#define returnArray(isolate,result) /* FIXME: implement this */
	#define returnVoid(isolate)
	#define checkArgCount(args,isolate,count) if (args.Length()!=count) { throwWrongNumberOfArguments(isolate); return; }
#else
	#define RET Handle<Value>
	#define ARGS Arguments
	#define newFunctionTemplate(isolate,func) FunctionTemplate::New(func)
	#define newLocalFunction(isolate,func) Local<Function>::New(func)
	#define resetConstructor(constructor,isolate,tpl)
	#define returnObject(object) return scope.Close(object)
	#define returnBoolean(isolate,result) return scope.Close(Boolean::New(result))
	#define returnString(isolate,result) if (result) { return scope.Close(String::NewFromUtf8(result)); } else { return scope.Close(Null()); }
	#define returnInteger(isolate,result) return scope.Close(Integer::New(result))
	#define returnNumber(isolate,result) return scope.Close(Number::New(result))
	#define returnArray(isolate,result) /* FIXME: implement this */ return scope.Close()
	#define returnVoid(isolate) return scope.Close()
	#define scope(isolate) scope
	#define checkArgCount(args,isolate,count) if (args.Length()!=count) { throwWrongNumberOfArguments(isolate); returnBoolean(isolate,false); }
	#define NewFromUtf8(isolate,str) New(str)
#endif


// convenience macros
#define throwWrongNumberOfArguments(isolate) isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,"Wrong number of arguments")))
#define throwInvalidArgumentType(isolate) isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,"Invalid argument type")))
#define argToString(arg) *(String::Utf8Value(arg))



// SQLRConnection declarations...
class SQLRConnection : public ObjectWrap {
	friend class SQLRCursor;
	public:
		static void	Init(Handle<Object> exports);
	private:
		explicit	SQLRConnection();
				~SQLRConnection();
		static RET	New(const ARGS &args);

		static RET	setConnectTimeout(const ARGS &args);
		static RET	setAuthenticationTimeout(const ARGS &args);
		static RET	setResponseTimeout(const ARGS &args);
		static RET	endSession(const ARGS &args);
		static RET	suspendSession(const ARGS &args);
		static RET	getConnectionPort(const ARGS &args);
		static RET	getConnectionSocket(const ARGS &args);
		static RET	resumeSession(const ARGS &args);
		static RET	ping(const ARGS &args);
		static RET	identify(const ARGS &args);
		static RET	dbVersion(const ARGS &args);
		static RET	dbHostName(const ARGS &args);
		static RET	dbIpAddress(const ARGS &args);
		static RET	serverVersion(const ARGS &args);
		static RET	clientVersion(const ARGS &args);
		static RET	bindFormat(const ARGS &args);
		static RET	selectDatabase(const ARGS &args);
		static RET	getCurrentDatabase(const ARGS &args);
		static RET	getLastInsertId(const ARGS &args);
		static RET	autoCommitOn(const ARGS &args);
		static RET	autoCommitOff(const ARGS &args);
		static RET	begin(const ARGS &args);
		static RET	commit(const ARGS &args);
		static RET	rollback(const ARGS &args);
		static RET	errorMessage(const ARGS &args);
		static RET	errorNumber(const ARGS &args);
		static RET	debugOn(const ARGS &args);
		static RET	debugOff(const ARGS &args);
		static RET	getDebug(const ARGS &args);
		static RET	setDebugFile(const ARGS &args);
		static RET	setClientInfo(const ARGS &args);
		static RET	getClientInfo(const ARGS &args);

		static Persistent<Function>	constructor;

		static sqlrconnection	*sqlrcon(const ARGS &args);
		sqlrconnection		*sqlrc;
};

Persistent<Function> SQLRConnection::constructor;



// SQLRCursor declarations...
class SQLRCursor : public ObjectWrap {
	public:
		static void	Init(Handle<Object> exports);
	private:
		explicit	SQLRCursor();
				~SQLRCursor();
		static RET	New(const ARGS &args);

		static RET	setResultSetBufferSize(const ARGS &args);
		static RET	getResultSetBufferSize(const ARGS &args);
		static RET	dontGetColumnInfo(const ARGS &args);
		static RET	getColumnInfo(const ARGS &args);
		static RET	mixedCaseColumnNames(const ARGS &args);
		static RET	upperCaseColumnNames(const ARGS &args);
		static RET	lowerCaseColumnNames(const ARGS &args);
		static RET	cacheToFile(const ARGS &args);
		static RET	setCacheTtl(const ARGS &args);
		static RET	getCacheFileName(const ARGS &args);
		static RET	cacheOff(const ARGS &args);
		static RET	getDatabaseList(const ARGS &args);
		static RET	getTableList(const ARGS &args);
		static RET	getColumnList(const ARGS &args);
		static RET	sendQuery(const ARGS &args);
		static RET	sendFileQuery(const ARGS &args);
		static RET	prepareQuery(const ARGS &args);
		static RET	prepareFileQuery(const ARGS &args);
		static RET	substitution(const ARGS &args);
		static RET	substitutions(const ARGS &args);
		static RET	inputBind(const ARGS &args);
		static RET	inputBindBlob(const ARGS &args);
		static RET	inputBindClob(const ARGS &args);
		static RET	inputBinds(const ARGS &args);
		static RET	defineOutputBindString(const ARGS &args);
		static RET	defineOutputBindInteger(const ARGS &args);
		static RET	defineOutputBindDouble(const ARGS &args);
		static RET	defineOutputBindBlob(const ARGS &args);
		static RET	defineOutputBindClob(const ARGS &args);
		static RET	defineOutputBindCursor(const ARGS &args);
		static RET	clearBinds(const ARGS &args);
		static RET	countBindVariables(const ARGS &args);
		static RET	validateBinds(const ARGS &args);
		static RET	validBind(const ARGS &args);
		static RET	executeQuery(const ARGS &args);
		static RET	fetchFromBindCursor(const ARGS &args);
		static RET	getOutputBindString(const ARGS &args);
		static RET	getOutputBindInteger(const ARGS &args);
		static RET	getOutputBindDouble(const ARGS &args);
		static RET	getOutputBindBlob(const ARGS &args);
		static RET	getOutputBindClob(const ARGS &args);
		static RET	getOutputBindLength(const ARGS &args);
		static RET	getOutputBindCursor(const ARGS &args);
		static RET	openCachedResultSet(const ARGS &args);
		static RET	colCount(const ARGS &args);
		static RET	rowCount(const ARGS &args);
		static RET	totalRows(const ARGS &args);
		static RET	affectedRows(const ARGS &args);
		static RET	firstRowIndex(const ARGS &args);
		static RET	endOfResultSet(const ARGS &args);
		static RET	errorMessage(const ARGS &args);
		static RET	errorNumber(const ARGS &args);
		static RET	getNullsAsEmptyStrings(const ARGS &args);
		static RET	getNullsAsNulls(const ARGS &args);
		static RET	getField(const ARGS &args);
		static RET	getFieldAsInteger(const ARGS &args);
		static RET	getFieldAsDouble(const ARGS &args);
		static RET	getFieldLength(const ARGS &args);
		static RET	getRow(const ARGS &args);
		static RET	getRowLengths(const ARGS &args);
		static RET	getColumnNames(const ARGS &args);
		static RET	getColumnName(const ARGS &args);
		static RET	getColumnType(const ARGS &args);
		static RET	getColumnLength(const ARGS &args);
		static RET	getColumnPrecision(const ARGS &args);
		static RET	getColumnScale(const ARGS &args);
		static RET	getColumnIsNullable(const ARGS &args);
		static RET	getColumnIsPrimaryKey(const ARGS &args);
		static RET	getColumnIsUnique(const ARGS &args);
		static RET	getColumnIsPartOfKey(const ARGS &args);
		static RET	getColumnIsUnsigned(const ARGS &args);
		static RET	getColumnIsZeroFilled(const ARGS &args);
		static RET	getColumnIsBinary(const ARGS &args);
		static RET	getColumnIsAutoIncrement(const ARGS &args);
		static RET	getLongest(const ARGS &args);
		static RET	suspendResultSet(const ARGS &args);
		static RET	getResultSetId(const ARGS &args);
		static RET	resumeResultSet(const ARGS &args);
		static RET	resumeCachedResultSet(const ARGS &args);
		static RET	closeResultSet(const ARGS &args);

		static Persistent<Function>	constructor;

		static sqlrcursor	*sqlrcur(const ARGS &args);
		sqlrcursor		*sqlrc;
};

Persistent<Function> SQLRCursor::constructor;



// SQLRConnection methods...
void SQLRConnection::Init(Handle<Object> exports) {

	Isolate	*isolate=Isolate::GetCurrent();

	Local<FunctionTemplate>	tpl=newFunctionTemplate(isolate,New);
	tpl->SetClassName(String::NewFromUtf8(isolate,"SQLRConnection"));
	// internal field count is the number of non-static member variables
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	NODE_SET_PROTOTYPE_METHOD(tpl,"setConnectTimeout",setConnectTimeout);
	NODE_SET_PROTOTYPE_METHOD(tpl,"setAuthenticationTimeout",
						setAuthenticationTimeout);
	NODE_SET_PROTOTYPE_METHOD(tpl,"setResponseTimeout",setResponseTimeout);
	NODE_SET_PROTOTYPE_METHOD(tpl,"endSession",endSession);
	NODE_SET_PROTOTYPE_METHOD(tpl,"suspendSession",suspendSession);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getConnectionPort",getConnectionPort);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getConnectionSocket",
						getConnectionSocket);
	NODE_SET_PROTOTYPE_METHOD(tpl,"resumeSession",resumeSession);
	NODE_SET_PROTOTYPE_METHOD(tpl,"ping",ping);
	NODE_SET_PROTOTYPE_METHOD(tpl,"identify",identify);
	NODE_SET_PROTOTYPE_METHOD(tpl,"dbVersion",dbVersion);
	NODE_SET_PROTOTYPE_METHOD(tpl,"dbHostName",dbHostName);
	NODE_SET_PROTOTYPE_METHOD(tpl,"dbIpAddress",dbIpAddress);
	NODE_SET_PROTOTYPE_METHOD(tpl,"serverVersion",serverVersion);
	NODE_SET_PROTOTYPE_METHOD(tpl,"clientVersion",clientVersion);
	NODE_SET_PROTOTYPE_METHOD(tpl,"bindFormat",bindFormat);
	NODE_SET_PROTOTYPE_METHOD(tpl,"selectDatabase",selectDatabase);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getCurrentDatabase",getCurrentDatabase);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getLastInsertId",getLastInsertId);
	NODE_SET_PROTOTYPE_METHOD(tpl,"autoCommitOn",autoCommitOn);
	NODE_SET_PROTOTYPE_METHOD(tpl,"autoCommitOff",autoCommitOff);
	NODE_SET_PROTOTYPE_METHOD(tpl,"begin",begin);
	NODE_SET_PROTOTYPE_METHOD(tpl,"commit",commit);
	NODE_SET_PROTOTYPE_METHOD(tpl,"rollback",rollback);
	NODE_SET_PROTOTYPE_METHOD(tpl,"errorMessage",errorMessage);
	NODE_SET_PROTOTYPE_METHOD(tpl,"errorNumber",errorNumber);
	NODE_SET_PROTOTYPE_METHOD(tpl,"debugOn",debugOn);
	NODE_SET_PROTOTYPE_METHOD(tpl,"debugOff",debugOff);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getDebug",getDebug);
	NODE_SET_PROTOTYPE_METHOD(tpl,"setDebugFile",setDebugFile);
	NODE_SET_PROTOTYPE_METHOD(tpl,"setClientInfo",setClientInfo);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getClientInfo",getClientInfo);

	resetConstructor(constructor,isolate,tpl);
	exports->Set(String::NewFromUtf8(isolate,"SQLRConnection"),
						tpl->GetFunction());
}

SQLRConnection::SQLRConnection() {
}

SQLRConnection::~SQLRConnection() {
}

RET SQLRConnection::New(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	if (args.IsConstructCall()) {

		checkArgCount(args,isolate,7);

		// invoked as constructor: new SQLRConnection(...)
		SQLRConnection	*obj=new SQLRConnection();
		obj->sqlrc=new sqlrconnection(argToString(args[0]),
						args[1]->Uint32Value(),
						argToString(args[2]),
						argToString(args[3]),
						argToString(args[4]),
						args[5]->Int32Value(),
						args[6]->Int32Value(),
						true);
		obj->Wrap(args.This());
		returnObject(args.This());
	} else {
		// invoked as function: SQLRConnection(...)
		const int	argc=1;
		Local<Value>	argv[argc]={args[0]};
		Local<Function>	cons=newLocalFunction(isolate,constructor);
		returnObject(cons->NewInstance(argc,argv));
	}
}

RET SQLRConnection::setConnectTimeout(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	sqlrcon(args)->setConnectTimeout(args[0]->Int32Value(),
						args[1]->Int32Value());

	returnVoid(isolate);
}

RET SQLRConnection::setAuthenticationTimeout(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	sqlrcon(args)->setAuthenticationTimeout(args[0]->Int32Value(),
						args[1]->Int32Value());

	returnVoid(isolate);
}

RET SQLRConnection::setResponseTimeout(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	sqlrcon(args)->setResponseTimeout(args[0]->Int32Value(),
						args[1]->Int32Value());

	returnVoid(isolate);
}

RET SQLRConnection::endSession(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcon(args)->endSession();

	returnVoid(isolate);
}

RET SQLRConnection::suspendSession(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcon(args)->suspendSession();

	returnBoolean(isolate,result);
}

RET SQLRConnection::getConnectionPort(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint16_t	result=sqlrcon(args)->getConnectionPort();

	returnInteger(isolate,result);
}

RET SQLRConnection::getConnectionSocket(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->getConnectionSocket();

	returnString(isolate,result);
}

RET SQLRConnection::resumeSession(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	bool	result=sqlrcon(args)->resumeSession(args[0]->Int32Value(),
							argToString(args[1]));

	returnBoolean(isolate,result);
}

RET SQLRConnection::ping(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcon(args)->ping();

	returnBoolean(isolate,result);
}

RET SQLRConnection::identify(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->identify();

	returnString(isolate,result);
}

RET SQLRConnection::dbVersion(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->dbVersion();

	returnString(isolate,result);
}

RET SQLRConnection::dbHostName(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->dbHostName();

	returnString(isolate,result);
}

RET SQLRConnection::dbIpAddress(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->dbIpAddress();

	returnString(isolate,result);
}

RET SQLRConnection::serverVersion(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->serverVersion();

	returnString(isolate,result);
}

RET SQLRConnection::clientVersion(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->clientVersion();

	returnString(isolate,result);
}

RET SQLRConnection::bindFormat(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->bindFormat();

	returnString(isolate,result);
}

RET SQLRConnection::selectDatabase(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=sqlrcon(args)->selectDatabase(argToString(args[0]));

	returnBoolean(isolate,result);
}

RET SQLRConnection::getCurrentDatabase(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->getCurrentDatabase();

	returnString(isolate,result);
}

RET SQLRConnection::getLastInsertId(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint64_t	result=sqlrcon(args)->getLastInsertId();

	returnInteger(isolate,result);
}

RET SQLRConnection::autoCommitOn(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcon(args)->autoCommitOn();

	returnBoolean(isolate,result);
}

RET SQLRConnection::autoCommitOff(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcon(args)->autoCommitOff();

	returnBoolean(isolate,result);
}

RET SQLRConnection::begin(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcon(args)->begin();

	returnBoolean(isolate,result);
}

RET SQLRConnection::commit(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcon(args)->commit();

	returnBoolean(isolate,result);
}

RET SQLRConnection::rollback(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcon(args)->rollback();

	returnBoolean(isolate,result);
}

RET SQLRConnection::errorMessage(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->errorMessage();

	returnString(isolate,result);
}

RET SQLRConnection::errorNumber(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	int64_t		result=sqlrcon(args)->errorNumber();

	returnInteger(isolate,result);
}

RET SQLRConnection::debugOn(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcon(args)->debugOn();

	returnVoid(isolate);
}

RET SQLRConnection::debugOff(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcon(args)->debugOff();

	returnVoid(isolate);
}

RET SQLRConnection::getDebug(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcon(args)->getDebug();

	returnBoolean(isolate,result);
}

RET SQLRConnection::setDebugFile(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcon(args)->setDebugFile(argToString(args[0]));

	returnVoid(isolate);
}

RET SQLRConnection::setClientInfo(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcon(args)->setClientInfo(argToString(args[0]));

	returnVoid(isolate);
}

RET SQLRConnection::getClientInfo(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcon(args)->getClientInfo();

	returnString(isolate,result);
}

sqlrconnection *SQLRConnection::sqlrcon(const ARGS &args) {
	return ObjectWrap::Unwrap<SQLRConnection>(args.Holder())->sqlrc;
}



// SQLRCursor methods...
void SQLRCursor::Init(Handle<Object> exports) {

	Isolate	*isolate=Isolate::GetCurrent();

	Local<FunctionTemplate>	tpl=newFunctionTemplate(isolate,New);
	tpl->SetClassName(String::NewFromUtf8(isolate,"SQLRCursor"));
	// internal field count is the number of non-static member variables
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	NODE_SET_PROTOTYPE_METHOD(tpl,"setResultSetBufferSize",
						setResultSetBufferSize);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getResultSetBufferSize",
						getResultSetBufferSize);
	NODE_SET_PROTOTYPE_METHOD(tpl,"dontGetColumnInfo",dontGetColumnInfo);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnInfo",getColumnInfo);
	NODE_SET_PROTOTYPE_METHOD(tpl,"mixedCaseColumnNames",
						mixedCaseColumnNames);
	NODE_SET_PROTOTYPE_METHOD(tpl,"upperCaseColumnNames",
						upperCaseColumnNames);
	NODE_SET_PROTOTYPE_METHOD(tpl,"lowerCaseColumnNames",
						lowerCaseColumnNames);
	NODE_SET_PROTOTYPE_METHOD(tpl,"cacheToFile",cacheToFile);
	NODE_SET_PROTOTYPE_METHOD(tpl,"setCacheTtl",setCacheTtl);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getCacheFileName",getCacheFileName);
	NODE_SET_PROTOTYPE_METHOD(tpl,"cacheOff",cacheOff);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getDatabaseList",getDatabaseList);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getTableList",getTableList);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnList",getColumnList);
	NODE_SET_PROTOTYPE_METHOD(tpl,"sendQuery",sendQuery);
	NODE_SET_PROTOTYPE_METHOD(tpl,"sendFileQuery",sendFileQuery);
	NODE_SET_PROTOTYPE_METHOD(tpl,"prepareQuery",prepareQuery);
	NODE_SET_PROTOTYPE_METHOD(tpl,"prepareFileQuery",prepareFileQuery);
	NODE_SET_PROTOTYPE_METHOD(tpl,"substitution",substitution);
	NODE_SET_PROTOTYPE_METHOD(tpl,"substitutions",substitutions);
	NODE_SET_PROTOTYPE_METHOD(tpl,"inputBind",inputBind);
	NODE_SET_PROTOTYPE_METHOD(tpl,"inputBindBlob",inputBindBlob);
	NODE_SET_PROTOTYPE_METHOD(tpl,"inputBindClob",inputBindClob);
	NODE_SET_PROTOTYPE_METHOD(tpl,"inputBinds",inputBinds);
	NODE_SET_PROTOTYPE_METHOD(tpl,"defineOutputBindString",
						defineOutputBindString);
	NODE_SET_PROTOTYPE_METHOD(tpl,"defineOutputBindInteger",
						defineOutputBindInteger);
	NODE_SET_PROTOTYPE_METHOD(tpl,"defineOutputBindDouble",
						defineOutputBindDouble);
	NODE_SET_PROTOTYPE_METHOD(tpl,"defineOutputBindBlob",
						defineOutputBindBlob);
	NODE_SET_PROTOTYPE_METHOD(tpl,"defineOutputBindClob",
						defineOutputBindClob);
	NODE_SET_PROTOTYPE_METHOD(tpl,"defineOutputBindCursor",
						defineOutputBindCursor);
	NODE_SET_PROTOTYPE_METHOD(tpl,"clearBinds",clearBinds);
	NODE_SET_PROTOTYPE_METHOD(tpl,"countBindVariables",countBindVariables);
	NODE_SET_PROTOTYPE_METHOD(tpl,"validateBinds",validateBinds);
	NODE_SET_PROTOTYPE_METHOD(tpl,"validBind",validBind);
	NODE_SET_PROTOTYPE_METHOD(tpl,"executeQuery",executeQuery);
	NODE_SET_PROTOTYPE_METHOD(tpl,"fetchFromBindCursor",
						fetchFromBindCursor);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getOutputBindString",
						getOutputBindString);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getOutputBindInteger",
						getOutputBindInteger);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getOutputBindDouble",
						getOutputBindDouble);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getOutputBindClob",getOutputBindClob);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getOutputBindLength",
						getOutputBindLength);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getOutputBindCursor",
						getOutputBindCursor);
	NODE_SET_PROTOTYPE_METHOD(tpl,"openCachedResultSet",
						openCachedResultSet);
	NODE_SET_PROTOTYPE_METHOD(tpl,"colCount",colCount);
	NODE_SET_PROTOTYPE_METHOD(tpl,"rowCount",rowCount);
	NODE_SET_PROTOTYPE_METHOD(tpl,"totalRows",totalRows);
	NODE_SET_PROTOTYPE_METHOD(tpl,"affectedRows",affectedRows);
	NODE_SET_PROTOTYPE_METHOD(tpl,"firstRowIndex",firstRowIndex);
	NODE_SET_PROTOTYPE_METHOD(tpl,"endOfResultSet",endOfResultSet);
	NODE_SET_PROTOTYPE_METHOD(tpl,"errorMessage",errorMessage);
	NODE_SET_PROTOTYPE_METHOD(tpl,"errorNumber",errorNumber);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getNullsAsEmptyStrings",
						getNullsAsEmptyStrings);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getNullsAsNulls",getNullsAsNulls);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getField",getField);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getFieldAsInteger",getFieldAsInteger);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getFieldAsDouble",getFieldAsDouble);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getFieldLength",getFieldLength);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getRow",getRow);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getRowLengths",getRowLengths);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnNames",getColumnNames);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnName",getColumnName);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnType",getColumnType);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnLength",getColumnLength);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnPrecision",getColumnPrecision);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnScale",getColumnScale);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnIsNullable",
						getColumnIsNullable);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnIsPrimaryKey",
						getColumnIsPrimaryKey);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnIsUnique",
						getColumnIsUnique);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnIsPartOfKey",
						getColumnIsPartOfKey);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnIsUnsigned",
						getColumnIsUnsigned);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnIsZeroFilled",
						getColumnIsZeroFilled);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnIsBinary",
						getColumnIsBinary);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getColumnIsAutoIncrement",
						getColumnIsAutoIncrement);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getLongest",getLongest);
	NODE_SET_PROTOTYPE_METHOD(tpl,"suspendResultSet",suspendResultSet);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getResultSetId",getResultSetId);
	NODE_SET_PROTOTYPE_METHOD(tpl,"resumeResultSet",resumeResultSet);
	NODE_SET_PROTOTYPE_METHOD(tpl,"resumeCachedResultSet",
						resumeCachedResultSet);
	NODE_SET_PROTOTYPE_METHOD(tpl,"closeResultSet",closeResultSet);

	resetConstructor(constructor,isolate,tpl);
	exports->Set(String::NewFromUtf8(isolate,"SQLRCursor"),
						tpl->GetFunction());
}

SQLRCursor::SQLRCursor() {
}

SQLRCursor::~SQLRCursor() {
}

RET SQLRCursor::New(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	if (args.IsConstructCall()) {

		checkArgCount(args,isolate,1);

		// invoked as constructor: new SQLRCursor(...)
		SQLRConnection	*sqlrcon=
			node::ObjectWrap::Unwrap<SQLRConnection>(
							args[0]->ToObject());
		SQLRCursor	*obj=new SQLRCursor();
		obj->sqlrc=new sqlrcursor(sqlrcon->sqlrc,true);
		obj->Wrap(args.This());
		returnObject(args.This());
	} else {
		// invoked as function: SQLRCursor(...)
		const int	argc=1;
		Local<Value>	argv[argc]={args[0]};
		Local<Function>	cons=newLocalFunction(isolate,constructor);
		returnObject(cons->NewInstance(argc,argv));
	}
}

RET SQLRCursor::setResultSetBufferSize(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcur(args)->setResultSetBufferSize(args[0]->Uint32Value());

	returnVoid(isolate);
}

RET SQLRCursor::getResultSetBufferSize(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint64_t	result=sqlrcur(args)->getResultSetBufferSize();

	returnInteger(isolate,result);
}

RET SQLRCursor::dontGetColumnInfo(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->dontGetColumnInfo();

	returnVoid(isolate);
}

RET SQLRCursor::getColumnInfo(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->getColumnInfo();

	returnVoid(isolate);
}

RET SQLRCursor::mixedCaseColumnNames(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->mixedCaseColumnNames();

	returnVoid(isolate);
}

RET SQLRCursor::upperCaseColumnNames(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->upperCaseColumnNames();

	returnVoid(isolate);
}

RET SQLRCursor::lowerCaseColumnNames(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->lowerCaseColumnNames();

	returnVoid(isolate);
}

RET SQLRCursor::cacheToFile(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcur(args)->cacheToFile(argToString(args[0]));

	returnVoid(isolate);
}

RET SQLRCursor::setCacheTtl(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcur(args)->setCacheTtl(args[0]->Uint32Value());

	returnVoid(isolate);
}

RET SQLRCursor::getCacheFileName(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcur(args)->getCacheFileName();

	returnString(isolate,result);
}

RET SQLRCursor::cacheOff(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->cacheOff();

	returnVoid(isolate);
}

RET SQLRCursor::getDatabaseList(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=sqlrcur(args)->getDatabaseList(argToString(args[0]));

	returnBoolean(isolate,result);
}

RET SQLRCursor::getTableList(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=sqlrcur(args)->getTableList(argToString(args[0]));

	returnBoolean(isolate,result);
}

RET SQLRCursor::getColumnList(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	bool	result=sqlrcur(args)->getColumnList(argToString(args[0]),
							argToString(args[1]));

	returnBoolean(isolate,result);
}

RET SQLRCursor::sendQuery(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	bool	result=false;

	if (args.Length()==1) {
		result=sqlrcur(args)->sendQuery(argToString(args[0]));
	} else if (args.Length()==2) {
		result=sqlrcur(args)->sendQuery(argToString(args[0]),
						args[1]->Uint32Value());
	} else {
		throwWrongNumberOfArguments(isolate);
	}

	returnBoolean(isolate,result);
}

RET SQLRCursor::sendFileQuery(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	bool	result=sqlrcur(args)->sendFileQuery(argToString(args[0]),
							argToString(args[1]));

	returnBoolean(isolate,result);
}

RET SQLRCursor::prepareQuery(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	if (args.Length()==1) {
		sqlrcur(args)->prepareQuery(argToString(args[0]));
	} else if (args.Length()==2) {
		sqlrcur(args)->prepareQuery(argToString(args[0]),
						args[1]->Uint32Value());
	} else {
		throwWrongNumberOfArguments(isolate);
	}

	returnVoid(isolate);
}

RET SQLRCursor::prepareFileQuery(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	bool	result=sqlrcur(args)->prepareFileQuery(argToString(args[0]),
							argToString(args[1]));

	returnBoolean(isolate,result);
}

RET SQLRCursor::substitution(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	if (args.Length()==2) {
		if (args[1]->IsString()) {
			sqlrcur(args)->substitution(argToString(args[0]),
							argToString(args[1]));
		} else if (args[1]->IsInt32() ||
				args[1]->IsUint32() ||
				args[1]->IsNumber()) {
			sqlrcur(args)->substitution(argToString(args[0]),
							args[1]->Int32Value());
		} else {
			throwInvalidArgumentType(isolate);
		}
	} else if (args.Length()==4) {
		sqlrcur(args)->substitution(argToString(args[0]),
						args[1]->NumberValue(),
						args[2]->Uint32Value(),
						args[3]->Uint32Value());
	} else {
		throwWrongNumberOfArguments(isolate);
	}

	returnVoid(isolate);
}

RET SQLRCursor::substitutions(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	// FIXME: array parameters...
	/*sqlrcur(args)->substitutions(const char **variables,
						const char **values);
	sqlrcur(args)->substitutions(const char **variables,
						const int64_t *values);
	sqlrcur(args)->substitutions(const char **variables,
						const double *values,
						const uint32_t *precisions,
						const uint32_t *scales);*/

	returnVoid(isolate);
}

RET SQLRCursor::inputBind(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	if (args.Length()==2) {

		if (args[1]->IsString()) {

			// string
			sqlrcur(args)->inputBind(argToString(args[0]),
							argToString(args[1]));
		} else if (args[1]->IsInt32() ||
				args[1]->IsUint32() ||
				args[1]->IsNumber()) {

			// integer
			sqlrcur(args)->inputBind(argToString(args[0]),
							args[1]->Int32Value());
		} else {
			throwInvalidArgumentType(isolate);
		}

	} else if (args.Length()==3) {

		// string with length
		sqlrcur(args)->inputBind(argToString(args[0]),
						argToString(args[1]),
						args[2]->Uint32Value());

	} else if (args.Length()==4) {

		// decimal
		sqlrcur(args)->inputBind(argToString(args[0]),
						args[1]->NumberValue(),
						args[2]->Uint32Value(),
						args[3]->Uint32Value());

	} else if (args.Length()==9) {

		// date
		sqlrcur(args)->inputBind(argToString(args[0]),
						args[1]->Int32Value(),
						args[2]->Int32Value(),
						args[3]->Int32Value(),
						args[4]->Int32Value(),
						args[5]->Int32Value(),
						args[6]->Int32Value(),
						args[7]->Int32Value(),
						argToString(args[8]));

	} else {
		throwWrongNumberOfArguments(isolate);
	}

	returnVoid(isolate);
}

RET SQLRCursor::inputBindBlob(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,3);

	sqlrcur(args)->inputBindBlob(argToString(args[0]),
					argToString(args[1]),
					args[2]->Uint32Value());

	returnVoid(isolate);
}

RET SQLRCursor::inputBindClob(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,3);

	sqlrcur(args)->inputBindClob(argToString(args[0]),
					argToString(args[1]),
					args[2]->Uint32Value());

	returnVoid(isolate);
}

RET SQLRCursor::inputBinds(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	// FIXME: array parameters...
	/*sqlrcur(args)->inputBinds(const char **variables,
 					const char **values);
	sqlrcur(args)->inputBinds(const char **variables,
					const int64_t *values);
	sqlrcur(args)->inputBinds(const char **variables,
					const double *values,
					const uint32_t *precisions,
					const uint32_t *scales);*/

	returnVoid(isolate);
}

RET SQLRCursor::defineOutputBindString(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	sqlrcur(args)->defineOutputBindString(argToString(args[0]),
						args[1]->Uint32Value());

	returnVoid(isolate);
}

RET SQLRCursor::defineOutputBindInteger(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcur(args)->defineOutputBindInteger(argToString(args[0]));

	returnVoid(isolate);
}

RET SQLRCursor::defineOutputBindDouble(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcur(args)->defineOutputBindDouble(argToString(args[0]));

	returnVoid(isolate);
}

RET SQLRCursor::defineOutputBindBlob(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcur(args)->defineOutputBindBlob(argToString(args[0]));

	returnVoid(isolate);
}

RET SQLRCursor::defineOutputBindClob(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcur(args)->defineOutputBindClob(argToString(args[0]));

	returnVoid(isolate);
}

RET SQLRCursor::defineOutputBindCursor(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	sqlrcur(args)->defineOutputBindCursor(argToString(args[0]));

	returnVoid(isolate);
}

RET SQLRCursor::clearBinds(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->clearBinds();

	returnVoid(isolate);
}

RET SQLRCursor::countBindVariables(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint16_t	result=sqlrcur(args)->countBindVariables();

	returnInteger(isolate,result);
}

RET SQLRCursor::validateBinds(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->validateBinds();

	returnVoid(isolate);
}

RET SQLRCursor::validBind(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=sqlrcur(args)->validBind(argToString(args[0]));

	returnBoolean(isolate,result);
}

RET SQLRCursor::executeQuery(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcur(args)->executeQuery();

	returnBoolean(isolate,result);
}

RET SQLRCursor::fetchFromBindCursor(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcur(args)->fetchFromBindCursor();

	returnBoolean(isolate,result);
}

RET SQLRCursor::getOutputBindString(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	const char	*result=sqlrcur(args)->getOutputBindString(
						argToString(args[0]));

	returnString(isolate,result);
}

RET SQLRCursor::getOutputBindInteger(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	int64_t	result=sqlrcur(args)->getOutputBindInteger(
						argToString(args[0]));

	returnInteger(isolate,result);
}

RET SQLRCursor::getOutputBindDouble(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	double	result=sqlrcur(args)->getOutputBindDouble(
						argToString(args[0]));

	returnNumber(isolate,result);
}

RET SQLRCursor::getOutputBindBlob(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	const char	*result=sqlrcur(args)->getOutputBindBlob(
						argToString(args[0]));

	returnString(isolate,result);
}

RET SQLRCursor::getOutputBindClob(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	const char	*result=sqlrcur(args)->getOutputBindClob(
						argToString(args[0]));

	returnString(isolate,result);
}

RET SQLRCursor::getOutputBindLength(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	uint32_t	result=sqlrcur(args)->getOutputBindLength(
						argToString(args[0]));

	returnInteger(isolate,result);
}

RET SQLRCursor::getOutputBindCursor(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	// FIXME: support this...
	/*sqlrcursor	*result=sqlrcur(args)->getOutputBindCursor(
						argToString(args[0]));
	returnObject(result);*/

	returnVoid(isolate);
}

RET SQLRCursor::openCachedResultSet(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=sqlrcur(args)->openCachedResultSet(
						argToString(args[0]));

	returnBoolean(isolate,result);
}

RET SQLRCursor::colCount(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint32_t	result=sqlrcur(args)->colCount();

	returnInteger(isolate,result);
}

RET SQLRCursor::rowCount(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint64_t	result=sqlrcur(args)->rowCount();

	returnInteger(isolate,result);
}

RET SQLRCursor::totalRows(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint64_t	result=sqlrcur(args)->totalRows();

	returnInteger(isolate,result);
}

RET SQLRCursor::affectedRows(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint64_t	result=sqlrcur(args)->affectedRows();

	returnInteger(isolate,result);
}

RET SQLRCursor::firstRowIndex(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint64_t	result=sqlrcur(args)->firstRowIndex();

	returnInteger(isolate,result);
}

RET SQLRCursor::endOfResultSet(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	bool	result=sqlrcur(args)->endOfResultSet();

	returnBoolean(isolate,result);
}

RET SQLRCursor::errorMessage(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	const char	*result=sqlrcur(args)->errorMessage();

	returnString(isolate,result);
}

RET SQLRCursor::errorNumber(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	int64_t	result=sqlrcur(args)->errorNumber();

	returnInteger(isolate,result);
}

RET SQLRCursor::getNullsAsEmptyStrings(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->getNullsAsEmptyStrings();

	returnVoid(isolate);
}

RET SQLRCursor::getNullsAsNulls(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->getNullsAsNulls();

	returnVoid(isolate);
}

RET SQLRCursor::getField(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	const char	*result=NULL;

	if (args[1]->IsInt32() || args[1]->IsUint32() || args[1]->IsNumber()) {
		result=sqlrcur(args)->getField(args[0]->Uint32Value(),
						args[1]->Uint32Value());
	} else if (args[1]->IsString()) {
		result=sqlrcur(args)->getField(args[0]->Uint32Value(),
						argToString(args[1]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnString(isolate,result);
}

RET SQLRCursor::getFieldAsInteger(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	int64_t	result=0;

	if (args[1]->IsInt32() || args[1]->IsUint32() || args[1]->IsNumber()) {
		result=sqlrcur(args)->getFieldAsInteger(
						args[0]->Uint32Value(),
						args[1]->Uint32Value());
	} else if (args[1]->IsString()) {
		result=sqlrcur(args)->getFieldAsInteger(
						args[0]->Uint32Value(),
						argToString(args[1]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnInteger(isolate,result);
}

RET SQLRCursor::getFieldAsDouble(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	double	result=0;

	if (args[1]->IsInt32() || args[1]->IsUint32() || args[1]->IsNumber()) {
		result=sqlrcur(args)->getFieldAsDouble(
						args[0]->Uint32Value(),
						args[1]->Uint32Value());
	} else if (args[1]->IsString()) {
		result=sqlrcur(args)->getFieldAsDouble(
						args[0]->Uint32Value(),
						argToString(args[1]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnNumber(isolate,result);
}

RET SQLRCursor::getFieldLength(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	uint32_t	result=0;

	if (args[1]->IsInt32() || args[1]->IsUint32() || args[1]->IsNumber()) {
		result=sqlrcur(args)->getFieldLength(
						args[0]->Uint32Value(),
						args[1]->Uint32Value());
	} else if (args[1]->IsString()) {
		result=sqlrcur(args)->getFieldLength(
						args[0]->Uint32Value(),
						argToString(args[1]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnInteger(isolate,result);
}

RET SQLRCursor::getRow(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	// FIXME: return array of strings
	/*const char * const *result=sqlrcur(args)->getRow(
						args[0]->Uint32Value());*/

	returnArray(args,result);
}

RET SQLRCursor::getRowLengths(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	// FIXME: return array of uint32's
	/*uint32_t	*result=sqlrcur(args)->getRowLengths(
						args[0]->Uint32Value());*/

	returnArray(isolate,result);
}

RET SQLRCursor::getColumnNames(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	// FIXME: return array of strings
	//const char * const *result=sqlrcur(args)->getColumnNames();

	returnArray(args,result);
}

RET SQLRCursor::getColumnName(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	const char	*result=sqlrcur(args)->getColumnName(
						args[0]->Uint32Value());

	returnString(isolate,result);
}

RET SQLRCursor::getColumnType(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	const char	*result=NULL;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnType(args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnType(argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnString(isolate,result);
}

RET SQLRCursor::getColumnLength(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	uint32_t	result=0;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnLength(args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnLength(argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnInteger(isolate,result);
}

RET SQLRCursor::getColumnPrecision(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	uint32_t	result=0;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnPrecision(
						args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnPrecision(
						argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnInteger(isolate,result);
}

RET SQLRCursor::getColumnScale(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	uint32_t	result=0;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnScale(args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnScale(argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnInteger(isolate,result);
}

RET SQLRCursor::getColumnIsNullable(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=false;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsNullable(
						args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsNullable(
						argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnBoolean(isolate,result);
}

RET SQLRCursor::getColumnIsPrimaryKey(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=false;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsPrimaryKey(
						args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsPrimaryKey(
						argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}


	returnBoolean(isolate,result);
}

RET SQLRCursor::getColumnIsUnique(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=false;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsUnique(
						args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsUnique(
						argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnBoolean(isolate,result);
}

RET SQLRCursor::getColumnIsPartOfKey(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=false;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsPartOfKey(
						args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsPartOfKey(
						argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnBoolean(isolate,result);
}

RET SQLRCursor::getColumnIsUnsigned(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=false;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsUnsigned(
						args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsUnsigned(
						argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnBoolean(isolate,result);
}

RET SQLRCursor::getColumnIsZeroFilled(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=false;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsZeroFilled(
						args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsZeroFilled(
						argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnBoolean(isolate,result);
}

RET SQLRCursor::getColumnIsBinary(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=false;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsBinary(
						args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsBinary(
						argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnBoolean(isolate,result);
}

RET SQLRCursor::getColumnIsAutoIncrement(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=false;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsAutoIncrement(
						args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsAutoIncrement(
						argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnBoolean(isolate,result);
}

RET SQLRCursor::getLongest(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	uint32_t	result=0;

	if (args[0]->IsInt32() || args[0]->IsUint32() || args[0]->IsNumber()) {
		result=sqlrcur(args)->getLongest(args[0]->Uint32Value());
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getLongest(argToString(args[0]));
	} else {
		throwInvalidArgumentType(isolate);
	}

	returnInteger(isolate,result);
}

RET SQLRCursor::suspendResultSet(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->suspendResultSet();

	returnVoid(isolate);
}

RET SQLRCursor::getResultSetId(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	uint16_t	result=sqlrcur(args)->getResultSetId();

	returnInteger(isolate,result);
}

RET SQLRCursor::resumeResultSet(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=sqlrcur(args)->resumeResultSet(args[0]->Uint32Value());

	returnBoolean(isolate,result);
}

RET SQLRCursor::resumeCachedResultSet(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,2);

	bool	result=sqlrcur(args)->resumeCachedResultSet(
						args[0]->Uint32Value(),
						argToString(args[1]));

	returnBoolean(isolate,result);
}

RET SQLRCursor::closeResultSet(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcur(args)->closeResultSet();

	returnVoid(isolate);
}

sqlrcursor *SQLRCursor::sqlrcur(const ARGS &args) {
	return ObjectWrap::Unwrap<SQLRCursor>(args.Holder())->sqlrc;
}



// module functions...
void init(Handle<Object> exports) {
	SQLRConnection::Init(exports);
	SQLRCursor::Init(exports);
}

NODE_MODULE(sqlrelay,init)
