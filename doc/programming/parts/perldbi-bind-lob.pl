#!/usr/bin/env perl

use DBI;

my $dbh=DBI->connect("DBI:SQLRelay:host=sqlrserver;port=9000;socket=/tmp/test.socket","testuser","testpassword");

my $sth=$dbh->prepare("insert into mytable values (:clobval,:blobval)");

$sth->bind_param(":clobval","test clob",DBD::SQLRelay::SQL_CLOB);
$sth->bind_param(":blobval","test blob",{type=>DBD::SQLRelay::SQL_BLOB,length=>9});

$sth->execute();

... process the result set ...

$dbh->disconnect;
