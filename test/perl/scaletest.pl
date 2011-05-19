#! /usr/bin/env perl

# Copyright (c) 2001  David Muse
# See the file COPYING for more information.


use SQLRelay::Connection;
use SQLRelay::Cursor;


# instantiation
$con1=SQLRelay::Connection->new("localhost",9000,"/tmp/test.socket",
							"test","test",0,1);
$cur1=SQLRelay::Cursor->new($con1);
$cur1->sendQuery("select 1 from dual");


$con2=SQLRelay::Connection->new("localhost",9000,"/tmp/test.socket",
							"test","test",0,1);
$cur2=SQLRelay::Cursor->new($con2);
$cur2->sendQuery("select 1 from dual");



$con3=SQLRelay::Connection->new("localhost",9000,"/tmp/test.socket",
							"test","test",0,1);
$cur3=SQLRelay::Cursor->new($con3);
$cur3->sendQuery("select 1 from dual");
