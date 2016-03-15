// Copyright (c) 2012-2015  David Muse
// See the file COPYING for more information

using System;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace SQLRClient
{

public class SQLRConnection : IDisposable
{
    /** Initiates a connection to "server" on "port" or to the unix "socket" on
     *  the local machine and auths with "user" and "password".  Failed
     *  connections will be retried for "tries" times, waiting "retrytime"
     *  seconds between each try.  If "tries" is 0 then retries will continue
     *  forever.  If "retrytime" is 0 then retries will be attempted on a
     *  default interval.  If the "socket" parameter is nether NULL nor "" then
     *  an attempt will be made to connect through it before attempting to
     *  connect to "server" on "port".  If it is NULL or "" then no attempt
     *  will be made to connect through the socket.*/
    public SQLRConnection(String server, UInt16 port, String socket, String user, String password, Int32 retrytime, Int32 tries)
    {
        sqlrconref = sqlrcon_alloc_copyrefs(server, port, socket, user, password, retrytime, tries, true);
    }
    
    /** Dispose framework */
    private Boolean disposed = false;
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }
    protected virtual void Dispose(Boolean disposing)
    {
        if (!disposed)
        {
            sqlrcon_free(sqlrconref);
            disposed = true;
        }
    }

    /** Disconnects and ends the session if it hasn't been terminated
     *  already. */
    ~SQLRConnection()
    {
        Dispose(false);
    }
    
    
    
    /** Sets the server connect timeout in seconds and milliseconds.  Setting
     *  either parameter to -1 disables the timeout.  You can also set this
     *  timeout using the SQLR_CLIENT_CONNECT_TIMEOUT environment variable. */
    public void setConnectTimeout(Int32 timeoutsec, Int32 timeoutusec)
    {
        sqlrcon_setConnectTimeout(sqlrconref, timeoutsec, timeoutusec);
    }
    
    /** Sets the authentication timeout in seconds and milliseconds.  Setting
     *  either parameter to -1 disables the timeout.   You can also set this
     *  timeout using the SQLR_CLIENT_AUTHENTICATION_TIMEOUT environment
     *  variable. */
    public void setAuthenticationTimeout(Int32 timeoutsec, Int32 timeoutusec)
    {
        sqlrcon_setAuthenticationTimeout(sqlrconref, timeoutsec, timeoutusec);
    }
    
    /** Sets the response timeout (for queries, commits, rollbacks, pings,
      * etc.) in seconds and milliseconds.  Setting either parameter to -1
      * disables the timeout.  You can also set this timeout using the
      * SQLR_CLIENT_RESPONSE_TIMEOUT environment variable. */
    public void setResponseTimeout(Int32 timeoutsec, Int32 timeoutusec)
    {
        sqlrcon_setResponseTimeout(sqlrconref, timeoutsec, timeoutusec);
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
    public void enableKerberos(String service, String mech, String flags)
    {
        sqlrcon_enableKerberos(sqlrconref, service, mech, flags);
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
    public void enableTls(String version, String cert, String password, String ciphers, String validate, String ca, UInt16 depth)
    {
        sqlrcon_enableTls(sqlrconref, version, cert, password, ciphers, validate, ca, depth);
    }

    /** Disables encryption. */
    public void disableEncryption()
    {
        sqlrcon_disableEncryption(sqlrconref);
    }



    /** Ends the session. */
    public void endSession()
    {
        sqlrcon_endSession(sqlrconref);
    }
    
    /** Disconnects this connection from the current session but leaves the
     *  session open so that another connection can connect to it using
     *  sqlrcon_resumeSession(). */
    public Boolean suspendSession()
    {
        return sqlrcon_suspendSession(sqlrconref)!=0;
    }
    
    /** Returns the inet port that the connection is communicating over.  This
     *  parameter may be passed to another connection for use in the
     *  sqlrcon_resumeSession() command.  Note: The result this function returns
     *  is only valid after a call to suspendSession(). */
    public UInt16 getConnectionPort()
    {
        return sqlrcon_getConnectionPort(sqlrconref);
    }
    
    /** Returns the unix socket that the connection is communicating over.  This
     *  parameter may be passed to another connection for use in the
     *  sqlrcon_resumeSession() command.  Note: The result this function returns
     *  is only valid after a call to suspendSession(). */
    public String getConnectionSocket()
    {
    	return sqlrcon_getConnectionSocket(sqlrconref);
    }
    
    /** Resumes a session previously left open using sqlrcon_suspendSession().
     *  Returns true on success and false on failure. */
    public Boolean resumeSession(UInt16 port, String socket)
    {
    	return sqlrcon_resumeSession(sqlrconref, port, socket)!=0;
    }
    
    
    
    /** Returns true if the database is up and false if it's down. */
    public Boolean ping()
    {
        return sqlrcon_ping(sqlrconref)!=0;
    }
    
    /** Returns the type of database: oracle, postgresql, mysql, etc. */
    public String identify()
    {
        return sqlrcon_identify(sqlrconref);
    }
    
    /** Returns the version of the database */
    public String dbVersion()
    {
    	return sqlrcon_dbVersion(sqlrconref);
    }
    
    /** Returns the host name of the database */
    public String dbHostName()
    {
    	return sqlrcon_dbHostName(sqlrconref);
    }
    
    /** Returns the ip address of the database */
    public String dbIpAddress()
    {
    	return sqlrcon_dbIpAddress(sqlrconref);
    }
    
    /** Returns the version of the sqlrelay server software. */
    public String serverVersion()
    {
        return sqlrcon_serverVersion(sqlrconref);
    }
    
    /** Returns the version of the sqlrelay client software. */
    public String clientVersion()
    {
        return sqlrcon_clientVersion(sqlrconref);
    }
    
    /** Returns a String representing the format
     *  of the bind variables used in the db. */
    public String bindFormat()
    {
        return sqlrcon_bindFormat(sqlrconref);
    }
    
    
    
    /** Sets the current database/schema to "database" */
    public Boolean selectDatabase(String database)
    {
        return sqlrcon_selectDatabase(sqlrconref, database)!=0;
    }
    
    /** Returns the database/schema that is currently in use. */
    public String getCurrentDatabase()
    {
        return sqlrcon_getCurrentDatabase(sqlrconref);
    }
    
    
    
    /** Returns the value of the autoincrement column for the last insert */
    public UInt64 getLastInsertId()
    {
        return sqlrcon_getLastInsertId(sqlrconref);
    }
    
    
    
    /** Instructs the database to perform a commit after every successful
     *  query. */
    public Boolean autoCommitOn()
    {
        return sqlrcon_autoCommitOn(sqlrconref)!=0;
    }
    
    /** Instructs the database to wait for the client to tell it when to
     *  commit. */
    public Boolean autoCommitOff()
    {
        return sqlrcon_autoCommitOff(sqlrconref)!=0;
    }

    /** Begins a transaction.  Returns true if the begin
     *  succeeded, false if it failed.  If the database
     *  automatically begins a new transaction when a
     *  commit or rollback is issued then this doesn't
     *  do anything unless SQL Relay is faking transaction
     *  blocks. */
    public Boolean begin()
    {
        return (sqlrcon_begin(sqlrconref) == 1);
    }
    
    
    /** Issues a commit.  Returns true if the commit succeeded and false if it failed. */
    public Boolean commit()
    {
        return (sqlrcon_commit(sqlrconref) == 1);
    }
    
    /** Issues a rollback.  Returns true if the rollback succeeded, false if it failed. */
    public Boolean rollback()
    {
        return (sqlrcon_rollback(sqlrconref) == 1);
    }
    
    
    
    /** If an operation failed and generated an error, the error message is
     *  available here.  If there is no error then this method returns NULL */
    public String errorMessage()
    {
        return sqlrcon_errorMessage(sqlrconref);
    }
    
    /** If an operation failed and generated an error, the error number is
     *  available here.  If there is no error then this method returns 0. */
    public Int64 errorNumber()
    {
        return sqlrcon_errorNumber(sqlrconref);
    }
    
    
    /** Causes verbose debugging information to be sent to standard output.
     *  Another way to do this is to start a query with "-- debug\n".
     *  Yet another way is to set the environment variable SQLR_CLIENT_DEBUG
     *  to "ON" */
    public void debugOn()
    {
        sqlrcon_debugOn(sqlrconref);
    }
    
    /** Turns debugging off. */
    public void debugOff()
    {
        sqlrcon_debugOff(sqlrconref);
    }
    
    /** Returns false if debugging is off and true if debugging is on. */
    public Boolean getDebug()
    {
        return sqlrcon_getDebug(sqlrconref)!=0;
    }

    /** Allows you to specify a file to write debug to.
     *  Setting "filename" to NULL or an empty string causes debug
     *  to be written to standard output (the default). */
    public void setDebugFile(String filename)
    {
        sqlrcon_setDebugFile(sqlrconref,filename);
    }

    /** Allows you to set a string that will be passed to the server and
     *  ultimately included in server-side logging along with queries that were
     *  run by this instance of the client. */
    public void setClientInfo(String clientinfo)
    {
        sqlrcon_setClientInfo(sqlrconref,clientinfo);
    }

    /** Returns the string that was set by setClientInfo(). */
    public String getClientInfo()
    {
        return sqlrcon_getClientInfo(sqlrconref);
    }

    /** Returns a pointer to the internal connection structure */
    public IntPtr getInternalConnectionStructure()
    {
        return sqlrconref;
    }

    private IntPtr sqlrconref;

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr sqlrcon_alloc_copyrefs(String server, UInt16 port, String socket, String user, String password, Int32 retrytime, Int32 tries, Boolean copyreferences);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_free(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_setConnectTimeout(IntPtr sqlrconref, Int32 timeoutsec, Int32 timeoutusec);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_setAuthenticationTimeout(IntPtr sqlrconref, Int32 timeoutsec, Int32 timeoutusec);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_setResponseTimeout(IntPtr sqlrconref, Int32 timeoutsec, Int32 timeoutusec);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_enableKerberos(IntPtr sqlrconref, String service, String mech, String flags);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_enableTls(IntPtr sqlrconref, String versoin, String cert, String password, String ciphers, String validate, String ca, UInt16 depth);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_disableEncryption(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_endSession(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_suspendSession(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt16 sqlrcon_getConnectionPort(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_getConnectionSocket(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_resumeSession(IntPtr sqlrconref, UInt16 port, String socket);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_ping(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_identify(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_dbVersion(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_dbHostName(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_dbIpAddress(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_serverVersion(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_clientVersion(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_bindFormat(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_selectDatabase(IntPtr sqlrconref, String database);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_getCurrentDatabase(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern UInt64 sqlrcon_getLastInsertId(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_autoCommitOn(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_autoCommitOff(IntPtr sqlrconref);

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_begin(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_commit(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_rollback(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_errorMessage(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int64 sqlrcon_errorNumber(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_debugOn(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_debugOff(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern Int32 sqlrcon_getDebug(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_setDebugFile(IntPtr sqlrconref, String filename);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_setClientInfo(IntPtr sqlrconref, String clientinfo);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern String sqlrcon_getClientInfo(IntPtr sqlrconref);
}

}
