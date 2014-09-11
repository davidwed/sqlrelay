<html><pre><?php

	print("\n");

	dl("pdo_sqlrelay.so");

	$host="localhost";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="test";
	$password="test";
	$dsn="sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=1";


	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	$dbh->exec("drop table testtable");
	$dbh->exec("create table testtable (teststring varchar(200), testblob blob)");

	$value="test test test";
	#$value="";

	$stmt=$dbh->prepare("insert into testtable values (?,?)");
	$stmt->bindValue(1,$value);
	$stmt->bindValue(2,$value,PDO::PARAM_LOB);
	$stmt->execute();

	$stmt=$dbh->prepare("select * from testtable");
	$stmt->execute();
	$result=$stmt->fetch(PDO::FETCH_NUM);
	print("string:\n");
	print($result[0]."\n");
	print("lob:\n");
	print(stream_get_contents($result[1])."\n");

	$stmt=null;

	#$dbh->exec("drop table testtable");

?></pre></html>
