#!/usr/bin/env perl

use DBI;

my $dbh=DBI->connect("DBI:SQLRelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");

my $sth=$dbh->prepare("select * from my_table where col1=:1 and col2=:2 and col3=:3");

$sth->bind_param(1,"hello");
$sth->bind_param(2,1);
$sth->bind_param(3,5.5);

$sth->execute();

... process the result set ...

$dbh->disconnect;
