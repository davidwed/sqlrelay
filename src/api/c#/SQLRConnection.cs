using System.Runtime.InteropServices;

public class SQLRConnection
{

    /** 
     *  Initiates a connection to "server" on "port" or to the unix "socket" on
     *  the local machine and authenticates with "user" and "password".  Failed
     *  connections will be retried for "tries" times on interval "retrytime".
     *  If "tries" is 0 then retries will continue forever.  If "retrytime" is 0
     *  then retries will be attempted on a default interval.
     *  If the "socket" parameter is nether NULL nor "" then an attempt will be
     *  made to connect through it before attempting to connect to "server" on
     *  "port".  If it is NULL or "" then no attempt will be made to connect
     *  through the socket.*/
    [DllImport("libsqlrclientwrapper.dll")]
    IntPtr sqlrcon_alloc(string server, ushort port, string socket, string user, string password, int retrytime, int tries);
    
    /** 
     *  Disconnects and ends the session if it hasn't been terminated
     *  already. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcon_free(IntPtr sqlrconref);
    
    
    
    /** 
     *  Sets the server connect timeout in seconds and milliseconds.
     *  Setting either parameter to -1 disables the timeout. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcon_setTimeout(IntPtr sqlrconref, int timeoutsec, int timeoutusec);
    
    /** 
     *  Ends the session. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcon_endSession(IntPtr sqlrconref);
    
    /** 
     *  Disconnects this connection from the current session but leaves the
     *  session open so that another connection can connect to it using
     *  sqlrcon_resumeSession(). */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcon_suspendSession(IntPtr sqlrconref);
    
    /** 
     *  Returns the inet port that the connection is communicating over.  This
     *  parameter may be passed to another connection for use in the
     *  sqlrcon_resumeSession() command.  Note: The result this function returns
     *  is only valid after a call to suspendSession(). */
    [DllImport("libsqlrclientwrapper.dll")]
    ushort sqlrcon_getConnectionPort(IntPtr sqlrconref);
    
    /** 
     *  Returns the unix socket that the connection is communicating over.  This
     *  parameter may be passed to another connection for use in the
     *  sqlrcon_resumeSession() command.  Note: The result this function returns
     *  is only valid after a call to suspendSession(). */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcon_getConnectionSocket(IntPtr sqlrconref);
    
    /** 
     *  Resumes a session previously left open using sqlrcon_suspendSession().
     *  Returns 1 on success and 0 on failure. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcon_resumeSession(IntPtr sqlrconref, ushort port, string socket);
    
    
    
    /** 
     *  Returns 1 if the database is up and 0 if it's down. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcon_ping(IntPtr sqlrconref);
    
    /** 
     *  Returns the type of database: oracle8, postgresql, mysql, etc. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcon_identify(IntPtr sqlrconref);
    
    /** 
     *  Returns the version of the database */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcon_dbVersion(IntPtr sqlrconref);
    
    /** 
     *  Returns the version of the sqlrelay server software. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcon_serverVersion(IntPtr sqlrconref);
    
    /** 
     *  Returns the version of the sqlrelay client software. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcon_clientVersion(IntPtr sqlrconref);
    
    /** 
     *  Returns a string representing the format
     *  of the bind variables used in the db. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcon_bindFormat(IntPtr sqlrconref);
    
    
    
    /** 
     *  Sets the current database/schema to "database" */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcon_selectDatabase(IntPtr sqlrconref, string database);
    
    /** 
     *  Returns the database/schema that is currently in use. */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcon_getCurrentDatabase(IntPtr sqlrconref);
    
    
    
    /** 
     *  Returns the value of the autoincrement column for the last insert */
    [DllImport("libsqlrclientwrapper.dll")]
    ulong sqlrcon_getLastInsertId(IntPtr sqlrconref);
    
    
    
    /** 
     *  Instructs the database to perform a commit after every successful
     *  query. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcon_autoCommitOn(IntPtr sqlrconref);
    
    /** 
     *  Instructs the database to wait for the client to tell it when to
     *  commit. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcon_autoCommitOff(IntPtr sqlrconref);
    
    
    
    /** 
     *  Issues a commit.  Returns 1 if the commit succeeded, 0 if it failed and
     *  -1 if an error occurred. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcon_commit(IntPtr sqlrconref);
    
    /** 
     *  Issues a rollback.  Returns 1 if the rollback succeeded, 0 if it failed
     *  and -1 if an error occurred. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcon_rollback(IntPtr sqlrconref);
    
    
    
    /** 
     *  If an operation failed and generated an error, the error message is
     *  available here.  If there is no error then this method returns NULL */
    [DllImport("libsqlrclientwrapper.dll")]
    string sqlrcon_errorMessage(IntPtr sqlrconref);
    
    /** 
     *  If an operation failed and generated an error, the error number is
     *  available here.  If there is no error then this method returns 0. */
    [DllImport("libsqlrclientwrapper.dll")]
    long sqlrcon_errorNumber(IntPtr sqlrconref);
    
    
    /** 
     *  Causes verbose debugging information to be sent to standard output.
     *  Another way to do this is to start a query with "-- debug\n".
     *  Yet another way is to set the environment variable SQLRDEBUG to "ON" */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcon_debugOn(IntPtr sqlrconref);
    
    /** 
     *  Turns debugging off. */
    [DllImport("libsqlrclientwrapper.dll")]
    void sqlrcon_debugOff(IntPtr sqlrconref);
    
    /** 
     *  Returns 0 if debugging is off and 1 if debugging is on. */
    [DllImport("libsqlrclientwrapper.dll")]
    int sqlrcon_getDebug(IntPtr sqlrconref);

}
