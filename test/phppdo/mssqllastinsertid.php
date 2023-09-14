<?php
# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

	$host="sqlrelay";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="testuser";
	$password="testpassword";
	$dsn = "sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;fetchlobsasstrings=1;debug=0";
	#$dsn = "odbc:dsn=mssqlodbc;uid=test;pwd=test";

	$dbh=new PDO($dsn,$user,$password);
	try {
		$dbh->exec("drop table test");
	} catch (Exception $e) {
	}
	$dbh->exec("create table test (MYID BIGINT NOT NULL IDENTITY(99999999999, 9), MYNAME NVARCHAR(100) NOT NULL)");
	$dbh->exec("insert into test (MYNAME) values ('This is my name')");
	echo("last insert id: " . $dbh->lastInsertId());
	$dbh->exec("drop table test");
	echo("\n");		
?>
