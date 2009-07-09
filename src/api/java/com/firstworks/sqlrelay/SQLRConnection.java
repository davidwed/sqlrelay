// Copyright (c) 2001  David Muse
// See the file COPYING for more information.
package com.firstworks.sqlrelay;

public class SQLRConnection {

	static {
		System.loadLibrary("SQLRConnection");
	}

	/** 
	 *  Initiates a connection to "server" on "port"
	 *  or to the unix "socket" on the local machine
	 *  and authenticates with "user" and "password".
	 *  Failed connections will be retried for 
	 *  "tries" times on interval "retrytime".
	 *  If "tries" is 0 then retries will continue
	 *  forever.  If "retrytime" is 0 then retries
	 *  will be attempted on a default interval.
	 *
	 *  If the "socket" parameter is neither 
	 *  NULL nor "" then an attempt will be made to 
	 *  connect through it before attempting to 
	 *  connect to "server" on "port".  If it is 
	 *  NULL or "" then no attempt will be made to 
	 *  connect through the socket.  */
	public SQLRConnection(String server, short port, String socket,
						String user, String password,
						int retrytime, int tries) {
		connection=alloc(server,port,socket,
						user,password,retrytime,tries);
	}
	/** Disconnects and ends the session if
	 * it hasn't been ended already.  */
	public native void	delete();

	/** Sets the server connect timeout in seconds
	 *  and milliseconds.  Setting either parameter
	 *  to -1 disables the timeout. */
	public native void	setTimeout(int timeoutsec, int timeoutusec);

	/** Ends the session. */
	public native void	endSession();

	/** Disconnects this connection from the current
	 *  session but leaves the session open so 
	 *  that another connection can connect to it 
	 *  using resumeSession().  */
	public native boolean	suspendSession();

	/** Returns the inet port that the connection is 
	 *  communicating over. This parameter may be 
	 *  passed to another connection for use in
	 *  the resumeSession() method.
	 *  Note: the value returned by this method is only
	 *  valid after a call to suspendSession().*/
	public native short	getConnectionPort();

	/** Returns the unix socket that the connection 
	 *  is communicating over. This parameter may be 
	 *  passed to another connection for use in
	 *  the resumeSession() method.
	 *  Note: the value returned by this method is only
	 *  valid after a call to suspendSession().*/
	public native String	getConnectionSocket();

	/** Resumes a session previously left open 
	 *  using suspendSession().
	 *  Returns 1 on success and 0 on failure. */
	public native boolean	resumeSession(short port, String socket);


	/** Returns 1 if the database is up and 0
	 *  if it's down.  */
	public native boolean	ping();
	/** Returns the type of database: 
	 *    oracle8, postgresql, mysql, etc.  */
	public native String	identify();
	/** Returns the version of the database */
	public native String	dbVersion();
	/** Returns the version of the sqlrelay server software */
	public native String	serverVersion();
	/** Returns the version of the sqlrelay client software */
	public native String	clientVersion();
	/** Returns a string representing the format
	 *  of the bind variables used in the db. */
	public native String	bindFormat();


	/** Instructs the database to perform a commit
	 *  after every successful query. */
	public native boolean	autoCommitOn();
	/** Instructs the database to wait for the 
	 *  client to tell it when to commit. */
	public native boolean	autoCommitOff();
	/** Issues a commit. Returns 1 if the commit succeeded, 0 if it
	 *  failed and -1 if an error occurred. */
	public native boolean	commit();
	/** Issues a rollback. Returns 1 if the rollback succeeded, 0 if it
	 *  failed and -1 if an error occurred. */
	public native boolean	rollback();


	/** Causes verbose debugging information to be 
	 *  sent to standard output.  Another way to do
	 *  this is to start a query with "-- debug\n". */
	public native void	debugOn();

	/** Turns debugging off. */
	public native void	debugOff();

	/** Returns 0 if debugging is off and 1 if 
	 *  debugging is on. */
	public native boolean	getDebug();


	/** connection is used internally, it's just
	 *  public to make the JNI wrapper work faster.  */
	public long	connection;
	private native long	alloc(String server, short port, 
						String socket, String user, 
						String password, 
						int retrytime, int tries);
}
