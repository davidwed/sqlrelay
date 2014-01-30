<?php

dl("pdo_sqlrelay.so");

function tcPrintException($tc, $e)
{
	// just print
	echo "exception - tc:{$tc}, code:", $e->getCode(), ", message:", $e->getMessage(), PHP_EOL;
}

function tcCheck($tc, $value, $expect)
{
	if ($value === $expect)
	{
		echo "success - tc:{$tc}", PHP_EOL;
	}
	else
	{
		echo "failure - tc:{$tc}, {$value} !== {$expect}", PHP_EOL;
	}
}

function tcSuccess($tc, $msg = "success")
{
	echo "success - tc:{$tc}, msg:{$msg}", PHP_EOL;
}

function tcFailure($tc, $msg = "failure", $exit = TRUE)
{
	echo "failure - tc:{$tc}, msg:{$msg}", PHP_EOL;
	
	if ($exit)
	{
		exit(0);
	}
}


// for pdo_mysql
$user = "testuser";
$pass = "testpassword";
$dsn = "mysql:host=db64;port=3306;dbname=testdb";


define('SQLRELAY_TEST', 1);
if (defined('SQLRELAY_TEST'))
{
	// for pdo_sqlrelay
	echo "SQLRELAY_TEST",PHP_EOL;
	$user = "test";
	$pass = "test";
	$dsn = "sqlrelay:host=localhost;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0";
}


// pdo options
$options = array(
	PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
	PDO::ATTR_TIMEOUT => 5,
	PDO::ATTR_EMULATE_PREPARES => false,
	#PDO::ATTR_EMULATE_PREPARES => true,
	PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8",
	PDO::MYSQL_ATTR_FOUND_ROWS => true,
);

$db = null;


// ----------------------------------------------------------------------------------------------
$tc = "connect failure test";
try 
{
	$db = new PDO($dsn, $user, $user, $options); // invalid password
	tcFailure($tc, "expect expection about connect failure.. but not throw", false);
}
catch(PDOException $e)
{
	tcPrintException($tc, $e);
	tcSuccess($tc);
}

// so db variable check
try
{
	$db = new PDO($dsn, $user, $user, $options); // invalid password
	
	if (empty($db))
	{
		tcSuccess($tc);
	}
	else
	{
		tcFailure($tc, "expect db variable is empty.. but not empty", false);
	}
}
catch (PDOException $e)
{
	tcSuccess($tc);
}

// anyway can not check... conect success or failure..


// ----------------------------------------------------------------------------------------------
$tc = "connect success test";
try
{
	$db = new PDO($dsn, $user, $pass, $options); // valid password
	tcSuccess($tc, "is this really success?..");
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcFailure($tc, "expect login success.. but exception throw");
}


// ----------------------------------------------------------------------------------------------
$tc = "ping test";
try 
{
	$stmt = $db->query("SELECT 1 AS val;");
	$row = $stmt->fetch(PDO::FETCH_ASSOC);
	
	if ($row['val'] == 1) //
	{
		tcSuccess($tc);
	}
	else
	{
		tcFailure($tc, "expect val = 1 but {$row['val']}", false);
	}
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcFailure($tc, "why failure?");
}
unset($stmt);


// ----------------------------------------------------------------------------------------------
$tc = "invalid query test";
try
{
	$stmt = $db->query("SELECT * FROM notExistTable;");
	tcFailure($tc, "expect exception about invalid query.. but not throw", false);
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcSuccess($tc);
}


// ----------------------------------------------------------------------------------------------

try
{
	$sql = <<<SQL
DROP TABLE testtable;
SQL;
	
	$db->exec($sql);
}
catch (PDOException $e)
{
	
}


$tc = "create table test";
try
{
	$sql = <<<SQL
CREATE TABLE testtable(
	rid INT NOT NULL AUTO_INCREMENT,
	uid INT NOT NULL,
	value1 INT NOT NULL,
	value2 INT NOT NULL,
	value3 INT NOT NULL,
	msg1 VARCHAR(32) NOT NULL,
	msg2 VARCHAR(32) NOT NULL,
	msg3 VARCHAR(32) NOT NULL,
	createAt DATETIME NOT NULL,
	PRIMARY KEY(rid),
	INDEX uid (uid)
);
SQL;
	$rowcnt = $db->exec($sql);
	
	tcSuccess($tc);
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcFailure($tc, "why failure?");
}

// ----------------------------------------------------------------------------------------------

