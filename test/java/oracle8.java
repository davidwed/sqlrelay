// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

import com.firstworks.sqlrelay.SQLRConnection;
import com.firstworks.sqlrelay.SQLRCursor;



class oracle8 {
	
	
	
	
	
	
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
	
	public static void	main(String[] args) {
		String	dbtype;
		String[]	bindvars={"1","2","3","4","5"};
		String[]	bindvals={"4","testchar4","testvarchar4","01-JAN-2004","testlong4"};
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
		int	port;
		String	socket;
		int	id;
		String	filename;
		String[]	arraybindvars={"var1","var2","var3","var4","var5"};
		String[]	arraybindvals={"7","testchar7","testvarchar7","01-JAN-2007","testlong7"};
		long[]	fieldlens;
	
	
		// usage...
		if (args.length<5) {
			System.out.println("usage: java oracle8 host port socket user password");
			System.exit(0);
		}
	
	
		// instantiation
		SQLRConnection con=new SQLRConnection(args[0],Integer.parseInt(args[1]), 
						args[2],args[3],args[4],0,1);
		SQLRCursor cur=new SQLRCursor(con);
	
		// get database type
		System.out.println("IDENTIFY: ");
		checkSuccess(con.identify(),"oracle8");
		System.out.println();
	
		// ping
		System.out.println("PING: ");
		checkSuccess(con.ping(),1);
		System.out.println();
	
		// drop existing table
		cur.sendQuery("drop table testtable");
	
		System.out.println("CREATE TEMPTABLE: ");
		checkSuccess(cur.sendQuery("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long)"),1);
		System.out.println();
	
		System.out.println("INSERT: ");
		checkSuccess(cur.sendQuery("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1')"),1);
		System.out.println();
	
		System.out.println("AFFECTED ROWS: ");
		checkSuccess(cur.affectedRows(),1);
		System.out.println();
	
		System.out.println("BIND BY POSITION: ");
		cur.prepareQuery("insert into testtable values (:1,:2,:3,:4,:5)");
		cur.inputBind("1",2);
		cur.inputBind("2","testchar2");
		cur.inputBind("3","testvarchar2");
		cur.inputBind("4","01-JAN-2002");
		cur.inputBind("5","testlong2");
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("1",3);
		cur.inputBind("2","testchar3");
		cur.inputBind("3","testvarchar3");
		cur.inputBind("4","01-JAN-2003");
		cur.inputBind("5","testlong3");
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("ARRAY OF BINDS BY POSITION: ");
		cur.prepareQuery("insert into testtable values (:1,:2,:3,:4,:5)");
		cur.inputBinds(bindvars,bindvals);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("BIND BY NAME: ");
		cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5)");
		cur.inputBind("var1",5);
		cur.inputBind("var2","testchar5");
		cur.inputBind("var3","testvarchar5");
		cur.inputBind("var4","01-JAN-2005");
		cur.inputBind("var5","testlong5");
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("var1",6);
		cur.inputBind("var2","testchar6");
		cur.inputBind("var3","testvarchar6");
		cur.inputBind("var4","01-JAN-2006");
		cur.inputBind("var5","testlong6");
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("ARRAY OF BINDS BY NAME: ");
		cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5)");
		cur.inputBinds(arraybindvars,arraybindvals);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("BIND BY NAME WITH VALIDATION: ");
		cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5)");
		cur.inputBind("var1",8);
		cur.inputBind("var2","testchar8");
		cur.inputBind("var3","testvarchar8");
		cur.inputBind("var4","01-JAN-2008");
		cur.inputBind("var5","testlong8");
		cur.inputBind("var6","junkvalue");
		cur.validateBinds();
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("OUTPUT BIND BY NAME: ");
		cur.prepareQuery("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;");
		cur.defineOutputBind("numvar",10);
		cur.defineOutputBind("stringvar",10);
		cur.defineOutputBind("floatvar",10);
		checkSuccess(cur.executeQuery(),1);
		numvar=cur.getOutputBind("numvar");
		stringvar=cur.getOutputBind("stringvar");
		floatvar=cur.getOutputBind("floatvar");
		checkSuccess(numvar,"1");
		checkSuccess(stringvar,"hello");
		checkSuccess(floatvar,"2.5");
		System.out.println();
	
		System.out.println("OUTPUT BIND BY NAME WITH VALIDATION: ");
		cur.clearBinds();
		cur.defineOutputBind("numvar",10);
		cur.defineOutputBind("stringvar",10);
		cur.defineOutputBind("floatvar",10);
		cur.defineOutputBind("dummyvar",10);
		cur.validateBinds();
		checkSuccess(cur.executeQuery(),1);
		numvar=cur.getOutputBind("numvar");
		stringvar=cur.getOutputBind("stringvar");
		floatvar=cur.getOutputBind("floatvar");
		checkSuccess(numvar,"1");
		checkSuccess(stringvar,"hello");
		checkSuccess(floatvar,"2.5");
		System.out.println();
	
		System.out.println("OUTPUT BIND BY POSITION: ");
		cur.prepareQuery("begin  :1:=1; :2:='hello'; :3:=2.5; end;");
		cur.defineOutputBind("1",10);
		cur.defineOutputBind("2",10);
		cur.defineOutputBind("3",10);
		checkSuccess(cur.executeQuery(),1);
		numvar=cur.getOutputBind("1");
		stringvar=cur.getOutputBind("2");
		floatvar=cur.getOutputBind("3");
		checkSuccess(numvar,"1");
		checkSuccess(stringvar,"hello");
		checkSuccess(floatvar,"2.5");
		System.out.println();
	
		System.out.println("SELECT: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
		System.out.println();
	
		System.out.println("COLUMN COUNT: ");
		checkSuccess(cur.colCount(),5);
		System.out.println();
	
		System.out.println("COLUMN NAMES: ");
		checkSuccess(cur.getColumnName(0),"TESTNUMBER");
		checkSuccess(cur.getColumnName(1),"TESTCHAR");
		checkSuccess(cur.getColumnName(2),"TESTVARCHAR");
		checkSuccess(cur.getColumnName(3),"TESTDATE");
		checkSuccess(cur.getColumnName(4),"TESTLONG");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"TESTNUMBER");
		checkSuccess(cols[1],"TESTCHAR");
		checkSuccess(cols[2],"TESTVARCHAR");
		checkSuccess(cols[3],"TESTDATE");
		checkSuccess(cols[4],"TESTLONG");
		System.out.println();
	
		System.out.println("COLUMN TYPES: ");
		checkSuccess(cur.getColumnType(0),"NUMBER");
		checkSuccess(cur.getColumnType("testnumber"),"NUMBER");
		checkSuccess(cur.getColumnType(1),"CHAR");
		checkSuccess(cur.getColumnType("testchar"),"CHAR");
		checkSuccess(cur.getColumnType(2),"VARCHAR2");
		checkSuccess(cur.getColumnType("testvarchar"),"VARCHAR2");
		checkSuccess(cur.getColumnType(3),"DATE");
		checkSuccess(cur.getColumnType("testdate"),"DATE");
		checkSuccess(cur.getColumnType(4),"LONG");
		checkSuccess(cur.getColumnType("testlong"),"LONG");
		System.out.println();
	
		System.out.println("COLUMN LENGTH: ");
		checkSuccess(cur.getColumnLength(0),22);
		checkSuccess(cur.getColumnLength("testnumber"),22);
		checkSuccess(cur.getColumnLength(1),40);
		checkSuccess(cur.getColumnLength("testchar"),40);
		checkSuccess(cur.getColumnLength(2),40);
		checkSuccess(cur.getColumnLength("testvarchar"),40);
		checkSuccess(cur.getColumnLength(3),7);
		checkSuccess(cur.getColumnLength("testdate"),7);
		checkSuccess(cur.getColumnLength(4),0);
		checkSuccess(cur.getColumnLength("testlong"),0);
		System.out.println();
	
		System.out.println("LONGEST COLUMN: ");
		checkSuccess(cur.getLongest(0),1);
		checkSuccess(cur.getLongest("testnumber"),1);
		checkSuccess(cur.getLongest(1),40);
		checkSuccess(cur.getLongest("testchar"),40);
		checkSuccess(cur.getLongest(2),12);
		checkSuccess(cur.getLongest("testvarchar"),12);
		checkSuccess(cur.getLongest(3),9);
		checkSuccess(cur.getLongest("testdate"),9);
		System.out.println();
	
		System.out.println("ROW COUNT: ");
		checkSuccess(cur.rowCount(),8);
		System.out.println();
	
		System.out.println("TOTAL ROWS: ");
		checkSuccess(cur.totalRows(),-1);
		System.out.println();
	
		System.out.println("FIRST ROW INDEX: ");
		checkSuccess(cur.firstRowIndex(),0);
		System.out.println();
	
		System.out.println("END OF RESULT SET: ");
		checkSuccess(cur.endOfResultSet(),1);
		System.out.println();
	
		System.out.println("FIELDS BY INDEX: ");
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"testchar1                               ");
		checkSuccess(cur.getField(0,2),"testvarchar1");
		checkSuccess(cur.getField(0,3),"01-JAN-01");
		checkSuccess(cur.getField(0,4),"testlong1");
		System.out.println();
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(7,1),"testchar8                               ");
		checkSuccess(cur.getField(7,2),"testvarchar8");
		checkSuccess(cur.getField(7,3),"01-JAN-08");
		checkSuccess(cur.getField(7,4),"testlong8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY INDEX: ");
		checkSuccess(cur.getFieldLength(0,0),1);
		checkSuccess(cur.getFieldLength(0,1),40);
		checkSuccess(cur.getFieldLength(0,2),12);
		checkSuccess(cur.getFieldLength(0,3),9);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,0),1);
		checkSuccess(cur.getFieldLength(7,1),40);
		checkSuccess(cur.getFieldLength(7,2),12);
		checkSuccess(cur.getFieldLength(7,3),9);
		System.out.println();
	
		System.out.println("FIELDS BY NAME: ");
		checkSuccess(cur.getField(0,"testnumber"),"1");
		checkSuccess(cur.getField(0,"testchar"),"testchar1                               ");
		checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1");
		checkSuccess(cur.getField(0,"testdate"),"01-JAN-01");
		checkSuccess(cur.getField(0,"testlong"),"testlong1");
		System.out.println();
		checkSuccess(cur.getField(7,"testnumber"),"8");
		checkSuccess(cur.getField(7,"testchar"),"testchar8                               ");
		checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8");
		checkSuccess(cur.getField(7,"testdate"),"01-JAN-08");
		checkSuccess(cur.getField(7,"testlong"),"testlong8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY NAME: ");
		checkSuccess(cur.getFieldLength(0,"testnumber"),1);
		checkSuccess(cur.getFieldLength(0,"testchar"),40);
		checkSuccess(cur.getFieldLength(0,"testvarchar"),12);
		checkSuccess(cur.getFieldLength(0,"testdate"),9);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,"testnumber"),1);
		checkSuccess(cur.getFieldLength(7,"testchar"),40);
		checkSuccess(cur.getFieldLength(7,"testvarchar"),12);
		checkSuccess(cur.getFieldLength(7,"testdate"),9);
		System.out.println();
	
		System.out.println("FIELDS BY ARRAY: ");
		fields=cur.getRow(0);
		checkSuccess(fields[0],"1");
		checkSuccess(fields[1],"testchar1                               ");
		checkSuccess(fields[2],"testvarchar1");
		checkSuccess(fields[3],"01-JAN-01");
		checkSuccess(fields[4],"testlong1");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY ARRAY: ");
		fieldlens=cur.getRowLengths(0);
		checkSuccess(fieldlens[0],1);
		checkSuccess(fieldlens[1],40);
		checkSuccess(fieldlens[2],12);
		checkSuccess(fieldlens[3],9);
		System.out.println();
	
		System.out.println("INDIVIDUAL SUBSTITUTIONS: ");
		cur.prepareQuery("select $(var1),'$(var2)',$(var3) from dual");
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
	
		System.out.println("OUTPUT BIND: ");
		cur.prepareQuery("begin :var1:='hello'; end;");
		cur.defineOutputBind("var1",10);
		checkSuccess(cur.executeQuery(),1);
		checkSuccess(cur.getOutputBind("var1"),"hello");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("select $(var1),$(var2),$(var3) from dual");
		cur.substitutions(subvars,subvallongs);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
		
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"2");
		checkSuccess(cur.getField(0,2),"3");
		System.out.println();
		
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("select '$(var1)','$(var2)','$(var3)' from dual");
		cur.substitutions(subvars,subvalstrings);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"hi");
		checkSuccess(cur.getField(0,1),"hello");
		checkSuccess(cur.getField(0,2),"bye");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("select $(var1),$(var2),$(var3) from dual");
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
		checkSuccess(cur.sendQuery("select null,1,null from dual"),1);
		checkSuccess(cur.getField(0,0),null);
		checkSuccess(cur.getField(0,1),"1");
		checkSuccess(cur.getField(0,2),null);
		cur.getNullsAsEmptyStrings();
		checkSuccess(cur.sendQuery("select null,1,null from dual"),1);
		checkSuccess(cur.getField(0,0),"");
		checkSuccess(cur.getField(0,1),"1");
		checkSuccess(cur.getField(0,2),"");
		cur.getNullsAsNulls();
		System.out.println();
	
		System.out.println("RESULT SET BUFFER SIZE: ");
		checkSuccess(cur.getResultSetBufferSize(),0);
		cur.setResultSetBufferSize(2);
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
		checkSuccess(cur.getColumnName(0),null);
		checkSuccess(cur.getColumnLength(0),0);
		checkSuccess(cur.getColumnType(0),null);
		cur.getColumnInfo();
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
		checkSuccess(cur.getColumnName(0),"TESTNUMBER");
		checkSuccess(cur.getColumnLength(0),22);
		checkSuccess(cur.getColumnType(0),"NUMBER");
		System.out.println();
	
		System.out.println("SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
		filename=cur.getCacheFileName();
		checkSuccess(filename,"cachefile1");
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet(filename),1);
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
	
		System.out.println("COLUMN COUNT FOR CACHED RESULT SET: ");
		checkSuccess(cur.colCount(),5);
		System.out.println();
	
		System.out.println("COLUMN NAMES FOR CACHED RESULT SET: ");
		checkSuccess(cur.getColumnName(0),"TESTNUMBER");
		checkSuccess(cur.getColumnName(1),"TESTCHAR");
		checkSuccess(cur.getColumnName(2),"TESTVARCHAR");
		checkSuccess(cur.getColumnName(3),"TESTDATE");
		checkSuccess(cur.getColumnName(4),"TESTLONG");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"TESTNUMBER");
		checkSuccess(cols[1],"TESTCHAR");
		checkSuccess(cols[2],"TESTVARCHAR");
		checkSuccess(cols[3],"TESTDATE");
		checkSuccess(cols[4],"TESTLONG");
		System.out.println();
	
		System.out.println("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: ");
		cur.setResultSetBufferSize(2);
		cur.cacheToFile("cachefile1");
		cur.setCacheTtl(200);
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
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
	
		System.out.println("COMMIT AND ROLLBACK: ");
		SQLRConnection secondcon=new SQLRConnection(args[0],
					Integer.parseInt(args[1]), 
					args[2],args[3],args[4],0,1);
		SQLRCursor secondcur=new SQLRCursor(secondcon);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"0");
		checkSuccess(con.commit(),1);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"8");
		checkSuccess(con.autoCommitOn(),1);
		checkSuccess(cur.sendQuery("insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10')"),1);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"9");
		checkSuccess(con.autoCommitOff(),1);
		System.out.println();

		System.out.println("FINISHED SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
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
		cur.sendQuery("drop table testtable");
	
		// invalid queries...
		System.out.println("INVALID QUERIES: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),0);
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
