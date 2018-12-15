<html><pre><?php
# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

	function checkSuccess($value,$success) {

		if ($value==$success) {
			echo("success ");
		} else {
			echo("$value != $success ");
			echo("failure ");
			sqlrcur_free($cur);
			sqlrcon_free($con);
			exit(1);
		}
	}

	$host="sqlrelay";
	$port=9000;
	$socket="/tmp/test.socket";
	$user="test";
	$password="test";

	# instantiation
	$con=sqlrcon_alloc($host,$port,$socket,$user,$password,0,1);
	$cur=sqlrcur_alloc($con);

	# get database type
	echo("IDENTIFY: \n");
	checkSuccess(sqlrcon_identify($con),"sqlite");
	echo("\n");

	# ping
	echo("PING: \n");
	checkSuccess(sqlrcon_ping($con),1);
	echo("\n");

	# drop existing table
	sqlrcur_sendQuery($cur,"begin");
	sqlrcur_sendQuery($cur,"drop table testtable");
	sqlrcon_commit($con);

	# create a new table
	echo("CREATE TEMPTABLE: \n");
	sqlrcur_sendQuery($cur,"begin");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable (testint int, testfloat float, testchar char(40), testvarchar varchar(40))"),1);
	sqlrcon_commit($con);
	echo("\n");

	echo("INSERT: \n");
	sqlrcur_sendQuery($cur,"begin");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (1,1.1,'testchar1','testvarchar1')"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (2,2.2,'testchar2','testvarchar2')"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (3,3.3,'testchar3','testvarchar3')"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (4,4.4,'testchar4','testvarchar4')"),1);
	echo("\n");

	echo("AFFECTED ROWS: \n");
	checkSuccess(sqlrcur_affectedRows($cur),0);
	echo("\n");

	echo("BIND BY NAME: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (:var1,:var2,:var3,:var4)");
	checkSuccess(sqlrcur_countBindVariables($cur),4);
	sqlrcur_inputBind($cur,"var1",5);
	sqlrcur_inputBind($cur,"var2",5.5,4,1);
	sqlrcur_inputBind($cur,"var3","testchar5");
	sqlrcur_inputBind($cur,"var4","testvarchar5");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"var1",6);
	sqlrcur_inputBind($cur,"var2",6.6,4,1);
	sqlrcur_inputBind($cur,"var3","testchar6");
	sqlrcur_inputBind($cur,"var4","testvarchar6");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY BIND BY NAME: \n");
	sqlrcur_clearBinds($cur);
	$bindvars=array("var1","var2","var3","var4");
	$bindvals=array(7,7.7,"testchar7","testvarchar7");
	$precs=array(0,2,0,0);
	$scales=array(0,1,0,0);
	sqlrcur_inputBinds($cur,$bindvars,$bindvals,$precs,$scales);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"var1",8);
	sqlrcur_inputBind($cur,"var2",8.8,4,1);
	sqlrcur_inputBind($cur,"var3","testchar8");
	sqlrcur_inputBind($cur,"var4","testvarchar8");
	sqlrcur_validateBinds($cur);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("SELECT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	echo("\n");

	echo("COLUMN COUNT: \n");
	checkSuccess(sqlrcur_colCount($cur),4);
	echo("\n");

	echo("COLUMN NAMES: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"testint");
	checkSuccess(sqlrcur_getColumnName($cur,1),"testfloat");
	checkSuccess(sqlrcur_getColumnName($cur,2),"testchar");
	checkSuccess(sqlrcur_getColumnName($cur,3),"testvarchar");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"testint");
	checkSuccess($cols[1],"testfloat");
	checkSuccess($cols[2],"testchar");
	checkSuccess($cols[3],"testvarchar");
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess(sqlrcur_getColumnType($cur,0),"INTEGER");
	checkSuccess(sqlrcur_getColumnType($cur,"testint"),"INTEGER");
	checkSuccess(sqlrcur_getColumnType($cur,1),"FLOAT");
	checkSuccess(sqlrcur_getColumnType($cur,"testfloat"),"FLOAT");
	checkSuccess(sqlrcur_getColumnType($cur,2),"STRING");
	checkSuccess(sqlrcur_getColumnType($cur,"testchar"),"STRING");
	checkSuccess(sqlrcur_getColumnType($cur,3),"STRING");
	checkSuccess(sqlrcur_getColumnType($cur,"testvarchar"),"STRING");
	echo("\n");

	echo("COLUMN LENGTH: \n");
	checkSuccess(sqlrcur_getColumnLength($cur,0),0);
	checkSuccess(sqlrcur_getColumnLength($cur,"testint"),0);
	checkSuccess(sqlrcur_getColumnLength($cur,1),0);
	checkSuccess(sqlrcur_getColumnLength($cur,"testfloat"),0);
	checkSuccess(sqlrcur_getColumnLength($cur,2),0);
	checkSuccess(sqlrcur_getColumnLength($cur,"testchar"),0);
	checkSuccess(sqlrcur_getColumnLength($cur,3),0);
	checkSuccess(sqlrcur_getColumnLength($cur,"testvarchar"),0);
	echo("\n");

	echo("LONGEST COLUMN: \n");
	checkSuccess(sqlrcur_getLongest($cur,0),1);
	checkSuccess(sqlrcur_getLongest($cur,"testint"),1);
	checkSuccess(sqlrcur_getLongest($cur,1),3);
	checkSuccess(sqlrcur_getLongest($cur,"testfloat"),3);
	checkSuccess(sqlrcur_getLongest($cur,2),9);
	checkSuccess(sqlrcur_getLongest($cur,"testchar"),9);
	checkSuccess(sqlrcur_getLongest($cur,3),12);
	checkSuccess(sqlrcur_getLongest($cur,"testvarchar"),12);
	echo("\n");

	echo("ROW COUNT: \n");
	checkSuccess(sqlrcur_rowCount($cur),8);
	echo("\n");

	echo("TOTAL ROWS: \n");
	checkSuccess(sqlrcur_totalRows($cur),0);
	echo("\n");

	echo("FIRST ROW INDEX: \n");
	checkSuccess(sqlrcur_firstRowIndex($cur),0);
	echo("\n");

	echo("END OF RESULT SET: \n");
	checkSuccess(sqlrcur_endOfResultSet($cur),1);
	echo("\n");

	echo("FIELDS BY INDEX: \n");
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"1.1");
	checkSuccess(sqlrcur_getField($cur,0,2),"testchar1");
	checkSuccess(sqlrcur_getField($cur,0,3),"testvarchar1");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,7,1),"8.8");
	checkSuccess(sqlrcur_getField($cur,7,2),"testchar8");
	checkSuccess(sqlrcur_getField($cur,7,3),"testvarchar8");
	echo("\n");

	echo("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,1),3);
	checkSuccess(sqlrcur_getFieldLength($cur,0,2),9);
	checkSuccess(sqlrcur_getFieldLength($cur,0,3),12);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,1),3);
	checkSuccess(sqlrcur_getFieldLength($cur,7,2),9);
	checkSuccess(sqlrcur_getFieldLength($cur,7,3),12);
	echo("\n");

	echo("FIELDS BY NAME: \n");
	checkSuccess(sqlrcur_getField($cur,0,"testint"),"1");
	checkSuccess(sqlrcur_getField($cur,0,"testfloat"),"1.1");
	checkSuccess(sqlrcur_getField($cur,0,"testchar"),"testchar1");
	checkSuccess(sqlrcur_getField($cur,0,"testvarchar"),"testvarchar1");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,"testint"),"8");
	checkSuccess(sqlrcur_getField($cur,7,"testfloat"),"8.8");
	checkSuccess(sqlrcur_getField($cur,7,"testchar"),"testchar8");
	checkSuccess(sqlrcur_getField($cur,7,"testvarchar"),"testvarchar8");
	echo("\n");

	echo("FIELD LENGTHS BY NAME: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testint"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testfloat"),3);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testchar"),9);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testvarchar"),12);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testint"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testfloat"),3);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testchar"),9);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testvarchar"),12);
	echo("\n");

	echo("FIELDS BY ARRAY: \n");
	$fields=sqlrcur_getRow($cur,0);
	checkSuccess($fields[0],"1");
	checkSuccess($fields[1],"1.1");
	checkSuccess($fields[2],"testchar1");
	checkSuccess($fields[3],"testvarchar1");
	echo("\n");

	echo("FIELD LENGTHS BY ARRAY: \n");
	$fieldlens=sqlrcur_getRowLengths($cur,0);
	checkSuccess($fieldlens[0],1);
	checkSuccess($fieldlens[1],3);
	checkSuccess($fieldlens[2],9);
	checkSuccess($fieldlens[3],12);
	echo("\n");



	echo("FIELDS BY ASSOCIATIVE ARRAY: \n");
	$fields=sqlrcur_getRowAssoc($cur,0);
	checkSuccess($fields["testint"],"1");
	checkSuccess($fields["testfloat"],"1.1");
	checkSuccess($fields["testchar"],"testchar1");
	checkSuccess($fields["testvarchar"],"testvarchar1");
	echo("\n");
	$fields=sqlrcur_getRowAssoc($cur,7);
	checkSuccess($fields["testint"],"8");
	checkSuccess($fields["testfloat"],"8.8");
	checkSuccess($fields["testchar"],"testchar8");
	checkSuccess($fields["testvarchar"],"testvarchar8");
	echo("\n");

	echo("FIELD LENGTHS BY ASSOCIATIVE ARRAY: \n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,0);
	checkSuccess($fieldlengths["testint"],1);
	checkSuccess($fieldlengths["testfloat"],3);
	checkSuccess($fieldlengths["testchar"],9);
	checkSuccess($fieldlengths["testvarchar"],12);
	echo("\n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,7);
	checkSuccess($fieldlengths["testint"],1);
	checkSuccess($fieldlengths["testfloat"],3);
	checkSuccess($fieldlengths["testchar"],9);
	checkSuccess($fieldlengths["testvarchar"],12);
	echo("\n");



	echo("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_sendQuery($cur,"drop table testtable1");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable1 (col1 int, col2 char, col3 float)"),1);
	sqlrcur_prepareQuery($cur,"insert into testtable1 values ($(var1),'$(var2)',$(var3))");
	sqlrcur_substitution($cur,"var1",1);
	sqlrcur_substitution($cur,"var2","hello");
	sqlrcur_substitution($cur,"var3",10.5556,6,4);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("FIELDS: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"hello");
	checkSuccess(sqlrcur_getField($cur,0,2),"10.5556");
	checkSuccess(sqlrcur_sendQuery($cur,"delete from testtable1"),1);
	echo("\n");

	echo("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable1 values ($(var1),'$(var2)',$(var3))");
	$vars=array("var1","var2","var3");
	$vals=array(1,"hello",10.5556);
	$precs=array(0,0,6);
	$scales=array(0,0,4);
	sqlrcur_substitutions($cur,$vars,$vals,$precs,$scales);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("FIELDS: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"hello");
	checkSuccess(sqlrcur_getField($cur,0,2),"10.5556");
	checkSuccess(sqlrcur_sendQuery($cur,"delete from testtable1"),1);
	echo("\n");

	echo("NULLS as Nulls: \n");
	sqlrcur_getNullsAsNulls($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable1 values (1,NULL,NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),NULL);
	checkSuccess(sqlrcur_getField($cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"");
	checkSuccess(sqlrcur_getField($cur,0,2),"");
	sqlrcur_getNullsAsNulls($cur);
	echo("\n");

	echo("RESULT SET BUFFER SIZE: \n");
	checkSuccess(sqlrcur_getResultSetBufferSize($cur),0);
	sqlrcur_setResultSetBufferSize($cur,2);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getResultSetBufferSize($cur),2);
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),0);
	checkSuccess(sqlrcur_endOfResultSet($cur),0);
	checkSuccess(sqlrcur_rowCount($cur),2);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,1,0),"2");
	checkSuccess(sqlrcur_getField($cur,2,0),"3");
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),2);
	checkSuccess(sqlrcur_endOfResultSet($cur),0);
	checkSuccess(sqlrcur_rowCount($cur),4);
	checkSuccess(sqlrcur_getField($cur,6,0),"7");
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),6);
	checkSuccess(sqlrcur_endOfResultSet($cur),0);
	checkSuccess(sqlrcur_rowCount($cur),8);
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),8);
	checkSuccess(sqlrcur_endOfResultSet($cur),1);
	checkSuccess(sqlrcur_rowCount($cur),8);
	echo("\n");

	echo("DONT GET COLUMN INFO: \n");
	sqlrcur_dontGetColumnInfo($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getColumnName($cur,0),NULL);
	checkSuccess(sqlrcur_getColumnLength($cur,0),0);
	checkSuccess(sqlrcur_getColumnType($cur,0),NULL);
	sqlrcur_getColumnInfo($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getColumnName($cur,0),"testint");
	checkSuccess(sqlrcur_getColumnLength($cur,0),0);
	checkSuccess(sqlrcur_getColumnType($cur,0),"INTEGER");
	echo("\n");

	echo("SUSPENDED SESSION: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet($cur);
	checkSuccess(sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess(sqlrcon_resumeSession($con,$conport,$consocket),1);
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,1,0),"2");
	checkSuccess(sqlrcur_getField($cur,2,0),"3");
	checkSuccess(sqlrcur_getField($cur,3,0),"4");
	checkSuccess(sqlrcur_getField($cur,4,0),"5");
	checkSuccess(sqlrcur_getField($cur,5,0),"6");
	checkSuccess(sqlrcur_getField($cur,6,0),"7");
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet($cur);
	checkSuccess(sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess(sqlrcon_resumeSession($con,$conport,$consocket),1);
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,1,0),"2");
	checkSuccess(sqlrcur_getField($cur,2,0),"3");
	checkSuccess(sqlrcur_getField($cur,3,0),"4");
	checkSuccess(sqlrcur_getField($cur,4,0),"5");
	checkSuccess(sqlrcur_getField($cur,5,0),"6");
	checkSuccess(sqlrcur_getField($cur,6,0),"7");
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet($cur);
	checkSuccess(sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess(sqlrcon_resumeSession($con,$conport,$consocket),1);
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,1,0),"2");
	checkSuccess(sqlrcur_getField($cur,2,0),"3");
	checkSuccess(sqlrcur_getField($cur,3,0),"4");
	checkSuccess(sqlrcur_getField($cur,4,0),"5");
	checkSuccess(sqlrcur_getField($cur,5,0),"6");
	checkSuccess(sqlrcur_getField($cur,6,0),"7");
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");

	echo("SUSPENDED RESULT SET: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getField($cur,2,0),"3");
	$id=sqlrcur_getResultSetId($cur);
	sqlrcur_suspendResultSet($cur);
	checkSuccess(sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess(sqlrcon_resumeSession($con,$conport,$consocket),1);
	checkSuccess(sqlrcur_resumeResultSet($cur,$id),1);
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),4);
	checkSuccess(sqlrcur_endOfResultSet($cur),0);
	checkSuccess(sqlrcur_rowCount($cur),6);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),6);
	checkSuccess(sqlrcur_endOfResultSet($cur),0);
	checkSuccess(sqlrcur_rowCount($cur),8);
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),8);
	checkSuccess(sqlrcur_endOfResultSet($cur),1);
	checkSuccess(sqlrcur_rowCount($cur),8);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("CACHED RESULT SET: \n");
	sqlrcur_cacheToFile($cur,"cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");

	echo("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_colCount($cur),4);
	echo("\n");

	echo("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"testint");
	checkSuccess(sqlrcur_getColumnName($cur,1),"testfloat");
	checkSuccess(sqlrcur_getColumnName($cur,2),"testchar");
	checkSuccess(sqlrcur_getColumnName($cur,3),"testvarchar");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"testint");
	checkSuccess($cols[1],"testfloat");
	checkSuccess($cols[2],"testchar");
	checkSuccess($cols[3],"testvarchar");
	echo("\n");

	echo("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("FROM ONE CACHE FILE TO ANOTHER: \n");
	sqlrcur_cacheToFile($cur,"cachefile2");
	checkSuccess(sqlrcur_openCachedResultSet($cur,"cachefile1"),1);
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,"cachefile2"),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	echo("\n");

	echo("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"cachefile2");
	checkSuccess(sqlrcur_openCachedResultSet($cur,"cachefile1"),1);
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,"cachefile2"),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getField($cur,2,0),"3");
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"cachefile1");
	$id=sqlrcur_getResultSetId($cur);
	sqlrcur_suspendResultSet($cur);
	checkSuccess(sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	echo("\n");
	checkSuccess(sqlrcon_resumeSession($con,$conport,$consocket),1);
	checkSuccess(sqlrcur_resumeCachedResultSet($cur,$id,$filename),1);
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),4);
	checkSuccess(sqlrcur_endOfResultSet($cur),0);
	checkSuccess(sqlrcur_rowCount($cur),6);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),6);
	checkSuccess(sqlrcur_endOfResultSet($cur),0);
	checkSuccess(sqlrcur_rowCount($cur),8);
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),8);
	checkSuccess(sqlrcur_endOfResultSet($cur),1);
	checkSuccess(sqlrcur_rowCount($cur),8);
	sqlrcur_cacheOff($cur);
	echo("\n");
	checkSuccess(sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("COMMIT AND ROLLBACK: \n");
	$secondcon=sqlrcon_alloc($host,$port,$socket,$user,$password,0,1);
	$secondcur=sqlrcur_alloc($secondcon);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"0");
	checkSuccess(sqlrcon_commit($con),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"8");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (10,10.1,'testchar10','testvarchar10')"),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"9");
	echo("\n");

	# drop existing table
	sqlrcur_sendQuery($cur,"drop table testtable");

	# invalid queries...
	echo("INVALID QUERIES: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable"),0);
	echo("\n");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (1,2,3,4)"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (1,2,3,4)"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (1,2,3,4)"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (1,2,3,4)"),0);
	echo("\n");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable"),0);
	echo("\n");

?></pre></html>
