// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#ifdef _WIN32
	#define _SSIZE_T_DEFINED
#endif
#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

using namespace v8;
using namespace node;

// macros to deal with differences between major versions of node.js
#if NODE_MAJOR_VERSION > 0 || NODE_MINOR_VERSION >= 12

	#if NODE_MAJOR_VERSION >= 12
		#define Handle Local
	#endif

	#define RET void
	#define ARGS FunctionCallbackInfo<Value>

	#define initLocalScope() Isolate *isolate=Isolate::GetCurrent(); HandleScope	localscope(isolate)

	#define resetConstructor(constructor,tpl) constructor.Reset(isolate,GetFunction(tpl))

	#define returnObject(object) args.GetReturnValue().Set(object)
	#define returnString(result) if (result) { args.GetReturnValue().Set(newString(result)); } else { args.GetReturnValue().Set(Null(isolate)); }
	#define returnBoolean(result) args.GetReturnValue().Set(newBoolean(result))
	#define returnInteger(result) args.GetReturnValue().Set(newInteger(result))
	#define returnUnsignedInteger(result) args.GetReturnValue().Set(newUnsignedInteger(result))
	#define returnInt32(result) args.GetReturnValue().Set(newInt32(result))
	#define returnUint32(result) args.GetReturnValue().Set(newUint32(result))
	#define returnNumber(result) args.GetReturnValue().Set(newNumber(result))
	#define returnVoid()

	#define newFunctionTemplate(func) FunctionTemplate::New(isolate,func)
	#define newLocalFunction(func) Local<Function>::New(isolate,func)
	#define newString(val) String::NewFromUtf8(isolate,val)
	#define newBoolean(val) Boolean::New(isolate,val)
	#define newInteger(val) Integer::New(isolate,val)
	#define newUnsignedInteger(val) Integer::NewFromUnsigned(isolate,val)
	#define newUint32(val) Uint32::New(isolate,val)
	#define newInt32(val) Int32::New(isolate,val)
	#define newNumber(val) Number::New(isolate,val)
	#define newArray(len) Array::New(isolate,len)
	#if NODE_MAJOR_VERSION >= 10
		#define newInstance(argc,argv) cons->NewInstance(isolate->GetCurrentContext(),argc,argv).ToLocalChecked()
	#else
		#define newInstance(argc,argv) cons->NewInstance(argc,argv)
	#endif

	#define checkArgCount(args,count) if (args.Length()!=count) { throwWrongNumberOfArguments(); return; }

#else

	#define RET Handle<Value>
	#define ARGS Arguments

	#define initLocalScope() HandleScope	localscope

	#define resetConstructor(constructor,tpl)

	#define returnObject(object) return localscope.Close(object)
	#define returnBoolean(result) return localscope.Close(Boolean::New(result))
	#define returnString(result) if (result) { return localscope.Close(String::New(result)); } else { return localscope.Close(Null()); }
	#define returnInteger(result) return localscope.Close(newInteger(result))
	#define returnUnsignedInteger(result) return localscope.Close(newUnsignedInteger(result))
	#define returnInt32(result) return localscope.Close(newInt32(result))
	#define returnUint32(result) return localscope.Close(newUint32(result))
	#define returnNumber(result) return localscope.Close(newNumber(result))
	#define returnVoid() return localscope.Close(Null())

	#define newFunctionTemplate(func) FunctionTemplate::New(func)
	#define newLocalFunction(func) Local<Function>::New(func)
	#define newString(val) String::New(val)
	#define newBoolean(val) Boolean::New(val)
	#define newInteger(val) Integer::New(val)
	#define newUnsignedInteger(val) Integer::NewFromUnsigned(val)
	#define newUint32(val) Uint32::New(val)
	#define newInt32(val) Int32::New(val)
	#define newNumber(val) Number::New(val)
	#define newArray(len) Array::New(len)
	#define newInstance(argc,argv) cons->NewInstance(argc,argv)

	#define checkArgCount(args,count) if (args.Length()!=count) { throwWrongNumberOfArguments(); returnBoolean(false); }

#endif


// convenience macros
#if NODE_MAJOR_VERSION < 12
	#define toString(arg) ((arg->IsNull())?NULL:*(String::Utf8Value(arg)))
	#define toInt32(arg) arg->Int32Value()
	#define toUint32(arg) arg->Uint32Value()
	#define toInteger(arg) arg->IntegerValue()
	#define toNumber(arg) arg->NumberValue()
	#define toObject(arg) arg->ToObject()
	#define GetFunction(arg) arg->GetFunction()
