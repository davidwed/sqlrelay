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
	$dsn = "sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;debug=1";


	# instantiation
	$dbh=new PDO($dsn,$user,$password);
	if(!$dbh){
		die("new PDO failed");
	}

	# drop existing table
	$dbh->exec("drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	checkSuccess($dbh->exec("create table testtable (testnumber number, testchar char(40), testvarchar varchar(40), testdate date, testlong long, testclob clob, testblob blob)"),0);
	echo("\n");

	echo("INSERT: \n");
	checkSuccess($dbh->exec("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1','testclob1',empty_blob())"),1);
	echo("\n");

	# doesn't work with oracle unless translatebindvariables="yes" is set
	echo("BIND BY POSITION: \n");
	$stmt=$dbh->prepare("insert into testtable values (?,?,?,?,?,?,?)");
	checkSuccess($stmt->bindValue(1,2,PDO::PARAM_INT),true);
	checkSuccess($stmt->bindValue(2,"testchar2"),true);
	checkSuccess($stmt->bindValue(3,"testvarchar2"),true);
	checkSuccess($stmt->bindValue(4,"01-JAN-2002"),true);
	checkSuccess($stmt->bindValue(5,"testlong2"),true);
	checkSuccess($stmt->bindValue(6,"testclob2"),true);
	checkSuccess($stmt->bindValue(7,"testblob2",PDO::PARAM_LOB),true);
	checkSuccess($stmt->execute(),true);
	$param1=3;
	$param2="testchar3";
	$param3="testvarchar3";
	$param4="01-JAN-2003";
	$param5="testlong3";
	$param6="testclob3";
	$param7="testblob3";
	checkSuccess($stmt->bindParam(1,$param1,PDO::PARAM_INT),true);
	checkSuccess($stmt->bindParam(2,$param2),true);
	checkSuccess($stmt->bindParam(3,$param3),true);
	checkSuccess($stmt->bindParam(4,$param4),true);
	checkSuccess($stmt->bindValue(6,$param5),true);
	checkSuccess($stmt->bindValue(6,$param6),true);
	checkSuccess($stmt->bindValue(7,$param7,PDO::PARAM_LOB),true);
	checkSuccess($stmt->execute(),true);
	echo("\n");

	echo("ARRAY OF BINDS BY POSITION: \n");
	$stmt=$dbh->prepare("insert into testtable values (?,?,?,?,?,?,empty_blob())");
	$param1=4;
	$param2="testchar4";
	$param3="testvarchar4";
	$param4="01-JAN-2004";
	$param5="testlong4";
	$param6="testclob4";
	checkSuccess($stmt->execute(array($param1,$param2,$param3,$param4,$param5,$param6)),true);
	echo("\n");

	echo("BIND BY NAME: \n");
	$stmt=$dbh->prepare("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
	checkSuccess($stmt->bindValue(":var1",5,PDO::PARAM_INT),true);
	checkSuccess($stmt->bindValue(":var2","testchar5"),true);
	checkSuccess($stmt->bindValue(":var3","testvarchar5"),true);
	checkSuccess($stmt->bindValue(":var4","01-JAN-2005"),true);
	checkSuccess($stmt->bindValue(":var5","testlong5"),true);
	checkSuccess($stmt->bindValue(":var6","testclob5"),true);
	checkSuccess($stmt->bindValue(":var7","testblob5",PDO::PARAM_LOB),true);
	checkSuccess($stmt->execute(),true);
	$param1=6;
	$param2="testchar6";
	$param3="testvarchar6";
	$param4="01-JAN-2006";
	$param5="testlong6";
	$param6="testclob6";
	$param7="testblob6";
	checkSuccess($stmt->bindParam(":var1",$param1,PDO::PARAM_INT),true);
	checkSuccess($stmt->bindParam(":var2",$param2),true);
	checkSuccess($stmt->bindParam(":var3",$param3),true);
	checkSuccess($stmt->bindParam(":var4",$param4),true);
	checkSuccess($stmt->bindValue(":var5",$param5),true);
	checkSuccess($stmt->bindValue(":var6",$param6),true);
	checkSuccess($stmt->bindValue(":var7",$param7,PDO::PARAM_LOB),true);
	checkSuccess($stmt->execute(),true);
	echo("\n");

	echo("ARRAY OF BINDS BY NAME: \n");
	$stmt=$dbh->prepare("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,empty_blob())");
	$param1=7;
	$param2="testchar7";
	$param3="testvarchar7";
	$param4="01-JAN-2007";
	$param5="testlong7";
	$param6="testclob7";
	checkSuccess($stmt->execute(array(":var1"=>$param1,":var2"=>$param2,":var3"=>$param3,":var4"=>$param4,":var5"=>$param5,":var6"=>$param6)),true);
	echo("\n");

	echo("SELECT: \n");
	$stmt=$dbh->query("select * from testtable order by testnumber");
	echo("\n");
	
	echo("COLUMN COUNT: \n");
	checkSuccess($stmt->columnCount(),7);
	$meta0=$stmt->getColumnMeta(0);
	$meta1=$stmt->getColumnMeta(1);
	$meta2=$stmt->getColumnMeta(2);
	$meta3=$stmt->getColumnMeta(3);
	$meta4=$stmt->getColumnMeta(4);
	$meta5=$stmt->getColumnMeta(5);
	$meta6=$stmt->getColumnMeta(6);
	echo("\n");

	echo("COLUMN NAMES: \n");
	checkSuccess($meta0["name"],"TESTNUMBER");
	checkSuccess($meta1["name"],"TESTCHAR");
	checkSuccess($meta2["name"],"TESTVARCHAR");
	checkSuccess($meta3["name"],"TESTDATE");
	checkSuccess($meta4["name"],"TESTLONG");
	checkSuccess($meta5["name"],"TESTCLOB");
	checkSuccess($meta6["name"],"TESTBLOB");
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess($meta0["native_type"],"NUMBER");
	checkSuccess($meta1["native_type"],"CHAR");
	checkSuccess($meta2["native_type"],"VARCHAR2");
	checkSuccess($meta3["native_type"],"DATE");
	checkSuccess($meta4["native_type"],"LONG");
	checkSuccess($meta5["native_type"],"CLOB");
	checkSuccess($meta6["native_type"],"BLOB");
	checkSuccess($meta0["pdo_type"],PDO::PARAM_INT);
	checkSuccess($meta1["pdo_type"],PDO::PARAM_STR);
	checkSuccess($meta2["pdo_type"],PDO::PARAM_STR);
	checkSuccess($meta3["pdo_type"],PDO::PARAM_STR);
	checkSuccess($meta4["pdo_type"],PDO::PARAM_LOB);
	checkSuccess($meta5["pdo_type"],PDO::PARAM_LOB);
	checkSuccess($meta6["pdo_type"],PDO::PARAM_LOB);
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess($meta0["len"],22);
	checkSuccess($meta1["len"],40);
	checkSuccess($meta2["len"],40);
	checkSuccess($meta3["len"],7);
	checkSuccess($meta4["len"],0);
	checkSuccess($meta5["len"],0);
	checkSuccess($meta6["len"],0);
	echo("\n");

	echo("ROW COUNT: \n");
	checkSuccess($stmt->rowCount(),7);
	echo("\n");

	echo("FIELDS BY INDEX: \n");
	echo("\n");

	# dbh methods:
	#  beginTransaction
	#  commit
	#  errorCode
	#  errorInfo
	#  getAttribute
	#  inTransaction
	#  lastInsertId
	#  query
	#    * FETCH_COLUMN
	#    * FETCH_CLASS
	#    * FETCH_INTO
	#  quote
	#  rollBack
	#  setAttribute

	# statement methods:
	#  bindColumn
	#  closeCursor
	#  debugDumpParams
	#  errorCode
	#  errorInfo
	#  fetch
	#  fetchAll
	#  fetchColumn
	#  fetchObject
	#  getAttribute
	#  nextRowset
	#  setAttribute
	#  setFetchMode

	$dbh->exec("drop table testtable");

?></pre></html>
