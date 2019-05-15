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

#include <ruby.h>
#include "../c++/sqlrelay/sqlrclient.h"

#include "rubyincludes.h"

extern "C" {

// sqlrconnection methods
static void sqlrcon_free(void *sqlrcon) {
	delete (sqlrconnection *)sqlrcon;
}

#ifndef STR2CSTR
	#define STR2CSTR(v) StringValuePtr(v)
#endif

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
	sqlrcon->setConnectTimeout(NUM2INT(timeoutsec),NUM2INT(timeoutusec));
	return Qnil;
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
	sqlrcon->setAuthenticationTimeout(NUM2INT(timeoutsec),
						NUM2INT(timeoutusec));
	return Qnil;
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
	sqlrcon->setResponseTimeout(NUM2INT(timeoutsec),NUM2INT(timeoutusec));
	return Qnil;
}

/**
 *  call-seq:
 *  setBindVariablesDelimiters(delimiters)
 *
 *  Sets which delimiters are used to identify bind variables
 *  in countBindVariables() and validateBinds().  Valid
 *  delimiters include ?,:,@, and $.  Defaults to "?:@$" */
static VALUE setBindVariableDelimiters(VALUE self, VALUE delimiters) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	sqlrcon->setBindVariableDelimiters(STR2CSTR(delimiters));
	return Qnil;
}

/** Returns true if question marks (?) are considered to be
 *  valid bind variable delimiters. */
static VALUE getBindVariableDelimiterQuestionMarkSupported(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	result=sqlrcon->getBindVariableDelimiterQuestionMarkSupported();
	return INT2NUM(result);
}

/** Returns true if colons (:) are considered to be
 *  valid bind variable delimiters. */
static VALUE getBindVariableDelimiterColonSupported(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	result=sqlrcon->getBindVariableDelimiterColonSupported();
	return INT2NUM(result);
}

/** Returns true if at-signs (@) are considered to be
 *  valid bind variable delimiters. */
static VALUE getBindVariableDelimiterAtSignSupported(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	result=sqlrcon->getBindVariableDelimiterAtSignSupported();
	return INT2NUM(result);
}

/** Returns true if dollar signs ($) are considered to be
 *  valid bind variable delimiters. */
static VALUE getBindVariableDelimiterDollarSignSupported(VALUE self) {
	sqlrconnection	*sqlrcon;
	bool result;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	result=sqlrcon->getBindVariableDelimiterDollarSignSupported();
	return INT2NUM(result);
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
	sqlrcon->enableKerberos(STR2CSTR(service),
				STR2CSTR(mech),
				STR2CSTR(flags));
	return Qnil;
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
	sqlrcon->enableTls(STR2CSTR(version),
				STR2CSTR(cert),
				STR2CSTR(password),
				STR2CSTR(ciphers),
				STR2CSTR(validate),
				STR2CSTR(ca),
				NUM2INT(depth));
	return Qnil;
}

/** Disables encryption. */
static VALUE sqlrcon_disableEncryption(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	sqlrcon->disableEncryption();
	return Qnil;
}

/** Ends the session. */
static VALUE sqlrcon_endSession(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	sqlrcon->endSession();
	return Qnil;
}

/** Disconnects this connection from the current session but leaves the session
 *  open so that another connection can connect to it using
 *  sqlrcon_resumeSession(). */
static VALUE sqlrcon_suspendSession(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->suspendSession());
}

/** Returns the inet port that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession(). */
static VALUE sqlrcon_getConnectionPort(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->getConnectionPort());
}

/** Returns the unix socket that the connection is communicating over.  This
 *  parameter may be passed to another connection for use in the
 *  sqlrcon_resumeSession() command.  Note: The result this function returns
 *  is only valid after a call to suspendSession(). */
static VALUE sqlrcon_getConnectionSocket(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->getConnectionSocket();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/**
 *  call-seq:
 *  resumeSession(port,socket)
 *
 *  Resumes a session previously left open using sqlrcon_suspendSession().
 *  Returns 1 on success and 0 on failure. */
static VALUE sqlrcon_resumeSession(VALUE self, VALUE port, VALUE socket) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->resumeSession(NUM2INT(port), 
							STR2CSTR(socket)));
}

/** Returns 1 if the database is up and 0 if it's down. */
static VALUE sqlrcon_ping(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->ping());
}