#else
	#define toString(arg) ((arg->IsNull())?NULL:*(String::Utf8Value(isolate,arg)))
	#define toInt32(arg) arg->Int32Value(isolate->GetCurrentContext()).ToChecked()
	#define toUint32(arg) arg->Uint32Value(isolate->GetCurrentContext()).ToChecked()
	#define toInteger(arg) arg->IntegerValue(isolate->GetCurrentContext()).ToChecked()
	#define toNumber(arg) arg->NumberValue(isolate->GetCurrentContext()).ToChecked()
	#define toObject(arg) arg->ToObject(isolate->GetCurrentContext()).ToLocalChecked()
	#define GetFunction(arg) arg->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()
#endif
#define toArray(arg) Handle<Array>::Cast(arg);

#if NODE_MAJOR_VERSION > 0 || NODE_MINOR_VERSION >= 12
	#define throwWrongNumberOfArguments() isolate->ThrowException(Exception::TypeError(newString("Wrong number of arguments")))
	#define throwInvalidArgumentType() isolate->ThrowException(Exception::TypeError(newString("Invalid argument type")))
#else
	#define throwWrongNumberOfArguments() ThrowException(Exception::TypeError(newString("Wrong number of arguments")))
	#define throwInvalidArgumentType() ThrowException(Exception::TypeError(newString("Invalid argument type")))
#endif



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
		static RET	setBindVariableDelimiters(const ARGS &args);
		static RET	getBindVariableDelimiterQuestionMarkSupported(
							const ARGS &args);
		static RET	getBindVariableDelimiterColonSupported(
							const ARGS &args);
		static RET	getBindVariableDelimiterAtSignSupported(
							const ARGS &args);
		static RET	getBindVariableDelimiterDollarSignSupported(
							const ARGS &args);
		static RET	enableKerberos(const ARGS &args);
		static RET	enableTls(const ARGS &args);
		static RET	disableEncryption(const ARGS &args);
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

	initLocalScope();

	Local<FunctionTemplate>	tpl=newFunctionTemplate(New);
	tpl->SetClassName(newString("SQLRConnection"));
	// internal field count is the number of non-static member variables
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	NODE_SET_PROTOTYPE_METHOD(tpl,"setConnectTimeout",setConnectTimeout);
	NODE_SET_PROTOTYPE_METHOD(tpl,"setAuthenticationTimeout",
						setAuthenticationTimeout);
	NODE_SET_PROTOTYPE_METHOD(tpl,"setResponseTimeout",setResponseTimeout);
	NODE_SET_PROTOTYPE_METHOD(tpl,
			"setBindVariableDelimiters",
			setBindVariableDelimiters);
	NODE_SET_PROTOTYPE_METHOD(tpl,
			"getBindVariableDelimiterQuestionMarkSupported",
			getBindVariableDelimiterQuestionMarkSupported);
	NODE_SET_PROTOTYPE_METHOD(tpl,
			"getBindVariableDelimiterColonSupported",
			getBindVariableDelimiterColonSupported);
	NODE_SET_PROTOTYPE_METHOD(tpl,
			"getBindVariableDelimiterAtSignSupported",
			getBindVariableDelimiterAtSignSupported);
	NODE_SET_PROTOTYPE_METHOD(tpl,
			"getBindVariableDelimiterDollarSignSupported",
			getBindVariableDelimiterDollarSignSupported);
	NODE_SET_PROTOTYPE_METHOD(tpl,"enableKerberos",enableKerberos);
	NODE_SET_PROTOTYPE_METHOD(tpl,"enableTls",enableTls);
	NODE_SET_PROTOTYPE_METHOD(tpl,"disableEncryption",disableEncryption);
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

	resetConstructor(constructor,tpl);
	exports->Set(newString("SQLRConnection"),GetFunction(tpl));
}

SQLRConnection::SQLRConnection() {
}

SQLRConnection::~SQLRConnection() {
}

RET SQLRConnection::New(const ARGS &args) {

	initLocalScope();

	if (args.IsConstructCall()) {

		checkArgCount(args,7);

		// invoked as constructor: new SQLRConnection(...)
		SQLRConnection	*obj=new SQLRConnection();
		obj->sqlrc=new sqlrconnection(toString(args[0]),
						toUint32(args[1]),
						toString(args[2]),
						toString(args[3]),
						toString(args[4]),
						toInt32(args[5]),
						toInt32(args[6]),
						true);
		obj->Wrap(args.This());
		returnObject(args.This());

	} else {

		// invoked as function: SQLRConnection(...)
		const int	argc=1;
		Local<Value>	argv[argc]={args[0]};
		Local<Function>	cons=newLocalFunction(constructor);
		returnObject(newInstance(argc,argv));
	}
}

