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
	$user="";
	$password="";
	$tlscert="/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem";
	$tlsca="/usr/local/firstworks/etc/sqlrelay.conf.d/ca.pem";
	if (strtoupper(substr(PHP_OS,0,3))==='WIN') {
		$tlscert="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\client.pfx";
		$tlsca="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\ca.pfx";
	}
	$dsn = "sqlrelay:host=$host;port=$port;socket=$socket;tries=0;retrytime=1;tls=yes;tlscert=$tlscert;tlsvalidate=ca;tlsca=$tlsca;debug=0";


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

	echo("LAST INSERT ID: \n");
	checkSuccess($dbh->lastInsertId(),0);
	echo("\n");

	# doesn't work with oracle unless translatebindvariables="yes" is set
	echo("BIND BY POSITION: \n");
	$stmt=$dbh->prepare("insert into testtable values (:1,:2,:3,:4,:5,:6,:7)");
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
	checkSuccess($stmt->bindValue(5,$param5),true);
	checkSuccess($stmt->bindValue(6,$param6),true);
	checkSuccess($stmt->bindValue(7,$param7,PDO::PARAM_LOB),true);
	checkSuccess($stmt->execute(),true);
	echo("\n");

	echo("ROW COUNT: \n");
	checkSuccess($stmt->rowCount(),1);
	echo("\n");

	echo("ARRAY OF BINDS BY POSITION: \n");
	$stmt=$dbh->prepare("insert into testtable values (:1,:2,:3,:4,:5,:6,empty_blob())");
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
	checkSuccess($stmt->bindValue("var1",5,PDO::PARAM_INT),true);
	checkSuccess($stmt->bindValue("var2","testchar5"),true);
	checkSuccess($stmt->bindValue("var3","testvarchar5"),true);
	checkSuccess($stmt->bindValue("var4","01-JAN-2005"),true);
	checkSuccess($stmt->bindValue("var5","testlong5"),true);
	checkSuccess($stmt->bindValue("var6","testclob5"),true);
	checkSuccess($stmt->bindValue("var7","testblob5",PDO::PARAM_LOB),true);
	checkSuccess($stmt->execute(),true);
	$param1=6;
	$param2="testchar6";
	$param3="testvarchar6";
	$param4="01-JAN-2006";
	$param5="testlong6";
	$param6="testclob6";
	$param7="testblob6";
	checkSuccess($stmt->bindParam("var1",$param1,PDO::PARAM_INT),true);
	checkSuccess($stmt->bindParam("var2",$param2),true);
	checkSuccess($stmt->bindParam("var3",$param3),true);
	checkSuccess($stmt->bindParam("var4",$param4),true);
	checkSuccess($stmt->bindValue("var5",$param5),true);
	checkSuccess($stmt->bindValue("var6",$param6),true);
	checkSuccess($stmt->bindValue("var7",$param7,PDO::PARAM_LOB),true);
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
	checkSuccess($stmt->execute(array("var1"=>$param1,"var2"=>$param2,"var3"=>$param3,"var4"=>$param4,"var5"=>$param5,"var6"=>$param6)),true);
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

	echo("FIELDS BY INDEX: \n");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],1);
	checkSuccess($result[1],"testchar1                               ");
	checkSuccess($result[2],"testvarchar1");
	checkSuccess($result[3],"01-JAN-01");
	checkSuccess(stream_get_contents($result[4]),"testlong1");
	checkSuccess(stream_get_contents($result[5]),"testclob1");
	checkSuccess(stream_get_contents($result[6]),"");
	echo("\n");

	echo("FIELDS BY NAME: \n");
	$result=$stmt->fetch(PDO::FETCH_ASSOC);
	checkSuccess($result["TESTNUMBER"],2);
	checkSuccess($result["TESTCHAR"],"testchar2                               ");
	checkSuccess($result["TESTVARCHAR"],"testvarchar2");
	checkSuccess($result["TESTDATE"],"01-JAN-02");
	checkSuccess(stream_get_contents($result["TESTLONG"]),"testlong2");
	checkSuccess(stream_get_contents($result["TESTCLOB"]),"testclob2");
	checkSuccess(stream_get_contents($result["TESTBLOB"]),"testblob2");
	echo("\n");

	echo("FIELDS BY NAME AND INDEX: \n");
	$result=$stmt->fetch();
	checkSuccess($result[0],3);
	checkSuccess($result[1],"testchar3                               ");
	checkSuccess($result[2],"testvarchar3");
	checkSuccess($result[3],"01-JAN-03");
	checkSuccess(stream_get_contents($result[4]),"testlong3");
	rewind($result[4]);
	checkSuccess(stream_get_contents($result[5]),"testclob3");
	rewind($result[5]);
	checkSuccess(stream_get_contents($result[6]),"testblob3");
	rewind($result[6]);
	checkSuccess($result["TESTNUMBER"],3);
	checkSuccess($result["TESTCHAR"],"testchar3                               ");
	checkSuccess($result["TESTVARCHAR"],"testvarchar3");
	checkSuccess($result["TESTDATE"],"01-JAN-03");
	checkSuccess(stream_get_contents($result["TESTLONG"]),"testlong3");
	checkSuccess(stream_get_contents($result["TESTCLOB"]),"testclob3");
	checkSuccess(stream_get_contents($result["TESTBLOB"]),"testblob3");
	echo("\n");

	echo("FETCH COLUMN: \n");
	checkSuccess($stmt->fetchColumn(0),"4");
	checkSuccess($stmt->fetchColumn(0),"5");
	checkSuccess($stmt->fetchColumn(0),"6");
	echo("\n");

	echo("FETCH ALL: \n");
	$stmt=$dbh->query("select * from testtable order by testnumber");
	$result=$stmt->fetchAll();
	checkSuccess($result[0][0],1);
	checkSuccess($result[1][0],2);
	checkSuccess($result[2][0],3);
	checkSuccess($result[3][0],4);
	checkSuccess($result[4][0],5);
	checkSuccess($result[5][0],6);
	checkSuccess($result[6][0],7);
	checkSuccess($result[0][2],"testvarchar1");
	checkSuccess($result[1][2],"testvarchar2");
	checkSuccess($result[2][2],"testvarchar3");
	checkSuccess($result[3][2],"testvarchar4");
	checkSuccess($result[4][2],"testvarchar5");
	checkSuccess($result[5][2],"testvarchar6");
	checkSuccess($result[6][2],"testvarchar7");
	checkSuccess($result[0]["TESTNUMBER"],1);
	checkSuccess($result[1]["TESTNUMBER"],2);
	checkSuccess($result[2]["TESTNUMBER"],3);
	checkSuccess($result[3]["TESTNUMBER"],4);
	checkSuccess($result[4]["TESTNUMBER"],5);
	checkSuccess($result[5]["TESTNUMBER"],6);
	checkSuccess($result[6]["TESTNUMBER"],7);
	checkSuccess($result[0]["TESTVARCHAR"],"testvarchar1");
	checkSuccess($result[1]["TESTVARCHAR"],"testvarchar2");
	checkSuccess($result[2]["TESTVARCHAR"],"testvarchar3");
	checkSuccess($result[3]["TESTVARCHAR"],"testvarchar4");
	checkSuccess($result[4]["TESTVARCHAR"],"testvarchar5");
	checkSuccess($result[5]["TESTVARCHAR"],"testvarchar6");
	checkSuccess($result[6]["TESTVARCHAR"],"testvarchar7");
	echo("\n");

	echo("FETCH OBJECT: \n");
	$stmt=$dbh->query("select * from testtable order by testnumber");
	$result=$stmt->fetchObject();
	checkSuccess($result->TESTNUMBER,1);
	checkSuccess($result->TESTCHAR,"testchar1                               ");
	checkSuccess($result->TESTVARCHAR,"testvarchar1");
	checkSuccess($result->TESTDATE,"01-JAN-01");
	checkSuccess(stream_get_contents($result->TESTLONG),"testlong1");
	checkSuccess(stream_get_contents($result->TESTCLOB),"testclob1");
	checkSuccess(stream_get_contents($result->TESTBLOB),"");
	echo("\n");

	echo("FETCH ORIENTATIONS: \n");
	$stmt=$dbh->query("select * from testtable order by testnumber");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"1");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_FIRST);
	checkSuccess($result[0],"1");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_NEXT);
	checkSuccess($result[0],"2");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_PRIOR);
	checkSuccess($result[0],"1");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_LAST);
	checkSuccess($result[0],"7");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_ABS,3);
	checkSuccess($result[0],"4");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_ABS,4);
	checkSuccess($result[0],"5");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_ABS,5);
	checkSuccess($result[0],"6");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_REL,1);
	checkSuccess($result[0],"7");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_REL,-1);
	checkSuccess($result[0],"6");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_REL,-1);
	checkSuccess($result[0],"5");
	echo("\n");

	echo("FETCH FORWARD ONLY: \n");
	$stmt=$dbh->prepare("select * from testtable order by testnumber",
				array(PDO::ATTR_CURSOR=>PDO::CURSOR_FWDONLY));
	checkSuccess($stmt->execute(),true);
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_FIRST);
	checkSuccess($result[0],"1");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_FIRST);
	checkSuccess($result,false);
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_ABS,1);
	checkSuccess($result[0],"2");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_ABS,1);
	checkSuccess($result,false);
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_REL,1);
	checkSuccess($result[0],"3");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_REL,2);
	checkSuccess($result[0],"5");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_REL,-1);
	checkSuccess($result,false);
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_PRIOR);
	checkSuccess($result,false);
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_NEXT);
	checkSuccess($result[0],"6");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_LAST);
	checkSuccess($result[0],"7");
	echo("\n");

	echo("RESULT SET BUFFER SIZE: \n");
	$stmt=$dbh->prepare("select * from testtable order by testnumber");
	checkSuccess($stmt->setAttribute(
				PDO::SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE,2),1);
	checkSuccess($stmt->getAttribute(
				PDO::SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE),2);
	checkSuccess($stmt->execute(),true);
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"1");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"2");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"3");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_LAST);
	checkSuccess($result[0],"7");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_FIRST);
	checkSuccess($result,false);
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_ABS,0);
	checkSuccess($result,false);
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_ABS,6);
	checkSuccess($result[0],"7");
	$result=$stmt->fetch(PDO::FETCH_NUM,PDO::FETCH_ORI_ABS,5);
	checkSuccess($result,false);
	echo("\n");

	echo("DON'T GET COLUMN INFO: \n");
	$stmt=$dbh->prepare("select * from testtable order by testnumber");
	checkSuccess($stmt->setAttribute(
				PDO::SQLRELAY_ATTR_DONT_GET_COLUMN_INFO,1),1);
	checkSuccess($stmt->execute(),true);
	$meta0=$stmt->getColumnMeta(0);
	checkSuccess($meta0["name"],null);
	checkSuccess($meta0["native_type"],null);
	checkSuccess($meta0["pdo_type"],PDO::PARAM_STR);
	checkSuccess($meta0["len"],null);
	$stmt=$dbh->prepare("select * from testtable order by testnumber");
	checkSuccess($stmt->setAttribute(
				PDO::SQLRELAY_ATTR_DONT_GET_COLUMN_INFO,0),1);
	checkSuccess($stmt->execute(),true);
	$meta0=$stmt->getColumnMeta(0);
	checkSuccess($meta0["name"],"TESTNUMBER");
	checkSuccess($meta0["native_type"],"NUMBER");
	checkSuccess($meta0["pdo_type"],PDO::PARAM_INT);
	checkSuccess($meta0["len"],22);
	echo("\n");

	echo("OTHER DRIVER-SPECIFIC OPTIONS: \n");
	checkSuccess($dbh->getAttribute(PDO::SQLRELAY_ATTR_BIND_FORMAT),":*");
	checkSuccess($dbh->getAttribute(
				PDO::SQLRELAY_ATTR_DB_TYPE),"oracle");
	echo("db version: ".$dbh->getAttribute(
				PDO::SQLRELAY_ATTR_DB_VERSION)."\n");
	echo("db host name: ".$dbh->getAttribute(
				PDO::SQLRELAY_ATTR_DB_HOST_NAME)."\n");
	echo("db ip address: ".$dbh->getAttribute(
				PDO::SQLRELAY_ATTR_DB_IP_ADDRESS)."\n");
	echo("current db: ".$dbh->getAttribute(
				PDO::SQLRELAY_ATTR_CURRENT_DB)."\n");
	echo("\n");

	echo("BOUND COLUMNS: \n");
	$stmt=$dbh->prepare("select * from testtable order by testnumber");
	$col1=0;
	$col2=0;
	$col3=0;
	$col4=0;
	$col5=0;
	$col6=0;
	$col7=0;
	$stmt->bindColumn(1,$col1);
	$stmt->bindColumn(2,$col2);
	$stmt->bindColumn(3,$col3);
	$stmt->bindColumn(4,$col4);
	$stmt->bindColumn(5,$col5,PDO::PARAM_LOB);
	$stmt->bindColumn(6,$col6,PDO::PARAM_LOB);
	$stmt->bindColumn(7,$col7,PDO::PARAM_LOB);
	checkSuccess($stmt->execute(),1);
	checkSuccess($stmt->fetch(PDO::FETCH_BOUND),TRUE);
	checkSuccess($col1,1);
	checkSuccess($col2,"testchar1                               ");
	checkSuccess($col3,"testvarchar1");
	checkSuccess($col4,"01-JAN-01");
	checkSuccess(stream_get_contents($col5),"testlong1");
	checkSuccess(stream_get_contents($col6),"testclob1");
	checkSuccess(stream_get_contents($col7),"");
	checkSuccess($stmt->fetch(PDO::FETCH_BOUND),TRUE);
	checkSuccess($col1,2);
	checkSuccess($col2,"testchar2                               ");
	checkSuccess($col3,"testvarchar2");
	checkSuccess($col4,"01-JAN-02");
	checkSuccess(stream_get_contents($col5),"testlong2");
	checkSuccess(stream_get_contents($col6),"testclob2");
	checkSuccess(stream_get_contents($col7),"testblob2");
	echo("\n");

	echo("STRINGIFY: \n");
	checkSuccess($dbh->setAttribute(PDO::ATTR_STRINGIFY_FETCHES,TRUE),1);
	$stmt=$dbh->query("select * from testtable order by testnumber");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"1");
	checkSuccess($result[1],"testchar1                               ");
	checkSuccess($result[2],"testvarchar1");
	checkSuccess($result[3],"01-JAN-01");
	checkSuccess($result[4],"testlong1");
	checkSuccess($result[5],"testclob1");
	checkSuccess($result[6],"");
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"2");
	checkSuccess($result[1],"testchar2                               ");
	checkSuccess($result[2],"testvarchar2");
	checkSuccess($result[3],"01-JAN-02");
	checkSuccess($result[4],"testlong2");
	checkSuccess($result[5],"testclob2");
	checkSuccess($result[6],"testblob2");
	checkSuccess($dbh->setAttribute(PDO::ATTR_STRINGIFY_FETCHES,FALSE),1);
	echo("\n");

	echo("SUSPENDED SESSION: \n");
	$stmt=$dbh->query("select * from testtable order by testnumber");
	$stmt->suspendResultSet();
	checkSuccess($dbh->suspendSession(),1);
	$port=$dbh->getConnectionPort();
	$socket=$dbh->getConnectionSocket();
	checkSuccess($dbh->resumeSession($port,$socket),1);
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"1");
	checkSuccess($result[1],"testchar1                               ");
	checkSuccess($result[2],"testvarchar1");
	checkSuccess($result[3],"01-JAN-01");
	checkSuccess(stream_get_contents($result[4]),"testlong1");
	checkSuccess(stream_get_contents($result[5]),"testclob1");
	checkSuccess(stream_get_contents($result[6]),"");
	echo("\n");
	$stmt=$dbh->query("select * from testtable order by testnumber");
	$stmt->suspendResultSet();
	checkSuccess($dbh->suspendSession(),1);
	$port=$dbh->getConnectionPort();
	$socket=$dbh->getConnectionSocket();
	checkSuccess($dbh->resumeSession($port,$socket),1);
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"1");
	checkSuccess($result[1],"testchar1                               ");
	checkSuccess($result[2],"testvarchar1");
	checkSuccess($result[3],"01-JAN-01");
	checkSuccess(stream_get_contents($result[4]),"testlong1");
	checkSuccess(stream_get_contents($result[5]),"testclob1");
	checkSuccess(stream_get_contents($result[6]),"");
	echo("\n");
	$stmt=$dbh->query("select * from testtable order by testnumber");
	$stmt->suspendResultSet();
	checkSuccess($dbh->suspendSession(),1);
	$port=$dbh->getConnectionPort();
	$socket=$dbh->getConnectionSocket();
	checkSuccess($dbh->resumeSession($port,$socket),1);
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"1");
	checkSuccess($result[1],"testchar1                               ");
	checkSuccess($result[2],"testvarchar1");
	checkSuccess($result[3],"01-JAN-01");
	checkSuccess(stream_get_contents($result[4]),"testlong1");
	checkSuccess(stream_get_contents($result[5]),"testclob1");
	checkSuccess(stream_get_contents($result[6]),"");
	echo("\n");

	echo("SUSPENDED RESULT SET: \n");
	$stmt=$dbh->prepare("select * from testtable order by testnumber");
	checkSuccess($stmt->setAttribute(
				PDO::SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE,1),1);
	checkSuccess($stmt->execute(),1);
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"1");
	$id=$stmt->getResultSetId();
	$stmt->suspendResultSet();
	checkSuccess($dbh->suspendSession(),1);
	$port=$dbh->getConnectionPort();
	$socket=$dbh->getConnectionSocket();
	$dbh=new PDO($dsn,$user,$password);
	$stmt=$dbh->prepare(null);
	checkSuccess($dbh->resumeSession($port,$socket),1);
	$stmt->resumeResultSet($id);
	$result=$stmt->fetch(PDO::FETCH_NUM);
	checkSuccess($result[0],"2");
	$result=$stmt->fetchAll();
	checkSuccess($result[0][0],"3");
	checkSuccess($result[1][0],"4");
	checkSuccess($result[2][0],"5");
	checkSuccess($result[3][0],"6");
	checkSuccess($result[4][0],"7");
	echo("\n");

	echo("COMMIT AND ROLLBACK: \n");
	$dbh->exec("drop table testtable1");
	checkSuccess($dbh->exec("create table testtable1 (testnumber number)"),0);
	checkSuccess($dbh->inTransaction(),0);
	$dbh->beginTransaction();
	checkSuccess($dbh->inTransaction(),1);
	checkSuccess($dbh->exec("insert into testtable1 values (1)"),1);
	$dbh2=new PDO($dsn,$user,$password);
	$stmt2=$dbh2->query("select count(*) from testtable1");
	$result2=$stmt2->fetch();
	checkSuccess($result2[0],0);
	$dbh->commit();
	$stmt2=$dbh2->query("select count(*) from testtable1");
	$result2=$stmt2->fetch();
	checkSuccess($result2[0],1);
	$dbh->beginTransaction();
	checkSuccess($dbh->exec("insert into testtable1 values (1)"),1);
	$stmt2=$dbh2->query("select count(*) from testtable1");
	$result2=$stmt2->fetch();
	checkSuccess($result2[0],1);
	$dbh->rollback();
	$stmt2=$dbh2->query("select count(*) from testtable1");
	$result2=$stmt2->fetch();
	checkSuccess($result2[0],1);
	echo("\n");

	echo("AUTOCOMMIT: \n");
	checkSuccess($dbh->inTransaction(),0);
	$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT,TRUE);
	checkSuccess($dbh->exec("insert into testtable1 values (1)"),1);
	$stmt2=$dbh2->query("select count(*) from testtable1");
	$result2=$stmt2->fetch();
	checkSuccess($result2[0],2);
	checkSuccess($dbh->inTransaction(),0);
	$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT,FALSE);
	$dbh->beginTransaction();
	checkSuccess($dbh->exec("insert into testtable1 values (1)"),1);
	$stmt2=$dbh2->query("select count(*) from testtable1");
	$result2=$stmt2->fetch();
	checkSuccess($result2[0],2);
	$dbh->commit();
	$stmt2=$dbh2->query("select count(*) from testtable1");
	$result2=$stmt2->fetch();
	checkSuccess($result2[0],3);
	echo("\n");

	echo("CLOSE CURSOR\n");
	$stmt=$dbh2->prepare("select * from testtable");
	checkSuccess($stmt->execute(),1);
	checkSuccess($stmt->closeCursor(),1);
	checkSuccess($stmt->execute(),1);
	echo("\n");

	echo("CLIENT AND SERVER VERSIONS: \n");
	checkSuccess($dbh->getAttribute(PDO::ATTR_CLIENT_VERSION),
			$dbh->getAttribute(PDO::ATTR_SERVER_VERSION));
	echo("\n");

	# drop testtables
	$dbh->exec("drop table testtable");
	$dbh->exec("drop table testtable1");

