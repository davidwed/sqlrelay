// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

import com.firstworks.sqlrelay.SQLRConnection;
import com.firstworks.sqlrelay.SQLRCursor;


class msql {
	
	
	
	
	
	
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
	
	
		// usage...
		if (args.length<5) {
			System.out.println("usage: java msql host port socket user password");
			System.exit(0);
		}
	
	
		// instantiation
		SQLRConnection con=new SQLRConnection(args[0],
					(short)Integer.parseInt(args[1]), 
						args[2],args[3],args[4],0,1);
		SQLRCursor cur=new SQLRCursor(con);
	
		// get database type
		System.out.println("IDENTIFY: ");
		checkSuccess(con.identify(),"msql");
		System.out.println();
	
		// ping
		System.out.println("PING: ");
		checkSuccess(con.ping(),1);
		System.out.println();
	
		// drop existing table
		cur.sendQuery("drop table testtable");
	
		System.out.println("CREATE TEMPTABLE: ");
		checkSuccess(cur.sendQuery("create table testtable (testchar char(40), testdate date, testint int, testmoney money, testreal real, testtext text(40), testtime time, testuint uint)"),1);
		System.out.println();
	
		System.out.println("INSERT: ");
		checkSuccess(cur.sendQuery("insert into testtable values ('char1','01-Jan-2001',1,1.00,1.1,'text1','01:00:00',1)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values ('char2','01-Jan-2002',2,2.00,2.1,'text2','02:00:00',2)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values ('char3','01-Jan-2003',3,3.00,3.1,'text3','03:00:00',3)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values ('char4','01-Jan-2004',4,4.00,4.1,'text4','04:00:00',4)"),1);
		System.out.println();
	
		System.out.println("AFFECTED ROWS: ");
		checkSuccess(cur.affectedRows(),0);
		System.out.println();
	
