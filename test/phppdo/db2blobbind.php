<html><pre><?php

	print("\n");

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

	$dbh->exec("drop table testtable");
	$dbh->exec("create table testtable (teststring varchar(200), testblob blob)");

	$value="data from string";

	$stream=fopen("test.blob","w+b");
	fwrite($stream,"test test test test");
	fclose($stream);

	$stream=fopen("test.blob","rb");

	$stmt=$dbh->prepare("insert into testtable values (?,?)");
	$stmt->bindValue(1,$value);
	$stmt->bindValue(2,$stream,PDO::PARAM_LOB);
	$stmt->execute();

	fclose($stream);
	unlink("test.blob");

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
