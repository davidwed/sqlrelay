// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

import com.firstworks.sqlrelay.SQLRConnection;
import com.firstworks.sqlrelay.SQLRCursor;


class sqlite {
	
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
		checkSuccess(con.identify(),"sqlite");
		System.out.println();
	
		// ping
		System.out.println("PING: ");
		checkSuccess(con.ping(),1);
		System.out.println();
	
		// drop existing table
		cur.sendQuery("begin transaction");
		cur.sendQuery("drop table testtable");
		con.commit();
	
		// create a new table
		System.out.println("CREATE TEMPTABLE: ");
		cur.sendQuery("begin transaction");
		checkSuccess(cur.sendQuery("create table testtable (testint int, testfloat float, testchar char(40), testvarchar varchar(40))"),1);
		con.commit();
		System.out.println();
	
		System.out.println("INSERT: ");
		cur.sendQuery("begin transaction");
		checkSuccess(cur.sendQuery("insert into testtable values (1,1.1,'testchar1','testvarchar1')"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (2,2.2,'testchar2','testvarchar2')"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (3,3.3,'testchar3','testvarchar3')"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (4,4.4,'testchar4','testvarchar4')"),1);
		System.out.println();
	
		System.out.println("AFFECTED ROWS: ");
		checkSuccess(cur.affectedRows(),0);
		System.out.println();
	
		System.out.println("BIND BY NAME: ");
		cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4)");
		checkSuccess(cur.countBindVariables(),4);
		cur.inputBind("var1",5);
		cur.inputBind("var2",5.5,4,1);
		cur.inputBind("var3","testchar5");
		cur.inputBind("var4","testvarchar5");
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("var1",6);
		cur.inputBind("var2",6.6,4,1);
		cur.inputBind("var3","testchar6");
		cur.inputBind("var4","testvarchar6");
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("var1",7);
		cur.inputBind("var2",7.7,4,1);
		cur.inputBind("var3","testchar7");
		cur.inputBind("var4","testvarchar7");
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("BIND BY NAME WITH VALIDATION: ");
		cur.clearBinds();
		cur.inputBind("var1",8);
		cur.inputBind("var2",8.8,4,1);
		cur.inputBind("var3","testchar8");
		cur.inputBind("var4","testvarchar8");
		cur.validateBinds();
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("SELECT: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		System.out.println();
	
		System.out.println("COLUMN COUNT: ");
		checkSuccess(cur.colCount(),4);
		System.out.println();
	
		System.out.println("COLUMN NAMES: ");
		checkSuccess(cur.getColumnName(0),"testint");
		checkSuccess(cur.getColumnName(1),"testfloat");
		checkSuccess(cur.getColumnName(2),"testchar");
		checkSuccess(cur.getColumnName(3),"testvarchar");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"testint");
		checkSuccess(cols[1],"testfloat");
		checkSuccess(cols[2],"testchar");
		checkSuccess(cols[3],"testvarchar");
		System.out.println();
	
		System.out.println("COLUMN TYPES: ");
		checkSuccess(cur.getColumnType(0),"INTEGER");
		checkSuccess(cur.getColumnType("testint"),"INTEGER");
		checkSuccess(cur.getColumnType(1),"FLOAT");
		checkSuccess(cur.getColumnType("testfloat"),"FLOAT");
		checkSuccess(cur.getColumnType(2),"STRING");
		checkSuccess(cur.getColumnType("testchar"),"STRING");
		checkSuccess(cur.getColumnType(3),"STRING");
		checkSuccess(cur.getColumnType("testvarchar"),"STRING");
		System.out.println();
	
		System.out.println("COLUMN LENGTH: ");
		checkSuccess(cur.getColumnLength(0),0);
		checkSuccess(cur.getColumnLength("testint"),0);
		checkSuccess(cur.getColumnLength(1),0);
		checkSuccess(cur.getColumnLength("testfloat"),0);
		checkSuccess(cur.getColumnLength(2),0);
		checkSuccess(cur.getColumnLength("testchar"),0);
		checkSuccess(cur.getColumnLength(3),0);
		checkSuccess(cur.getColumnLength("testvarchar"),0);
		System.out.println();
	
		System.out.println("LONGEST COLUMN: ");
		checkSuccess(cur.getLongest(0),1);
		checkSuccess(cur.getLongest("testint"),1);
		checkSuccess(cur.getLongest(1),3);
		checkSuccess(cur.getLongest("testfloat"),3);
		checkSuccess(cur.getLongest(2),9);
		checkSuccess(cur.getLongest("testchar"),9);
		checkSuccess(cur.getLongest(3),12);
		checkSuccess(cur.getLongest("testvarchar"),12);
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
		checkSuccess(cur.getField(0,1),"1.1");
		checkSuccess(cur.getField(0,2),"testchar1");
		checkSuccess(cur.getField(0,3),"testvarchar1");
		System.out.println();
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(7,1),"8.8");
		checkSuccess(cur.getField(7,2),"testchar8");
		checkSuccess(cur.getField(7,3),"testvarchar8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY INDEX: ");
		checkSuccess(cur.getFieldLength(0,0),1);
		checkSuccess(cur.getFieldLength(0,1),3);
		checkSuccess(cur.getFieldLength(0,2),9);
		checkSuccess(cur.getFieldLength(0,3),12);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,0),1);
		checkSuccess(cur.getFieldLength(7,1),3);
		checkSuccess(cur.getFieldLength(7,2),9);
		checkSuccess(cur.getFieldLength(7,3),12);
		System.out.println();
	
		System.out.println("FIELDS BY NAME: ");
		checkSuccess(cur.getField(0,"testint"),"1");
		checkSuccess(cur.getField(0,"testfloat"),"1.1");
		checkSuccess(cur.getField(0,"testchar"),"testchar1");
		checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1");
		System.out.println();
		checkSuccess(cur.getField(7,"testint"),"8");
		checkSuccess(cur.getField(7,"testfloat"),"8.8");
		checkSuccess(cur.getField(7,"testchar"),"testchar8");
		checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY NAME: ");
		checkSuccess(cur.getFieldLength(0,"testint"),1);
		checkSuccess(cur.getFieldLength(0,"testfloat"),3);
		checkSuccess(cur.getFieldLength(0,"testchar"),9);
		checkSuccess(cur.getFieldLength(0,"testvarchar"),12);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,"testint"),1);
		checkSuccess(cur.getFieldLength(7,"testfloat"),3);
		checkSuccess(cur.getFieldLength(7,"testchar"),9);
		checkSuccess(cur.getFieldLength(7,"testvarchar"),12);
		System.out.println();
	
		System.out.println("FIELDS BY ARRAY: ");
		fields=cur.getRow(0);
		checkSuccess(fields[0],"1");
		checkSuccess(fields[1],"1.1");
		checkSuccess(fields[2],"testchar1");
		checkSuccess(fields[3],"testvarchar1");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY ARRAY: ");
		fieldlens=cur.getRowLengths(0);
		checkSuccess(fieldlens[0],1);
		checkSuccess(fieldlens[1],3);
		checkSuccess(fieldlens[2],9);
		checkSuccess(fieldlens[3],12);
		System.out.println();
	
		System.out.println("INDIVIDUAL SUBSTITUTIONS: ");
		cur.sendQuery("drop table testtable1");
		checkSuccess(cur.sendQuery("create table testtable1 (col1 int, col2 char, col3 float)"),1);
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
		cur.prepareQuery("insert into testtable1 values ('$(var1)','$(var2)','$(var3)')");
		cur.substitutions(subvars,subvalstrings);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"hi");
		checkSuccess(cur.getField(0,1),"hello");
		checkSuccess(cur.getField(0,2),"bye");
		checkSuccess(cur.sendQuery("delete from testtable1"),1);
		System.out.println();
	
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))");
		cur.substitutions(subvars,subvallongs);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"2");
		checkSuccess(cur.getField(0,2),"3.0");
		checkSuccess(cur.sendQuery("delete from testtable1"),1);
		System.out.println();
	
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))");
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
		cur.getNullsAsNulls();
		checkSuccess(cur.sendQuery("insert into testtable1 values (1,null,null)"),1);
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),null);
		checkSuccess(cur.getField(0,2),null);
		cur.getNullsAsEmptyStrings();
		checkSuccess(cur.sendQuery("select * from testtable1"),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"");
		checkSuccess(cur.getField(0,2),"");
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
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(cur.getColumnName(0),null);
		checkSuccess(cur.getColumnLength(0),0);
		checkSuccess(cur.getColumnType(0),null);
		cur.getColumnInfo();
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		checkSuccess(cur.getColumnName(0),"testint");
		checkSuccess(cur.getColumnLength(0),0);
		checkSuccess(cur.getColumnType(0),"INTEGER");
		System.out.println();
	
		System.out.println("SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		filename=cur.getCacheFileName();
		checkSuccess(filename,"cachefile1");
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet(filename),1);
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
	
		System.out.println("COLUMN COUNT FOR CACHED RESULT SET: ");
		checkSuccess(cur.colCount(),4);
		System.out.println();
	
		System.out.println("COLUMN NAMES FOR CACHED RESULT SET: ");
		checkSuccess(cur.getColumnName(0),"testint");
		checkSuccess(cur.getColumnName(1),"testfloat");
		checkSuccess(cur.getColumnName(2),"testchar");
		checkSuccess(cur.getColumnName(3),"testvarchar");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"testint");
		checkSuccess(cols[1],"testfloat");
		checkSuccess(cols[2],"testchar");
		checkSuccess(cols[3],"testvarchar");
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
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
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

	    	System.out.println("COMMIT AND ROLLBACK: \n");
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
	    	checkSuccess(cur.sendQuery("insert into testtable values (10,10.1,'testchar10','testvarchar10')"),1);
	    	checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
	    	checkSuccess(secondcur.getField(0,0),"9");
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
		cur.sendQuery("drop table testtable");
	
		// invalid queries...
		System.out.println("INVALID QUERIES: ");
		checkSuccess(cur.sendQuery("select * from testtable"),0);
		checkSuccess(cur.sendQuery("select * from testtable"),0);
		checkSuccess(cur.sendQuery("select * from testtable"),0);
		checkSuccess(cur.sendQuery("select * from testtable"),0);
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