# output binds don't appear to work with PDO for PHP7
if (PHP_VERSION_ID < 70000) {
	echo("OUTPUT BIND BY NAME: \n");
	$stmt=$dbh->prepare("begin  :numvar:=1; :stringvar:='hello'; end;");
	$param1=0;
	$param2="";
	checkSuccess($stmt->bindParam(":numvar",$param1,PDO::PARAM_INT|PDO::PARAM_INPUT_OUTPUT),true);
	checkSuccess($stmt->bindParam(":stringvar",$param2,PDO::PARAM_STR|PDO::PARAM_INPUT_OUTPUT,10),true);
	checkSuccess($stmt->execute(),1);
	checkSuccess($param1,1);
	checkSuccess($param2,"hello");
	echo("\n");

	echo("OUTPUT BIND BY POSITION: \n");
	$stmt=$dbh->prepare("begin  :1:=1; :2:='hello'; end;");
	$param1=0;
	$param2="";
	checkSuccess($stmt->bindParam(1,$param1,PDO::PARAM_INT|PDO::PARAM_INPUT_OUTPUT),true);
	checkSuccess($stmt->bindParam(2,$param2,PDO::PARAM_STR|PDO::PARAM_INPUT_OUTPUT,10),true);
	checkSuccess($stmt->execute(),1);
	checkSuccess($param1,1);
	checkSuccess($param2,"hello");
	echo("\n");

	echo("CLOB AND BLOB OUTPUT BIND: \n");
	$dbh->exec("drop table testtable1");
	checkSuccess($dbh->exec("create table testtable1 (testclob clob, testblob blob)"),0);
	$stmt=$dbh->prepare("insert into testtable1 values ('hello',:var1)");
	checkSuccess($stmt->bindValue("var1","hello",PDO::PARAM_LOB),true);
	checkSuccess($stmt->execute(),1);
	$stmt=$dbh->prepare("begin  select testblob into :blobvar from testtable1; end;");
	$param1="";
	checkSuccess($stmt->bindParam(":blobvar",$param1,PDO::PARAM_LOB|PDO::PARAM_INPUT_OUTPUT),true);
	checkSuccess($stmt->execute(),1);
	checkSuccess(stream_get_contents($param1),"hello");
	echo("\n");

	echo("CLOB AND BLOB OUTPUT BIND TO AND FROM FILE: \n");
	$dbh->exec("drop table testtable1");
	checkSuccess($dbh->exec("create table testtable1 (testclob clob, testblob blob)"),0);
	$stmt=$dbh->prepare("insert into testtable1 values ('hello',:var1)");
	$stream=fopen("test.blob","w+b");
	fwrite($stream,"hello");
	fclose($stream);
	$stream=fopen("test.blob","rb");
	checkSuccess($stmt->bindValue("var1",$stream,PDO::PARAM_LOB),true);
	checkSuccess($stmt->execute(),1);
	fclose($stream);
	unlink("test.blob");
	$stmt=$dbh->prepare("begin  select testblob into :blobvar from testtable1; end;");
	$stream=fopen("test.blob","w+b");
	checkSuccess($stmt->bindParam(":blobvar",$stream,PDO::PARAM_LOB|PDO::PARAM_INPUT_OUTPUT),true);
	checkSuccess($stmt->execute(),1);
	checkSuccess(stream_get_contents($stream),"hello");
	fclose($stream);
	unlink("test.blob");
	echo("\n");
}

	$dbh->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_SILENT);

	# this throws an execption and doesn't continue on PHP7
	try {

		echo("NON-LAZY CONNECT: \n");
		$dsn = "sqlrelay:host=invalidhost;port=0;socket=/invalidsocket;tries=1;retrytime=1;tls=yes;tlscert=$tlscert;tlsvalidate=ca;tlsca=$tlsca;debug=0;lazyconnect=0";
		checkSuccess(new PDO($dsn,$user,$password),0);
		echo("\n");

	} catch (Exception $e) {
		echo($e->getMessage());
		echo("\n");
	}

	echo("INVALID QUERIES: \n");
	checkSuccess($dbh->query("select 1"),0);
	checkSuccess($dbh->errorCode(),"HY000");
	$info=$dbh->errorInfo();
	checkSuccess($info[0],"HY000");
	checkSuccess($info[1],923);
	checkSuccess($info[2],"ORA-00923: FROM keyword not found where expected");
	$stmt=$dbh->prepare("select 1");
	checkSuccess($stmt->execute(),0);
	checkSuccess($stmt->errorCode(),"HY000");
	$info=$stmt->errorInfo();
	checkSuccess($info[0],"HY000");
	checkSuccess($info[1],923);
	checkSuccess($info[2],"ORA-00923: FROM keyword not found where expected");
	echo("\n");

	echo("QUOTE: \n");
	checkSuccess($dbh->quote("select * from table"),"'select * from table'");
	checkSuccess($dbh->quote("Naughty ' string"),"'Naughty '' string'");
	checkSuccess($dbh->quote("Co'mpl''ex \"st'\"ring"),"'Co''mpl''''ex \"st''\"ring'");
	echo("\n");

	echo("INVALID OPERATIONS: \n");
	checkSuccess($stmt->nextRowset(),0);
	checkSuccess($stmt->setAttribute(PDO::ATTR_AUTOCOMMIT,FALSE),0);
	checkSuccess($stmt->getAttribute(PDO::ATTR_AUTOCOMMIT),0);
	#checkSuccess($stmt->bindValue(1,1,9999),true);
	echo("\n");

	$dbh->exec("drop table testtable");

?></pre></html>
