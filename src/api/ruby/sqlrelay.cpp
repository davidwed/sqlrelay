// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#ifdef _WIN32
	#include <rudiments/private/winsock.h>
	#define DLEXPORT __declspec(dllexport)

	// works around "The C++ Standard Library forbids macroizing keywords.""
	// with VC2012 and up
	#define _XKEYCHECK_H
#else
	#define DLEXPORT
#endif

#ifndef STR2CSTR
	#define STR2CSTR(v) StringValuePtr(v)
#endif

#include <rudiments/bytestring.h>
#include <ruby.h>
#include "../c++/sqlrelay/sqlrclient.h"

#include "rubyincludes.h"

#ifdef HAVE_RUBY_THREAD_H
#include <ruby/thread.h>
#endif
struct params {
	sqlrconnection	*sqlrcon;
	sqlrcursor	*sqlrcur;
	VALUE		one;
	VALUE		two;
	VALUE		three;
	VALUE		four;
	VALUE		five;
	VALUE		six;
	VALUE		seven;
};

#ifdef HAVE_RUBY_THREAD_H
	#define	CALL(function,prms) \
		rb_thread_call_without_gvl((void *(*)(void *))function,&prms,NULL,NULL);
	#define	RCALL(resulttype,function,prms) \
		result=(resulttype)rb_thread_call_without_gvl((void *(*)(void *))function,&prms,NULL,NULL);
	#define	RCALLNAME(resulttype,function,prms) \
		name=(resulttype)rb_thread_call_without_gvl((void *(*)(void *))function,&prms,NULL,NULL);
	#define	RCALLCOPY(resulttype,function,prms) \
		bytestring::copy(&result,rb_thread_call_without_gvl((void *(*)(void *))function,&prms,NULL,NULL),sizeof(resulttype));
	#define	RLCALL(restype,resfunc,resprms,lentype,lenfunc,lenprms) \
		result=(restype)rb_thread_call_without_gvl((void *(*)(void *))resfunc,&resprms,NULL,NULL); \
		length=(lentype)rb_thread_call_without_gvl((void *(*)(void *))lenfunc,&lenprms,NULL,NULL);
#else
	#define	CALL(function,prms) \
		function(&prms);
	#define	RCALL(resulttype,function,prms) \
		result=function(&prms);
#endif
#define CON(psqlrcon,function) \
	struct params prms; \
	prms.sqlrcon=psqlrcon; \
	CALL(function,prms)
#define RCON(psqlrcon,resulttype,function) \
	struct params prms; \
	prms.sqlrcon=psqlrcon; \
	RCALL(resulttype,function,prms)
#define CON1(psqlrcon,function,pone) \
	struct params prms; \
	prms.sqlrcon=psqlrcon; \
	prms.one=pone; \
	CALL(function,prms)
#define RCON1(psqlrcon,resulttype,function,pone) \
	struct params prms; \
	prms.sqlrcon=psqlrcon; \
	prms.one=pone; \
	RCALL(resulttype,function,prms)
#define CON2(psqlrcon,function,pone,ptwo) \
	struct params prms; \
	prms.sqlrcon=psqlrcon; \
	prms.one=pone; \
	prms.two=ptwo; \
	CALL(function,prms)
#define RCON2(psqlrcon,resulttype,function,pone,ptwo) \
	struct params prms; \
	prms.sqlrcon=psqlrcon; \
	prms.one=pone; \
	prms.two=ptwo; \
	RCALL(resulttype,function,prms)
#define CON3(psqlrcon,function,pone,ptwo,pthree) \
	struct params prms; \
	prms.sqlrcon=psqlrcon; \
	prms.one=pone; \
	prms.two=ptwo; \
	prms.three=pthree; \
	CALL(function,prms)
#define CON7(psqlrcon,function,pone,ptwo,pthree,pfour,pfive,psix,pseven) \
	struct params prms; \
	prms.sqlrcon=psqlrcon; \
	prms.one=pone; \
	prms.two=ptwo; \
	prms.three=pthree; \
	prms.four=pfour; \
	prms.five=pfive; \
	prms.six=psix; \
	prms.seven=pseven; \
	CALL(function,prms)

#define CUR(psqlrcur,function) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	CALL(function,prms)
#define RCUR(psqlrcur,resulttype,function) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	RCALL(resulttype,function,prms)
#define CUR1(psqlrcur,function,pone) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	CALL(function,prms)
#define RCUR1(psqlrcur,resulttype,function,pone) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	RCALL(resulttype,function,prms)
#define RCUR1NAME(psqlrcur,resulttype,function,pone) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	RCALLNAME(resulttype,function,prms)
#define RCUR1COPY(psqlrcur,resulttype,function,pone) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	RCALLCOPY(resulttype,function,prms)
#define RLCUR1(psqlrcur,restype,resfunc,respone,lentype,lenfunc,lenone) \
	struct params resprms; \
	resprms.sqlrcur=psqlrcur; \
	resprms.one=respone; \
	struct params lenprms; \
	lenprms.sqlrcur=psqlrcur; \
	lenprms.one=lenone; \
	RLCALL(restype,resfunc,resprms,lentype,lenfunc,lenprms);
#define CUR2(psqlrcur,function,pone,ptwo) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	prms.two=ptwo; \
	CALL(function,prms)
#define RCUR2(psqlrcur,resulttype,function,pone,ptwo) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	prms.two=ptwo; \
	RCALL(resulttype,function,prms)
#define RCUR2COPY(psqlrcur,resulttype,function,pone,ptwo) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	prms.two=ptwo; \
	RCALLCOPY(resulttype,function,prms)
#define RLCUR2(psqlrcur,restype,resfunc,respone,resptwo,lentype,lenfunc,lenone,lentwo) \
	struct params resprms; \
	resprms.sqlrcur=psqlrcur; \
	resprms.one=respone; \
	resprms.two=resptwo; \
	struct params lenprms; \
	lenprms.sqlrcur=psqlrcur; \
	lenprms.one=lenone; \
	lenprms.two=lentwo; \
	RLCALL(restype,resfunc,resprms,lentype,lenfunc,lenprms);
#define CUR3(psqlrcur,function,pone,ptwo,pthree) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	prms.two=ptwo; \
	prms.three=pthree; \
	CALL(function,prms)
#define CUR4(psqlrcur,function,pone,ptwo,pthree,pfour) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	prms.two=ptwo; \
	prms.three=pthree; \
	prms.four=pfour; \
	CALL(function,prms)
#define CUR7(psqlrcur,function,pone,ptwo,pthree,pfour,pfive,psix,pseven) \
	struct params prms; \
	prms.sqlrcur=psqlrcur; \
	prms.one=pone; \
	prms.two=ptwo; \
	prms.three=pthree; \
	prms.four=pfour; \
	prms.five=pfive; \
	prms.six=psix; \
	prms.seven=pseven; \
	CALL(function,prms)



extern "C" {

struct rb_blocking_region_buffer;
extern void rb_thread_blocking_region_end(struct rb_blocking_region_buffer *);
extern struct rb_blocking_region_buffer *rb_thread_blocking_region_begin();

// sqlrconnection methods
static void sqlrcon_free(void *sqlrcon) {
	delete (sqlrconnection *)sqlrcon;
}

/**
 *  call-seq:
 *  new(host, port, socket, user, password, retrytime, tries)
 *
 *  Initiates a connection to "host" on "port" or to the unix "socket" on
 *  the local machine and auths with "user" and "password".  Failed
 *  connections will be retried for "tries" times, waiting "retrytime" seconds
 *  between each try.  If "tries" is 0 then retries will continue forever.  If
 *  "retrytime" is 0 then retries will be attempted on a default interval.  If
 *  the "socket" parameter is nether nil nor "" then an attempt will be made to
 *  connect through it before attempting to connect to "host" on "port".  If it
 *  is nil or "" then no attempt will be made to connect through the socket.*/
static VALUE sqlrcon_new(VALUE self, VALUE host, VALUE port, VALUE socket,
				VALUE user, VALUE password, 
				VALUE retrytime, VALUE tries) {
	const char	*socketstr;
	if (socket==Qnil) {
		socketstr="";
	} else {
		socketstr=STR2CSTR(socket);
	}
	sqlrconnection	*sqlrcon=new sqlrconnection(STR2CSTR(host),
							NUM2INT(port),
							socketstr,
							STR2CSTR(user),
							STR2CSTR(password),
							NUM2INT(retrytime),
							NUM2INT(tries),
							true);
	return Data_Wrap_Struct(self,0,sqlrcon_free,(void *)sqlrcon);
}

static void setConnectTimeout(params *p) {
	p->sqlrcon->setConnectTimeout(NUM2INT(p->one),NUM2INT(p->two));
}
/**
 *  call-seq:
 *  setConnectTimeout(timeoutsec,timeoutusec)
 *
 *  Sets the server connect timeout in seconds and
 *  milliseconds.  Setting either parameter to -1 disables the
 *  timeout.  You can also set this timeout using the
 *  SQLR_CLIENT_CONNECT_TIMEOUT environment variable. */
static VALUE sqlrcon_setConnectTimeout(VALUE self,
				VALUE timeoutsec, VALUE timeoutusec) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON2(sqlrcon,setConnectTimeout,timeoutsec,timeoutusec)
	return Qnil;
}

static void setAuthenticationTimeout(params *p) {
	p->sqlrcon->setAuthenticationTimeout(NUM2INT(p->one),NUM2INT(p->two));
}
/**
 *  call-seq:
 *  setAuthenticationTimeout(timeoutsec,timeoutusec)
 *
 *  Sets the authentication timeout in seconds and
 *  milliseconds.  Setting either parameter to -1 disables the
 *  timeout.   You can also set this timeout using the
 *  SQLR_CLIENT_AUTHENTICATION_TIMEOUT environment variable. */
static VALUE sqlrcon_setAuthenticationTimeout(VALUE self,
				VALUE timeoutsec, VALUE timeoutusec) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON2(sqlrcon,setAuthenticationTimeout,timeoutsec,timeoutusec)
	return Qnil;
}

static void setResponseTimeout(params *p) {
	p->sqlrcon->setResponseTimeout(NUM2INT(p->one),NUM2INT(p->two));
}
/**
 *  call-seq:
 *  setResponseTimeout(timeoutsec,timeoutusec)
 *
 *  Sets the response timeout (for queries, commits, rollbacks,
 *  pings, etc.) in seconds and milliseconds.  Setting either
 *  parameter to -1 disables the timeout.  You can also set
 *  this timeout using the SQLR_CLIENT_RESPONSE_TIMEOUT
 *  environment variable. */
static VALUE sqlrcon_setResponseTimeout(VALUE self,
				VALUE timeoutsec, VALUE timeoutusec) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON2(sqlrcon,setResponseTimeout,timeoutsec,timeoutusec)
	return Qnil;
}

