<?php
# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

	$host="sqlrelay";
	$port=8000;
	$socket="/tmp/test.socket";
	$user="test";
	$password="test";
	$dsn = "sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;fetchlobsasstrings=1;debug=1";
	#$dsn = "odbc:dsn=mssqlodbc;uid=testuser;pwd=testpassword";

	$dbh=new PDO($dsn,$user,$password);
	$stmt=$dbh->query("select ".
		"cast('1958-01-15 01:02:03' as datetime) ".
					"as datetimevalue, ".
		"cast('1958-01-15' as date) ".
					"as datevalue, ".
		"cast('varbinary(max) data' as varbinary(max)) ".
					"as varbinarymaxvalue, ".
		"cast('image data' as image) ".
					"as imagevalue, ".
		"cast('text data' as text) ".
					"as textvalue, ".
		"cast('ntext data' as ntext) ".
					"as ntextvalue, ".
		"cast('<document><element/></document>' as xml) ".
					"as xmlvalue");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	echo("$result[0]\n");
	echo("$result[1]\n");
	echo("$result[2]\n");
	echo("$result[3]\n");
	echo("$result[4]\n");
	echo("$result[5]\n");
	echo("$result[6]\n");
	#echo(stream_get_contents($result[2])."\n");
	#echo(stream_get_contents($result[3])."\n");
	#echo(stream_get_contents($result[4])."\n");
	#echo(stream_get_contents($result[5])."\n");
	#echo(stream_get_contents($result[6])."\n");
	echo("\n");		
?>
