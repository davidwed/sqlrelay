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
	checkSuccess($db,$db->query("drop table testtable"),DB_OK);

	echo("CREATE TEMPTABLE: \n");
	checkSuccess($db,$db->query("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long)"),DB_OK);
	echo("\n");

	echo("INSERT: \n");
	checkSuccess($db,$db->query("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1')"),DB_OK);
	echo("\n");

	echo("AFFECTED ROWS: \n");
	//checkSuccess($db,$db->affectedRows(),1);
	echo("\n");

	echo("BIND BY POSITION: \n");
	$res=$db->prepare("insert into testtable values (:var1,:var2,:var3,:var4,:var5)");
	$bindvars=array("1" => 2,
			"2" => "testchar2",
			"3" => "testvarchar2",
			"4" => "01-JAN-2002",
			"5" => "testlong2");
	$res=$db->execute($bindvars);

	$db->disconnect();
/*
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"1",3);
	sqlrcur_inputBind($cur,"2","testchar3");
	sqlrcur_inputBind($cur,"3","testvarchar3");
	sqlrcur_inputBind($cur,"4","01-JAN-2003");
	sqlrcur_inputBind($cur,"5","testlong3");
	sqlrcur_inputBindClob($cur,"6","testclob3",9);
	sqlrcur_inputBindBlob($cur,"7","testblob3",9);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY OF BINDS BY POSITION: \n");
	sqlrcur_clearBinds($cur);
	$bindvars=array("1","2","3","4","5");
	$bindvals=array("4","testchar4","testvarchar4","01-JAN-2004","testlong4");
	sqlrcur_inputBinds($cur,$bindvars,$bindvals);
	sqlrcur_inputBindClob($cur,"6","testclob4",9);
	sqlrcur_inputBindBlob($cur,"7","testblob4",9);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
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
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"var1",6);
	sqlrcur_inputBind($cur,"var2","testchar6");
	sqlrcur_inputBind($cur,"var3","testvarchar6");
	sqlrcur_inputBind($cur,"var4","01-JAN-2006");
	sqlrcur_inputBind($cur,"var5","testlong6");
	sqlrcur_inputBindClob($cur,"6","testclob6",9);
	sqlrcur_inputBindBlob($cur,"7","testblob6",9);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY OF BINDS BY NAME: \n");
	sqlrcur_clearBinds($cur);
	$arraybindvars=array("var1","var2","var3","var4","var5");
	$arraybindvals=array("7","testchar7","testvarchar7","01-JAN-2007","testlong7");
	sqlrcur_inputBinds($cur,$arraybindvars,$arraybindvals);
	sqlrcur_inputBindClob($cur,"var6","testclob7",9);
	sqlrcur_inputBindBlob($cur,"var7","testblob7",9);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
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
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("OUTPUT BIND BY NAME: \n");
	sqlrcur_prepareQuery($cur,"begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;");
	sqlrcur_defineOutputBind($cur,"numvar",10);
	sqlrcur_defineOutputBind($cur,"stringvar",10);
	sqlrcur_defineOutputBind($cur,"floatvar",10);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	$numvar=sqlrcur_getOutputBind($cur,"numvar");
	$stringvar=sqlrcur_getOutputBind($cur,"stringvar");
	$floatvar=sqlrcur_getOutputBind($cur,"floatvar");
	checkSuccess($db,$numvar,"1");
	checkSuccess($db,$stringvar,"hello");
	checkSuccess($db,$floatvar,"2.5");
	echo("\n");

	echo("OUTPUT BIND BY NAME: \n");
	sqlrcur_clearBinds($cur);
	sqlrcur_defineOutputBind($cur,"1",10);
	sqlrcur_defineOutputBind($cur,"2",10);
	sqlrcur_defineOutputBind($cur,"3",10);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	$numvar=sqlrcur_getOutputBind($cur,"1");
	$stringvar=sqlrcur_getOutputBind($cur,"2");
	$floatvar=sqlrcur_getOutputBind($cur,"3");
	checkSuccess($db,$numvar,"1");
	checkSuccess($db,$stringvar,"hello");
	checkSuccess($db,$floatvar,"2.5");
	echo("\n");

	echo("OUTPUT BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds($cur);
	sqlrcur_defineOutputBind($cur,"numvar",10);
	sqlrcur_defineOutputBind($cur,"stringvar",10);
	sqlrcur_defineOutputBind($cur,"floatvar",10);
	sqlrcur_defineOutputBind($cur,"dummyvar",10);
	sqlrcur_validateBinds($cur);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	$numvar=sqlrcur_getOutputBind($cur,"numvar");
	$stringvar=sqlrcur_getOutputBind($cur,"stringvar");
	$floatvar=sqlrcur_getOutputBind($cur,"floatvar");
	checkSuccess($db,$numvar,"1");
	checkSuccess($db,$stringvar,"hello");
	checkSuccess($db,$floatvar,"2.5");
	echo("\n");

	echo("SELECT: \n");
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	echo("\n");

	echo("COLUMN COUNT: \n");
	checkSuccess($db,sqlrcur_colCount($cur),7);
	echo("\n");

	echo("COLUMN NAMES: \n");
	checkSuccess($db,sqlrcur_getColumnName($cur,0),"TESTNUMBER");
	checkSuccess($db,sqlrcur_getColumnName($cur,1),"TESTCHAR");
	checkSuccess($db,sqlrcur_getColumnName($cur,2),"TESTVARCHAR");
	checkSuccess($db,sqlrcur_getColumnName($cur,3),"TESTDATE");
	checkSuccess($db,sqlrcur_getColumnName($cur,4),"TESTLONG");
	checkSuccess($db,sqlrcur_getColumnName($cur,5),"TESTCLOB");
	checkSuccess($db,sqlrcur_getColumnName($cur,6),"TESTBLOB");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($db,$cols[0],"TESTNUMBER");
	checkSuccess($db,$cols[1],"TESTCHAR");
	checkSuccess($db,$cols[2],"TESTVARCHAR");
	checkSuccess($db,$cols[3],"TESTDATE");
	checkSuccess($db,$cols[4],"TESTLONG");
	checkSuccess($db,$cols[5],"TESTCLOB");
	checkSuccess($db,$cols[6],"TESTBLOB");
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess($db,sqlrcur_getColumnType($cur,0),"NUMBER");
	checkSuccess($db,sqlrcur_getColumnType($cur,"testnumber"),"NUMBER");
	checkSuccess($db,sqlrcur_getColumnType($cur,1),"CHAR");
	checkSuccess($db,sqlrcur_getColumnType($cur,"testchar"),"CHAR");
	checkSuccess($db,sqlrcur_getColumnType($cur,2),"VARCHAR2");
	checkSuccess($db,sqlrcur_getColumnType($cur,"testvarchar"),"VARCHAR2");
	checkSuccess($db,sqlrcur_getColumnType($cur,3),"DATE");
	checkSuccess($db,sqlrcur_getColumnType($cur,"testdate"),"DATE");
	checkSuccess($db,sqlrcur_getColumnType($cur,4),"LONG");
	checkSuccess($db,sqlrcur_getColumnType($cur,"testlong"),"LONG");
	checkSuccess($db,sqlrcur_getColumnType($cur,5),"CLOB");
	checkSuccess($db,sqlrcur_getColumnType($cur,"testclob"),"CLOB");
	checkSuccess($db,sqlrcur_getColumnType($cur,6),"BLOB");
	checkSuccess($db,sqlrcur_getColumnType($cur,"testblob"),"BLOB");
	echo("\n");

	echo("COLUMN LENGTH: \n");
	checkSuccess($db,sqlrcur_getColumnLength($cur,0),22);
	checkSuccess($db,sqlrcur_getColumnLength($cur,"testnumber"),22);
	checkSuccess($db,sqlrcur_getColumnLength($cur,1),40);
	checkSuccess($db,sqlrcur_getColumnLength($cur,"testchar"),40);
	checkSuccess($db,sqlrcur_getColumnLength($cur,2),40);
	checkSuccess($db,sqlrcur_getColumnLength($cur,"testvarchar"),40);
	checkSuccess($db,sqlrcur_getColumnLength($cur,3),7);
	checkSuccess($db,sqlrcur_getColumnLength($cur,"testdate"),7);
	checkSuccess($db,sqlrcur_getColumnLength($cur,4),0);
	checkSuccess($db,sqlrcur_getColumnLength($cur,"testlong"),0);
	checkSuccess($db,sqlrcur_getColumnLength($cur,5),0);
	checkSuccess($db,sqlrcur_getColumnLength($cur,"testclob"),0);
	checkSuccess($db,sqlrcur_getColumnLength($cur,6),0);
	checkSuccess($db,sqlrcur_getColumnLength($cur,"testblob"),0);
	echo("\n");

	echo("LONGEST COLUMN: \n");
	checkSuccess($db,sqlrcur_getLongest($cur,0),1);
	checkSuccess($db,sqlrcur_getLongest($cur,"testnumber"),1);
	checkSuccess($db,sqlrcur_getLongest($cur,1),40);
	checkSuccess($db,sqlrcur_getLongest($cur,"testchar"),40);
	checkSuccess($db,sqlrcur_getLongest($cur,2),12);
	checkSuccess($db,sqlrcur_getLongest($cur,"testvarchar"),12);
	checkSuccess($db,sqlrcur_getLongest($cur,3),9);
	checkSuccess($db,sqlrcur_getLongest($cur,"testdate"),9);
	checkSuccess($db,sqlrcur_getLongest($cur,4),9);
	checkSuccess($db,sqlrcur_getLongest($cur,"testlong"),9);
	checkSuccess($db,sqlrcur_getLongest($cur,5),9);
	checkSuccess($db,sqlrcur_getLongest($cur,"testclob"),9);
	checkSuccess($db,sqlrcur_getLongest($cur,6),9);
	checkSuccess($db,sqlrcur_getLongest($cur,"testblob"),9);
	echo("\n");

	echo("ROW COUNT: \n");
	checkSuccess($db,sqlrcur_rowCount($cur),8);
	echo("\n");

	echo("TOTAL ROWS: \n");
	checkSuccess($db,sqlrcur_totalRows($cur),-1);
	echo("\n");

	echo("FIRST ROW INDEX: \n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),0);
	echo("\n");

	echo("END OF RESULT SET: \n");
	checkSuccess($db,sqlrcur_endOfResultSet($cur),1);
	echo("\n");

	echo("FIELDS BY INDEX: \n");
	checkSuccess($db,sqlrcur_getField($cur,0,0),"1");
	checkSuccess($db,sqlrcur_getField($cur,0,1),"testchar1                               ");
	checkSuccess($db,sqlrcur_getField($cur,0,2),"testvarchar1");
	checkSuccess($db,sqlrcur_getField($cur,0,3),"01-JAN-01");
	checkSuccess($db,sqlrcur_getField($cur,0,4),"testlong1");
	checkSuccess($db,sqlrcur_getField($cur,0,5),"testclob1");
	checkSuccess($db,sqlrcur_getField($cur,0,6),"");
	echo("\n");
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	checkSuccess($db,sqlrcur_getField($cur,7,1),"testchar8                               ");
	checkSuccess($db,sqlrcur_getField($cur,7,2),"testvarchar8");
	checkSuccess($db,sqlrcur_getField($cur,7,3),"01-JAN-08");
	checkSuccess($db,sqlrcur_getField($cur,7,4),"testlong8");
	checkSuccess($db,sqlrcur_getField($cur,7,5),"testclob8");
	checkSuccess($db,sqlrcur_getField($cur,7,6),"testblob8");
	echo("\n");

	echo("FIELD LENGTHS BY INDEX: \n");
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,0),1);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,1),40);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,2),12);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,3),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,4),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,5),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,6),0);
	echo("\n");
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,0),1);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,1),40);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,2),12);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,3),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,4),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,5),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,6),9);
	echo("\n");

	echo("FIELDS BY NAME: \n");
	checkSuccess($db,sqlrcur_getField($cur,0,"testnumber"),"1");
	checkSuccess($db,sqlrcur_getField($cur,0,"testchar"),"testchar1                               ");
	checkSuccess($db,sqlrcur_getField($cur,0,"testvarchar"),"testvarchar1");
	checkSuccess($db,sqlrcur_getField($cur,0,"testdate"),"01-JAN-01");
	checkSuccess($db,sqlrcur_getField($cur,0,"testlong"),"testlong1");
	checkSuccess($db,sqlrcur_getField($cur,0,"testclob"),"testclob1");
	checkSuccess($db,sqlrcur_getField($cur,0,"testblob"),"");
	echo("\n");
	checkSuccess($db,sqlrcur_getField($cur,7,"testnumber"),"8");
	checkSuccess($db,sqlrcur_getField($cur,7,"testchar"),"testchar8                               ");
	checkSuccess($db,sqlrcur_getField($cur,7,"testvarchar"),"testvarchar8");
	checkSuccess($db,sqlrcur_getField($cur,7,"testdate"),"01-JAN-08");
	checkSuccess($db,sqlrcur_getField($cur,7,"testlong"),"testlong8");
	checkSuccess($db,sqlrcur_getField($cur,7,"testclob"),"testclob8");
	checkSuccess($db,sqlrcur_getField($cur,7,"testblob"),"testblob8");
	echo("\n");

	echo("FIELD LENGTHS BY NAME: \n");
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,"testnumber"),1);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,"testchar"),40);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,"testvarchar"),12);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,"testdate"),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,"testlong"),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,"testclob"),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,0,"testblob"),0);
	echo("\n");
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,"testnumber"),1);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,"testchar"),40);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,"testvarchar"),12);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,"testdate"),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,"testlong"),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,"testclob"),9);
	checkSuccess($db,sqlrcur_getFieldLength($cur,7,"testblob"),9);
	echo("\n");

	echo("FIELDS BY ARRAY: \n");
	$fields=sqlrcur_getRow($cur,0);
	checkSuccess($db,$fields[0],"1");
	checkSuccess($db,$fields[1],"testchar1                               ");
	checkSuccess($db,$fields[2],"testvarchar1");
	checkSuccess($db,$fields[3],"01-JAN-01");
	checkSuccess($db,$fields[4],"testlong1");
	checkSuccess($db,$fields[5],"testclob1");
	checkSuccess($db,$fields[6],"");
	echo("\n");

	echo("FIELD LENGTHS BY ARRAY: \n");
	$fieldlens=sqlrcur_getRowLengths($cur,0);
	checkSuccess($db,$fieldlens[0],1);
	checkSuccess($db,$fieldlens[1],40);
	checkSuccess($db,$fieldlens[2],12);
	checkSuccess($db,$fieldlens[3],9);
	checkSuccess($db,$fieldlens[4],9);
	checkSuccess($db,$fieldlens[5],9);
	checkSuccess($db,$fieldlens[6],0);
	echo("\n");

	echo("FIELDS BY ASSOCIATIVE ARRAY: \n");
	$fields=sqlrcur_getRowAssoc($cur,0);
	checkSuccess($db,$fields["TESTNUMBER"],1);
	checkSuccess($db,$fields["TESTCHAR"],"testchar1                               ");
	checkSuccess($db,$fields["TESTVARCHAR"],"testvarchar1");
	checkSuccess($db,$fields["TESTDATE"],"01-JAN-01");
	checkSuccess($db,$fields["TESTLONG"],"testlong1");
	checkSuccess($db,$fields["TESTCLOB"],"testclob1");
	checkSuccess($db,$fields["TESTBLOB"],"");
	echo("\n");
	$fields=sqlrcur_getRowAssoc($cur,7);
	checkSuccess($db,$fields["TESTNUMBER"],8);
	checkSuccess($db,$fields["TESTCHAR"],"testchar8                               ");
	checkSuccess($db,$fields["TESTVARCHAR"],"testvarchar8");
	checkSuccess($db,$fields["TESTDATE"],"01-JAN-08");
	checkSuccess($db,$fields["TESTLONG"],"testlong8");
	checkSuccess($db,$fields["TESTCLOB"],"testclob8");
	checkSuccess($db,$fields["TESTBLOB"],"testblob8");
	echo("\n");

	echo("FIELD LENGTHS BY ASSOCIATIVE ARRAY: \n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,0);
	checkSuccess($db,$fieldlengths["TESTNUMBER"],1);
	checkSuccess($db,$fieldlengths["TESTCHAR"],40);
	checkSuccess($db,$fieldlengths["TESTVARCHAR"],12);
	checkSuccess($db,$fieldlengths["TESTDATE"],9);
	checkSuccess($db,$fieldlengths["TESTLONG"],9);
	checkSuccess($db,$fieldlengths["TESTCLOB"],9);
	checkSuccess($db,$fieldlengths["TESTBLOB"],0);
	echo("\n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,7);
	checkSuccess($db,$fieldlengths["TESTNUMBER"],1);
	checkSuccess($db,$fieldlengths["TESTCHAR"],40);
	checkSuccess($db,$fieldlengths["TESTVARCHAR"],12);
	checkSuccess($db,$fieldlengths["TESTDATE"],9);
	checkSuccess($db,$fieldlengths["TESTLONG"],9);
	checkSuccess($db,$fieldlengths["TESTCLOB"],9);
	checkSuccess($db,$fieldlengths["TESTBLOB"],9);
	echo("\n");

	echo("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery($cur,"select $(var1),'$(var2)',$(var3) from dual");
	sqlrcur_substitution($cur,"var1",1);
	sqlrcur_substitution($cur,"var2","hello");
	sqlrcur_substitution($cur,"var3",10.5556,6,4);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("FIELDS: \n");
	checkSuccess($db,sqlrcur_getField($cur,0,0),"1");
	checkSuccess($db,sqlrcur_getField($cur,0,1),"hello");
	checkSuccess($db,sqlrcur_getField($cur,0,2),"10.5556");
	echo("\n");

	echo("OUTPUT BIND: \n");
	sqlrcur_prepareQuery($cur,"begin :var1:='hello'; end;");
	sqlrcur_defineOutputBind($cur,"var1",10);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	checkSuccess($db,sqlrcur_getOutputBind($cur,"var1"),"hello");
	echo("\n");

	echo("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery($cur,"select $(var1),'$(var2)',$(var3) from dual");
	$vars=array("var1","var2","var3");
	$vals=array(1,"hello",10.5556);
	$precs=array(0,0,6);
	$scales=array(0,0,4);
	sqlrcur_substitutions($cur,$vars,$vals,$precs,$scales);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("FIELDS: \n");
	checkSuccess($db,sqlrcur_getField($cur,0,0),"1");
	checkSuccess($db,sqlrcur_getField($cur,0,1),"hello");
	checkSuccess($db,sqlrcur_getField($cur,0,2),"10.5556");
	echo("\n");

	echo("NULLS as Nulls: \n");
	sqlrcur_getNullsAsNulls($cur);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select NULL,1,NULL from dual"),1);
	checkSuccess($db,sqlrcur_getField($cur,0,0),NULL);
	checkSuccess($db,sqlrcur_getField($cur,0,1),"1");
	checkSuccess($db,sqlrcur_getField($cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings($cur);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select NULL,1,NULL from dual"),1);
	checkSuccess($db,sqlrcur_getField($cur,0,0),"");
	checkSuccess($db,sqlrcur_getField($cur,0,1),"1");
	checkSuccess($db,sqlrcur_getField($cur,0,2),"");
	sqlrcur_getNullsAsNulls($cur);
	echo("\n");

	echo("RESULT SET BUFFER SIZE: \n");
	checkSuccess($db,sqlrcur_getResultSetBufferSize($cur),0);
	sqlrcur_setResultSetBufferSize($cur,2);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	checkSuccess($db,sqlrcur_getResultSetBufferSize($cur),2);
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),0);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),0);
	checkSuccess($db,sqlrcur_rowCount($cur),2);
	checkSuccess($db,sqlrcur_getField($cur,0,0),"1");
	checkSuccess($db,sqlrcur_getField($cur,1,0),"2");
	checkSuccess($db,sqlrcur_getField($cur,2,0),"3");
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),2);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),0);
	checkSuccess($db,sqlrcur_rowCount($cur),4);
	checkSuccess($db,sqlrcur_getField($cur,6,0),"7");
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),6);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),0);
	checkSuccess($db,sqlrcur_rowCount($cur),8);
	checkSuccess($db,sqlrcur_getField($cur,8,0),NULL);
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),8);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),1);
	checkSuccess($db,sqlrcur_rowCount($cur),8);
	echo("\n");

	echo("DONT GET COLUMN INFO: \n");
	sqlrcur_dontGetColumnInfo($cur);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	checkSuccess($db,sqlrcur_getColumnName($cur,0),NULL);
	checkSuccess($db,sqlrcur_getColumnLength($cur,0),0);
	checkSuccess($db,sqlrcur_getColumnType($cur,0),NULL);
	sqlrcur_getColumnInfo($cur);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	checkSuccess($db,sqlrcur_getColumnName($cur,0),"TESTNUMBER");
	checkSuccess($db,sqlrcur_getColumnLength($cur,0),22);
	checkSuccess($db,sqlrcur_getColumnType($cur,0),"NUMBER");
	echo("\n");

	echo("SUSPENDED SESSION: \n");
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	sqlrcur_suspendResultSet($cur);
	checkSuccess($db,sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess($db,sqlrcon_resumeSession($con,$conport,$consocket),1);
	echo("\n");
	checkSuccess($db,sqlrcur_getField($cur,0,0),"1");
	checkSuccess($db,sqlrcur_getField($cur,1,0),"2");
	checkSuccess($db,sqlrcur_getField($cur,2,0),"3");
	checkSuccess($db,sqlrcur_getField($cur,3,0),"4");
	checkSuccess($db,sqlrcur_getField($cur,4,0),"5");
	checkSuccess($db,sqlrcur_getField($cur,5,0),"6");
	checkSuccess($db,sqlrcur_getField($cur,6,0),"7");
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	sqlrcur_suspendResultSet($cur);
	checkSuccess($db,sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess($db,sqlrcon_resumeSession($con,$conport,$consocket),1);
	echo("\n");
	checkSuccess($db,sqlrcur_getField($cur,0,0),"1");
	checkSuccess($db,sqlrcur_getField($cur,1,0),"2");
	checkSuccess($db,sqlrcur_getField($cur,2,0),"3");
	checkSuccess($db,sqlrcur_getField($cur,3,0),"4");
	checkSuccess($db,sqlrcur_getField($cur,4,0),"5");
	checkSuccess($db,sqlrcur_getField($cur,5,0),"6");
	checkSuccess($db,sqlrcur_getField($cur,6,0),"7");
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	sqlrcur_suspendResultSet($cur);
	checkSuccess($db,sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess($db,sqlrcon_resumeSession($con,$conport,$consocket),1);
	echo("\n");
	checkSuccess($db,sqlrcur_getField($cur,0,0),"1");
	checkSuccess($db,sqlrcur_getField($cur,1,0),"2");
	checkSuccess($db,sqlrcur_getField($cur,2,0),"3");
	checkSuccess($db,sqlrcur_getField($cur,3,0),"4");
	checkSuccess($db,sqlrcur_getField($cur,4,0),"5");
	checkSuccess($db,sqlrcur_getField($cur,5,0),"6");
	checkSuccess($db,sqlrcur_getField($cur,6,0),"7");
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	echo("\n");

	echo("SUSPENDED RESULT SET: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	checkSuccess($db,sqlrcur_getField($cur,2,0),"3");
	$id=sqlrcur_getResultSetId($cur);
	sqlrcur_suspendResultSet($cur);
	checkSuccess($db,sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	checkSuccess($db,sqlrcon_resumeSession($con,$conport,$consocket),1);
	checkSuccess($db,sqlrcur_resumeResultSet($cur,$id),1);
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),4);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),0);
	checkSuccess($db,sqlrcur_rowCount($cur),6);
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),6);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),0);
	checkSuccess($db,sqlrcur_rowCount($cur),8);
	checkSuccess($db,sqlrcur_getField($cur,8,0),NULL);
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),8);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),1);
	checkSuccess($db,sqlrcur_rowCount($cur),8);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("CACHED RESULT SET: \n");
	sqlrcur_cacheToFile($cur,"/tmp/cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($db,$filename,"/tmp/cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess($db,sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	echo("\n");

	echo("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess($db,sqlrcur_colCount($cur),7);
	echo("\n");

	echo("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess($db,sqlrcur_getColumnName($cur,0),"TESTNUMBER");
	checkSuccess($db,sqlrcur_getColumnName($cur,1),"TESTCHAR");
	checkSuccess($db,sqlrcur_getColumnName($cur,2),"TESTVARCHAR");
	checkSuccess($db,sqlrcur_getColumnName($cur,3),"TESTDATE");
	checkSuccess($db,sqlrcur_getColumnName($cur,4),"TESTLONG");
	checkSuccess($db,sqlrcur_getColumnName($cur,5),"TESTCLOB");
	checkSuccess($db,sqlrcur_getColumnName($cur,6),"TESTBLOB");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($db,$cols[0],"TESTNUMBER");
	checkSuccess($db,$cols[1],"TESTCHAR");
	checkSuccess($db,$cols[2],"TESTVARCHAR");
	checkSuccess($db,$cols[3],"TESTDATE");
	checkSuccess($db,$cols[4],"TESTLONG");
	checkSuccess($db,$cols[5],"TESTCLOB");
	checkSuccess($db,$cols[6],"TESTBLOB");
	echo("\n");

	echo("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"/tmp/cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($db,$filename,"/tmp/cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess($db,sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	checkSuccess($db,sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("FROM ONE CACHE FILE TO ANOTHER: \n");
	sqlrcur_cacheToFile($cur,"/tmp/cachefile2");
	checkSuccess($db,sqlrcur_openCachedResultSet($cur,"/tmp/cachefile1"),1);
	sqlrcur_cacheOff($cur);
	checkSuccess($db,sqlrcur_openCachedResultSet($cur,"/tmp/cachefile2"),1);
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	checkSuccess($db,sqlrcur_getField($cur,8,0),NULL);
	echo("\n");

	echo("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"/tmp/cachefile2");
	checkSuccess($db,sqlrcur_openCachedResultSet($cur,"/tmp/cachefile1"),1);
	sqlrcur_cacheOff($cur);
	checkSuccess($db,sqlrcur_openCachedResultSet($cur,"/tmp/cachefile2"),1);
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	checkSuccess($db,sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"/tmp/cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),1);
	checkSuccess($db,sqlrcur_getField($cur,2,0),"3");
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($db,$filename,"/tmp/cachefile1");
	$id=sqlrcur_getResultSetId($cur);
	sqlrcur_suspendResultSet($cur);
	checkSuccess($db,sqlrcon_suspendSession($con),1);
	$conport=sqlrcon_getConnectionPort($con);
	$consocket=sqlrcon_getConnectionSocket($con);
	echo("\n");
	checkSuccess($db,sqlrcon_resumeSession($con,$conport,$consocket),1);
	checkSuccess($db,sqlrcur_resumeCachedResultSet($cur,$id,$filename),1);
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),4);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),0);
	checkSuccess($db,sqlrcur_rowCount($cur),6);
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),6);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),0);
	checkSuccess($db,sqlrcur_rowCount($cur),8);
	checkSuccess($db,sqlrcur_getField($cur,8,0),NULL);
	echo("\n");
	checkSuccess($db,sqlrcur_firstRowIndex($cur),8);
	checkSuccess($db,sqlrcur_endOfResultSet($cur),1);
	checkSuccess($db,sqlrcur_rowCount($cur),8);
	sqlrcur_cacheOff($cur);
	echo("\n");
	checkSuccess($db,sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess($db,sqlrcur_getField($cur,7,0),"8");
	checkSuccess($db,sqlrcur_getField($cur,8,0),NULL);
	sqlrcur_setResultSetBufferSize($cur,0);
	echo("\n");

	echo("COMMIT AND ROLLBACK: \n");
	$secondcon=sqlrcon_alloc($host,$port,$socket,$user,$password,0,1);
	$secondcur=sqlrcur_alloc($secondcon);
	checkSuccess($db,sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess($db,sqlrcur_getField($secondcur,0,0),"0");
	checkSuccess($db,sqlrcon_commit($con),1);
	checkSuccess($db,sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess($db,sqlrcur_getField($secondcur,0,0),"8");
	checkSuccess($db,sqlrcon_autoCommitOn($con),1);
	checkSuccess($db,sqlrcur_sendQuery($cur,"insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10','testclob10',empty_blob())"),1);
	checkSuccess($db,sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess($db,sqlrcur_getField($secondcur,0,0),"9");
	checkSuccess($db,sqlrcon_autoCommitOff($con),1);
	echo("\n");


	echo("CLOB AND BLOB OUTPUT BIND: \n");
	sqlrcur_sendQuery($cur,"drop table testtable1");
	checkSuccess($db,sqlrcur_sendQuery($cur,"create table testtable1 (testclob clob, testblob blob)"),1);
	sqlrcur_prepareQuery($cur,"insert into testtable1 values ('hello',:var1)");
	sqlrcur_inputBindBlob($cur,"var1","hello",5);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	sqlrcur_prepareQuery($cur,"begin select testclob into :clobvar from testtable1;  select testblob into :blobvar from testtable1; end;");
	sqlrcur_defineOutputBindClob($cur,"clobvar");
	sqlrcur_defineOutputBindBlob($cur,"blobvar");
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	$clobvar=sqlrcur_getOutputBind($cur,"clobvar");
	$clobvarlength=sqlrcur_getOutputBindLength($cur,"clobvar");
	$blobvar=sqlrcur_getOutputBind($cur,"blobvar");
	$blobvarlength=sqlrcur_getOutputBindLength($cur,"blobvar");
	checkSuccess($db,$clobvar,"hello");
	checkSuccess($db,$clobvarlength,5);
	checkSuccess($db,$blobvar,"hello",5);
	checkSuccess($db,$blobvarlength,5);
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
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	sqlrcur_sendQuery($cur,"select * from testtable1");
	checkSuccess($db,sqlrcur_getField($cur,0,0),NULL);
	checkSuccess($db,sqlrcur_getField($cur,0,1),NULL);
	checkSuccess($db,sqlrcur_getField($cur,0,2),NULL);
	checkSuccess($db,sqlrcur_getField($cur,0,3),NULL);
	sqlrcur_sendQuery($cur,"drop table testtable1");
	echo("\n");

	echo("CURSOR BINDS: \n");
	checkSuccess($db,sqlrcur_sendQuery($cur,"create or replace package types as type cursorType is ref cursor; end;"),1);
	checkSuccess($db,sqlrcur_sendQuery($cur,"create or replace function sp_testtable return types.cursortype as l_cursor    types.cursorType; begin open l_cursor for select * from testtable; return l_cursor; end;"),1);
	sqlrcur_prepareQuery($cur,"begin  :curs:=sp_testtable; end;");
	sqlrcur_defineOutputBindCursor($cur,"curs");
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	$bindcur=sqlrcur_getOutputBindCursor($cur,"curs");
	checkSuccess($db,sqlrcur_fetchFromBindCursor($bindcur),1);
	checkSuccess($db,sqlrcur_getField($bindcur,0,0),"1");
	checkSuccess($db,sqlrcur_getField($bindcur,1,0),"2");
	checkSuccess($db,sqlrcur_getField($bindcur,2,0),"3");
	checkSuccess($db,sqlrcur_getField($bindcur,3,0),"4");
	checkSuccess($db,sqlrcur_getField($bindcur,4,0),"5");
	checkSuccess($db,sqlrcur_getField($bindcur,5,0),"6");
	checkSuccess($db,sqlrcur_getField($bindcur,6,0),"7");
	checkSuccess($db,sqlrcur_getField($bindcur,7,0),"8");
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
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	sqlrcur_sendQuery($cur,"select testclob from testtable2");
	checkSuccess($db,$clobval,sqlrcur_getField($cur,0,"testclob"));
	sqlrcur_prepareQuery($cur,"begin select testclob into :clobbindval from testtable2; end;");
	sqlrcur_defineOutputBindClob($cur,"clobbindval");
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	$clobbindvar=sqlrcur_getOutputBind($cur,"clobbindval");
	checkSuccess($db,sqlrcur_getOutputBindLength($cur,"clobbindval"),8*1024);
	checkSuccess($db,$clobval,$clobbindvar);
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
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	sqlrcur_sendQuery($cur,"select testval from testtable2");
	checkSuccess($db,$testval,sqlrcur_getField($cur,0,"testval"));
	$query="begin :bindval:='".$testval."'; end;";
	sqlrcur_prepareQuery($cur,$query);
	sqlrcur_defineOutputBind($cur,"bindval",4000);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	checkSuccess($db,sqlrcur_getOutputBindLength($cur,"bindval"),4000);
	checkSuccess($db,sqlrcur_getOutputBind($cur,"bindval"),$testval);
	sqlrcur_sendQuery($cur,"drop table testtable2");
	echo("\n");

	echo("NEGATIVE INPUT BIND\n");
	sqlrcur_sendQuery($cur,"create table testtable2 (testval number)");
	sqlrcur_prepareQuery($cur,"insert into testtable2 values (:testval)");
	sqlrcur_inputBind($cur,"testval",-1);
	checkSuccess($db,sqlrcur_executeQuery($cur),1);
	sqlrcur_sendQuery($cur,"select testval from testtable2");
	checkSuccess($db,sqlrcur_getField($cur,0,"testval"),"-1");
	sqlrcur_sendQuery($cur,"drop table testtable2");
	echo("\n");


	# drop existing table
	sqlrcur_sendQuery($cur,"drop table testtable");

	# invalid queries...
	echo("INVALID QUERIES: \n");
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),0);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),0);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),0);
	checkSuccess($db,sqlrcur_sendQuery($cur,"select * from testtable order by testnumber"),0);
	echo("\n");
	checkSuccess($db,sqlrcur_sendQuery($cur,"insert into testtable values (1,2,3,4)"),0);
	checkSuccess($db,sqlrcur_sendQuery($cur,"insert into testtable values (1,2,3,4)"),0);
	checkSuccess($db,sqlrcur_sendQuery($cur,"insert into testtable values (1,2,3,4)"),0);
	checkSuccess($db,sqlrcur_sendQuery($cur,"insert into testtable values (1,2,3,4)"),0);
	echo("\n");
	checkSuccess($db,sqlrcur_sendQuery($cur,"create table testtable"),0);
	checkSuccess($db,sqlrcur_sendQuery($cur,"create table testtable"),0);
	checkSuccess($db,sqlrcur_sendQuery($cur,"create table testtable"),0);
	checkSuccess($db,sqlrcur_sendQuery($cur,"create table testtable"),0);
	echo("\n");
*/
?></pre></html>