RET SQLRConnection::setConnectTimeout(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	sqlrcon(args)->setConnectTimeout(toInt32(args[0]),toInt32(args[1]));

	returnVoid();
}

RET SQLRConnection::setAuthenticationTimeout(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	sqlrcon(args)->setAuthenticationTimeout(toInt32(args[0]),
						toInt32(args[1]));

	returnVoid();
}

RET SQLRConnection::setResponseTimeout(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	sqlrcon(args)->setResponseTimeout(toInt32(args[0]),toInt32(args[1]));

	returnVoid();
}

RET SQLRConnection::setBindVariableDelimiters(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcon(args)->setBindVariableDelimiters(toString(args[0]));

	returnVoid();
}

RET SQLRConnection::getBindVariableDelimiterQuestionMarkSupported(
							const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->
			getBindVariableDelimiterQuestionMarkSupported();

	returnBoolean(result);
}

RET SQLRConnection::getBindVariableDelimiterColonSupported(
							const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->
			getBindVariableDelimiterColonSupported();

	returnBoolean(result);
}

RET SQLRConnection::getBindVariableDelimiterAtSignSupported(
							const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->
			getBindVariableDelimiterAtSignSupported();

	returnBoolean(result);
}

RET SQLRConnection::getBindVariableDelimiterDollarSignSupported(
							const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->
			getBindVariableDelimiterDollarSignSupported();

	returnBoolean(result);
}

RET SQLRConnection::enableKerberos(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,3);

	sqlrcon(args)->enableKerberos(toString(args[0]),
					toString(args[1]),
					toString(args[2]));

	returnVoid();
}

RET SQLRConnection::enableTls(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,7);

	sqlrcon(args)->enableTls(toString(args[0]),
					toString(args[1]),
					toString(args[2]),
					toString(args[3]),
					toString(args[4]),
					toString(args[5]),
					toUint32(args[6]));

	returnVoid();
}

RET SQLRConnection::disableEncryption(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcon(args)->disableEncryption();

	returnVoid();
}

RET SQLRConnection::endSession(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcon(args)->endSession();

	returnVoid();
}

RET SQLRConnection::suspendSession(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->suspendSession();

	returnBoolean(result);
}

RET SQLRConnection::getConnectionPort(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint16_t	result=sqlrcon(args)->getConnectionPort();

	returnInt32(result);
}

RET SQLRConnection::getConnectionSocket(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->getConnectionSocket();

	returnString(result);
}

RET SQLRConnection::resumeSession(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	bool	result=sqlrcon(args)->resumeSession(toInt32(args[0]),
							toString(args[1]));

	returnBoolean(result);
}

RET SQLRConnection::ping(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->ping();

	returnBoolean(result);
}

RET SQLRConnection::identify(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->identify();

	returnString(result);
}

RET SQLRConnection::dbVersion(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->dbVersion();

	returnString(result);
}

RET SQLRConnection::dbHostName(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->dbHostName();

	returnString(result);
}

RET SQLRConnection::dbIpAddress(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->dbIpAddress();

	returnString(result);
}

RET SQLRConnection::serverVersion(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->serverVersion();

	returnString(result);
}

RET SQLRConnection::clientVersion(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->clientVersion();

	returnString(result);
}

RET SQLRConnection::bindFormat(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->bindFormat();

	returnString(result);
}

RET SQLRConnection::selectDatabase(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=sqlrcon(args)->selectDatabase(toString(args[0]));

	returnBoolean(result);
}

RET SQLRConnection::getCurrentDatabase(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->getCurrentDatabase();

	returnString(result);
}

RET SQLRConnection::getLastInsertId(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint64_t	result=sqlrcon(args)->getLastInsertId();

	returnUnsignedInteger(result);
}

RET SQLRConnection::autoCommitOn(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->autoCommitOn();

	returnBoolean(result);
}

RET SQLRConnection::autoCommitOff(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->autoCommitOff();

	returnBoolean(result);
}

RET SQLRConnection::begin(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->begin();

	returnBoolean(result);
}

RET SQLRConnection::commit(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->commit();

	returnBoolean(result);
}

RET SQLRConnection::rollback(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->rollback();

	returnBoolean(result);
}

RET SQLRConnection::errorMessage(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->errorMessage();

	returnString(result);
}

RET SQLRConnection::errorNumber(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	int64_t		result=sqlrcon(args)->errorNumber();

	returnInteger(result);
}

