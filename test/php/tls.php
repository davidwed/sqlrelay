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
	$user=null;
	$password=null;

	$cert="/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem";
	$ca="/usr/local/firstworks/etc/sqlrelay.conf.d/ca.pem";
	if (strtoupper(substr(PHP_OS,0,3))==='WIN') {
		$cert="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\client.pfx";
		$ca="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\ca.pfx";
	}

	# instantiation
	$con=sqlrcon_alloc($host,$port,$socket,$user,$password,0,1);
	$cur=sqlrcur_alloc($con);
	sqlrcon_enableTls($con,null,$cert,null,null,"ca",$ca,0);

	# get database type
	echo("IDENTIFY: \n");
	checkSuccess(sqlrcon_identify($con),"oracle");
	echo("\n");

	# ping
	echo("PING: \n");
	checkSuccess(sqlrcon_ping($con),1);
	echo("\n");

	# drop existing table
	sqlrcur_sendQuery($cur,"drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long, testclob clob, testblob blob)"),1);
	echo("\n");

	echo("INSERT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1','testclob1',empty_blob())"),1);
	echo("\n");

	echo("AFFECTED ROWS: \n");
	checkSuccess(sqlrcur_affectedRows($cur),1);
	echo("\n");

	echo("BIND BY POSITION: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
	checkSuccess(sqlrcur_countBindVariables($cur),7);
	sqlrcur_inputBind($cur,"1",2);
	sqlrcur_inputBind($cur,"2","testchar2");
	sqlrcur_inputBind($cur,"3","testvarchar2");
	sqlrcur_inputBind($cur,"4","01-JAN-2002");
	sqlrcur_inputBind($cur,"5","testlong2");
	sqlrcur_inputBindClob($cur,"6","testclob2",9);
	sqlrcur_inputBindBlob($cur,"7","testblob2",9);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"1",3);
	sqlrcur_inputBind($cur,"2","testchar3");
	sqlrcur_inputBind($cur,"3","testvarchar3");
	sqlrcur_inputBind($cur,"4","01-JAN-2003");
	sqlrcur_inputBind($cur,"5","testlong3");
	sqlrcur_inputBindClob($cur,"6","testclob3",9);
	sqlrcur_inputBindBlob($cur,"7","testblob3",9);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY OF BINDS BY POSITION: \n");
	sqlrcur_clearBinds($cur);
	$bindvars=array("1","2","3","4","5");
	$bindvals=array("4","testchar4","testvarchar4","01-JAN-2004","testlong4");
	sqlrcur_inputBinds($cur,$bindvars,$bindvals);
	sqlrcur_inputBindClob($cur,"6","testclob4",9);
	sqlrcur_inputBindBlob($cur,"7","testblob4",9);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("BIND BY NAME: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
	sqlrcur_inputBind($cur,"var1",5);
	sqlrcur_inputBind($cur,"var2","testchar5");
	sqlrcur_inputBind($cur,"var3","testvarchar5");
	sqlrcur_inputBind($cur,"var4","01-JAN-2005");
	sqlrcur_inputBind($cur,"var5","testlong5");
	sqlrcur_inputBindClob($cur,"6","testclob5",9);
	sqlrcur_inputBindBlob($cur,"7","testblob5",9);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"var1",6);
	sqlrcur_inputBind($cur,"var2","testchar6");
	sqlrcur_inputBind($cur,"var3","testvarchar6");
	sqlrcur_inputBind($cur,"var4","01-JAN-2006");
	sqlrcur_inputBind($cur,"var5","testlong6");
	sqlrcur_inputBindClob($cur,"6","testclob6",9);
	sqlrcur_inputBindBlob($cur,"7","testblob6",9);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY OF BINDS BY NAME: \n");
	sqlrcur_clearBinds($cur);
	$arraybindvars=array("var1","var2","var3","var4","var5");
	$arraybindvals=array("7","testchar7","testvarchar7","01-JAN-2007","testlong7");
	sqlrcur_inputBinds($cur,$arraybindvars,$arraybindvals);
	sqlrcur_inputBindClob($cur,"var6","testclob7",9);
	sqlrcur_inputBindBlob($cur,"var7","testblob7",9);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"var1",8);
	sqlrcur_inputBind($cur,"var2","testchar8");
	sqlrcur_inputBind($cur,"var3","testvarchar8");
	sqlrcur_inputBind($cur,"var4","01-JAN-2008");
	sqlrcur_inputBind($cur,"var5","testlong8");
	sqlrcur_inputBindClob($cur,"var6","testclob8",9);
	sqlrcur_inputBindBlob($cur,"var7","testblob8",9);
	sqlrcur_inputBind($cur,"var9","junkvalue");
	sqlrcur_validateBinds($cur);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("OUTPUT BIND BY NAME: \n");
	sqlrcur_prepareQuery($cur,"begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;");
	sqlrcur_defineOutputBindInteger($cur,"numvar");
	sqlrcur_defineOutputBindString($cur,"stringvar",10);
	sqlrcur_defineOutputBindDouble($cur,"floatvar");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$numvar=sqlrcur_getOutputBindInteger($cur,"numvar");
	$stringvar=sqlrcur_getOutputBindString($cur,"stringvar");
	$floatvar=sqlrcur_getOutputBindDouble($cur,"floatvar");
	checkSuccess($numvar,1);
	checkSuccess($stringvar,"hello");
	checkSuccess($floatvar,2.5);
	echo("\n");

	echo("OUTPUT BIND BY NAME: \n");
	sqlrcur_clearBinds($cur);
	sqlrcur_defineOutputBindInteger($cur,"1");
	sqlrcur_defineOutputBindString($cur,"2",10);
	sqlrcur_defineOutputBindDouble($cur,"3");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$numvar=sqlrcur_getOutputBindInteger($cur,"1");
	$stringvar=sqlrcur_getOutputBindString($cur,"2");
	$floatvar=sqlrcur_getOutputBindDouble($cur,"3");
	checkSuccess($numvar,1);
	checkSuccess($stringvar,"hello");
	checkSuccess($floatvar,2.5);
	echo("\n");

	echo("OUTPUT BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds($cur);
	sqlrcur_defineOutputBindInteger($cur,"numvar");
	sqlrcur_defineOutputBindString($cur,"stringvar",10);
	sqlrcur_defineOutputBindDouble($cur,"floatvar");
	sqlrcur_defineOutputBindString($cur,"dummyvar",10);
	sqlrcur_validateBinds($cur);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$numvar=sqlrcur_getOutputBindInteger($cur,"numvar");
	$stringvar=sqlrcur_getOutputBindString($cur,"stringvar");
	$floatvar=sqlrcur_getOutputBindDouble($cur,"floatvar");
	checkSuccess($numvar,1);
	checkSuccess($stringvar,"hello");
	checkSuccess($floatvar,2.5);
	echo("\n");

	echo("SELECT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	echo("\n");

	echo("COLUMN COUNT: \n");
	checkSuccess(sqlrcur_colCount($cur),7);
	echo("\n");

	echo("COLUMN NAMES: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"TESTNUMBER");
	checkSuccess(sqlrcur_getColumnName($cur,1),"TESTCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,2),"TESTVARCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,3),"TESTDATE");
	checkSuccess(sqlrcur_getColumnName($cur,4),"TESTLONG");
	checkSuccess(sqlrcur_getColumnName($cur,5),"TESTCLOB");
	checkSuccess(sqlrcur_getColumnName($cur,6),"TESTBLOB");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"TESTNUMBER");
	checkSuccess($cols[1],"TESTCHAR");
	checkSuccess($cols[2],"TESTVARCHAR");
	checkSuccess($cols[3],"TESTDATE");
	checkSuccess($cols[4],"TESTLONG");
	checkSuccess($cols[5],"TESTCLOB");
	checkSuccess($cols[6],"TESTBLOB");
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess(sqlrcur_getColumnType($cur,0),"NUMBER");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTNUMBER"),"NUMBER");
	checkSuccess(sqlrcur_getColumnType($cur,1),"CHAR");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTCHAR"),"CHAR");
	checkSuccess(sqlrcur_getColumnType($cur,2),"VARCHAR2");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTVARCHAR"),"VARCHAR2");
	checkSuccess(sqlrcur_getColumnType($cur,3),"DATE");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTDATE"),"DATE");
	checkSuccess(sqlrcur_getColumnType($cur,4),"LONG");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTLONG"),"LONG");
	checkSuccess(sqlrcur_getColumnType($cur,5),"CLOB");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTCLOB"),"CLOB");
	checkSuccess(sqlrcur_getColumnType($cur,6),"BLOB");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTBLOB"),"BLOB");
	echo("\n");

	echo("COLUMN LENGTH: \n");
	checkSuccess(sqlrcur_getColumnLength($cur,0),22);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTNUMBER"),22);
	checkSuccess(sqlrcur_getColumnLength($cur,1),40);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTCHAR"),40);
	checkSuccess(sqlrcur_getColumnLength($cur,2),40);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTVARCHAR"),40);
	checkSuccess(sqlrcur_getColumnLength($cur,3),7);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTDATE"),7);
	checkSuccess(sqlrcur_getColumnLength($cur,4),0);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTLONG"),0);
	checkSuccess(sqlrcur_getColumnLength($cur,5),0);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTCLOB"),0);
	checkSuccess(sqlrcur_getColumnLength($cur,6),0);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTBLOB"),0);
	echo("\n");

	echo("LONGEST COLUMN: \n");
	checkSuccess(sqlrcur_getLongest($cur,0),1);
	checkSuccess(sqlrcur_getLongest($cur,"TESTNUMBER"),1);
	checkSuccess(sqlrcur_getLongest($cur,1),40);
	checkSuccess(sqlrcur_getLongest($cur,"TESTCHAR"),40);
	checkSuccess(sqlrcur_getLongest($cur,2),12);
	checkSuccess(sqlrcur_getLongest($cur,"TESTVARCHAR"),12);
	checkSuccess(sqlrcur_getLongest($cur,3),9);
	checkSuccess(sqlrcur_getLongest($cur,"TESTDATE"),9);
	checkSuccess(sqlrcur_getLongest($cur,4),9);
	checkSuccess(sqlrcur_getLongest($cur,"TESTLONG"),9);
	checkSuccess(sqlrcur_getLongest($cur,5),9);
	checkSuccess(sqlrcur_getLongest($cur,"TESTCLOB"),9);
	checkSuccess(sqlrcur_getLongest($cur,6),9);
	checkSuccess(sqlrcur_getLongest($cur,"TESTBLOB"),9);
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
	checkSuccess(sqlrcur_getField($cur,0,5),"testclob1");
	checkSuccess(sqlrcur_getField($cur,0,6),"");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,7,1),"testchar8                               ");
	checkSuccess(sqlrcur_getField($cur,7,2),"testvarchar8");
	checkSuccess(sqlrcur_getField($cur,7,3),"01-JAN-08");
	checkSuccess(sqlrcur_getField($cur,7,4),"testlong8");
	checkSuccess(sqlrcur_getField($cur,7,5),"testclob8");
	checkSuccess(sqlrcur_getField($cur,7,6),"testblob8");
	echo("\n");

	echo("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,1),40);
	checkSuccess(sqlrcur_getFieldLength($cur,0,2),12);
	checkSuccess(sqlrcur_getFieldLength($cur,0,3),9);
	checkSuccess(sqlrcur_getFieldLength($cur,0,4),9);
	checkSuccess(sqlrcur_getFieldLength($cur,0,5),9);
	checkSuccess(sqlrcur_getFieldLength($cur,0,6),0);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,1),40);
	checkSuccess(sqlrcur_getFieldLength($cur,7,2),12);
	checkSuccess(sqlrcur_getFieldLength($cur,7,3),9);
	checkSuccess(sqlrcur_getFieldLength($cur,7,4),9);
	checkSuccess(sqlrcur_getFieldLength($cur,7,5),9);
	checkSuccess(sqlrcur_getFieldLength($cur,7,6),9);
	echo("\n");

	echo("FIELDS BY NAME: \n");
	checkSuccess(sqlrcur_getField($cur,0,"TESTNUMBER"),"1");
	checkSuccess(sqlrcur_getField($cur,0,"TESTCHAR"),"testchar1                               ");
	checkSuccess(sqlrcur_getField($cur,0,"TESTVARCHAR"),"testvarchar1");
	checkSuccess(sqlrcur_getField($cur,0,"TESTDATE"),"01-JAN-01");
	checkSuccess(sqlrcur_getField($cur,0,"TESTLONG"),"testlong1");
	checkSuccess(sqlrcur_getField($cur,0,"TESTCLOB"),"testclob1");
	checkSuccess(sqlrcur_getField($cur,0,"TESTBLOB"),"");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,"TESTNUMBER"),"8");
	checkSuccess(sqlrcur_getField($cur,7,"TESTCHAR"),"testchar8                               ");
	checkSuccess(sqlrcur_getField($cur,7,"TESTVARCHAR"),"testvarchar8");
	checkSuccess(sqlrcur_getField($cur,7,"TESTDATE"),"01-JAN-08");
	checkSuccess(sqlrcur_getField($cur,7,"TESTLONG"),"testlong8");
	checkSuccess(sqlrcur_getField($cur,7,"TESTCLOB"),"testclob8");
	checkSuccess(sqlrcur_getField($cur,7,"TESTBLOB"),"testblob8");
	echo("\n");

	echo("FIELD LENGTHS BY NAME: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTNUMBER"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTCHAR"),40);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTVARCHAR"),12);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTDATE"),9);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTLONG"),9);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTCLOB"),9);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTBLOB"),0);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTNUMBER"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTCHAR"),40);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTVARCHAR"),12);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTDATE"),9);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTLONG"),9);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTCLOB"),9);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTBLOB"),9);
	echo("\n");

	echo("FIELDS BY ARRAY: \n");
	$fields=sqlrcur_getRow($cur,0);
	checkSuccess($fields[0],"1");
	checkSuccess($fields[1],"testchar1                               ");
	checkSuccess($fields[2],"testvarchar1");
	checkSuccess($fields[3],"01-JAN-01");
	checkSuccess($fields[4],"testlong1");
	checkSuccess($fields[5],"testclob1");
	checkSuccess($fields[6],"");
	echo("\n");

	echo("FIELD LENGTHS BY ARRAY: \n");
	$fieldlens=sqlrcur_getRowLengths($cur,0);
	checkSuccess($fieldlens[0],1);
	checkSuccess($fieldlens[1],40);
	checkSuccess($fieldlens[2],12);
	checkSuccess($fieldlens[3],9);
	checkSuccess($fieldlens[4],9);
	checkSuccess($fieldlens[5],9);
	checkSuccess($fieldlens[6],0);
	echo("\n");

	echo("FIELDS BY ASSOCIATIVE ARRAY: \n");
	$fields=sqlrcur_getRowAssoc($cur,0);
	checkSuccess($fields["TESTNUMBER"],1);
	checkSuccess($fields["TESTCHAR"],"testchar1                               ");
	checkSuccess($fields["TESTVARCHAR"],"testvarchar1");
	checkSuccess($fields["TESTDATE"],"01-JAN-01");
	checkSuccess($fields["TESTLONG"],"testlong1");
	checkSuccess($fields["TESTCLOB"],"testclob1");
	checkSuccess($fields["TESTBLOB"],"");
	echo("\n");
	$fields=sqlrcur_getRowAssoc($cur,7);
	checkSuccess($fields["TESTNUMBER"],8);
	checkSuccess($fields["TESTCHAR"],"testchar8                               ");
	checkSuccess($fields["TESTVARCHAR"],"testvarchar8");
	checkSuccess($fields["TESTDATE"],"01-JAN-08");
	checkSuccess($fields["TESTLONG"],"testlong8");
	checkSuccess($fields["TESTCLOB"],"testclob8");
	checkSuccess($fields["TESTBLOB"],"testblob8");
	echo("\n");

	echo("FIELD LENGTHS BY ASSOCIATIVE ARRAY: \n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,0);
	checkSuccess($fieldlengths["TESTNUMBER"],1);
	checkSuccess($fieldlengths["TESTCHAR"],40);
	checkSuccess($fieldlengths["TESTVARCHAR"],12);
	checkSuccess($fieldlengths["TESTDATE"],9);
	checkSuccess($fieldlengths["TESTLONG"],9);
	checkSuccess($fieldlengths["TESTCLOB"],9);
	checkSuccess($fieldlengths["TESTBLOB"],0);
	echo("\n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,7);
	checkSuccess($fieldlengths["TESTNUMBER"],1);
	checkSuccess($fieldlengths["TESTCHAR"],40);
	checkSuccess($fieldlengths["TESTVARCHAR"],12);
	checkSuccess($fieldlengths["TESTDATE"],9);
	checkSuccess($fieldlengths["TESTLONG"],9);
	checkSuccess($fieldlengths["TESTCLOB"],9);
	checkSuccess($fieldlengths["TESTBLOB"],9);
	echo("\n");

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
	sqlrcur_defineOutputBindString($cur,"var1",10);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	checkSuccess(sqlrcur_getOutputBindString($cur,"var1"),"hello");
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
	sqlrcur_cacheToFile($cur,"cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");

	echo("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_colCount($cur),7);
	echo("\n");

	echo("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"TESTNUMBER");
	checkSuccess(sqlrcur_getColumnName($cur,1),"TESTCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,2),"TESTVARCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,3),"TESTDATE");
	checkSuccess(sqlrcur_getColumnName($cur,4),"TESTLONG");
	checkSuccess(sqlrcur_getColumnName($cur,5),"TESTCLOB");
	checkSuccess(sqlrcur_getColumnName($cur,6),"TESTBLOB");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"TESTNUMBER");
	checkSuccess($cols[1],"TESTCHAR");
	checkSuccess($cols[2],"TESTVARCHAR");
	checkSuccess($cols[3],"TESTDATE");
	checkSuccess($cols[4],"TESTLONG");
	checkSuccess($cols[5],"TESTCLOB");
	checkSuccess($cols[6],"TESTBLOB");
	echo("\n");

	echo("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
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
	sqlrcon_enableTls($secondcon,null,$cert,null,null,"ca",$ca,0);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"0");
	checkSuccess(sqlrcon_commit($con),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"8");
	checkSuccess(sqlrcon_autoCommitOn($con),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10','testclob10',empty_blob())"),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"9");
	checkSuccess(sqlrcon_autoCommitOff($con),1);
	echo("\n");


	echo("CLOB AND BLOB OUTPUT BIND: \n");
	sqlrcur_sendQuery($cur,"drop table testtable1");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable1 (testclob clob, testblob blob)"),1);
	sqlrcur_prepareQuery($cur,"insert into testtable1 values ('hello',:var1)");
	sqlrcur_inputBindBlob($cur,"var1","hello",5);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_prepareQuery($cur,"begin select testclob into :clobvar from testtable1;  select testblob into :blobvar from testtable1; end;");
	sqlrcur_defineOutputBindClob($cur,"clobvar");
	sqlrcur_defineOutputBindBlob($cur,"blobvar");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$clobvar=sqlrcur_getOutputBindClob($cur,"clobvar");
	$clobvarlength=sqlrcur_getOutputBindLength($cur,"clobvar");
	$blobvar=sqlrcur_getOutputBindBlob($cur,"blobvar");
	$blobvarlength=sqlrcur_getOutputBindLength($cur,"blobvar");
	checkSuccess($clobvar,"hello");
	checkSuccess($clobvarlength,5);
	checkSuccess($blobvar,"hello",5);
	checkSuccess($blobvarlength,5);
	sqlrcur_sendQuery($cur,"drop table testtable1");
	echo("\n");

	echo("NULL AND EMPTY CLOBS AND CLOBS: \n");
	sqlrcur_getNullsAsNulls($cur);
	sqlrcur_sendQuery($cur,"create table testtable1 (testclob1 clob, testclob2 clob, testblob1 blob, testblob2 blob)");
	sqlrcur_prepareQuery($cur,"insert into testtable1 values (:var1,:var2,:var3,:var4)");
	sqlrcur_inputBindClob($cur,"var1","",0);
	sqlrcur_inputBindClob($cur,"var2",NULL,0);
	sqlrcur_inputBindBlob($cur,"var3","",0);
	sqlrcur_inputBindBlob($cur,"var4",NULL,0);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_sendQuery($cur,"select * from testtable1");
	checkSuccess(sqlrcur_getField($cur,0,0),NULL);
	checkSuccess(sqlrcur_getField($cur,0,1),NULL);
	checkSuccess(sqlrcur_getField($cur,0,2),NULL);
	checkSuccess(sqlrcur_getField($cur,0,3),NULL);
	sqlrcur_sendQuery($cur,"drop table testtable1");
	echo("\n");

	echo("CURSOR BINDS: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"create or replace package types as type cursorType is ref cursor; end;"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"create or replace function sp_testtable return types.cursortype as l_cursor    types.cursorType; begin open l_cursor for select * from testtable; return l_cursor; end;"),1);
	sqlrcur_prepareQuery($cur,"begin  :curs:=sp_testtable; end;");
	sqlrcur_defineOutputBindCursor($cur,"curs");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$bindcur=sqlrcur_getOutputBindCursor($cur,"curs");
	checkSuccess(sqlrcur_fetchFromBindCursor($bindcur),1);
	checkSuccess(sqlrcur_getField($bindcur,0,0),"1");
	checkSuccess(sqlrcur_getField($bindcur,1,0),"2");
	checkSuccess(sqlrcur_getField($bindcur,2,0),"3");
	checkSuccess(sqlrcur_getField($bindcur,3,0),"4");
	checkSuccess(sqlrcur_getField($bindcur,4,0),"5");
	checkSuccess(sqlrcur_getField($bindcur,5,0),"6");
	checkSuccess(sqlrcur_getField($bindcur,6,0),"7");
	checkSuccess(sqlrcur_getField($bindcur,7,0),"8");
	sqlrcur_free($bindcur);
	echo("\n");

	echo("LONG CLOB: \n");
	sqlrcur_sendQuery($cur,"drop table testtable2");
	sqlrcur_sendQuery($cur,"create table testtable2 (testclob clob)");
	sqlrcur_prepareQuery($cur,"insert into testtable2 values (:clobval)");
	$clobval="";
	for ($i=0; $i<8*1024; $i++) {
		$clobval=$clobval.'C';
	}
	sqlrcur_inputBindClob($cur,"clobval",$clobval,8*1024);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_sendQuery($cur,"select testclob from testtable2");
	checkSuccess($clobval,sqlrcur_getField($cur,0,"TESTCLOB"));
	sqlrcur_prepareQuery($cur,"begin select testclob into :clobbindval from testtable2; end;");
	sqlrcur_defineOutputBindClob($cur,"clobbindval");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	$clobbindvar=sqlrcur_getOutputBindClob($cur,"clobbindval");
	checkSuccess(sqlrcur_getOutputBindLength($cur,"clobbindval"),8*1024);
	checkSuccess($clobval,$clobbindvar);
	sqlrcur_sendQuery($cur,"drop table testtable2");
	echo("\n");


	echo("LONG OUTPUT BIND\n");
	sqlrcur_sendQuery($cur,"drop table testtable2");
	sqlrcur_sendQuery($cur,"create table testtable2 (testval varchar2(4000))");
	sqlrcur_prepareQuery($cur,"insert into testtable2 values (:testval)");
	$testval="";
	for ($i=0; $i<4000; $i++) {
		$testval=$testval."C";
	}
	sqlrcur_inputBind($cur,"testval",$testval);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_sendQuery($cur,"select testval from testtable2");
	checkSuccess($testval,sqlrcur_getField($cur,0,"TESTVAL"));
	$query="begin :bindval:='".$testval."'; end;";
	sqlrcur_prepareQuery($cur,$query);
	sqlrcur_defineOutputBindString($cur,"bindval",4000);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	checkSuccess(sqlrcur_getOutputBindLength($cur,"bindval"),4000);
	checkSuccess(sqlrcur_getOutputBindString($cur,"bindval"),$testval);
	sqlrcur_sendQuery($cur,"drop table testtable2");
	echo("\n");

	echo("NEGATIVE INPUT BIND\n");
	sqlrcur_sendQuery($cur,"create table testtable2 (testval number)");
	sqlrcur_prepareQuery($cur,"insert into testtable2 values (:testval)");
	sqlrcur_inputBind($cur,"testval",-1);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_sendQuery($cur,"select testval from testtable2");
	checkSuccess(sqlrcur_getField($cur,0,"TESTVAL"),"-1");
	sqlrcur_sendQuery($cur,"drop table testtable2");
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