		System.out.println("BIND BY NAME: ");
		cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7,:var8)");
		checkSuccess(cur.countBindVariables(),8);
		cur.inputBind("var1","char5");
		cur.inputBind("var2","01-Jan-2005");
		cur.inputBind("var3",5);
		cur.inputBind("var4",5.00,3,2);
		cur.inputBind("var5",5.1,2,1);
		cur.inputBind("var6","text5");
		cur.inputBind("var7","05:00:00");
		cur.inputBind("var8",5);
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("var1","char6");
		cur.inputBind("var2","01-Jan-2006");
		cur.inputBind("var3",6);
		cur.inputBind("var4",6.00,3,2);
		cur.inputBind("var5",6.1,2,1);
		cur.inputBind("var6","text6");
		cur.inputBind("var7","06:00:00");
		cur.inputBind("var8",6);
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("var1","char7");
		cur.inputBind("var2","01-Jan-2007");
		cur.inputBind("var3",7);
		cur.inputBind("var4",7.00,3,2);
		cur.inputBind("var5",7.1,2,1);
		cur.inputBind("var6","text7");
		cur.inputBind("var7","07:00:00");
		cur.inputBind("var8",7);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("BIND BY NAME WITH VALIDATION: ");
		cur.clearBinds();
		cur.inputBind("var1","char8");
		cur.inputBind("var2","01-Jan-2008");
		cur.inputBind("var3",8);
		cur.inputBind("var4",8.00,3,2);
		cur.inputBind("var5",8.1,2,1);
		cur.inputBind("var6","text8");
		cur.inputBind("var7","08:00:00");
		cur.inputBind("var8",8);
		cur.inputBind("var9","junkvalue");
		cur.validateBinds();
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("SELECT: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		System.out.println();
	
		System.out.println("COLUMN COUNT: ");
		checkSuccess(cur.colCount(),8);
		System.out.println();
	
		System.out.println("COLUMN NAMES: ");
		checkSuccess(cur.getColumnName(0),"testchar");
		checkSuccess(cur.getColumnName(1),"testdate");
		checkSuccess(cur.getColumnName(2),"testint");
		checkSuccess(cur.getColumnName(3),"testmoney");
		checkSuccess(cur.getColumnName(4),"testreal");
		checkSuccess(cur.getColumnName(5),"testtext");
		checkSuccess(cur.getColumnName(6),"testtime");
		checkSuccess(cur.getColumnName(7),"testuint");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"testchar");
		checkSuccess(cols[1],"testdate");
		checkSuccess(cols[2],"testint");
		checkSuccess(cols[3],"testmoney");
		checkSuccess(cols[4],"testreal");
		checkSuccess(cols[5],"testtext");
		checkSuccess(cols[6],"testtime");
		checkSuccess(cols[7],"testuint");
		System.out.println();
	
		System.out.println("COLUMN TYPES: ");
		checkSuccess(cur.getColumnType(0),"CHAR");
		checkSuccess(cur.getColumnType("testchar"),"CHAR");
		checkSuccess(cur.getColumnType(1),"DATE");
		checkSuccess(cur.getColumnType("testdate"),"DATE");
		checkSuccess(cur.getColumnType(2),"INT");
		checkSuccess(cur.getColumnType("testint"),"INT");
		checkSuccess(cur.getColumnType(3),"MONEY");
		checkSuccess(cur.getColumnType("testmoney"),"MONEY");
		checkSuccess(cur.getColumnType(4),"REAL");
		checkSuccess(cur.getColumnType("testreal"),"REAL");
		checkSuccess(cur.getColumnType(5),"TEXT");
		checkSuccess(cur.getColumnType("testtext"),"TEXT");
		checkSuccess(cur.getColumnType(6),"TIME");
		checkSuccess(cur.getColumnType("testtime"),"TIME");
		checkSuccess(cur.getColumnType(7),"UINT");
		checkSuccess(cur.getColumnType("testuint"),"UINT");
		System.out.println();
	
		System.out.println("COLUMN LENGTH: ");
		checkSuccess(cur.getColumnLength(0),40);
		checkSuccess(cur.getColumnLength("testchar"),40);
		checkSuccess(cur.getColumnLength(1),4);
		checkSuccess(cur.getColumnLength("testdate"),4);
		checkSuccess(cur.getColumnLength(2),4);
		checkSuccess(cur.getColumnLength("testint"),4);
		checkSuccess(cur.getColumnLength(3),4);
		checkSuccess(cur.getColumnLength("testmoney"),4);
		checkSuccess(cur.getColumnLength(4),8);
		checkSuccess(cur.getColumnLength("testreal"),8);
		checkSuccess(cur.getColumnLength(5),40);
		checkSuccess(cur.getColumnLength("testtext"),40);
		checkSuccess(cur.getColumnLength(6),4);
		checkSuccess(cur.getColumnLength("testtime"),4);
		checkSuccess(cur.getColumnLength(7),4);
		checkSuccess(cur.getColumnLength("testuint"),4);
		System.out.println();
	
		System.out.println("LONGEST COLUMN: ");
		checkSuccess(cur.getLongest(0),5);
		checkSuccess(cur.getLongest("testchar"),5);
		checkSuccess(cur.getLongest(1),11);
		checkSuccess(cur.getLongest("testdate"),11);
		checkSuccess(cur.getLongest(2),1);
		checkSuccess(cur.getLongest("testint"),1);
		checkSuccess(cur.getLongest(3),4);
		checkSuccess(cur.getLongest("testmoney"),4);
		checkSuccess(cur.getLongest(4),3);
		checkSuccess(cur.getLongest("testreal"),3);
		checkSuccess(cur.getLongest(5),5);
		checkSuccess(cur.getLongest("testtext"),5);
		checkSuccess(cur.getLongest(6),8);
		checkSuccess(cur.getLongest("testtime"),8);
		checkSuccess(cur.getLongest(7),1);
		checkSuccess(cur.getLongest("testuint"),1);
		System.out.println();
	
		System.out.println("ROW COUNT: ");
		checkSuccess(cur.rowCount(),8);
		System.out.println();
	
		System.out.println("TOTAL ROWS: ");
		checkSuccess(cur.totalRows(),8);
		System.out.println();
	
		System.out.println("FIRST ROW INDEX: ");
		checkSuccess(cur.firstRowIndex(),0);
		System.out.println();
	
		System.out.println("END OF RESULT SET: ");
		checkSuccess(cur.endOfResultSet(),1);
		System.out.println();
	
		System.out.println("FIELDS BY INDEX: ");
		checkSuccess(cur.getField(0,0),"char1");
		checkSuccess(cur.getField(0,1),"01-Jan-2001");
		checkSuccess(cur.getField(0,2),"1");
		checkSuccess(cur.getField(0,3),"1.00");
		checkSuccess(cur.getField(0,4),"1.1");
		checkSuccess(cur.getField(0,5),"text1");
		checkSuccess(cur.getField(0,6),"01:00:00");
		checkSuccess(cur.getField(0,7),"1");
		System.out.println();
		checkSuccess(cur.getField(7,0),"char8");
		checkSuccess(cur.getField(7,1),"01-Jan-2008");
		checkSuccess(cur.getField(7,2),"8");
		checkSuccess(cur.getField(7,3),"8.00");
		checkSuccess(cur.getField(7,4),"8.1");
		checkSuccess(cur.getField(7,5),"text8");
		checkSuccess(cur.getField(7,6),"08:00:00");
		checkSuccess(cur.getField(7,7),"8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY INDEX: ");
		checkSuccess(cur.getFieldLength(0,0),5);
		checkSuccess(cur.getFieldLength(0,1),11);
		checkSuccess(cur.getFieldLength(0,2),1);
		checkSuccess(cur.getFieldLength(0,3),4);
		checkSuccess(cur.getFieldLength(0,4),3);
		checkSuccess(cur.getFieldLength(0,5),5);
		checkSuccess(cur.getFieldLength(0,6),8);
		checkSuccess(cur.getFieldLength(0,7),1);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,0),5);
		checkSuccess(cur.getFieldLength(7,1),11);
		checkSuccess(cur.getFieldLength(7,2),1);
		checkSuccess(cur.getFieldLength(7,3),4);
		checkSuccess(cur.getFieldLength(7,4),3);
		checkSuccess(cur.getFieldLength(7,5),5);
		checkSuccess(cur.getFieldLength(7,6),8);
		checkSuccess(cur.getFieldLength(7,7),1);
		System.out.println();
	
		System.out.println("FIELDS BY NAME: ");
		checkSuccess(cur.getField(0,"testchar"),"char1");
		checkSuccess(cur.getField(0,"testdate"),"01-Jan-2001");
		checkSuccess(cur.getField(0,"testint"),"1");
		checkSuccess(cur.getField(0,"testmoney"),"1.00");
		checkSuccess(cur.getField(0,"testreal"),"1.1");
		checkSuccess(cur.getField(0,"testtext"),"text1");
		checkSuccess(cur.getField(0,"testtime"),"01:00:00");
		checkSuccess(cur.getField(0,"testuint"),"1");
		System.out.println();
		checkSuccess(cur.getField(7,"testchar"),"char8");
		checkSuccess(cur.getField(7,"testdate"),"01-Jan-2008");
		checkSuccess(cur.getField(7,"testint"),"8");
		checkSuccess(cur.getField(7,"testmoney"),"8.00");
		checkSuccess(cur.getField(7,"testreal"),"8.1");
		checkSuccess(cur.getField(7,"testtext"),"text8");
		checkSuccess(cur.getField(7,"testtime"),"08:00:00");
		checkSuccess(cur.getField(7,"testuint"),"8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY NAME: ");
		checkSuccess(cur.getFieldLength(0,"testchar"),5);
		checkSuccess(cur.getFieldLength(0,"testdate"),11);
		checkSuccess(cur.getFieldLength(0,"testint"),1);
		checkSuccess(cur.getFieldLength(0,"testmoney"),4);
		checkSuccess(cur.getFieldLength(0,"testreal"),3);
		checkSuccess(cur.getFieldLength(0,"testtext"),5);
		checkSuccess(cur.getFieldLength(0,"testtime"),8);
		checkSuccess(cur.getFieldLength(0,"testuint"),1);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,"testchar"),5);
		checkSuccess(cur.getFieldLength(7,"testdate"),11);
		checkSuccess(cur.getFieldLength(7,"testint"),1);
		checkSuccess(cur.getFieldLength(7,"testmoney"),4);
		checkSuccess(cur.getFieldLength(7,"testreal"),3);
		checkSuccess(cur.getFieldLength(7,"testtext"),5);
		checkSuccess(cur.getFieldLength(7,"testtime"),8);
		checkSuccess(cur.getFieldLength(7,"testuint"),1);
		System.out.println();
	
		System.out.println("FIELDS BY ARRAY: ");
		fields=cur.getRow(0);
		checkSuccess(fields[0],"char1");
		checkSuccess(fields[1],"01-Jan-2001");
		checkSuccess(fields[2],"1");
		checkSuccess(fields[3],"1.00");
		checkSuccess(fields[4],"1.1");
		checkSuccess(fields[5],"text1");
		checkSuccess(fields[6],"01:00:00");
		checkSuccess(fields[7],"1");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY ARRAY: ");
		fieldlens=cur.getRowLengths(0);
		checkSuccess(fieldlens[0],5);
		checkSuccess(fieldlens[1],11);
		checkSuccess(fieldlens[2],1);
		checkSuccess(fieldlens[3],4);
		checkSuccess(fieldlens[4],3);
		checkSuccess(fieldlens[5],5);
		checkSuccess(fieldlens[6],8);
		checkSuccess(fieldlens[7],1);
		System.out.println();
	
		System.out.println("INDIVIDUAL SUBSTITUTIONS: ");
		cur.sendQuery("drop table testtable1");
		checkSuccess(cur.sendQuery("create table testtable1 (col1 int, col2 char(40), col3 real)"),1);
		cur.prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))");
		cur.substitution("var1",1);
		cur.substitution("var2","hello");
		cur.substitution("var3",10.5556,6,4);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"hello");
		checkSuccess(cur.getField(0,2),"10.5556");
		checkSuccess(cur.sendQuery("delete from testtable1"),1);
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.sendQuery("drop table testtable1");
		cur.sendQuery("create table testtable1 (col1 char(40), col2 char(40), col3 char(40))");
		cur.prepareQuery("insert into testtable1 values ('$(var1)','$(var2)','$(var3)')");
		cur.substitutions(subvars,subvalstrings);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"hi");
		checkSuccess(cur.getField(0,1),"hello");
		checkSuccess(cur.getField(0,2),"bye");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.sendQuery("drop table testtable1");
		cur.sendQuery("create table testtable1 (col1 int, col2 int, col3 int)");
		cur.prepareQuery("insert into testtable1 values ($(var1),$(var2),$(var3))");
		cur.substitutions(subvars,subvallongs);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"2");
		checkSuccess(cur.getField(0,2),"3");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.sendQuery("drop table testtable1");
		cur.sendQuery("create table testtable1 (col1 real, col2 real, col3 real)");
		cur.prepareQuery("insert into testtable1 values ($(var1),$(var2),$(var3))");
		cur.substitutions(subvars,subvaldoubles,precs,scales);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"10.55");
		checkSuccess(cur.getField(0,1),"10.556");
		checkSuccess(cur.getField(0,2),"10.5556");
		checkSuccess(cur.sendQuery("delete from testtable1"),1);
		System.out.println();
	
		System.out.println("nullS as Nulls: ");
		cur.sendQuery("drop table testtable1");
		cur.sendQuery("create table testtable1 (col1 char(40), col2 char(40), col3 char(40))");
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
		checkSuccess(cur.sendQuery("drop table testtable1"),1);
		cur.getNullsAsNulls();
		System.out.println();
	
		System.out.println("RESULT SET BUFFER SIZE: ");
		checkSuccess(cur.getResultSetBufferSize(),0);
		cur.setResultSetBufferSize(2);
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(cur.getResultSetBufferSize(),2);
		System.out.println();
		checkSuccess(cur.firstRowIndex(),0);
		checkSuccess(cur.endOfResultSet(),0);
		checkSuccess(cur.rowCount(),2);
		checkSuccess(cur.getField(0,0),"char1");
		checkSuccess(cur.getField(1,0),"char2");
		checkSuccess(cur.getField(2,0),"char3");
		System.out.println();
		checkSuccess(cur.firstRowIndex(),2);
		checkSuccess(cur.endOfResultSet(),0);
		checkSuccess(cur.rowCount(),4);
		checkSuccess(cur.getField(6,0),"char7");
		checkSuccess(cur.getField(7,0),"char8");
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
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(cur.getColumnName(0),null);
		checkSuccess(cur.getColumnLength(0),0);
		checkSuccess(cur.getColumnType(0),null);
		cur.getColumnInfo();
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(cur.getColumnName(0),"testchar");
		checkSuccess(cur.getColumnLength(0),40);
		checkSuccess(cur.getColumnType(0),"CHAR");
		System.out.println();
	
		System.out.println("SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		checkSuccess(con.resumeSession(port,socket),1);
		System.out.println();
		checkSuccess(cur.getField(0,0),"char1");
		checkSuccess(cur.getField(1,0),"char2");
		checkSuccess(cur.getField(2,0),"char3");
		checkSuccess(cur.getField(3,0),"char4");
		checkSuccess(cur.getField(4,0),"char5");
		checkSuccess(cur.getField(5,0),"char6");
		checkSuccess(cur.getField(6,0),"char7");
		checkSuccess(cur.getField(7,0),"char8");
		System.out.println();
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		checkSuccess(con.resumeSession(port,socket),1);
		System.out.println();
		checkSuccess(cur.getField(0,0),"char1");
		checkSuccess(cur.getField(1,0),"char2");
		checkSuccess(cur.getField(2,0),"char3");
		checkSuccess(cur.getField(3,0),"char4");
		checkSuccess(cur.getField(4,0),"char5");
		checkSuccess(cur.getField(5,0),"char6");
		checkSuccess(cur.getField(6,0),"char7");
		checkSuccess(cur.getField(7,0),"char8");
		System.out.println();
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		checkSuccess(con.resumeSession(port,socket),1);
		System.out.println();
		checkSuccess(cur.getField(0,0),"char1");
		checkSuccess(cur.getField(1,0),"char2");
		checkSuccess(cur.getField(2,0),"char3");
		checkSuccess(cur.getField(3,0),"char4");
		checkSuccess(cur.getField(4,0),"char5");
		checkSuccess(cur.getField(5,0),"char6");
		checkSuccess(cur.getField(6,0),"char7");
		checkSuccess(cur.getField(7,0),"char8");
		System.out.println();
	
		System.out.println("SUSPENDED RESULT SET: ");
		cur.setResultSetBufferSize(2);
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(cur.getField(2,0),"char3");
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
		checkSuccess(cur.getField(7,0),"char8");
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
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		filename=cur.getCacheFileName();
		checkSuccess(filename,"cachefile1");
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet(filename),1);
		checkSuccess(cur.getField(7,0),"char8");
		System.out.println();
	
		System.out.println("COLUMN COUNT FOR CACHED RESULT SET: ");
		checkSuccess(cur.colCount(),8);
		System.out.println();
	
		System.out.println("COLUMN NAMES FOR CACHED RESULT SET: ");
		checkSuccess(cur.getColumnName(0),"testchar");
		checkSuccess(cur.getColumnName(1),"testdate");
		checkSuccess(cur.getColumnName(2),"testint");
		checkSuccess(cur.getColumnName(3),"testmoney");
		checkSuccess(cur.getColumnName(4),"testreal");
		checkSuccess(cur.getColumnName(5),"testtext");
		checkSuccess(cur.getColumnName(6),"testtime");
		checkSuccess(cur.getColumnName(7),"testuint");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"testchar");
		checkSuccess(cols[1],"testdate");
		checkSuccess(cols[2],"testint");
		checkSuccess(cols[3],"testmoney");
		checkSuccess(cols[4],"testreal");
		checkSuccess(cols[5],"testtext");
		checkSuccess(cols[6],"testtime");
		checkSuccess(cols[7],"testuint");
		System.out.println();
	
		System.out.println("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: ");
		cur.setResultSetBufferSize(2);
		cur.cacheToFile("cachefile1");
		cur.setCacheTtl(200);
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		filename=cur.getCacheFileName();
		checkSuccess(filename,"cachefile1");
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet(filename),1);
		checkSuccess(cur.getField(7,0),"char8");
		checkSuccess(cur.getField(8,0),null);
		cur.setResultSetBufferSize(0);
		System.out.println();
	
		System.out.println("FROM ONE CACHE FILE TO ANOTHER: ");
		cur.cacheToFile("cachefile2");
		checkSuccess(cur.openCachedResultSet("cachefile1"),1);
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet("cachefile2"),1);
		checkSuccess(cur.getField(7,0),"char8");
		checkSuccess(cur.getField(8,0),null);
		System.out.println();
	
		System.out.println("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: ");
		cur.setResultSetBufferSize(2);
		cur.cacheToFile("cachefile2");
		checkSuccess(cur.openCachedResultSet("cachefile1"),1);
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet("cachefile2"),1);
		checkSuccess(cur.getField(7,0),"char8");
		checkSuccess(cur.getField(8,0),null);
		cur.setResultSetBufferSize(0);
		System.out.println();
	
		System.out.println("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: ");
		cur.setResultSetBufferSize(2);
		cur.cacheToFile("cachefile1");
		cur.setCacheTtl(200);
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(cur.getField(2,0),"char3");
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
		checkSuccess(cur.getField(7,0),"char8");
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
		checkSuccess(cur.getField(7,0),"char8");
		checkSuccess(cur.getField(8,0),null);
		cur.setResultSetBufferSize(0);
		System.out.println();
	
		System.out.println("COMMIT AND ROLLBACK: ");
		SQLRConnection secondcon=new SQLRConnection(args[0],
					(short)Integer.parseInt(args[1]), 
						args[2],args[3],args[4],0,1);
		SQLRCursor secondcur=new SQLRCursor(secondcon);
		checkSuccess(secondcur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(secondcur.getField(0,0),"char1");
		checkSuccess(con.commit(),1);
		checkSuccess(secondcur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(secondcur.getField(0,0),"char1");
		checkSuccess(con.autoCommitOn(),1);
		checkSuccess(cur.sendQuery("insert into testtable values ('char10','01-Jan-2010',10,10.00,10.1,'text10','10:00:00',10)"),1);
		checkSuccess(secondcur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(secondcur.getField(8,0),"char10");
		checkSuccess(con.autoCommitOff(),1);
		System.out.println();

		System.out.println("FINISHED SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(cur.getField(4,2),"5");
		checkSuccess(cur.getField(5,2),"6");
		checkSuccess(cur.getField(6,2),"7");
		checkSuccess(cur.getField(7,2),"8");
		id=cur.getResultSetId();
		cur.suspendResultSet();
		checkSuccess(con.suspendSession(),1);
		port=con.getConnectionPort();
		socket=con.getConnectionSocket();
		checkSuccess(con.resumeSession(port,socket),1);
		checkSuccess(cur.resumeResultSet(id),1);
		checkSuccess(cur.getField(4,2),null);
		checkSuccess(cur.getField(5,2),null);
		checkSuccess(cur.getField(6,2),null);
		checkSuccess(cur.getField(7,2),null);
		System.out.println();
	
		// drop existing table
		cur.sendQuery("drop table testtable");
	
		// invalid queries...
		System.out.println("INVALID QUERIES: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),0);
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
