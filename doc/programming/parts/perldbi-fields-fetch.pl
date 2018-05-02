#!/usr/bin/env perl

use DBI;

my $dbh=DBI->connect("DBI:SQLRelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");

my $sth=$dbh->prepare("select * from my_table");

$sth->execute();
$sth->bind_columns(undef,\$col1,\$col2,\$col3,\$col4);

while ($sth->fetch()) {
        print "$col, $col2, $col3, $col4\n";
}

$dbh->disconnect;
