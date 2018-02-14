#!/usr/bin/env perl

use DBI;

my $dbh=DBI->connect("DBI:SQLRelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");

my $sth1=$dbh->prepare("select * from my_first_table");

$sth1->execute();
$sth1->bind_columns(undef,\$col1,\$col2,\$col3);

while ($sth1->fetch()) {
        my $sth2=$dbh->prepare("insert into my_second_table values (:0, :1, :2, sysdate)");
        $sth2->execute(undef,$col1,$col2,$col3);
}

$dbh->disconnect;