/** Returns the type of database: oracle, postgresql, mysql, etc. */
static VALUE sqlrcon_identify(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->identify();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** Returns the version of the database */
static VALUE sqlrcon_dbVersion(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->dbVersion();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** Returns the host name of the database */
static VALUE sqlrcon_dbHostName(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->dbHostName();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** Returns the ip address of the database */
static VALUE sqlrcon_dbIpAddress(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->dbIpAddress();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** Returns the version of the sqlrelay server software. */
static VALUE sqlrcon_serverVersion(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->serverVersion();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** Returns the version of the sqlrelay client software. */
static VALUE sqlrcon_clientVersion(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->clientVersion();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** Returns a string representing the format
 *  of the bind variables used in the db. */
static VALUE sqlrcon_bindFormat(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->bindFormat();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/**
 *  call-seq:
 *  selectDatabase(database)
 *
 *  Sets the current database/schema to "database" */
static VALUE sqlrcon_selectDatabase(VALUE self, VALUE db) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->selectDatabase(STR2CSTR(db)));
}

/** Returns the database/schema that is currently in use. */
static VALUE sqlrcon_getCurrentDatabase(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->getCurrentDatabase();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** Returns the value of the autoincrement column for the last insert */
static VALUE sqlrcon_getLastInsertId(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->getLastInsertId());
}

/** Instructs the database to perform a commit after every successful query. */
static VALUE sqlrcon_autoCommitOn(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->autoCommitOn());
}

/** Instructs the database to wait for the client to tell it when to commit. */
static VALUE sqlrcon_autoCommitOff(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->autoCommitOff());
}

/** Begins a transaction.  Returns true if the begin
 *  succeeded, false if it failed.  If the database
 *  automatically begins a new transaction when a
 *  commit or rollback is issued then this doesn't
 *  do anything unless SQL Relay is faking transaction
 *  blocks. */
static VALUE sqlrcon_begin(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->begin());
}

/** Issues a commit.  Returns true if the commit succeeded,
 *  false if it failed. */
static VALUE sqlrcon_commit(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->commit());
}

/** Issues a rollback.  Returns true if the rollback succeeded,
 *  false if it failed. */
static VALUE sqlrcon_rollback(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->rollback());
}

/** If an operation failed and generated an error, the error message is
 *  available here.  If there is no error then this method returns nil */
static VALUE sqlrcon_errorMessage(VALUE self) {
	sqlrconnection *sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->errorMessage();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** If an operation failed and generated an error, the error number is
 *  available here.  If there is no error then this method returns 0. */
static VALUE sqlrcon_errorNumber(VALUE self) {
	sqlrconnection *sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->errorNumber());
}

/** Causes verbose debugging information to be sent to standard output.
 *  Another way to do this is to start a query with "-- debug\n".  Yet
 *  another way is to set the environment variable SQLR_CLIENT_DEBUG
 *  to "ON" */
static VALUE sqlrcon_debugOn(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	sqlrcon->debugOn();
	return Qnil;
}

/** Turns debugging off. */
static VALUE sqlrcon_debugOff(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	sqlrcon->debugOff();
	return Qnil;
}

/** Returns 0 if debugging is off and 1 if debugging is on. */
static VALUE sqlrcon_getDebug(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->getDebug());
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
	sqlrcon->setDebugFile(STR2CSTR(filename));
	return Qnil;
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
	sqlrcon->setClientInfo(STR2CSTR(clientinfo));
	return Qnil;
}

/** Returns the string that was set by setClientInfo(). */
static VALUE sqlrcon_getClientInfo(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	const char	*result=sqlrcon->getClientInfo();
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
				(CAST)setBindVariableDelimiters,1);
	rb_define_method(csqlrconnection,
			"getBindVariableDelimiterQuestionMarkSupported",
			(CAST)getBindVariableDelimiterQuestionMarkSupported,0);
	rb_define_method(csqlrconnection,
			"getBindVariableDelimiterColonSupported",
			(CAST)getBindVariableDelimiterColonSupported,0);
	rb_define_method(csqlrconnection,
			"getBindVariableDelimiterAtSignSupported",
			(CAST)getBindVariableDelimiterAtSignSupported,0);
	rb_define_method(csqlrconnection,
			"getBindVariableDelimiterDollarSignSupported",
			(CAST)getBindVariableDelimiterDollarSignSupported,0);
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

