# Copyright (c) 2003 Takeshi Taguchi
# See the file COPYING for more information

# Initiates a connection to "server" on "port"
# or to the unix "socket" on the local machine
# and authenticates with "user" and "password".
# Failed connections will be retried for 
# "tries" times, waiting "retrytime" seconds
# between each try.  If "tries" is 0 then retries
# will continue forever.  If "retrytime" is 0 then
# retries will be attempted on a default interval.
#
# If the "socket" parameter is neither 
# NULL nor "" then an attempt will be made to 
# connect through it before attempting to 
# connect to "server" on "port".  If it is 
# NULL or "" then no attempt will be made to 
# connect through the socket.
proc sqlrconCmd {server port socket user password retrytime tries} 


# Disconnects and ends the session if
# it hasn't been ended already.
proc sqlrconDelete {} 



# Sets the server connect timeout in seconds and
# milliseconds.  Setting either parameter to -1 disables the
# timeout.  You can also set this timeout using the
# SQLR_CLIENT_CONNECT_TIMEOUT environment variable.
proc setConnectTimeout {timeoutsec timeoutusec} 

# Sets the authentication timeout in seconds and
# milliseconds.  Setting either parameter to -1 disables the
# timeout.   You can also set this timeout using the
# SQLR_CLIENT_AUTHENTICATION_TIMEOUT environment variable.
proc setAuthenticationTimeout {timeoutsec timeoutusec} 

# Sets the response timeout (for queries, commits, rollbacks,
# pings, etc.) in seconds and milliseconds.  Setting either
# parameter to -1 disables the timeout.  You can also set
# this timeout using the SQLR_CLIENT_RESPONSE_TIMEOUT
# environment variable.
proc setResponseTimeout {timeoutsec timeoutusec} 



# Ends the session.
proc endSession {} 

# Disconnects this connection from the current
# session but leaves the session open so 
# that another connection can connect to it 
# using resumeSession {}.
proc suspendSession {} 

# Returns the inet port that the connection is 
# communicating over. This parameter may be 
# passed to another connection for use in
# the resumeSession {} method.
# Note: The value this method returns is only
# valid after a call to suspendSession {}.
proc getConnectionPort {} 

# Returns the unix socket that the connection 
# is communicating over. This parameter may be 
# passed to another connection for use in
# the resumeSession {} method.
# Note: The value this method returns is only
# valid after a call to suspendSession {}.
proc getConnectionSocket {} 

# Resumes a session previously left open 
# using suspendSession {}.
# Returns true on success and false on failure.
proc resumeSession {port socket} 



# Returns true if the database is up and false
# if it's down.
proc ping {} 

# Returns the type of database: 
# oracle postgresql mysql etc.
proc identify {} 

# Returns the version of the database
proc dbVersion {} 

# Returns the host name of the database
proc dbHostName {} 

# Returns the ip address of the database
proc dbIpAddress {} 

# Returns the version of the sqlrelay server software.
proc serverVersion {} 

# Returns the version of the sqlrelay client software.
proc clientVersion {} 

# Returns a string representing the format
# of the bind variables used in the db.
proc bindFormat {} 



# Sets the current database/schema to "database"
proc selectDatabase {database} 

# Returns the database/schema that is currently in use.
proc getCurrentDatabase {} 



# Returns the value of the autoincrement
# column for the last insert
proc getLastInsertId {} 



# Instructs the database to perform a commit
# after every successful query.
proc autoCommitOn {} 

# Instructs the database to wait for the 
# client to tell it when to commit.
proc autoCommitOff {} 



# Begins a transaction.  Returns true if the begin
# succeeded, false if it failed.  If the database
# automatically begins a new transaction when a
# commit or rollback is issued then this doesn't
# do anything unless SQL Relay is faking transaction
# blocks.
proc begin {} 

# Issues a commit.  Returns true if the commit
# succeeded false if it failed.
proc commit {} 

# Issues a rollback.  Returns true if the rollback
# succeeded false if it failed.
proc rollback {} 



# If an operation failed and generated an
# error the error message is available here.
# If there is no error then this method 
# returns NULL.
proc errorMessage {} 



# If an operation failed and generated an
# error, the error number is available here.
# If there is no error then this method 
# returns 0.
proc errorNumber {}



# Causes verbose debugging information to be 
# sent to standard output.  Another way to do
# this is to start a query with "-- debug\n".
# Another way is to set the environment variable
# SQLR_CLIENT_DEBUG to "ON"
proc debugOn {} 

# Turns debugging off.
proc debugOff {} 

# Returns false if debugging is off and true
# if debugging is on.
proc getDebug {} 

# Allows you to specify a file to write debug to.
# Setting "debugfilename" to NULL or an empty string causes debug
# to be written to standard output (the default).
proc setDebugFile {debugfilename}

# Allows you to set a string that will be passed to the
# server and ultimately included in server-side logging
# along with queries that were run by this instance of
# the client.
proc setClientInfo {clientinfo} 

# Returns the string that was set by setClientInfo().
proc getClientInfo {} 
