// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

import com.firstworks.sqlrelay.SQLRConnection;
import com.firstworks.sqlrelay.SQLRCursor;


class postgresql {
	
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
	
		System.out.println("IDENTIFY: ");
		checkSuccess(con.identify(),"postgresql");
		System.out.println();
	
		// ping
		System.out.println("PING: ");
		checkSuccess(con.ping(),1);
		System.out.println();
	
		// drop existing table
		cur.sendQuery("drop table testtable");
	
		System.out.println("CREATE TEMPTABLE: ");
		checkSuccess(cur.sendQuery("create table testtable (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
		System.out.println();
	
		System.out.println("BEGIN TRANSCTION: ");
		checkSuccess(cur.sendQuery("begin"),1);
		System.out.println();
	
		System.out.println("INSERT: ");
		checkSuccess(cur.sendQuery("insert into testtable values (1,1.1,1.1,1,'testchar1','testvarchar1','01/01/2001','01:00:00',null)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (2,2.2,2.2,2,'testchar2','testvarchar2','01/01/2002','02:00:00',null)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (3,3.3,3.3,3,'testchar3','testvarchar3','01/01/2003','03:00:00',null)"),1);
		checkSuccess(cur.sendQuery("insert into testtable values (4,4.4,4.4,4,'testchar4','testvarchar4','01/01/2004','04:00:00',null)"),1);
		System.out.println();
	
		System.out.println("AFFECTED ROWS: ");
		checkSuccess(cur.affectedRows(),1);
		System.out.println();
	
		System.out.println("BIND BY POSITION: ");
		cur.prepareQuery("insert into testtable values ($1,$2,$3,$4,$5,$6,$7,$8)");
		checkSuccess(cur.countBindVariables(),8);
		cur.inputBind("1",5);
		cur.inputBind("2",5.5,4,2);
		cur.inputBind("3",5.5,4,2);
		cur.inputBind("4",5);
		cur.inputBind("5","testchar5");
		cur.inputBind("6","testvarchar5");
		cur.inputBind("7","01/01/2005");
		cur.inputBind("8","05:00:00");
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("1",6);
		cur.inputBind("2",6.6,4,2);
		cur.inputBind("3",6.6,4,2);
		cur.inputBind("4",6);
		cur.inputBind("5","testchar6");
		cur.inputBind("6","testvarchar6");
		cur.inputBind("7","01/01/2006");
		cur.inputBind("8","06:00:00");
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("1",7);
		cur.inputBind("2",7.7,4,2);
		cur.inputBind("3",7.7,4,2);
		cur.inputBind("4",7);
		cur.inputBind("5","testchar7");
		cur.inputBind("6","testvarchar7");
		cur.inputBind("7","01/01/2007");
		cur.inputBind("8","07:00:00");
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("BIND BY POSITION WITH VALIDATION: ");
		cur.clearBinds();
		cur.inputBind("1",8);
		cur.inputBind("2",8.8,4,2);
		cur.inputBind("3",8.8,4,2);
		cur.inputBind("4",8);
		cur.inputBind("5","testchar8");
		cur.inputBind("6","testvarchar8");
		cur.inputBind("7","01/01/2008");
		cur.inputBind("8","08:00:00");
		cur.validateBinds();
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("SELECT: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
		System.out.println();
	
		System.out.println("COLUMN COUNT: ");
		checkSuccess(cur.colCount(),9);
		System.out.println();
	
		System.out.println("COLUMN NAMES: ");
		checkSuccess(cur.getColumnName(0),"testint");
		checkSuccess(cur.getColumnName(1),"testfloat");
		checkSuccess(cur.getColumnName(2),"testreal");
		checkSuccess(cur.getColumnName(3),"testsmallint");
		checkSuccess(cur.getColumnName(4),"testchar");
		checkSuccess(cur.getColumnName(5),"testvarchar");
		checkSuccess(cur.getColumnName(6),"testdate");
		checkSuccess(cur.getColumnName(7),"testtime");
		checkSuccess(cur.getColumnName(8),"testtimestamp");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"testint");
		checkSuccess(cols[1],"testfloat");
		checkSuccess(cols[2],"testreal");
		checkSuccess(cols[3],"testsmallint");
		checkSuccess(cols[4],"testchar");
		checkSuccess(cols[5],"testvarchar");
		checkSuccess(cols[6],"testdate");
		checkSuccess(cols[7],"testtime");
		checkSuccess(cols[8],"testtimestamp");
		System.out.println();
	
		System.out.println("COLUMN TYPES: ");
		checkSuccess(cur.getColumnType(0),"int4");
		checkSuccess(cur.getColumnType("testint"),"int4");
		checkSuccess(cur.getColumnType(1),"float8");
		checkSuccess(cur.getColumnType("testfloat"),"float8");
		checkSuccess(cur.getColumnType(2),"float4");
		checkSuccess(cur.getColumnType("testreal"),"float4");
		checkSuccess(cur.getColumnType(3),"int2");
		checkSuccess(cur.getColumnType("testsmallint"),"int2");
		checkSuccess(cur.getColumnType(4),"bpchar");
		checkSuccess(cur.getColumnType("testchar"),"bpchar");
		checkSuccess(cur.getColumnType(5),"varchar");
		checkSuccess(cur.getColumnType("testvarchar"),"varchar");
		checkSuccess(cur.getColumnType(6),"date");
		checkSuccess(cur.getColumnType("testdate"),"date");
		checkSuccess(cur.getColumnType(7),"time");
		checkSuccess(cur.getColumnType("testtime"),"time");
		checkSuccess(cur.getColumnType(8),"timestamp");
		checkSuccess(cur.getColumnType("testtimestamp"),"timestamp");
		System.out.println();
	
		System.out.println("COLUMN LENGTH: ");
		checkSuccess(cur.getColumnLength(0),4);
		checkSuccess(cur.getColumnLength("testint"),4);
		checkSuccess(cur.getColumnLength(1),8);
		checkSuccess(cur.getColumnLength("testfloat"),8);
		checkSuccess(cur.getColumnLength(2),4);
		checkSuccess(cur.getColumnLength("testreal"),4);
		checkSuccess(cur.getColumnLength(3),2);
		checkSuccess(cur.getColumnLength("testsmallint"),2);
		checkSuccess(cur.getColumnLength(4),44);
		checkSuccess(cur.getColumnLength("testchar"),44);
		checkSuccess(cur.getColumnLength(5),44);
		checkSuccess(cur.getColumnLength("testvarchar"),44);
		checkSuccess(cur.getColumnLength(6),4);
		checkSuccess(cur.getColumnLength("testdate"),4);
		checkSuccess(cur.getColumnLength(7),8);
		checkSuccess(cur.getColumnLength("testtime"),8);
		checkSuccess(cur.getColumnLength(8),8);
		checkSuccess(cur.getColumnLength("testtimestamp"),8);
		System.out.println();
	
		System.out.println("LONGEST COLUMN: ");
		checkSuccess(cur.getLongest(0),1);
		checkSuccess(cur.getLongest("testint"),1);
		checkSuccess(cur.getLongest(1),3);
		checkSuccess(cur.getLongest("testfloat"),3);
		checkSuccess(cur.getLongest(2),3);
		checkSuccess(cur.getLongest("testreal"),3);
		checkSuccess(cur.getLongest(3),1);
		checkSuccess(cur.getLongest("testsmallint"),1);
		checkSuccess(cur.getLongest(4),40);
		checkSuccess(cur.getLongest("testchar"),40);
		checkSuccess(cur.getLongest(5),12);
		checkSuccess(cur.getLongest("testvarchar"),12);
		checkSuccess(cur.getLongest(6),10);
		checkSuccess(cur.getLongest("testdate"),10);
		checkSuccess(cur.getLongest(7),8);
		checkSuccess(cur.getLongest("testtime"),8);
		System.out.println();
	
		System.out.println("ROW COUNT: ");
		checkSuccess(cur.rowCount(),8);
		System.out.println();
	
		/*System.out.println("TOTAL ROWS: ");
		checkSuccess(cur.totalRows(),8);
		System.out.println();*/
	
		System.out.println("FIRST ROW INDEX: ");
		checkSuccess(cur.firstRowIndex(),0);
		System.out.println();
	
		System.out.println("END OF RESULT SET: ");
		checkSuccess(cur.endOfResultSet(),1);
		System.out.println();
	
		System.out.println("FIELDS BY INDEX: ");
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"1.1");
		checkSuccess(cur.getField(0,2),"1.1");
		checkSuccess(cur.getField(0,3),"1");
		checkSuccess(cur.getField(0,4),"testchar1                               ");
		checkSuccess(cur.getField(0,5),"testvarchar1");
		checkSuccess(cur.getField(0,6),"2001-01-01");
		checkSuccess(cur.getField(0,7),"01:00:00");
		System.out.println();
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(7,1),"8.8");
		checkSuccess(cur.getField(7,2),"8.8");
		checkSuccess(cur.getField(7,3),"8");
		checkSuccess(cur.getField(7,4),"testchar8                               ");
		checkSuccess(cur.getField(7,5),"testvarchar8");
		checkSuccess(cur.getField(7,6),"2008-01-01");
		checkSuccess(cur.getField(7,7),"08:00:00");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY INDEX: ");
		checkSuccess(cur.getFieldLength(0,0),1);
		checkSuccess(cur.getFieldLength(0,1),3);
		checkSuccess(cur.getFieldLength(0,2),3);
		checkSuccess(cur.getFieldLength(0,3),1);
		checkSuccess(cur.getFieldLength(0,4),40);
		checkSuccess(cur.getFieldLength(0,5),12);
		checkSuccess(cur.getFieldLength(0,6),10);
		checkSuccess(cur.getFieldLength(0,7),8);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,0),1);
		checkSuccess(cur.getFieldLength(7,1),3);
		checkSuccess(cur.getFieldLength(7,2),3);
		checkSuccess(cur.getFieldLength(7,3),1);
		checkSuccess(cur.getFieldLength(7,4),40);
		checkSuccess(cur.getFieldLength(7,5),12);
		checkSuccess(cur.getFieldLength(7,6),10);
		checkSuccess(cur.getFieldLength(7,7),8);
		System.out.println();
	
		System.out.println("FIELDS BY NAME: ");
		checkSuccess(cur.getField(0,"testint"),"1");
		checkSuccess(cur.getField(0,"testfloat"),"1.1");
		checkSuccess(cur.getField(0,"testreal"),"1.1");
		checkSuccess(cur.getField(0,"testsmallint"),"1");
		checkSuccess(cur.getField(0,"testchar"),"testchar1                               ");
		checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1");
		checkSuccess(cur.getField(0,"testdate"),"2001-01-01");
		checkSuccess(cur.getField(0,"testtime"),"01:00:00");
		System.out.println();
		checkSuccess(cur.getField(7,"testint"),"8");
		checkSuccess(cur.getField(7,"testfloat"),"8.8");
		checkSuccess(cur.getField(7,"testreal"),"8.8");
		checkSuccess(cur.getField(7,"testsmallint"),"8");
		checkSuccess(cur.getField(7,"testchar"),"testchar8                               ");
		checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8");
		checkSuccess(cur.getField(7,"testdate"),"2008-01-01");
		checkSuccess(cur.getField(7,"testtime"),"08:00:00");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY NAME: ");
		checkSuccess(cur.getFieldLength(0,"testint"),1);
		checkSuccess(cur.getFieldLength(0,"testfloat"),3);
		checkSuccess(cur.getFieldLength(0,"testreal"),3);
		checkSuccess(cur.getFieldLength(0,"testsmallint"),1);
		checkSuccess(cur.getFieldLength(0,"testchar"),40);
		checkSuccess(cur.getFieldLength(0,"testvarchar"),12);
		checkSuccess(cur.getFieldLength(0,"testdate"),10);
		checkSuccess(cur.getFieldLength(0,"testtime"),8);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,"testint"),1);
		checkSuccess(cur.getFieldLength(7,"testfloat"),3);
		checkSuccess(cur.getFieldLength(7,"testreal"),3);
		checkSuccess(cur.getFieldLength(7,"testsmallint"),1);
		checkSuccess(cur.getFieldLength(7,"testchar"),40);
		checkSuccess(cur.getFieldLength(7,"testvarchar"),12);
		checkSuccess(cur.getFieldLength(7,"testdate"),10);
		checkSuccess(cur.getFieldLength(7,"testtime"),8);
		System.out.println();
	
		System.out.println("FIELDS BY ARRAY: ");
		fields=cur.getRow(0);
		checkSuccess(fields[0],"1");
		checkSuccess(fields[1],"1.1");
		checkSuccess(fields[2],"1.1");
		checkSuccess(fields[3],"1");
		checkSuccess(fields[4],"testchar1                               ");
		checkSuccess(fields[5],"testvarchar1");
		checkSuccess(fields[6],"2001-01-01");
		checkSuccess(fields[7],"01:00:00");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY ARRAY: ");
		fieldlens=cur.getRowLengths(0);
		checkSuccess(fieldlens[0],1);
		checkSuccess(fieldlens[1],3);
		checkSuccess(fieldlens[2],3);
		checkSuccess(fieldlens[3],1);
		checkSuccess(fieldlens[4],40);
		checkSuccess(fieldlens[5],12);
		checkSuccess(fieldlens[6],10);
		checkSuccess(fieldlens[7],8);
		System.out.println();
	
		System.out.println("INDIVIDUAL SUBSTITUTIONS: ");
		cur.prepareQuery("select $(var1),'$(var2)',$(var3)");
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
		cur.prepareQuery("select $(var1),$(var2),$(var3)");
		cur.substitutions(subvars,subvallongs);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
		
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"2");
		checkSuccess(cur.getField(0,2),"3");
		System.out.println();
		
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("select '$(var1)','$(var2)','$(var3)'");
		cur.substitutions(subvars,subvalstrings);
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("FIELDS: ");
		checkSuccess(cur.getField(0,0),"hi");
		checkSuccess(cur.getField(0,1),"hello");
		checkSuccess(cur.getField(0,2),"bye");
		System.out.println();
	
		System.out.println("ARRAY SUBSTITUTIONS: ");
		cur.prepareQuery("select $(var1),$(var2),$(var3)");
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
		checkSuccess(cur.sendQuery("select null,1,null"),1);
		checkSuccess(cur.getField(0,0),null);
		checkSuccess(cur.getField(0,1),"1");
		checkSuccess(cur.getField(0,2),null);
		cur.getNullsAsEmptyStrings();
		checkSuccess(cur.sendQuery("select null,1,null"),1);
		checkSuccess(cur.getField(0,0),"");
		checkSuccess(cur.getField(0,1),"1");
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
		checkSuccess(cur.getColumnLength(0),4);
		checkSuccess(cur.getColumnType(0),"int4");
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
		checkSuccess(cur.colCount(),9);
		System.out.println();
	
		System.out.println("COLUMN NAMES FOR CACHED RESULT SET: ");
		checkSuccess(cur.getColumnName(0),"testint");
		checkSuccess(cur.getColumnName(1),"testfloat");
		checkSuccess(cur.getColumnName(2),"testreal");
		checkSuccess(cur.getColumnName(3),"testsmallint");
		checkSuccess(cur.getColumnName(4),"testchar");
		checkSuccess(cur.getColumnName(5),"testvarchar");
		checkSuccess(cur.getColumnName(6),"testdate");
		checkSuccess(cur.getColumnName(7),"testtime");
		checkSuccess(cur.getColumnName(8),"testtimestamp");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"testint");
		checkSuccess(cols[1],"testfloat");
		checkSuccess(cols[2],"testreal");
		checkSuccess(cols[3],"testsmallint");
		checkSuccess(cols[4],"testchar");
		checkSuccess(cols[5],"testvarchar");
		checkSuccess(cols[6],"testdate");
		checkSuccess(cols[7],"testtime");
		checkSuccess(cols[8],"testtimestamp");
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
		//checkSuccess(con.autoCommitOn(),1);
		checkSuccess(cur.sendQuery("insert into testtable values (10,10.1,10.1,10,'testchar10','testvarchar10','01/01/2010','10:00:00',null)"),1);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"9");
		//checkSuccess(con.autoCommitOff(),1);
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

		System.out.println("STORED PROCEDURES: ");
		// return no values
		cur.sendQuery("drop function testfunc(int,float,char(20))");
		checkSuccess(cur.sendQuery("create function testfunc(int,float,char(20)) returns void as ' declare in1 int; in2 float; in3 char(20); begin in1:=$1; in2:=$2; in3:=$3; return; end;' language plpgsql"),1);
		cur.prepareQuery("select testfunc($1,$2,$3)");
		cur.inputBind("1",1);
		cur.inputBind("2",1.1,4,2);
		cur.inputBind("3","hello");
		checkSuccess(cur.executeQuery(),1);
		cur.sendQuery("drop function testfunc(int,float,char(20))");
		System.out.println();
		// return single value
		cur.sendQuery("drop function testfunc(int,float,char(20))");
		checkSuccess(cur.sendQuery("create function testfunc(int,float,char(20)) returns int as ' begin return $1; end;' language plpgsql"),1);
		cur.prepareQuery("select * from testfunc($1,$2,$3)");
		cur.inputBind("1",1);
		cur.inputBind("2",1.1,4,2);
		cur.inputBind("3","hello");
		checkSuccess(cur.executeQuery(),1);
		checkSuccess(cur.getField(0,0),"1");
		cur.sendQuery("drop function testfunc(int,float,char(20))");
		System.out.println();
		// return multiple values
		cur.sendQuery("drop function testfunc(int,char(20))");
		checkSuccess(cur.sendQuery("create function testfunc(int,float,char(20)) returns record as ' declare output record; begin select $1,$2,$3 into output; return output; end;' language plpgsql"),1);
		cur.prepareQuery("select * from testfunc($1,$2,$3) as (col1 int, col2 float, col3 bpchar)");
		cur.inputBind("1",1);
		cur.inputBind("2",1.1,4,2);
		cur.inputBind("3","hello");
		checkSuccess(cur.executeQuery(),1);
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"1.1");
		checkSuccess(cur.getField(0,2),"hello");
		cur.sendQuery("drop function testfunc(int,float,char(20))");
		System.out.println();
		// return result set
		cur.sendQuery("drop function testfunc()");
		checkSuccess(cur.sendQuery("create function testfunc() returns setof record as ' declare output record; begin for output in select * from testtable loop return next output; end loop; return; end;' language plpgsql"),1);
		checkSuccess(cur.sendQuery("select * from testfunc() as (testint int, testfloat float, testreal real, testsmallint smallint, testchar char(40), testvarchar varchar(40), testdate date, testtime time, testtimestamp timestamp)"),1);
		checkSuccess(cur.getField(4,0),"5");
		checkSuccess(cur.getField(5,0),"6");
		checkSuccess(cur.getField(6,0),"7");
		checkSuccess(cur.getField(7,0),"8");
		cur.sendQuery("drop function testfunc()");
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