static void setBindVariableDelimiters(params *p) {
	p->sqlrcon->setBindVariableDelimiters(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  setBindVariablesDelimiters(delimiters)
 *
 *  Sets which delimiters are used to identify bind variables
 *  in countBindVariables() and validateBinds().  Valid
 *  delimiters include ?,:,@, and $.  Defaults to "?:@$" */
static VALUE sqlrcon_setBindVariableDelimiters(VALUE self, VALUE delimiters) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON1(sqlrcon,setBindVariableDelimiters,delimiters);
	return Qnil;
}

static bool getBindVariableDelimiterQuestionMarkSupported(params *p) {
	return p->sqlrcon->getBindVariableDelimiterQuestionMarkSupported();
}
/** Returns true if question marks (?) are considered to be
 *  valid bind variable delimiters. */
static VALUE sqlrcon_getBindVariableDelimiterQuestionMarkSupported(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,getBindVariableDelimiterQuestionMarkSupported);
	return INT2NUM(result);
}

static bool getBindVariableDelimiterColonSupported(params *p) {
	return p->sqlrcon->getBindVariableDelimiterColonSupported();
}
/** Returns true if colons (:) are considered to be
 *  valid bind variable delimiters. */
static VALUE sqlrcon_getBindVariableDelimiterColonSupported(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,getBindVariableDelimiterColonSupported);
	return INT2NUM(result);
}

static bool getBindVariableDelimiterAtSignSupported(params *p) {
	return p->sqlrcon->getBindVariableDelimiterAtSignSupported();
}
/** Returns true if at-signs (@) are considered to be
 *  valid bind variable delimiters. */
static VALUE sqlrcon_getBindVariableDelimiterAtSignSupported(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,getBindVariableDelimiterAtSignSupported);
	return INT2NUM(result);
}

static bool getBindVariableDelimiterDollarSignSupported(params *p) {
	return p->sqlrcon->getBindVariableDelimiterDollarSignSupported();
}
/** Returns true if dollar signs ($) are considered to be
 *  valid bind variable delimiters. */
static VALUE sqlrcon_getBindVariableDelimiterDollarSignSupported(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,getBindVariableDelimiterDollarSignSupported);
	return INT2NUM(result);
}

static void enableKerberos(params *p) {
	p->sqlrcon->enableKerberos(STR2CSTR(p->one),
					STR2CSTR(p->two),
					STR2CSTR(p->three));
}
/** Enables Kerberos authentication and encryption.
 *
 *  "service" indicates the Kerberos service name of the
 *  SQL Relay server.  If left empty or NULL then the service
 *  name "sqlrelay" will be used. "sqlrelay" is the default
 *  service name of the SQL Relay server.  Note that on Windows
 *  platforms the service name must be fully qualified,
 *  including the host and realm name.  For example:
 *  "sqlrelay/sqlrserver.firstworks.com@AD.FIRSTWORKS.COM".
 *
 *  "mech" indicates the specific Kerberos mechanism to use.
 *  On Linux/Unix platforms, this should be a string
 *  representation of the mechnaism's OID, such as:
 *      { 1 2 840 113554 1 2 2 }
 *  On Windows platforms, this should be a string like:
 *      Kerberos
 *  If left empty or NULL then the default mechanism will be
 *  used.  Only set this if you know that you have a good
 *  reason to.
 *
 *  "flags" indicates what Kerberos flags to use.  Multiple
 *  flags may be specified, separated by commas.  If left
 *  empty or NULL then a defalt set of flags will be used.
 *  Only set this if you know that you have a good reason to.
 *
 *  Valid flags include:
 *   * GSS_C_MUTUAL_FLAG
 *   * GSS_C_REPLAY_FLAG
 *   * GSS_C_SEQUENCE_FLAG
 *   * GSS_C_CONF_FLAG
 *   * GSS_C_INTEG_FLAG
 *
 *  For a full list of flags, consult the GSSAPI documentation,
 *  though note that only the flags listed above are supported
 *  on Windows. */
static VALUE sqlrcon_enableKerberos(VALUE self,
				VALUE service, VALUE mech, VALUE flags) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON3(sqlrcon,enableKerberos,service,mech,flags);
	return Qnil;
}


static void enableTls(params *p) {
	p->sqlrcon->enableTls(STR2CSTR(p->one),
				STR2CSTR(p->two),
				STR2CSTR(p->three),
				STR2CSTR(p->four),
				STR2CSTR(p->five),
				STR2CSTR(p->six),
				NUM2INT(p->seven));
}
/** Enables TLS/SSL encryption, and optionally authentication.
 *
 *  "version" specifies the TLS/SSL protocol version that the
 *  client will attempt to use.  Valid values include SSL2,
 *  SSL3, TLS1, TLS1.1, TLS1.2 or any more recent version of
 *  TLS, as supported by and enabled in the underlying TLS/SSL
 *  library.  If left blank or empty then the highest supported
 *  version will be negotiated.
 *
 *  "cert" is the file name of the certificate chain file to
 *  send to the SQL Relay server.  This is only necessary if
 *  the SQL Relay server is configured to authenticate and
 *  authorize clients by certificate.
 *
 *  If "cert" contains a password-protected private key, then
 *  "password" may be supplied to access it.  If the private
 *  key is not password-protected, then this argument is
 *  ignored, and may be left empty or NULL.
 *
 *  "ciphers" is a list of ciphers to allow.  Ciphers may be
 *  separated by spaces, commas, or colons.  If "ciphers" is
 *  empty or NULL then a default set is used.  Only set this if
 *  you know that you have a good reason to.
 *
 *  For a list of valid ciphers on Linux/Unix platforms, see:
 *      man ciphers
 *
 *  For a list of valid ciphers on Windows platforms, see:
 *      https://msdn.microsoft.com/en-us/library/windows/desktop/aa375549%28v=vs.85%29.aspx
 *  On Windows platforms, the ciphers (alg_id's) should omit
 *  CALG_ and may be given with underscores or dashes.
 *  For example: 3DES_112
 *
 *  "validate" indicates whether to validate the SQL Relay's
 *  server certificate, and may be set to one of the following:
 *      "no" - Don't validate the server's certificate.
 *      "ca" - Validate that the server's certificate was
 *             signed by a trusted certificate authority.
 *      "ca+host" - Perform "ca" validation and also validate
 *             that one of the subject altenate names (or the
 *             common name if no SANs are present) in the
 *             certificate matches the host parameter.
 *             (Falls back to "ca" validation when a unix
 *             socket is used.)
 *      "ca+domain" - Perform "ca" validation and also validate
 *             that the domain name of one of the subject
 *             alternate names (or the common name if no SANs
 *             are present) in the certificate matches the
 *             domain name of the host parameter.  (Falls back
 *             to "ca" validation when a unix socket is used.)
 *
 *  "ca" is the location of a certificate authority file to
 *  use, in addition to the system's root certificates, when
 *  validating the SQL Relay server's certificate.  This is
 *  useful if the SQL Relay server's certificate is self-signed.
 *
 *  On Windows, "ca" must be a file name.
 *
 *  On non-Windows systems, "ca" can be either a file or
 *  directory name.  If it is a directory name, then all
 *  certificate authority files found in that directory will be
 *  used.  If it a file name, then only that file will be used.
 *
 *
 *  Note that the supported "cert" and "ca" file formats may
 *  vary between platforms.  A variety of file formats are
 *  generally supported on Linux/Unix platfoms (.pem, .pfx,
 *  etc.) but only the .pfx format is currently supported on
 *  Windows. */
static VALUE sqlrcon_enableTls(VALUE self,
				VALUE version, VALUE cert, VALUE password,
				VALUE ciphers, VALUE validate, VALUE ca,
				VALUE depth) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON7(sqlrcon,enableTls,version,cert,password,ciphers,validate,ca,depth);
	return Qnil;
}

static void disableEncryption(params *p) {
	p->sqlrcon->disableEncryption();
}
/** Disables encryption. */
static VALUE sqlrcon_disableEncryption(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON(sqlrcon,disableEncryption);
	return Qnil;
}

static void endSession(params *p) {
	p->sqlrcon->endSession();
}
/** Ends the session. */
static VALUE sqlrcon_endSession(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON(sqlrcon,endSession);
	return Qnil;
}

static bool suspendSession(params *p) {
	return p->sqlrcon->suspendSession();
}
/** Disconnects this connection from the current session but leaves the session
 *  open so that another connection can connect to it using
 *  sqlrcon_resumeSession(). */
static VALUE sqlrcon_suspendSession(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,suspendSession);
	return INT2NUM(result);
}

static uint16_t getConnectionPort(params *p) {
	return p->sqlrcon->getConnectionPort();
}
/** Returns the inet port that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession(). */
static VALUE sqlrcon_getConnectionPort(VALUE self) {
	sqlrconnection	*sqlrcon;
	uint16_t	result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,uint64_t,getConnectionPort);
	return INT2NUM(result);
}

static const char *getConnectionSocket(params *p) {
	return p->sqlrcon->getConnectionSocket();
}
/** Returns the unix socket that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession(). */
static VALUE sqlrcon_getConnectionSocket(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,getConnectionSocket);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static bool resumeSession(params *p) {
	return p->sqlrcon->resumeSession(NUM2INT(p->one),STR2CSTR(p->two));
}
/**
 *  call-seq:
 *  resumeSession(port,socket)
 *
 *  Resumes a session previously left open using sqlrcon_suspendSession().
 *  Returns 1 on success and 0 on failure. */
static VALUE sqlrcon_resumeSession(VALUE self, VALUE port, VALUE socket) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON2(sqlrcon,bool,resumeSession,port,socket);
	return INT2NUM(result);
}

static bool ping(params *p) {
	return p->sqlrcon->ping();
}
/** Returns 1 if the database is up and 0 if it's down. */
static VALUE sqlrcon_ping(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,ping);
	return INT2NUM(result);
}

static const char *identify(params *p) {
	return p->sqlrcon->identify();
}
/** Returns the type of database: oracle, postgresql, mysql, etc. */
static VALUE sqlrcon_identify(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,identify);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static const char *dbVersion(params *p) {
	return p->sqlrcon->dbVersion();
}
/** Returns the version of the database */
static VALUE sqlrcon_dbVersion(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,dbVersion);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static const char *dbHostName(params *p) {
	return p->sqlrcon->dbHostName();
}
/** Returns the host name of the database */
static VALUE sqlrcon_dbHostName(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,dbHostName);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static const char *dbIpAddress(params *p) {
	return p->sqlrcon->dbIpAddress();
}
/** Returns the ip address of the database */
static VALUE sqlrcon_dbIpAddress(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,dbIpAddress);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static const char *serverVersion(params *p) {
	return p->sqlrcon->serverVersion();
}
/** Returns the version of the sqlrelay server software. */
static VALUE sqlrcon_serverVersion(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,serverVersion);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static const char *clientVersion(params *p) {
	return p->sqlrcon->clientVersion();
}
/** Returns the version of the sqlrelay client software. */
static VALUE sqlrcon_clientVersion(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,clientVersion);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static const char *bindFormat(params *p) {
	return p->sqlrcon->bindFormat();
}
/** Returns a string representing the format
 *  of the bind variables used in the db. */