/**
 *  call-seq:
 *  setResultSetBufferSize(rows)
 *
 *  Sets the number of rows of the result set to buffer at a time.
 *  0 (the default) means buffer the entire result set. */
static VALUE sqlrcur_setResultSetBufferSize(VALUE self, VALUE rows) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->setResultSetBufferSize(NUM2INT(rows));
	return Qnil;
}

/** Returns the number of result set rows that will be buffered at a time or
 *  0 for the entire result set. */
static VALUE sqlrcur_getResultSetBufferSize(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->getResultSetBufferSize());
}

/** Tells the server not to send any column info (names, types, sizes).  If
 *  you don't need that info, you should call this function to improve
 *  performance. */
static VALUE sqlrcur_dontGetColumnInfo(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->dontGetColumnInfo();
	return Qnil;
}

/** Tells the server to send column info. */
static VALUE sqlrcur_getColumnInfo(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->getColumnInfo();
	return Qnil;
}

/** Columns names are returned in the same case as they are defined in the
 *  database.  This is the default. */
static VALUE sqlrcur_mixedCaseColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->mixedCaseColumnNames();
	return Qnil;
}

/** Columns names are converted to upper case. */
static VALUE sqlrcur_upperCaseColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->upperCaseColumnNames();
	return Qnil;
}

/** Columns names are converted to lower case. */
static VALUE sqlrcur_lowerCaseColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->lowerCaseColumnNames();
	return Qnil;
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
	sqlrcur->cacheToFile(STR2CSTR(filename));
	return Qnil;
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
	sqlrcur->setCacheTtl(NUM2INT(ttl));
	return Qnil;
}

/** Returns the name of the file containing
 *  the most recently cached result set. */
static VALUE sqlrcur_getCacheFileName(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*result=sqlrcur->getCacheFileName();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** Sets query caching off. */
static VALUE sqlrcur_cacheOff(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->cacheOff();
	return Qnil;
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->getDatabaseList(STR2CSTR(wild)));
}

/**
 *  call-seq:
 *  getTableList(wild)
 *
 *  Sends a query that returns a list of tables matching "wild".  If wild is
 *  empty or nil then a list of all tables will be returned. */
static VALUE sqlrcur_getTableList(VALUE self, VALUE wild) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->getTableList(STR2CSTR(wild)));
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->getColumnList(STR2CSTR(table),STR2CSTR(wild)));
}

/**
 *  call-seq:
 *  sendQuery(query)
 *
 *  Sends "query" directly and gets a result set. */
static VALUE sqlrcur_sendQuery(VALUE self, VALUE query) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->sendQuery(STR2CSTR(query)));
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->sendQuery(STR2CSTR(query),NUM2INT(length)));
}

/**
 *  call-seq:
 *  sendFileQuery(path,filename)
 *
 *  Sends the query in file "path"/"filename" and gets a result set. */
static VALUE sqlrcur_sendFileQuery(VALUE self, VALUE path, VALUE filename) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->sendFileQuery(STR2CSTR(path),
						STR2CSTR(filename))); 
}

/**
 *  call-seq:
 *  prepareQuery(query)
 *
 *  Prepare to execute "query". */
static VALUE sqlrcur_prepareQuery(VALUE self, VALUE query) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->prepareQuery(STR2CSTR(query));
	return Qnil;
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
	sqlrcur->prepareQuery(STR2CSTR(query),NUM2INT(length));
	return Qnil;
}

/**
 *  call-seq:
 *  prepareFileQuery(path,filename)
 *
 *  Prepare to execute the contents of "path"/"filename". */
static VALUE sqlrcur_prepareFileQuery(VALUE self, VALUE path, VALUE filename) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->prepareFileQuery(STR2CSTR(path),
						STR2CSTR(filename)));
}

/** Clears all bind variables. */
static VALUE sqlrcur_clearBinds(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->clearBinds();
	return Qnil;
}

/** Parses the previously prepared query, counts the number of bind variables
 *  defined in it and returns that number. */
