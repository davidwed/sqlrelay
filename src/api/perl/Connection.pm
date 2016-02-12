# Copyright (c) 2000-2001  David Muse
# See the file COPYING for more information

package SQLRelay::Connection;

require DynaLoader;
@ISA = 'DynaLoader';

bootstrap SQLRelay::Connection;

1;
__END__

=head1 NAME

    SQLRelay::Connection - Perl API for SQL Relay

=head1 SYNOPSIS

        use SQLRelay::Connection;
        use SQLRelay::Cursor;

        my $sc=SQLRelay::Connection->new("testhost",9000,"",
                                          "testuser","testpassword",0,1);
        my $ss=SQLRelay::Cursor->new($sc);

        $ss->sendQuery("select table_name from user_tables");
        $sc->endSession();

        for (my $i=0; $i<$ss->rowCount(); $i++) {
                print $ss->getField($i,"table_name"), "\n";
        }

=head1 DESCRIPTION

    SQLRelay::Connection

        new(server, port, socket, user, password, retrytime, tries);
            # Initiates a connection to "server" on "port"
            # or to the unix "socket" on the local machine
            # and auths with "user" and "password".
            # Failed connections will be retried for 
            # "tries" times, waiting "retrytime" seconds between each
            # try.  If "tries" is 0 then retries will continue forever.
            # If "retrytime" is 0 then retries will be attempted on
            # a default interval.
            #
            # If the "socket" parameter is neither
            # NULL nor "" then an attempt will be made to
            # connect through it before attempting to
            # connect to "server" on "port".  If it is
            # NULL or "" then no attempt will be made to
            # connect through the socket.

        DESTROY();
            # Disconnects and ends the session if
            # it hasn't been ended already.

        setConnectTimeout(timeoutsec, timeoutusec);
            # Sets the server connect timeout in seconds and
            # milliseconds.  Setting either parameter to -1 disables the
            # timeout.  You can also set this timeout using the
            # SQLR_CLIENT_CONNECT_TIMEOUT environment variable.

        setAuthenticationTimeout(timeoutsec, timeoutusec);
            # Sets the authentication timeout in seconds and
            # milliseconds.  Setting either parameter to -1 disables the
            # timeout.   You can also set this timeout using the
            # SQLR_CLIENT_AUTHENTICATION_TIMEOUT environment variable. */

        setResponseTimeout(timeoutsec, timeoutusec);
            # Sets the response timeout (for queries, commits, rollbacks,
            # pings, etc.) in seconds and milliseconds.  Setting either
            # parameter to -1 disables the timeout.  You can also set
            # this timeout using the SQLR_CLIENT_RESPONSE_TIMEOUT
            # environment variable.

        endSession();
            # Ends the session.

        suspendSession();
            # Leaves the session open so another client
            # can connect to it.
            
        getConnectionPort();
            # Returns the inet port that the client is 
            # communicating over. This parameter may be 
            # passed to another client for use in
            # the resumeSession() command below.
            # Note: the value returned by this method is
            # only valid after a call to suspendSession().

        getConnectionSocket();
            # Returns the unix socket that the client is 
            # communicating over. This parameter may be 
            # passed to another client for use in
            # the resumeSession() command below.
            # Note: the value returned by this method is
            # only valid after a call to suspendSession().

        resumeSession(port,socket);
            # Resumes a session previously left open 
            # using suspendSession().
            # Returns true on success and false on failure.


        ping();
            # Returns true if the database is up and false
            # if it's down.

        identify();
            # Returns the type of database:
            #   oracle, postgresql, mysql, etc.

        dbVersion();
            # Returns the version of the database

        dbHostName();
            # Returns the host name of the database

        dbIpAddress();
            # Returns the ip address of the database

        serverVersion();
            # Returns the version of the SQL Relay server software

        clientVersion();
            # Returns the version of the SQL Relay client software

        bindFormat();
            # Returns a string representing the format
            # of the bind variables used in the db.


        selectDatabase(database);
            # Sets the current database/schema to "database"
        getCurrentDatabase();
            # Returns the database/schema that is currently in use.

        getLastInsertId();
            # Returns the value of the autoincrement
            # column for the last insert

        autoCommitOn();
            # Instructs the database to perform a commit
            # after every successful query.
            # Returns true if setting autocommit on succeeded
            # and false if it failed.

        autoCommitOff();
            # Instructs the database to wait for the 
            # client to tell it when to commit.
            # Returns true if setting autocommit off succeeded
            # and false if it failed.

        begin();
            # Begins a transaction.  Returns true if the begin
            # succeeded, false if it failed.  If the database
            # automatically begins a new transaction when a
            # commit or rollback is issued then this doesn't
            # do anything unless SQL Relay is faking transaction
            # blocks.

        commit();
            # Issues a commit.  Returns true if the commit
            # succeeded, false if it failed.

        rollback();
            # Issues a rollback.  Returns true if the rollback
            # succeeded, false if it failed.


        errorMessage();
            # If an operation failed and generated an error, the
            # error message is available here.  If there is no
            # error then this method returns NULL.

        errorNumber();
            # If an operation failed and generated an
            # error, the error number is available here.
            # If there is no error then this method 
            # returns 0.


        debugOn();
            # Causes verbose debugging information to be 
            # sent to standard output.  Another way to do 
            # this is to start a query with "-- debug\n".
            # Yet another way is to set the environment
            # variable SQLR_CLIENT_DEBUG to "ON"

        debugOff();
            # Turns debugging off.

        getDebug();
            # Returns true if debugging is currently on and false
            # if debugging is currently off.

        setDebugFile(filename);
            # Allows you to specify a file to write debug to.
            # Setting "filename" to NULL or an empty string causes debug
            # to be written to standard output (the default).

        setClientInfo(clientinfo);
            # Allows you to set a string that will be passed to the
            # server and ultimately included in server-side logging
            # along with queries that were run by this instance of
            # the client.

        getClientInfo();
            # Returns the string that was set by setClientInfo().

=head1 AUTHOR

    David Muse
    david.muse@firstworks.com

=cut
