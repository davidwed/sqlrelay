// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

import com.firstworks.sqlrelay.SQLRConnection;
import com.firstworks.sqlrelay.SQLRCursor;


class oracle {

	private static void checkSuccess(String value, String success, int length) {
	
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
	
		if (value.regionMatches(0,success,0,length)) {
			System.out.print("success ");
		} else {
			System.out.print(value+"!="+success+" ");
			System.out.print("failure ");
			System.exit(1);
		}
	}
	
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
	
	private static void checkSuccess(byte[] value, String success, int length) {
	
		if (success==null) {
			if (value==null) {
				System.out.print("success ");
				return;
			} else {
				System.out.print("failure ");
				
				
				System.exit(1);
			}
		}

		byte[]	successvalue=success.getBytes();
	
		for (int index=0; index<length; index++) {
			if (value[index]!=successvalue[index]) {
				System.out.println("failure ");
				System.exit(1);
			}
		}

		System.out.println("success ");
	}
	
	private static void checkSuccess(long value, int success) {
	
		if (value==success) {
			System.out.println("success ");
		} else {
			System.out.println("failure ");
			
			
			System.exit(1);
		}
	}
	
	private static void checkSuccess(double value, double success) {
	
		if (value==success) {
			System.out.println("success ");
		} else {
			System.out.println("failure ");
			
			
			System.exit(1);
		}
	}
	
