#include <sqlrelay/sqlrclient.h>
#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

using namespace v8;
using namespace node;



// deal with differences between major versions of node.js
#if NODE_MINOR_VERSION >= 12
	#define RET void
	#define ARGS FunctionCallbackInfo<Value>
	#define newFunctionTemplate(isolate,func) FunctionTemplate::New(isolate,func)
	#define newLocalFunction(isolate,func) Local<Function>::New(isolate,func)
	#define resetConstructor(constructor,isolate,tpl) constructor.Reset(isolate,tpl->GetFunction())
	#define returnObject(object) args.GetReturnValue().Set(object)
	#define returnBoolean(isolate,result) args.GetReturnValue().Set(Boolean::New(isolate,result))
#else
	#define RET Handle<Value>
	#define ARGS Arguments
	#define newFunctionTemplate(isolate,func) FunctionTemplate::New(func)
	#define newLocalFunction(isolate,func) Local<Function>::New(func)
	#define NewFromUtf8(isolate,str) New(str)
	#define resetConstructor(constructor,isolate,tpl)
	#define returnObject(object) return scope.Close(object)
	#define returnBoolean(isolate,result) return scope.Close(Boolean::New(result))
	#define scope(isolate) scope
#endif



// sqlrconnection wrapper declarations...
class SQLRConnection : public ObjectWrap {
	friend class SQLRCursor;
	public:
		static void	Init(Handle<Object> exports);
	private:
		explicit	SQLRConnection();
				~SQLRConnection();
		static RET	New(const ARGS &args);
		static RET	ping(const ARGS &args);

		static Persistent<Function>	constructor;

		static sqlrconnection *sqlrcon(const ARGS &args);
		sqlrconnection	*sqlrc;
};

Persistent<Function> SQLRConnection::constructor;



// sqlrcursor wrapper declarations...
class SQLRCursor : public ObjectWrap {
	public:
		static void	Init(Handle<Object> exports);
	private:
		explicit	SQLRCursor();
				~SQLRCursor();
		static RET	New(const ARGS &args);
		static RET	sendQuery(const ARGS &args);

		static Persistent<Function>	constructor;

		static sqlrcursor *sqlrcur(const ARGS &args);
		sqlrcursor	*sqlrc;
};

Persistent<Function> SQLRCursor::constructor;



// sqlrconnection wrapper methods...
void SQLRConnection::Init(Handle<Object> exports) {

	Isolate	*isolate=Isolate::GetCurrent();

	Local<FunctionTemplate>	tpl=newFunctionTemplate(isolate,New);
	tpl->SetClassName(String::NewFromUtf8(isolate,"SQLRConnection"));
	// internal field count is the number of non-static member variables
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	NODE_SET_PROTOTYPE_METHOD(tpl,"ping",ping);

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
		// invoked as constructor: new SQLRConnection(...)
		SQLRConnection	*obj=new SQLRConnection();
		obj->sqlrc=new sqlrconnection("localhost",9000,"",
							"test","test",0,1);
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

RET SQLRConnection::ping(const ARGS &args) {

	Isolate		*isolate=Isolate::GetCurrent();
	HandleScope	scope(isolate);

	bool	result=sqlrcon(args)->ping();

	returnBoolean(isolate,result);
}

sqlrconnection *SQLRConnection::sqlrcon(const ARGS &args) {
	return ObjectWrap::Unwrap<SQLRConnection>(args.Holder())->sqlrc;
}



// sqlrcursor wrapper methods...
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
		// invoked as constructor: new SQLRCursor(...)
		SQLRConnection	*sqlrcon=
			node::ObjectWrap::Unwrap<SQLRConnection>(
							args[0]->ToObject());
		SQLRCursor	*obj=new SQLRCursor();
		obj->sqlrc=new sqlrcursor(sqlrcon->sqlrc);
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

	bool	result=sqlrcur(args)->sendQuery("select 1");

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