RET SQLRConnection::debugOn(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcon(args)->debugOn();

	returnVoid();
}

RET SQLRConnection::debugOff(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcon(args)->debugOff();

	returnVoid();
}

RET SQLRConnection::getDebug(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcon(args)->getDebug();

	returnBoolean(result);
}

RET SQLRConnection::setDebugFile(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcon(args)->setDebugFile(toString(args[0]));

	returnVoid();
}

RET SQLRConnection::setClientInfo(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcon(args)->setClientInfo(toString(args[0]));

	returnVoid();
}

RET SQLRConnection::getClientInfo(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcon(args)->getClientInfo();

	returnString(result);
}

sqlrconnection *SQLRConnection::sqlrcon(const ARGS &args) {
	return ObjectWrap::Unwrap<SQLRConnection>(args.Holder())->sqlrc;
}



// SQLRCursor methods...
void SQLRCursor::Init(Handle<Object> exports) {

	initLocalScope();

	Local<FunctionTemplate>	tpl=newFunctionTemplate(New);
	tpl->SetClassName(newString("SQLRCursor"));
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
	NODE_SET_PROTOTYPE_METHOD(tpl,"getOutputBindBlob",
						getOutputBindBlob);
	NODE_SET_PROTOTYPE_METHOD(tpl,"getOutputBindClob",
						getOutputBindClob);
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

	resetConstructor(constructor,tpl);
	exports->Set(newString("SQLRCursor"),GetFunction(tpl));
}

SQLRCursor::SQLRCursor() {
}

SQLRCursor::~SQLRCursor() {
}

RET SQLRCursor::New(const ARGS &args) {

	initLocalScope();

	if (args.IsConstructCall()) {

		checkArgCount(args,1);

		// invoked as constructor: new SQLRCursor(...)
		sqlrconnection	*sqlrcon=
			node::ObjectWrap::Unwrap<SQLRConnection>(
						toObject(args[0]))->sqlrc;

		SQLRCursor	*obj=new SQLRCursor();
		obj->sqlrc=new sqlrcursor(sqlrcon,true);
		obj->Wrap(args.This());
		returnObject(args.This());

	} else {

		// invoked as function: SQLRCursor(...)
		const int	argc=1;
		Local<Value>	argv[argc]={args[0]};
		Local<Function>	cons=newLocalFunction(constructor);
		returnObject(newInstance(argc,argv));
	}
}

RET SQLRCursor::setResultSetBufferSize(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcur(args)->setResultSetBufferSize(toInteger(args[0]));

	returnVoid();
}

RET SQLRCursor::getResultSetBufferSize(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint64_t	result=sqlrcur(args)->getResultSetBufferSize();

	returnUnsignedInteger(result);
}

RET SQLRCursor::dontGetColumnInfo(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->dontGetColumnInfo();

	returnVoid();
}

RET SQLRCursor::getColumnInfo(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->getColumnInfo();

	returnVoid();
}

RET SQLRCursor::mixedCaseColumnNames(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->mixedCaseColumnNames();

	returnVoid();
}

RET SQLRCursor::upperCaseColumnNames(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->upperCaseColumnNames();

	returnVoid();
}

RET SQLRCursor::lowerCaseColumnNames(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->lowerCaseColumnNames();

	returnVoid();
}

RET SQLRCursor::cacheToFile(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcur(args)->cacheToFile(toString(args[0]));

	returnVoid();
}

RET SQLRCursor::setCacheTtl(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcur(args)->setCacheTtl(toUint32(args[0]));

	returnVoid();
}

RET SQLRCursor::getCacheFileName(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcur(args)->getCacheFileName();

	returnString(result);
}

RET SQLRCursor::cacheOff(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->cacheOff();

	returnVoid();
}

RET SQLRCursor::getDatabaseList(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=sqlrcur(args)->getDatabaseList(toString(args[0]));

	returnBoolean(result);
}

RET SQLRCursor::getTableList(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=sqlrcur(args)->getTableList(toString(args[0]));

	returnBoolean(result);
}

RET SQLRCursor::getColumnList(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	bool	result=sqlrcur(args)->getColumnList(toString(args[0]),
							toString(args[1]));

	returnBoolean(result);
}

RET SQLRCursor::sendQuery(const ARGS &args) {

	initLocalScope();

	bool	result=false;

	if (args.Length()==1) {
		result=sqlrcur(args)->sendQuery(toString(args[0]));
	} else if (args.Length()==2) {
		result=sqlrcur(args)->sendQuery(toString(args[0]),
						toUint32(args[1]));
	} else {
		throwWrongNumberOfArguments();
	}

	returnBoolean(result);
}

