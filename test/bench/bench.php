<html><pre><?php
	# Copyright (c) 2015  David Muse
	# See the file COPYING for more information.

	# sqlrelay connection
	$sqlrelay=array(
		"dsn" => "sqlrelay:host=sqlrserver;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0",
		"user" => "test",
		"password" => "test"
	);

	# mysql connection
	$mysql=array(
		"dsn" => "mysql:host=db64;port=3360;dbname=testdb",
		"user" => "testuser",
		"password" => "testpassword"
	);

	# set current configuration
	$conn=$sqlrelay;


	# create db connection
	$dbh=new PDO($conn["dsn"],$conn["user"],$conn["password"]);
	if(!$dbh){
		die("new PDO failed");
	}

	# clean up
	$dbh=null;

?></pre></html>
