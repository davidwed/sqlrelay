#!/usr/bin/env perl

use DBI;

my $dbh=DBI->connect("DBI:SQLRelay:host=sqlrserver;port=9000;socket=/tmp/example.socket","exampleuser","examplepassword");

my $sth=$dbh->prepare("insert into mytable values (:floatval)");

$sth->bind_param(":floatval,5.5,{precision=>10,scale=>3});

$sth->execute();

... process the result set ...

$dbh->disconnect;
