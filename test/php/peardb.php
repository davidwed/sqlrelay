<?php

function checkSuccess($db,$value,$success) {

	if ($value==$success) {
		echo("success ");
	} else {
		echo("failure ");
		$db->disconnect();
		exit(0);
	}
}

require_once 'DB.php';
$user = $_REQUEST["user"];
$pass = $_REQUEST["password"];
$host = $_REQUEST["host"];
$port = $_REQUEST["port"];
$socket = $_REQUEST["socket"];

$dsn = "sqlrelay://$user:$pass@$host:$port/$db_name";

$db = DB::connect($dsn);
if (DB::isError($db)) {
        die ($db->getMessage());
}



// simple query/fetch
echo("SIMPLE QUERY/FETCH\n");
$res = $db->query("select 1,2,3,4 from dual");
if (DB::isError($res)) {
        die ($res->getMessage());
}

while ($row = $res->fetchRow()) {
    $numElements = count($row);
    checkSuccess($db,$numElements,4);
    checkSuccess($db,$row[0],1);
    checkSuccess($db,$row[1],2);
    checkSuccess($db,$row[2],3);
    checkSuccess($db,$row[3],4);
}

$res->free();
echo("\n\n");


// fetch modes
echo("FETCH MODES\n");

$sql = ('select 1 as ID, \'David\' as NAME, \'dmuse@firstworks.com\' as EMAIL from dual');

// ordered
$res = $db->query($sql);
$row = $res->fetchRow(DB_FETCHMODE_ORDERED);
checkSuccess($db,$row[0],1);
checkSuccess($db,$row[1],'David');
checkSuccess($db,$row[2],'dmuse@firstworks.com');
$res->free();

$res = $db->query($sql);
$row = $res->fetchRow(DB_FETCHMODE_ORDERED,0);
checkSuccess($db,$row[0],1);
checkSuccess($db,$row[1],'David');
checkSuccess($db,$row[2],'dmuse@firstworks.com');
$res->free();

$res = $db->query($sql);
$res->fetchInto($row,DB_FETCHMODE_ORDERED);
checkSuccess($db,$row[0],1);
checkSuccess($db,$row[1],'David');
checkSuccess($db,$row[2],'dmuse@firstworks.com');
$res->free();

$res = $db->query($sql);
$res->fetchInto($row,DB_FETCHMODE_ORDERED,0);
checkSuccess($db,$row[0],1);
checkSuccess($db,$row[1],'David');
checkSuccess($db,$row[2],'dmuse@firstworks.com');
$res->free();

$db->setFetchMode(DB_FETCHMODE_ORDERED);
$res = $db->query($sql);
$row = $res->fetchRow();
checkSuccess($db,$row[0],1);
checkSuccess($db,$row[1],'David');
checkSuccess($db,$row[2],'dmuse@firstworks.com');
$res->free();

$db->setFetchMode(DB_FETCHMODE_ORDERED);
$res = $db->query($sql);
$row = $res->fetchRow(0);
checkSuccess($db,$row[0],1);
checkSuccess($db,$row[1],'David');
checkSuccess($db,$row[2],'dmuse@firstworks.com');
$res->free();

// assoc
$res = $db->query($sql);
$row = $res->fetchRow(DB_FETCHMODE_ASSOC);
checkSuccess($db,$row['ID'],1);
checkSuccess($db,$row['NAME'],'David');
checkSuccess($db,$row['EMAIL'],'dmuse@firstworks.com');
$res->free();

$res = $db->query($sql);
$row = $res->fetchRow(DB_FETCHMODE_ASSOC,0);
checkSuccess($db,$row['ID'],1);
checkSuccess($db,$row['NAME'],'David');
checkSuccess($db,$row['EMAIL'],'dmuse@firstworks.com');
$res->free();

$res = $db->query($sql);
$res->fetchInto($row,DB_FETCHMODE_ASSOC);
checkSuccess($db,$row['ID'],1);
checkSuccess($db,$row['NAME'],'David');
checkSuccess($db,$row['EMAIL'],'dmuse@firstworks.com');
$res->free();

$res = $db->query($sql);
$res->fetchInto($row,DB_FETCHMODE_ASSOC,0);
checkSuccess($db,$row['ID'],1);
checkSuccess($db,$row['NAME'],'David');
checkSuccess($db,$row['EMAIL'],'dmuse@firstworks.com');
$res->free();

$db->setFetchMode(DB_FETCHMODE_ASSOC);
$res = $db->query($sql);
$row = $res->fetchRow();
checkSuccess($db,$row['ID'],1);
checkSuccess($db,$row['NAME'],'David');
checkSuccess($db,$row['EMAIL'],'dmuse@firstworks.com');
$res->free();