static VALUE sqlrcon_bindFormat(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,bindFormat);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static bool selectDatabase(params *p) {
	return p->sqlrcon->selectDatabase(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  selectDatabase(database)
 *
 *  Sets the current database/schema to "database" */
static VALUE sqlrcon_selectDatabase(VALUE self, VALUE db) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON1(sqlrcon,bool,selectDatabase,db);
	return INT2NUM(result);
}

static const char *getCurrentDatabase(params *p) {
	return p->sqlrcon->getCurrentDatabase();
}
/** Returns the database/schema that is currently in use. */
static VALUE sqlrcon_getCurrentDatabase(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,getCurrentDatabase);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static uint64_t getLastInsertId(params *p) {
	return p->sqlrcon->getLastInsertId();
}
/** Returns the value of the autoincrement column for the last insert */
static VALUE sqlrcon_getLastInsertId(VALUE self) {
	sqlrconnection	*sqlrcon;
	uint64_t	result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,uint64_t,getLastInsertId);
	return INT2NUM(result);
}

static bool autoCommitOn(params *p) {
	return p->sqlrcon->autoCommitOn();
}
/** Instructs the database to perform a commit after every successful query. */
static VALUE sqlrcon_autoCommitOn(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,autoCommitOn);
	return INT2NUM(result);
}

static bool autoCommitOff(params *p) {
	return p->sqlrcon->autoCommitOff();
}
/** Instructs the database to wait for the client to tell it when to commit. */
static VALUE sqlrcon_autoCommitOff(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,autoCommitOff);
	return INT2NUM(result);
}

static bool begin(params *p) {
	return p->sqlrcon->begin();
}
/** Begins a transaction.  Returns true if the begin
 *  succeeded, false if it failed.  If the database
 *  automatically begins a new transaction when a
 *  commit or rollback is issued then this doesn't
 *  do anything unless SQL Relay is faking transaction
 *  blocks. */
static VALUE sqlrcon_begin(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,begin);
	return INT2NUM(result);
}

static bool commit(params *p) {
	return p->sqlrcon->commit();
}
/** Issues a commit.  Returns true if the commit succeeded,
 *  false if it failed. */
static VALUE sqlrcon_commit(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,commit);
	return INT2NUM(result);
}

static bool rollback(params *p) {
	return p->sqlrcon->rollback();
}
/** Issues a rollback.  Returns true if the rollback succeeded,
 *  false if it failed. */
static VALUE sqlrcon_rollback(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,rollback);
	return INT2NUM(result);
}

static const char *conErrorMessage(params *p) {
	return p->sqlrcon->errorMessage();
}
/** If an operation failed and generated an error, the error message is
 *  available here.  If there is no error then this method returns nil */
static VALUE sqlrcon_errorMessage(VALUE self) {
	sqlrconnection *sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,conErrorMessage);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static int64_t conErrorNumber(params *p) {
	return p->sqlrcon->errorNumber();
}
/** If an operation failed and generated an error, the error number is
 *  available here.  If there is no error then this method returns 0. */
static VALUE sqlrcon_errorNumber(VALUE self) {
	sqlrconnection *sqlrcon;
	int64_t		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,int64_t,conErrorNumber);
	return INT2NUM(result);
}

static void debugOn(params *p) {
	p->sqlrcon->debugOn();
}
/** Causes verbose debugging information to be sent to standard output.
 *  Another way to do this is to start a query with "-- debug\n".  Yet
 *  another way is to set the environment variable SQLR_CLIENT_DEBUG
 *  to "ON" */
static VALUE sqlrcon_debugOn(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON(sqlrcon,debugOn);
	return Qnil;
}

static void debugOff(params *p) {
	p->sqlrcon->debugOff();
}
/** Turns debugging off. */
static VALUE sqlrcon_debugOff(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON(sqlrcon,debugOff);
	return Qnil;
}

static bool getDebug(params *p) {
	return p->sqlrcon->getDebug();
}
/** Returns 0 if debugging is off and 1 if debugging is on. */
static VALUE sqlrcon_getDebug(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool		result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,bool,getDebug);
	return INT2NUM(result);
}

static void setDebugFile(params *p) {
	p->sqlrcon->setDebugFile(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  setDebugFile(filename)
 * 
 *  Allows you to specify a file to write debug to.
 *  Setting "filename" to NULL or an empty string causes debug
 *  to be written to standard output (the default). */
static VALUE sqlrcon_setDebugFile(VALUE self, VALUE filename) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON1(sqlrcon,setDebugFile,filename);
	return Qnil;
}

static void setClientInfo(params *p) {
	p->sqlrcon->setClientInfo(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  setClientInfo(clientinfo)
 * 
 *  Allows you to set a string that will be passed to the server and ultimately
 *  included in server-side logging along with queries that were run by this
 *  instance of the client. */
static VALUE sqlrcon_setClientInfo(VALUE self, VALUE clientinfo) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	CON1(sqlrcon,setClientInfo,clientinfo);
	return Qnil;
}

static const char *getClientInfo(params *p) {
	return p->sqlrcon->getClientInfo();
}
/** Returns the string that was set by setClientInfo(). */
static VALUE sqlrcon_getClientInfo(VALUE self) {
	sqlrconnection	*sqlrcon;
	const char	*result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	RCON(sqlrcon,const char *,getClientInfo);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}



VALUE csqlrconnection;

/** */
void Init_SQLRConnection() {
	csqlrconnection=rb_define_class("SQLRConnection", rb_cObject);
	rb_define_singleton_method(csqlrconnection,"new",
				(CAST)sqlrcon_new,7);
	rb_define_method(csqlrconnection,"setConnectTimeout",
				(CAST)sqlrcon_setConnectTimeout,2);
	rb_define_method(csqlrconnection,"setAuthenticationTimeout",
				(CAST)sqlrcon_setAuthenticationTimeout,2);
	rb_define_method(csqlrconnection,"setResponseTimeout",
				(CAST)sqlrcon_setResponseTimeout,2);
	rb_define_method(csqlrconnection,"setBindVariableDelimiters",
				(CAST)sqlrcon_setBindVariableDelimiters,1);
	rb_define_method(csqlrconnection,
		"getBindVariableDelimiterQuestionMarkSupported",
		(CAST)sqlrcon_getBindVariableDelimiterQuestionMarkSupported,0);
	rb_define_method(csqlrconnection,
		"getBindVariableDelimiterColonSupported",
		(CAST)sqlrcon_getBindVariableDelimiterColonSupported,0);
	rb_define_method(csqlrconnection,
		"getBindVariableDelimiterAtSignSupported",
		(CAST)sqlrcon_getBindVariableDelimiterAtSignSupported,0);
	rb_define_method(csqlrconnection,
		"getBindVariableDelimiterDollarSignSupported",
		(CAST)sqlrcon_getBindVariableDelimiterDollarSignSupported,0);
	rb_define_method(csqlrconnection,"enableKerberos",
				(CAST)sqlrcon_enableKerberos,3);
	rb_define_method(csqlrconnection,"enableTls",
				(CAST)sqlrcon_enableTls,7);
	rb_define_method(csqlrconnection,"disableEncryption",
				(CAST)sqlrcon_disableEncryption,0);
	rb_define_method(csqlrconnection,"endSession",
				(CAST)sqlrcon_endSession,0);
	rb_define_method(csqlrconnection,"suspendSession",
				(CAST)sqlrcon_suspendSession,0);
	rb_define_method(csqlrconnection,"getConnectionPort",
				(CAST)sqlrcon_getConnectionPort,0);
	rb_define_method(csqlrconnection,"getConnectionSocket",
				(CAST)sqlrcon_getConnectionSocket,0);
	rb_define_method(csqlrconnection,"resumeSession",
				(CAST)sqlrcon_resumeSession,2);
	rb_define_method(csqlrconnection,"ping",
				(CAST)sqlrcon_ping,0);
	rb_define_method(csqlrconnection,"identify",
				(CAST)sqlrcon_identify,0);
	rb_define_method(csqlrconnection,"dbVersion",
				(CAST)sqlrcon_dbVersion,0);
	rb_define_method(csqlrconnection,"dbHostName",
				(CAST)sqlrcon_dbHostName,0);
	rb_define_method(csqlrconnection,"dbIpAddress",
				(CAST)sqlrcon_dbIpAddress,0);
	rb_define_method(csqlrconnection,"serverVersion",
				(CAST)sqlrcon_serverVersion,0);
	rb_define_method(csqlrconnection,"clientVersion",
				(CAST)sqlrcon_clientVersion,0);
	rb_define_method(csqlrconnection,"bindFormat",
				(CAST)sqlrcon_bindFormat,0);
	rb_define_method(csqlrconnection,"selectDatabase",
				(CAST)sqlrcon_selectDatabase,1);
	rb_define_method(csqlrconnection,"getCurrentDatabase",
				(CAST)sqlrcon_getCurrentDatabase,0);
	rb_define_method(csqlrconnection,"getLastInsertId",
				(CAST)sqlrcon_getLastInsertId,0);
	rb_define_method(csqlrconnection,"autoCommitOn",
				(CAST)sqlrcon_autoCommitOn,0);
	rb_define_method(csqlrconnection,"autoCommitOff",
				(CAST)sqlrcon_autoCommitOff,0);
	rb_define_method(csqlrconnection,"begin",
				(CAST)sqlrcon_begin,0);
	rb_define_method(csqlrconnection,"commit",
				(CAST)sqlrcon_commit,0);
	rb_define_method(csqlrconnection,"rollback",
				(CAST)sqlrcon_rollback,0);
	rb_define_method(csqlrconnection,"errorMessage",
				(CAST)sqlrcon_errorMessage,0);
	rb_define_method(csqlrconnection,"errorNumber",
				(CAST)sqlrcon_errorNumber,0);
	rb_define_method(csqlrconnection,"debugOn",
				(CAST)sqlrcon_debugOn,0);
	rb_define_method(csqlrconnection,"debugOff",
				(CAST)sqlrcon_debugOff,0);
	rb_define_method(csqlrconnection,"getDebug",
				(CAST)sqlrcon_getDebug,0);
	rb_define_method(csqlrconnection,"setDebugFile",
				(CAST)sqlrcon_setDebugFile,1);
	rb_define_method(csqlrconnection,"setClientInfo",
				(CAST)sqlrcon_setClientInfo,1);
	rb_define_method(csqlrconnection,"getClientInfo",
				(CAST)sqlrcon_getClientInfo,0);
}



// sqlrcursor methods
VALUE csqlrcursor;

static void sqlrcur_free(void *sqlrcur) {
	delete (sqlrcursor *)sqlrcur;
}

/**
 *  call-seq:
 *  new(connection)
 *
 *  Creates a cursor to run queries and fetch
 *  result sets using connection "connection" */
static VALUE sqlrcur_new(VALUE self, VALUE connection) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(connection,sqlrconnection,sqlrcon);
	sqlrcursor	*sqlrcur=new sqlrcursor(sqlrcon,true);
	return Data_Wrap_Struct(self,0,sqlrcur_free,(void *)sqlrcur);
}

static void setResultSetBufferSize(params *p) {
	p->sqlrcur->setResultSetBufferSize(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  setResultSetBufferSize(rows)
 *
 *  Sets the number of rows of the result set to buffer at a time.
 *  0 (the default) means buffer the entire result set. */
static VALUE sqlrcur_setResultSetBufferSize(VALUE self, VALUE rows) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR1(sqlrcur,setResultSetBufferSize,rows);
	return Qnil;
}

static uint64_t getResultSetBufferSize(params *p) {
	return p->sqlrcur->getResultSetBufferSize();
}
/** Returns the number of result set rows that will be buffered at a time or
 *  0 for the entire result set. */
static VALUE sqlrcur_getResultSetBufferSize(VALUE self) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,uint64_t,getResultSetBufferSize);
	return INT2NUM(result);
}

