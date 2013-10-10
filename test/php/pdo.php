<html><pre><?php
# Copyright (c) 2013  David Muse
# See the file COPYING for more information.

	function checkSuccessQuery($db,$value) {

		if ($value!=false) {
			echo("success ");
		} else {
			echo("failure ");
			$db->disconnect();
			exit(0);
		}
	}

	function checkSuccess($db,$value,$success) {

		if ($value==$success) {
			echo("success ");
		} else {
			echo("$value != $success ");
			echo("failure ");
			$db->disconnect();
			exit(0);
		}
	}

	$host="localhost";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="test";
	$password="test";
	$dsn = "postgresql:$host";


	# instantiation
	$db=new PDO($dsn,$user,$password);
	if(!$db){
		die("new PDO failed");
	}

	# drop existing table
	$db->query("drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	checkSuccessQuery($db,$db->query("create table testtable (testnumber number, testchar char(40), testvarchar varchar(40), testdate date)"));
	echo("\n");

	echo("INSERT: \n");
	checkSuccessQuery($db,$db->query("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001')"));
	echo("\n");

	echo("AFFECTED ROWS: \n");
	checkSuccess($db,$db->rowCount(),1);
	echo("\n");

	$db->query("drop table testtable");

?></pre></html>