RET SQLRCursor::sendFileQuery(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	bool	result=sqlrcur(args)->sendFileQuery(toString(args[0]),
							toString(args[1]));

	returnBoolean(result);
}

RET SQLRCursor::prepareQuery(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	if (args.Length()==1) {
		sqlrcur(args)->prepareQuery(toString(args[0]));
	} else if (args.Length()==2) {
		sqlrcur(args)->prepareQuery(toString(args[0]),
						toUint32(args[1]));
	} else {
		throwWrongNumberOfArguments();
	}

	returnVoid();
}

RET SQLRCursor::prepareFileQuery(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	bool	result=sqlrcur(args)->prepareFileQuery(toString(args[0]),
							toString(args[1]));

	returnBoolean(result);
}

RET SQLRCursor::substitution(const ARGS &args) {

	initLocalScope();

	if (args.Length()==2) {
		if (args[1]->IsString() || args[1]->IsNull()) {
			sqlrcur(args)->substitution(toString(args[0]),
							toString(args[1]));
		} else if (args[1]->IsNumber()) {
			sqlrcur(args)->substitution(toString(args[0]),
							toInteger(args[1]));
		} else {
			throwInvalidArgumentType();
		}
	} else if (args.Length()==4) {
		sqlrcur(args)->substitution(toString(args[0]),
						toNumber(args[1]),
						toUint32(args[2]),
						toUint32(args[3]));
	} else {
		throwWrongNumberOfArguments();
	}

	returnVoid();
}

RET SQLRCursor::substitutions(const ARGS &args) {

	initLocalScope();

	if (args.Length()==2) {

		if (args[0]->IsArray() && args[1]->IsArray()) {
			
			Handle<Array>	vars=toArray(args[0]);
			Handle<Array>	vals=toArray(args[1]);

			if (vars->Length()) {

				Local<Value>	first=
					vals->Get(newInteger(0));

				if (first->IsString() || first->IsNull()) {

					for (uint32_t i=0;
						i<vars->Length(); i++) {

						sqlrcur(args)->
							substitution(

							toString(vars->Get(
							newInteger(i))),

							toString(vals->Get(
							newInteger(i)))
							);
					}

				} else if (first->IsNumber()) {

					for (uint32_t i=0;
						i<vars->Length(); i++) {

						sqlrcur(args)->
							substitution(

							toString(vars->Get(
							newInteger(i))),

							toInteger(vals->Get(
							newInteger(i))));
					}

				} else {
					throwInvalidArgumentType();
				}
			}
		} else {
			throwInvalidArgumentType();
		}
	} else if (args.Length()==4) {

		if (args[0]->IsArray() && args[1]->IsArray() &&
			args[2]->IsArray() && args[3]->IsArray()) {
			
			Handle<Array>	vars=toArray(args[0]);
			Handle<Array>	vals=toArray(args[1]);
			Handle<Array>	precs=toArray(args[2]);
			Handle<Array>	scales=toArray(args[3]);

			if (vars->Length()) {

				Local<Value>	first=
					vals->Get(newInteger(0));

				if (first->IsNumber()) {

					for (uint32_t i=0;
						i<vars->Length(); i++) {

						sqlrcur(args)->substitution(

							toString(vars->Get(
							newInteger(i))),

							toNumber(
							vals->Get(
							newInteger(i))),

							toUint32(
							precs->Get(
							newInteger(i))),

							toUint32(
							scales->Get(
							newInteger(i))));
					}

				} else {
					throwInvalidArgumentType();
				}
			}
		} else {
			throwInvalidArgumentType();
		}
	} else {
		throwWrongNumberOfArguments();
	}

	returnVoid();
}

RET SQLRCursor::inputBind(const ARGS &args) {

	initLocalScope();

	if (args.Length()==2) {

		if (args[1]->IsString() || args[1]->IsNull()) {

			// string
			sqlrcur(args)->inputBind(toString(args[0]),
							toString(args[1]));
		} else if (args[1]->IsNumber()) {

			// integer
			sqlrcur(args)->inputBind(toString(args[0]),
							toInteger(args[1]));
		} else {
			throwInvalidArgumentType();
		}

	} else if (args.Length()==3) {

		// string with length
		sqlrcur(args)->inputBind(toString(args[0]),
						toString(args[1]),
						toUint32(args[2]));

	} else if (args.Length()==4) {

		// decimal
		sqlrcur(args)->inputBind(toString(args[0]),
						toNumber(args[1]),
						toUint32(args[2]),
						toUint32(args[3]));

	} else if (args.Length()==9) {

		// date
		sqlrcur(args)->inputBind(toString(args[0]),
						toInt32(args[1]),
						toInt32(args[2]),
						toInt32(args[3]),
						toInt32(args[4]),
						toInt32(args[5]),
						toInt32(args[6]),
						toInt32(args[7]),
						toString(args[8]),
						toInt32(args[9]));

	} else {
		throwWrongNumberOfArguments();
	}

	returnVoid();
}