static void dontGetColumnInfo(params *p) {
	p->sqlrcur->dontGetColumnInfo();
}
/** Tells the server not to send any column info (names, types, sizes).  If
 *  you don't need that info, you should call this function to improve
 *  performance. */
static VALUE sqlrcur_dontGetColumnInfo(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,dontGetColumnInfo);
	return Qnil;
}

static void getColumnInfo(params *p) {
	p->sqlrcur->getColumnInfo();
}
/** Tells the server to send column info. */
static VALUE sqlrcur_getColumnInfo(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,getColumnInfo);
	return Qnil;
}

static void mixedCaseColumnNames(params *p) {
	p->sqlrcur->mixedCaseColumnNames();
}
/** Columns names are returned in the same case as they are defined in the
 *  database.  This is the default. */
static VALUE sqlrcur_mixedCaseColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,mixedCaseColumnNames);
	return Qnil;
}

static void upperCaseColumnNames(params *p) {
	p->sqlrcur->upperCaseColumnNames();
}
/** Columns names are converted to upper case. */
static VALUE sqlrcur_upperCaseColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,upperCaseColumnNames);
	return Qnil;
}

static void lowerCaseColumnNames(params *p) {
	p->sqlrcur->lowerCaseColumnNames();
}
/** Columns names are converted to lower case. */
static VALUE sqlrcur_lowerCaseColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,lowerCaseColumnNames);
	return Qnil;
}

static void cacheToFile(params *p) {
	p->sqlrcur->cacheToFile(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  cacheToFile(filename)
 *
 *  Sets query caching on.  Future queries will be cached to the
 *  file "filename".
 * 
 *  A default time-to-live of 10 minutes is also set.
 * 
 *  Note that once cacheToFile() is called, the result sets of all
 *  future queries will be cached to that file until another call to
 *  cacheToFile() changes which file to cache to or a call to
 *  cacheOff() turns off caching. */
static VALUE sqlrcur_cacheToFile(VALUE self, VALUE filename) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR1(sqlrcur,cacheToFile,filename);
	return Qnil;
}

static void setCacheTtl(params *p) {
	p->sqlrcur->setCacheTtl(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  setCacheTtl(ttl)
 *
 *  Sets the time-to-live for cached result sets. The sqlr-cachemanger will
 *  remove each cached result set "ttl" seconds after it's created, provided
 *  it's scanning the directory containing the cache files. */
static VALUE sqlrcur_setCacheTtl(VALUE self, VALUE ttl) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR1(sqlrcur,setCacheTtl,ttl);
	return Qnil;
}

static const char *getCacheFileName(params *p) {
	return p->sqlrcur->getCacheFileName();
}
/** Returns the name of the file containing
 *  the most recently cached result set. */
static VALUE sqlrcur_getCacheFileName(VALUE self) {
	sqlrcursor	*sqlrcur;
	const char	*result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,const char *,getCacheFileName);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static void cacheOff(params *p) {
	p->sqlrcur->cacheOff();
}
/** Sets query caching off. */
static VALUE sqlrcur_cacheOff(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,cacheOff);
	return Qnil;
}

static bool getDatabaseList(params *p) {
	return p->sqlrcur->getDatabaseList(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  getDatabaseList(wild)
 *
 *  Sends a query that returns a list of databases/schemas matching "wild".
 *  If wild is empty or nil then a list of all databases/schemas will be
 *  returned. */
static VALUE sqlrcur_getDatabaseList(VALUE self, VALUE wild) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,bool,getDatabaseList,wild);
	return INT2NUM(result);
}

static bool getTableList(params *p) {
	return p->sqlrcur->getTableList(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  getTableList(wild)
 *
 *  Sends a query that returns a list of tables matching "wild".  If wild is
 *  empty or nil then a list of all tables will be returned. */
static VALUE sqlrcur_getTableList(VALUE self, VALUE wild) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,bool,getTableList,wild);
	return INT2NUM(result);
}

static bool getColumnList(params *p) {
	return p->sqlrcur->getColumnList(STR2CSTR(p->one),STR2CSTR(p->two));
}
/**
 *  call-seq:
 *  getColumnList(table,wild)
 *
 *  Sends a query that returns a list of columns in the table specified by the
 *  "table" parameter matching "wild".  If wild is empty or nil then a list of
 *  all columns will be returned. */
static VALUE sqlrcur_getColumnList(VALUE self, VALUE table, VALUE wild) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR2(sqlrcur,bool,getColumnList,table,wild);
	return INT2NUM(result);
}

static bool sendQuery(params *p) {
	return p->sqlrcur->sendQuery(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  sendQuery(query)
 *
 *  Sends "query" directly and gets a result set. */
static VALUE sqlrcur_sendQuery(VALUE self, VALUE query) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,bool,sendQuery,query);
	return INT2NUM(result);
}

static bool sendQueryWithLength(params *p) {
	return p->sqlrcur->sendQuery(STR2CSTR(p->one),NUM2INT(p->two));
}
/**
 *  call-seq:
 *  sendQueryWithLength(query,length)
 *
 *  Sends "query" with length "length" directly and gets a result set. This
 *  function must be used if the query contains binary data. */
static VALUE sqlrcur_sendQueryWithLength(VALUE self,
					VALUE query, VALUE length) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR2(sqlrcur,bool,sendQueryWithLength,query,length);
	return INT2NUM(result);
}

static bool sendFileQuery(params *p) {
	return p->sqlrcur->sendFileQuery(STR2CSTR(p->one),STR2CSTR(p->two));
}
/**
 *  call-seq:
 *  sendFileQuery(path,filename)
 *
 *  Sends the query in file "path"/"filename" and gets a result set. */
static VALUE sqlrcur_sendFileQuery(VALUE self, VALUE path, VALUE filename) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR2(sqlrcur,bool,sendFileQuery,path,filename);
	return INT2NUM(result);
}

static void prepareQuery(params *p) {
	p->sqlrcur->prepareQuery(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  prepareQuery(query)
 *
 *  Prepare to execute "query". */
static VALUE sqlrcur_prepareQuery(VALUE self, VALUE query) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR1(sqlrcur,prepareQuery,query);
	return Qnil;
}

static void prepareQueryWithLength(params *p) {
	p->sqlrcur->prepareQuery(STR2CSTR(p->one),NUM2INT(p->two));
}
/**
 *  call-seq:
 *  prepareQuery(query,length)
 *
 *  Prepare to execute "query" with length "length".  This function must be
 *  used if the query contains binary data. */
static VALUE sqlrcur_prepareQueryWithLength(VALUE self,
					VALUE query, VALUE length) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR2(sqlrcur,prepareQueryWithLength,query,length);
	return Qnil;
}

static bool prepareFileQuery(params *p) {
	return p->sqlrcur->prepareFileQuery(STR2CSTR(p->one),STR2CSTR(p->two));
}
/**
 *  call-seq:
 *  prepareFileQuery(path,filename)
 *
 *  Prepare to execute the contents of "path"/"filename". */
static VALUE sqlrcur_prepareFileQuery(VALUE self, VALUE path, VALUE filename) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR2(sqlrcur,bool,prepareFileQuery,path,filename);
	return INT2NUM(result);
}

static void clearBinds(params *p) {
	p->sqlrcur->clearBinds();
}
/** Clears all bind variables. */
static VALUE sqlrcur_clearBinds(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,clearBinds);
	return Qnil;
}

static uint16_t countBindVariables(params *p) {
	return p->sqlrcur->countBindVariables();
}
/** Parses the previously prepared query, counts the number of bind variables
 *  defined in it and returns that number. */
static VALUE sqlrcur_countBindVariables(VALUE self) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,uint64_t,countBindVariables);
	return INT2NUM(result);
}

static void substitutionStr(params *p) {
	p->sqlrcur->substitution(STR2CSTR(p->one),STR2CSTR(p->two));
}
static void substitutionInt(params *p) {
	p->sqlrcur->substitution(STR2CSTR(p->one),NUM2INT(p->two));
}
static void substitutionDbl(params *p) {
	p->sqlrcur->substitution(STR2CSTR(p->one),NUM2DBL(p->two), 
					(unsigned short)NUM2INT(p->three),
					(unsigned short)NUM2INT(p->four));
}
static void substitutionNull(params *p) {
	p->sqlrcur->substitution(STR2CSTR(p->one),(const char *)NULL);
}
/**
 *  call-seq:
 *  substitution(variable,value,(precision),(scale))
 *
 *  Defines a substitution variable.  The value may be a string, integer or
 *  decimal.  If it is a decimal then the precision and scale may be
 *  specified. */
static VALUE sqlrcur_substitution(int argc, VALUE *argv, VALUE self) {
	sqlrcursor	*sqlrcur;
	VALUE	variable;
	VALUE	value;
	VALUE	precision;
	VALUE	scale;
	bool	result=true;
	rb_scan_args(argc,argv,"22",&variable,&value,&precision,&scale);
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
		CUR2(sqlrcur,substitutionStr,variable,value);
	} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
		CUR2(sqlrcur,substitutionInt,variable,value);
	} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
		CUR4(sqlrcur,substitutionDbl,variable,value,precision,scale);
	} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
		CUR1(sqlrcur,substitutionNull,variable);
	} else {
		result=false;
	}
	return INT2NUM(result);
}

static void inputBindStrLen(params *p) {
	p->sqlrcur->inputBind(STR2CSTR(p->one),
					STR2CSTR(p->two),
					NUM2INT(p->three));
}
static void inputBindStr(params *p) {
	p->sqlrcur->inputBind(STR2CSTR(p->one),STR2CSTR(p->two));
}
static void inputBindInt(params *p) {
	p->sqlrcur->inputBind(STR2CSTR(p->one),NUM2INT(p->two));
}
static void inputBindDbl(params *p) {
	p->sqlrcur->inputBind(STR2CSTR(p->one),NUM2DBL(p->two), 
					(unsigned short)NUM2INT(p->three),
					(unsigned short)NUM2INT(p->four));
}
static void inputBindNull(params *p) {
	p->sqlrcur->inputBind(STR2CSTR(p->one),(const char *)NULL);
}
/**
 *  call-seq:
 *  inputBind(variable,value,(precision),(scale))
 *
 *  Defines am input bind variable.  The value may be a string, integer or
 *  decimal.  If it is a decimal then the precision and scale may be
 *  specified. If you don't have the precision and scale then set them
 *  both to 0.  However in that case you may get unexpected rounding behavior
 *  if the server is faking binds. */
