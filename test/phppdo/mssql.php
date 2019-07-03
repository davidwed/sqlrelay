<?php
# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

	$host="sqlrelay";
	$port=8000;
	$socket="/tmp/test.socket";
	$user="test";
	$password="test";
	$dsn = "sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=1";
	#$dsn = "odbc:dsn=mssqlodbc;uid=testuser;pwd=testpassword";

	$dbh=new PDO($dsn,$user,$password);
	$stmt=$dbh->query("select ".
		"cast('1958-01-15 01:02:03' as datetime) as datetimevalue, ".
		"cast('1958-01-15' as date) as datevalue, ".
		"cast('<document><element/></document>' as xml) as xmlvalue, ".
		"cast('FOOB10' as text) as textvalue, ".
		"cast('FOOB11' as ntext) as ntextvalue, ".
		"cast('FOOB11' as image) as imagevalue");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	echo("$result[0]\n");
	echo("$result[1]\n");
	echo("$result[2]\n");
	echo("$result[3]\n");
	echo("$result[4]\n");
	echo("$result[5]\n");
	#echo(stream_get_contents($result[2])."\n");
	#echo(stream_get_contents($result[3])."\n");
	#echo(stream_get_contents($result[4])."\n");
	#echo(stream_get_contents($result[5])."\n");
	echo("\n");		
?>
