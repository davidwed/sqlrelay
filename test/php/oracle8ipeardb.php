<html><pre><?php
# Copyright (c) 2001  David Muse
# See the file COPYING for more information.

require_once 'DB.php';

	function checkSuccess($db,$value,$success) {

		if ($value==$success) {
			echo("success ");
		} else {
			echo("failure ");
			printf("%s!=%s\n",$value,$success);
			$db->disconnect();
			exit(0);
		}
	}

	$host=$_REQUEST["host"];
	$port=$_REQUEST["port"];
	$socket=$_REQUEST["socket"];
	$user=$_REQUEST["user"];
	$password=$_REQUEST["password"];
	$dsn = "sqlrelay://$user:$password@$host:$port/$db_name";

	# instantiation
	$db = DB::connect($dsn);
	if (DB::isError($db)) {
        	die ($db->getMessage());
	}

	# drop existing table
	$db->query("drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	checkSuccess($db,$db->query("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long)"),DB_OK);
	echo("\n");

	echo("INSERT: \n");
	checkSuccess($db,$db->query("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1')"),DB_OK);
	echo("\n");

	echo("AFFECTED ROWS: \n");
	checkSuccess($db,$db->affectedRows(),1);
	echo("\n");

	echo("BIND BY POSITION: \n");
	$res=$db->prepare("insert into testtable values (:var1,:var2,:var3,:var4,:var5)");
	$bindvars=array("1" => 2,
			"2" => "testchar2",
			"3" => "testvarchar2",
			"4" => "01-JAN-2002",
			"5" => "testlong2");
	$res=$db->execute($res,$bindvars);

	$bindvars=array("var1" => 3,
			"var2" => "testchar3",
			"var3" => "testvarchar3",
			"var4" => "01-JAN-2003",
			"var5" => "testlong3");
	$res=$db->execute($res,$bindvars);

	$bindvars=array("1" => 4,
			"2" => "testchar4",
			"3" => "testvarchar4",
			"4" => "01-JAN-2004",
			"5" => "testlong4");
	$res=$db->execute($res,$bindvars);

	$bindvars=array("var1" => 5,
			"var2" => "testchar5",
			"var3" => "testvarchar5",
			"var4" => "01-JAN-2005",
			"var5" => "testlong5");
	$res=$db->execute($res,$bindvars);

	$bindvars=array("1" => 6,
			"2" => "testchar6",
			"3" => "testvarchar6",
			"4" => "01-JAN-2006",
			"5" => "testlong6");
	$res=$db->execute($res,$bindvars);

	$bindvars=array("var1" => 7,
			"var2" => "testchar7",
			"var3" => "testvarchar7",
			"var4" => "01-JAN-2007",
			"var5" => "testlong7");
	$res=$db->execute($res,$bindvars);

	$bindvars=array("1" => 8,
			"2" => "testchar8",
			"3" => "testvarchar8",
			"4" => "01-JAN-2008",
			"5" => "testlong8");
	$res=$db->execute($res,$bindvars);

	echo("SELECT: \n");
	$res=$db->query("select * from testtable order by testnumber");
	echo("\n");

	echo("COLUMN COUNT: \n");
	checkSuccess($db,$res->numCols(),5);
	echo("\n");

	echo("COLUMN NAMES: \n");
	$tableinfo=$res->tableInfo();
	checkSuccess($db,$tableinfo[0]['name'],"TESTNUMBER");
	checkSuccess($db,$tableinfo[1]['name'],"TESTCHAR");
	checkSuccess($db,$tableinfo[2]['name'],"TESTVARCHAR");
	checkSuccess($db,$tableinfo[3]['name'],"TESTDATE");
	checkSuccess($db,$tableinfo[4]['name'],"TESTLONG");
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess($db,$tableinfo[0]['type'],"NUMBER");
	checkSuccess($db,$tableinfo[1]['type'],"CHAR");
	checkSuccess($db,$tableinfo[2]['type'],"VARCHAR2");
	checkSuccess($db,$tableinfo[3]['type'],"DATE");
	checkSuccess($db,$tableinfo[4]['type'],"LONG");
	echo("\n");

	echo("COLUMN LENGTH: \n");
	checkSuccess($db,$tableinfo[0]['len'],22);
	checkSuccess($db,$tableinfo[1]['len'],40);
	checkSuccess($db,$tableinfo[2]['len'],40);
	checkSuccess($db,$tableinfo[3]['len'],7);
	checkSuccess($db,$tableinfo[4]['len'],0);
	echo("\n");

	echo("ROW COUNT: \n");
	checkSuccess($db,$res->numRows(),8);
	echo("\n");

	echo("FIELDS BY ARRAY: \n");
	$row=$res->fetchRow(DB_FETCHMODE_ORDERED);
	checkSuccess($db,$row[0],"1");
	checkSuccess($db,$row[1],"testchar1                               ");
	checkSuccess($db,$row[2],"testvarchar1");
	checkSuccess($db,$row[3],"01-JAN-01");
	checkSuccess($db,$row[4],"testlong1");
	echo("\n");
	$row=$res->fetchRow(DB_FETCHMODE_ORDERED,7);
	checkSuccess($db,$row[0],"8");
	checkSuccess($db,$row[1],"testchar8                               ");
	checkSuccess($db,$row[2],"testvarchar8");
	checkSuccess($db,$row[3],"01-JAN-08");
	checkSuccess($db,$row[4],"testlong8");
	echo("\n");
	$res->free();

	echo("FIELDS BY ASSOCIATIVE ARRAY: \n");
	$res=$db->query("select * from testtable order by testnumber");
	$row=$res->fetchRow(DB_FETCHMODE_ASSOC);
	checkSuccess($db,$row['TESTNUMBER'],"1");
	checkSuccess($db,$row['TESTCHAR'],"testchar1                               ");
	checkSuccess($db,$row['TESTVARCHAR'],"testvarchar1");
	checkSuccess($db,$row['TESTDATE'],"01-JAN-01");
	checkSuccess($db,$row['TESTLONG'],"testlong1");
	echo("\n");
	$row=$res->fetchRow(DB_FETCHMODE_ASSOC,7);
	checkSuccess($db,$row['TESTNUMBER'],"8");
	checkSuccess($db,$row['TESTCHAR'],"testchar8                               ");
	checkSuccess($db,$row['TESTVARCHAR'],"testvarchar8");
	checkSuccess($db,$row['TESTDATE'],"01-JAN-08");
	checkSuccess($db,$row['TESTLONG'],"testlong8");
	echo("\n");
	$res->free();

	echo("FIELDS BY OBJECT: \n");
	$res=$db->query("select * from testtable order by testnumber");
	$row=$res->fetchRow(DB_FETCHMODE_OBJECT);
	checkSuccess($db,$row->TESTNUMBER,"1");
	checkSuccess($db,$row->TESTCHAR,"testchar1                               ");
	checkSuccess($db,$row->TESTVARCHAR,"testvarchar1");
	checkSuccess($db,$row->TESTDATE,"01-JAN-01");
	checkSuccess($db,$row->TESTLONG,"testlong1");
	echo("\n");
	$row=$res->fetchRow(DB_FETCHMODE_OBJECT,7);
	checkSuccess($db,$row->TESTNUMBER,"8");
	checkSuccess($db,$row->TESTCHAR,"testchar8                               ");
	checkSuccess($db,$row->TESTVARCHAR,"testvarchar8");
	checkSuccess($db,$row->TESTDATE,"01-JAN-08");
	checkSuccess($db,$row->TESTLONG,"testlong8");
	echo("\n");
	$res->free();

	echo("COMMIT AND ROLLBACK: \n");
	$db2 = DB::connect($dsn);
	if (DB::isError($db2)) {
        	die ($db2->getMessage());
	}

	checkSuccess($db2,$db2->getOne("select count(*) from testtable"),0);
	checkSuccess($db,$db->commit(),DB_OK);
	checkSuccess($db2,$db2->getOne("select count(*) from testtable"),8);
	$db2->disconnect();
	checkSuccess($db,$db->autoCommit(true),DB_OK);
	checkSuccess($db,$db->query("insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10')"),DB_OK);
	//checkSuccess($db2,$db2->getOne("select count(*) from testtable"),9);
	$db->autoCommit(false);

	$db2->disconnect();
	echo("\n");


	# drop existing table
	$db->query("drop table testtable");

	$db->disconnect();
?></pre></html>
