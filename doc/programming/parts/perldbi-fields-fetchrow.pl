#!/usr/bin/env perl

use DBI;

my $dbh=DBI->connect("DBI:SQLRelay:host=sqlrserver;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0","testuser","testpassword");

my $sth=$dbh->prepare("select * from user_tables");

$sth->execute();

while (@data=$sth->fetchrow_array()) {
        
        foreach $col (@data) {
                print "\"$col\",";
        }
        print "\n";
}

$dbh->disconnect;
