using System;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace SQLRClient
{

public class SQLRConnection : IDisposable
{
    /** Initiates a connection to "server" on "port" or to the unix "socket" on
     *  the local machine and authenticates with "user" and "password".  Failed
     *  connections will be retried for "tries" times on interval "retrytime".
     *  If "tries" is 0 then retries will continue forever.  If "retrytime" is 0
     *  then retries will be attempted on a default interval.
     *  If the "socket" parameter is nether NULL nor "" then an attempt will be
     *  made to connect through it before attempting to connect to "server" on
     *  "port".  If it is NULL or "" then no attempt will be made to connect
     *  through the socket.*/
    public SQLRConnection(String server, UInt16 port, String socket, String user, String password, Int32 retrytime, Int32 tries)
    {
        sqlrconref = sqlrcon_alloc_copyrefs(server, port, socket, user, password, retrytime, tries, true);
    }
    
    /** Dispose framework */
    private Boolean  disposed = false;
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }
    protected virtual void Dispose(Boolean  disposing)
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
    
    
    
    /** Sets the server connect timeout in seconds and milliseconds.
     *  Setting either parameter to -1 disables the timeout. */
    public void setTimeout(Int32 timeoutsec, Int32 timeoutusec)
    {
        sqlrcon_setTimeout(sqlrconref, timeoutsec, timeoutusec);
    }
    
    /** Ends the session. */
    public void endSession()
    {
        sqlrcon_endSession(sqlrconref);
    }
    
    /** Disconnects this connection from the current session but leaves the
     *  session open so that another connection can connect to it using
     *  sqlrcon_resumeSession(). */
    public Boolean  suspendSession()
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
    public Boolean  resumeSession(UInt16 port, String socket)
    {
    	return sqlrcon_resumeSession(sqlrconref, port, socket)!=0;
    }
    
    
    
    /** Returns true if the database is up and false if it's down. */
    public Boolean  ping()
    {
        return sqlrcon_ping(sqlrconref)!=0;
    }
    
    /** Returns the type of database: oracle8, postgresql, mysql, etc. */
    public String identify()
    {
        return sqlrcon_identify(sqlrconref);
    }
    
    /** Returns the version of the database */
    public String dbVersion()
    {
    	return sqlrcon_dbVersion(sqlrconref);
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
    public Boolean  selectDatabase(String database)
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
    public Boolean  autoCommitOn()
    {
        return sqlrcon_autoCommitOn(sqlrconref)!=0;
    }
    
    /** Instructs the database to wait for the client to tell it when to
     *  commit. */
    public Boolean  autoCommitOff()
    {
        return sqlrcon_autoCommitOff(sqlrconref)!=0;
    }
    
    
    
    /** Issues a commit.  Returns 1 if the commit succeeded, 0 if it failed and
     *  -1 if an error occurred. */
    public Int32 commit()
    {
        return sqlrcon_commit(sqlrconref);
    }
    
    /** Issues a rollback.  Returns 1 if the rollback succeeded, 0 if it failed
     *  and -1 if an error occurred. */
    public Int32 rollback()
    {
        return sqlrcon_rollback(sqlrconref);
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
     *  Yet another way is to set the environment variable SQLRDEBUG to "ON" */
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
    public Boolean  getDebug()
    {
        return sqlrcon_getDebug(sqlrconref)!=0;
    }

    /** Returns a pointer to the internal connection structure */
    public IntPtr getInternalConnectionStructure()
    {
        return sqlrconref;
    }

    private IntPtr sqlrconref;

    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr sqlrcon_alloc_copyrefs(String server, UInt16 port, String socket, String user, String password, Int32 retrytime, Int32 tries, Boolean  copyreferences);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_free(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern void sqlrcon_setTimeout(IntPtr sqlrconref, Int32 timeoutsec, Int32 timeoutusec);
    
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
}

}