$db->setFetchMode(DB_FETCHMODE_ASSOC);
$res = $db->query($sql);
$row = $res->fetchRow(0);
checkSuccess($db,$row['ID'],1);
checkSuccess($db,$row['NAME'],'David');
checkSuccess($db,$row['EMAIL'],'dmuse@firstworks.com');
$res->free();

// object
$res = $db->query($sql);
$row = $res->fetchRow(DB_FETCHMODE_OBJECT);
checkSuccess($db,$row->ID,1);
checkSuccess($db,$row->NAME,'David');
checkSuccess($db,$row->EMAIL,'dmuse@firstworks.com');
$res->free();

$res = $db->query($sql);
$row = $res->fetchRow(DB_FETCHMODE_OBJECT,0);
checkSuccess($db,$row->ID,1);
checkSuccess($db,$row->NAME,'David');
checkSuccess($db,$row->EMAIL,'dmuse@firstworks.com');

$res = $db->query($sql);
$res->fetchInto($row,DB_FETCHMODE_OBJECT);
checkSuccess($db,$row->ID,1);
checkSuccess($db,$row->NAME,'David');
checkSuccess($db,$row->EMAIL,'dmuse@firstworks.com');
$res->free();

$res = $db->query($sql);
$res->fetchInto($row,DB_FETCHMODE_OBJECT,0);
checkSuccess($db,$row->ID,1);
checkSuccess($db,$row->NAME,'David');
checkSuccess($db,$row->EMAIL,'dmuse@firstworks.com');
$res->free();

$db->setFetchMode(DB_FETCHMODE_OBJECT);
$res = $db->query($sql);
$row = $res->fetchRow();
checkSuccess($db,$row->ID,1);
checkSuccess($db,$row->NAME,'David');
checkSuccess($db,$row->EMAIL,'dmuse@firstworks.com');
$res->free();

$db->setFetchMode(DB_FETCHMODE_OBJECT);
$res = $db->query($sql);
$row = $res->fetchRow(0);
checkSuccess($db,$row->ID,1);
checkSuccess($db,$row->NAME,'David');
checkSuccess($db,$row->EMAIL,'dmuse@firstworks.com');
$res->free();

// invalid row
$db->setFetchMode(DB_FETCHMODE_ORDERED);
$res = $db->query($sql);
$row = $res->fetchRow(1);
$res->free();

$db->setFetchMode(DB_FETCHMODE_ASSOC);
$res = $db->query($sql);
$row = $res->fetchRow(1);
$res->free();

$db->setFetchMode(DB_FETCHMODE_OBJECT);
$res = $db->query($sql);
$row = $res->fetchRow(1);
$res->free();

echo("\n\n");


// quick data retrieving
echo("QUICK DATA RETRIEVING\n");
checkSuccess($db,$db->getOne($sql),1);

$db->setFetchMode(DB_FETCHMODE_ORDERED);
$row = $db->getRow($sql);
checkSuccess($db,$row[0],1);
checkSuccess($db,$row[1],'David');
checkSuccess($db,$row[2],'dmuse@firstworks.com');

$db->setFetchMode(DB_FETCHMODE_ASSOC);
$row = $db->getRow($sql);
checkSuccess($db,$row['ID'],1);
checkSuccess($db,$row['NAME'],'David');
checkSuccess($db,$row['EMAIL'],'dmuse@firstworks.com');

$db->setFetchMode(DB_FETCHMODE_OBJECT);
$row = $db->getRow($sql);
checkSuccess($db,$row->ID,1);
checkSuccess($db,$row->NAME,'David');
checkSuccess($db,$row->EMAIL,'dmuse@firstworks.com');

// FIXME: test getCol() too...
echo("\n\n");


// row/column info
echo("ROW/COLUMN INFO\n");

$res = $db->query($sql);
checkSuccess($db,$res->numRows(),1);
checkSuccess($db,$res->numCols(),3);
checkSuccess($db,$db->affectedRows(),0);
echo("\n\n");

