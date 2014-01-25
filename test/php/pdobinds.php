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
	$dsn="sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=1";


	# instantiation
	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	# get databaes type
	$dbtype=$dbh->getAttribute(PDO::SQLRELAY_ATTR_DB_TYPE);

	# drop existing table
	$dbh->exec("drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	checkSuccess($dbh->exec("create table testtable (testnumber int)"),0);
	echo("\n");

	echo("BIND BY POSITION: \n");
	$queryvar="";
	$bindvar=1;
	switch ($dbtype) {
		case "oracle8":
		case "sqlite":
			$queryvar=":1";
			break;
		case "sybase":
		case "freetds":
			$queryvar="@1";
			break;
		case "db2":
		case "firebird":
		case "mysql":
			$queryvar="?";
			break;
		case "postgresql":
			$queryvar="$1";
			break;
	}
	echo("queryvar: $queryvar\n");
	$stmt=$dbh->prepare("insert into testtable values ($queryvar)");
	checkSuccess($stmt->bindValue(1,2,PDO::PARAM_INT),true);
	checkSuccess($stmt->execute(),true);
	echo("\n");

	echo("BIND BY NAME: \n");
	$queryvar="";
	$bindvar="";
	switch ($dbtype) {
		case "oracle8":
		case "sqlite":
			$queryvar=":var1";
			$bindvar="var1";
			break;
		case "sybase":
		case "freetds":
			$queryvar="@var1";
			$bindvar="var1";
			break;
		case "db2":
		case "firebird":
		case "mysql":
			$queryvar="?";
			$bindvar="1";
			break;
		case "postgresql":
			$queryvar="$1";
			$bindvar="1";
			break;
	}
	echo("queryvar: $queryvar   bindvar: $bindvar\n");
	$stmt=$dbh->prepare("insert into testtable values ($queryvar)");
	checkSuccess($stmt->bindValue($bindvar,2,PDO::PARAM_INT),true);
	checkSuccess($stmt->execute(),true);
	echo("\n");

	$dbh->exec("drop table testtable");

?></pre></html>
