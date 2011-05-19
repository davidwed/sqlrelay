// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

import com.firstworks.sqlrelay.SQLRConnection;
import com.firstworks.sqlrelay.SQLRCursor;


class mysql {
	
	
	
	
	
	
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
	
	
		// instantiation
		SQLRConnection con=new SQLRConnection("localhost",
						(short)9000,
						"/tmp/test.socket",
						"test","test",0,1);
		SQLRCursor cur=new SQLRCursor(con);
	
		// get database type
		System.out.println("IDENTIFY: ");
		checkSuccess(con.identify(),"mysql");
	
		// ping
		System.out.println("PING: ");
		checkSuccess(con.ping(),1);
		System.out.println();
	
		// drop existing table
		cur.sendQuery("drop table testtable");
	
		// create a new table
		System.out.println("CREATE TEMPTABLE: ");
		checkSuccess(cur.sendQuery("create table testdb.testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(2,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testtext text, testvarchar varchar(40), testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testtimestamp timestamp)"),1);
		System.out.println();
	
		System.out.println("INSERT: ");
		checkSuccess(cur.sendQuery("insert into testdb.testtable values (1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','text1','varchar1','tinytext1','mediumtext1','longtext1',null)"),1);
		checkSuccess(cur.sendQuery("insert into testdb.testtable values (2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','text2','varchar2','tinytext2','mediumtext2','longtext2',null)"),1);
		checkSuccess(cur.sendQuery("insert into testdb.testtable values (3,3,3,3,3,3.1,3.1,3.1,'2003-01-01','03:00:00','2003-01-01 03:00:00','2003','char3','text3','varchar3','tinytext3','mediumtext3','longtext3',null)"),1);
		checkSuccess(cur.sendQuery("insert into testdb.testtable values (4,4,4,4,4,4.1,4.1,4.1,'2004-01-01','04:00:00','2004-01-01 04:00:00','2004','char4','text4','varchar4','tinytext4','mediumtext4','longtext4',null)"),1);
		System.out.println();
	
		System.out.println("AFFECTED ROWS: ");
		checkSuccess(cur.affectedRows(),1);
		System.out.println();
	
		System.out.println("BIND BY POSITION: ");
		cur.prepareQuery("insert into testdb.testtable values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,null)");
		checkSuccess(cur.countBindVariables(),18);
		cur.inputBind("1",5);
		cur.inputBind("2",5);
		cur.inputBind("3",5);
		cur.inputBind("4",5);
		cur.inputBind("5",5);
		cur.inputBind("6",5.1,2,1);
		cur.inputBind("7",5.1,2,1);
		cur.inputBind("8",5.1,2,1);
		cur.inputBind("9","2005-01-01");
		cur.inputBind("10","05:00:00");
		cur.inputBind("11","2005-01-01 05:00:00");
		cur.inputBind("12","2005");
		cur.inputBind("13","char5");
		cur.inputBind("14","text5");
		cur.inputBind("15","varchar5");
		cur.inputBind("16","tinytext5");
		cur.inputBind("17","mediumtext5");
		cur.inputBind("18","longtext5");
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("1",6);
		cur.inputBind("2",6);
		cur.inputBind("3",6);
		cur.inputBind("4",6);
		cur.inputBind("5",6);
		cur.inputBind("6",6.1,2,1);
		cur.inputBind("7",6.1,2,1);
		cur.inputBind("8",6.1,2,1);
		cur.inputBind("9","2006-01-01");
		cur.inputBind("10","06:00:00");
		cur.inputBind("11","2006-01-01 06:00:00");
		cur.inputBind("12","2006");
		cur.inputBind("13","char6");
		cur.inputBind("14","text6");
		cur.inputBind("15","varchar6");
		cur.inputBind("16","tinytext6");
		cur.inputBind("17","mediumtext6");
		cur.inputBind("18","longtext6");
		checkSuccess(cur.executeQuery(),1);
		cur.clearBinds();
		cur.inputBind("1",7);
		cur.inputBind("2",7);
		cur.inputBind("3",7);
		cur.inputBind("4",7);
		cur.inputBind("5",7);
		cur.inputBind("6",7.1,2,1);
		cur.inputBind("7",7.1,2,1);
		cur.inputBind("8",7.1,2,1);
		cur.inputBind("9","2007-01-01");
		cur.inputBind("10","07:00:00");
		cur.inputBind("11","2007-01-01 07:00:00");
		cur.inputBind("12","2007");
		cur.inputBind("13","char7");
		cur.inputBind("14","text7");
		cur.inputBind("15","varchar7");
		cur.inputBind("16","tinytext7");
		cur.inputBind("17","mediumtext7");
		cur.inputBind("18","longtext7");
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("BIND BY POSITION WITH VALIDATION: ");
		cur.clearBinds();
		cur.inputBind("1",8);
		cur.inputBind("2",8);
		cur.inputBind("3",8);
		cur.inputBind("4",8);
		cur.inputBind("5",8);
		cur.inputBind("6",8.1,2,1);
		cur.inputBind("7",8.1,2,1);
		cur.inputBind("8",8.1,2,1);
		cur.inputBind("9","2008-01-01");
		cur.inputBind("10","08:00:00");
		cur.inputBind("11","2008-01-01 08:00:00");
		cur.inputBind("12","2008");
		cur.inputBind("13","char8");
		cur.inputBind("14","text8");
		cur.inputBind("15","varchar8");
		cur.inputBind("16","tinytext8");
		cur.inputBind("17","mediumtext8");
		cur.inputBind("18","longtext8");
		cur.validateBinds();
		checkSuccess(cur.executeQuery(),1);
		System.out.println();
	
