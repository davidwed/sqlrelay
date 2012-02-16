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
    public SQLRConnection(string server, ushort port, string socket, string user, string password, int retrytime, int tries)
    {
        sqlrconref = sqlrcon_alloc_copyrefs(server, port, socket, user, password, retrytime, tries, true);
    }
    
    /** Dispose framework */
    private bool disposed = false;
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }
    protected virtual void Dispose(bool disposing)
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
    public void setTimeout(int timeoutsec, int timeoutusec)
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
    public bool suspendSession()
    {
        return sqlrcon_suspendSession(sqlrconref)!=0;
    }
    
    /** Returns the inet port that the connection is communicating over.  This
     *  parameter may be passed to another connection for use in the
     *  sqlrcon_resumeSession() command.  Note: The result this function returns
     *  is only valid after a call to suspendSession(). */
    public ushort getConnectionPort()
    {
        return sqlrcon_getConnectionPort(sqlrconref);
    }
    
    /** Returns the unix socket that the connection is communicating over.  This
     *  parameter may be passed to another connection for use in the
     *  sqlrcon_resumeSession() command.  Note: The result this function returns
     *  is only valid after a call to suspendSession(). */
    public string getConnectionSocket()
    {
    	return sqlrcon_getConnectionSocket(sqlrconref);
    }
    
    /** Resumes a session previously left open using sqlrcon_suspendSession().
     *  Returns true on success and false on failure. */
    public bool resumeSession(ushort port, string socket)
    {
    	return sqlrcon_resumeSession(sqlrconref, port, socket)!=0;
    }
    
    
    
    /** Returns true if the database is up and false if it's down. */
    public bool ping()
    {
        return sqlrcon_ping(sqlrconref)!=0;
    }
    
    /** Returns the type of database: oracle8, postgresql, mysql, etc. */
    public string identify()
    {
        return sqlrcon_identify(sqlrconref);
    }
    
    /** Returns the version of the database */
    public string dbVersion()
    {
    	return sqlrcon_dbVersion(sqlrconref);
    }
    
    /** Returns the version of the sqlrelay server software. */
    public string serverVersion()
    {
        return sqlrcon_serverVersion(sqlrconref);
    }
    
    /** Returns the version of the sqlrelay client software. */
    public string clientVersion()
    {
        return sqlrcon_clientVersion(sqlrconref);
    }
    
    /** Returns a string representing the format
     *  of the bind variables used in the db. */
    public string bindFormat()
    {
        return sqlrcon_bindFormat(sqlrconref);
    }
    
    
    
    /** Sets the current database/schema to "database" */
    public bool selectDatabase(string database)
    {
        return sqlrcon_selectDatabase(sqlrconref, database)!=0;
    }
    
    /** Returns the database/schema that is currently in use. */
    public string getCurrentDatabase()
    {
        return sqlrcon_getCurrentDatabase(sqlrconref);
    }
    
    
    
    /** Returns the value of the autoincrement column for the last insert */
    public ulong getLastInsertId()
    {
        return sqlrcon_getLastInsertId(sqlrconref);
    }
    
    
    
    /** Instructs the database to perform a commit after every successful
     *  query. */
    public bool autoCommitOn()
    {
        return sqlrcon_autoCommitOn(sqlrconref)!=0;
    }
    
    /** Instructs the database to wait for the client to tell it when to
     *  commit. */
    public bool autoCommitOff()
    {
        return sqlrcon_autoCommitOff(sqlrconref)!=0;
    }
    
    
    
    /** Issues a commit.  Returns 1 if the commit succeeded, 0 if it failed and
     *  -1 if an error occurred. */
    public int commit()
    {
        return sqlrcon_commit(sqlrconref);
    }
    
    /** Issues a rollback.  Returns 1 if the rollback succeeded, 0 if it failed
     *  and -1 if an error occurred. */
    public int rollback()
    {
        return sqlrcon_rollback(sqlrconref);
    }
    
    
    
    /** If an operation failed and generated an error, the error message is
     *  available here.  If there is no error then this method returns NULL */
    public string errorMessage()
    {
        return sqlrcon_errorMessage(sqlrconref);
    }
    
    /** If an operation failed and generated an error, the error number is
     *  available here.  If there is no error then this method returns 0. */
    public long errorNumber()
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
    public bool getDebug()
    {
        return sqlrcon_getDebug(sqlrconref)!=0;
    }

    /** Returns a pointer to the internal connection structure */
    public IntPtr getInternalConnectionStructure()
    {
        return sqlrconref;
    }

    private IntPtr sqlrconref;

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern IntPtr sqlrcon_alloc_copyrefs(string server, ushort port, string socket, string user, string password, int retrytime, int tries, bool copyreferences);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcon_free(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcon_setTimeout(IntPtr sqlrconref, int timeoutsec, int timeoutusec);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcon_endSession(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcon_suspendSession(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern ushort sqlrcon_getConnectionPort(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcon_getConnectionSocket(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcon_resumeSession(IntPtr sqlrconref, ushort port, string socket);

    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcon_ping(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcon_identify(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcon_dbVersion(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcon_serverVersion(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcon_clientVersion(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcon_bindFormat(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcon_selectDatabase(IntPtr sqlrconref, string database);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcon_getCurrentDatabase(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern ulong sqlrcon_getLastInsertId(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcon_autoCommitOn(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcon_autoCommitOff(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcon_commit(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcon_rollback(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern string sqlrcon_errorMessage(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern long sqlrcon_errorNumber(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcon_debugOn(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern void sqlrcon_debugOff(IntPtr sqlrconref);
    
    [DllImport("libsqlrclientwrapper.dll")]
    private static extern int sqlrcon_getDebug(IntPtr sqlrconref);
}

}