static VALUE sqlrcur_inputBind(int argc, VALUE *argv, VALUE self) {
	sqlrcursor	*sqlrcur;
	VALUE	variable;
	VALUE	value;
	VALUE	precision;
	VALUE	scale;
	bool	success=true;
	rb_scan_args(argc,argv,"22",&variable,&value,&precision,&scale);
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
		if ((rb_obj_is_instance_of(precision,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(precision,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(precision,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(precision,rb_cNumeric)==Qtrue) && 
			NUM2INT(precision)>0) {
			// in this case, the precision parameter is actually
			// the string length
			CUR3(sqlrcur,inputBindStrLen,variable,value,precision);
		} else {
			CUR2(sqlrcur,inputBindStr,variable,value);
		}
	} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
		CUR2(sqlrcur,inputBindInt,variable,value);
	} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
		CUR4(sqlrcur,inputBindDbl,variable,value,precision,scale);
	} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
		CUR1(sqlrcur,inputBindNull,variable);
	} else {
		success=false;
	}
	return INT2NUM(success);
}

static void inputBindBlob(params *p) {
	p->sqlrcur->inputBindBlob(STR2CSTR(p->one),
					STR2CSTR(p->two),
					NUM2INT(p->three));
}
static void inputBindBlobNull(params *p) {
	p->sqlrcur->inputBindBlob(STR2CSTR(p->one),
					NULL,
					NUM2INT(p->two));
}
/**
 *  call-seq:
 *  inputBindBlob(variable,value,size)
 *
 *  Defines a binary lob input bind variable. */
static VALUE sqlrcur_inputBindBlob(VALUE self, VALUE variable,
					VALUE value, VALUE size) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	bool	success=true;
	if (value==Qnil) {
		CUR2(sqlrcur,inputBindBlobNull,variable,size);
	} else if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
		CUR3(sqlrcur,inputBindBlob,variable,value,size);
	} else {
		success=false;
	}
	return INT2NUM(success);
}

static void inputBindClob(params *p) {
	p->sqlrcur->inputBindClob(STR2CSTR(p->one),
					STR2CSTR(p->two),
					NUM2INT(p->three));
}
static void inputBindClobNull(params *p) {
	p->sqlrcur->inputBindClob(STR2CSTR(p->one),
					NULL,
					NUM2INT(p->two));
}
/**
 *  call-seq:
 *  inputBindClob(variable,value,size)
 *
 *  Defines a character lob input bind variable. */
static VALUE sqlrcur_inputBindClob(VALUE self, VALUE variable,
					VALUE value, VALUE size) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	bool	success=true;
	if (value==Qnil) {
		CUR2(sqlrcur,inputBindClobNull,variable,size);
	} else if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
		CUR3(sqlrcur,inputBindClob,variable,value,size);
	} else {
		success=false;
	}
	return INT2NUM(success);
}

static void defineOutputBindString(params *p) {
	p->sqlrcur->defineOutputBindString(STR2CSTR(p->one),NUM2INT(p->two));
}
/**
 *  call-seq:
 *  defineOutputBindString(variable,length)
 *
 *  Defines a string output bind variable.
 *  "length" bytes will be reserved to store the value. */
static VALUE sqlrcur_defineOutputBindString(VALUE self, VALUE variable,
							VALUE bufferlength) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR2(sqlrcur,defineOutputBindString,variable,bufferlength);
	return Qnil;
}

static void defineOutputBindInteger(params *p) {
	p->sqlrcur->defineOutputBindInteger(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  defineOutputBindInteger(variable)
 *
 *  Defines an integer output bind variable. */
static VALUE sqlrcur_defineOutputBindInteger(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR1(sqlrcur,defineOutputBindInteger,variable);
	return Qnil;
}

static void defineOutputBindDouble(params *p) {
	p->sqlrcur->defineOutputBindDouble(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  defineOutputBindDouble(variable)
 *
 *  Defines an decimal output bind variable. */
static VALUE sqlrcur_defineOutputBindDouble(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR1(sqlrcur,defineOutputBindDouble,variable);
	return Qnil;
}

static void defineOutputBindBlob(params *p) {
	p->sqlrcur->defineOutputBindBlob(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  defineOuptutBindBlob(variable)
 *
 *  Defines a binary lob output bind variable */
static VALUE sqlrcur_defineOutputBindBlob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR1(sqlrcur,defineOutputBindBlob,variable);
	return Qnil;
}

static void defineOutputBindClob(params *p) {
	p->sqlrcur->defineOutputBindClob(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  defineOutputBindClob(variable)
 *
 *  Defines a character lob output bind variable */
static VALUE sqlrcur_defineOutputBindClob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR1(sqlrcur,defineOutputBindClob,variable);
	return Qnil;
}

static void defineOutputBindCursor(params *p) {
	p->sqlrcur->defineOutputBindCursor(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  defineOutputBindCursor(variable)
 *
 *  Defines a cursor output bind variable */
static VALUE sqlrcur_defineOutputBindCursor(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR1(sqlrcur,defineOutputBindCursor,variable);
	return Qnil;
}

/**
 *  call-seq:
 *  substitutions(variables,values,(precisions),(scales))
 *
 *  Defines an array of substitution variables.  The values may be strings,
 *  integers or decimals.  If they are decimals then the precisions and scales
 *  may also be specified. */
static VALUE sqlrcur_substitutions(int argc, VALUE *argv, VALUE self) {
	sqlrcursor	*sqlrcur;
	VALUE	variables;
	VALUE	values;
	VALUE	precisions;
	VALUE	scales;
	int	argcount=rb_scan_args(argc,argv,"22",
					&variables,&values,&precisions,&scales);
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (variables==Qnil || values==Qnil) {
		return Qnil;
	}
	VALUE	variable;
	VALUE	value;
	VALUE	precision=0;
	VALUE	scale=0;
	bool	success=true;
	for (;;) {
		variable=rb_ary_shift(variables);
		if (variable==Qnil) {
			break;
		}
		value=rb_ary_shift(values);
		if (argcount==4) {
			precision=rb_ary_shift(precisions);
			scale=rb_ary_shift(scales);
		}
		if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
			CUR2(sqlrcur,substitutionStr,variable,value);
		} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
			CUR2(sqlrcur,substitutionInt,variable,value);
		} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
			CUR4(sqlrcur,substitutionDbl,variable,
						value,precision,scale);
		} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
			CUR1(sqlrcur,substitutionNull,variable);
		} else {
			success=false;
		}
	}
	return INT2NUM(success);
}

/**
 *  call-seq:
 *  inputBinds(variables,values,(precisions),(scales))
 *
 *  Defines an array of input bind variables.  The values may be strings,
 *  integers or decimals.  If they are decimals then the precisions and scales
 *  may also be specified. */
static VALUE sqlrcur_inputBinds(int argc, VALUE *argv, VALUE self) {
	sqlrcursor	*sqlrcur;
	VALUE	variables;
	VALUE	values;
	VALUE	precisions;
	VALUE	scales;
	int	argcount=rb_scan_args(argc,argv,"22",
				&variables,&values,&precisions,&scales);
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (variables==Qnil || values==Qnil) {
		return Qnil;
	}
	VALUE	variable;
	VALUE	value;
	VALUE	precision=0;
	VALUE	scale=0;
	bool	success=true;
	for (;;) {
		variable=rb_ary_shift(variables);
		if (variable==Qnil) {
			break;
		}
		value=rb_ary_shift(values);
		if (argcount==4) {
			precision=rb_ary_shift(precisions);
			scale=rb_ary_shift(scales);
		}
		if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
			CUR2(sqlrcur,inputBindStr,variable,value);
		} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
			CUR2(sqlrcur,inputBindInt,variable,value);
		} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
			CUR4(sqlrcur,inputBindDbl,variable,
						value,precision,scale);
		} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
			CUR1(sqlrcur,inputBindNull,variable);
		} else {
			success=false;
		}
	}
	return INT2NUM(success);
}

static void validateBinds(params *p) {
	p->sqlrcur->validateBinds();
}
/** If you are binding to any variables that might not actually be in your
 *  query, call this to ensure that the database won't try to bind them unless
 *  they really are in the query.  There is a performance penalty for calling
 *  this function */
static VALUE sqlrcur_validateBinds(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,validateBinds);
	return Qnil;
}

static bool validBind(params *p) {
	return p->sqlrcur->validBind(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  validBind(variable)
 *
 *  Returns true if "variable" was a valid bind variable of the query. */
static VALUE sqlrcur_validBind(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,bool,validBind,variable);
	return INT2NUM(result);
}

static bool executeQuery(params *p) {
	return p->sqlrcur->executeQuery();
}
/** Execute the query that was previously prepared and bound. */
static VALUE sqlrcur_executeQuery(VALUE self) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,bool,executeQuery);
	return INT2NUM(result);
}

static bool fetchFromBindCursor(params *p) {
	return p->sqlrcur->fetchFromBindCursor();
}
/** Fetch from a cursor that was returned as an output bind variable. */
static VALUE sqlrcur_fetchFromBindCursor(VALUE self) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,bool,fetchFromBindCursor);
	return INT2NUM(result);
}

static const char *getOutputBindString(params *p) {
	return p->sqlrcur->getOutputBindString(STR2CSTR(p->one));
}
static uint64_t getOutputBindLength(params *p) {
	return p->sqlrcur->getOutputBindLength(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  getOutputBindString(variable)
 *
 *  Get the value stored in a previously defined string output bind variable. */
static VALUE sqlrcur_getOutputBindString(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	const char	*result;
	uint64_t	length;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RLCUR1(sqlrcur,const char *,getOutputBindString,variable,
				uint64_t,getOutputBindLength,variable);
	if (result) {
		return rb_str_new(result,length);
	} else {
		return Qnil;
	}
}

static const char *getOutputBindBlob(params *p) {
	return p->sqlrcur->getOutputBindBlob(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  getOutputBindBlob(variable)
 *
 *  Get the value stored in a previously defined
 *  binary lob output bind variable. */
static VALUE sqlrcur_getOutputBindBlob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	const char	*result;
	uint64_t	length;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RLCUR1(sqlrcur,const char *,getOutputBindBlob,variable,
				uint64_t,getOutputBindLength,variable);
	if (result) {
		return rb_str_new(result,length);
	} else {
		return Qnil;
	}
}

static const char *getOutputBindClob(params *p) {
	return p->sqlrcur->getOutputBindClob(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  getOutputBindClob(variable)
 *
 *  Get the value stored in a previously defined
 *  character lob output bind variable. */
static VALUE sqlrcur_getOutputBindClob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	const char	*result;
	long		length;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RLCUR1(sqlrcur,const char *,getOutputBindClob,variable,
				uint64_t,getOutputBindLength,variable);
	if (result) {
		return rb_str_new(result,length);
	} else {
		return Qnil;
	}
}

static int64_t getOutputBindInteger(params *p) {
	return p->sqlrcur->getOutputBindInteger(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  getOutputBindInteger(variable)
 *
 *  Get the value stored in a previously defined
 *  integer output bind variable. */
static VALUE sqlrcur_getOutputBindInteger(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	int64_t		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,int64_t,getOutputBindInteger,variable);
	return INT2NUM(result);
}

// FIXME: get this working...
/*static double getOutputBindDouble(params *p) {
	return p->sqlrcur->getOutputBindDouble(STR2CSTR(p->one));
}*/
/**
 *  call-seq:
 *  getOutputBindDouble(variable)
 *
 *  Get the value stored in a previously defined
 *  decimal output bind variable. */
static VALUE sqlrcur_getOutputBindDouble(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	double		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	//RCUR1COPY(sqlrcur,double,getOutputBindDouble,variable);
	result=sqlrcur->getOutputBindDouble(STR2CSTR(variable));
	return rb_float_new(result);
}

/**
 *  call-seq:
 *  getOutputBindLength(variable)
 *
 *  Get the length of the value stored in a previously
 *  defined output bind variable. */
static VALUE sqlrcur_getOutputBindLength(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,uint64_t,getOutputBindLength,variable);
	return INT2NUM(result);
}
static sqlrcursor *getOutputBindCursor(params *p) {
	return p->sqlrcur->getOutputBindCursor(STR2CSTR(p->one),true);
}
/**
 *  call-seq:
 *  getOutputBindCursor(variable)
 *
 *  Get the cursor associated with a previously defined output bind variable. */
static VALUE sqlrcur_getOutputBindCursor(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	sqlrcursor	*result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,sqlrcursor *,getOutputBindCursor,variable);
	return Data_Wrap_Struct(csqlrcursor,0,sqlrcur_free,(void *)result);
}

static bool openCachedResultSet(params *p) {
	return p->sqlrcur->openCachedResultSet(STR2CSTR(p->one));
}
/**
 *  call-seq:
 *  openCachedResultSet(filename)
 *
 *  Opens a cached result set.  Returns 1 on success and 0 on failure. */
static VALUE sqlrcur_openCachedResultSet(VALUE self, VALUE filename) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,bool,openCachedResultSet,filename);
	return INT2NUM(result);
}

static uint32_t colCount(params *p) {
	return p->sqlrcur->colCount();
}
/** Returns the number of columns in the current result set. */
static VALUE sqlrcur_colCount(VALUE self) {
	sqlrcursor	*sqlrcur;
	uint32_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,uint64_t,colCount);
	return INT2NUM(result);
}