static VALUE sqlrcur_countBindVariables(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->countBindVariables());
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
		sqlrcur->substitution(STR2CSTR(variable),STR2CSTR(value));
	} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
		sqlrcur->substitution(STR2CSTR(variable),NUM2INT(value));
	} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
		sqlrcur->substitution(STR2CSTR(variable),NUM2DBL(value), 
					(unsigned short)NUM2INT(precision),
					(unsigned short)NUM2INT(scale));
	} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
		sqlrcur->substitution(STR2CSTR(variable),(const char *)NULL);
	} else {
		result=false;
	}
	return INT2NUM(result);
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
			sqlrcur->inputBind(STR2CSTR(variable),
					STR2CSTR(value),NUM2INT(precision));
		} else {
			sqlrcur->inputBind(STR2CSTR(variable),STR2CSTR(value));
		}
	} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
		sqlrcur->inputBind(STR2CSTR(variable),NUM2INT(value));
	} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
		sqlrcur->inputBind(STR2CSTR(variable),NUM2DBL(value), 
					(unsigned short)NUM2INT(precision),
					(unsigned short)NUM2INT(scale));
	} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
		sqlrcur->inputBind(STR2CSTR(variable),(const char *)NULL);
	} else {
		success=false;
	}
	return INT2NUM(success);
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
		sqlrcur->inputBindBlob(STR2CSTR(variable),NULL,NUM2INT(size));
	} else if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
		sqlrcur->inputBindBlob(STR2CSTR(variable),
				STR2CSTR(value),NUM2INT(size));
	} else {
		success=false;
	}
	return INT2NUM(success);
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
		sqlrcur->inputBindClob(STR2CSTR(variable),NULL,NUM2INT(size));
	} else if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
		sqlrcur->inputBindClob(STR2CSTR(variable),
				STR2CSTR(value),NUM2INT(size));
	} else {
		success=false;
	}
	return INT2NUM(success);
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
	sqlrcur->defineOutputBindString(STR2CSTR(variable),NUM2INT(bufferlength));
	return Qnil;
}

/**
 *  call-seq:
 *  defineOutputBindInteger(variable)
 *
 *  Defines an integer output bind variable. */
static VALUE sqlrcur_defineOutputBindInteger(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->defineOutputBindInteger(STR2CSTR(variable));
	return Qnil;
}

/**
 *  call-seq:
 *  defineOutputBindDouble(variable)
 *
 *  Defines an decimal output bind variable. */
static VALUE sqlrcur_defineOutputBindDouble(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->defineOutputBindDouble(STR2CSTR(variable));
	return Qnil;
}

/**
 *  call-seq:
 *  defineOuptutBindBlob(variable)
 *
 *  Defines a binary lob output bind variable */
static VALUE sqlrcur_defineOutputBindBlob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->defineOutputBindBlob(STR2CSTR(variable));
	return Qnil;
}

/**
 *  call-seq:
 *  defineOutputBindClob(variable)
 *
 *  Defines a character lob output bind variable */
static VALUE sqlrcur_defineOutputBindClob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->defineOutputBindClob(STR2CSTR(variable));
	return Qnil;
}

/**
 *  call-seq:
 *  defineOutputBindCursor(variable)
 *
 *  Defines a cursor output bind variable */
static VALUE sqlrcur_defineOutputBindCursor(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->defineOutputBindCursor(STR2CSTR(variable));
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
			sqlrcur->substitution(STR2CSTR(variable),
						STR2CSTR(value));
		} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
			sqlrcur->substitution(STR2CSTR(variable),
						NUM2INT(value));
		} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
			sqlrcur->substitution(STR2CSTR(variable),
					NUM2DBL(value), 
					(unsigned short)NUM2INT(precision),
					(unsigned short)NUM2INT(scale));
		} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
			sqlrcur->substitution(STR2CSTR(variable),
						(const char *)NULL);
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
			sqlrcur->inputBind(STR2CSTR(variable),STR2CSTR(value));
		} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
			sqlrcur->inputBind(STR2CSTR(variable),NUM2INT(value));
		} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
			sqlrcur->inputBind(STR2CSTR(variable),NUM2DBL(value), 
					(unsigned short)NUM2INT(precision),
					(unsigned short)NUM2INT(scale));
		} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
			sqlrcur->inputBind(STR2CSTR(variable),
						(const char *)NULL);
		} else {
			success=false;
		}
	}
	return INT2NUM(success);
}

/** If you are binding to any variables that might not actually be in your
 *  query, call this to ensure that the database won't try to bind them unless
 *  they really are in the query.  There is a performance penalty for calling
 *  this function */
static VALUE sqlrcur_validateBinds(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->validateBinds();
	return Qnil;
}

/**
 *  call-seq:
 *  validBind(variable)
 *
 *  Returns true if "variable" was a valid bind variable of the query. */