RET SQLRCursor::inputBindBlob(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,3);

	sqlrcur(args)->inputBindBlob(toString(args[0]),
					toString(args[1]),
					toUint32(args[2]));

	returnVoid();
}

RET SQLRCursor::inputBindClob(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,3);

	sqlrcur(args)->inputBindClob(toString(args[0]),
					toString(args[1]),
					toUint32(args[2]));

	returnVoid();
}

RET SQLRCursor::inputBinds(const ARGS &args) {

	initLocalScope();

	if (args.Length()==2) {

		if (args[0]->IsArray() && args[1]->IsArray()) {
			
			Handle<Array>	vars=toArray(args[0]);
			Handle<Array>	vals=toArray(args[1]);

			if (vars->Length()) {

				Local<Value>	first=
					vals->Get(newInteger(0));

				if (first->IsString() || first->IsNull()) {

					for (uint32_t i=0;
						i<vars->Length(); i++) {

						sqlrcur(args)->
							inputBind(

							toString(vars->Get(
							newInteger(i))),

							toString(vals->Get(
							newInteger(i)))
							);
					}

				} else if (first->IsNumber()) {

					for (uint32_t i=0;
						i<vars->Length(); i++) {

						sqlrcur(args)->
							inputBind(

							toString(vars->Get(
							newInteger(i))),

							toInteger(
							vals->Get(
							newInteger(i))));
					}

				} else {
					throwInvalidArgumentType();
				}
			}
		} else {
			throwInvalidArgumentType();
		}
	} else if (args.Length()==4) {

		if (args[0]->IsArray() && args[1]->IsArray() &&
			args[2]->IsArray() && args[3]->IsArray()) {
			
			Handle<Array>	vars=toArray(args[0]);
			Handle<Array>	vals=toArray(args[1]);
			Handle<Array>	precs=toArray(args[2]);
			Handle<Array>	scales=toArray(args[3]);

			if (vars->Length()) {

				Local<Value>	first=
					vals->Get(newInteger(0));

				if (first->IsNumber()) {

					for (uint32_t i=0;
						i<vars->Length(); i++) {

						sqlrcur(args)->inputBind(

							toString(
							vars->Get(
							newInteger(i))),

							toNumber(
							vals->Get(
							newInteger(i))),

							toUint32(
							precs->Get(
							newInteger(i))),

							toUint32(
							scales->Get(
							newInteger(i))));
					}

				} else {
					throwInvalidArgumentType();
				}
			}
		} else {
			throwInvalidArgumentType();
		}
	} else {
		throwWrongNumberOfArguments();
	}

	returnVoid();
}

RET SQLRCursor::defineOutputBindString(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	sqlrcur(args)->defineOutputBindString(toString(args[0]),
						toUint32(args[1]));

	returnVoid();
}

RET SQLRCursor::defineOutputBindInteger(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcur(args)->defineOutputBindInteger(toString(args[0]));

	returnVoid();
}

RET SQLRCursor::defineOutputBindDouble(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcur(args)->defineOutputBindDouble(toString(args[0]));

	returnVoid();
}

RET SQLRCursor::defineOutputBindBlob(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcur(args)->defineOutputBindBlob(toString(args[0]));

	returnVoid();
}

RET SQLRCursor::defineOutputBindClob(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcur(args)->defineOutputBindClob(toString(args[0]));

	returnVoid();
}

RET SQLRCursor::defineOutputBindCursor(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	sqlrcur(args)->defineOutputBindCursor(toString(args[0]));

	returnVoid();
}

RET SQLRCursor::clearBinds(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->clearBinds();

	returnVoid();
}

RET SQLRCursor::countBindVariables(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint16_t	result=sqlrcur(args)->countBindVariables();

	returnUint32(result);
}

RET SQLRCursor::validateBinds(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->validateBinds();

	returnVoid();
}

RET SQLRCursor::validBind(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=sqlrcur(args)->validBind(toString(args[0]));

	returnBoolean(result);
}

