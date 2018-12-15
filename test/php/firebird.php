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

	$subvars=array("var1","var2","var3");
	$subvalstrings=array("hi","hello","bye");
	$subvallongs=array(1,2,3);
	$subvaldoubles=array(10.55,10.556,10.5556);
	$precs=array(4,5,6);
	$scales=array(2,3,4);

	# instantiation
	$con=sqlrcon_alloc($host,$port,$socket,$user,$password,0,1);
	$cur=sqlrcur_alloc($con);

	# get database type
	echo("IDENTIFY: \n");
	checkSuccess(sqlrcon_identify($con),"firebird");
	echo("\n");

	# ping
	echo("PING: \n");
	checkSuccess(sqlrcon_ping($con),1);
	echo("\n");

	# clear table
	sqlrcur_sendQuery($cur,"delete from testtable");
	sqlrcon_commit($con);

	echo("INSERT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (1,1,1.1,1.1,1.1,1.1,'01-JAN-2001','01:00:00','testchar1','testvarchar1',NULL,NULL)"),1);
	echo("\n");


	echo("BIND BY POSITION: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (?,?,?,?,?,?,?,?,?,?,?,NULL)");
	checkSuccess(sqlrcur_countBindVariables($cur),11);
	sqlrcur_inputBind($cur,"1",2);
	sqlrcur_inputBind($cur,"2",2);
	sqlrcur_inputBind($cur,"3",2.2,2,1);
	sqlrcur_inputBind($cur,"4",2.2,2,1);
	sqlrcur_inputBind($cur,"5",2.2,2,1);
	sqlrcur_inputBind($cur,"6",2.2,2,1);
	sqlrcur_inputBind($cur,"7","01-JAN-2002");
	sqlrcur_inputBind($cur,"8","02:00:00");
	sqlrcur_inputBind($cur,"9","testchar2");
	sqlrcur_inputBind($cur,"10","testvarchar2");
	sqlrcur_inputBind($cur,"11",NULL);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"1",3);
	sqlrcur_inputBind($cur,"2",3);
	sqlrcur_inputBind($cur,"3",3.3,2,1);
	sqlrcur_inputBind($cur,"4",3.3,2,1);
	sqlrcur_inputBind($cur,"5",3.3,2,1);
	sqlrcur_inputBind($cur,"6",3.3,2,1);
	sqlrcur_inputBind($cur,"7","01-JAN-2003");
	sqlrcur_inputBind($cur,"8","03:00:00");
	sqlrcur_inputBind($cur,"9","testchar3");
	sqlrcur_inputBind($cur,"10","testvarchar3");
	sqlrcur_inputBind($cur,"11",NULL);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY OF BINDS BY POSITION: \n");
	sqlrcur_clearBinds($cur);
	$bindvars=array("1","2","3","4","5","6",
				"7","8","9","10","11");
	$bindvals=array("4","4","4.4","4.4","4.4","4.4",
				"01-JAN-2004","04:00:00",
				"testchar4","testvarchar4",NULL);
	sqlrcur_inputBinds($cur,$bindvars,$bindvals);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("INSERT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (5,5,5.5,5.5,5.5,5.5,'01-JAN-2005','05:00:00','testchar5','testvarchar5',NULL,NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (6,6,6.6,6.6,6.6,6.6,'01-JAN-2006','06:00:00','testchar6','testvarchar6',NULL,NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (7,7,7.7,7.7,7.7,7.7,'01-JAN-2007','07:00:00','testchar7','testvarchar7',NULL,NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (8,8,8.8,8.8,8.8,8.8,'01-JAN-2008','08:00:00','testchar8','testvarchar8',NULL,NULL)"),1);
	echo("\n");

	echo("AFFECTED ROWS: \n");
	checkSuccess(sqlrcur_affectedRows($cur),0);
	echo("\n");

	echo("STORED PROCEDURE: \n");
	sqlrcur_prepareQuery($cur,"select * from testproc(?,?,?,NULL)");
	sqlrcur_inputBind($cur,"1",1);
	sqlrcur_inputBind($cur,"2",1.1,2,1);
	sqlrcur_inputBind($cur,"3","hello");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"1.1000");
	checkSuccess(sqlrcur_getField($cur,0,2),"hello");
	sqlrcur_prepareQuery($cur,"execute procedure testproc ?, ?, ?, NULL");
	sqlrcur_inputBind($cur,"1",1);
	sqlrcur_inputBind($cur,"2",1.1,2,1);
	sqlrcur_inputBind($cur,"3","hello");
	sqlrcur_defineOutputBindInteger($cur,"1");
	sqlrcur_defineOutputBindDouble($cur,"2");
	sqlrcur_defineOutputBindString($cur,"3",20);
	sqlrcur_defineOutputBindBlob($cur,"4");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	checkSuccess(sqlrcur_getOutputBindInteger($cur,"1"),1);
	//checkSuccess(sqlrcur_getOutputBindDouble($cur,"2"),1.1);
	checkSuccess(sqlrcur_getOutputBindString($cur,"3"),"hello               ");
	echo("\n");

	echo("SELECT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
	echo("\n");

	echo("COLUMN COUNT: \n");
	checkSuccess(sqlrcur_colCount($cur),12);
	echo("\n");

	echo("COLUMN NAMES: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"TESTINTEGER");
	checkSuccess(sqlrcur_getColumnName($cur,1),"TESTSMALLINT");
	checkSuccess(sqlrcur_getColumnName($cur,2),"TESTDECIMAL");
	checkSuccess(sqlrcur_getColumnName($cur,3),"TESTNUMERIC");
	checkSuccess(sqlrcur_getColumnName($cur,4),"TESTFLOAT");
	checkSuccess(sqlrcur_getColumnName($cur,5),"TESTDOUBLE");
	checkSuccess(sqlrcur_getColumnName($cur,6),"TESTDATE");
	checkSuccess(sqlrcur_getColumnName($cur,7),"TESTTIME");
	checkSuccess(sqlrcur_getColumnName($cur,8),"TESTCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,9),"TESTVARCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,10),"TESTTIMESTAMP");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"TESTINTEGER");
	checkSuccess($cols[1],"TESTSMALLINT");
	checkSuccess($cols[2],"TESTDECIMAL");
	checkSuccess($cols[3],"TESTNUMERIC");
	checkSuccess($cols[4],"TESTFLOAT");
	checkSuccess($cols[5],"TESTDOUBLE");
	checkSuccess($cols[6],"TESTDATE");
	checkSuccess($cols[7],"TESTTIME");
	checkSuccess($cols[8],"TESTCHAR");
	checkSuccess($cols[9],"TESTVARCHAR");
	checkSuccess($cols[10],"TESTTIMESTAMP");
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess(sqlrcur_getColumnType($cur,0),"INTEGER");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTINTEGER"),"INTEGER");
	checkSuccess(sqlrcur_getColumnType($cur,1),"SMALLINT");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTSMALLINT"),"SMALLINT");
	checkSuccess(sqlrcur_getColumnType($cur,2),"DECIMAL");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTDECIMAL"),"DECIMAL");
	checkSuccess(sqlrcur_getColumnType($cur,3),"NUMERIC");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTNUMERIC"),"NUMERIC");
	checkSuccess(sqlrcur_getColumnType($cur,4),"FLOAT");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTFLOAT"),"FLOAT");
	checkSuccess(sqlrcur_getColumnType($cur,5),"DOUBLE PRECISION");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTDOUBLE"),"DOUBLE PRECISION");
	checkSuccess(sqlrcur_getColumnType($cur,6),"DATE");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTDATE"),"DATE");
	checkSuccess(sqlrcur_getColumnType($cur,7),"TIME");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTTIME"),"TIME");
	checkSuccess(sqlrcur_getColumnType($cur,8),"CHAR");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTCHAR"),"CHAR");
	checkSuccess(sqlrcur_getColumnType($cur,9),"VARCHAR");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTVARCHAR"),"VARCHAR");
	checkSuccess(sqlrcur_getColumnType($cur,10),"TIMESTAMP");
	checkSuccess(sqlrcur_getColumnType($cur,"TESTTIMESTAMP"),"TIMESTAMP");
	echo("\n");

	echo("COLUMN LENGTH: \n");
	checkSuccess(sqlrcur_getColumnLength($cur,0),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTINTEGER"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,1),2);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTSMALLINT"),2);
	checkSuccess(sqlrcur_getColumnLength($cur,2),8);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTDECIMAL"),8);
	checkSuccess(sqlrcur_getColumnLength($cur,3),8);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTNUMERIC"),8);
	checkSuccess(sqlrcur_getColumnLength($cur,4),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTFLOAT"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,5),8);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTDOUBLE"),8);
	checkSuccess(sqlrcur_getColumnLength($cur,6),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTDATE"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,7),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTTIME"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,8),50);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTCHAR"),50);
	checkSuccess(sqlrcur_getColumnLength($cur,9),50);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTVARCHAR"),50);
	checkSuccess(sqlrcur_getColumnLength($cur,10),8);
	checkSuccess(sqlrcur_getColumnLength($cur,"TESTTIMESTAMP"),8);
	echo("\n");

	echo("LONGEST COLUMN: \n");
	checkSuccess(sqlrcur_getLongest($cur,0),1);
	checkSuccess(sqlrcur_getLongest($cur,"TESTINTEGER"),1);
	checkSuccess(sqlrcur_getLongest($cur,1),1);
	checkSuccess(sqlrcur_getLongest($cur,"TESTSMALLINT"),1);
	checkSuccess(sqlrcur_getLongest($cur,2),4);
	checkSuccess(sqlrcur_getLongest($cur,"TESTDECIMAL"),4);
	checkSuccess(sqlrcur_getLongest($cur,3),4);
	checkSuccess(sqlrcur_getLongest($cur,"TESTNUMERIC"),4);
	checkSuccess(sqlrcur_getLongest($cur,4),6);
	checkSuccess(sqlrcur_getLongest($cur,"TESTFLOAT"),6);
	checkSuccess(sqlrcur_getLongest($cur,5),6);
	checkSuccess(sqlrcur_getLongest($cur,"TESTDOUBLE"),6);
	checkSuccess(sqlrcur_getLongest($cur,6),10);
	checkSuccess(sqlrcur_getLongest($cur,"TESTDATE"),10);
	checkSuccess(sqlrcur_getLongest($cur,7),8);
	checkSuccess(sqlrcur_getLongest($cur,"TESTTIME"),8);
	checkSuccess(sqlrcur_getLongest($cur,8),50);
	checkSuccess(sqlrcur_getLongest($cur,"TESTCHAR"),50);
	checkSuccess(sqlrcur_getLongest($cur,9),12);
	checkSuccess(sqlrcur_getLongest($cur,"TESTVARCHAR"),12);
	checkSuccess(sqlrcur_getLongest($cur,10),0);
	checkSuccess(sqlrcur_getLongest($cur,"TESTTIMESTAMP"),0);
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
	checkSuccess(sqlrcur_getField($cur,0,1),"1");
	checkSuccess(sqlrcur_getField($cur,0,2),"1.10");
	checkSuccess(sqlrcur_getField($cur,0,3),"1.10");
	checkSuccess(sqlrcur_getField($cur,0,4),"1.1000");
	checkSuccess(sqlrcur_getField($cur,0,5),"1.1000");
	checkSuccess(sqlrcur_getField($cur,0,6),"2001:01:01");
	checkSuccess(sqlrcur_getField($cur,0,7),"01:00:00");
	checkSuccess(sqlrcur_getField($cur,0,8),"testchar1                                         ");
	checkSuccess(sqlrcur_getField($cur,0,9),"testvarchar1");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,7,1),"8");
	checkSuccess(sqlrcur_getField($cur,7,2),"8.80");
	checkSuccess(sqlrcur_getField($cur,7,3),"8.80");
	checkSuccess(sqlrcur_getField($cur,7,4),"8.8000");
	checkSuccess(sqlrcur_getField($cur,7,5),"8.8000");
	checkSuccess(sqlrcur_getField($cur,7,6),"2008:01:01");
	checkSuccess(sqlrcur_getField($cur,7,7),"08:00:00");
	checkSuccess(sqlrcur_getField($cur,7,8),"testchar8                                         ");
	checkSuccess(sqlrcur_getField($cur,7,9),"testvarchar8");
	echo("\n");

	echo("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,1),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,2),4);
	checkSuccess(sqlrcur_getFieldLength($cur,0,3),4);
	checkSuccess(sqlrcur_getFieldLength($cur,0,4),6);
	checkSuccess(sqlrcur_getFieldLength($cur,0,5),6);
	checkSuccess(sqlrcur_getFieldLength($cur,0,6),10);
	checkSuccess(sqlrcur_getFieldLength($cur,0,7),8);
	checkSuccess(sqlrcur_getFieldLength($cur,0,8),50);
	checkSuccess(sqlrcur_getFieldLength($cur,0,9),12);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,1),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,2),4);
	checkSuccess(sqlrcur_getFieldLength($cur,7,3),4);
	checkSuccess(sqlrcur_getFieldLength($cur,7,4),6);
	checkSuccess(sqlrcur_getFieldLength($cur,7,5),6);
	checkSuccess(sqlrcur_getFieldLength($cur,7,6),10);
	checkSuccess(sqlrcur_getFieldLength($cur,7,7),8);
	checkSuccess(sqlrcur_getFieldLength($cur,7,8),50);
	checkSuccess(sqlrcur_getFieldLength($cur,7,9),12);
	echo("\n");

	echo("FIELDS BY NAME: \n");
	checkSuccess(sqlrcur_getField($cur,0,"TESTINTEGER"),"1");
	checkSuccess(sqlrcur_getField($cur,0,"TESTSMALLINT"),"1");
	checkSuccess(sqlrcur_getField($cur,0,"TESTDECIMAL"),"1.10");
	checkSuccess(sqlrcur_getField($cur,0,"TESTNUMERIC"),"1.10");
	checkSuccess(sqlrcur_getField($cur,0,"TESTFLOAT"),"1.1000");
	checkSuccess(sqlrcur_getField($cur,0,"TESTDOUBLE"),"1.1000");
	checkSuccess(sqlrcur_getField($cur,0,"TESTDATE"),"2001:01:01");
	checkSuccess(sqlrcur_getField($cur,0,"TESTTIME"),"01:00:00");
	checkSuccess(sqlrcur_getField($cur,0,"TESTCHAR"),"testchar1                                         ");
	checkSuccess(sqlrcur_getField($cur,0,"TESTVARCHAR"),"testvarchar1");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,"TESTINTEGER"),"8");
	checkSuccess(sqlrcur_getField($cur,7,"TESTSMALLINT"),"8");
	checkSuccess(sqlrcur_getField($cur,7,"TESTDECIMAL"),"8.80");
	checkSuccess(sqlrcur_getField($cur,7,"TESTNUMERIC"),"8.80");
	checkSuccess(sqlrcur_getField($cur,7,"TESTFLOAT"),"8.8000");
	checkSuccess(sqlrcur_getField($cur,7,"TESTDOUBLE"),"8.8000");
	checkSuccess(sqlrcur_getField($cur,7,"TESTDATE"),"2008:01:01");
	checkSuccess(sqlrcur_getField($cur,7,"TESTTIME"),"08:00:00");
	checkSuccess(sqlrcur_getField($cur,7,"TESTCHAR"),"testchar8                                         ");
	checkSuccess(sqlrcur_getField($cur,7,"TESTVARCHAR"),"testvarchar8");
	echo("\n");

	echo("FIELD LENGTHS BY NAME: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTINTEGER"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTSMALLINT"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTDECIMAL"),4);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTNUMERIC"),4);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTFLOAT"),6);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTDOUBLE"),6);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTDATE"),10);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTTIME"),8);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTCHAR"),50);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"TESTVARCHAR"),12);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTINTEGER"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTSMALLINT"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTDECIMAL"),4);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTNUMERIC"),4);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTFLOAT"),6);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTDOUBLE"),6);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTDATE"),10);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTTIME"),8);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTCHAR"),50);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"TESTVARCHAR"),12);
	echo("\n");

	echo("FIELDS BY ARRAY: \n");
	$fields=sqlrcur_getRow($cur,0);
	checkSuccess($fields[0],"1");
	checkSuccess($fields[1],"1");
	checkSuccess($fields[2],"1.10");
	checkSuccess($fields[3],"1.10");
	checkSuccess($fields[4],"1.1000");
	checkSuccess($fields[5],"1.1000");
	checkSuccess($fields[6],"2001:01:01");
	checkSuccess($fields[7],"01:00:00");
	checkSuccess($fields[8],"testchar1                                         ");
	checkSuccess($fields[9],"testvarchar1");
	echo("\n");

	echo("FIELD LENGTHS BY ARRAY: \n");
	$fieldlens=sqlrcur_getRowLengths($cur,0);
	checkSuccess($fieldlens[0],1);
	checkSuccess($fieldlens[1],1);
	checkSuccess($fieldlens[2],4);
	checkSuccess($fieldlens[3],4);
	checkSuccess($fieldlens[4],6);
	checkSuccess($fieldlens[5],6);
	checkSuccess($fieldlens[6],10);
	checkSuccess($fieldlens[7],8);
	checkSuccess($fieldlens[8],50);
	checkSuccess($fieldlens[9],12);
	echo("\n");


	echo("FIELDS BY ASSOCIATIVE ARRAY: \n");
	$fields=sqlrcur_getRowAssoc($cur,0);
	checkSuccess($fields["TESTINTEGER"],"1");
	checkSuccess($fields["TESTSMALLINT"],"1");
	checkSuccess($fields["TESTDECIMAL"],"1.10");
	checkSuccess($fields["TESTNUMERIC"],"1.10");
	checkSuccess($fields["TESTFLOAT"],"1.1000");
	checkSuccess($fields["TESTDOUBLE"],"1.1000");
	checkSuccess($fields["TESTDATE"],"2001:01:01");
	checkSuccess($fields["TESTTIME"],"01:00:00");
	checkSuccess($fields["TESTCHAR"],"testchar1                                         ");
	checkSuccess($fields["TESTVARCHAR"],"testvarchar1");
	echo("\n");
	$fields=sqlrcur_getRowAssoc($cur,7);
	checkSuccess($fields["TESTINTEGER"],"8");
	checkSuccess($fields["TESTSMALLINT"],"8");
	checkSuccess($fields["TESTDECIMAL"],"8.80");
	checkSuccess($fields["TESTNUMERIC"],"8.80");
	checkSuccess($fields["TESTFLOAT"],"8.8000");
	checkSuccess($fields["TESTDOUBLE"],"8.8000");
	checkSuccess($fields["TESTDATE"],"2008:01:01");
	checkSuccess($fields["TESTTIME"],"08:00:00");
	checkSuccess($fields["TESTCHAR"],"testchar8                                         ");
	checkSuccess($fields["TESTVARCHAR"],"testvarchar8");
	echo("\n");

	echo("FIELD LENGTHS BY ASSOCIATIVE ARRAY: \n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,0);
	checkSuccess($fieldlengths["TESTINTEGER"],1);
	checkSuccess($fieldlengths["TESTSMALLINT"],1);
	checkSuccess($fieldlengths["TESTDECIMAL"],4);
	checkSuccess($fieldlengths["TESTNUMERIC"],4);
	checkSuccess($fieldlengths["TESTFLOAT"],6);
	checkSuccess($fieldlengths["TESTDOUBLE"],6);
	checkSuccess($fieldlengths["TESTDATE"],10);
	checkSuccess($fieldlengths["TESTTIME"],8);
	checkSuccess($fieldlengths["TESTCHAR"],50);
	checkSuccess($fieldlengths["TESTVARCHAR"],12);
	echo("\n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,7);
	checkSuccess($fieldlengths["TESTINTEGER"],1);
	checkSuccess($fieldlengths["TESTSMALLINT"],1);
	checkSuccess($fieldlengths["TESTDECIMAL"],4);
	checkSuccess($fieldlengths["TESTNUMERIC"],4);
	checkSuccess($fieldlengths["TESTFLOAT"],6);
	checkSuccess($fieldlengths["TESTDOUBLE"],6);
	checkSuccess($fieldlengths["TESTDATE"],10);
	checkSuccess($fieldlengths["TESTTIME"],8);
	checkSuccess($fieldlengths["TESTCHAR"],50);
	checkSuccess($fieldlengths["TESTVARCHAR"],12);
	echo("\n");


	echo("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery($cur,"select $(var1),'$(var2)',$(var3) from rdb\$database");
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

	echo("ARRAY SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery($cur,"select $(var1),'$(var2)',$(var3) from rdb\$database");
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
	checkSuccess(sqlrcur_sendQuery($cur,"select 1,NULL,NULL from rdb\$database"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),NULL);
	checkSuccess(sqlrcur_getField($cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select 1,NULL,NULL from rdb\$database"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"");
	checkSuccess(sqlrcur_getField($cur,0,2),"");
	sqlrcur_getNullsAsNulls($cur);
	echo("\n");

	echo("RESULT SET BUFFER SIZE: \n");
	checkSuccess(sqlrcur_getResultSetBufferSize($cur),0);
	sqlrcur_setResultSetBufferSize($cur,2);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
	checkSuccess(sqlrcur_getColumnName($cur,0),NULL);
	checkSuccess(sqlrcur_getColumnLength($cur,0),0);
	checkSuccess(sqlrcur_getColumnType($cur,0),NULL);
	sqlrcur_getColumnInfo($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
	checkSuccess(sqlrcur_getColumnName($cur,0),"TESTINTEGER");
	checkSuccess(sqlrcur_getColumnLength($cur,0),4);
	checkSuccess(sqlrcur_getColumnType($cur,0),"INTEGER");
	echo("\n");

	echo("SUSPENDED SESSION: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
	$filename=sqlrcur_getCacheFileName($cur);
	checkSuccess($filename,"cachefile1");
	sqlrcur_cacheOff($cur);
	checkSuccess(sqlrcur_openCachedResultSet($cur,$filename),1);
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	echo("\n");

	echo("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_colCount($cur),12);
	echo("\n");

	echo("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"TESTINTEGER");
	checkSuccess(sqlrcur_getColumnName($cur,1),"TESTSMALLINT");
	checkSuccess(sqlrcur_getColumnName($cur,2),"TESTDECIMAL");
	checkSuccess(sqlrcur_getColumnName($cur,3),"TESTNUMERIC");
	checkSuccess(sqlrcur_getColumnName($cur,4),"TESTFLOAT");
	checkSuccess(sqlrcur_getColumnName($cur,5),"TESTDOUBLE");
	checkSuccess(sqlrcur_getColumnName($cur,6),"TESTDATE");
	checkSuccess(sqlrcur_getColumnName($cur,7),"TESTTIME");
	checkSuccess(sqlrcur_getColumnName($cur,8),"TESTCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,9),"TESTVARCHAR");
	checkSuccess(sqlrcur_getColumnName($cur,10),"TESTTIMESTAMP");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"TESTINTEGER");
	checkSuccess($cols[1],"TESTSMALLINT");
	checkSuccess($cols[2],"TESTDECIMAL");
	checkSuccess($cols[3],"TESTNUMERIC");
	checkSuccess($cols[4],"TESTFLOAT");
	checkSuccess($cols[5],"TESTDOUBLE");
	checkSuccess($cols[6],"TESTDATE");
	checkSuccess($cols[7],"TESTTIME");
	checkSuccess($cols[8],"TESTCHAR");
	checkSuccess($cols[9],"TESTVARCHAR");
	checkSuccess($cols[10],"TESTTIMESTAMP");
	echo("\n");

	echo("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	sqlrcur_setResultSetBufferSize($cur,2);
	sqlrcur_cacheToFile($cur,"cachefile1");
	sqlrcur_setCacheTtl($cur,200);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
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
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testinteger"),1);
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

	#echo("COMMIT AND ROLLBACK: \n");
	$secondcon=sqlrcon_alloc($host,
				$port, 
				$socket,$user,$password,0,1);
	$secondcur=sqlrcur_alloc($secondcon);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"0");
	checkSuccess(sqlrcon_commit($con),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"8");
	checkSuccess(sqlrcon_autoCommitOn($con),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (10,10,10.1,10.1,10.1,10.1,'01-JAN-2010','10:00:00','testchar10','testvarchar10',NULL,NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"9");
	checkSuccess(sqlrcon_autoCommitOff($con),1);
	echo("\n");

	# drop existing table
	sqlrcon_commit($con);
	sqlrcur_sendQuery($cur,"delete from testtable");
	sqlrcon_commit($con);
	echo("\n");

	# invalid queries...
	echo("INVALID QUERIES: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1 order by testinteger"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1 order by testinteger"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1 order by testinteger"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable1 order by testinteger"),0);
	echo("\n");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable1 values (1,2,3,4)"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable1 values (1,2,3,4)"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable1 values (1,2,3,4)"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable1 values (1,2,3,4)"),0);
	echo("\n");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable"),0);
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable"),0);
	echo("\n");

?></pre></html>