static VALUE sqlrcur_validBind(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->validBind(STR2CSTR(variable)));
}

/** Execute the query that was previously prepared and bound. */
static VALUE sqlrcur_executeQuery(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->executeQuery());
}

/** Fetch from a cursor that was returned as an output bind variable. */
static VALUE sqlrcur_fetchFromBindCursor(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->fetchFromBindCursor());
}

/**
 *  call-seq:
 *  getOutputBindString(variable)
 *
 *  Get the value stored in a previously defined string output bind variable. */
static VALUE sqlrcur_getOutputBindString(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*varname=STR2CSTR(variable);
	const char	*result=sqlrcur->getOutputBindString(varname);
	long		length=sqlrcur->getOutputBindLength(varname);
	if (result) {
		return rb_str_new(result,length);
	} else {
		return Qnil;
	}
}

/**
 *  call-seq:
 *  getOutputBindBlob(variable)
 *
 *  Get the value stored in a previously defined
 *  binary lob output bind variable. */
static VALUE sqlrcur_getOutputBindBlob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*varname=STR2CSTR(variable);
	const char	*result=sqlrcur->getOutputBindBlob(varname);
	long		length=sqlrcur->getOutputBindLength(varname);
	if (result) {
		return rb_str_new(result,length);
	} else {
		return Qnil;
	}
}

/**
 *  call-seq:
 *  getOutputBindClob(variable)
 *
 *  Get the value stored in a previously defined
 *  character lob output bind variable. */
static VALUE sqlrcur_getOutputBindClob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*varname=STR2CSTR(variable);
	const char	*result=sqlrcur->getOutputBindClob(varname);
	long		length=sqlrcur->getOutputBindLength(varname);
	if (result) {
		return rb_str_new(result,length);
	} else {
		return Qnil;
	}
}

/**
 *  call-seq:
 *  getOutputBindInteger(variable)
 *
 *  Get the value stored in a previously defined
 *  integer output bind variable. */
static VALUE sqlrcur_getOutputBindInteger(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*varname=STR2CSTR(variable);
	long		result=sqlrcur->getOutputBindInteger(varname);
	return INT2NUM(result);
}

/**
 *  call-seq:
 *  getOutputBindDouble(variable)
 *
 *  Get the value stored in a previously defined
 *  decimal output bind variable. */
static VALUE sqlrcur_getOutputBindDouble(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*varname=STR2CSTR(variable);
	double		result=sqlrcur->getOutputBindDouble(varname);
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->getOutputBindLength(STR2CSTR(variable)));
}

/**
 *  call-seq:
 *  getOutputBindCursor(variable)
 *
 *  Get the cursor associated with a previously defined output bind variable. */
static VALUE sqlrcur_getOutputBindCursor(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcursor	*returnsqlrcur=sqlrcur->getOutputBindCursor(
							STR2CSTR(variable),
							true);
	return Data_Wrap_Struct(csqlrcursor,0,sqlrcur_free,
					(void *)returnsqlrcur);
}

/**
 *  call-seq:
 *  openCachedResultSet(filename)
 *
 *  Opens a cached result set.  Returns 1 on success and 0 on failure. */
static VALUE sqlrcur_openCachedResultSet(VALUE self, VALUE filename) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->openCachedResultSet(STR2CSTR(filename)));
}

/** Returns the number of columns in the current result set. */
static VALUE sqlrcur_colCount(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->colCount());
}

/** Returns the number of rows in the current result set. */
static VALUE sqlrcur_rowCount(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->rowCount());
}

/** Returns the total number of rows that will be returned in the result set.
 *  Not all databases support this call.  Don't use it for applications which
 *  are designed to be portable across databases.  -1 is returned by databases
 *  which don't support this option. */
static VALUE sqlrcur_totalRows(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->totalRows());
}

/** Returns the number of rows that were updated, inserted or deleted by the
 *  query.  Not all databases support this call.  Don't use it for applications
 *  which are designed to be portable across databases.  -1 is returned by
 *  databases which don't support this option. */
static VALUE sqlrcur_affectedRows(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->affectedRows());
}

/** Returns the index of the first buffered row.  This is useful when buffering
 *  only part of the result set at a time. */
static VALUE sqlrcur_firstRowIndex(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->firstRowIndex());
}