		System.out.println("SELECT: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
		System.out.println();
	
		System.out.println("COLUMN COUNT: ");
		checkSuccess(cur.colCount(),19);
		System.out.println();
	
		System.out.println("COLUMN NAMES: ");
		checkSuccess(cur.getColumnName(0),"testtinyint");
		checkSuccess(cur.getColumnName(1),"testsmallint");
		checkSuccess(cur.getColumnName(2),"testmediumint");
		checkSuccess(cur.getColumnName(3),"testint");
		checkSuccess(cur.getColumnName(4),"testbigint");
		checkSuccess(cur.getColumnName(5),"testfloat");
		checkSuccess(cur.getColumnName(6),"testreal");
		checkSuccess(cur.getColumnName(7),"testdecimal");
		checkSuccess(cur.getColumnName(8),"testdate");
		checkSuccess(cur.getColumnName(9),"testtime");
		checkSuccess(cur.getColumnName(10),"testdatetime");
		checkSuccess(cur.getColumnName(11),"testyear");
		checkSuccess(cur.getColumnName(12),"testchar");
		checkSuccess(cur.getColumnName(13),"testtext");
		checkSuccess(cur.getColumnName(14),"testvarchar");
		checkSuccess(cur.getColumnName(15),"testtinytext");
		checkSuccess(cur.getColumnName(16),"testmediumtext");
		checkSuccess(cur.getColumnName(17),"testlongtext");
		checkSuccess(cur.getColumnName(18),"testtimestamp");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"testtinyint");
		checkSuccess(cols[1],"testsmallint");
		checkSuccess(cols[2],"testmediumint");
		checkSuccess(cols[3],"testint");
		checkSuccess(cols[4],"testbigint");
		checkSuccess(cols[5],"testfloat");
		checkSuccess(cols[6],"testreal");
		checkSuccess(cols[7],"testdecimal");
		checkSuccess(cols[8],"testdate");
		checkSuccess(cols[9],"testtime");
		checkSuccess(cols[10],"testdatetime");
		checkSuccess(cols[11],"testyear");
		checkSuccess(cols[12],"testchar");
		checkSuccess(cols[13],"testtext");
		checkSuccess(cols[14],"testvarchar");
		checkSuccess(cols[15],"testtinytext");
		checkSuccess(cols[16],"testmediumtext");
		checkSuccess(cols[17],"testlongtext");
		checkSuccess(cols[18],"testtimestamp");
		System.out.println();
	