static uint64_t rowCount(params *p) {
	return p->sqlrcur->rowCount();
}
/** Returns the number of rows in the current result set. */
static VALUE sqlrcur_rowCount(VALUE self) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,uint64_t,rowCount);
	return INT2NUM(result);
}

static uint64_t totalRows(params *p) {
	return p->sqlrcur->totalRows();
}
/** Returns the total number of rows that will be returned in the result set.
 *  Not all databases support this call.  Don't use it for applications which
 *  are designed to be portable across databases.  -1 is returned by databases
 *  which don't support this option. */
static VALUE sqlrcur_totalRows(VALUE self) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,uint64_t,totalRows);
	return INT2NUM(result);
}

static uint64_t affectedRows(params *p) {
	return p->sqlrcur->affectedRows();
}
/** Returns the number of rows that were updated, inserted or deleted by the
 *  query.  Not all databases support this call.  Don't use it for applications
 *  which are designed to be portable across databases.  -1 is returned by
 *  databases which don't support this option. */
static VALUE sqlrcur_affectedRows(VALUE self) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,uint64_t,affectedRows);
	return INT2NUM(result);
}

static uint64_t firstRowIndex(params *p) {
	return p->sqlrcur->firstRowIndex();
}
/** Returns the index of the first buffered row.  This is useful when buffering
 *  only part of the result set at a time. */
static VALUE sqlrcur_firstRowIndex(VALUE self) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,uint64_t,firstRowIndex);
	return INT2NUM(result);
}

static bool endOfResultSet(params *p) {
	return p->sqlrcur->endOfResultSet();
}
/** Returns 0 if part of the result set is still pending on the server and 1 if
 *  not.  This function can only return 0 if setResultSetBufferSize() has been
 *  called with a parameter other than 0. */
static VALUE sqlrcur_endOfResultSet(VALUE self) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,bool,endOfResultSet);
	return INT2NUM(result);
}

static const char *curErrorMessage(params *p) {
	return p->sqlrcur->errorMessage();
}
/** If a query failed and generated an error, the error message is available
 *  here.  If the query succeeded then this function returns a nil. */
static VALUE sqlrcur_errorMessage(VALUE self) {
	sqlrcursor	*sqlrcur;
	const char	*result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,const char *,curErrorMessage);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static uint64_t curErrorNumber(params *p) {
	return p->sqlrcur->errorNumber();
}
/** If a query failed and generated an error, the error number is
 *  available here.  If there is no error then this method returns 0. */
static VALUE sqlrcur_errorNumber(VALUE self) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,uint64_t,curErrorNumber);
	return INT2NUM(result);
}

static void getNullsAsEmptyStrings(params *p) {
	p->sqlrcur->getNullsAsEmptyStrings();
}
/** Tells the connection to return NULL fields and output bind variables as
 *  empty strings.  This is the default. */
static VALUE sqlrcur_getNullsAsEmptyStrings(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,getNullsAsEmptyStrings);
	return Qnil;
}

static void getNullsAsNulls(params *p) {
	p->sqlrcur->getNullsAsNulls();
}
/** Tells the connection to return NULL fields
 *  and output bind variables as nil's. */
static VALUE sqlrcur_getNullsAsNils(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,getNullsAsNulls);
	return Qnil;
}

static const char *getFieldStr(params *p) {
	return p->sqlrcur->getField(NUM2INT(p->one),STR2CSTR(p->two));
}
static uint64_t getFieldLengthStr(params *p) {
	return p->sqlrcur->getFieldLength(NUM2INT(p->one),STR2CSTR(p->two));
}
static const char *getFieldInt(params *p) {
	return p->sqlrcur->getField(NUM2INT(p->one),NUM2INT(p->two));
}
static uint64_t getFieldLengthInt(params *p) {
	return p->sqlrcur->getFieldLength(NUM2INT(p->one),NUM2INT(p->two));
}
/**
 *  call-seq:
 *  getField(row,col)
 *
 *  Returns the specified field as a string.  "col" may be specified as the 
 *  column name or number. */
static VALUE sqlrcur_getField(VALUE self, VALUE row, VALUE col) {
	sqlrcursor	*sqlrcur;
	const char	*result;
	uint64_t	length;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RLCUR2(sqlrcur,const char *,getFieldStr,row,col,
				uint64_t,getFieldLengthStr,row,col);
	} else {
		RLCUR2(sqlrcur,const char *,getFieldInt,row,col,
				uint64_t,getFieldLengthInt,row,col);
	}
	if (result) {
		return rb_str_new(result,length);
	} else {
		return Qnil;
	}
}
  
static int64_t getFieldAsIntegerStr(params *p) {
	return p->sqlrcur->getFieldAsInteger(NUM2INT(p->one),STR2CSTR(p->two));
}
static int64_t getFieldAsIntegerInt(params *p) {
	return p->sqlrcur->getFieldAsInteger(NUM2INT(p->one),NUM2INT(p->two));
}
/**
 *  call-seq:
 *  getFieldAsInteger(row,col)
 *
 *  Returns the specified field as an integer.  "col" may be specified as the
 *  column name or number. */
static VALUE sqlrcur_getFieldAsInteger(VALUE self, VALUE row, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	int64_t	result;
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR2(sqlrcur,int64_t,getFieldAsIntegerStr,row,col);
	} else {
		RCUR2(sqlrcur,int64_t,getFieldAsIntegerInt,row,col);
	}
	return INT2NUM(result);
}

static double getFieldAsDoubleStr(params *p) {
	return p->sqlrcur->getFieldAsDouble(NUM2INT(p->one),STR2CSTR(p->two));
}
static double getFieldAsDoubleInt(params *p) {
	return p->sqlrcur->getFieldAsDouble(NUM2INT(p->one),NUM2INT(p->two));
}
/**
 *  call-seq:
 *  getFieldAsDouble(row,col)
 *
 *  Returns the specified field as an decimal.  "col" may be specified as the
 *  column name or number. */
static VALUE sqlrcur_getFieldAsDouble(VALUE self, VALUE row, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	double	result;
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR2COPY(sqlrcur,double,getFieldAsDoubleStr,row,col);
	} else {
		RCUR2COPY(sqlrcur,double,getFieldAsDoubleInt,row,col);
	}
	return rb_float_new(result);
}

/**
 *  call-seq:
 *  getFieldLength(row,col)
 *
 *  Returns the length of the specified row and column.  "col" may be specified
 *  as the column name or number. */
static VALUE sqlrcur_getFieldLength(VALUE self, VALUE row, VALUE col) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR2(sqlrcur,uint64_t,getFieldLengthStr,row,col);
	} else {
		RCUR2(sqlrcur,uint64_t,getFieldLengthInt,row,col);
	}
	return INT2NUM(result);
}

static const char * const *getRow(params *p) {
	return p->sqlrcur->getRow(NUM2INT(p->one));
}
static uint32_t *getRowLengths(params *p) {
	return p->sqlrcur->getRowLengths(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getRow(row)
 *
 *  Returns an array of the values of the fields in the specified row. */
static VALUE sqlrcur_getRow(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	const char * const *result;
	uint32_t	*length;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	VALUE	fieldary=rb_ary_new2(sqlrcur->colCount());
	RLCUR1(sqlrcur,const char * const *,getRow,row,
				uint32_t *,getRowLengths,row);
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		if (result[i]) {
			rb_ary_store(fieldary,i,rb_str_new(result[i],
								length[i]));
		} else {
			rb_ary_store(fieldary,i,Qnil);
		}
	}
	return fieldary;
}

static const char *getColumnName(params *p) {
	return p->sqlrcur->getColumnName(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getRowHash(row)
 *
 *  Returns a hash of the values of the fields in the specified row. */
static VALUE sqlrcur_getRowHash(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	const char * const *result;
	uint32_t	*length;
	const char	*name;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RLCUR1(sqlrcur,const char * const *,getRow,row,
				uint32_t *,getRowLengths,row);
	VALUE	fieldhash=rb_hash_new();
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		RCUR1NAME(sqlrcur,const char *,getColumnName,INT2NUM(i));
		if (result[i]) {
			rb_hash_aset(fieldhash,
					rb_str_new2(name),
					rb_str_new(result[i],length[i]));
		} else {
			rb_hash_aset(fieldhash,
					rb_str_new2(name),
					Qnil);
		}
	}
	return fieldhash;
}

/**
 *  call-seq:
 *  getRowLengths(row)
 *
 *  Returns an array of the lengths of the fields in the specified row. */
static VALUE sqlrcur_getRowLengths(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	uint32_t	*result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,uint32_t *,getRowLengths,row);
	if (!result) {
		return Qnil;
	}
	VALUE	lengthary=rb_ary_new2(sqlrcur->colCount());
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		rb_ary_store(lengthary,i,INT2NUM(result[i]));
	}
	return lengthary;
}

/**
 *  call-seq:
 *  getRowLengthsHash(row)
 *
 *  Returns a hash of the lengths of the fields in the specified row. */
