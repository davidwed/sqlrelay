#!/usr/bin/env perl

# Copyright (c) 2000-2001  David Muse
# See the file COPYING for more information.

use DBI;

#usage...
if ($#ARGV+1<6) {
	print("usage: perldbi.pl host port socket user password query\n");
	exit;
}



print "INSTANTIATION\n";
my $dbh=DBI->connect("DBI:SQLRelay:host=$ARGV[0];port=$ARGV[1];socket=$ARGV[2];debug=1",$ARGV[3],$ARGV[4]) or die DBI->errstr;
print "\n\n";



print "PING\n";
$dbh->ping();
print "\n\n";



print "QUERY FUNCTIONS\n";
my $sth=$dbh->prepare($ARGV[5]) or die DBI->errstr;

$sth->execute() or die DBI->errstr;

while (@data=$sth->fetchrow_array()) {
	
	foreach $col (@data) {
		print "\"$col\",";
	}
	print "\n";
}
print "\n\n";


my $sth=$dbh->prepare("select 1,2,3,4 from dual") or die DBI->errstr;
$sth->execute() or die DBI->errstr;
$sth->bind_columns(undef,\$col1,\$col2,\$col3,\$col4);
while ($sth->fetch()) {
	print "$col1, $col2, $col3, $col4\n";
}
print "\n\n";



print "COMMIT/ROLLBACK\n";
$dbh->commit();
$dbh->rollback();
print "\n\n";



print "BIND FUNCTIONS\n";
my $sth=$dbh->prepare("select :1,:2,:3 from dual") or die DBI->errstr;

$sth->bind_param(1,"hello");
$sth->bind_param(2,1);
$sth->bind_param(3,5.5);

$sth->execute() or die DBI->errstr;
print "\n\n";


print "BIND FUNCTIONS\n";
my $sth=$dbh->prepare("select :0,:1,:2 from dual") or die DBI->errstr;
$sth->execute(undef,"hello",1,5.5) or die DBI->errstr;
print "\n\n";



print "DISCONNECT\n";
$dbh->disconnect;
