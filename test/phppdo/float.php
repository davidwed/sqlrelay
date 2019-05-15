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
	$dsn = "sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=0";


	# instantiation
	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	# drop existing table
	$dbh->exec("drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	$dbh->exec("create table testtable (testfloat float, testdouble double)");
	echo("\n");

	echo("INSERT: \n");
	checkSuccess($dbh->exec("insert into testtable values (3.14,3.14)"),1);
	checkSuccess($dbh->exec("insert into testtable values (6.28,6.28)"),1);
	checkSuccess($dbh->exec("insert into testtable values (9.42,9.42)"),1);
	echo("\n");

	echo("SELECT: \n");
	$stmt=$dbh->query("select * from testtable order by testfloat");
	echo("\n");
	
	echo("FIELDS BY INDEX: \n");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"3.14");
	checkSuccess($result[1],"3.14");
	echo("\n");

	echo("FIELDS BY NAME: \n");
	$result=$stmt->fetch(PDO::FETCH_ASSOC);
	checkSuccess($result["testfloat"],"6.28");
	checkSuccess($result["testdouble"],"6.28");
	var_dump($result);
	echo("\n");

	echo("FIELDS BY NAME AND INDEX: \n");
	$result=$stmt->fetch();
	checkSuccess($result[0],"9.42");
	checkSuccess($result[1],"9.42");
	echo("\n");

	$dbh->exec("drop table testtable");

?></pre></html>