RET SQLRCursor::executeQuery(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcur(args)->executeQuery();

	returnBoolean(result);
}

RET SQLRCursor::fetchFromBindCursor(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcur(args)->fetchFromBindCursor();

	returnBoolean(result);
}

RET SQLRCursor::getOutputBindString(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	const char	*result=sqlrcur(args)->getOutputBindString(
						toString(args[0]));

	returnString(result);
}

RET SQLRCursor::getOutputBindInteger(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	int64_t	result=sqlrcur(args)->getOutputBindInteger(
						toString(args[0]));

	returnInteger(result);
}

RET SQLRCursor::getOutputBindDouble(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	double	result=sqlrcur(args)->getOutputBindDouble(
						toString(args[0]));

	returnNumber(result);
}

RET SQLRCursor::getOutputBindBlob(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	const char	*result=sqlrcur(args)->getOutputBindBlob(
						toString(args[0]));

	returnString(result);
}

RET SQLRCursor::getOutputBindClob(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	const char	*result=sqlrcur(args)->getOutputBindClob(
						toString(args[0]));

	returnString(result);
}

RET SQLRCursor::getOutputBindLength(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	uint32_t	result=sqlrcur(args)->getOutputBindLength(
						toString(args[0]));

	returnUint32(result);
}

RET SQLRCursor::getOutputBindCursor(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	SQLRCursor	*obj=new SQLRCursor();
	obj->sqlrc=sqlrcur(args)->getOutputBindCursor(toString(args[0]),true);
	obj->Wrap(args.This());
	returnObject(args.This());
}

RET SQLRCursor::openCachedResultSet(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=sqlrcur(args)->openCachedResultSet(
						toString(args[0]));

	returnBoolean(result);
}

RET SQLRCursor::colCount(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint32_t	result=sqlrcur(args)->colCount();

	returnUint32(result);
}

RET SQLRCursor::rowCount(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint64_t	result=sqlrcur(args)->rowCount();

	returnUnsignedInteger(result);
}

RET SQLRCursor::totalRows(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint64_t	result=sqlrcur(args)->totalRows();

	returnUnsignedInteger(result);
}

RET SQLRCursor::affectedRows(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint64_t	result=sqlrcur(args)->affectedRows();

	returnUnsignedInteger(result);
}

RET SQLRCursor::firstRowIndex(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint64_t	result=sqlrcur(args)->firstRowIndex();

	returnUnsignedInteger(result);
}

RET SQLRCursor::endOfResultSet(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	bool	result=sqlrcur(args)->endOfResultSet();

	returnBoolean(result);
}

RET SQLRCursor::errorMessage(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char	*result=sqlrcur(args)->errorMessage();

	returnString(result);
}

RET SQLRCursor::errorNumber(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	int64_t	result=sqlrcur(args)->errorNumber();

	returnInteger(result);
}

RET SQLRCursor::getNullsAsEmptyStrings(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->getNullsAsEmptyStrings();

	returnVoid();
}

RET SQLRCursor::getNullsAsNulls(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->getNullsAsNulls();

	returnVoid();
}

RET SQLRCursor::getField(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	const char	*result=NULL;

	if (args[1]->IsNumber()) {
		result=sqlrcur(args)->getField(toInteger(args[0]),
							toUint32(args[1]));
	} else if (args[1]->IsString()) {
		result=sqlrcur(args)->getField(toInteger(args[0]),
							toString(args[1]));
	} else {
		throwInvalidArgumentType();
	}

	returnString(result);
}

RET SQLRCursor::getFieldAsInteger(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	int64_t	result=0;

	if (args[1]->IsNumber()) {
		result=sqlrcur(args)->getFieldAsInteger(
						toInteger(args[0]),
						toUint32(args[1]));
	} else if (args[1]->IsString()) {
		result=sqlrcur(args)->getFieldAsInteger(
						toInteger(args[0]),
						toString(args[1]));
	} else {
		throwInvalidArgumentType();
	}

	returnInteger(result);
}

RET SQLRCursor::getFieldAsDouble(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	double	result=0;

	if (args[1]->IsNumber()) {
		result=sqlrcur(args)->getFieldAsDouble(
						toInteger(args[0]),
						toUint32(args[1]));
	} else if (args[1]->IsString()) {
		result=sqlrcur(args)->getFieldAsDouble(
						toInteger(args[0]),
						toString(args[1]));
	} else {
		throwInvalidArgumentType();
	}

	returnNumber(result);
}

