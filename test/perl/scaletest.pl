#! /usr/bin/env perl

# Copyright (c) 2001  David Muse
# See the file COPYING for more information.


use Firstworks::SQLRConnection;
use Firstworks::SQLRCursor;


# usage...
if ($#ARGV+1<5) {
	print("usage: scaletest.pl host port socket user password");
	exit;
}


# instantiation
$con1=Firstworks::SQLRConnection->new($ARGV[0],$ARGV[1], 
				$ARGV[2],$ARGV[3],$ARGV[4],0,1);
$cur1=Firstworks::SQLRCursor->new($con1);
$cur1->sendQuery("select 1 from dual");


$con2=Firstworks::SQLRConnection->new($ARGV[0],$ARGV[1], 
				$ARGV[2],$ARGV[3],$ARGV[4],0,1);
$cur2=Firstworks::SQLRCursor->new($con2);
$cur2->sendQuery("select 1 from dual");



$con3=Firstworks::SQLRConnection->new($ARGV[0],$ARGV[1], 
				$ARGV[2],$ARGV[3],$ARGV[4],0,1);
$cur3=Firstworks::SQLRCursor->new($con3);
$cur3->sendQuery("select 1 from dual");