// table info
echo("TABLE INFO\n");
$tableinfo = $res->tableInfo();
checkSuccess($db,$tableinfo[0]['table'],'');
checkSuccess($db,$tableinfo[0]['name'],'ID');
checkSuccess($db,$tableinfo[0]['type'],'NUMBER');
checkSuccess($db,$tableinfo[0]['len'],2);
checkSuccess($db,$tableinfo[0]['flags'],'');
checkSuccess($db,$tableinfo[1]['table'],'');
checkSuccess($db,$tableinfo[1]['name'],'NAME');
checkSuccess($db,$tableinfo[1]['type'],'CHAR');
checkSuccess($db,$tableinfo[1]['len'],5);
checkSuccess($db,$tableinfo[1]['flags'],'');
checkSuccess($db,$tableinfo[2]['table'],'');
checkSuccess($db,$tableinfo[2]['name'],'EMAIL');
checkSuccess($db,$tableinfo[2]['type'],'CHAR');
checkSuccess($db,$tableinfo[2]['len'],20);
checkSuccess($db,$tableinfo[2]['flags'],'');
echo("\n\n");
$tableinfo = $res->tableInfo(DB_TABLEINFO_ORDER);
checkSuccess($db,$tableinfo['num_fields'],3);
checkSuccess($db,$tableinfo['order']['ID'],0);
checkSuccess($db,$tableinfo['order']['NAME'],1);
checkSuccess($db,$tableinfo['order']['EMAIL'],2);
checkSuccess($db,$tableinfo[0]['table'],'');
checkSuccess($db,$tableinfo[0]['name'],'ID');
checkSuccess($db,$tableinfo[0]['type'],'NUMBER');
checkSuccess($db,$tableinfo[0]['len'],2);
checkSuccess($db,$tableinfo[0]['flags'],'');
checkSuccess($db,$tableinfo[1]['table'],'');
checkSuccess($db,$tableinfo[1]['name'],'NAME');
checkSuccess($db,$tableinfo[1]['type'],'CHAR');
checkSuccess($db,$tableinfo[1]['len'],5);
checkSuccess($db,$tableinfo[1]['flags'],'');
checkSuccess($db,$tableinfo[2]['table'],'');
checkSuccess($db,$tableinfo[2]['name'],'EMAIL');
checkSuccess($db,$tableinfo[2]['type'],'CHAR');
checkSuccess($db,$tableinfo[2]['len'],20);
checkSuccess($db,$tableinfo[2]['flags'],'');
echo("\n\n");
$tableinfo = $res->tableInfo(DB_TABLEINFO_ORDERTABLE);
checkSuccess($db,$tableinfo['num_fields'],3);
checkSuccess($db,$tableinfo['ordertable']['']['ID'],0);
checkSuccess($db,$tableinfo['ordertable']['']['NAME'],1);
checkSuccess($db,$tableinfo['ordertable']['']['EMAIL'],2);
checkSuccess($db,$tableinfo[0]['table'],'');
checkSuccess($db,$tableinfo[0]['name'],'ID');
checkSuccess($db,$tableinfo[0]['type'],'NUMBER');
checkSuccess($db,$tableinfo[0]['len'],2);
checkSuccess($db,$tableinfo[0]['flags'],'');
checkSuccess($db,$tableinfo[1]['table'],'');
checkSuccess($db,$tableinfo[1]['name'],'NAME');
checkSuccess($db,$tableinfo[1]['type'],'CHAR');
checkSuccess($db,$tableinfo[1]['len'],5);
checkSuccess($db,$tableinfo[1]['flags'],'');
checkSuccess($db,$tableinfo[2]['table'],'');
checkSuccess($db,$tableinfo[2]['name'],'EMAIL');
checkSuccess($db,$tableinfo[2]['type'],'CHAR');
checkSuccess($db,$tableinfo[2]['len'],20);
checkSuccess($db,$tableinfo[2]['flags'],'');
echo("\n\n");
$tableinfo = $res->tableInfo(DB_TABLEINFO_ORDERTABLE|DB_TABLEINFO_ORDER);
checkSuccess($db,$tableinfo['num_fields'],3);
checkSuccess($db,$tableinfo['order']['ID'],0);
checkSuccess($db,$tableinfo['order']['NAME'],1);
checkSuccess($db,$tableinfo['order']['EMAIL'],2);
checkSuccess($db,$tableinfo['ordertable']['']['ID'],0);
checkSuccess($db,$tableinfo['ordertable']['']['NAME'],1);
checkSuccess($db,$tableinfo['ordertable']['']['EMAIL'],2);
checkSuccess($db,$tableinfo[0]['table'],'');
checkSuccess($db,$tableinfo[0]['name'],'ID');
checkSuccess($db,$tableinfo[0]['type'],'NUMBER');
checkSuccess($db,$tableinfo[0]['len'],2);
checkSuccess($db,$tableinfo[0]['flags'],'');
checkSuccess($db,$tableinfo[1]['table'],'');
checkSuccess($db,$tableinfo[1]['name'],'NAME');
checkSuccess($db,$tableinfo[1]['type'],'CHAR');
checkSuccess($db,$tableinfo[1]['len'],5);
checkSuccess($db,$tableinfo[1]['flags'],'');
checkSuccess($db,$tableinfo[2]['table'],'');
checkSuccess($db,$tableinfo[2]['name'],'EMAIL');
checkSuccess($db,$tableinfo[2]['type'],'CHAR');
checkSuccess($db,$tableinfo[2]['len'],20);
checkSuccess($db,$tableinfo[2]['flags'],'');
echo("\n\n");

// FIXME: test prepare/execute

// FIXME: test commit/rollback

$db->disconnect();
?>
