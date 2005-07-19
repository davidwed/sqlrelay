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

	$host=$_REQUEST["host"];
	$port=$_REQUEST["port"];
	$socket=$_REQUEST["socket"];
	$user=$_REQUEST["user"];
	$password=$_REQUEST["password"];

	# instantiation
	$con=sqlrcon_alloc($host,$port,$socket,$user,$password,0,1);
	$cur=sqlrcur_alloc($con);

	# get database type
	echo("IDENTIFY: \n");
	checkSuccess(sqlrcon_identify($con),"oracle8");
	echo("\n");

	# ping
	echo("PING: \n");
	checkSuccess(sqlrcon_ping($con),1);
	echo("\n");

	# drop existing table
	sqlrcur_sendQuery($cur,"drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long)"),1);
	echo("\n");

	echo("INSERT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1')"),1);
	echo("\n");

	echo("AFFECTED ROWS: \n");
	checkSuccess(sqlrcur_affectedRows($cur),1);
	echo("\n");

	echo("BIND BY POSITION: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (:1,:2,:3,:4,:5)");
	checkSuccess(sqlrcur_countBindVariables($cur),5);
	sqlrcur_inputBind($cur,"1",2);
	sqlrcur_inputBind($cur,"2","testchar2");
	sqlrcur_inputBind($cur,"3","testvarchar2");
	sqlrcur_inputBind($cur,"4","01-JAN-2002");
	sqlrcur_inputBind($cur,"5","testlong2");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"1",3);
	sqlrcur_inputBind($cur,"2","testchar3");
	sqlrcur_inputBind($cur,"3","testvarchar3");
	sqlrcur_inputBind($cur,"4","01-JAN-2003");
	sqlrcur_inputBind($cur,"5","testlong3");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY OF BINDS BY POSITION: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (:1,:2,:3,:4,:5)");
	$bindvars=array("1","2","3","4","5");
	$bindvals=array("4","testchar4","testvarchar4","01-JAN-2004","testlong4");
	sqlrcur_inputBinds($cur,$bindvars,$bindvals);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("BIND BY NAME: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5)");
	sqlrcur_inputBind($cur,"var1",5);
	sqlrcur_inputBind($cur,"var2","testchar5");
	sqlrcur_inputBind($cur,"var3","testvarchar5");
	sqlrcur_inputBind($cur,"var4","01-JAN-2005");
	sqlrcur_inputBind($cur,"var5","testlong5");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"var1",6);
	sqlrcur_inputBind($cur,"var2","testchar6");
	sqlrcur_inputBind($cur,"var3","testvarchar6");
	sqlrcur_inputBind($cur,"var4","01-JAN-2006");
	sqlrcur_inputBind($cur,"var5","testlong6");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY OF BINDS BY NAME: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5)");
	$arraybindvars=array("var1","var2","var3","var4","var5");
	$arraybindvals=array("7","testchar7","testvarchar7","01-JAN-2007","testlong7");
	sqlrcur_inputBinds($cur,$arraybindvars,$arraybindvals);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5)");
	sqlrcur_inputBind($cur,"var1",8);
	sqlrcur_inputBind($cur,"var2","testchar8");
	sqlrcur_inputBind($cur,"var3","testvarchar8");
	sqlrcur_inputBind($cur,"var4","01-JAN-2008");
	sqlrcur_inputBind($cur,"var5","testlong8");
	sqlrcur_inputBind($cur,"var6","junkvalue");
	sqlrcur_validateBinds($cur);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("OUTPUT BIND BY NAME: \n");
	sqlrcur_prepareQuery($cur,"begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;");
	sqlrcur_defineOutputBind($cur,"numvar",10);
	sqlrcur_defineOutputBind($cur,"stringvar",10);
	sqlrcur_defineOutputBind($cur,"floatvar",10);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$numvar=sqlrcur_getOutputBind($cur,"numvar");
	$stringvar=sqlrcur_getOutputBind($cur,"stringvar");
	$floatvar=sqlrcur_getOutputBind($cur,"floatvar");
	checkSuccess($numvar,"1");
	checkSuccess($stringvar,"hello");
	checkSuccess($floatvar,"2.5");
	echo("\n");

	echo("OUTPUT BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds($cur);
	sqlrcur_defineOutputBind($cur,"numvar",10);
	sqlrcur_defineOutputBind($cur,"stringvar",10);
	sqlrcur_defineOutputBind($cur,"floatvar",10);
	sqlrcur_defineOutputBind($cur,"dummyvar",10);
	sqlrcur_validateBinds($cur);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$numvar=sqlrcur_getOutputBind($cur,"numvar");
	$stringvar=sqlrcur_getOutputBind($cur,"stringvar");
	$floatvar=sqlrcur_getOutputBind($cur,"floatvar");
	checkSuccess($numvar,"1");
	checkSuccess($stringvar,"hello");
	checkSuccess($floatvar,"2.5");
	echo("\n");

	echo("OUTPUT BIND BY POSITION: \n");
	sqlrcur_prepareQuery($cur,"begin  :1:=1; :2:='hello'; :3:=2.5; end;");
	sqlrcur_defineOutputBind($cur,"1",10);
	sqlrcur_defineOutputBind($cur,"2",10);
	sqlrcur_defineOutputBind($cur,"3",10);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$numvar=sqlrcur_getOutputBind($cur,"1");
	$stringvar=sqlrcur_getOutputBind($cur,"2");
	$floatvar=sqlrcur_getOutputBind($cur,"3");
	checkSuccess($numvar,"1");
	checkSuccess($stringvar,"hello");
	checkSuccess($floatvar,"2.5");
	echo("\n");

	echo("SELECT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	echo("\n");

	echo("COLUMN COUNT: \n");
	checkSuccess(sqlrcur_colCount($cur),5);
	echo("\n");

	echo("COLUMN NAMES: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"TESTNUMBER");
	checkSuccess(sqlrcur_getColumnName($cur,1),"TESTCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,2),"TESTVARCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,3),"TESTDATE");
	checkSuccess(sqlrcur_getColumnName($cur,4),"TESTLONG");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"TESTNUMBER");
	checkSuccess($cols[1],"TESTCHAR");
	checkSuccess($cols[2],"TESTVARCHAR");
	checkSuccess($cols[3],"TESTDATE");
	checkSuccess($cols[4],"TESTLONG");
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess(sqlrcur_getColumnType($cur,0),"NUMBER");
	checkSuccess(sqlrcur_getColumnType($cur,"testnumber"),"NUMBER");
	checkSuccess(sqlrcur_getColumnType($cur,1),"CHAR");
	checkSuccess(sqlrcur_getColumnType($cur,"testchar"),"CHAR");
	checkSuccess(sqlrcur_getColumnType($cur,2),"VARCHAR2");
	checkSuccess(sqlrcur_getColumnType($cur,"testvarchar"),"VARCHAR2");
	checkSuccess(sqlrcur_getColumnType($cur,3),"DATE");
	checkSuccess(sqlrcur_getColumnType($cur,"testdate"),"DATE");
	checkSuccess(sqlrcur_getColumnType($cur,4),"LONG");
	checkSuccess(sqlrcur_getColumnType($cur,"testlong"),"LONG");
	echo("\n");

	echo("COLUMN LENGTH: \n");
	checkSuccess(sqlrcur_getColumnLength($cur,0),22);
	checkSuccess(sqlrcur_getColumnLength($cur,"testnumber"),22);
	checkSuccess(sqlrcur_getColumnLength($cur,1),40);
	checkSuccess(sqlrcur_getColumnLength($cur,"testchar"),40);
	checkSuccess(sqlrcur_getColumnLength($cur,2),40);
	checkSuccess(sqlrcur_getColumnLength($cur,"testvarchar"),40);
	checkSuccess(sqlrcur_getColumnLength($cur,3),7);
	checkSuccess(sqlrcur_getColumnLength($cur,"testdate"),7);
	checkSuccess(sqlrcur_getColumnLength($cur,4),0);
	checkSuccess(sqlrcur_getColumnLength($cur,"testlong"),0);
	echo("\n");

	echo("LONGEST COLUMN: \n");
	checkSuccess(sqlrcur_getLongest($cur,0),1);
	checkSuccess(sqlrcur_getLongest($cur,"testnumber"),1);
	checkSuccess(sqlrcur_getLongest($cur,1),40);
	checkSuccess(sqlrcur_getLongest($cur,"testchar"),40);
	checkSuccess(sqlrcur_getLongest($cur,2),12);
	checkSuccess(sqlrcur_getLongest($cur,"testvarchar"),12);
	checkSuccess(sqlrcur_getLongest($cur,3),9);
	checkSuccess(sqlrcur_getLongest($cur,"testdate"),9);
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
	checkSuccess(sqlrcur_getField($cur,0,1),"testchar1                               ");
	checkSuccess(sqlrcur_getField($cur,0,2),"testvarchar1");
	checkSuccess(sqlrcur_getField($cur,0,3),"01-JAN-01");
	checkSuccess(sqlrcur_getField($cur,0,4),"testlong1");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,7,1),"testchar8                               ");
	checkSuccess(sqlrcur_getField($cur,7,2),"testvarchar8");
	checkSuccess(sqlrcur_getField($cur,7,3),"01-JAN-08");
	checkSuccess(sqlrcur_getField($cur,7,4),"testlong8");
	echo("\n");

	echo("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,1),40);
	checkSuccess(sqlrcur_getFieldLength($cur,0,2),12);
	checkSuccess(sqlrcur_getFieldLength($cur,0,3),9);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,1),40);
	checkSuccess(sqlrcur_getFieldLength($cur,7,2),12);
	checkSuccess(sqlrcur_getFieldLength($cur,7,3),9);
	echo("\n");

	echo("FIELDS BY NAME: \n");
	checkSuccess(sqlrcur_getField($cur,0,"testnumber"),"1");
	checkSuccess(sqlrcur_getField($cur,0,"testchar"),"testchar1                               ");
	checkSuccess(sqlrcur_getField($cur,0,"testvarchar"),"testvarchar1");
	checkSuccess(sqlrcur_getField($cur,0,"testdate"),"01-JAN-01");
	checkSuccess(sqlrcur_getField($cur,0,"testlong"),"testlong1");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,"testnumber"),"8");
	checkSuccess(sqlrcur_getField($cur,7,"testchar"),"testchar8                               ");
	checkSuccess(sqlrcur_getField($cur,7,"testvarchar"),"testvarchar8");
	checkSuccess(sqlrcur_getField($cur,7,"testdate"),"01-JAN-08");
	checkSuccess(sqlrcur_getField($cur,7,"testlong"),"testlong8");
	echo("\n");

	echo("FIELD LENGTHS BY NAME: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testnumber"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testchar"),40);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testvarchar"),12);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testdate"),9);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testnumber"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testchar"),40);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testvarchar"),12);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testdate"),9);
	echo("\n");

	echo("FIELDS BY ARRAY: \n");
	$fields=sqlrcur_getRow($cur,0);
	checkSuccess($fields[0],"1");
	checkSuccess($fields[1],"testchar1                               ");
	checkSuccess($fields[2],"testvarchar1");
	checkSuccess($fields[3],"01-JAN-01");
	checkSuccess($fields[4],"testlong1");
	echo("\n");

	echo("FIELD LENGTHS BY ARRAY: \n");
	$fieldlens=sqlrcur_getRowLengths($cur,0);
	checkSuccess($fieldlens[0],1);
	checkSuccess($fieldlens[1],40);
	checkSuccess($fieldlens[2],12);
	checkSuccess($fieldlens[3],9);
	echo("\n");

	echo("FIELDS BY ASSOCIATIVE ARRAY: \n");
	$fields=sqlrcur_getRowAssoc($cur,0);
	checkSuccess($fields["TESTNUMBER"],1);
	checkSuccess($fields["TESTCHAR"],"testchar1                               ");
	checkSuccess($fields["TESTVARCHAR"],"testvarchar1");
	checkSuccess($fields["TESTDATE"],"01-JAN-01");
	checkSuccess($fields["TESTLONG"],"testlong1");
	echo("\n");
	$fields=sqlrcur_getRowAssoc($cur,7);
	checkSuccess($fields["TESTNUMBER"],8);
	checkSuccess($fields["TESTCHAR"],"testchar8                               ");
	checkSuccess($fields["TESTVARCHAR"],"testvarchar8");
	checkSuccess($fields["TESTDATE"],"01-JAN-08");
	checkSuccess($fields["TESTLONG"],"testlong8");
	echo("\n");

	echo("FIELD LENGTHS BY ASSOCIATIVE ARRAY: \n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,0);
	checkSuccess($fieldlengths["TESTNUMBER"],1);
	checkSuccess($fieldlengths["TESTCHAR"],40);
	checkSuccess($fieldlengths["TESTVARCHAR"],12);
	checkSuccess($fieldlengths["TESTDATE"],9);
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,7);
	checkSuccess($fieldlengths["TESTNUMBER"],1);
	checkSuccess($fieldlengths["TESTCHAR"],40);
	checkSuccess($fieldlengths["TESTVARCHAR"],12);
	checkSuccess($fieldlengths["TESTDATE"],9);
	echo("\n");
	
	echo("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery($cur,"select $(var1),'$(var2)',$(var3) from dual");
	sqlrcur_substitution($cur,"var1",1);
	sqlrcur_substitution($cur,"var2","hello");
	sqlrcur_substitution($cur,"var3",10.5556,6,4);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("FIELDS: \n");

	echo("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery($cur,"select $(var1),'$(var2)',$(var3) from dual");
	sqlrcur_substitution($cur,"var1",1);
	sqlrcur_substitution($cur,"var2","hello");
	sqlrcur_substitution($cur,"var3",10.5556,6,4);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("FIELDS: \n");
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"hello");
	checkSuccess(sqlrcur_getField($cur,0,2),"10.5556");
	echo("\n");

	echo("OUTPUT BIND: \n");
	sqlrcur_prepareQuery($cur,"begin :var1:='hello'; end;");
	sqlrcur_defineOutputBind($cur,"var1",10);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	checkSuccess(sqlrcur_getOutputBind($cur,"var1"),"hello");
	echo("\n");

	echo("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery($cur,"select $(var1),'$(var2)',$(var3) from dual");
	$vars=array("var1","var2","var3");
	$vals=array(1,"hello",10.5556);
	$precs=array(0,0,6);
	$scales=array(0,0,4);
	sqlrcur_substitutions($cur,$vars,$vals,$precs,$scales);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("FIELDS: \n");
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"hello");
	checkSuccess(sqlrcur_getField($cur,0,2),"10.5556");
	echo("\n");

	echo("NULLS as Nulls: \n");
	sqlrcur_getNullsAsNulls($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select NULL,1,NULL from dual"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),NULL);
	checkSuccess(sqlrcur_getField($cur,0,1),"1");
	checkSuccess(sqlrcur_getField($cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select NULL,1,NULL from dual"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"");
	checkSuccess(sqlrcur_getField($cur,0,1),"1");
	checkSuccess(sqlrcur_getField($cur,0,2),"");
	sqlrcur_getNullsAsNulls($cur);
	echo("\n");

	echo("RESULT SET BUFFER SIZE: \n");
	checkSuccess(sqlrcur_getResultSetBufferSize($cur),0);
	sqlrcur_setResultSetBufferSize($cur,2);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	checkSuccess(sqlrcur_getColumnName($cur,0),NULL);
	checkSuccess(sqlrcur_getColumnLength($cur,0),0);
	checkSuccess(sqlrcur_getColumnType($cur,0),NULL);
	sqlrcur_getColumnInfo($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	checkSuccess(sqlrcur_getColumnName($cur,0),"TESTNUMBER");
	checkSuccess(sqlrcur_getColumnLength($cur,0),22);
	checkSuccess(sqlrcur_getColumnType($cur,0),"NUMBER");
	echo("\n");

	echo("SUSPENDED SESSION: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
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
	sqlrcur_cacheToFile($cur,"/tmp/cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"/tmp/cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");

	echo("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_colCount($cur),5);
	echo("\n");

	echo("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"TESTNUMBER");
	checkSuccess(sqlrcur_getColumnName($cur,1),"TESTCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,2),"TESTVARCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,3),"TESTDATE");
	checkSuccess(sqlrcur_getColumnName($cur,4),"TESTLONG");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"TESTNUMBER");
	checkSuccess($cols[1],"TESTCHAR");
	checkSuccess($cols[2],"TESTVARCHAR");
	checkSuccess($cols[3],"TESTDATE");
	checkSuccess($cols[4],"TESTLONG");
	echo("\n");

	echo("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"/tmp/cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"/tmp/cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("FROM ONE CACHE FILE TO ANOTHER: \n");
	sqlrcur_cacheToFile($cur,"/tmp/cachefile2");
	checkSuccess(sqlrcur_openCachedResultSet($cur,"/tmp/cachefile1"),1);
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,"/tmp/cachefile2"),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	echo("\n");

	echo("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"/tmp/cachefile2");
	checkSuccess(sqlrcur_openCachedResultSet($cur,"/tmp/cachefile1"),1);
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,"/tmp/cachefile2"),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"/tmp/cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	checkSuccess(sqlrcur_getField($cur,2,0),"3");
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
	checkSuccess(sqlrcon_autoCommitOn($con),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10')"),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"9");
	checkSuccess(sqlrcon_autoCommitOff($con),1);
	echo("\n");

	# drop existing table
	sqlrcur_sendQuery($cur,"drop table testtable");

	# invalid queries...
	echo("INVALID QUERIES: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),0);
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
