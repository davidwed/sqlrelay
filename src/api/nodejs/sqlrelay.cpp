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
	#define returnString(isolate,result) args.GetReturnValue().Set(String::NewFromUtf8(isolate,result))
	#define returnInteger(isolate,result) args.GetReturnValue().Set(Integer::New(isolate,result))
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
	#define returnString(isolate,result) return scope.Close(String::NewFromUtf8(result))
	#define returnInteger(isolate,result) return scope.Close(Integer::New(result))
	#define returnVoid(isolate) return scope.Close()
	#define scope(isolate) scope
	#define checkArgCount(args,isolate,count) if (args.Length()!=count) { throwWrongNumberOfArguments(isolate); returnBoolean(isolate,false); }
	#define NewFromUtf8(isolate,str) New(str)
#endif


// convenience macros
#define throwWrongNumberOfArguments(isolate) isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,"Wrong number of arguments")))
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
		static RET	sendQuery(const ARGS &args);

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

	checkArgCount(args,isolate,0);

	sqlrcon(args)->setConnectTimeout(args[0]->Int32Value(),
						args[1]->Int32Value());

	returnVoid(isolate);
}

RET SQLRConnection::setAuthenticationTimeout(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

	sqlrcon(args)->setAuthenticationTimeout(args[0]->Int32Value(),
						args[1]->Int32Value());

	returnVoid(isolate);
}

RET SQLRConnection::setResponseTimeout(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

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

	checkArgCount(args,isolate,0);

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

	checkArgCount(args,isolate,0);

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

	checkArgCount(args,isolate,0);

	sqlrcon(args)->setDebugFile(argToString(args[0]));

	returnVoid(isolate);
}

RET SQLRConnection::setClientInfo(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,0);

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

	NODE_SET_PROTOTYPE_METHOD(tpl,"sendQuery",sendQuery);

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

RET SQLRCursor::sendQuery(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	checkArgCount(args,isolate,1);

	bool	result=sqlrcur(args)->sendQuery(argToString(args[0]));

	returnBoolean(isolate,result);
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