	private static void checkSuccess(boolean value, int success) {
	
		if (((value)?1:0)==success) {
			System.out.println("success ");
		} else {
			System.out.println("failure ");
			
			
			System.exit(1);
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
		String	clobvar;
		long	clobvarlength;
		byte[]	blobvar;
		long	blobvarlength;
		long	numvar;
		String	stringvar;
		double	floatvar;
		String[]	cols;
		String[]	fields;
		short	port;
		String	socket;
		short	id;
		String	filename;
		String[]	arraybindvars={"var1","var2","var3","var4","var5"};
		String[]	arraybindvals={"7","testchar7","testvarchar7","01-JAN-2007","testlong7"};
		long[]	fieldlens;
	
	
		// instantiation
		SQLRConnection con=new SQLRConnection("sqlrelay",
						(short)9000,
						"/tmp/test.socket",
						"test","test",0,1);
		SQLRCursor cur=new SQLRCursor(con);
	
		// get database type
		System.out.println("IDENTIFY: ");
		checkSuccess(con.identify(),"oracle");
		System.out.println();
	
		// ping
		System.out.println("PING: ");
		checkSuccess(con.ping(),1);
		System.out.println();

		System.out.println("BIND VALIDATION: ");
		cur.sendQuery("drop table testtable1");
		cur.sendQuery("create table testtable1 (col1 varchar2(20), col2 varchar2(20), col3 varchar2(20))");
		cur.prepareQuery("insert into testtable1 values ($(var1),$(var2),$(var3))");
		cur.inputBind("var1",1);
		cur.inputBind("var2",2);
		cur.inputBind("var3",3);
		cur.substitution("var1",":var1");
		checkSuccess(cur.validBind("var1"),1);
		checkSuccess(cur.validBind("var2"),0);
		checkSuccess(cur.validBind("var3"),0);
		checkSuccess(cur.validBind("var4"),0);
		System.out.println();
		cur.substitution("var2",":var2");
		checkSuccess(cur.validBind("var1"),1);
		checkSuccess(cur.validBind("var2"),1);
		checkSuccess(cur.validBind("var3"),0);
		checkSuccess(cur.validBind("var4"),0);
		System.out.println();
		cur.substitution("var3",":var3");
		checkSuccess(cur.validBind("var1"),1);
		checkSuccess(cur.validBind("var2"),1);
		checkSuccess(cur.validBind("var3"),1);
		checkSuccess(cur.validBind("var4"),0);
		checkSuccess(cur.executeQuery(),1);
		cur.sendQuery("drop table testtable1");
		System.out.println();
	
		// drop existing table
		cur.sendQuery("drop table testtable");
	
		System.out.println("CREATE TEMPTABLE: ");
		checkSuccess(cur.sendQuery("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long, testclob clob, testblob blob)"),1);
		System.out.println();
	
		System.out.println("INSERT: ");
		checkSuccess(cur.sendQuery("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1','testclob1',empty_blob())"),1);
		System.out.println();
	
		System.out.println("AFFECTED ROWS: ");
		checkSuccess(cur.affectedRows(),1);
		System.out.println();
	
		System.out.println("BIND BY POSITION: ");
		cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
		checkSuccess(cur.countBindVariables(),7);
		cur.inputBind("1",2);
		cur.inputBind("2","testchar2");
		cur.inputBind("3","testvarchar2");
		cur.inputBind("4","01-JAN-2002");
		cur.inputBind("5","testlong2");
		cur.inputBindClob("6","testclob2",9);
		cur.inputBindBlob("7",(new String("testblob2")).getBytes(),9);
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("1",3);
		cur.inputBind("2","testchar3");
		cur.inputBind("3","testvarchar3");
		cur.inputBind("4","01-JAN-2003");
		cur.inputBind("5","testlong3");
		cur.inputBindClob("6","testclob3",9);
		cur.inputBindBlob("7",(new String("testblob3")).getBytes(),9);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("ARRAY OF BINDS BY POSITION: ");
		cur.clearBinds();
		cur.inputBinds(bindvars,bindvals);
		cur.inputBindClob("var6","testclob4",9);
		cur.inputBindBlob("var7",
				(new String("testblob4")).getBytes(),9);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("BIND BY NAME: ");
		cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
		cur.inputBind("var1",5);
		cur.inputBind("var2","testchar5");
		cur.inputBind("var3","testvarchar5");
		cur.inputBind("var4","01-JAN-2005");
		cur.inputBind("var5","testlong5");
		cur.inputBindClob("var6","testclob5",9);
		cur.inputBindBlob("var7",
				(new String("testblob5")).getBytes(),9);
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("var1",6);
		cur.inputBind("var2","testchar6");
		cur.inputBind("var3","testvarchar6");
		cur.inputBind("var4","01-JAN-2006");
		cur.inputBind("var5","testlong6");
		cur.inputBindClob("var6","testclob6",9);
		cur.inputBindBlob("var7",
				(new String("testblob6")).getBytes(),9);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("ARRAY OF BINDS BY NAME: ");
		cur.clearBinds();
		cur.inputBinds(arraybindvars,arraybindvals);
		cur.inputBindClob("var6","testclob7",9);
		cur.inputBindBlob("var7",
				(new String("testblob7")).getBytes(),9);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("BIND BY NAME WITH VALIDATION: ");
		cur.clearBinds();
		cur.inputBind("var1",8);
		cur.inputBind("var2","testchar8");
		cur.inputBind("var3","testvarchar8");
		cur.inputBind("var4","01-JAN-2008");
		cur.inputBind("var5","testlong8");
		cur.inputBindClob("var6","testclob8",9);
		cur.inputBindBlob("var7",
				(new String("testblob8")).getBytes(),9);
		cur.inputBind("var9","junkvalue");
		cur.validateBinds();
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("OUTPUT BIND BY NAME: ");
		cur.prepareQuery("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;");
		cur.defineOutputBindInteger("numvar");
		cur.defineOutputBindString("stringvar",10);
		cur.defineOutputBindDouble("floatvar");
		checkSuccess(cur.executeQuery(),1);
		numvar=cur.getOutputBindInteger("numvar");
		stringvar=cur.getOutputBindString("stringvar");
		floatvar=cur.getOutputBindDouble("floatvar");
		checkSuccess(numvar,1);
		checkSuccess(stringvar,"hello");
		checkSuccess(floatvar,2.5);
		System.out.println();
	
		System.out.println("OUTPUT BIND BY POSITION: ");
		cur.clearBinds();
		cur.defineOutputBindInteger("1");
		cur.defineOutputBindString("2",10);
		cur.defineOutputBindDouble("3");
		checkSuccess(cur.executeQuery(),1);
		numvar=cur.getOutputBindInteger("1");
		stringvar=cur.getOutputBindString("2");
		floatvar=cur.getOutputBindDouble("3");
		checkSuccess(numvar,1);
		checkSuccess(stringvar,"hello");
		checkSuccess(floatvar,2.5);
		System.out.println();
	
		System.out.println("OUTPUT BIND BY NAME WITH VALIDATION: ");
		cur.clearBinds();
		cur.defineOutputBindInteger("numvar");
		cur.defineOutputBindString("stringvar",10);
		cur.defineOutputBindDouble("floatvar");
		cur.defineOutputBindString("dummyvar",10);
		cur.validateBinds();
		checkSuccess(cur.executeQuery(),1);
		numvar=cur.getOutputBindInteger("numvar");
		stringvar=cur.getOutputBindString("stringvar");
		floatvar=cur.getOutputBindDouble("floatvar");
		checkSuccess(numvar,1);
		checkSuccess(stringvar,"hello");
		checkSuccess(floatvar,2.5);
		System.out.println();
	
		System.out.println("SELECT: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testnumber"),1);
		System.out.println();
	
		System.out.println("COLUMN COUNT: ");
		checkSuccess(cur.colCount(),7);
		System.out.println();
	
		System.out.println("COLUMN NAMES: ");
		checkSuccess(cur.getColumnName(0),"TESTNUMBER");
		checkSuccess(cur.getColumnName(1),"TESTCHAR");
		checkSuccess(cur.getColumnName(2),"TESTVARCHAR");
		checkSuccess(cur.getColumnName(3),"TESTDATE");
		checkSuccess(cur.getColumnName(4),"TESTLONG");
		checkSuccess(cur.getColumnName(5),"TESTCLOB");
		checkSuccess(cur.getColumnName(6),"TESTBLOB");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"TESTNUMBER");
		checkSuccess(cols[1],"TESTCHAR");
		checkSuccess(cols[2],"TESTVARCHAR");
		checkSuccess(cols[3],"TESTDATE");
		checkSuccess(cols[4],"TESTLONG");
		checkSuccess(cols[5],"TESTCLOB");
		checkSuccess(cols[6],"TESTBLOB");
		System.out.println();
	
