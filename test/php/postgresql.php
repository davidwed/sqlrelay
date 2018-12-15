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

	echo("IDENTIFY: \n");
	checkSuccess(sqlrcon_identify($con),"postgresql");
	echo("\n");

	# ping
	echo("PING: \n");
	checkSuccess(sqlrcon_ping($con),1);
	echo("\n");

	# drop existing table
	sqlrcur_sendQuery($cur,"drop table testtable");

	echo("CREATE TEMPTABLE: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"create table testtable (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
	echo("\n");

	echo("BEGIN TRANSCTION: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"begin"),1);
	echo("\n");

	echo("INSERT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (3,3.3,3.3,3,'testchar3','testvarchar3','01/01/2003','03:00:00',NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (4,4.4,4.4,4,'testchar4','testvarchar4','01/01/2004','04:00:00',NULL)"),1);
	echo("\n");

	echo("AFFECTED ROWS: \n");
	checkSuccess(sqlrcur_affectedRows($cur),1);
	echo("\n");

	echo("BIND BY NAME: \n");
	sqlrcur_prepareQuery($cur,"insert into testtable values (\$1,\$2,\$3,\$4,\$5,\$6,\$7,\$8)");
	checkSuccess(sqlrcur_countBindVariables($cur),8);
	sqlrcur_inputBind($cur,"1",5);
	sqlrcur_inputBind($cur,"2",5.5,4,2);
	sqlrcur_inputBind($cur,"3",5.5,4,2);
	sqlrcur_inputBind($cur,"4",5);
	sqlrcur_inputBind($cur,"5","testchar5");
	sqlrcur_inputBind($cur,"6","testvarchar5");
	sqlrcur_inputBind($cur,"7","01/01/2005");
	sqlrcur_inputBind($cur,"8","05:00:00");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"1",6);
	sqlrcur_inputBind($cur,"2",6.6,4,2);
	sqlrcur_inputBind($cur,"3",6.6,4,2);
	sqlrcur_inputBind($cur,"4",6);
	sqlrcur_inputBind($cur,"5","testchar6");
	sqlrcur_inputBind($cur,"6","testvarchar6");
	sqlrcur_inputBind($cur,"7","01/01/2006");
	sqlrcur_inputBind($cur,"8","06:00:00");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("ARRAY BIND BY NAME: \n");
	sqlrcur_clearBinds($cur);
	$bindvars=array("1","2","3","4","5","6","7","8");
	$bindvals=array(7,7.7,7.7,7,"testchar7","testvarchar7","01/01/2007","07:00:00");
	$precs=array(0,2,2,0,0,0,0,0);
	$scales=array(0,1,1,0,0,0,0,0);
	sqlrcur_inputBinds($cur,$bindvars,$bindvals,$precs,$scales);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("BIND BY NAME WITH VALIDATION: \n");
	sqlrcur_clearBinds($cur);
	sqlrcur_inputBind($cur,"1",8);
	sqlrcur_inputBind($cur,"2",8.8,4,2);
	sqlrcur_inputBind($cur,"3",8.8,4,2);
	sqlrcur_inputBind($cur,"4",8);
	sqlrcur_inputBind($cur,"5","testchar8");
	sqlrcur_inputBind($cur,"6","testvarchar8");
	sqlrcur_inputBind($cur,"7","01/01/2008");
	sqlrcur_inputBind($cur,"8","08:00:00");
	sqlrcur_validateBinds($cur);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	echo("\n");

	echo("SELECT: \n");
	checkSuccess(sqlrcur_sendQuery($cur,"select * from testtable order by testint"),1);
	echo("\n");

	echo("COLUMN COUNT: \n");
	checkSuccess(sqlrcur_colCount($cur),9);
	echo("\n");

	echo("COLUMN NAMES: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"testint");
	checkSuccess(sqlrcur_getColumnName($cur,1),"testfloat");
	checkSuccess(sqlrcur_getColumnName($cur,2),"testreal");
	checkSuccess(sqlrcur_getColumnName($cur,3),"testsmallint");
	checkSuccess(sqlrcur_getColumnName($cur,4),"testchar");
	checkSuccess(sqlrcur_getColumnName($cur,5),"testvarchar");
	checkSuccess(sqlrcur_getColumnName($cur,6),"testdate");
	checkSuccess(sqlrcur_getColumnName($cur,7),"testtime");
	checkSuccess(sqlrcur_getColumnName($cur,8),"testtimestamp");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"testint");
	checkSuccess($cols[1],"testfloat");
	checkSuccess($cols[2],"testreal");
	checkSuccess($cols[3],"testsmallint");
	checkSuccess($cols[4],"testchar");
	checkSuccess($cols[5],"testvarchar");
	checkSuccess($cols[6],"testdate");
	checkSuccess($cols[7],"testtime");
	checkSuccess($cols[8],"testtimestamp");
	echo("\n");

	echo("COLUMN TYPES: \n");
	checkSuccess(sqlrcur_getColumnType($cur,0),"int4");
	checkSuccess(sqlrcur_getColumnType($cur,"testint"),"int4");
	checkSuccess(sqlrcur_getColumnType($cur,1),"float8");
	checkSuccess(sqlrcur_getColumnType($cur,"testfloat"),"float8");
	checkSuccess(sqlrcur_getColumnType($cur,2),"float4");
	checkSuccess(sqlrcur_getColumnType($cur,"testreal"),"float4");
	checkSuccess(sqlrcur_getColumnType($cur,3),"int2");
	checkSuccess(sqlrcur_getColumnType($cur,"testsmallint"),"int2");
	checkSuccess(sqlrcur_getColumnType($cur,4),"bpchar");
	checkSuccess(sqlrcur_getColumnType($cur,"testchar"),"bpchar");
	checkSuccess(sqlrcur_getColumnType($cur,5),"varchar");
	checkSuccess(sqlrcur_getColumnType($cur,"testvarchar"),"varchar");
	checkSuccess(sqlrcur_getColumnType($cur,6),"date");
	checkSuccess(sqlrcur_getColumnType($cur,"testdate"),"date");
	checkSuccess(sqlrcur_getColumnType($cur,7),"time");
	checkSuccess(sqlrcur_getColumnType($cur,"testtime"),"time");
	checkSuccess(sqlrcur_getColumnType($cur,8),"timestamp");
	checkSuccess(sqlrcur_getColumnType($cur,"testtimestamp"),"timestamp");
	echo("\n");

	echo("COLUMN LENGTH: \n");
	checkSuccess(sqlrcur_getColumnLength($cur,0),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"testint"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,1),8);
	checkSuccess(sqlrcur_getColumnLength($cur,"testfloat"),8);
	checkSuccess(sqlrcur_getColumnLength($cur,2),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"testreal"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,3),2);
	checkSuccess(sqlrcur_getColumnLength($cur,"testsmallint"),2);
	checkSuccess(sqlrcur_getColumnLength($cur,4),44);
	checkSuccess(sqlrcur_getColumnLength($cur,"testchar"),44);
	checkSuccess(sqlrcur_getColumnLength($cur,5),44);
	checkSuccess(sqlrcur_getColumnLength($cur,"testvarchar"),44);
	checkSuccess(sqlrcur_getColumnLength($cur,6),4);
	checkSuccess(sqlrcur_getColumnLength($cur,"testdate"),4);
	checkSuccess(sqlrcur_getColumnLength($cur,7),8);
	checkSuccess(sqlrcur_getColumnLength($cur,"testtime"),8);
	checkSuccess(sqlrcur_getColumnLength($cur,8),8);
	checkSuccess(sqlrcur_getColumnLength($cur,"testtimestamp"),8);
	echo("\n");

	echo("LONGEST COLUMN: \n");
	checkSuccess(sqlrcur_getLongest($cur,0),1);
	checkSuccess(sqlrcur_getLongest($cur,"testint"),1);
	checkSuccess(sqlrcur_getLongest($cur,1),3);
	checkSuccess(sqlrcur_getLongest($cur,"testfloat"),3);
	checkSuccess(sqlrcur_getLongest($cur,2),3);
	checkSuccess(sqlrcur_getLongest($cur,"testreal"),3);
	checkSuccess(sqlrcur_getLongest($cur,3),1);
	checkSuccess(sqlrcur_getLongest($cur,"testsmallint"),1);
	checkSuccess(sqlrcur_getLongest($cur,4),40);
	checkSuccess(sqlrcur_getLongest($cur,"testchar"),40);
	checkSuccess(sqlrcur_getLongest($cur,5),12);
	checkSuccess(sqlrcur_getLongest($cur,"testvarchar"),12);
	checkSuccess(sqlrcur_getLongest($cur,6),10);
	checkSuccess(sqlrcur_getLongest($cur,"testdate"),10);
	checkSuccess(sqlrcur_getLongest($cur,7),8);
	checkSuccess(sqlrcur_getLongest($cur,"testtime"),8);
	echo("\n");

	echo("ROW COUNT: \n");
	checkSuccess(sqlrcur_rowCount($cur),8);
	echo("\n");

	/*echo("TOTAL ROWS: \n");
	checkSuccess(sqlrcur_totalRows($cur),8);
	echo("\n");*/

	echo("FIRST ROW INDEX: \n");
	checkSuccess(sqlrcur_firstRowIndex($cur),0);
	echo("\n");

	echo("END OF RESULT SET: \n");
	checkSuccess(sqlrcur_endOfResultSet($cur),1);
	echo("\n");

	echo("FIELDS BY INDEX: \n");
	checkSuccess(sqlrcur_getField($cur,0,0),"1");
	checkSuccess(sqlrcur_getField($cur,0,1),"1.1");
	checkSuccess(sqlrcur_getField($cur,0,2),"1.1");
	checkSuccess(sqlrcur_getField($cur,0,3),"1");
	checkSuccess(sqlrcur_getField($cur,0,4),"testchar1                               ");
	checkSuccess(sqlrcur_getField($cur,0,5),"testvarchar1");
	checkSuccess(sqlrcur_getField($cur,0,6),"2001-01-01");
	checkSuccess(sqlrcur_getField($cur,0,7),"01:00:00");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,0),"8");
	checkSuccess(sqlrcur_getField($cur,7,1),"8.8");
	checkSuccess(sqlrcur_getField($cur,7,2),"8.8");
	checkSuccess(sqlrcur_getField($cur,7,3),"8");
	checkSuccess(sqlrcur_getField($cur,7,4),"testchar8                               ");
	checkSuccess(sqlrcur_getField($cur,7,5),"testvarchar8");
	checkSuccess(sqlrcur_getField($cur,7,6),"2008-01-01");
	checkSuccess(sqlrcur_getField($cur,7,7),"08:00:00");
	echo("\n");

	echo("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,1),3);
	checkSuccess(sqlrcur_getFieldLength($cur,0,2),3);
	checkSuccess(sqlrcur_getFieldLength($cur,0,3),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,4),40);
	checkSuccess(sqlrcur_getFieldLength($cur,0,5),12);
	checkSuccess(sqlrcur_getFieldLength($cur,0,6),10);
	checkSuccess(sqlrcur_getFieldLength($cur,0,7),8);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,0),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,1),3);
	checkSuccess(sqlrcur_getFieldLength($cur,7,2),3);
	checkSuccess(sqlrcur_getFieldLength($cur,7,3),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,4),40);
	checkSuccess(sqlrcur_getFieldLength($cur,7,5),12);
	checkSuccess(sqlrcur_getFieldLength($cur,7,6),10);
	checkSuccess(sqlrcur_getFieldLength($cur,7,7),8);
	echo("\n");

	echo("FIELDS BY NAME: \n");
	checkSuccess(sqlrcur_getField($cur,0,"testint"),"1");
	checkSuccess(sqlrcur_getField($cur,0,"testfloat"),"1.1");
	checkSuccess(sqlrcur_getField($cur,0,"testreal"),"1.1");
	checkSuccess(sqlrcur_getField($cur,0,"testsmallint"),"1");
	checkSuccess(sqlrcur_getField($cur,0,"testchar"),"testchar1                               ");
	checkSuccess(sqlrcur_getField($cur,0,"testvarchar"),"testvarchar1");
	checkSuccess(sqlrcur_getField($cur,0,"testdate"),"2001-01-01");
	checkSuccess(sqlrcur_getField($cur,0,"testtime"),"01:00:00");
	echo("\n");
	checkSuccess(sqlrcur_getField($cur,7,"testint"),"8");
	checkSuccess(sqlrcur_getField($cur,7,"testfloat"),"8.8");
	checkSuccess(sqlrcur_getField($cur,7,"testreal"),"8.8");
	checkSuccess(sqlrcur_getField($cur,7,"testsmallint"),"8");
	checkSuccess(sqlrcur_getField($cur,7,"testchar"),"testchar8                               ");
	checkSuccess(sqlrcur_getField($cur,7,"testvarchar"),"testvarchar8");
	checkSuccess(sqlrcur_getField($cur,7,"testdate"),"2008-01-01");
	checkSuccess(sqlrcur_getField($cur,7,"testtime"),"08:00:00");
	echo("\n");

	echo("FIELD LENGTHS BY NAME: \n");
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testint"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testfloat"),3);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testreal"),3);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testsmallint"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testchar"),40);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testvarchar"),12);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testdate"),10);
	checkSuccess(sqlrcur_getFieldLength($cur,0,"testtime"),8);
	echo("\n");
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testint"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testfloat"),3);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testreal"),3);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testsmallint"),1);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testchar"),40);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testvarchar"),12);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testdate"),10);
	checkSuccess(sqlrcur_getFieldLength($cur,7,"testtime"),8);
	echo("\n");

	echo("FIELDS BY ARRAY: \n");
	$fields=sqlrcur_getRow($cur,0);
	checkSuccess($fields[0],"1");
	checkSuccess($fields[1],"1.1");
	checkSuccess($fields[2],"1.1");
	checkSuccess($fields[3],"1");
	checkSuccess($fields[4],"testchar1                               ");
	checkSuccess($fields[5],"testvarchar1");
	checkSuccess($fields[6],"2001-01-01");
	checkSuccess($fields[7],"01:00:00");
	echo("\n");

	echo("FIELD LENGTHS BY ARRAY: \n");
	$fieldlens=sqlrcur_getRowLengths($cur,0);
	checkSuccess($fieldlens[0],1);
	checkSuccess($fieldlens[1],3);
	checkSuccess($fieldlens[2],3);
	checkSuccess($fieldlens[3],1);
	checkSuccess($fieldlens[4],40);
	checkSuccess($fieldlens[5],12);
	checkSuccess($fieldlens[6],10);
	checkSuccess($fieldlens[7],8);
	echo("\n");

	echo("FIELDS BY ASSOCIATIVE ARRAY: \n");
	$fields=sqlrcur_getRowAssoc($cur,0);
	checkSuccess($fields["testint"],"1");
	checkSuccess($fields["testfloat"],"1.1");
	checkSuccess($fields["testreal"],"1.1");
	checkSuccess($fields["testsmallint"],"1");
	checkSuccess($fields["testchar"],"testchar1                               ");
	checkSuccess($fields["testvarchar"],"testvarchar1");
	checkSuccess($fields["testdate"],"2001-01-01");
	checkSuccess($fields["testtime"],"01:00:00");
	echo("\n");
	$fields=sqlrcur_getRowAssoc($cur,7);
	checkSuccess($fields["testint"],"8");
	checkSuccess($fields["testfloat"],"8.8");
	checkSuccess($fields["testreal"],"8.8");
	checkSuccess($fields["testsmallint"],"8");
	checkSuccess($fields["testchar"],"testchar8                               ");
	checkSuccess($fields["testvarchar"],"testvarchar8");
	checkSuccess($fields["testdate"],"2008-01-01");
	checkSuccess($fields["testtime"],"08:00:00");
	echo("\n");

	echo("FIELD LENGTHS BY ASSOCIATIVE ARRAY: \n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,0);
	checkSuccess($fieldlengths["testint"],1);
	checkSuccess($fieldlengths["testfloat"],3);
	checkSuccess($fieldlengths["testreal"],3);
	checkSuccess($fieldlengths["testsmallint"],1);
	checkSuccess($fieldlengths["testchar"],40);
	checkSuccess($fieldlengths["testvarchar"],12);
	checkSuccess($fieldlengths["testdate"],10);
	checkSuccess($fieldlengths["testtime"],8);
	echo("\n");
	$fieldlengths=sqlrcur_getRowLengthsAssoc($cur,7);
	checkSuccess($fieldlengths["testint"],1);
	checkSuccess($fieldlengths["testfloat"],3);
	checkSuccess($fieldlengths["testreal"],3);
	checkSuccess($fieldlengths["testsmallint"],1);
	checkSuccess($fieldlengths["testchar"],40);
	checkSuccess($fieldlengths["testvarchar"],12);
	checkSuccess($fieldlengths["testdate"],10);
	checkSuccess($fieldlengths["testtime"],8);
	echo("\n");

	echo("INDIVIDUAL SUBSTITUTIONS: \n");
	sqlrcur_prepareQuery($cur,"select $(var1),'$(var2)',$(var3)");
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
	sqlrcur_prepareQuery($cur,"select $(var1),'$(var2)',$(var3)");
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
	checkSuccess(sqlrcur_sendQuery($cur,"select NULL,1,NULL"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),NULL);
	checkSuccess(sqlrcur_getField($cur,0,1),"1");
	checkSuccess(sqlrcur_getField($cur,0,2),NULL);
	sqlrcur_getNullsAsEmptyStrings($cur);
	checkSuccess(sqlrcur_sendQuery($cur,"select NULL,1,NULL"),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"");
	checkSuccess(sqlrcur_getField($cur,0,1),"1");
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
	checkSuccess(sqlrcur_getColumnLength($cur,0),4);
	checkSuccess(sqlrcur_getColumnType($cur,0),"int4");
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
	checkSuccess(sqlrcur_colCount($cur),9);
	echo("\n");

	echo("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(sqlrcur_getColumnName($cur,0),"testint");
	checkSuccess(sqlrcur_getColumnName($cur,1),"testfloat");
	checkSuccess(sqlrcur_getColumnName($cur,2),"testreal");
	checkSuccess(sqlrcur_getColumnName($cur,3),"testsmallint");
	checkSuccess(sqlrcur_getColumnName($cur,4),"testchar");
	checkSuccess(sqlrcur_getColumnName($cur,5),"testvarchar");
	checkSuccess(sqlrcur_getColumnName($cur,6),"testdate");
	checkSuccess(sqlrcur_getColumnName($cur,7),"testtime");
	checkSuccess(sqlrcur_getColumnName($cur,8),"testtimestamp");
	$cols=sqlrcur_getColumnNames($cur);
	checkSuccess($cols[0],"testint");
	checkSuccess($cols[1],"testfloat");
	checkSuccess($cols[2],"testreal");
	checkSuccess($cols[3],"testsmallint");
	checkSuccess($cols[4],"testchar");
	checkSuccess($cols[5],"testvarchar");
	checkSuccess($cols[6],"testdate");
	checkSuccess($cols[7],"testtime");
	checkSuccess($cols[8],"testtimestamp");
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
	#checkSuccess(sqlrcon_autoCommitOn($con),1);
	checkSuccess(sqlrcur_sendQuery($cur,"insert into testtable values (10,10.1,10.1,10,'testchar10','testvarchar10','01/01/2010','10:00:00',NULL)"),1);
	checkSuccess(sqlrcur_sendQuery($secondcur,"select count(*) from testtable"),1);
	checkSuccess(sqlrcur_getField($secondcur,0,0),"9");
	#checkSuccess(sqlrcon_autoCommitOff($con),1);
	echo("\n");

	# stored procedures
	echo("STORED PROCEDURES: \n");
	sqlrcur_sendQuery($cur,"drop function testfunc(int)");
	checkSuccess(sqlrcur_sendQuery($cur,"create function testfunc(int) returns int as ' begin return \$1; end;' language plpgsql"),1);
	sqlrcur_prepareQuery($cur,"select * from testfunc(\$1)");
	sqlrcur_inputBind($cur,"1",5);
	checkSuccess(sqlrcur_executeQuery($cur),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"5");
	sqlrcur_sendQuery($cur,"drop function testfunc(int)");

	sqlrcur_sendQuery($cur,"drop function testfunc(int,char(20))");
	checkSuccess(sqlrcur_sendQuery($cur,"create function testfunc(int, char(20)) returns record as ' declare output record; begin select $1,$2 into output; return output; end;' language plpgsql"),1);
	sqlrcur_prepareQuery($cur,"select * from testfunc(\$1,\$2) as (col1 int, col2 bpchar)");
	sqlrcur_inputBind($cur,"1",5);
	sqlrcur_inputBind($cur,"2","hello");
	checkSuccess(sqlrcur_executeQuery($cur),1);
	checkSuccess(sqlrcur_getField($cur,0,0),"5");
	checkSuccess(sqlrcur_getField($cur,0,1),"hello");
	sqlrcur_sendQuery($cur,"drop function testfunc(int,char(20))");
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

	sqlrcur_free($secondcur);
	sqlrcon_free($secondcon);

	sqlrcur_free($cur);
	sqlrcon_free($con);
?></pre></html>
