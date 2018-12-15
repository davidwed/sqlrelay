// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

import com.firstworks.sqlrelay.SQLRConnection;
import com.firstworks.sqlrelay.SQLRCursor;


class db2 {
	
	private static void checkSuccess(String value, String success) {
	
		if (success==null) {
			if (value==null) {
				System.out.print("success ");
				return;
			} else {
				System.out.print(value+"!="+success+" ");
				System.out.print("failure ");
				System.exit(1);
			}
		}
	
		if (value.equals(success)) {
			System.out.print("success ");
		} else {
			System.out.print(value+"!="+success+" ");
			System.out.print("failure ");
			System.exit(1);
		}
	}
	
	private static void checkSuccess(long value, int success) {
	
		if (value==success) {
			System.out.print("success ");
		} else {
			System.out.print(value+"!="+success+" ");
			System.out.print("failure ");
			System.exit(1);
		}
	}
	
	private static void checkSuccess(boolean value, int success) {
	
		if (((value)?1:0)==success) {
			System.out.print("success ");
		} else {
			System.out.print(value+"!="+success+" ");
			System.out.print("failure ");
			System.exit(1);
		}
	}
	
	public static void	main(String[] args) {
	
		String	dbtype;
		String[]	bindvars={"1","2","3","4","5","6","7","8","9","10"};
		String[]	bindvals={"4","4","4","4.4","4.4","4.4",
				"testchar4","testvarchar4","01/01/2004","04:00:00"};
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
		SQLRConnection con=new SQLRConnection("sqlrelay",
						(short)9000,
						"/tmp/test.socket",
						"test","test",0,1);
		SQLRCursor cur=new SQLRCursor(con);
	
		// get database type
		System.out.println("IDENTIFY: ");
		checkSuccess(con.identify(),"db2");
		System.out.println();
	
		// ping
		System.out.println("PING: ");
		checkSuccess(con.ping(),1);
		System.out.println();
	
		// drop existing table
		cur.sendQuery("drop table testtable");
	
		System.out.println("CREATE TEMPTABLE: ");
		checkSuccess(cur.sendQuery("create table testtable (testsmallint smallint, testint integer, testbigint bigint, testdecimal decimal(10,2), testreal real, testdouble double, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
		System.out.println();
	
		System.out.println("INSERT: ");
		checkSuccess(cur.sendQuery("insert into testtable values (1,1,1,1.1,1.1,1.1,'testchar1','testvarchar1','01/01/2001','01:00:00',null)"),1);
		System.out.println();
	
		System.out.println("BIND BY POSITION: ");
		cur.prepareQuery("insert into testtable values (?,?,?,?,?,?,?,?,?,?,null)");
		checkSuccess(cur.countBindVariables(),10);
		cur.inputBind("1",2);
		cur.inputBind("2",2);
		cur.inputBind("3",2);
		cur.inputBind("4",2.2,4,2);
		cur.inputBind("5",2.2,4,2);
		cur.inputBind("6",2.2,4,2);
		cur.inputBind("7","testchar2");
		cur.inputBind("8","testvarchar2");
		cur.inputBind("9","01/01/2002");
		cur.inputBind("10","02:00:00");
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("1",3);
		cur.inputBind("2",3);
		cur.inputBind("3",3);
		cur.inputBind("4",3.3,4,2);
		cur.inputBind("5",3.3,4,2);
		cur.inputBind("6",3.3,4,2);
		cur.inputBind("7","testchar3");
		cur.inputBind("8","testvarchar3");
		cur.inputBind("9","01/01/2003");
		cur.inputBind("10","03:00:00");
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("ARRAY OF BINDS BY POSITION: ");
		cur.clearBinds();
		cur.inputBinds(bindvars,bindvals);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("INSERT: ");
		checkSuccess(cur.sendQuery("insert into testtable values (5,5,5,5.5,5.5,5.5,'testchar5','testvarchar5','01/01/2005','05:00:00',null)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (6,6,6,6.6,6.6,6.6,'testchar6','testvarchar6','01/01/2006','06:00:00',null)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (7,7,7,7.7,7.7,7.7,'testchar7','testvarchar7','01/01/2007','07:00:00',null)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (8,8,8,8.8,8.8,8.8,'testchar8','testvarchar8','01/01/2008','08:00:00',null)"),1);
		System.out.println();
	
		System.out.println("AFFECTED ROWS: ");
		checkSuccess(cur.affectedRows(),1);
		System.out.println();

		System.out.println("STORED PROCEDURE: ");
		cur.sendQuery("drop procedure testproc");
		checkSuccess(cur.sendQuery("create procedure testproc(in invar int, out outvar int) language sql begin set outvar = invar; end"),1);
		cur.prepareQuery("call testproc(?,?)");
		cur.inputBind("1",5);
		cur.defineOutputBindInteger("2");
		checkSuccess(cur.executeQuery(),1);
		checkSuccess(cur.getOutputBindInteger("2"),5);
		checkSuccess(cur.sendQuery("drop procedure testproc"),1);
		System.out.println();
	
		System.out.println("SELECT: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
		System.out.println();
	
		System.out.println("COLUMN COUNT: ");
		checkSuccess(cur.colCount(),11);
		System.out.println();
	
		System.out.println("COLUMN NAMES: ");
		checkSuccess(cur.getColumnName(0),"TESTSMALLINT");
		checkSuccess(cur.getColumnName(1),"TESTINT");
		checkSuccess(cur.getColumnName(2),"TESTBIGINT");
		checkSuccess(cur.getColumnName(3),"TESTDECIMAL");
		checkSuccess(cur.getColumnName(4),"TESTREAL");
		checkSuccess(cur.getColumnName(5),"TESTDOUBLE");
		checkSuccess(cur.getColumnName(6),"TESTCHAR");
		checkSuccess(cur.getColumnName(7),"TESTVARCHAR");
		checkSuccess(cur.getColumnName(8),"TESTDATE");
		checkSuccess(cur.getColumnName(9),"TESTTIME");
		checkSuccess(cur.getColumnName(10),"TESTTIMESTAMP");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"TESTSMALLINT");
		checkSuccess(cols[1],"TESTINT");
		checkSuccess(cols[2],"TESTBIGINT");
		checkSuccess(cols[3],"TESTDECIMAL");
		checkSuccess(cols[4],"TESTREAL");
		checkSuccess(cols[5],"TESTDOUBLE");
		checkSuccess(cols[6],"TESTCHAR");
		checkSuccess(cols[7],"TESTVARCHAR");
		checkSuccess(cols[8],"TESTDATE");
		checkSuccess(cols[9],"TESTTIME");
		checkSuccess(cols[10],"TESTTIMESTAMP");
		System.out.println();
	
		System.out.println("COLUMN TYPES: ");
		checkSuccess(cur.getColumnType(0),"SMALLINT");
		checkSuccess(cur.getColumnType("TESTSMALLINT"),"SMALLINT");
		checkSuccess(cur.getColumnType(1),"INTEGER");
		checkSuccess(cur.getColumnType("TESTINT"),"INTEGER");
		checkSuccess(cur.getColumnType(2),"BIGINT");
		checkSuccess(cur.getColumnType("TESTBIGINT"),"BIGINT");
		checkSuccess(cur.getColumnType(3),"DECIMAL");
		checkSuccess(cur.getColumnType("TESTDECIMAL"),"DECIMAL");
		checkSuccess(cur.getColumnType(4),"REAL");
		checkSuccess(cur.getColumnType("TESTREAL"),"REAL");
		checkSuccess(cur.getColumnType(5),"DOUBLE");
		checkSuccess(cur.getColumnType("TESTDOUBLE"),"DOUBLE");
		checkSuccess(cur.getColumnType(6),"CHAR");
		checkSuccess(cur.getColumnType("TESTCHAR"),"CHAR");
		checkSuccess(cur.getColumnType(7),"VARCHAR");
		checkSuccess(cur.getColumnType("TESTVARCHAR"),"VARCHAR");
		checkSuccess(cur.getColumnType(8),"DATE");
		checkSuccess(cur.getColumnType("TESTDATE"),"DATE");
		checkSuccess(cur.getColumnType(9),"TIME");
		checkSuccess(cur.getColumnType("TESTTIME"),"TIME");
		checkSuccess(cur.getColumnType(10),"TIMESTAMP");
		checkSuccess(cur.getColumnType("TESTTIMESTAMP"),"TIMESTAMP");
		System.out.println();
	
		System.out.println("COLUMN LENGTH: ");
		checkSuccess(cur.getColumnLength(0),2);
		checkSuccess(cur.getColumnLength("TESTSMALLINT"),2);
		checkSuccess(cur.getColumnLength(1),4);
		checkSuccess(cur.getColumnLength("TESTINT"),4);
		checkSuccess(cur.getColumnLength(2),8);
		checkSuccess(cur.getColumnLength("TESTBIGINT"),8);
		checkSuccess(cur.getColumnLength(3),12);
		checkSuccess(cur.getColumnLength("TESTDECIMAL"),12);
		checkSuccess(cur.getColumnLength(4),4);
		checkSuccess(cur.getColumnLength("TESTREAL"),4);
		checkSuccess(cur.getColumnLength(5),8);
		checkSuccess(cur.getColumnLength("TESTDOUBLE"),8);
		checkSuccess(cur.getColumnLength(6),40);
		checkSuccess(cur.getColumnLength("TESTCHAR"),40);
		checkSuccess(cur.getColumnLength(7),40);
		checkSuccess(cur.getColumnLength("TESTVARCHAR"),40);
		checkSuccess(cur.getColumnLength(8),6);
		checkSuccess(cur.getColumnLength("TESTDATE"),6);
		checkSuccess(cur.getColumnLength(9),6);
		checkSuccess(cur.getColumnLength("TESTTIME"),6);
		checkSuccess(cur.getColumnLength(10),16);
		checkSuccess(cur.getColumnLength("TESTTIMESTAMP"),16);
		System.out.println();
	
		System.out.println("LONGEST COLUMN: ");
		checkSuccess(cur.getLongest(0),1);
		checkSuccess(cur.getLongest("TESTSMALLINT"),1);
		checkSuccess(cur.getLongest(1),1);
		checkSuccess(cur.getLongest("TESTINT"),1);
		checkSuccess(cur.getLongest(2),1);
		checkSuccess(cur.getLongest("TESTBIGINT"),1);
		checkSuccess(cur.getLongest(3),4);
		checkSuccess(cur.getLongest("TESTDECIMAL"),4);
		//checkSuccess(cur.getLongest(4),3);
		//checkSuccess(cur.getLongest("TESTREAL"),3);
		//checkSuccess(cur.getLongest(5),3);
		//checkSuccess(cur.getLongest("TESTDOUBLE"),3);
		checkSuccess(cur.getLongest(6),40);
		checkSuccess(cur.getLongest("TESTCHAR"),40);
		checkSuccess(cur.getLongest(7),12);
		checkSuccess(cur.getLongest("TESTVARCHAR"),12);
		checkSuccess(cur.getLongest(8),10);
		checkSuccess(cur.getLongest("TESTDATE"),10);
		checkSuccess(cur.getLongest(9),8);
		checkSuccess(cur.getLongest("TESTTIME"),8);
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
		checkSuccess(cur.getField(0,2),"1");
		checkSuccess(cur.getField(0,3),"1.10");
		//checkSuccess(cur.getField(0,4),"1.1");
		//checkSuccess(cur.getField(0,5),"1.1");
		checkSuccess(cur.getField(0,6),"testchar1                               ");
		checkSuccess(cur.getField(0,7),"testvarchar1");
		checkSuccess(cur.getField(0,8),"2001-01-01");
		checkSuccess(cur.getField(0,9),"01:00:00");
		System.out.println();
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(7,1),"8");
		checkSuccess(cur.getField(7,2),"8");
		checkSuccess(cur.getField(7,3),"8.80");
		//checkSuccess(cur.getField(7,4),"8.8");
		//checkSuccess(cur.getField(7,5),"8.8");
		checkSuccess(cur.getField(7,6),"testchar8                               ");
		checkSuccess(cur.getField(7,7),"testvarchar8");
		checkSuccess(cur.getField(7,8),"2008-01-01");
		checkSuccess(cur.getField(7,9),"08:00:00");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY INDEX: ");
		checkSuccess(cur.getFieldLength(0,0),1);
		checkSuccess(cur.getFieldLength(0,1),1);
		checkSuccess(cur.getFieldLength(0,2),1);
		checkSuccess(cur.getFieldLength(0,3),4);
		//checkSuccess(cur.getFieldLength(0,4),3);
		//checkSuccess(cur.getFieldLength(0,5),3);
		checkSuccess(cur.getFieldLength(0,6),40);
		checkSuccess(cur.getFieldLength(0,7),12);
		checkSuccess(cur.getFieldLength(0,8),10);
		checkSuccess(cur.getFieldLength(0,9),8);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,0),1);
		checkSuccess(cur.getFieldLength(7,1),1);
		checkSuccess(cur.getFieldLength(7,2),1);
		checkSuccess(cur.getFieldLength(7,3),4);
		//checkSuccess(cur.getFieldLength(7,4),3);
		//checkSuccess(cur.getFieldLength(7,5),3);
		checkSuccess(cur.getFieldLength(7,6),40);
		checkSuccess(cur.getFieldLength(7,7),12);
		checkSuccess(cur.getFieldLength(7,8),10);
		checkSuccess(cur.getFieldLength(7,9),8);
		System.out.println();
	
