// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

import com.firstworks.sqlrelay.SQLRConnection;
import com.firstworks.sqlrelay.SQLRCursor;


class firebird {
	
	
	
	
	
	
	private static void checkSuccess(String value, String success) {
	
		if (success==null) {
			if (value==null) {
				System.out.println("success ");
				return;
			} else {
				System.out.println("failure ");
				
				
				System.exit(0);
			}
		}
	
		if (value.equals(success)) {
			System.out.println("success ");
		} else {
			System.out.println("failure ");
			
			
			System.exit(0);
		}
	}
	
	private static void checkSuccess(long value, int success) {
	
		if (value==success) {
			System.out.println("success ");
		} else {
			System.out.println("failure ");
			
			
			System.exit(0);
		}
	}
	
	private static void checkSuccess(boolean value, int success) {
	
		if (((value)?1:0)==success) {
			System.out.println("success ");
		} else {
			System.out.println("failure ");
			
			
			System.exit(0);
		}
	}
	
	public static void	main(String[] args) {
	
		String	dbtype;
		String[]	bindvars={"1","2","3","4","5","6",
					"7","8","9","10","11"};
		String[]	bindvals={"4","4","4.4","4.4","4.4","4.4",
					"01-JAN-2004","04:00:00",
					"testchar4","testvarchar4",null};
		String[]	subvars={"var1","var2","var3"};
		String[]	subvalstrings={"hi","hello","bye"};
		long[]	subvallongs={1,2,3};
		double[]	subvaldoubles={10.55,10.556,10.5556};
		int[]	precs={4,5,6};
		int[]	scales={2,3,4};
		String	numvar;
		String	stringvar;
		String	floatvar;
		String[]	cols;
		String[]	fields;
		short	port;
		String	socket;
		short	id;
		String	filename;
		long[]	fieldlens;
	
		// instantiation
		SQLRConnection con=new SQLRConnection("localhost",
						(short)9000,
						"/tmp/test.socket",
						"test","test",0,1);
		SQLRCursor cur=new SQLRCursor(con);
	
		// get database type
		System.out.println("IDENTIFY: ");
		checkSuccess(con.identify(),"firebird");
		System.out.println();
	
		// ping
		System.out.println("PING: ");
		checkSuccess(con.ping(),1);
		System.out.println();
	
		// clear table
		cur.sendQuery("delete from testtable");
		con.commit();
	
		System.out.println("INSERT: ");
		checkSuccess(cur.sendQuery("insert into testtable values (1,1,1.1,1.1,1.1,1.1,'01-JAN-2001','01:00:00','testchar1','testvarchar1',null)"),1);
		System.out.println();
	
	
		System.out.println("BIND BY POSITION: ");

		cur.prepareQuery("insert into testtable values (?,?,?,?,?,?,?,?,?,?,?)");
		checkSuccess(cur.countBindVariables(),11);
		cur.inputBind("1",2);
		cur.inputBind("2",2);
		cur.inputBind("3",2.2,2,1);
		cur.inputBind("4",2.2,2,1);
		cur.inputBind("5",2.2,2,1);
		cur.inputBind("6",2.2,2,1);
		cur.inputBind("7","01-JAN-2002");
		cur.inputBind("8","02:00:00");
		cur.inputBind("9","testchar2");
		cur.inputBind("10","testvarchar2");
		cur.inputBind("11",null);
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("1",3);
		cur.inputBind("2",3);
		cur.inputBind("3",3.3,2,1);
		cur.inputBind("4",3.3,2,1);
		cur.inputBind("5",3.3,2,1);
		cur.inputBind("6",3.3,2,1);
		cur.inputBind("7","01-JAN-2003");
		cur.inputBind("8","03:00:00");
		cur.inputBind("9","testchar3");
		cur.inputBind("10","testvarchar3");
		cur.inputBind("11",null);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("ARRAY OF BINDS BY POSITION: ");
		cur.clearBinds();
		cur.inputBinds(bindvars,bindvals);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("INSERT: ");
		checkSuccess(cur.sendQuery("insert into testtable values (5,5,5.5,5.5,5.5,5.5,'01-JAN-2005','05:00:00','testchar5','testvarchar5',null)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (6,6,6.6,6.6,6.6,6.6,'01-JAN-2006','06:00:00','testchar6','testvarchar6',null)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (7,7,7.7,7.7,7.7,7.7,'01-JAN-2007','07:00:00','testchar7','testvarchar7',null)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (8,8,8.8,8.8,8.8,8.8,'01-JAN-2008','08:00:00','testchar8','testvarchar8',null)"),1);
		System.out.println();
	
		System.out.println("AFFECTED ROWS: ");
		checkSuccess(cur.affectedRows(),0);
		System.out.println();

	    System.out.println("STORED PROCEDURE: ");
	    checkSuccess(cur.sendQuery("create procedure testproc(invar integer) returns (outvar integer) as begin outvar = invar; suspend; end"),1);
	    cur.prepareQuery("select * from testproc(?)");
	    cur.inputBind("1",5);
	    checkSuccess(cur.executeQuery(),1);
	    checkSuccess(cur.getField(0,0),"5");
	    checkSuccess(cur.sendQuery("drop procedure testproc"),1);
	    System.out.println();
	
		System.out.println("SELECT: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		System.out.println();
	
		System.out.println("COLUMN COUNT: ");
		checkSuccess(cur.colCount(),11);
		System.out.println();
	
		System.out.println("COLUMN NAMES: ");
		checkSuccess(cur.getColumnName(0),"TESTINTEGER");
		checkSuccess(cur.getColumnName(1),"TESTSMALLINT");
		checkSuccess(cur.getColumnName(2),"TESTDECIMAL");
		checkSuccess(cur.getColumnName(3),"TESTNUMERIC");
		checkSuccess(cur.getColumnName(4),"TESTFLOAT");
		checkSuccess(cur.getColumnName(5),"TESTDOUBLE");
		checkSuccess(cur.getColumnName(6),"TESTDATE");
		checkSuccess(cur.getColumnName(7),"TESTTIME");
		checkSuccess(cur.getColumnName(8),"TESTCHAR");
		checkSuccess(cur.getColumnName(9),"TESTVARCHAR");
		checkSuccess(cur.getColumnName(10),"TESTTIMESTAMP");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"TESTINTEGER");
		checkSuccess(cols[1],"TESTSMALLINT");
		checkSuccess(cols[2],"TESTDECIMAL");
		checkSuccess(cols[3],"TESTNUMERIC");
		checkSuccess(cols[4],"TESTFLOAT");
		checkSuccess(cols[5],"TESTDOUBLE");
		checkSuccess(cols[6],"TESTDATE");
		checkSuccess(cols[7],"TESTTIME");
		checkSuccess(cols[8],"TESTCHAR");
		checkSuccess(cols[9],"TESTVARCHAR");
		checkSuccess(cols[10],"TESTTIMESTAMP");
		System.out.println();
	
		System.out.println("COLUMN TYPES: ");
		checkSuccess(cur.getColumnType(0),"INTEGER");
		checkSuccess(cur.getColumnType("TESTINTEGER"),"INTEGER");
		checkSuccess(cur.getColumnType(1),"SMALLINT");
		checkSuccess(cur.getColumnType("TESTSMALLINT"),"SMALLINT");
		checkSuccess(cur.getColumnType(2),"DECIMAL");
		checkSuccess(cur.getColumnType("TESTDECIMAL"),"DECIMAL");
		checkSuccess(cur.getColumnType(3),"NUMERIC");
		checkSuccess(cur.getColumnType("TESTNUMERIC"),"NUMERIC");
		checkSuccess(cur.getColumnType(4),"FLOAT");
		checkSuccess(cur.getColumnType("TESTFLOAT"),"FLOAT");
		checkSuccess(cur.getColumnType(5),"DOUBLE PRECISION");
		checkSuccess(cur.getColumnType("TESTDOUBLE"),"DOUBLE PRECISION");
		checkSuccess(cur.getColumnType(6),"DATE");
		checkSuccess(cur.getColumnType("TESTDATE"),"DATE");
		checkSuccess(cur.getColumnType(7),"TIME");
		checkSuccess(cur.getColumnType("TESTTIME"),"TIME");
		checkSuccess(cur.getColumnType(8),"CHAR");
		checkSuccess(cur.getColumnType("TESTCHAR"),"CHAR");
		checkSuccess(cur.getColumnType(9),"VARCHAR");
		checkSuccess(cur.getColumnType("TESTVARCHAR"),"VARCHAR");
		checkSuccess(cur.getColumnType(10),"TIMESTAMP");
		checkSuccess(cur.getColumnType("TESTTIMESTAMP"),"TIMESTAMP");
		System.out.println();
	
		System.out.println("COLUMN LENGTH: ");
		checkSuccess(cur.getColumnLength(0),4);
		checkSuccess(cur.getColumnLength("TESTINTEGER"),4);
		checkSuccess(cur.getColumnLength(1),2);
		checkSuccess(cur.getColumnLength("TESTSMALLINT"),2);
		checkSuccess(cur.getColumnLength(2),8);
		checkSuccess(cur.getColumnLength("TESTDECIMAL"),8);
		checkSuccess(cur.getColumnLength(3),8);
		checkSuccess(cur.getColumnLength("TESTNUMERIC"),8);
		checkSuccess(cur.getColumnLength(4),4);
		checkSuccess(cur.getColumnLength("TESTFLOAT"),4);
		checkSuccess(cur.getColumnLength(5),8);
		checkSuccess(cur.getColumnLength("TESTDOUBLE"),8);
		checkSuccess(cur.getColumnLength(6),4);
		checkSuccess(cur.getColumnLength("TESTDATE"),4);
		checkSuccess(cur.getColumnLength(7),4);
		checkSuccess(cur.getColumnLength("TESTTIME"),4);
		checkSuccess(cur.getColumnLength(8),50);
		checkSuccess(cur.getColumnLength("TESTCHAR"),50);
		checkSuccess(cur.getColumnLength(9),50);
		checkSuccess(cur.getColumnLength("TESTVARCHAR"),50);
		checkSuccess(cur.getColumnLength(10),8);
		checkSuccess(cur.getColumnLength("TESTTIMESTAMP"),8);
		System.out.println();
	
		System.out.println("LONGEST COLUMN: ");
		checkSuccess(cur.getLongest(0),1);
		checkSuccess(cur.getLongest("TESTINTEGER"),1);
		checkSuccess(cur.getLongest(1),1);
		checkSuccess(cur.getLongest("TESTSMALLINT"),1);
		checkSuccess(cur.getLongest(2),4);
		checkSuccess(cur.getLongest("TESTDECIMAL"),4);
		checkSuccess(cur.getLongest(3),4);
		checkSuccess(cur.getLongest("TESTNUMERIC"),4);
		checkSuccess(cur.getLongest(4),6);
		checkSuccess(cur.getLongest("TESTFLOAT"),6);
		checkSuccess(cur.getLongest(5),6);
		checkSuccess(cur.getLongest("TESTDOUBLE"),6);
		checkSuccess(cur.getLongest(6),10);
		checkSuccess(cur.getLongest("TESTDATE"),10);
		checkSuccess(cur.getLongest(7),8);
		checkSuccess(cur.getLongest("TESTTIME"),8);
		checkSuccess(cur.getLongest(8),50);
		checkSuccess(cur.getLongest("TESTCHAR"),50);
		checkSuccess(cur.getLongest(9),12);
		checkSuccess(cur.getLongest("TESTVARCHAR"),12);
		checkSuccess(cur.getLongest(10),0);
		checkSuccess(cur.getLongest("TESTTIMESTAMP"),0);
		System.out.println();
	
		System.out.println("ROW COUNT: ");
		checkSuccess(cur.rowCount(),8);
		System.out.println();
	
		System.out.println("TOTAL ROWS: ");
		checkSuccess(cur.totalRows(),0);
		System.out.println();
	
		System.out.println("FIRST ROW INDEX: ");
		checkSuccess(cur.firstRowIndex(),0);
		System.out.println();
	
		System.out.println("END OF RESULT SET: ");
		checkSuccess(cur.endOfResultSet(),1);
		System.out.println();
	
		System.out.println("FIELDS BY INDEX: ");
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"1");
		checkSuccess(cur.getField(0,2),"1.10");
		checkSuccess(cur.getField(0,3),"1.10");
		checkSuccess(cur.getField(0,4),"1.1000");
		checkSuccess(cur.getField(0,5),"1.1000");
		checkSuccess(cur.getField(0,6),"2001:01:01");
		checkSuccess(cur.getField(0,7),"01:00:00");
		checkSuccess(cur.getField(0,8),"testchar1                                         ");
		checkSuccess(cur.getField(0,9),"testvarchar1");
		System.out.println();
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(7,1),"8");
		checkSuccess(cur.getField(7,2),"8.80");
		checkSuccess(cur.getField(7,3),"8.80");
		checkSuccess(cur.getField(7,4),"8.8000");
		checkSuccess(cur.getField(7,5),"8.8000");
		checkSuccess(cur.getField(7,6),"2008:01:01");
		checkSuccess(cur.getField(7,7),"08:00:00");
		checkSuccess(cur.getField(7,8),"testchar8                                         ");
		checkSuccess(cur.getField(7,9),"testvarchar8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY INDEX: ");
		checkSuccess(cur.getFieldLength(0,0),1);
		checkSuccess(cur.getFieldLength(0,1),1);
		checkSuccess(cur.getFieldLength(0,2),4);
		checkSuccess(cur.getFieldLength(0,3),4);
		checkSuccess(cur.getFieldLength(0,4),6);
		checkSuccess(cur.getFieldLength(0,5),6);
		checkSuccess(cur.getFieldLength(0,6),10);
		checkSuccess(cur.getFieldLength(0,7),8);
		checkSuccess(cur.getFieldLength(0,8),50);
		checkSuccess(cur.getFieldLength(0,9),12);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,0),1);
		checkSuccess(cur.getFieldLength(7,1),1);
		checkSuccess(cur.getFieldLength(7,2),4);
		checkSuccess(cur.getFieldLength(7,3),4);
		checkSuccess(cur.getFieldLength(7,4),6);
		checkSuccess(cur.getFieldLength(7,5),6);
		checkSuccess(cur.getFieldLength(7,6),10);
		checkSuccess(cur.getFieldLength(7,7),8);
		checkSuccess(cur.getFieldLength(7,8),50);
		checkSuccess(cur.getFieldLength(7,9),12);
		System.out.println();
	
		System.out.println("FIELDS BY NAME: ");
		checkSuccess(cur.getField(0,"TESTINTEGER"),"1");
		checkSuccess(cur.getField(0,"TESTSMALLINT"),"1");
		checkSuccess(cur.getField(0,"TESTDECIMAL"),"1.10");
		checkSuccess(cur.getField(0,"TESTNUMERIC"),"1.10");
		checkSuccess(cur.getField(0,"TESTFLOAT"),"1.1000");
		checkSuccess(cur.getField(0,"TESTDOUBLE"),"1.1000");
		checkSuccess(cur.getField(0,"TESTDATE"),"2001:01:01");
		checkSuccess(cur.getField(0,"TESTTIME"),"01:00:00");
		checkSuccess(cur.getField(0,"TESTCHAR"),"testchar1                                         ");
		checkSuccess(cur.getField(0,"TESTVARCHAR"),"testvarchar1");
		System.out.println();
		checkSuccess(cur.getField(7,"TESTINTEGER"),"8");
		checkSuccess(cur.getField(7,"TESTSMALLINT"),"8");
		checkSuccess(cur.getField(7,"TESTDECIMAL"),"8.80");
		checkSuccess(cur.getField(7,"TESTNUMERIC"),"8.80");
		checkSuccess(cur.getField(7,"TESTFLOAT"),"8.8000");
		checkSuccess(cur.getField(7,"TESTDOUBLE"),"8.8000");
		checkSuccess(cur.getField(7,"TESTDATE"),"2008:01:01");
		checkSuccess(cur.getField(7,"TESTTIME"),"08:00:00");
		checkSuccess(cur.getField(7,"TESTCHAR"),"testchar8                                         ");
		checkSuccess(cur.getField(7,"TESTVARCHAR"),"testvarchar8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY NAME: ");
		checkSuccess(cur.getFieldLength(0,"TESTINTEGER"),1);
		checkSuccess(cur.getFieldLength(0,"TESTSMALLINT"),1);
		checkSuccess(cur.getFieldLength(0,"TESTDECIMAL"),4);
		checkSuccess(cur.getFieldLength(0,"TESTNUMERIC"),4);
		checkSuccess(cur.getFieldLength(0,"TESTFLOAT"),6);
		checkSuccess(cur.getFieldLength(0,"TESTDOUBLE"),6);
		checkSuccess(cur.getFieldLength(0,"TESTDATE"),10);
		checkSuccess(cur.getFieldLength(0,"TESTTIME"),8);
		checkSuccess(cur.getFieldLength(0,"TESTCHAR"),50);
		checkSuccess(cur.getFieldLength(0,"TESTVARCHAR"),12);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,"TESTINTEGER"),1);
		checkSuccess(cur.getFieldLength(7,"TESTSMALLINT"),1);
		checkSuccess(cur.getFieldLength(7,"TESTDECIMAL"),4);
		checkSuccess(cur.getFieldLength(7,"TESTNUMERIC"),4);
		checkSuccess(cur.getFieldLength(7,"TESTFLOAT"),6);
		checkSuccess(cur.getFieldLength(7,"TESTDOUBLE"),6);
		checkSuccess(cur.getFieldLength(7,"TESTDATE"),10);
		checkSuccess(cur.getFieldLength(7,"TESTTIME"),8);
		checkSuccess(cur.getFieldLength(7,"TESTCHAR"),50);
		checkSuccess(cur.getFieldLength(7,"TESTVARCHAR"),12);
		System.out.println();
	
		System.out.println("FIELDS BY ARRAY: ");
		fields=cur.getRow(0);
		checkSuccess(fields[0],"1");
		checkSuccess(fields[1],"1");
		checkSuccess(fields[2],"1.10");
		checkSuccess(fields[3],"1.10");
		checkSuccess(fields[4],"1.1000");
		checkSuccess(fields[5],"1.1000");
		checkSuccess(fields[6],"2001:01:01");
		checkSuccess(fields[7],"01:00:00");
		checkSuccess(fields[8],"testchar1                                         ");
		checkSuccess(fields[9],"testvarchar1");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY ARRAY: ");
		fieldlens=cur.getRowLengths(0);
		checkSuccess(fieldlens[0],1);
		checkSuccess(fieldlens[1],1);
		checkSuccess(fieldlens[2],4);
		checkSuccess(fieldlens[3],4);
		checkSuccess(fieldlens[4],6);
		checkSuccess(fieldlens[5],6);
		checkSuccess(fieldlens[6],10);
		checkSuccess(fieldlens[7],8);
		checkSuccess(fieldlens[8],50);
		checkSuccess(fieldlens[9],12);
		System.out.println();
	
		System.out.println("INDIVIDUAL SUBSTITUTIONS: ");
		cur.prepareQuery("select $(var1),'$(var2)','$(var3)' from rdb$database");
		cur.substitution("var1",1);
		cur.substitution("var2","hello");
		cur.substitution("var3",10.5556,6,4);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"hello");
		checkSuccess(cur.getField(0,2),"10.5556");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("select '$(var1)','$(var2)','$(var3)' from rdb$database");
		cur.substitutions(subvars,subvalstrings);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"hi");
		checkSuccess(cur.getField(0,1),"hello");
		checkSuccess(cur.getField(0,2),"bye");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("select $(var1),$(var2),$(var3) from rdb$database");
		cur.substitutions(subvars,subvallongs);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"2");
		checkSuccess(cur.getField(0,2),"3");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("select $(var1),$(var2),$(var3) from rdb$database");
		cur.substitutions(subvars,subvaldoubles,precs,scales);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"10.55");
		checkSuccess(cur.getField(0,1),"10.556");
		checkSuccess(cur.getField(0,2),"10.5556");
		System.out.println();
	
		System.out.println("nullS as Nulls: ");
		cur.getNullsAsNulls();
		checkSuccess(cur.sendQuery("select 1,null,null from rdb$database"),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),null);
		checkSuccess(cur.getField(0,2),null);
		cur.getNullsAsEmptyStrings();
		checkSuccess(cur.sendQuery("select 1,null,null from rdb$database"),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"");
		checkSuccess(cur.getField(0,2),"");
		cur.getNullsAsNulls();
		System.out.println();
	
		System.out.println("RESULT SET BUFFER SIZE: ");
		checkSuccess(cur.getResultSetBufferSize(),0);
		cur.setResultSetBufferSize(2);
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		checkSuccess(cur.getResultSetBufferSize(),2);
		System.out.println();
		checkSuccess(cur.firstRowIndex(),0);
		checkSuccess(cur.endOfResultSet(),0);
		checkSuccess(cur.rowCount(),2);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(1,0),"2");
		checkSuccess(cur.getField(2,0),"3");
		System.out.println();
		checkSuccess(cur.firstRowIndex(),2);
		checkSuccess(cur.endOfResultSet(),0);
		checkSuccess(cur.rowCount(),4);
		checkSuccess(cur.getField(6,0),"7");
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
		checkSuccess(cur.firstRowIndex(),6);
		checkSuccess(cur.endOfResultSet(),0);
		checkSuccess(cur.rowCount(),8);
		checkSuccess(cur.getField(8,0),null);
		System.out.println();
		checkSuccess(cur.firstRowIndex(),8);
		checkSuccess(cur.endOfResultSet(),1);
		checkSuccess(cur.rowCount(),8);
		System.out.println();
	
		System.out.println("DONT GET COLUMN INFO: ");
		cur.dontGetColumnInfo();
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		checkSuccess(cur.getColumnName(0),null);
		checkSuccess(cur.getColumnLength(0),0);
		checkSuccess(cur.getColumnType(0),null);
		cur.getColumnInfo();
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		checkSuccess(cur.getColumnName(0),"TESTINTEGER");
		checkSuccess(cur.getColumnLength(0),4);
		checkSuccess(cur.getColumnType(0),"INTEGER");
		System.out.println();
	
		System.out.println("SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		checkSuccess(con.resumeSession(port,socket),1);
		System.out.println();
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(1,0),"2");
		checkSuccess(cur.getField(2,0),"3");
		checkSuccess(cur.getField(3,0),"4");
		checkSuccess(cur.getField(4,0),"5");
		checkSuccess(cur.getField(5,0),"6");
		checkSuccess(cur.getField(6,0),"7");
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		checkSuccess(con.resumeSession(port,socket),1);
		System.out.println();
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(1,0),"2");
		checkSuccess(cur.getField(2,0),"3");
		checkSuccess(cur.getField(3,0),"4");
		checkSuccess(cur.getField(4,0),"5");
		checkSuccess(cur.getField(5,0),"6");
		checkSuccess(cur.getField(6,0),"7");
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		checkSuccess(con.resumeSession(port,socket),1);
		System.out.println();
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(1,0),"2");
		checkSuccess(cur.getField(2,0),"3");
		checkSuccess(cur.getField(3,0),"4");
		checkSuccess(cur.getField(4,0),"5");
		checkSuccess(cur.getField(5,0),"6");
		checkSuccess(cur.getField(6,0),"7");
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
	
		System.out.println("SUSPENDED RESULT SET: ");
		cur.setResultSetBufferSize(2);
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		checkSuccess(cur.getField(2,0),"3");
		id=cur.getResultSetId();
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		checkSuccess(con.resumeSession(port,socket),1);
		checkSuccess(cur.resumeResultSet(id),1);
		System.out.println();
		checkSuccess(cur.firstRowIndex(),4);
		checkSuccess(cur.endOfResultSet(),0);
		checkSuccess(cur.rowCount(),6);
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
		checkSuccess(cur.firstRowIndex(),6);
		checkSuccess(cur.endOfResultSet(),0);
		checkSuccess(cur.rowCount(),8);
		checkSuccess(cur.getField(8,0),null);
		System.out.println();
		checkSuccess(cur.firstRowIndex(),8);
		checkSuccess(cur.endOfResultSet(),1);
		checkSuccess(cur.rowCount(),8);
		cur.setResultSetBufferSize(0);
		System.out.println();
	
		System.out.println("CACHED RESULT SET: ");
		cur.cacheToFile("cachefile1");
		cur.setCacheTtl(200);
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		filename=cur.getCacheFileName();
		checkSuccess(filename,"cachefile1");
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet(filename),1);
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
	
		System.out.println("COLUMN COUNT FOR CACHED RESULT SET: ");
		checkSuccess(cur.colCount(),11);
		System.out.println();
	
		System.out.println("COLUMN NAMES FOR CACHED RESULT SET: ");
		checkSuccess(cur.getColumnName(0),"TESTINTEGER");
		checkSuccess(cur.getColumnName(1),"TESTSMALLINT");
		checkSuccess(cur.getColumnName(2),"TESTDECIMAL");
		checkSuccess(cur.getColumnName(3),"TESTNUMERIC");
		checkSuccess(cur.getColumnName(4),"TESTFLOAT");
		checkSuccess(cur.getColumnName(5),"TESTDOUBLE");
		checkSuccess(cur.getColumnName(6),"TESTDATE");
		checkSuccess(cur.getColumnName(7),"TESTTIME");
		checkSuccess(cur.getColumnName(8),"TESTCHAR");
		checkSuccess(cur.getColumnName(9),"TESTVARCHAR");
		checkSuccess(cur.getColumnName(10),"TESTTIMESTAMP");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"TESTINTEGER");
		checkSuccess(cols[1],"TESTSMALLINT");
		checkSuccess(cols[2],"TESTDECIMAL");
		checkSuccess(cols[3],"TESTNUMERIC");
		checkSuccess(cols[4],"TESTFLOAT");
		checkSuccess(cols[5],"TESTDOUBLE");
		checkSuccess(cols[6],"TESTDATE");
		checkSuccess(cols[7],"TESTTIME");
		checkSuccess(cols[8],"TESTCHAR");
		checkSuccess(cols[9],"TESTVARCHAR");
		checkSuccess(cols[10],"TESTTIMESTAMP");
		System.out.println();
	
		System.out.println("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: ");
		cur.setResultSetBufferSize(2);
		cur.cacheToFile("cachefile1");
		cur.setCacheTtl(200);
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		filename=cur.getCacheFileName();
		checkSuccess(filename,"cachefile1");
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet(filename),1);
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(8,0),null);
		cur.setResultSetBufferSize(0);
		System.out.println();
	
		System.out.println("FROM ONE CACHE FILE TO ANOTHER: ");
		cur.cacheToFile("cachefile2");
		checkSuccess(cur.openCachedResultSet("cachefile1"),1);
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet("cachefile2"),1);
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(8,0),null);
		System.out.println();
	
		System.out.println("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: ");
		cur.setResultSetBufferSize(2);
		cur.cacheToFile("cachefile2");
		checkSuccess(cur.openCachedResultSet("cachefile1"),1);
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet("cachefile2"),1);
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(8,0),null);
		cur.setResultSetBufferSize(0);
		System.out.println();
	
		System.out.println("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: ");
		cur.setResultSetBufferSize(2);
		cur.cacheToFile("cachefile1");
		cur.setCacheTtl(200);
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		checkSuccess(cur.getField(2,0),"3");
		filename=cur.getCacheFileName();
		checkSuccess(filename,"cachefile1");
		id=cur.getResultSetId();
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		System.out.println();
		checkSuccess(con.resumeSession(port,socket),1);
		checkSuccess(cur.resumeCachedResultSet(id,filename),1);
		System.out.println();
		checkSuccess(cur.firstRowIndex(),4);
		checkSuccess(cur.endOfResultSet(),0);
		checkSuccess(cur.rowCount(),6);
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
		checkSuccess(cur.firstRowIndex(),6);
		checkSuccess(cur.endOfResultSet(),0);
		checkSuccess(cur.rowCount(),8);
		checkSuccess(cur.getField(8,0),null);
		System.out.println();
		checkSuccess(cur.firstRowIndex(),8);
		checkSuccess(cur.endOfResultSet(),1);
		checkSuccess(cur.rowCount(),8);
		cur.cacheOff();
		System.out.println();
		checkSuccess(cur.openCachedResultSet(filename),1);
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(8,0),null);
		cur.setResultSetBufferSize(0);
		System.out.println();
	
		//System.out.println("COMMIT AND ROLLBACK: ");
		SQLRConnection secondcon=new SQLRConnection("localhost",
						(short)9000,
						"/tmp/test.socket",
						"test","test",0,1);
		SQLRCursor secondcur=new SQLRCursor(secondcon);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"0");
		checkSuccess(con.commit(),1);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"8");
		checkSuccess(con.autoCommitOn(),1);
		checkSuccess(cur.sendQuery("insert into testtable values (10,10,10.1,10.1,10.1,10.1,'01-JAN-2010','10:00:00','testchar10','testvarchar10',null)"),1);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"9");
		checkSuccess(con.autoCommitOff(),1);
		System.out.println();

		System.out.println("FINISHED SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testinteger"),1);
		checkSuccess(cur.getField(4,0),"5");
		checkSuccess(cur.getField(5,0),"6");
		checkSuccess(cur.getField(6,0),"7");
		checkSuccess(cur.getField(7,0),"8");
		id=cur.getResultSetId();
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		checkSuccess(con.resumeSession(port,socket),1);
		checkSuccess(cur.resumeResultSet(id),1);
		checkSuccess(cur.getField(4,0),null);
		checkSuccess(cur.getField(5,0),null);
		checkSuccess(cur.getField(6,0),null);
		checkSuccess(cur.getField(7,0),null);
		System.out.println();
	
		// drop existing table
		con.commit();
		cur.sendQuery("delete from testtable");
		con.commit();
		System.out.println();
	
		// invalid queries...
		System.out.println("INVALID QUERIES: ");
		checkSuccess(cur.sendQuery("select * from testtable1 order by testinteger"),0);
		checkSuccess(cur.sendQuery("select * from testtable1 order by testinteger"),0);
		checkSuccess(cur.sendQuery("select * from testtable1 order by testinteger"),0);
		checkSuccess(cur.sendQuery("select * from testtable1 order by testinteger"),0);
		System.out.println();
		checkSuccess(cur.sendQuery("insert into testtable1 values (1,2,3,4)"),0);
		checkSuccess(cur.sendQuery("insert into testtable1 values (1,2,3,4)"),0);
		checkSuccess(cur.sendQuery("insert into testtable1 values (1,2,3,4)"),0);
		checkSuccess(cur.sendQuery("insert into testtable1 values (1,2,3,4)"),0);
		System.out.println();
		checkSuccess(cur.sendQuery("create table testtable"),0);
		checkSuccess(cur.sendQuery("create table testtable"),0);
		checkSuccess(cur.sendQuery("create table testtable"),0);
		checkSuccess(cur.sendQuery("create table testtable"),0);
		System.out.println();
	
	}
}
