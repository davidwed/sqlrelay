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
	#$user="testuser";
	#$password="testpassword";
	#$dsn = "mysql:host=db64;port=3306;dbname=testdb";


	# instantiation
	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	# drop existing table
	$dbh->exec("drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	$dbh->exec("create table testtable (testint int, testfloat float, testchar varchar(20), testblob blob, testdate datetime)");
	echo("\n");

	echo("INSERT: \n");
	checkSuccess($dbh->exec("insert into testtable values (NULL,NULL,NULL,NULL,NULL)"),1);
	checkSuccess($dbh->exec("insert into testtable values (1,1.1,'1','1','2001-01-01')"),1);
	checkSuccess($dbh->exec("insert into testtable values (0,0.0,'0','0','0000-00-00 00:00:00')"),1);
	echo("\n");

	echo("FIELDS BY INDEX (as NULL): \n");
	$stmt=$dbh->query("select * from testtable");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"");
	checkSuccess($result[1],"");
	checkSuccess($result[2],"");
	checkSuccess(stream_get_contents($result[3]),"");
	checkSuccess($result[4],"");
echo("\n");
print_r($result);
echo("\n");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],1);
	checkSuccess($result[1],1.1);
	checkSuccess($result[2],"1");
	checkSuccess(stream_get_contents($result[3]),"1");
	checkSuccess($result[4],"2001-01-01 00:00:00");
echo("\n");
print_r($result);
echo("\n");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],0);
	checkSuccess($result[1],0.0);
	checkSuccess($result[2],"0");
	checkSuccess(stream_get_contents($result[3]),"0");
	checkSuccess($result[4],"0000-00-00 00:00:00");
echo("\n");
print_r($result);
echo("\n");
	echo("\n");

	echo("FIELDS BY INDEX (as empty strings): \n");
	$stmt=$dbh->prepare("select * from testtable");
	$stmt->setAttribute(
		PDO::SQLRELAY_ATTR_GET_NULLS_AS_EMPTY_STRINGS,false);
	$stmt->execute();
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],null);
	checkSuccess($result[1],null);
	checkSuccess($result[2],null);
	checkSuccess($result[3],null);
	checkSuccess($result[4],null);
echo("\n");
print_r($result);
echo("\n");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],1);
	checkSuccess($result[1],1.1);
	checkSuccess($result[2],"1");
	checkSuccess(stream_get_contents($result[3]),"1");
	checkSuccess($result[4],"2001-01-01 00:00:00");
echo("\n");
print_r($result);
echo("\n");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],0);
	checkSuccess($result[1],0.0);
	checkSuccess($result[2],"0");
	checkSuccess(stream_get_contents($result[3]),"0");
	checkSuccess($result[4],"0000-00-00 00:00:00");
echo("\n");
print_r($result);
echo("\n");
	echo("\n");

	echo("NULL INTEGER INPUT BIND\n");
	$stmt=$dbh->prepare("select ?,?");
	$param1=null;
	$param2="";
	$stmt->bindParam("1",$param1,PDO::PARAM_INT);
	$stmt->bindParam("2",$param2,PDO::PARAM_INT);
	$stmt->setAttribute(
		PDO::SQLRELAY_ATTR_GET_NULLS_AS_EMPTY_STRINGS,true);
	$stmt->execute();
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"");
	checkSuccess($result[1],"");
	$param1=null;
	$param2="";
	$stmt->setAttribute(
		PDO::SQLRELAY_ATTR_GET_NULLS_AS_EMPTY_STRINGS,false);
	$stmt->execute();
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],null);
	checkSuccess($result[1],null);
	echo("\n");

	$dbh->exec("drop table testtable");

?></pre></html>