static VALUE sqlrcur_getRowLengthsHash(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	uint32_t	*result;
	const char	*name;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,uint32_t *,getRowLengths,row);
	VALUE	lengthhash=rb_hash_new();
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		RCUR1NAME(sqlrcur,const char *,getColumnName,INT2NUM(i));
		rb_hash_aset(lengthhash,
				rb_str_new2(name),
				INT2NUM(result[i]));
	}
	return lengthhash;
}

static const char * const *getColumnNames(params *p) {
	return p->sqlrcur->getColumnNames();
}
/** Returns an array of the column names of the current result set. */
static VALUE sqlrcur_getColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	const char * const *result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,const char * const *,getColumnNames);
	if (!result) {
		return Qnil;
	}
	VALUE	nameary=rb_ary_new2(sqlrcur->colCount());
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		if (result[i]) {
			rb_ary_store(nameary,i,rb_str_new2(result[i]));
		} else {
			rb_ary_store(nameary,i,Qnil);
		}
	}
	return nameary;
}

/**
 *  call-seq:
 *  getColumnName(col)
 *
 *  Returns the name of the specified column. */
static VALUE sqlrcur_getColumnName(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	const char	*result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,const char *,getColumnName,col);
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static const char *getColumnTypeStr(params *p) {
	return p->sqlrcur->getColumnType(STR2CSTR(p->one));
}
static const char *getColumnTypeInt(params *p) {
	return p->sqlrcur->getColumnType(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnType(col)
 *
 *  Returns the type of the specified column. "col" may be specified as the
 *  column name or number. */
static VALUE sqlrcur_getColumnType(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*result;
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,const char *,getColumnTypeStr,col);
	} else {
		RCUR1(sqlrcur,const char *,getColumnTypeInt,col);
	}
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}
 
static uint32_t getColumnLengthStr(params *p) {
	return p->sqlrcur->getColumnLength(STR2CSTR(p->one));
}
static uint32_t getColumnLengthInt(params *p) {
	return p->sqlrcur->getColumnLength(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnLength(col)
 *
 *  Returns the length of the specified column. "col" may be specified as the
 *  column name or number. */
static VALUE sqlrcur_getColumnLength(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	uint32_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,uint64_t,getColumnLengthStr,col);
	} else {
		RCUR1(sqlrcur,uint64_t,getColumnLengthInt,col);
	}
	return INT2NUM(result);
}

static uint32_t getColumnPrecisionStr(params *p) {
	return p->sqlrcur->getColumnPrecision(STR2CSTR(p->one));
}
static uint32_t getColumnPrecisionInt(params *p) {
	return p->sqlrcur->getColumnPrecision(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnPrecision(col)
 *
 *  Returns the precision of the specified column.  Precision is the total
 *  number of digits in a number.  eg: 123.45 has a precision of 5.  For
 *  non-numeric types, it's the number of characters in the string.  "col"
 *  may be specified as the column name or number. */
static VALUE sqlrcur_getColumnPrecision(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	uint32_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,uint64_t,getColumnPrecisionStr,col);
	} else {
		RCUR1(sqlrcur,uint64_t,getColumnPrecisionInt,col);
	}
	return INT2NUM(result);
}

static uint32_t getColumnScaleStr(params *p) {
	return p->sqlrcur->getColumnScale(STR2CSTR(p->one));
}
static uint32_t getColumnScaleInt(params *p) {
	return p->sqlrcur->getColumnScale(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnScale(col)
 *
 *  Returns the scale of the specified column.  Scale is the total number of
 *  digits to the right of the decimal point in a number.  eg: 123.45 has a
 *  scale of 2.  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnScale(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	uint32_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,uint64_t,getColumnScaleStr,col);
	} else {
		RCUR1(sqlrcur,uint64_t,getColumnScaleInt,col);
	}
	return INT2NUM(result);
}

static bool getColumnIsNullableStr(params *p) {
	return p->sqlrcur->getColumnIsNullable(STR2CSTR(p->one));
}
static bool getColumnIsNullableInt(params *p) {
	return p->sqlrcur->getColumnIsNullable(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnIsNullable(col)
 *
 *  Returns 1 if the specified column can contain nulls and 0 otherwise.
 *  "col" may be specified as the colum name or number. */
static VALUE sqlrcur_getColumnIsNullable(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,bool,getColumnIsNullableStr,col);
	} else {
		RCUR1(sqlrcur,bool,getColumnIsNullableInt,col);
	}
	return INT2NUM(result);
}

static bool getColumnIsPrimaryKeyStr(params *p) {
	return p->sqlrcur->getColumnIsPrimaryKey(STR2CSTR(p->one));
}
static bool getColumnIsPrimaryKeyInt(params *p) {
	return p->sqlrcur->getColumnIsPrimaryKey(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnIsPrimaryKey(col)
 *
 *  Returns 1 if the specified column is a primary key and 0 otherwise.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsPrimaryKey(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,bool,getColumnIsPrimaryKeyStr,col);
	} else {
		RCUR1(sqlrcur,bool,getColumnIsPrimaryKeyInt,col);
	}
	return INT2NUM(result);
}

static bool getColumnIsUniqueStr(params *p) {
	return p->sqlrcur->getColumnIsUnique(STR2CSTR(p->one));
}
static bool getColumnIsUniqueInt(params *p) {
	return p->sqlrcur->getColumnIsUnique(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnIsUnique(col)
 *
 *  Returns 1 if the specified column is unique and 0 otherwise. 
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsUnique(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,bool,getColumnIsUniqueStr,col);
	} else {
		RCUR1(sqlrcur,bool,getColumnIsUniqueInt,col);
	}
	return INT2NUM(result);
}

static bool getColumnIsPartOfKeyStr(params *p) {
	return p->sqlrcur->getColumnIsPartOfKey(STR2CSTR(p->one));
}
static bool getColumnIsPartOfKeyInt(params *p) {
	return p->sqlrcur->getColumnIsPartOfKey(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnIsPartOfKey(col)
 *
 *  Returns 1 if the specified column is part of a composite key and 0
 *  otherwise.  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsPartOfKey(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,bool,getColumnIsPartOfKeyStr,col);
	} else {
		RCUR1(sqlrcur,bool,getColumnIsPartOfKeyInt,col);
	}
	return INT2NUM(result);
}

static bool getColumnIsUnsignedStr(params *p) {
	return p->sqlrcur->getColumnIsUnsigned(STR2CSTR(p->one));
}
static bool getColumnIsUnsignedInt(params *p) {
	return p->sqlrcur->getColumnIsUnsigned(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnIsUnsigned(col)
 *
 *  Returns 1 if the specified column is an unsigned number and 0 otherwise.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsUnsigned(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,bool,getColumnIsUnsignedStr,col);
	} else {
		RCUR1(sqlrcur,bool,getColumnIsUnsignedInt,col);
	}
	return INT2NUM(result);
}

static bool getColumnIsZeroFilledStr(params *p) {
	return p->sqlrcur->getColumnIsZeroFilled(STR2CSTR(p->one));
}
static bool getColumnIsZeroFilledInt(params *p) {
	return p->sqlrcur->getColumnIsZeroFilled(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnIsZeroFilled(col)
 *
 *  Returns 1 if the specified column was created with the zero-fill flag and
 *  0 otherwise.  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsZeroFilled(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,bool,getColumnIsZeroFilledStr,col);
	} else {
		RCUR1(sqlrcur,bool,getColumnIsZeroFilledInt,col);
	}
	return INT2NUM(result);
}

static bool getColumnIsBinaryStr(params *p) {
	return p->sqlrcur->getColumnIsBinary(STR2CSTR(p->one));
}
static bool getColumnIsBinaryInt(params *p) {
	return p->sqlrcur->getColumnIsBinary(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnIsBinary(col)
 *
 *  Returns 1 if the specified column contains binary data and 0 otherwise.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsBinary(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,bool,getColumnIsBinaryStr,col);
	} else {
		RCUR1(sqlrcur,bool,getColumnIsBinaryInt,col);
	}
	return INT2NUM(result);
}

static bool getColumnIsAutoIncrementStr(params *p) {
	return p->sqlrcur->getColumnIsAutoIncrement(STR2CSTR(p->one));
}
static bool getColumnIsAutoIncrementInt(params *p) {
	return p->sqlrcur->getColumnIsAutoIncrement(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getColumnIsAutoIncrement(col)
 *  
 *  Returns 1 if the specified column auto-increments and 0 otherwise.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsAutoIncrement(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,bool,getColumnIsAutoIncrementStr,col);
	} else {
		RCUR1(sqlrcur,bool,getColumnIsAutoIncrementInt,col);
	}
	return INT2NUM(result);
}

static uint64_t getLongestStr(params *p) {
	return p->sqlrcur->getLongest(STR2CSTR(p->one));
}
static uint64_t getLongestInt(params *p) {
	return p->sqlrcur->getLongest(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  getLongest(col)
 *
 *  Returns the length of the longest field in the specified column.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getLongest(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	uint64_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		RCUR1(sqlrcur,uint64_t,getLongestStr,col);
	} else {
		RCUR1(sqlrcur,uint64_t,getLongestInt,col);
	}
	return INT2NUM(result);
}

static uint16_t getResultSetId(params *p) {
	return p->sqlrcur->getResultSetId();
}
/** Returns the internal ID of this result set.  This parameter may be passed
 *  to another statement for use in the resumeResultSet() function.  Note: The
 *  value this function returns is only valid after a call to
 *  suspendResultSet().*/
static VALUE sqlrcur_getResultSetId(VALUE self) {
	sqlrcursor	*sqlrcur;
	uint16_t	result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR(sqlrcur,uint64_t,getResultSetId);
	return INT2NUM(result);
}

static void suspendResultSet(params *p) {
	p->sqlrcur->suspendResultSet();
}
/** Tells the server to leave this result set open when the connection calls
 *  suspendSession() so that another connection can connect to it using
 *  resumeResultSet() after it calls resumeSession(). */
static VALUE sqlrcur_suspendResultSet(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,suspendResultSet);
	return Qnil;
}

static bool resumeResultSet(params *p) {
	return p->sqlrcur->resumeResultSet(NUM2INT(p->one));
}
/**
 *  call-seq:
 *  resumeResultSet(id)
 *
 *  Resumes a result set previously left open using suspendSession().
 *  Returns 1 on success and 0 on failure. */
static VALUE sqlrcur_resumeResultSet(VALUE self, VALUE id) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR1(sqlrcur,bool,resumeResultSet,id);
	return INT2NUM(result);
}

static bool resumeCachedResultSet(params *p) {
	return p->sqlrcur->resumeCachedResultSet(
				NUM2INT(p->one),STR2CSTR(p->two));
}
/**
 *  call-seq:
 *  resumeCachedResultSet(id,filename)
 *
 *  Resumes a result set previously left open using suspendSession() and
 *  continues caching the result set to "filename".  Returns 1 on success and 0
 *  on failure. */
static VALUE sqlrcur_resumeCachedResultSet(VALUE self, 
						VALUE id, VALUE filename) {
	sqlrcursor	*sqlrcur;
	bool		result;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	RCUR2(sqlrcur,bool,resumeCachedResultSet,id,filename);
	return INT2NUM(result);
}

