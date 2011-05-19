#!/usr/bin/env perl

# Copyright (c) 2000-2001  David Muse
# See the file COPYING for more information.

use DBI;


print "INSTANTIATION\n";
my $dbh=DBI->connect("DBI:SQLRelay:host=localhost;port=9000;socket=/tmp/test.socket;debug=1","test","test") or die DBI->errstr;
print "\n\n";



print "PING\n";
$dbh->ping();
print "\n\n";



print "QUERY FUNCTIONS\n";
my $sth=$dbh->prepare($ARGV[1]) or die DBI->errstr;

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
my $sth=$dbh->prepare("select :1,:2,:3 from dual") or die DBI->errstr;
$sth->execute("hello",1,5.5) or die DBI->errstr;
print "\n\n";


print "OUTPUT BIND FUNCTIONS\n";
my $numvar;
my $stringvar;
my $floatvar;
my $sth=$dbh->prepare("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;") or die DBI->errstr;
$sth->bind_param_inout('numvar',\$numvar,100);
$sth->bind_param_inout('stringvar',\$stringvar,100);
$sth->bind_param_inout('floatvar',\$floatvar,100);
$sth->execute() or die DBI->errstr;
print "numvar: $numvar\n";
print "stringvar: $stringvar\n";
print "floatvar: $floatvar\n";
print "\n\n";



print "DISCONNECT\n";
$dbh->disconnect;