		System.out.println("COLUMN TYPES: ");
		checkSuccess(cur.getColumnType(0),"NUMBER");
		checkSuccess(cur.getColumnType("TESTNUMBER"),"NUMBER");
		checkSuccess(cur.getColumnType(1),"CHAR");
		checkSuccess(cur.getColumnType("TESTCHAR"),"CHAR");
		checkSuccess(cur.getColumnType(2),"VARCHAR2");
		checkSuccess(cur.getColumnType("TESTVARCHAR"),"VARCHAR2");
		checkSuccess(cur.getColumnType(3),"DATE");
		checkSuccess(cur.getColumnType("TESTDATE"),"DATE");
		checkSuccess(cur.getColumnType(4),"LONG");
		checkSuccess(cur.getColumnType("TESTLONG"),"LONG");
		checkSuccess(cur.getColumnType(5),"CLOB");
		checkSuccess(cur.getColumnType("TESTCLOB"),"CLOB");
		checkSuccess(cur.getColumnType(6),"BLOB");
		checkSuccess(cur.getColumnType("TESTBLOB"),"BLOB");
		System.out.println();
	
		System.out.println("COLUMN LENGTH: ");
		checkSuccess(cur.getColumnLength(0),22);
		checkSuccess(cur.getColumnLength("TESTNUMBER"),22);
		checkSuccess(cur.getColumnLength(1),40);
		checkSuccess(cur.getColumnLength("TESTCHAR"),40);
		checkSuccess(cur.getColumnLength(2),40);
		checkSuccess(cur.getColumnLength("TESTVARCHAR"),40);
		checkSuccess(cur.getColumnLength(3),7);
		checkSuccess(cur.getColumnLength("TESTDATE"),7);
		checkSuccess(cur.getColumnLength(4),0);
		checkSuccess(cur.getColumnLength("TESTLONG"),0);
		checkSuccess(cur.getColumnLength(5),0);
		checkSuccess(cur.getColumnLength("TESTCLOB"),0);
		checkSuccess(cur.getColumnLength(6),0);
		checkSuccess(cur.getColumnLength("TESTBLOB"),0);
		System.out.println();
	