		System.out.println("COLUMN TYPES: ");
		checkSuccess(cur.getColumnType(0),"TINYINT");
		checkSuccess(cur.getColumnType(1),"SMALLINT");
		checkSuccess(cur.getColumnType(2),"MEDIUMINT");
		checkSuccess(cur.getColumnType(3),"INT");
		checkSuccess(cur.getColumnType(4),"BIGINT");
		checkSuccess(cur.getColumnType(5),"FLOAT");
		checkSuccess(cur.getColumnType(6),"REAL");
		checkSuccess(cur.getColumnType(7),"DECIMAL");
		checkSuccess(cur.getColumnType(8),"DATE");
		checkSuccess(cur.getColumnType(9),"TIME");
		checkSuccess(cur.getColumnType(10),"DATETIME");
		checkSuccess(cur.getColumnType(11),"YEAR");
		checkSuccess(cur.getColumnType(12),"STRING");
		checkSuccess(cur.getColumnType(13),"BLOB");
		checkSuccess(cur.getColumnType(14),"CHAR");
		checkSuccess(cur.getColumnType(15),"TINYBLOB");
		checkSuccess(cur.getColumnType(16),"MEDIUMBLOB");
		checkSuccess(cur.getColumnType(17),"LONGBLOB");
		checkSuccess(cur.getColumnType(18),"TIMESTAMP");
		checkSuccess(cur.getColumnType("testtinyint"),"TINYINT");
		checkSuccess(cur.getColumnType("testsmallint"),"SMALLINT");
		checkSuccess(cur.getColumnType("testmediumint"),"MEDIUMINT");
		checkSuccess(cur.getColumnType("testint"),"INT");
		checkSuccess(cur.getColumnType("testbigint"),"BIGINT");
		checkSuccess(cur.getColumnType("testfloat"),"FLOAT");
		checkSuccess(cur.getColumnType("testreal"),"REAL");
		checkSuccess(cur.getColumnType("testdecimal"),"DECIMAL");
		checkSuccess(cur.getColumnType("testdate"),"DATE");
		checkSuccess(cur.getColumnType("testtime"),"TIME");
		checkSuccess(cur.getColumnType("testdatetime"),"DATETIME");
		checkSuccess(cur.getColumnType("testyear"),"YEAR");
		checkSuccess(cur.getColumnType("testchar"),"STRING");
		checkSuccess(cur.getColumnType("testtext"),"BLOB");
		checkSuccess(cur.getColumnType("testvarchar"),"CHAR");
		checkSuccess(cur.getColumnType("testtinytext"),"TINYBLOB");
		checkSuccess(cur.getColumnType("testmediumtext"),"MEDIUMBLOB");
		checkSuccess(cur.getColumnType("testlongtext"),"LONGBLOB");
		checkSuccess(cur.getColumnType("testtimestamp"),"TIMESTAMP");
		System.out.println();
	
