<html><pre><?php
# Copyright (c) 2001  David Muse
# See the file COPYING for more information.

dl("sql_relay.so");

function checkSuccess($value,$success) {

	if ($value==$success) {
		echo("success ");
	} else {
		echo("failure ");
		sqlrcur_free($cur);
		sqlrcon_free($con);
		exit(0);
	}
}

	# instantiation
	$con=sqlrcon_alloc($host,$port,$socket,$user,$password,0,1);
	$cur=sqlrcur_alloc($con);

	# get database type
	echo("IDENTIFY: \n");
	checkSuccess(sqlrcon_identify($con),"msql");
	echo("\n");

	# ping
	echo("PING: \n");
	checkSuccess(sqlrcon_ping($con),1);
	echo("\n");

	# drop existing table
	sqlrcur_sendQuery($cur,"drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable (testchar char(40), testdate date, testint int, testmoney money, testreal real, testtext text(40), testtime time, testuint uint)"),1);
	echo("\n");

	echo("INSERT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values ('char1','01-Jan-2001',1,1.00,1.1,'text1','01:00:00',1)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values ('char2','01-Jan-2002',2,2.00,2.1,'text2','02:00:00',2)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values ('char3','01-Jan-2003',3,3.00,3.1,'text3','03:00:00',3)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values ('char4','01-Jan-2004',4,4.00,4.1,'text4','04:00:00',4)"),1);
	echo("\n");

	echo("AFFECTED ROWS: \n");
	checkSuccess(sqlrcur_affectedRows($cur),-1);
	echo("\n");

	echo("BIND BY NAME: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8)");
	sqlrcur_inputBind($cur,"var1","char5");
	sqlrcur_inputBind($cur,"var2","01-Jan-2005");
	sqlrcur_inputBind($cur,"var3",5);
	sqlrcur_inputBind($cur,"var4",5.00,3,2);
	sqlrcur_inputBind($cur,"var5",5.1,2,1);
	sqlrcur_inputBind($cur,"var6","text5");
	sqlrcur_inputBind($cur,"var7","05:00:00");
	sqlrcur_inputBind($cur,"var8",5);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"var1","char6");
	sqlrcur_inputBind($cur,"var2","01-Jan-2006");
	sqlrcur_inputBind($cur,"var3",6);
	sqlrcur_inputBind($cur,"var4",6.00,3,2);
	sqlrcur_inputBind($cur,"var5",6.1,2,1);
	sqlrcur_inputBind($cur,"var6","text6");
	sqlrcur_inputBind($cur,"var7","06:00:00");
	sqlrcur_inputBind($cur,"var8",6);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY BIND BY NAME: \n");
	sqlrcur_clearBinds($cur);
	$bindvars=array("var1","var2","var3","var4","var5","var6","var7","var8");
	$bindvals=array("char7","01-Jan-2007",7,7.00,7.1,"text7","07:00:00",7);
	$precs=array(0,0,0,3,2,0,0,0);
	$scales=array(0,0,0,2,1,0,0,0);
	sqlrcur_inputBinds($cur,$bindvars,$bindvals,$precs,$scales);
	checkSuccess(sqlrcur_executeQuery($cur),1);

	echo("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"var1","char8");
	sqlrcur_inputBind($cur,"var2","01-Jan-2008");
	sqlrcur_inputBind($cur,"var3",8);
	sqlrcur_inputBind($cur,"var4",8.00,3,2);
	sqlrcur_inputBind($cur,"var5",8.1,2,1);
	sqlrcur_inputBind($cur,"var6","text8");
	sqlrcur_inputBind($cur,"var7","08:00:00");
	sqlrcur_inputBind($cur,"var8",8);
	sqlrcur_inputBind($cur,"var9","junkvalue");
	sqlrcur_validateBinds($cur);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("SELECT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	echo("\n");

	echo("COLUMN COUNT: \n");
	checkSuccess(sqlrcur_colCount($cur),8);
	echo("\n");

	echo("COLUMN NAMES: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"testchar");
	checkSuccess(sqlrcur_getColumnName($cur,1),"testdate");
	checkSuccess(sqlrcur_getColumnName($cur,2),"testint");
	checkSuccess(sqlrcur_getColumnName($cur,3),"testmoney");
	checkSuccess(sqlrcur_getColumnName($cur,4),"testreal");
	checkSuccess(sqlrcur_getColumnName($cur,5),"testtext");
	checkSuccess(sqlrcur_getColumnName($cur,6),"testtime");
	checkSuccess(sqlrcur_getColumnName($cur,7),"testuint");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"testchar");
	checkSuccess($cols[1],"testdate");
	checkSuccess($cols[2],"testint");
	checkSuccess($cols[3],"testmoney");
	checkSuccess($cols[4],"testreal");
	checkSuccess($cols[5],"testtext");
	checkSuccess($cols[6],"testtime");
	checkSuccess($cols[7],"testuint");
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess(sqlrcur_getColumnType($cur,0),"CHAR");
	checkSuccess(sqlrcur_getColumnType($cur,"testchar"),"CHAR");
	checkSuccess(sqlrcur_getColumnType($cur,1),"DATE");
	checkSuccess(sqlrcur_getColumnType($cur,"testdate"),"DATE");
	checkSuccess(sqlrcur_getColumnType($cur,2),"INT");
	checkSuccess(sqlrcur_getColumnType($cur,"testint"),"INT");
	checkSuccess(sqlrcur_getColumnType($cur,3),"MONEY");
	checkSuccess(sqlrcur_getColumnType($cur,"testmoney"),"MONEY");
	checkSuccess(sqlrcur_getColumnType($cur,4),"REAL");
	checkSuccess(sqlrcur_getColumnType($cur,"testreal"),"REAL");
	checkSuccess(sqlrcur_getColumnType($cur,5),"TEXT");
	checkSuccess(sqlrcur_getColumnType($cur,"testtext"),"TEXT");
	checkSuccess(sqlrcur_getColumnType($cur,6),"TIME");
	checkSuccess(sqlrcur_getColumnType($cur,"testtime"),"TIME");
	checkSuccess(sqlrcur_getColumnType($cur,7),"UINT");
	checkSuccess(sqlrcur_getColumnType($cur,"testuint"),"UINT");
	echo("\n");

	echo("COLUMN LENGTH: \n");
	checkSuccess(sqlrcur_getColumnLength($cur,0),40);
	checkSuccess(sqlrcur_getColumnLength($cur,"testchar"),40);
	checkSuccess(sqlrcur_getColumnLength($cur,1),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"testdate"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,2),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"testint"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,3),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"testmoney"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,4),8);
	checkSuccess(sqlrcur_getColumnLength($cur,"testreal"),8);
	checkSuccess(sqlrcur_getColumnLength($cur,5),40);
	checkSuccess(sqlrcur_getColumnLength($cur,"testtext"),40);
	checkSuccess(sqlrcur_getColumnLength($cur,6),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"testtime"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,7),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"testuint"),4);
	echo("\n");

	echo("LONGEST COLUMN: \n");
	checkSuccess(sqlrcur_getLongest($cur,0),5);
	checkSuccess(sqlrcur_getLongest($cur,"testchar"),5);
	checkSuccess(sqlrcur_getLongest($cur,1),11);
	checkSuccess(sqlrcur_getLongest($cur,"testdate"),11);
	checkSuccess(sqlrcur_getLongest($cur,2),1);
	checkSuccess(sqlrcur_getLongest($cur,"testint"),1);
	checkSuccess(sqlrcur_getLongest($cur,3),4);
	checkSuccess(sqlrcur_getLongest($cur,"testmoney"),4);
	checkSuccess(sqlrcur_getLongest($cur,4),3);
	checkSuccess(sqlrcur_getLongest($cur,"testreal"),3);
	checkSuccess(sqlrcur_getLongest($cur,5),5);
	checkSuccess(sqlrcur_getLongest($cur,"testtext"),5);
	checkSuccess(sqlrcur_getLongest($cur,6),8);
	checkSuccess(sqlrcur_getLongest($cur,"testtime"),8);
	checkSuccess(sqlrcur_getLongest($cur,7),1);
	checkSuccess(sqlrcur_getLongest($cur,"testuint"),1);
	echo("\n");

	echo("ROW COUNT: \n");
	checkSuccess(sqlrcur_rowCount($cur),8);
	echo("\n");

	echo("TOTAL ROWS: \n");
	checkSuccess(sqlrcur_totalRows($cur),8);
	echo("\n");

	echo("FIRST ROW INDEX: \n");
	checkSuccess(sqlrcur_firstRowIndex($cur),0);
	echo("\n");

	echo("END OF RESULT SET: \n");
	checkSuccess(sqlrcur_endOfResultSet($cur),1);
	echo("\n");

	echo("FIELDS BY INDEX: \n");
	checkSuccess(sqlrcur_getField($cur,0,0),"char1");
	checkSuccess(sqlrcur_getField($cur,0,1),"01-Jan-2001");
	checkSuccess(sqlrcur_getField($cur,0,2),"1");
	checkSuccess(sqlrcur_getField($cur,0,3),"1.00");
	checkSuccess(sqlrcur_getField($cur,0,4),"1.1");
	checkSuccess(sqlrcur_getField($cur,0,5),"text1");
	checkSuccess(sqlrcur_getField($cur,0,6),"01:00:00");
	checkSuccess(sqlrcur_getField($cur,0,7),"1");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
	checkSuccess(sqlrcur_getField($cur,7,1),"01-Jan-2008");
	checkSuccess(sqlrcur_getField($cur,7,2),"8");
	checkSuccess(sqlrcur_getField($cur,7,3),"8.00");
	checkSuccess(sqlrcur_getField($cur,7,4),"8.1");
	checkSuccess(sqlrcur_getField($cur,7,5),"text8");
	checkSuccess(sqlrcur_getField($cur,7,6),"08:00:00");
	checkSuccess(sqlrcur_getField($cur,7,7),"8");
	echo("\n");

	echo("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,0),5);
	checkSuccess(sqlrcur_getFieldLength($cur,0,1),11);
	checkSuccess(sqlrcur_getFieldLength($cur,0,2),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,3),4);
	checkSuccess(sqlrcur_getFieldLength($cur,0,4),3);
	checkSuccess(sqlrcur_getFieldLength($cur,0,5),5);
	checkSuccess(sqlrcur_getFieldLength($cur,0,6),8);
	checkSuccess(sqlrcur_getFieldLength($cur,0,7),1);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,0),5);
	checkSuccess(sqlrcur_getFieldLength($cur,7,1),11);
	checkSuccess(sqlrcur_getFieldLength($cur,7,2),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,3),4);
	checkSuccess(sqlrcur_getFieldLength($cur,7,4),3);
	checkSuccess(sqlrcur_getFieldLength($cur,7,5),5);
	checkSuccess(sqlrcur_getFieldLength($cur,7,6),8);
	checkSuccess(sqlrcur_getFieldLength($cur,7,7),1);
	echo("\n");

	echo("FIELDS BY NAME: \n");
	checkSuccess(sqlrcur_getField($cur,0,"testchar"),"char1");
	checkSuccess(sqlrcur_getField($cur,0,"testdate"),"01-Jan-2001");
	checkSuccess(sqlrcur_getField($cur,0,"testint"),"1");
	checkSuccess(sqlrcur_getField($cur,0,"testmoney"),"1.00");
	checkSuccess(sqlrcur_getField($cur,0,"testreal"),"1.1");
	checkSuccess(sqlrcur_getField($cur,0,"testtext"),"text1");
	checkSuccess(sqlrcur_getField($cur,0,"testtime"),"01:00:00");
	checkSuccess(sqlrcur_getField($cur,0,"testuint"),"1");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,"testchar"),"char8");
	checkSuccess(sqlrcur_getField($cur,7,"testdate"),"01-Jan-2008");
	checkSuccess(sqlrcur_getField($cur,7,"testint"),"8");
	checkSuccess(sqlrcur_getField($cur,7,"testmoney"),"8.00");
	checkSuccess(sqlrcur_getField($cur,7,"testreal"),"8.1");
	checkSuccess(sqlrcur_getField($cur,7,"testtext"),"text8");
	checkSuccess(sqlrcur_getField($cur,7,"testtime"),"08:00:00");
	checkSuccess(sqlrcur_getField($cur,7,"testuint"),"8");
	echo("\n");

	echo("FIELD LENGTHS BY NAME: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testchar"),5);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testdate"),11);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testint"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testmoney"),4);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testreal"),3);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testtext"),5);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testtime"),8);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testuint"),1);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testchar"),5);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testdate"),11);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testint"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testmoney"),4);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testreal"),3);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testtext"),5);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testtime"),8);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testuint"),1);
	echo("\n");

	echo("FIELDS BY ARRAY: \n");
	$fields=sqlrcur_getRow($cur,0);
	checkSuccess($fields[0],"char1");
	checkSuccess($fields[1],"01-Jan-2001");
	checkSuccess($fields[2],"1");
	checkSuccess($fields[3],"1.00");
	checkSuccess($fields[4],"1.1");
	checkSuccess($fields[5],"text1");
	checkSuccess($fields[6],"01:00:00");
	checkSuccess($fields[7],"1");
	echo("\n");

	echo("FIELD LENGTHS BY ARRAY: \n");
	$fieldlens=sqlrcur_getRowLengths($cur,0);
	checkSuccess($fieldlens[0],5);
	checkSuccess($fieldlens[1],11);
	checkSuccess($fieldlens[2],1);
	checkSuccess($fieldlens[3],4);
	checkSuccess($fieldlens[4],3);
	checkSuccess($fieldlens[5],5);
	checkSuccess($fieldlens[6],8);
	checkSuccess($fieldlens[7],1);
	echo("\n");


	echo("FIELDS BY ASSOCIATIVE ARRAY: \n");
	$fields=sqlrcur_getRowAssoc($cur,0);
	checkSuccess($fields["testchar"],"char1");
	checkSuccess($fields["testdate"],"01-Jan-2001");
	checkSuccess($fields["testint"],"1");
	checkSuccess($fields["testmoney"],"1.00");
	checkSuccess($fields["testreal"],"1.1");
	checkSuccess($fields["testtext"],"text1");
	checkSuccess($fields["testtime"],"01:00:00");
	checkSuccess($fields["testuint"],"1");
	echo("\n");
	$fields=sqlrcur_getRowAssoc($cur,7);
	checkSuccess($fields["testchar"],"char8");
	checkSuccess($fields["testdate"],"01-Jan-2008");
	checkSuccess($fields["testint"],"8");
	checkSuccess($fields["testmoney"],"8.00");
	checkSuccess($fields["testreal"],"8.1");
	checkSuccess($fields["testtext"],"text8");
	checkSuccess($fields["testtime"],"08:00:00");
	checkSuccess($fields["testuint"],"8");
	echo("\n");

	echo("FIELD LENGTHS BY ASSOCIATIVE ARRAY: \n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,0);
	checkSuccess($fieldlengths["testchar"],5);
	checkSuccess($fieldlengths["testdate"],11);
	checkSuccess($fieldlengths["testint"],1);
	checkSuccess($fieldlengths["testmoney"],4);
	checkSuccess($fieldlengths["testreal"],3);
	checkSuccess($fieldlengths["testtext"],5);
	checkSuccess($fieldlengths["testtime"],8);
	checkSuccess($fieldlengths["testuint"],1);
	echo("\n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,7);
	checkSuccess($fieldlengths["testchar"],5);
	checkSuccess($fieldlengths["testdate"],11);
	checkSuccess($fieldlengths["testint"],1);
	checkSuccess($fieldlengths["testmoney"],4);
	checkSuccess($fieldlengths["testreal"],3);
	checkSuccess($fieldlengths["testtext"],5);
	checkSuccess($fieldlengths["testtime"],8);
	checkSuccess($fieldlengths["testuint"],1);
	echo("\n");


	echo("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_sendQuery($cur,"drop table testtable1");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable1 (col1 int, col2 char(40), col3 real)"),1);
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
	echo("\n");


	echo("NULLS as Nulls: \n");
	sqlrcur_sendQuery($cur,"drop table testtable1");
	sqlrcur_sendQuery($cur,"create table testtable1 (col1 char(40), col2 char(40), col3 char(40))");
	sqlrcur_getNullsAsUndefined($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable1 values ('1',NULL,NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),NULL);
	checkSuccess(sqlrcur_getField($cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"");
	checkSuccess(sqlrcur_getField($cur,0,2),"");
	checkSuccess(sqlrcur_sendQuery($cur,"drop table testtable1"),1);
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
	checkSuccess(sqlrcur_getField($cur,0,0),"char1");
	checkSuccess(sqlrcur_getField($cur,1,0),"char2");
	checkSuccess(sqlrcur_getField($cur,2,0),"char3");
	echo("\n");
	checkSuccess(sqlrcur_firstRowIndex($cur),2);
	checkSuccess(sqlrcur_endOfResultSet($cur),0);
	checkSuccess(sqlrcur_rowCount($cur),4);
	checkSuccess(sqlrcur_getField($cur,6,0),"char7");
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
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
	checkSuccess(sqlrcur_getColumnName($cur,0),"testchar");
	checkSuccess(sqlrcur_getColumnLength($cur,0),40);
	checkSuccess(sqlrcur_getColumnType($cur,0),"CHAR");
	echo("\n");

	echo("SUSPENDED SESSION: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet($cur);
	checkSuccess(sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess(sqlrcon_resumeSession($con,$conport,$consocket),1);
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,0,0),"char1");
	checkSuccess(sqlrcur_getField($cur,1,0),"char2");
	checkSuccess(sqlrcur_getField($cur,2,0),"char3");
	checkSuccess(sqlrcur_getField($cur,3,0),"char4");
	checkSuccess(sqlrcur_getField($cur,4,0),"char5");
	checkSuccess(sqlrcur_getField($cur,5,0),"char6");
	checkSuccess(sqlrcur_getField($cur,6,0),"char7");
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
	echo("\n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet($cur);
	checkSuccess(sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess(sqlrcon_resumeSession($con,$conport,$consocket),1);
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,0,0),"char1");
	checkSuccess(sqlrcur_getField($cur,1,0),"char2");
	checkSuccess(sqlrcur_getField($cur,2,0),"char3");
	checkSuccess(sqlrcur_getField($cur,3,0),"char4");
	checkSuccess(sqlrcur_getField($cur,4,0),"char5");
	checkSuccess(sqlrcur_getField($cur,5,0),"char6");
	checkSuccess(sqlrcur_getField($cur,6,0),"char7");
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
	echo("\n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	sqlrcur_suspendResultSet($cur);
	checkSuccess(sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess(sqlrcon_resumeSession($con,$conport,$consocket),1);
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,0,0),"char1");
	checkSuccess(sqlrcur_getField($cur,1,0),"char2");
	checkSuccess(sqlrcur_getField($cur,2,0),"char3");
	checkSuccess(sqlrcur_getField($cur,3,0),"char4");
	checkSuccess(sqlrcur_getField($cur,4,0),"char5");
	checkSuccess(sqlrcur_getField($cur,5,0),"char6");
	checkSuccess(sqlrcur_getField($cur,6,0),"char7");
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
	echo("\n");

	echo("SUSPENDED RESULT SET: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getField($cur,2,0),"char3");
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
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
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
	sqlrcur_cacheToFile($cur,"/tmp/cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"/tmp/cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
	echo("\n");

	echo("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_colCount($cur),8);
	echo("\n");

	echo("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"testchar");
	checkSuccess(sqlrcur_getColumnName($cur,1),"testdate");
	checkSuccess(sqlrcur_getColumnName($cur,2),"testint");
	checkSuccess(sqlrcur_getColumnName($cur,3),"testmoney");
	checkSuccess(sqlrcur_getColumnName($cur,4),"testreal");
	checkSuccess(sqlrcur_getColumnName($cur,5),"testtext");
	checkSuccess(sqlrcur_getColumnName($cur,6),"testtime");
	checkSuccess(sqlrcur_getColumnName($cur,7),"testuint");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"testchar");
	checkSuccess($cols[1],"testdate");
	checkSuccess($cols[2],"testint");
	checkSuccess($cols[3],"testmoney");
	checkSuccess($cols[4],"testreal");
	checkSuccess($cols[5],"testtext");
	checkSuccess($cols[6],"testtime");
	checkSuccess($cols[7],"testuint");
	echo("\n");

	echo("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"/tmp/cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"/tmp/cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("FROM ONE CACHE FILE TO ANOTHER: \n");
	sqlrcur_cacheToFile($cur,"/tmp/cachefile2");
	checkSuccess(sqlrcur_openCachedResultSet($cur,"/tmp/cachefile1"),1);
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,"/tmp/cachefile2"),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	echo("\n");

	echo("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"/tmp/cachefile2");
	checkSuccess(sqlrcur_openCachedResultSet($cur,"/tmp/cachefile1"),1);
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,"/tmp/cachefile2"),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"/tmp/cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getField($cur,2,0),"char3");
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"/tmp/cachefile1");
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
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
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
	checkSuccess(sqlrcur_getField($cur,7,0),"char8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("COMMIT AND ROLLBACK: \n");
	$secondcon=sqlrcon_alloc($host,$port, 
					$socket,$user,$password,0,1);
	$secondcur=sqlrcur_alloc($secondcon);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"char1");
	checkSuccess(sqlrcon_commit($con),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"char1");
	checkSuccess(sqlrcon_autoCommitOn($con),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values ('char10','01-Jan-2010',10,10.00,10.1,'text10','10:00:00',10)"),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select * from testtable order by testint"),1);
	checkSuccess(sqlrcur_getField($secondcur,8,0),"char10");
	checkSuccess(sqlrcon_autoCommitOff($con),1);
	echo("\n");

	# drop existing table
	sqlrcur_sendQuery($cur,"drop table testtable");

	# invalid queries...
	echo("INVALID QUERIES: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),0);
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