/** Returns 0 if part of the result set is still pending on the server and 1 if
 *  not.  This function can only return 0 if setResultSetBufferSize() has been
 *  called with a parameter other than 0. */
static VALUE sqlrcur_endOfResultSet(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->endOfResultSet());
}

/** If a query failed and generated an error, the error message is available
 *  here.  If the query succeeded then this function returns a nil. */
static VALUE sqlrcur_errorMessage(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*result=sqlrcur->errorMessage();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

/** If a query failed and generated an error, the error number is
 *  available here.  If there is no error then this method returns 0. */
static VALUE sqlrcur_errorNumber(VALUE self) {
	sqlrcursor *sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->errorNumber());
}

/** Tells the connection to return NULL fields and output bind variables as
 *  empty strings.  This is the default. */
static VALUE sqlrcur_getNullsAsEmptyStrings(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->getNullsAsEmptyStrings();
	return Qnil;
}

/** Tells the connection to return NULL fields
 *  and output bind variables as nil's. */
static VALUE sqlrcur_getNullsAsNils(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->getNullsAsNulls();
	return Qnil;
}

/**
 *  call-seq:
 *  getField(row,col)
 *
 *  Returns the specified field as a string.  "col" may be specified as the 
 *  column name or number. */
static VALUE sqlrcur_getField(VALUE self, VALUE row, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*result;
	long		length;
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		result=sqlrcur->getField(NUM2INT(row),STR2CSTR(col));
		length=sqlrcur->getFieldLength(NUM2INT(row),STR2CSTR(col));
	} else {
		result=sqlrcur->getField(NUM2INT(row),NUM2INT(col));
		length=sqlrcur->getFieldLength(NUM2INT(row),NUM2INT(col));
	}
	if (result) {
		return rb_str_new(result,length);
	} else {
		return Qnil;
	}
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
	long	result;
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		result=sqlrcur->getFieldAsInteger(NUM2INT(row),STR2CSTR(col));
	} else {
		result=sqlrcur->getFieldAsInteger(NUM2INT(row),NUM2INT(col));
	}
	return INT2NUM(result);
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
		result=sqlrcur->getFieldAsDouble(NUM2INT(row),STR2CSTR(col));
	} else {
		result=sqlrcur->getFieldAsDouble(NUM2INT(row),NUM2INT(col));
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM((int)sqlrcur->getFieldLength(NUM2INT(row),
								STR2CSTR(col)));
	} else {
		return INT2NUM((int)sqlrcur->getFieldLength(NUM2INT(row),
								NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getRow(row)
 *
 *  Returns an array of the values of the fields in the specified row. */
static VALUE sqlrcur_getRow(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char * const *fields=sqlrcur->getRow(NUM2INT(row));
	uint32_t	*lengths=sqlrcur->getRowLengths(NUM2INT(row));
	VALUE	fieldary=rb_ary_new2(sqlrcur->colCount());
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		if (fields[i]) {
			rb_ary_store(fieldary,i,rb_str_new(fields[i],
								lengths[i]));
		} else {
			rb_ary_store(fieldary,i,Qnil);
		}
	}
	return fieldary;
}

/**
 *  call-seq:
 *  getRowHash(row)
 *
 *  Returns a hash of the values of the fields in the specified row. */
static VALUE sqlrcur_getRowHash(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char * const *fields=sqlrcur->getRow(NUM2INT(row));
	uint32_t	*lengths=sqlrcur->getRowLengths(NUM2INT(row));
	VALUE	fieldhash=rb_hash_new();
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		if (fields[i]) {
			rb_hash_aset(fieldhash,
					rb_str_new2(sqlrcur->getColumnName(i)),
					rb_str_new(fields[i],lengths[i]));
		} else {
			rb_hash_aset(fieldhash,
					rb_str_new2(sqlrcur->getColumnName(i)),
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	uint32_t	*lengths=sqlrcur->getRowLengths(NUM2INT(row));
	if (!lengths) {
		return Qnil;
	}
	VALUE	lengthary=rb_ary_new2(sqlrcur->colCount());
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		rb_ary_store(lengthary,i,INT2NUM(lengths[i]));
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	uint32_t	*lengths=sqlrcur->getRowLengths(NUM2INT(row));
	VALUE	lengthhash=rb_hash_new();
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		rb_hash_aset(lengthhash,
				rb_str_new2(sqlrcur->getColumnName(i)),
				INT2NUM(lengths[i]));
	}
	return lengthhash;
}

/** Returns an array of the column names of the current result set. */
static VALUE sqlrcur_getColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char * const *names=sqlrcur->getColumnNames();
	if (!names) {
		return Qnil;
	}
	VALUE	nameary=rb_ary_new2(sqlrcur->colCount());
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {
		if (names[i]) {
			rb_ary_store(nameary,i,rb_str_new2(names[i]));
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	const char	*result=sqlrcur->getColumnName(NUM2INT(col));
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
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
		result=sqlrcur->getColumnType(STR2CSTR(col));
	} else {
		result=sqlrcur->getColumnType(NUM2INT(col));
	}
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}
 
/**
 *  call-seq:
 *  getColumnLength(col)
 *
 *  Returns the length of the specified column. "col" may be specified as the
 *  column name or number. */
static VALUE sqlrcur_getColumnLength(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnLength(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnLength(NUM2INT(col)));
	}
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnPrecision(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnPrecision(NUM2INT(col)));
	}
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnScale(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnScale(NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getColumnIsNullable(col)
 *
 *  Returns 1 if the specified column can contain nulls and 0 otherwise.
 *  "col" may be specified as the colum name or number. */
static VALUE sqlrcur_getColumnIsNullable(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnIsNullable(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnIsNullable(NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getColumnIsPrimaryKey(col)
 *
 *  Returns 1 if the specified column is a primary key and 0 otherwise.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsPrimaryKey(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnIsPrimaryKey(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnIsPrimaryKey(NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getColumnIsUnique(col)
 *
 *  Returns 1 if the specified column is unique and 0 otherwise. 
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsUnique(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnIsUnique(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnIsUnique(NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getColumnIsPartOfKey(col)
 *
 *  Returns 1 if the specified column is part of a composite key and 0
 *  otherwise.  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsPartOfKey(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnIsPartOfKey(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnIsPartOfKey(NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getColumnIsUnsigned(col)
 *
 *  Returns 1 if the specified column is an unsigned number and 0 otherwise.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsUnsigned(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnIsUnsigned(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnIsUnsigned(NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getColumnIsZeroFilled(col)
 *
 *  Returns 1 if the specified column was created with the zero-fill flag and
 *  0 otherwise.  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsZeroFilled(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnIsZeroFilled(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnIsZeroFilled(NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getColumnIsBinary(col)
 *
 *  Returns 1 if the specified column contains binary data and 0 otherwise.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsBinary(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnIsBinary(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnIsBinary(NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getColumnIsAutoIncrement(col)
 *  
 *  Returns 1 if the specified column auto-increments and 0 otherwise.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getColumnIsAutoIncrement(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(
			sqlrcur->getColumnIsAutoIncrement(STR2CSTR(col)));
	} else {
		return INT2NUM(
			sqlrcur->getColumnIsAutoIncrement(NUM2INT(col)));
	}
}

/**
 *  call-seq:
 *  getLongest(col)
 *
 *  Returns the length of the longest field in the specified column.
 *  "col" may be specified as the column name or number. */
static VALUE sqlrcur_getLongest(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getLongest(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getLongest(NUM2INT(col)));
	}
}

/** Returns the internal ID of this result set.  This parameter may be passed
 *  to another statement for use in the resumeResultSet() function.  Note: The
 *  value this function returns is only valid after a call to
 *  suspendResultSet().*/
static VALUE sqlrcur_getResultSetId(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->getResultSetId());
}

/** Tells the server to leave this result set open when the connection calls
 *  suspendSession() so that another connection can connect to it using
 *  resumeResultSet() after it calls resumeSession(). */
static VALUE sqlrcur_suspendResultSet(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->suspendResultSet();
	return Qnil;
}

/**
 *  call-seq:
 *  resumeResultSet(id)
 *
 *  Resumes a result set previously left open using suspendSession().
 *  Returns 1 on success and 0 on failure. */
static VALUE sqlrcur_resumeResultSet(VALUE self, VALUE id) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->resumeResultSet(NUM2INT(id)));
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
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->resumeCachedResultSet(NUM2INT(id),
							STR2CSTR(filename)));
}

/** Closes the current result set, if one is open.  Data
 *  that has been fetched already is still available but
 *  no more data may be fetched.  Server side resources
 *  for the result set are freed as well. */
static VALUE sqlrcur_closeResultSet(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->closeResultSet();
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
