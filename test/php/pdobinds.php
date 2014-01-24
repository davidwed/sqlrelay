<html><pre><?php
# Copyright (c) 2013  David Muse
# See the file COPYING for more information.

dl("pdo_sqlrelay.so");

	function checkSuccess($value,$success) {

		if ($value==$success) {
			echo("success ");
		} else {
			echo("$value != $success ");
			echo("failure ");
			exit(0);
		}
	}

	$host="localhost";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="test";
	$password="test";
	$dsn = "sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=0";


	# instantiation
	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	# drop existing table
	$dbh->exec("drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	checkSuccess($dbh->exec("create table testtable (testnumber number)"),0);
	echo("\n");

	echo("BIND BY NAME: \n");
	$stmt=$dbh->prepare("insert into testtable values (@var)");
	checkSuccess($stmt->bindValue("@var",2,PDO::PARAM_INT),true);
	checkSuccess($stmt->execute(),true);
	echo("\n");

	echo("BIND BY POSITION: \n");
	$stmt=$dbh->prepare("insert into testtable values (?)");
	checkSuccess($stmt->bindValue(1,2,PDO::PARAM_INT),true);
	checkSuccess($stmt->execute(),true);
	echo("\n");

	$dbh->exec("drop table testtable");

?></pre></html>
