<html><pre><?php
# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

	# sqlrelay
	$host="sqlrelay";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="test";
	$password="test";
	$dsn="sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=0";

	# mysql
	#$user="testuser";
	#$password="testpassword";
	#$dsn="mysql:host=db64;port=3306;dbname=testdb";


	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	$dbh->exec("drop table testdate");
	$dbh->exec("create table testdate (date1 datetime, date2 datetime, date3 datetime, date4 datetime, date5 datetime)");
	$dbh->exec("insert into testdate values (NULL,'','0','0000-00-00 00:00:00','2001-01-01')");

	$stmt=$dbh->query("select * from testdate");
	$result=$stmt->fetch(PDO::FETCH_ASSOC);

	echo("\n");
	print_r($result);
	echo("\n");

	$stmt=NULL;

	$dbh->exec("drop table testdate");

?></pre></html>