		System.out.println("FIELDS BY NAME: ");
		checkSuccess(cur.getField(0,"TESTSMALLINT"),"1");
		checkSuccess(cur.getField(0,"TESTINT"),"1");
		checkSuccess(cur.getField(0,"TESTBIGINT"),"1");
		checkSuccess(cur.getField(0,"TESTDECIMAL"),"1.10");
		//checkSuccess(cur.getField(0,"TESTREAL"),"1.1");
		//checkSuccess(cur.getField(0,"TESTDOUBLE"),"1.1");
		checkSuccess(cur.getField(0,"TESTCHAR"),"testchar1                               ");
		checkSuccess(cur.getField(0,"TESTVARCHAR"),"testvarchar1");
		checkSuccess(cur.getField(0,"TESTDATE"),"2001-01-01");
		checkSuccess(cur.getField(0,"TESTTIME"),"01:00:00");
		System.out.println();
		checkSuccess(cur.getField(7,"TESTSMALLINT"),"8");
		checkSuccess(cur.getField(7,"TESTINT"),"8");
		checkSuccess(cur.getField(7,"TESTBIGINT"),"8");
		checkSuccess(cur.getField(7,"TESTDECIMAL"),"8.80");
		//checkSuccess(cur.getField(7,"TESTREAL"),"8.8");
		//checkSuccess(cur.getField(7,"TESTDOUBLE"),"8.8");
		checkSuccess(cur.getField(7,"TESTCHAR"),"testchar8                               ");
		checkSuccess(cur.getField(7,"TESTVARCHAR"),"testvarchar8");
		checkSuccess(cur.getField(7,"TESTDATE"),"2008-01-01");
		checkSuccess(cur.getField(7,"TESTTIME"),"08:00:00");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY NAME: ");
		checkSuccess(cur.getFieldLength(0,"TESTSMALLINT"),1);
		checkSuccess(cur.getFieldLength(0,"TESTINT"),1);
		checkSuccess(cur.getFieldLength(0,"TESTBIGINT"),1);
		checkSuccess(cur.getFieldLength(0,"TESTDECIMAL"),4);
		//checkSuccess(cur.getFieldLength(0,"TESTREAL"),3);
		//checkSuccess(cur.getFieldLength(0,"TESTDOUBLE"),3);
		checkSuccess(cur.getFieldLength(0,"TESTCHAR"),40);
		checkSuccess(cur.getFieldLength(0,"TESTVARCHAR"),12);
		checkSuccess(cur.getFieldLength(0,"TESTDATE"),10);
		checkSuccess(cur.getFieldLength(0,"TESTTIME"),8);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,"TESTSMALLINT"),1);
		checkSuccess(cur.getFieldLength(7,"TESTINT"),1);
		checkSuccess(cur.getFieldLength(7,"TESTBIGINT"),1);
		checkSuccess(cur.getFieldLength(7,"TESTDECIMAL"),4);
		//checkSuccess(cur.getFieldLength(7,"TESTREAL"),3);
		//checkSuccess(cur.getFieldLength(7,"TESTDOUBLE"),3);
		checkSuccess(cur.getFieldLength(7,"TESTCHAR"),40);
		checkSuccess(cur.getFieldLength(7,"TESTVARCHAR"),12);
		checkSuccess(cur.getFieldLength(7,"TESTDATE"),10);
		checkSuccess(cur.getFieldLength(7,"TESTTIME"),8);
		System.out.println();
	
		System.out.println("FIELDS BY ARRAY: ");
		fields=cur.getRow(0);
		checkSuccess(fields[0],"1");
		checkSuccess(fields[1],"1");
		checkSuccess(fields[2],"1");
		checkSuccess(fields[3],"1.10");
		//checkSuccess(fields[4],"1.1");
		//checkSuccess(fields[5],"1.1");
		checkSuccess(fields[6],"testchar1                               ");
		checkSuccess(fields[7],"testvarchar1");
		checkSuccess(fields[8],"2001-01-01");
		checkSuccess(fields[9],"01:00:00");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY ARRAY: ");
		fieldlens=cur.getRowLengths(0);
		checkSuccess(fieldlens[0],1);
		checkSuccess(fieldlens[1],1);
		checkSuccess(fieldlens[2],1);
		checkSuccess(fieldlens[3],4);
		//checkSuccess(fieldlens[4],3);
		//checkSuccess(fieldlens[5],3);
		checkSuccess(fieldlens[6],40);
		checkSuccess(fieldlens[7],12);
		checkSuccess(fieldlens[8],10);
		checkSuccess(fieldlens[9],8);
		System.out.println();
	
		System.out.println("INDIVIDUAL SUBSTITUTIONS: ");
		cur.prepareQuery("values ($(var1),'$(var2)','$(var3)')");
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
		cur.prepareQuery("values ('$(var1)','$(var2)','$(var3)')");
		cur.substitutions(subvars,subvalstrings);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"hi");
		checkSuccess(cur.getField(0,1),"hello");
		checkSuccess(cur.getField(0,2),"bye");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("values ($(var1),$(var2),$(var3))");
		cur.substitutions(subvars,subvallongs);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"2");
		checkSuccess(cur.getField(0,2),"3");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("values ($(var1),$(var2),$(var3))");
		cur.substitutions(subvars,subvaldoubles,precs,scales);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"10.55");
		checkSuccess(cur.getField(0,1),"10.556");
		checkSuccess(cur.getField(0,2),"10.5556");
		System.out.println();
	
		System.out.println("nullS as Nulls: ");
		cur.sendQuery("drop table testtable1");
		cur.sendQuery("create table testtable1 (col1 char(1), col2 char(1), col3 char(1))");
		cur.getNullsAsNulls();
		checkSuccess(cur.sendQuery("insert into testtable1 values ('1',null,null)"),1);
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),null);
		checkSuccess(cur.getField(0,2),null);
		cur.getNullsAsEmptyStrings();
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"");
		checkSuccess(cur.getField(0,2),"");
		cur.sendQuery("drop table testtable1");
		cur.getNullsAsNulls();
		System.out.println();
		
		System.out.println("RESULT SET BUFFER SIZE: ");
		checkSuccess(cur.getResultSetBufferSize(),0);
		cur.setResultSetBufferSize(2);
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
		checkSuccess(cur.getColumnName(0),null);
		checkSuccess(cur.getColumnLength(0),0);
		checkSuccess(cur.getColumnType(0),null);
		cur.getColumnInfo();
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
		checkSuccess(cur.getColumnName(0),"TESTSMALLINT");
		checkSuccess(cur.getColumnLength(0),2);
		checkSuccess(cur.getColumnType(0),"SMALLINT");
		System.out.println();
	
		System.out.println("SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
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
		checkSuccess(cur.getColumnName(0),"TESTSMALLINT");
		checkSuccess(cur.getColumnName(1),"TESTINT");
		checkSuccess(cur.getColumnName(2),"TESTBIGINT");
		checkSuccess(cur.getColumnName(3),"TESTDECIMAL");
		checkSuccess(cur.getColumnName(4),"TESTREAL");
		checkSuccess(cur.getColumnName(5),"TESTDOUBLE");
		checkSuccess(cur.getColumnName(6),"TESTCHAR");
		checkSuccess(cur.getColumnName(7),"TESTVARCHAR");
		checkSuccess(cur.getColumnName(8),"TESTDATE");
		checkSuccess(cur.getColumnName(9),"TESTTIME");
		checkSuccess(cur.getColumnName(10),"TESTTIMESTAMP");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"TESTSMALLINT");
		checkSuccess(cols[1],"TESTINT");
		checkSuccess(cols[2],"TESTBIGINT");
		checkSuccess(cols[3],"TESTDECIMAL");
		checkSuccess(cols[4],"TESTREAL");
		checkSuccess(cols[5],"TESTDOUBLE");
		checkSuccess(cols[6],"TESTCHAR");
		checkSuccess(cols[7],"TESTVARCHAR");
		checkSuccess(cols[8],"TESTDATE");
		checkSuccess(cols[9],"TESTTIME");
		checkSuccess(cols[10],"TESTTIMESTAMP");
		System.out.println();
	
		System.out.println("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: ");
		cur.setResultSetBufferSize(2);
		cur.cacheToFile("cachefile1");
		cur.setCacheTtl(200);
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),1);
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

		System.out.println("FINISHED SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
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
		cur.sendQuery("drop table testtable");
		System.out.println();
	
		// invalid queries...
		System.out.println("INVALID QUERIES: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testsmallint"),0);
		System.out.println();
		checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0);
		checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0);
		checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0);
		checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0);
		System.out.println();
		checkSuccess(cur.sendQuery("create table testtable"),0);
		checkSuccess(cur.sendQuery("create table testtable"),0);
		checkSuccess(cur.sendQuery("create table testtable"),0);
		checkSuccess(cur.sendQuery("create table testtable"),0);
		System.out.println();
	}
}
