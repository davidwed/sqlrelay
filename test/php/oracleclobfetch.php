<html><pre><?php
# Copyright (c) 2001  David Muse
# See the file COPYING for more information.

ini_set('memory_limit','100M');
set_time_limit(600);

dl("sql_relay.so");

	function checkSuccess($value,$success) {

		if ($value==$success) {
			echo("success ");
		} else {
			echo("$value != $success ");
			echo("failure ");
			sqlrcur_free($cur);
			sqlrcon_free($con);
			exit(0);
		}
	}

	$host="localhost";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="test";
	$password="test";

	# instantiation
	$con=sqlrcon_alloc($host,$port,$socket,$user,$password,0,1);
	$cur=sqlrcur_alloc($con);

	echo("LONG CLOB: \n");
	sqlrcur_prepareQuery($cur,"begin select testclob into :clobbindval from testtable2; end;");
	sqlrcur_defineOutputBindClob($cur,"clobbindval");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$clobbindvar=sqlrcur_getOutputBind($cur,"clobbindval");
	checkSuccess(sqlrcur_getOutputBindLength($cur,"clobbindval"),20*1024*1024);
	echo("\n");

?></pre></html>