		System.out.println("LONGEST COLUMN: ");
		checkSuccess(cur.getLongest(0),1);
		checkSuccess(cur.getLongest("TESTNUMBER"),1);
		checkSuccess(cur.getLongest(1),40);
		checkSuccess(cur.getLongest("TESTCHAR"),40);
		checkSuccess(cur.getLongest(2),12);
		checkSuccess(cur.getLongest("TESTVARCHAR"),12);
		checkSuccess(cur.getLongest(3),9);
		checkSuccess(cur.getLongest("TESTDATE"),9);
		checkSuccess(cur.getLongest(4),9);
		checkSuccess(cur.getLongest("TESTLONG"),9);
		checkSuccess(cur.getLongest(5),9);
		checkSuccess(cur.getLongest("TESTCLOB"),9);
		checkSuccess(cur.getLongest(6),9);
		checkSuccess(cur.getLongest("TESTBLOB"),9);
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
		checkSuccess(cur.getField(0,1),"testchar1                               ");
		checkSuccess(cur.getField(0,2),"testvarchar1");
		checkSuccess(cur.getField(0,3),"01-JAN-01");
		checkSuccess(cur.getField(0,4),"testlong1");
		checkSuccess(cur.getField(0,5),"testclob1");
		checkSuccess(cur.getField(0,6),"");
		System.out.println();
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(7,1),"testchar8                               ");
		checkSuccess(cur.getField(7,2),"testvarchar8");
		checkSuccess(cur.getField(7,3),"01-JAN-08");
		checkSuccess(cur.getField(7,4),"testlong8");
		checkSuccess(cur.getField(7,5),"testclob8");
		checkSuccess(cur.getField(7,6),"testblob8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY INDEX: ");
		checkSuccess(cur.getFieldLength(0,0),1);
		checkSuccess(cur.getFieldLength(0,1),40);
		checkSuccess(cur.getFieldLength(0,2),12);
		checkSuccess(cur.getFieldLength(0,3),9);
		checkSuccess(cur.getFieldLength(0,4),9);
		checkSuccess(cur.getFieldLength(0,5),9);
		checkSuccess(cur.getFieldLength(0,6),0);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,0),1);
		checkSuccess(cur.getFieldLength(7,1),40);
		checkSuccess(cur.getFieldLength(7,2),12);
		checkSuccess(cur.getFieldLength(7,3),9);
		checkSuccess(cur.getFieldLength(7,4),9);
		checkSuccess(cur.getFieldLength(7,5),9);
		checkSuccess(cur.getFieldLength(7,6),9);
		System.out.println();
	
		System.out.println("FIELDS BY NAME: ");
		checkSuccess(cur.getField(0,"TESTNUMBER"),"1");
		checkSuccess(cur.getField(0,"TESTCHAR"),"testchar1                               ");
		checkSuccess(cur.getField(0,"TESTVARCHAR"),"testvarchar1");
		checkSuccess(cur.getField(0,"TESTDATE"),"01-JAN-01");
		checkSuccess(cur.getField(0,"TESTLONG"),"testlong1");
		checkSuccess(cur.getField(0,"TESTCLOB"),"testclob1");
		checkSuccess(cur.getField(0,"TESTBLOB"),"");
		System.out.println();
		checkSuccess(cur.getField(7,"TESTNUMBER"),"8");
		checkSuccess(cur.getField(7,"TESTCHAR"),"testchar8                               ");
		checkSuccess(cur.getField(7,"TESTVARCHAR"),"testvarchar8");
		checkSuccess(cur.getField(7,"TESTDATE"),"01-JAN-08");
		checkSuccess(cur.getField(7,"TESTLONG"),"testlong8");
		checkSuccess(cur.getField(7,"TESTCLOB"),"testclob8");
		checkSuccess(cur.getField(7,"TESTBLOB"),"testblob8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY NAME: ");
		checkSuccess(cur.getFieldLength(0,"TESTNUMBER"),1);
		checkSuccess(cur.getFieldLength(0,"TESTCHAR"),40);
		checkSuccess(cur.getFieldLength(0,"TESTVARCHAR"),12);
		checkSuccess(cur.getFieldLength(0,"TESTDATE"),9);
		checkSuccess(cur.getFieldLength(0,"TESTLONG"),9);
		checkSuccess(cur.getFieldLength(0,"TESTCLOB"),9);
		checkSuccess(cur.getFieldLength(0,"TESTBLOB"),0);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,"TESTNUMBER"),1);
		checkSuccess(cur.getFieldLength(7,"TESTCHAR"),40);
		checkSuccess(cur.getFieldLength(7,"TESTVARCHAR"),12);
		checkSuccess(cur.getFieldLength(7,"TESTDATE"),9);
		checkSuccess(cur.getFieldLength(7,"TESTLONG"),9);
		checkSuccess(cur.getFieldLength(7,"TESTCLOB"),9);
		checkSuccess(cur.getFieldLength(7,"TESTBLOB"),9);
		System.out.println();
	
		System.out.println("FIELDS BY ARRAY: ");
		fields=cur.getRow(0);
		checkSuccess(fields[0],"1");
		checkSuccess(fields[1],"testchar1                               ");
		checkSuccess(fields[2],"testvarchar1");
		checkSuccess(fields[3],"01-JAN-01");
		checkSuccess(fields[4],"testlong1");
		checkSuccess(fields[5],"testclob1");
		checkSuccess(fields[6],"");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY ARRAY: ");
		fieldlens=cur.getRowLengths(0);
		checkSuccess(fieldlens[0],1);
		checkSuccess(fieldlens[1],40);
		checkSuccess(fieldlens[2],12);
		checkSuccess(fieldlens[3],9);
		checkSuccess(fieldlens[4],9);
		checkSuccess(fieldlens[5],9);
		checkSuccess(fieldlens[6],0);
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
		cur.defineOutputBindString("var1",10);
		checkSuccess(cur.executeQuery(),1);
		checkSuccess(cur.getOutputBindString("var1"),"hello");
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
		checkSuccess(cur.colCount(),7);
		System.out.println();
	
		System.out.println("COLUMN NAMES FOR CACHED RESULT SET: ");
		checkSuccess(cur.getColumnName(0),"TESTNUMBER");
		checkSuccess(cur.getColumnName(1),"TESTCHAR");
		checkSuccess(cur.getColumnName(2),"TESTVARCHAR");
		checkSuccess(cur.getColumnName(3),"TESTDATE");
		checkSuccess(cur.getColumnName(4),"TESTLONG");
		checkSuccess(cur.getColumnName(5),"TESTCLOB");
		checkSuccess(cur.getColumnName(6),"TESTBLOB");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"TESTNUMBER");
		checkSuccess(cols[1],"TESTCHAR");
		checkSuccess(cols[2],"TESTVARCHAR");
		checkSuccess(cols[3],"TESTDATE");
		checkSuccess(cols[4],"TESTLONG");
		checkSuccess(cols[5],"TESTCLOB");
		checkSuccess(cols[6],"TESTBLOB");
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
		SQLRConnection secondcon=new SQLRConnection("sqlrelay",
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
		checkSuccess(cur.sendQuery("insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10','testclob10',empty_blob())"),1);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"9");
		checkSuccess(con.autoCommitOff(),1);
		System.out.println();


		System.out.println("CLOB AND BLOB OUTPUT BIND:");
		cur.sendQuery("drop table testtable1");
		checkSuccess(cur.sendQuery("create table testtable1 (testclob clob, testblob blob)"),1);
		cur.prepareQuery("insert into testtable1 values ('hello',:var1)");
		cur.inputBindBlob("var1",(new String("hello")).getBytes(),5);
		checkSuccess(cur.executeQuery(),1);
		cur.prepareQuery("begin select testclob into :clobvar from testtable1;  select testblob into :blobvar from testtable1; end;");
		cur.defineOutputBindClob("clobvar");
		cur.defineOutputBindBlob("blobvar");
		checkSuccess(cur.executeQuery(),1);
		clobvar=cur.getOutputBindClob("clobvar");
		clobvarlength=cur.getOutputBindLength("clobvar");
		blobvar=cur.getOutputBindBlob("blobvar");
		blobvarlength=cur.getOutputBindLength("blobvar");
		checkSuccess(clobvar,"hello",5);
		checkSuccess(clobvarlength,5);
		checkSuccess(blobvar,"hello",5);
		checkSuccess(blobvarlength,5);
		cur.sendQuery("drop table testtable1");
		System.out.println();

		System.out.println("NULL AND EMPTY CLOBS AND CLOBS:");
		cur.getNullsAsNulls();
		cur.sendQuery("create table testtable1 (testclob1 clob, testclob2 clob, testblob1 blob, testblob2 blob)");
		cur.prepareQuery("insert into testtable1 values (:var1,:var2,:var3,:var4)");
		cur.inputBindClob("var1","",0);
		cur.inputBindClob("var2",null,0);
		cur.inputBindBlob("var3",(new String("")).getBytes(),0);
		cur.inputBindBlob("var4",null,0);
		checkSuccess(cur.executeQuery(),1);
		cur.sendQuery("select * from testtable1");
		checkSuccess(cur.getField(0,0),null);
		checkSuccess(cur.getField(0,1),null);
		checkSuccess(cur.getField(0,2),null);
		checkSuccess(cur.getField(0,3),null);
		cur.sendQuery("drop table testtable1");
		System.out.println();

		System.out.println("CURSOR BINDS:");
		checkSuccess(cur.sendQuery("create or replace package types as type cursorType is ref cursor; end;"),1);
		checkSuccess(cur.sendQuery("create or replace function sp_testtable return types.cursortype as l_cursor    types.cursorType; begin open l_cursor for select * from testtable; return l_cursor; end;"),1);
		cur.prepareQuery("begin  :curs:=sp_testtable; end;");
		cur.defineOutputBindCursor("curs");
		checkSuccess(cur.executeQuery(),1);
		SQLRCursor	bindcur=cur.getOutputBindCursor("curs");
		checkSuccess(bindcur.fetchFromBindCursor(),1);
		checkSuccess(bindcur.getField(0,0),"1");
		checkSuccess(bindcur.getField(1,0),"2");
		checkSuccess(bindcur.getField(2,0),"3");
		checkSuccess(bindcur.getField(3,0),"4");
		checkSuccess(bindcur.getField(4,0),"5");
		checkSuccess(bindcur.getField(5,0),"6");
		checkSuccess(bindcur.getField(6,0),"7");
		checkSuccess(bindcur.getField(7,0),"8");
		System.out.println();

		System.out.println("LONG CLOB:");
		cur.sendQuery("drop table testtable2");
		cur.sendQuery("create table testtable2 (testclob clob)");
		cur.prepareQuery("insert into testtable2 values (:clobval)");
		StringBuffer	clobval=new StringBuffer();
		for (int i=0; i<8*1024; i++) {
			clobval.append('C');
		}
		cur.inputBindClob("clobval",clobval.toString(),8*1024);
		checkSuccess(cur.executeQuery(),1);
		cur.sendQuery("select testclob from testtable2");
		checkSuccess(clobval.toString(),cur.getField(0,"TESTCLOB"));
		cur.prepareQuery("begin select testclob into :clobbindval from testtable2; end;");
		cur.defineOutputBindClob("clobbindval");
		checkSuccess(cur.executeQuery(),1);
		String	clobbindvar=cur.getOutputBindClob("clobbindval");
		checkSuccess(cur.getOutputBindLength("clobbindval"),8*1024);
		checkSuccess(clobval.toString(),clobbindvar);
		cur.sendQuery("drop table testtable2");
		System.out.println();


		System.out.println("LONG OUTPUT BIND");
		cur.sendQuery("drop table testtable2");
		cur.sendQuery("create table testtable2 (testval varchar2(4000))");
		cur.prepareQuery("insert into testtable2 values (:testval)");
		StringBuffer	testval=new StringBuffer();
		for (int i=0; i<4000; i++) {
			testval.append('C');
		}
		cur.inputBind("testval",testval.toString());
		checkSuccess(cur.executeQuery(),1);
		cur.sendQuery("select testval from testtable2");
		checkSuccess(testval.toString(),cur.getField(0,"TESTVAL"));
		StringBuffer	query=new StringBuffer();
		query.append("begin :bindval:='");
		query.append(testval.toString());
		query.append("'; end;");
		cur.prepareQuery(query.toString());
		cur.defineOutputBindString("bindval",4000);
		checkSuccess(cur.executeQuery(),1);
		checkSuccess(cur.getOutputBindLength("bindval"),4000);
		checkSuccess(cur.getOutputBindString("bindval"),testval.toString());
		cur.sendQuery("drop table testtable2");
		System.out.println();

		System.out.println("NEGATIVE INPUT BIND");
		cur.sendQuery("create table testtable2 (testval number)");
		cur.prepareQuery("insert into testtable2 values (:testval)");
		cur.inputBind("testval",-1);
		checkSuccess(cur.executeQuery(),1);
		cur.sendQuery("select testval from testtable2");
		checkSuccess(cur.getField(0,"TESTVAL"),"-1");
		cur.sendQuery("drop table testtable2");
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