RET SQLRCursor::getFieldLength(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	uint32_t	result=0;

	if (args[1]->IsNumber()) {
		result=sqlrcur(args)->getFieldLength(
						toInteger(args[0]),
						toUint32(args[1]));
	} else if (args[1]->IsString()) {
		result=sqlrcur(args)->getFieldLength(
						toInteger(args[0]),
						toString(args[1]));
	} else {
		throwInvalidArgumentType();
	}

	returnUint32(result);
}

RET SQLRCursor::getRow(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	const char * const *fields=sqlrcur(args)->getRow(toInteger(args[0]));
	uint32_t	colcount=sqlrcur(args)->colCount();

	Handle<Array>	result=newArray(colcount);
	for (uint32_t i=0; i<colcount; i++) {
		result->Set(newInteger(i),newString(fields[i]));
	}

	returnObject(result);
}

RET SQLRCursor::getRowLengths(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	uint32_t	*lengths=sqlrcur(args)->getRowLengths(
						toInteger(args[0]));
	uint32_t	colcount=sqlrcur(args)->colCount();

	Handle<Array>	result=newArray(colcount);
	for (uint32_t i=0; i<colcount; i++) {
		result->Set(newInteger(i),
				newUint32(lengths[i]));
	}

	returnObject(result);
}

RET SQLRCursor::getColumnNames(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	const char * const *names=sqlrcur(args)->getColumnNames();
	uint32_t	colcount=sqlrcur(args)->colCount();

	Handle<Array>	result=newArray(colcount);
	for (uint32_t i=0; i<colcount; i++) {
		result->Set(newInteger(i),newString(names[i]));
	}

	returnObject(result);
}

RET SQLRCursor::getColumnName(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	const char	*result=sqlrcur(args)->getColumnName(
						toUint32(args[0]));

	returnString(result);
}

RET SQLRCursor::getColumnType(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	const char	*result=NULL;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnType(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnType(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnString(result);
}

RET SQLRCursor::getColumnLength(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	uint32_t	result=0;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnLength(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnLength(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnUint32(result);
}

RET SQLRCursor::getColumnPrecision(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	uint32_t	result=0;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnPrecision(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnPrecision(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnUint32(result);
}

RET SQLRCursor::getColumnScale(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	uint32_t	result=0;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnScale(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnScale(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnUint32(result);
}

RET SQLRCursor::getColumnIsNullable(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=false;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsNullable(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsNullable(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnBoolean(result);
}

RET SQLRCursor::getColumnIsPrimaryKey(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=false;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsPrimaryKey(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsPrimaryKey(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}


	returnBoolean(result);
}

RET SQLRCursor::getColumnIsUnique(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=false;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsUnique(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsUnique(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnBoolean(result);
}

RET SQLRCursor::getColumnIsPartOfKey(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=false;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsPartOfKey(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsPartOfKey(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnBoolean(result);
}

RET SQLRCursor::getColumnIsUnsigned(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=false;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsUnsigned(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsUnsigned(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnBoolean(result);
}

RET SQLRCursor::getColumnIsZeroFilled(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=false;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsZeroFilled(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsZeroFilled(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnBoolean(result);
}

RET SQLRCursor::getColumnIsBinary(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=false;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsBinary(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsBinary(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnBoolean(result);
}

RET SQLRCursor::getColumnIsAutoIncrement(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=false;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getColumnIsAutoIncrement(
						toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getColumnIsAutoIncrement(
						toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnBoolean(result);
}

RET SQLRCursor::getLongest(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	uint32_t	result=0;

	if (args[0]->IsNumber()) {
		result=sqlrcur(args)->getLongest(toUint32(args[0]));
	} else if (args[0]->IsString()) {
		result=sqlrcur(args)->getLongest(toString(args[0]));
	} else {
		throwInvalidArgumentType();
	}

	returnUint32(result);
}

RET SQLRCursor::suspendResultSet(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->suspendResultSet();

	returnVoid();
}

RET SQLRCursor::getResultSetId(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	uint16_t	result=sqlrcur(args)->getResultSetId();

	returnUint32(result);
}

RET SQLRCursor::resumeResultSet(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,1);

	bool	result=sqlrcur(args)->resumeResultSet(toUint32(args[0]));

	returnBoolean(result);
}

RET SQLRCursor::resumeCachedResultSet(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,2);

	bool	result=sqlrcur(args)->resumeCachedResultSet(
						toUint32(args[0]),
						toString(args[1]));

	returnBoolean(result);
}

RET SQLRCursor::closeResultSet(const ARGS &args) {

	initLocalScope();

	checkArgCount(args,0);

	sqlrcur(args)->closeResultSet();

	returnVoid();
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
