<html><pre><?php
# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

	$host="sqlrelay";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="testuser";
	$password="testpassword";
	$dsn = "sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=1";


	# instantiation
	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	#$stmt=$dbh->prepare("declare @s nvarchar(max) = replicate('X', 7999)\nset :out = @s\n");
	$stmt=$dbh->prepare("set :out = 'hello'\n");
	$out="hi";
	$stmt->bindValue(":out",$out,PDO::PARAM_STR|PDO::PARAM_INPUT_OUTPUT);
	$stmt->execute();


?></pre></html>
