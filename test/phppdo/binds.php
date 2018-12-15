<html><pre><?php
# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

	function checkSuccess($value,$success) {

		if ($value==$success) {
			echo("success ");
		} else {
			echo("$value != $success ");
			echo("failure ");
			exit(1);
		}
	}

	$host="sqlrelay";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="test";
	$password="test";
	$dsn="sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=0";


	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	$dbtype=$dbh->getAttribute(PDO::SQLRELAY_ATTR_DB_TYPE);

	if ($dbtype!="firebird") {
		$dbh->exec("drop table testtable");
		echo("CREATE TEMPTABLE: \n");
		checkSuccess($dbh->exec("create table testtable (testinteger int)"),0);
		echo("\n");
	}

	echo("BIND BY POSITION: \n");
	$queryvar="";
	$bindvar=1;
	switch ($dbtype) {
		case "oracle":
		case "sqlite":
			$queryvar=":1";
			break;
		case "sap":
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
	$stmt=$dbh->prepare("insert into testtable (testinteger) values ($queryvar)");
	checkSuccess($stmt->bindValue(1,2,PDO::PARAM_INT),true);
	checkSuccess($stmt->execute(),true);
	echo("\n");

	echo("BIND BY NAME: \n");
	$queryvar="";
	$bindvar="";
	switch ($dbtype) {
		case "oracle":
		case "sqlite":
			$queryvar=":var1";
			$bindvar="var1";
			break;
		case "sap":
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
	$stmt=$dbh->prepare("insert into testtable (testinteger) values ($queryvar)");
	checkSuccess($stmt->bindValue($bindvar,2,PDO::PARAM_INT),true);
	checkSuccess($stmt->execute(),true);
	echo("\n");

	$dbh->exec("delete from testtable");
	if ($dbtype!="firebird") {
		$dbh->exec("drop table testtable");
	}

?></pre></html>
