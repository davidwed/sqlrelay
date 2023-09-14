<html><pre><?php
# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

	# sqlrelay
	$host="sqlrelay";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="testuser";
	$password="testpassword";
	$dsn="sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=0";

	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	try {
		$dbh->exec("drop table testdate");
	} catch (Exception $e) {
	}
	$dbh->exec("create table testdate (date1 datetime, date2 datetime, date3 datetime)");
	$dbh->exec("insert into testdate values (NULL,'0000-01-01 00:00:00','2001-01-01')");

	$stmt=$dbh->query("select * from testdate");
	$result=$stmt->fetch(PDO::FETCH_ASSOC);

	echo("\n");
	print_r($result);
	echo("\n");

	$stmt=NULL;

	try {
		$dbh->exec("drop table testdate");
	} catch (Exception $e) {
	}

?></pre></html>
