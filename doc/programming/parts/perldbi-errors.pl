#!/usr/bin/env perl

use DBI;

my $dbh=DBI->connect("DBI:SQLRelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword")
        or die DBI->errstr;

my $sth=$dbh->prepare("select * from user_tables")
        or die DBI->errstr;

$sth->execute()
        or die DBI->errstr;

... process the result set ...

$dbh->disconnect;
