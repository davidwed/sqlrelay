#!/usr/bin/env perl

use DBI;

my $dbh=DBI->connect("DBI:SQLRelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");

my $sth=$dbh->prepare("select * from my_table");

$sth->execute();

for ($i=1; $i<=$sth->{NUM_OF_FIELDS}; $i++) {
       print "Column $i: $sth->{NAME}->[$i-1]\n";
}

$dbh->disconnect;