$tc = "insert with prepare";
try
{
	$sql = <<<SQL
INSERT INTO testtable(uid, value2, value1, value3, msg1, msg2, msg3, createAt)
   VALUES(:uid, :value2, :value1, :value3, :msg1, :msg2, :msg3, :createAt);
SQL;
	
	$stmt = $db->prepare($sql);
	
	$value1 = 100;
	$value2 = 200;
	$value3 = 300;
	$msg1 = 'Hello1';
	$msg2 = 'Hello2';
	$msg3 = 'Hello3';
	$createAt = date("Y-m-d H:i:s");
	
	for ($i = 1; $i <= 5; $i++)
	{
		
		tcCheck('bindValue', $stmt->bindValue(':uid', $i, PDO::PARAM_INT), true);
		tcCheck('bindValue', $stmt->bindValue(':value1', $value1, PDO::PARAM_INT), true);
		tcCheck('bindValue', $stmt->bindValue(':value2', $value2, PDO::PARAM_INT), true);
		tcCheck('bindValue', $stmt->bindValue(':value3', $value3, PDO::PARAM_INT), true);
		tcCheck('bindValue', $stmt->bindValue(':msg1', $msg1, PDO::PARAM_STR), true);
		tcCheck('bindValue', $stmt->bindValue(':msg2', $msg2, PDO::PARAM_STR), true);
		tcCheck('bindValue', $stmt->bindValue(':msg3', $msg3, PDO::PARAM_STR), true);
		tcCheck('bindValue', $stmt->bindValue(':createAt', $createAt, PDO::PARAM_STR), true);
		
		tcCheck('execute', $stmt->execute(), true);
		
		tcCheck('rowCount', $stmt->rowCount(), 1);
		
		$last = $db->lastInsertId();
		tcCheck('lastInsertId', (int)$last, $i);
	}
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcFailure($tc, "why failure?");
}
unset($stmt);

// ----------------------------------------------------------------------------------------------

$tc = "update test (same value)";
try
{
	$sql = <<<SQL
UPDATE testtable
   SET value1 = 100
 WHERE uid = 1;
SQL;
	
// 	$stmt = $db->query($sql);
// 	$rowcnt = $stmt->rowCount();
	$rowcnt = $db->exec($sql);
	
	if ($rowcnt === 1)
	{
		tcSuccess($tc);
	}
	else
	{
		tcFailure($tc, "expect rowcount 1.. but {$rowcnt}", false); // about PDO::MYSQL_ATTR_FOUND_ROWS
	}
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcFailure($tc, "why failure?");
}
unset($stmt);


// ----------------------------------------------------------------------------------------------

$tc = "update test (changed value)";
try
{
	$sql = <<<SQL
UPDATE testtable
   SET value1 = 1000
 WHERE uid = 1;
SQL;
	
	$stmt = $db->query($sql);
	$rowcnt = $stmt->rowCount();
//	$rowcnt = $db->exec($sql);
	
	if ($rowcnt === 1)
	{
		tcSuccess($tc);
	}
	else
	{
		tcFailure($tc, "expect rowcount 1.. but {$rowcnt}", false);
	}
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcFailure($tc, "why failure?");
}
unset($stmt);


// ----------------------------------------------------------------------------------------------

$tc = "select test (native type)";
try
{
	$sql = <<<SQL
SELECT *
  FROM testtable
 WHERE uid = 1;
SQL;
	
	$stmt = $db->query($sql);
	$rowcnt = $stmt->rowCount();
	
	tcCheck('selectRowCount', $rowcnt, 1);
	
	$row = $stmt->fetch(PDO::FETCH_ASSOC);
	
	tcCheck('nativeType', $row['value1'], 1000);
	tcCheck('nativeType', $row['value2'], 200);
	tcCheck('nativeType', $row['value3'], 300);
	tcCheck('nativeType', $row['msg1'], 'Hello1');
	tcCheck('nativeType', $row['msg2'], 'Hello2');
	tcCheck('nativeType', $row['msg3'], 'Hello3');
	
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcFailure($tc, "why failure?");
}
unset($stmt);


// ----------------------------------------------------------------------------------------------

$tc = "select test (native type multiple)";
try
{
	$sql = <<<SQL
SELECT *
  FROM testtable
 WHERE uid IN (1,2);
SQL;

	$stmt = $db->query($sql);
	$rowcnt = $stmt->rowCount();
	
	tcCheck('selectRowCount', $rowcnt, 2);
	
	$row = $stmt->fetch(PDO::FETCH_ASSOC);
	tcCheck('nativeType', $row['value1'], 1000);
	tcCheck('nativeType', $row['msg1'], 'Hello1');
	
	$row = $stmt->fetch(PDO::FETCH_ASSOC);
	tcCheck('nativeType', $row['value1'], 100);
	tcCheck('nativeType', $row['msg1'], 'Hello1');
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcFailure($tc, "why failure?");
}
unset($stmt);


// ----------------------------------------------------------------------------------------------

$tc = "delete test";
try
{
	$sql = <<<SQL
DELETE FROM testtable
 WHERE uid IN (1,2);
SQL;
	
	$stmt = $db->query($sql);
	$rowcnt = $stmt->rowCount();
//	$rowcnt = $db->exec($sql);
	
	tcCheck('selectRowCount', $rowcnt, 2);
	
	// reselect
	$sql = <<<SQL
SELECT *
  FROM testtable
 WHERE uid IN (1,2);
SQL;
	
	$stmt = $db->query($sql);
	$rowcnt = $stmt->rowCount();
	
	tcCheck('selectRowCount', $rowcnt, 0);
}
catch (PDOException $e)
{
	tcPrintException($tc, $e);
	tcFailure($tc, "why failure?");
}
unset($stmt);