static void closeResultSet(params *p) {
	p->sqlrcur->closeResultSet();
}
/** Closes the current result set, if one is open.  Data
 *  that has been fetched already is still available but
 *  no more data may be fetched.  Server side resources
 *  for the result set are freed as well. */
static VALUE sqlrcur_closeResultSet(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	CUR(sqlrcur,closeResultSet);
	return Qnil;
}

/** */
void Init_SQLRCursor() {
	csqlrcursor=rb_define_class("SQLRCursor", rb_cObject);
	rb_define_singleton_method(csqlrcursor,"new",
				(CAST)sqlrcur_new,1);
	rb_define_method(csqlrcursor,"setResultSetBufferSize",
				(CAST)sqlrcur_setResultSetBufferSize,1);
	rb_define_method(csqlrcursor,"getResultSetBufferSize",
				(CAST)sqlrcur_getResultSetBufferSize,0);
	rb_define_method(csqlrcursor,"dontGetColumnInfo",
				(CAST)sqlrcur_dontGetColumnInfo,0);
	rb_define_method(csqlrcursor,"getColumnInfo",
				(CAST)sqlrcur_getColumnInfo,0);
	rb_define_method(csqlrcursor,"mixedCaseColumnNames",
				(CAST)sqlrcur_mixedCaseColumnNames,0);
	rb_define_method(csqlrcursor,"upperCaseColumnNames",
				(CAST)sqlrcur_upperCaseColumnNames,0);
	rb_define_method(csqlrcursor,"lowerCaseColumnNames",
				(CAST)sqlrcur_lowerCaseColumnNames,0);
	rb_define_method(csqlrcursor,"cacheToFile",
				(CAST)sqlrcur_cacheToFile,1);
	rb_define_method(csqlrcursor,"setCacheTtl",
				(CAST)sqlrcur_setCacheTtl,1);
	rb_define_method(csqlrcursor,"getCacheFileName",
				(CAST)sqlrcur_getCacheFileName,0);
	rb_define_method(csqlrcursor,"cacheOff",
				(CAST)sqlrcur_cacheOff,0);
	rb_define_method(csqlrcursor,"getDatabaseList",
				(CAST)sqlrcur_getDatabaseList,1);
	rb_define_method(csqlrcursor,"getTableList",
				(CAST)sqlrcur_getTableList,1);
	rb_define_method(csqlrcursor,"getColumnList",
				(CAST)sqlrcur_getColumnList,2);
	rb_define_method(csqlrcursor,"sendQuery",
				(CAST)sqlrcur_sendQuery,1);
	rb_define_method(csqlrcursor,"sendQueryWithLength",
				(CAST)sqlrcur_sendQueryWithLength,2);
	rb_define_method(csqlrcursor,"sendFileQuery",
				(CAST)sqlrcur_sendFileQuery,2);
	rb_define_method(csqlrcursor,"prepareQuery",
				(CAST)sqlrcur_prepareQuery,1);
	rb_define_method(csqlrcursor,"prepareQueryWithLength",
				(CAST)sqlrcur_prepareQueryWithLength,2);
	rb_define_method(csqlrcursor,"prepareFileQuery",
				(CAST)sqlrcur_prepareFileQuery,2);
	rb_define_method(csqlrcursor,"clearBinds",
				(CAST)sqlrcur_clearBinds,0);
	rb_define_method(csqlrcursor,"countBindVariables",
				(CAST)sqlrcur_countBindVariables,0);
	rb_define_method(csqlrcursor,"substitution",
				(CAST)sqlrcur_substitution,-1);
	rb_define_method(csqlrcursor,"inputBind",
				(CAST)sqlrcur_inputBind,-1);
	rb_define_method(csqlrcursor,"inputBindBlob",
				(CAST)sqlrcur_inputBindBlob,3);
	rb_define_method(csqlrcursor,"inputBindClob",
				(CAST)sqlrcur_inputBindClob,3);
	rb_define_method(csqlrcursor,"defineOutputBindString",
				(CAST)sqlrcur_defineOutputBindString,2);
	rb_define_method(csqlrcursor,"defineOutputBindInteger",
				(CAST)sqlrcur_defineOutputBindInteger,1);
	rb_define_method(csqlrcursor,"defineOutputBindDouble",
				(CAST)sqlrcur_defineOutputBindDouble,1);
	rb_define_method(csqlrcursor,"defineOutputBindBlob",
				(CAST)sqlrcur_defineOutputBindBlob,1);
	rb_define_method(csqlrcursor,"defineOutputBindClob",
				(CAST)sqlrcur_defineOutputBindClob,1);
	rb_define_method(csqlrcursor,"defineOutputBindCursor",
				(CAST)sqlrcur_defineOutputBindCursor,1);
	rb_define_method(csqlrcursor,"substitutions",
				(CAST)sqlrcur_substitutions,-1);
	rb_define_method(csqlrcursor,"inputBinds",
				(CAST)sqlrcur_inputBinds,-1);
	rb_define_method(csqlrcursor,"validateBinds",
				(CAST)sqlrcur_validateBinds,0);
	rb_define_method(csqlrcursor,"validBind",
				(CAST)sqlrcur_validBind,1);
	rb_define_method(csqlrcursor,"executeQuery",
				(CAST)sqlrcur_executeQuery,0);
	rb_define_method(csqlrcursor,"fetchFromBindCursor",
				(CAST)sqlrcur_fetchFromBindCursor,0);
	rb_define_method(csqlrcursor,"getOutputBindString",
				(CAST)sqlrcur_getOutputBindString,1);
	rb_define_method(csqlrcursor,"getOutputBindBlob",
				(CAST)sqlrcur_getOutputBindBlob,1);
	rb_define_method(csqlrcursor,"getOutputBindClob",
				(CAST)sqlrcur_getOutputBindClob,1);
	rb_define_method(csqlrcursor,"getOutputBindInteger",
				(CAST)sqlrcur_getOutputBindInteger,1);
	rb_define_method(csqlrcursor,"getOutputBindDouble",
				(CAST)sqlrcur_getOutputBindDouble,1);
	rb_define_method(csqlrcursor,"getOutputBindLength",
				(CAST)sqlrcur_getOutputBindLength,1);
	rb_define_method(csqlrcursor,"getOutputBindCursor",
				(CAST)sqlrcur_getOutputBindCursor,1);
	rb_define_method(csqlrcursor,"openCachedResultSet",
				(CAST)sqlrcur_openCachedResultSet,1);
	rb_define_method(csqlrcursor,"colCount",
				(CAST)sqlrcur_colCount,0);
	rb_define_method(csqlrcursor,"rowCount",
				(CAST)sqlrcur_rowCount,0);
	rb_define_method(csqlrcursor,"totalRows",
				(CAST)sqlrcur_totalRows,0);
	rb_define_method(csqlrcursor,"affectedRows",
				(CAST)sqlrcur_affectedRows,0);
	rb_define_method(csqlrcursor,"firstRowIndex",
				(CAST)sqlrcur_firstRowIndex,0);
	rb_define_method(csqlrcursor,"endOfResultSet",
				(CAST)sqlrcur_endOfResultSet,0);
	rb_define_method(csqlrcursor,"errorMessage",
				(CAST)sqlrcur_errorMessage,0);
	rb_define_method(csqlrcursor,"errorNumber",
				(CAST)sqlrcur_errorNumber,0);
	rb_define_method(csqlrcursor,"getNullsAsEmptyStrings",
				(CAST)sqlrcur_getNullsAsEmptyStrings,0);
	rb_define_method(csqlrcursor,"getNullsAsNils",
				(CAST)sqlrcur_getNullsAsNils,0);
	rb_define_method(csqlrcursor,"getField",
				(CAST)sqlrcur_getField,2);
	rb_define_method(csqlrcursor,"getFieldAsInteger",
				(CAST)sqlrcur_getFieldAsInteger,2);
	rb_define_method(csqlrcursor,"getFieldAsDouble",
				(CAST)sqlrcur_getFieldAsDouble,2);
	rb_define_method(csqlrcursor,"getFieldLength",
				(CAST)sqlrcur_getFieldLength,2);
	rb_define_method(csqlrcursor,"getRow",
				(CAST)sqlrcur_getRow,1);
	rb_define_method(csqlrcursor,"getRowHash",
				(CAST)sqlrcur_getRowHash,1);
	rb_define_method(csqlrcursor,"getRowLengths",
				(CAST)sqlrcur_getRowLengths,1);
	rb_define_method(csqlrcursor,"getRowLengthsHash",
				(CAST)sqlrcur_getRowLengthsHash,1);
	rb_define_method(csqlrcursor,"getColumnNames",
				(CAST)sqlrcur_getColumnNames,0);
	rb_define_method(csqlrcursor,"getColumnName",
				(CAST)sqlrcur_getColumnName,1);
	rb_define_method(csqlrcursor,"getColumnType",
				(CAST)sqlrcur_getColumnType,1);
	rb_define_method(csqlrcursor,"getColumnLength",
				(CAST)sqlrcur_getColumnLength,1);
	rb_define_method(csqlrcursor,"getColumnPrecision",
				(CAST)sqlrcur_getColumnPrecision,1);
	rb_define_method(csqlrcursor,"getColumnScale",
				(CAST)sqlrcur_getColumnScale,1);
	rb_define_method(csqlrcursor,"getColumnIsNullable",
				(CAST)sqlrcur_getColumnIsNullable,1);
	rb_define_method(csqlrcursor,"getColumnIsPrimaryKey",
				(CAST)sqlrcur_getColumnIsPrimaryKey,1);
	rb_define_method(csqlrcursor,"getColumnIsUnique",
				(CAST)sqlrcur_getColumnIsUnique,1);
	rb_define_method(csqlrcursor,"getColumnIsPartOfKey",
				(CAST)sqlrcur_getColumnIsPartOfKey,1);
	rb_define_method(csqlrcursor,"getColumnIsUnsigned",
				(CAST)sqlrcur_getColumnIsUnsigned,1);
	rb_define_method(csqlrcursor,"getColumnIsZeroFilled",
				(CAST)sqlrcur_getColumnIsZeroFilled,1);
	rb_define_method(csqlrcursor,"getColumnIsBinary",
				(CAST)sqlrcur_getColumnIsBinary,1);
	rb_define_method(csqlrcursor,"getColumnIsAutoIncrement",
				(CAST)sqlrcur_getColumnIsAutoIncrement,1);
	rb_define_method(csqlrcursor,"getLongest",
				(CAST)sqlrcur_getLongest,1);
	rb_define_method(csqlrcursor,"getResultSetId",
				(CAST)sqlrcur_getResultSetId,0);
	rb_define_method(csqlrcursor,"suspendResultSet",
				(CAST)sqlrcur_suspendResultSet,0);
	rb_define_method(csqlrcursor,"resumeResultSet",
				(CAST)sqlrcur_resumeResultSet,1);
	rb_define_method(csqlrcursor,"resumeCachedResultSet",
				(CAST)sqlrcur_resumeCachedResultSet,2);
	rb_define_method(csqlrcursor,"closeResultSet",
				(CAST)sqlrcur_closeResultSet,0);
}

DLEXPORT void Init_sqlrelay() {
	Init_SQLRConnection();
	Init_SQLRCursor();
}

}
