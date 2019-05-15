# Copyright (c) 1999-2018 David Muse
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

        setBindVariableDelimiters(delimiters);
            # Sets which delimiters are used to identify bind variables
            # in countBindVariables() and validateBinds().  Valid
            # delimiters include ?,:,@, and $.  Defaults to "?:@$" */

        getBindVariableDelimiterQuestionMarkSupported();
            # Returns true if question marks (?) are considered to be
            # valid bind variable delimiters. */

        getBindVariableDelimiterColonSupported();
            # Returns true if colons (:) are considered to be
            # valid bind variable delimiters. */

        getBindVariableDelimiterAtSignSupported();
            # Returns true if at-signs (@) are considered to be
            # valid bind variable delimiters. */

        getBindVariableDelimiterDollarSignSupported();
            # Returns true if dollar signs ($) are considered to be
            # valid bind variable delimiters. */

        enableKerberos(service, mech, flags);
            # Enables Kerberos authentication and encryption.
            #
            #  "service" indicates the Kerberos service name of the
            #  SQL Relay server.  If left empty or NULL then the service
            #  name "sqlrelay" will be used. "sqlrelay" is the default
            #  service name of the SQL Relay server.  Note that on Windows
            #  platforms the service name must be fully qualified,
            #  including the host and realm name.  For example:
            #  "sqlrelay/sqlrserver.firstworks.com@AD.FIRSTWORKS.COM".
            #
            #  "mech" indicates the specific Kerberos mechanism to use.
            #  On Linux/Unix platforms, this should be a string
            #  representation of the mechnaism's OID, such as:
            #      { 1 2 840 113554 1 2 2 }
            #  On Windows platforms, this should be a string like:
            #      Kerberos
            #  If left empty or NULL then the default mechanism will be
            #  used.  Only set this if you know that you have a good
            #  reason to.
            #
            #  "flags" indicates what Kerberos flags to use.  Multiple
            #  flags may be specified, separated by commas.  If left
            #  empty or NULL then a defalt set of flags will be used.
            #  Only set this if you know that you have a good reason to.
            #
            #  Valid flags include:
            #   * GSS_C_MUTUAL_FLAG
            #   * GSS_C_REPLAY_FLAG
            #   * GSS_C_SEQUENCE_FLAG
            #   * GSS_C_CONF_FLAG
            #   * GSS_C_INTEG_FLAG
            #
            #  For a full list of flags, consult the GSSAPI documentation,
            #  though note that only the flags listed above are supported
            #  on Windows.

        enableTls(version, cert, password, ciphers, validate, ca, depth);
            # Enables TLS/SSL encryption, and optionally authentication.
            #
            #  "version" specifies the TLS/SSL protocol version that the
            #  client will attempt to use.  Valid values include SSL2,
            #  SSL3, TLS1, TLS1.1, TLS1.2 or any more recent version of
            #  TLS, as supported by and enabled in the underlying TLS/SSL
            #  library.  If left blank or empty then the highest supported
            #  version will be negotiated.
            #
            #  "cert" is the file name of the certificate chain file to
            #  send to the SQL Relay server.  This is only necessary if
            #  the SQL Relay server is configured to authenticate and
            #  authorize clients by certificate.
            #
            #  If "cert" contains a password-protected private key, then
            #  "password" may be supplied to access it.  If the private
            #  key is not password-protected, then this argument is
            #  ignored, and may be left empty or NULL.
            #
            #  "ciphers" is a list of ciphers to allow.  Ciphers may be
            #  separated by spaces, commas, or colons.  If "ciphers" is
            #  empty or NULL then a default set is used.  Only set this if
            #  you know that you have a good reason to.
            #
            #  For a list of valid ciphers on Linux/Unix platforms, see:
            #      man ciphers
            #
            #  For a list of valid ciphers on Windows platforms, see:
            #      https://msdn.microsoft.com/en-us/library/windows/desktop/aa375549%28v=vs.85%29.aspx
            #  On Windows platforms, the ciphers (alg_id's) should omit
            #  CALG_ and may be given with underscores or dashes.
            #  For example: 3DES_112
            #
            #  "validate" indicates whether to validate the SQL Relay's
            #  server certificate, and may be set to one of the following:
            #      "no" - Don't validate the server's certificate.
            #      "ca" - Validate that the server's certificate was
            #             signed by a trusted certificate authority.
            #      "ca+host" - Perform "ca" validation and also validate
            #             that one of the subject altenate names (or the
            #             common name if no SANs are present) in the
            #             certificate matches the host parameter.
            #             (Falls back to "ca" validation when a unix
            #             socket is used.)
            #      "ca+domain" - Perform "ca" validation and also validate
            #             that the domain name of one of the subject
            #             alternate names (or the common name if no SANs
            #             are present) in the certificate matches the
            #             domain name of the host parameter.  (Falls back
            #             to "ca" validation when a unix socket is used.)
            #
            #  "ca" is the location of a certificate authority file to
            #  use, in addition to the system's root certificates, when
            #  validating the SQL Relay server's certificate.  This is
            #  useful if the SQL Relay server's certificate is self-signed.
            #
            #  On Windows, "ca" must be a file name.
            #
            #  On non-Windows systems, "ca" can be either a file or
            #  directory name.  If it is a directory name, then all
            #  certificate authority files found in that directory will be
            #  used.  If it a file name, then only that file will be used.
            #
            #
            #  Note that the supported "cert" and "ca" file formats may
            #  vary between platforms.  A variety of file formats are
            #  generally supported on Linux/Unix platfoms (.pem, .pfx,
            #  etc.) but only the .pfx format is currently supported on
            #  Windows. */

        disableEncryption();
            # Disables encryption.

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