		System.out.println("COLUMN LENGTH: ");
		checkSuccess(cur.getColumnLength(0),1);
		checkSuccess(cur.getColumnLength(1),2);
		checkSuccess(cur.getColumnLength(2),3);
		checkSuccess(cur.getColumnLength(3),4);
		checkSuccess(cur.getColumnLength(4),8);
		checkSuccess(cur.getColumnLength(5),4);
		checkSuccess(cur.getColumnLength(6),8);
		checkSuccess(cur.getColumnLength(7),6);
		checkSuccess(cur.getColumnLength(8),3);
		checkSuccess(cur.getColumnLength(9),3);
		checkSuccess(cur.getColumnLength(10),8);
		checkSuccess(cur.getColumnLength(11),1);
		checkSuccess(cur.getColumnLength(12),40);
		checkSuccess(cur.getColumnLength(13),65535);
		checkSuccess(cur.getColumnLength(14),41);
		checkSuccess(cur.getColumnLength(15),255);
		checkSuccess(cur.getColumnLength(16),16777215);
		checkSuccess(cur.getColumnLength(17),2147483647);
		checkSuccess(cur.getColumnLength(18),4);
		checkSuccess(cur.getColumnLength("testtinyint"),1);
		checkSuccess(cur.getColumnLength("testsmallint"),2);
		checkSuccess(cur.getColumnLength("testmediumint"),3);
		checkSuccess(cur.getColumnLength("testint"),4);
		checkSuccess(cur.getColumnLength("testbigint"),8);
		checkSuccess(cur.getColumnLength("testfloat"),4);
		checkSuccess(cur.getColumnLength("testreal"),8);
		checkSuccess(cur.getColumnLength("testdecimal"),6);
		checkSuccess(cur.getColumnLength("testdate"),3);
		checkSuccess(cur.getColumnLength("testtime"),3);
		checkSuccess(cur.getColumnLength("testdatetime"),8);
		checkSuccess(cur.getColumnLength("testyear"),1);
		checkSuccess(cur.getColumnLength("testchar"),40);
		checkSuccess(cur.getColumnLength("testtext"),65535);
		checkSuccess(cur.getColumnLength("testvarchar"),41);
		checkSuccess(cur.getColumnLength("testtinytext"),255);
		checkSuccess(cur.getColumnLength("testmediumtext"),16777215);
		checkSuccess(cur.getColumnLength("testlongtext"),2147483647);
		checkSuccess(cur.getColumnLength("testtimestamp"),4);
		System.out.println();
	
