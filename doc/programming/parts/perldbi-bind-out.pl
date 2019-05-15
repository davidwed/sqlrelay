#!/usr/bin/env perl

use DBI;

my $dbh=DBI->connect("DBI:SQLRelay:host=sqlrserver;port=9000;socket=/tmp/example.socket","exampleuser","examplepassword");

my $sth=$dbh->prepare("begin; :1='hello'; :2=1; :3=5.5; end;");

my $hello;
my $integer;
my $float;
$sth->bind_param_inout(1,\$hello,10);
$sth->bind_param_inout(2,\$integer,10);
$sth->bind_param_inout(3,\$float,10);

$sth->execute();

print("$hello $integer $float\n");

$dbh->disconnect;