		System.out.println("LONGEST COLUMN: ");
		checkSuccess(cur.getLongest(0),1);
		checkSuccess(cur.getLongest(1),1);
		checkSuccess(cur.getLongest(2),1);
		checkSuccess(cur.getLongest(3),1);
		checkSuccess(cur.getLongest(4),1);
		checkSuccess(cur.getLongest(5),3);
		checkSuccess(cur.getLongest(6),3);
		checkSuccess(cur.getLongest(7),3);
		checkSuccess(cur.getLongest(8),10);
		checkSuccess(cur.getLongest(9),8);
		checkSuccess(cur.getLongest(10),19);
		checkSuccess(cur.getLongest(11),4);
		checkSuccess(cur.getLongest(12),5);
		checkSuccess(cur.getLongest(13),5);
		checkSuccess(cur.getLongest(14),8);
		checkSuccess(cur.getLongest(15),9);
		checkSuccess(cur.getLongest(16),11);
		checkSuccess(cur.getLongest(17),9);
		checkSuccess(cur.getLongest(18),19);
		checkSuccess(cur.getLongest("testtinyint"),1);
		checkSuccess(cur.getLongest("testsmallint"),1);
		checkSuccess(cur.getLongest("testmediumint"),1);
		checkSuccess(cur.getLongest("testint"),1);
		checkSuccess(cur.getLongest("testbigint"),1);
		checkSuccess(cur.getLongest("testfloat"),3);
		checkSuccess(cur.getLongest("testreal"),3);
		checkSuccess(cur.getLongest("testdecimal"),3);
		checkSuccess(cur.getLongest("testdate"),10);
		checkSuccess(cur.getLongest("testtime"),8);
		checkSuccess(cur.getLongest("testdatetime"),19);
		checkSuccess(cur.getLongest("testyear"),4);
		checkSuccess(cur.getLongest("testchar"),5);
		checkSuccess(cur.getLongest("testtext"),5);
		checkSuccess(cur.getLongest("testvarchar"),8);
		checkSuccess(cur.getLongest("testtinytext"),9);
		checkSuccess(cur.getLongest("testmediumtext"),11);
		checkSuccess(cur.getLongest("testlongtext"),9);
		checkSuccess(cur.getLongest("testtimestamp"),19);
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
		checkSuccess(cur.getField(0,0),"1");
		checkSuccess(cur.getField(0,1),"1");
		checkSuccess(cur.getField(0,2),"1");
		checkSuccess(cur.getField(0,3),"1");
		checkSuccess(cur.getField(0,4),"1");
		checkSuccess(cur.getField(0,5),"1.1");
		checkSuccess(cur.getField(0,6),"1.1");
		checkSuccess(cur.getField(0,7),"1.1");
		checkSuccess(cur.getField(0,8),"2001-01-01");
		checkSuccess(cur.getField(0,9),"01:00:00");
		checkSuccess(cur.getField(0,10),"2001-01-01 01:00:00");
		checkSuccess(cur.getField(0,11),"2001");
		checkSuccess(cur.getField(0,12),"char1");
		checkSuccess(cur.getField(0,13),"text1");
		checkSuccess(cur.getField(0,14),"varchar1");
		checkSuccess(cur.getField(0,15),"tinytext1");
		checkSuccess(cur.getField(0,16),"mediumtext1");
		checkSuccess(cur.getField(0,17),"longtext1");
		System.out.println();
		checkSuccess(cur.getField(7,0),"8");
		checkSuccess(cur.getField(7,1),"8");
		checkSuccess(cur.getField(7,2),"8");
		checkSuccess(cur.getField(7,3),"8");
		checkSuccess(cur.getField(7,4),"8");
		checkSuccess(cur.getField(7,5),"8.1");
		checkSuccess(cur.getField(7,6),"8.1");
		checkSuccess(cur.getField(7,7),"8.1");
		checkSuccess(cur.getField(7,8),"2008-01-01");
		checkSuccess(cur.getField(7,9),"08:00:00");
		checkSuccess(cur.getField(7,10),"2008-01-01 08:00:00");
		checkSuccess(cur.getField(7,11),"2008");
		checkSuccess(cur.getField(7,12),"char8");
		checkSuccess(cur.getField(7,13),"text8");
		checkSuccess(cur.getField(7,14),"varchar8");
		checkSuccess(cur.getField(7,15),"tinytext8");
		checkSuccess(cur.getField(7,16),"mediumtext8");
		checkSuccess(cur.getField(7,17),"longtext8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY INDEX: ");
		checkSuccess(cur.getFieldLength(0,0),1);
		checkSuccess(cur.getFieldLength(0,1),1);
		checkSuccess(cur.getFieldLength(0,2),1);
		checkSuccess(cur.getFieldLength(0,3),1);
		checkSuccess(cur.getFieldLength(0,4),1);
		checkSuccess(cur.getFieldLength(0,5),3);
		checkSuccess(cur.getFieldLength(0,6),3);
		checkSuccess(cur.getFieldLength(0,7),3);
		checkSuccess(cur.getFieldLength(0,8),10);
		checkSuccess(cur.getFieldLength(0,9),8);
		checkSuccess(cur.getFieldLength(0,10),19);
		checkSuccess(cur.getFieldLength(0,11),4);
		checkSuccess(cur.getFieldLength(0,12),5);
		checkSuccess(cur.getFieldLength(0,13),5);
		checkSuccess(cur.getFieldLength(0,14),8);
		checkSuccess(cur.getFieldLength(0,15),9);
		checkSuccess(cur.getFieldLength(0,16),11);
		checkSuccess(cur.getFieldLength(0,17),9);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,0),1);
		checkSuccess(cur.getFieldLength(7,1),1);
		checkSuccess(cur.getFieldLength(7,2),1);
		checkSuccess(cur.getFieldLength(7,3),1);
		checkSuccess(cur.getFieldLength(7,4),1);
		checkSuccess(cur.getFieldLength(7,5),3);
		checkSuccess(cur.getFieldLength(7,6),3);
		checkSuccess(cur.getFieldLength(7,7),3);
		checkSuccess(cur.getFieldLength(7,8),10);
		checkSuccess(cur.getFieldLength(7,9),8);
		checkSuccess(cur.getFieldLength(7,10),19);
		checkSuccess(cur.getFieldLength(7,11),4);
		checkSuccess(cur.getFieldLength(7,12),5);
		checkSuccess(cur.getFieldLength(7,13),5);
		checkSuccess(cur.getFieldLength(7,14),8);
		checkSuccess(cur.getFieldLength(7,15),9);
		checkSuccess(cur.getFieldLength(7,16),11);
		checkSuccess(cur.getFieldLength(7,17),9);
		System.out.println();
	
		System.out.println("FIELDS BY NAME: ");
		checkSuccess(cur.getField(0,"testtinyint"),"1");
		checkSuccess(cur.getField(0,"testsmallint"),"1");
		checkSuccess(cur.getField(0,"testmediumint"),"1");
		checkSuccess(cur.getField(0,"testint"),"1");
		checkSuccess(cur.getField(0,"testbigint"),"1");
		checkSuccess(cur.getField(0,"testfloat"),"1.1");
		checkSuccess(cur.getField(0,"testreal"),"1.1");
		checkSuccess(cur.getField(0,"testdecimal"),"1.1");
		checkSuccess(cur.getField(0,"testdate"),"2001-01-01");
		checkSuccess(cur.getField(0,"testtime"),"01:00:00");
		checkSuccess(cur.getField(0,"testdatetime"),"2001-01-01 01:00:00");
		checkSuccess(cur.getField(0,"testyear"),"2001");
		checkSuccess(cur.getField(0,"testchar"),"char1");
		checkSuccess(cur.getField(0,"testtext"),"text1");
		checkSuccess(cur.getField(0,"testvarchar"),"varchar1");
		checkSuccess(cur.getField(0,"testtinytext"),"tinytext1");
		checkSuccess(cur.getField(0,"testmediumtext"),"mediumtext1");
		checkSuccess(cur.getField(0,"testlongtext"),"longtext1");
		System.out.println();
		checkSuccess(cur.getField(7,"testtinyint"),"8");
		checkSuccess(cur.getField(7,"testsmallint"),"8");
		checkSuccess(cur.getField(7,"testmediumint"),"8");
		checkSuccess(cur.getField(7,"testint"),"8");
		checkSuccess(cur.getField(7,"testbigint"),"8");
		checkSuccess(cur.getField(7,"testfloat"),"8.1");
		checkSuccess(cur.getField(7,"testreal"),"8.1");
		checkSuccess(cur.getField(7,"testdecimal"),"8.1");
		checkSuccess(cur.getField(7,"testdate"),"2008-01-01");
		checkSuccess(cur.getField(7,"testtime"),"08:00:00");
		checkSuccess(cur.getField(7,"testdatetime"),"2008-01-01 08:00:00");
		checkSuccess(cur.getField(7,"testyear"),"2008");
		checkSuccess(cur.getField(7,"testchar"),"char8");
		checkSuccess(cur.getField(7,"testtext"),"text8");
		checkSuccess(cur.getField(7,"testvarchar"),"varchar8");
		checkSuccess(cur.getField(7,"testtinytext"),"tinytext8");
		checkSuccess(cur.getField(7,"testmediumtext"),"mediumtext8");
		checkSuccess(cur.getField(7,"testlongtext"),"longtext8");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY NAME: ");
		checkSuccess(cur.getFieldLength(0,"testtinyint"),1);
		checkSuccess(cur.getFieldLength(0,"testsmallint"),1);
		checkSuccess(cur.getFieldLength(0,"testmediumint"),1);
		checkSuccess(cur.getFieldLength(0,"testint"),1);
		checkSuccess(cur.getFieldLength(0,"testbigint"),1);
		checkSuccess(cur.getFieldLength(0,"testfloat"),3);
		checkSuccess(cur.getFieldLength(0,"testreal"),3);
		checkSuccess(cur.getFieldLength(0,"testdecimal"),3);
		checkSuccess(cur.getFieldLength(0,"testdate"),10);
		checkSuccess(cur.getFieldLength(0,"testtime"),8);
		checkSuccess(cur.getFieldLength(0,"testdatetime"),19);
		checkSuccess(cur.getFieldLength(0,"testyear"),4);
		checkSuccess(cur.getFieldLength(0,"testchar"),5);
		checkSuccess(cur.getFieldLength(0,"testtext"),5);
		checkSuccess(cur.getFieldLength(0,"testvarchar"),8);
		checkSuccess(cur.getFieldLength(0,"testtinytext"),9);
		checkSuccess(cur.getFieldLength(0,"testmediumtext"),11);
		checkSuccess(cur.getFieldLength(0,"testlongtext"),9);
		System.out.println();
		checkSuccess(cur.getFieldLength(7,"testtinyint"),1);
		checkSuccess(cur.getFieldLength(7,"testsmallint"),1);
		checkSuccess(cur.getFieldLength(7,"testmediumint"),1);
		checkSuccess(cur.getFieldLength(7,"testint"),1);
		checkSuccess(cur.getFieldLength(7,"testbigint"),1);
		checkSuccess(cur.getFieldLength(7,"testfloat"),3);
		checkSuccess(cur.getFieldLength(7,"testreal"),3);
		checkSuccess(cur.getFieldLength(7,"testdecimal"),3);
		checkSuccess(cur.getFieldLength(7,"testdate"),10);
		checkSuccess(cur.getFieldLength(7,"testtime"),8);
		checkSuccess(cur.getFieldLength(7,"testdatetime"),19);
		checkSuccess(cur.getFieldLength(7,"testyear"),4);
		checkSuccess(cur.getFieldLength(7,"testchar"),5);
		checkSuccess(cur.getFieldLength(7,"testtext"),5);
		checkSuccess(cur.getFieldLength(7,"testvarchar"),8);
		checkSuccess(cur.getFieldLength(7,"testtinytext"),9);
		checkSuccess(cur.getFieldLength(7,"testmediumtext"),11);
		checkSuccess(cur.getFieldLength(7,"testlongtext"),9);
		System.out.println();
	
		System.out.println("FIELDS BY ARRAY: ");
		fields=cur.getRow(0);
		checkSuccess(fields[0],"1");
		checkSuccess(fields[1],"1");
		checkSuccess(fields[2],"1");
		checkSuccess(fields[3],"1");
		checkSuccess(fields[4],"1");
		checkSuccess(fields[5],"1.1");
		checkSuccess(fields[6],"1.1");
		checkSuccess(fields[7],"1.1");
		checkSuccess(fields[8],"2001-01-01");
		checkSuccess(fields[9],"01:00:00");
		checkSuccess(fields[10],"2001-01-01 01:00:00");
		checkSuccess(fields[11],"2001");
		checkSuccess(fields[12],"char1");
		checkSuccess(fields[13],"text1");
		checkSuccess(fields[14],"varchar1");
		checkSuccess(fields[15],"tinytext1");
		checkSuccess(fields[16],"mediumtext1");
		checkSuccess(fields[17],"longtext1");
		System.out.println();
	
		System.out.println("FIELD LENGTHS BY ARRAY: ");
		fieldlens=cur.getRowLengths(0);
		checkSuccess(fieldlens[0],1);
		checkSuccess(fieldlens[1],1);
		checkSuccess(fieldlens[2],1);
		checkSuccess(fieldlens[3],1);
		checkSuccess(fieldlens[4],1);
		checkSuccess(fieldlens[5],3);
		checkSuccess(fieldlens[6],3);
		checkSuccess(fieldlens[7],3);
		checkSuccess(fieldlens[8],10);
		checkSuccess(fieldlens[9],8);
		checkSuccess(fieldlens[10],19);
		checkSuccess(fieldlens[11],4);
		checkSuccess(fieldlens[12],5);
		checkSuccess(fieldlens[13],5);
		checkSuccess(fieldlens[14],8);
		checkSuccess(fieldlens[15],9);
		checkSuccess(fieldlens[16],11);
		checkSuccess(fieldlens[17],9);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
		checkSuccess(cur.getColumnName(0),null);
		checkSuccess(cur.getColumnLength(0),0);
		checkSuccess(cur.getColumnType(0),null);
		System.out.println();
		cur.getColumnInfo();
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
		checkSuccess(cur.getColumnName(0),"testtinyint");
		checkSuccess(cur.getColumnLength(0),1);
		checkSuccess(cur.getColumnType(0),"TINYINT");
		System.out.println();
	
		System.out.println("SUSPENDED SESSION: ");
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
		filename=cur.getCacheFileName();
		checkSuccess(filename,"cachefile1");
		cur.cacheOff();
		checkSuccess(cur.openCachedResultSet(filename),1);
		checkSuccess(cur.getField(7,0),"8");
		System.out.println();
	
		System.out.println("COLUMN COUNT FOR CACHED RESULT SET: ");
		checkSuccess(cur.colCount(),19);
		System.out.println();
	
		System.out.println("COLUMN NAMES FOR CACHED RESULT SET: ");
		checkSuccess(cur.getColumnName(0),"testtinyint");
		checkSuccess(cur.getColumnName(1),"testsmallint");
		checkSuccess(cur.getColumnName(2),"testmediumint");
		checkSuccess(cur.getColumnName(3),"testint");
		checkSuccess(cur.getColumnName(4),"testbigint");
		checkSuccess(cur.getColumnName(5),"testfloat");
		checkSuccess(cur.getColumnName(6),"testreal");
		checkSuccess(cur.getColumnName(7),"testdecimal");
		checkSuccess(cur.getColumnName(8),"testdate");
		checkSuccess(cur.getColumnName(9),"testtime");
		checkSuccess(cur.getColumnName(10),"testdatetime");
		checkSuccess(cur.getColumnName(11),"testyear");
		checkSuccess(cur.getColumnName(12),"testchar");
		checkSuccess(cur.getColumnName(13),"testtext");
		checkSuccess(cur.getColumnName(14),"testvarchar");
		checkSuccess(cur.getColumnName(15),"testtinytext");
		checkSuccess(cur.getColumnName(16),"testmediumtext");
		checkSuccess(cur.getColumnName(17),"testlongtext");
		cols=cur.getColumnNames();
		checkSuccess(cols[0],"testtinyint");
		checkSuccess(cols[1],"testsmallint");
		checkSuccess(cols[2],"testmediumint");
		checkSuccess(cols[3],"testint");
		checkSuccess(cols[4],"testbigint");
		checkSuccess(cols[5],"testfloat");
		checkSuccess(cols[6],"testreal");
		checkSuccess(cols[7],"testdecimal");
		checkSuccess(cols[8],"testdate");
		checkSuccess(cols[9],"testtime");
		checkSuccess(cols[10],"testdatetime");
		checkSuccess(cols[11],"testyear");
		checkSuccess(cols[12],"testchar");
		checkSuccess(cols[13],"testtext");
		checkSuccess(cols[14],"testvarchar");
		checkSuccess(cols[15],"testtinytext");
		checkSuccess(cols[16],"testmediumtext");
		checkSuccess(cols[17],"testlongtext");
		System.out.println();
	
		System.out.println("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: ");
		cur.setResultSetBufferSize(2);
		cur.cacheToFile("cachefile1");
		cur.setCacheTtl(200);
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),1);
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
		SQLRConnection secondcon=new SQLRConnection("localhost",
						(short)9000,
						"/tmp/test.socket",
						"test","test",0,1);
		SQLRCursor secondcur=new SQLRCursor(secondcon);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"8");
		checkSuccess(con.commit(),1);
		checkSuccess(secondcon.commit(),1);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"8");
		checkSuccess(con.autoCommitOn(),1);
		checkSuccess(cur.sendQuery("insert into testdb.testtable values (10,10,10,10,10,10.1,10.1,10.1,'2010-01-01','10:00:00','2010-01-01 10:00:00','2010','char10','text10','varchar10','tinytext10','mediumtext10','longtext10',null)"),1);
		checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
		checkSuccess(secondcur.getField(0,0),"9");
		checkSuccess(con.autoCommitOff(),1);
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
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),0);
		checkSuccess(cur.sendQuery("select * from testtable order by testtinyint"),0);
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
