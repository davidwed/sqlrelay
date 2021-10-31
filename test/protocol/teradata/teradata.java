// Copyright (c) 2017  David Muse
// See the file COPYING for more information.

import java.sql.*;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

class teradata {

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

	public static void main(String args[]) throws Exception {

		String	host="localhost";
		if (args.length>0) {
			host=args[0];
		}

		Class.forName("com.teradata.jdbc.TeraDriver");


		// connect
		Connection	con=DriverManager.getConnection(
					"jdbc:teradata://"+host+"/",
					"testuser","testpassword");
		Statement	stmt=con.createStatement();


if (false) {
		// help session
		System.out.println("HELP SESSION:");
		ResultSet		rs=stmt.executeQuery("help session");
		ResultSetMetaData	rsmd=rs.getMetaData();
		checkSuccess(rs.next(),1);
		checkSuccess(rsmd.getColumnCount(),129);
		System.out.println();
		checkSuccess(rsmd.getColumnName(1),"User Name");
		checkSuccess(rsmd.getColumnName(2),"Account Name");
		checkSuccess(rsmd.getColumnName(3),"Logon Date");
		checkSuccess(rsmd.getColumnName(4),"Logon Time");
		checkSuccess(rsmd.getColumnName(5),"Current DataBase");
		checkSuccess(rsmd.getColumnName(6),"Collation");
		checkSuccess(rsmd.getColumnName(7),"Character Set");
		checkSuccess(rsmd.getColumnName(8),"Transaction Semantics");
		checkSuccess(rsmd.getColumnName(9),"Current DateForm");
		checkSuccess(rsmd.getColumnName(10),"Session Time Zone");
		checkSuccess(rsmd.getColumnName(11),"Default Character Type");
		checkSuccess(rsmd.getColumnName(12),"Export Latin");
		checkSuccess(rsmd.getColumnName(13),"Export Unicode");
		checkSuccess(rsmd.getColumnName(14),"Export Unicode Adjust");
		checkSuccess(rsmd.getColumnName(15),"Export KanjiSJIS");
		checkSuccess(rsmd.getColumnName(16),"Export Graphic");
		checkSuccess(rsmd.getColumnName(17),"Default Date Format");
		checkSuccess(rsmd.getColumnName(18),"Radix Separator");
		checkSuccess(rsmd.getColumnName(19),"Group Separator");
		checkSuccess(rsmd.getColumnName(20),"Grouping Rule");
		checkSuccess(rsmd.getColumnName(21),"Currency Radix Separator");
		checkSuccess(rsmd.getColumnName(22),"Currency Group Separator");
		checkSuccess(rsmd.getColumnName(23),"Currency Grouping Rule");
		checkSuccess(rsmd.getColumnName(24),"Currency Name");
		checkSuccess(rsmd.getColumnName(25),"Currency");
		checkSuccess(rsmd.getColumnName(26),"ISOCurrency");
		checkSuccess(rsmd.getColumnName(27),"Dual Currency Name");
		checkSuccess(rsmd.getColumnName(28),"Dual Currency");
		checkSuccess(rsmd.getColumnName(29),"Dual ISOCurrency");
		checkSuccess(rsmd.getColumnName(30),"Default ByteInt format");
		checkSuccess(rsmd.getColumnName(31),"Default Integer format");
		checkSuccess(rsmd.getColumnName(32),"Default SmallInt format");
		checkSuccess(rsmd.getColumnName(33),"Default Numeric format");
		checkSuccess(rsmd.getColumnName(34),"Default Real format");
		checkSuccess(rsmd.getColumnName(35),"Default Time format");
		checkSuccess(rsmd.getColumnName(36),"Default Timestamp format");
		checkSuccess(rsmd.getColumnName(37),"Current Role");
		checkSuccess(rsmd.getColumnName(38),"Logon Account");
		checkSuccess(rsmd.getColumnName(39),"Profile");
		checkSuccess(rsmd.getColumnName(40),"LDAP");
		checkSuccess(rsmd.getColumnName(41),"Audit Trail Id");
		checkSuccess(rsmd.getColumnName(42),"Current Isolation Level");
		checkSuccess(rsmd.getColumnName(43),"Default BigInt format");
		checkSuccess(rsmd.getColumnName(44),"QueryBand");
		checkSuccess(rsmd.getColumnName(45),"Proxy User");
		checkSuccess(rsmd.getColumnName(46),"Proxy Role");
		checkSuccess(rsmd.getColumnName(47),"Constraint1Name");
		checkSuccess(rsmd.getColumnName(48),"Constraint1Value");
		checkSuccess(rsmd.getColumnName(49),"Constraint2Name");
		checkSuccess(rsmd.getColumnName(50),"Constraint2Value");
		checkSuccess(rsmd.getColumnName(51),"Constraint3Name");
		checkSuccess(rsmd.getColumnName(52),"Constraint3Value");
		checkSuccess(rsmd.getColumnName(53),"Constraint4Name");
		checkSuccess(rsmd.getColumnName(54),"Constraint4Value");
		checkSuccess(rsmd.getColumnName(55),"Constraint5Name");
		checkSuccess(rsmd.getColumnName(56),"Constraint5Value");
		checkSuccess(rsmd.getColumnName(57),"Constraint6Name");
		checkSuccess(rsmd.getColumnName(58),"Constraint6Value");
		checkSuccess(rsmd.getColumnName(59),"Constraint7Name");
		checkSuccess(rsmd.getColumnName(60),"Constraint7Value");
		checkSuccess(rsmd.getColumnName(61),"Constraint8Name");
		checkSuccess(rsmd.getColumnName(62),"Constraint8Value");
		checkSuccess(rsmd.getColumnName(63),"Temporal Qualifier");
		checkSuccess(rsmd.getColumnName(64),"Calendar");
		checkSuccess(rsmd.getColumnName(65),"Export Width Rule Set");
		checkSuccess(rsmd.getColumnName(66),"Default Number format");
		checkSuccess(rsmd.getColumnName(67),"TTGranularity");
		checkSuccess(rsmd.getColumnName(68),"Redrive Participation");
		checkSuccess(rsmd.getColumnName(69),"User Dictionary Name");
		checkSuccess(rsmd.getColumnName(70),"User SQL Name");
		checkSuccess(rsmd.getColumnName(71),"User UEscape");
		checkSuccess(rsmd.getColumnName(72),"Account Dictionary Name");
		checkSuccess(rsmd.getColumnName(73),"Account SQL Name");
		checkSuccess(rsmd.getColumnName(74),"Account UEscape");
		checkSuccess(rsmd.getColumnName(75),
					"Current Database Dictionary Name");
		checkSuccess(rsmd.getColumnName(76),
					"Current Database SQL Name");
		checkSuccess(rsmd.getColumnName(77),
					"Current Database UEscape");
		checkSuccess(rsmd.getColumnName(78),
					"Current Role Dictionary Name");
		checkSuccess(rsmd.getColumnName(79),
					"Current Role SQL Name");
		checkSuccess(rsmd.getColumnName(80),
					"Current Role UEscape");
		checkSuccess(rsmd.getColumnName(81),
					"Logon Account Dictionary Name");
		checkSuccess(rsmd.getColumnName(82),
					"Logon Account SQL Name");
		checkSuccess(rsmd.getColumnName(83),
					"Logon Account UEscape");
		checkSuccess(rsmd.getColumnName(84),
					"Profile Dictionary Name");
		checkSuccess(rsmd.getColumnName(85),
					"Profile SQL Name");
		checkSuccess(rsmd.getColumnName(86),
					"Profile UEscape");
		checkSuccess(rsmd.getColumnName(87),
					"Audit Trail Id Dictionary Name");
		checkSuccess(rsmd.getColumnName(88),
					"Audit Trail Id SQL Name");
		checkSuccess(rsmd.getColumnName(89),
					"Audit Trail Id UEscape");
		checkSuccess(rsmd.getColumnName(90),
					"Proxy User Dictionary Name");
		checkSuccess(rsmd.getColumnName(91),
					"Proxy User SQL Name");
		checkSuccess(rsmd.getColumnName(92),
					"Proxy User UEscape");
		checkSuccess(rsmd.getColumnName(93),
					"Proxy Role Dictionary Name");
		checkSuccess(rsmd.getColumnName(94),
					"Proxy Role SQL Name");
		checkSuccess(rsmd.getColumnName(95),
					"Proxy Role UEscape");
		checkSuccess(rsmd.getColumnName(96),
					"Constraint1Name Dictionary Name");
		checkSuccess(rsmd.getColumnName(97),
					"Constraint1Name SQL Name");
		checkSuccess(rsmd.getColumnName(98),
					"Constraint1Name UEscape");
		checkSuccess(rsmd.getColumnName(99),
					"Constraint2Name Dictionary Name");
		checkSuccess(rsmd.getColumnName(100),
					"Constraint2Name SQL Name");
		checkSuccess(rsmd.getColumnName(101),
					"Constraint2Name UEscape");
		checkSuccess(rsmd.getColumnName(102),
					"Constraint3Name Dictionary Name");
		checkSuccess(rsmd.getColumnName(103),
					"Constraint3Name SQL Name");
		checkSuccess(rsmd.getColumnName(104),
					"Constraint3Name UEscape");
		checkSuccess(rsmd.getColumnName(105),
					"Constraint4Name Dictionary Name");
		checkSuccess(rsmd.getColumnName(106),
					"Constraint4Name SQL Name");
		checkSuccess(rsmd.getColumnName(107),
					"Constraint4Name UEscape");
		checkSuccess(rsmd.getColumnName(108),
					"Constraint5Name Dictionary Name");
		checkSuccess(rsmd.getColumnName(109),
					"Constraint5Name SQL Name");
		checkSuccess(rsmd.getColumnName(110),
					"Constraint5Name UEscape");
		checkSuccess(rsmd.getColumnName(111),
					"Constraint6Name Dictionary Name");
		checkSuccess(rsmd.getColumnName(112),
					"Constraint6Name SQL Name");
		checkSuccess(rsmd.getColumnName(113),
					"Constraint6Name UEscape");
		checkSuccess(rsmd.getColumnName(114),
					"Constraint7Name Dictionary Name");
		checkSuccess(rsmd.getColumnName(115),
					"Constraint7Name SQL Name");
		checkSuccess(rsmd.getColumnName(116),
					"Constraint7Name UEscape");
		checkSuccess(rsmd.getColumnName(117),
					"Constraint8Name Dictionary Name");
		checkSuccess(rsmd.getColumnName(118),
					"Constraint8Name SQL Name");
		checkSuccess(rsmd.getColumnName(119),
					"Constraint8Name UEscape");
		checkSuccess(rsmd.getColumnName(120),
					"Zone Name");
		checkSuccess(rsmd.getColumnName(121),
					"SearchUIFDBPath");
		checkSuccess(rsmd.getColumnName(122),
					"Transaction QueryBand");
		checkSuccess(rsmd.getColumnName(123),
					"Session QueryBand");
		checkSuccess(rsmd.getColumnName(124),
					"Profile QueryBand");
		checkSuccess(rsmd.getColumnName(125),
					"Unicode Pass Through");
		checkSuccess(rsmd.getColumnName(126),
					"Default Map Dictionary Name");
		checkSuccess(rsmd.getColumnName(127),
					"Default Map SQL Name");
		checkSuccess(rsmd.getColumnName(128),
					"Default Map UEscape");
		checkSuccess(rsmd.getColumnName(129),
					"Default Override");
		rs.close();
		System.out.println("\n");


		// drop
		try {
			stmt.executeUpdate("drop table testtable");
		} catch (Exception e) {
		}


		// create
		System.out.println("CREATE:");
		checkSuccess(stmt.executeUpdate(
					"create table testtable ("+
					"	col1 byteint,"+
					"	col2 smallint,"+
					"	col3 integer,"+
					"	col4 bigint,"+
					"	col5 decimal(10,3),"+
					"	col6 number(10,3),"+
					"	col7 float,"+
					"	col8 char(128),"+
					"	col9 varchar(128),"+
					"	col10 date,"+
					"	col11 time,"+
					"	col12 timestamp"+
					")"),0);
		System.out.println("\n");


		// insert
		System.out.println("INSERT:");
		checkSuccess(stmt.executeUpdate(
					"insert into testtable values ("+
					"1,"+
					"1,"+
					"1,"+
					"1,"+
					"1.123,"+
					"1.123,"+
					"1.123,"+
					"'hi1',"+
					"'hello1',"+
					"'2001-01-01',"+
					"'01:01:01',"+
					"'2001-01-01 01:01:01'"+
					")"),1);
		System.out.println("\n");


		// insert/bind
		System.out.println("INSERT BIND:");
		PreparedStatement	pstmt=con.prepareStatement(
					"insert into testtable values ("+
					"	?,?,?,?,?,?,?,?,?,?,?,?"+
					")");
		for (int i=2; i<10; i++) {
			pstmt.setShort(1,(short)i);
			pstmt.setShort(2,(short)i);
			pstmt.setInt(3,i);
			pstmt.setLong(4,(long)i);
			pstmt.setDouble(5,(double)i+0.123);
			pstmt.setDouble(6,(double)i+0.123);
			pstmt.setDouble(7,(double)i+0.123);
			pstmt.setString(8,"hi"+i);
			pstmt.setString(9,"hello"+i);
			DateFormat	fmt=new SimpleDateFormat("yyyy-MM-dd");
			pstmt.setDate(10,new java.sql.Date(
					fmt.parse("200"+i+"-0"+i+"-0"+i).
					getTime()));
			fmt=new SimpleDateFormat("hh:mm:ss");
			pstmt.setTime(11,new java.sql.Time(
					fmt.parse("0"+i+":0"+i+":0"+i).
					getTime()));
			fmt=new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
			pstmt.setTimestamp(12,new java.sql.Timestamp(
					fmt.parse("200"+i+"-0"+i+"-"+
						"0"+i+" 0"+i+":0"+i+":0"+i).
					getTime()));
			checkSuccess(pstmt.execute(),0);
		}
		System.out.println("\n");


		// select
		System.out.println("SELECT:");
		rs=stmt.executeQuery("select * from testtable order by col2");
		rsmd=rs.getMetaData();
		System.out.println("\n");

		System.out.println("SELECT - column count:");
		checkSuccess(rsmd.getColumnCount(),12);
		System.out.println("\n");

		System.out.println("SELECT - column info:");
		checkSuccess(rsmd.getColumnCount(),12);
		System.out.println();
		checkSuccess(rsmd.getColumnName(1),"col1");
		checkSuccess(rsmd.getColumnType(1),java.sql.Types.TINYINT);
		checkSuccess(rsmd.getColumnTypeName(1),"BYTEINT");
		checkSuccess(rsmd.getPrecision(1),3);
		checkSuccess(rsmd.getScale(1),0);
		checkSuccess(rsmd.isAutoIncrement(1),0);
		checkSuccess(rsmd.isCaseSensitive(1),0);
		checkSuccess(rsmd.isCurrency(1),0);
		checkSuccess(rsmd.isNullable(1),1);
		checkSuccess(rsmd.isSigned(1),1);
		System.out.println();
		checkSuccess(rsmd.getColumnName(2),"col2");
		checkSuccess(rsmd.getColumnType(2),java.sql.Types.SMALLINT);
		checkSuccess(rsmd.getColumnTypeName(2),"SMALLINT");
		checkSuccess(rsmd.getPrecision(2),5);
		checkSuccess(rsmd.getScale(2),0);
		checkSuccess(rsmd.isAutoIncrement(2),0);
		checkSuccess(rsmd.isCaseSensitive(2),0);
		checkSuccess(rsmd.isCurrency(2),0);
		checkSuccess(rsmd.isNullable(2),1);
		checkSuccess(rsmd.isSigned(2),1);
		System.out.println();
		checkSuccess(rsmd.getColumnName(3),"col3");
		checkSuccess(rsmd.getColumnType(3),java.sql.Types.INTEGER);
		checkSuccess(rsmd.getColumnTypeName(3),"INTEGER");
		checkSuccess(rsmd.getPrecision(3),10);
		checkSuccess(rsmd.getScale(3),0);
		checkSuccess(rsmd.isAutoIncrement(3),0);
		checkSuccess(rsmd.isCaseSensitive(3),0);
		checkSuccess(rsmd.isCurrency(3),0);
		checkSuccess(rsmd.isNullable(3),1);
		checkSuccess(rsmd.isSigned(3),1);
		System.out.println();
		checkSuccess(rsmd.getColumnName(4),"col4");
		checkSuccess(rsmd.getColumnType(4),java.sql.Types.BIGINT);
		checkSuccess(rsmd.getColumnTypeName(4),"BIGINT");
		checkSuccess(rsmd.getPrecision(4),19);
		checkSuccess(rsmd.getScale(4),0);
		checkSuccess(rsmd.isAutoIncrement(4),0);
		checkSuccess(rsmd.isCaseSensitive(4),0);
		checkSuccess(rsmd.isCurrency(4),0);
		checkSuccess(rsmd.isNullable(4),1);
		checkSuccess(rsmd.isSigned(4),1);
		System.out.println();
		checkSuccess(rsmd.getColumnName(5),"col5");
		checkSuccess(rsmd.getColumnType(5),java.sql.Types.DECIMAL);
		checkSuccess(rsmd.getColumnTypeName(5),"DECIMAL");
		checkSuccess(rsmd.getPrecision(5),10);
		checkSuccess(rsmd.getScale(5),3);
		checkSuccess(rsmd.isAutoIncrement(5),0);
		checkSuccess(rsmd.isCaseSensitive(5),0);
		checkSuccess(rsmd.isCurrency(5),0);
		checkSuccess(rsmd.isNullable(5),1);
		checkSuccess(rsmd.isSigned(5),1);
		System.out.println();
		checkSuccess(rsmd.getColumnName(6),"col6");
		// Teradata...
		//checkSuccess(rsmd.getColumnType(6),java.sql.Types.NUMERIC);
		//checkSuccess(rsmd.getColumnTypeName(6),"NUMERIC");
		// SQL Relay...
		// (ODBC backend returns SQL_DECIMAL instead of SQL_NUMERIC)...
		//checkSuccess(rsmd.getColumnType(6),java.sql.Types.DECIMAL);
		//checkSuccess(rsmd.getColumnTypeName(6),"DECIMAL");
		checkSuccess(rsmd.getPrecision(6),10);
		checkSuccess(rsmd.getScale(6),3);
		checkSuccess(rsmd.isAutoIncrement(6),0);
		checkSuccess(rsmd.isCaseSensitive(6),0);
		checkSuccess(rsmd.isCurrency(6),0);
		checkSuccess(rsmd.isNullable(6),1);
		checkSuccess(rsmd.isSigned(6),1);
		System.out.println();
		checkSuccess(rsmd.getColumnName(7),"col7");
		checkSuccess(rsmd.getColumnType(7),java.sql.Types.FLOAT);
		checkSuccess(rsmd.getColumnTypeName(7),"FLOAT");
		checkSuccess(rsmd.getPrecision(7),15);
		checkSuccess(rsmd.getScale(7),0);
		checkSuccess(rsmd.isAutoIncrement(7),0);
		checkSuccess(rsmd.isCaseSensitive(7),0);
		checkSuccess(rsmd.isCurrency(7),0);
		checkSuccess(rsmd.isNullable(7),1);
		checkSuccess(rsmd.isSigned(7),1);
		System.out.println();
		checkSuccess(rsmd.getColumnName(8),"col8");
		checkSuccess(rsmd.getColumnType(8),java.sql.Types.CHAR);
		checkSuccess(rsmd.getColumnTypeName(8),"CHAR");
		checkSuccess(rsmd.getPrecision(8),128);
		checkSuccess(rsmd.getScale(8),0);
		checkSuccess(rsmd.isAutoIncrement(8),0);
		checkSuccess(rsmd.isCaseSensitive(8),0);
		checkSuccess(rsmd.isCurrency(8),0);
		checkSuccess(rsmd.isNullable(8),1);
		checkSuccess(rsmd.isSigned(8),0);
		System.out.println();
		checkSuccess(rsmd.getColumnName(9),"col9");
		checkSuccess(rsmd.getColumnType(9),java.sql.Types.VARCHAR);
		checkSuccess(rsmd.getColumnTypeName(9),"VARCHAR");
		checkSuccess(rsmd.getPrecision(9),128);
		checkSuccess(rsmd.getScale(9),0);
		checkSuccess(rsmd.isAutoIncrement(9),0);
		checkSuccess(rsmd.isCaseSensitive(9),0);
		checkSuccess(rsmd.isCurrency(9),0);
		checkSuccess(rsmd.isNullable(9),1);
		checkSuccess(rsmd.isSigned(9),0);
		System.out.println();
		checkSuccess(rsmd.getColumnName(10),"col10");
		checkSuccess(rsmd.getColumnType(10),java.sql.Types.DATE);
		checkSuccess(rsmd.getColumnTypeName(10),"DATE");
		checkSuccess(rsmd.getPrecision(10),10);
		checkSuccess(rsmd.getScale(10),0);
		checkSuccess(rsmd.isAutoIncrement(10),0);
		checkSuccess(rsmd.isCaseSensitive(10),0);
		checkSuccess(rsmd.isCurrency(10),0);
		checkSuccess(rsmd.isNullable(10),1);
		checkSuccess(rsmd.isSigned(10),0);
		System.out.println();
		checkSuccess(rsmd.getColumnName(11),"col11");
		checkSuccess(rsmd.getColumnType(11),java.sql.Types.TIME);
		checkSuccess(rsmd.getColumnTypeName(11),"TIME");
		checkSuccess(rsmd.getPrecision(11),15);
		checkSuccess(rsmd.getScale(11),6);
		checkSuccess(rsmd.isAutoIncrement(11),0);
		checkSuccess(rsmd.isCaseSensitive(11),0);
		checkSuccess(rsmd.isCurrency(11),0);
		checkSuccess(rsmd.isNullable(11),1);
		checkSuccess(rsmd.isSigned(11),0);
		System.out.println();
		checkSuccess(rsmd.getColumnName(12),"col12");
		checkSuccess(rsmd.getColumnType(12),java.sql.Types.TIMESTAMP);
		checkSuccess(rsmd.getColumnTypeName(12),"TIMESTAMP");
		checkSuccess(rsmd.getPrecision(12),26);
		checkSuccess(rsmd.getScale(12),6);
		checkSuccess(rsmd.isAutoIncrement(12),0);
		checkSuccess(rsmd.isCaseSensitive(12),0);
		checkSuccess(rsmd.isCurrency(12),0);
		checkSuccess(rsmd.isNullable(12),1);
		checkSuccess(rsmd.isSigned(12),0);
		System.out.println("\n");

		System.out.println("SELECT - fields:");
		for (int i=1; i<10; i++) {
			checkSuccess(rs.next(),1);
			checkSuccess(rs.getString(1),""+i);
			checkSuccess(rs.getString(2),""+i);
			checkSuccess(rs.getString(3),""+i);
			checkSuccess(rs.getString(4),""+i);
			checkSuccess(rs.getString(5),i+".123");
			checkSuccess(rs.getString(6),i+".123");
			checkSuccess(rs.getString(7),i+".123");
			checkSuccess(rs.getString(8).trim(),"hi"+i);
			checkSuccess(rs.getString(9),"hello"+i);
			checkSuccess(rs.getString(10),"200"+i+"-0"+i+"-0"+i);
			checkSuccess(rs.getString(11),"0"+i+":0"+i+":0"+i);
			checkSuccess(rs.getString(12),"200"+i+"-0"+i+"-0"+i+
							" "+
							"0"+i+":0"+i+":0"+i+
							".0");
			System.out.println();
		}
		rs.close();
		System.out.println();


		// update
		System.out.println("UPDATE:");
		checkSuccess(stmt.executeUpdate(
				"update testtable set col1=3 where col1=1"),1);
		checkSuccess(stmt.executeUpdate(
				"update testtable set col1=4 where col1=2"),1);
		System.out.println("\n");


		// delete
		System.out.println("DELETE:");
		checkSuccess(stmt.executeUpdate("delete from testtable"),9);
		System.out.println("\n");


		// even nulls
		System.out.println("EVEN NULLS:");
		checkSuccess(stmt.executeUpdate(
					"insert into testtable values ("+
					"1,"+
					"null,"+
					"1,"+
					"null,"+
					"1.123,"+
					"null,"+
					"1.123,"+
					"null,"+
					"'hello1',"+
					"null,"+
					"'01:01:01',"+
					"null"+
					")"),1);
		System.out.println();
		rs=stmt.executeQuery("select * from testtable");
		rsmd=rs.getMetaData();
		checkSuccess(rs.next(),1);
		checkSuccess(rs.getString(1),"1");
		checkSuccess(rs.getString(2),null);
		checkSuccess(rs.getString(3),"1");
		checkSuccess(rs.getString(4),null);
		checkSuccess(rs.getString(5),"1.123");
		checkSuccess(rs.getString(6),null);
		checkSuccess(rs.getString(7),"1.123");
		checkSuccess(rs.getString(8),null);
		checkSuccess(rs.getString(9),"hello1");
		checkSuccess(rs.getString(10),null);
		checkSuccess(rs.getString(11),"01:01:01");
		checkSuccess(rs.getString(12),null);
		System.out.println();
		checkSuccess(stmt.executeUpdate("delete from testtable"),1);
		System.out.println("\n");


		// odd nulls
		System.out.println("ODD NULLS:");
		checkSuccess(stmt.executeUpdate(
					"insert into testtable values ("+
					"null,"+
					"1,"+
					"null,"+
					"1,"+
					"null,"+
					"1.123,"+
					"null,"+
					"'hi1',"+
					"null,"+
					"'2001-01-01',"+
					"null,"+
					"'2001-01-01 01:01:01'"+
					")"),1);
		System.out.println();
		rs=stmt.executeQuery("select * from testtable");
		rsmd=rs.getMetaData();
		checkSuccess(rs.next(),1);
		checkSuccess(rs.getString(1),null);
		checkSuccess(rs.getString(2),"1");
		checkSuccess(rs.getString(3),null);
		checkSuccess(rs.getString(4),"1");
		checkSuccess(rs.getString(5),null);
		checkSuccess(rs.getString(6),"1.123");
		checkSuccess(rs.getString(7),null);
		checkSuccess(rs.getString(8).trim(),"hi1");
		checkSuccess(rs.getString(9),null);
		checkSuccess(rs.getString(10),"2001-01-01");
		checkSuccess(rs.getString(11),null);
		checkSuccess(rs.getString(12),"2001-01-01 01:01:01.0");
		System.out.println();
		checkSuccess(stmt.executeUpdate("delete from testtable"),1);
		System.out.println("\n");


		// even null binds
		System.out.println("EVEN NULL BINDS:");
		pstmt=con.prepareStatement(
					"insert into testtable values ("+
					"	?,?,?,?,?,?,?,?,?,?,?,?"+
					")");
		pstmt.setShort(1,(short)1);
		pstmt.setNull(2,java.sql.Types.SMALLINT);
		pstmt.setInt(3,1);
		pstmt.setNull(4,java.sql.Types.BIGINT);
		pstmt.setDouble(5,1.123);
		pstmt.setNull(6,java.sql.Types.CHAR);
		pstmt.setDouble(7,1.123);
		pstmt.setNull(8,java.sql.Types.CHAR);
		pstmt.setString(9,"hello1");
		pstmt.setNull(10,java.sql.Types.DATE);
		DateFormat	fmt=new SimpleDateFormat("hh:mm:ss");
		pstmt.setTime(11,new java.sql.Time(
					fmt.parse("01:01:01").
					getTime()));
		fmt=new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
		pstmt.setNull(12,java.sql.Types.TIMESTAMP);
		checkSuccess(pstmt.execute(),0);
		System.out.println();
		rs=stmt.executeQuery("select * from testtable");
		rsmd=rs.getMetaData();
		checkSuccess(rs.next(),1);
		checkSuccess(rs.getString(1),"1");
		checkSuccess(rs.getString(2),null);
		checkSuccess(rs.getString(3),"1");
		checkSuccess(rs.getString(4),null);
		checkSuccess(rs.getString(5),"1.123");
		checkSuccess(rs.getString(6),null);
		checkSuccess(rs.getString(7),"1.123");
		checkSuccess(rs.getString(8),null);
		checkSuccess(rs.getString(9),"hello1");
		checkSuccess(rs.getString(10),null);
		checkSuccess(rs.getString(11),"01:01:01");
		checkSuccess(rs.getString(12),null);
		System.out.println();
		checkSuccess(stmt.executeUpdate("delete from testtable"),1);
		System.out.println("\n");


		// odd null binds
		System.out.println("ODD NULL BINDS:");
		pstmt=con.prepareStatement(
					"insert into testtable values ("+
					"	?,?,?,?,?,?,?,?,?,?,?,?"+
					")");
		pstmt.setNull(1,java.sql.Types.SMALLINT);
		pstmt.setShort(2,(short)1);
		pstmt.setNull(3,java.sql.Types.INTEGER);
		pstmt.setLong(4,(long)1);
		pstmt.setNull(5,java.sql.Types.DOUBLE);
		pstmt.setDouble(6,1.123);
		pstmt.setNull(7,java.sql.Types.DOUBLE);
		pstmt.setString(8,"hi1");
		pstmt.setNull(9,java.sql.Types.CHAR);
		fmt=new SimpleDateFormat("yyyy-MM-dd");
		pstmt.setDate(10,new java.sql.Date(
					fmt.parse("2001-01-01").
					getTime()));
		pstmt.setNull(11,java.sql.Types.TIME);
		fmt=new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
		pstmt.setTimestamp(12,new java.sql.Timestamp(
					fmt.parse("2001-01-01 01:01:01").
					getTime()));
		checkSuccess(pstmt.execute(),0);
		System.out.println();
		rs=stmt.executeQuery("select * from testtable");
		rsmd=rs.getMetaData();
		checkSuccess(rs.next(),1);
		checkSuccess(rs.getString(1),null);
		checkSuccess(rs.getString(2),"1");
		checkSuccess(rs.getString(3),null);
		checkSuccess(rs.getString(4),"1");
		checkSuccess(rs.getString(5),null);
		checkSuccess(rs.getString(6),"1.123");
		checkSuccess(rs.getString(7),null);
		checkSuccess(rs.getString(8).trim(),"hi1");
		checkSuccess(rs.getString(9),null);
		checkSuccess(rs.getString(10),"2001-01-01");
		checkSuccess(rs.getString(11),null);
		checkSuccess(rs.getString(12),"2001-01-01 01:01:01.0");
		System.out.println();
		checkSuccess(stmt.executeUpdate("delete from testtable"),1);
		System.out.println("\n");

/*
		// fastload
		System.out.println("FASTLOAD:");
		//int	count=20000;
		//int	count=5000;
		//int	count=500;
		int	count=10;
		//int	count=2;
		//int	count=1;
		for (int i=1; i<=count; i++) {
			pstmt.setInt(1,(i>127)?127:i);
			pstmt.setInt(2,i);
			pstmt.setInt(3,i);
			pstmt.setInt(4,i);
			pstmt.setDouble(5,2.345);
			pstmt.setDouble(6,2.345);
			pstmt.setDouble(7,2.345);
			pstmt.setString(8,"hi");
			pstmt.setString(9,"hello");
			fmt=new SimpleDateFormat("yyyy-MM-dd");
			pstmt.setDate(10,new java.sql.Date(
						fmt.parse("2002-02-02").
						getTime()));
			fmt=new SimpleDateFormat("hh:mm:ss");
			pstmt.setTime(11,new java.sql.Time(
						fmt.parse("02:02:02").
						getTime()));
			fmt=new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
			pstmt.setTimestamp(12,new java.sql.Timestamp(
					fmt.parse("2002-02-02 02:02:02").
					getTime()));
			pstmt.addBatch();
		}
		int updatecounts[]=pstmt.executeBatch();

		// select
		System.out.println("SELECT:");
		rs=stmt.executeQuery("select * from testtable");
		displayResult(rs);
		rs.close();
*/


		// close
		stmt.close();


		// drop
		System.out.println("DROP:");
		checkSuccess(stmt.executeUpdate("drop table testtable"),26);
		// (teradata returns 26 for drop-table, for some reason)
		System.out.println("\n");
}

		con.close();
	}

	static void displayResult(ResultSet rs) throws Exception {
		int	row=1;
		while (rs.next()) {
			System.out.println(row+" {");
			System.out.println("	"+rs.getInt(1));
			System.out.println("	"+rs.getInt(2));
			System.out.println("	"+rs.getInt(3));
			System.out.println("	"+rs.getInt(4));
			System.out.println("	"+rs.getDouble(5));
			System.out.println("	"+rs.getDouble(6));
			System.out.println("	"+rs.getDouble(7));
			System.out.println("	"+rs.getString(8));
			System.out.println("	"+rs.getString(9));
			System.out.println("	"+rs.getString(10));
			System.out.println("	"+rs.getString(11));
			System.out.println("	"+rs.getString(12));
			System.out.println("}");
			row++;
		}
	}

	static void displayStringResult(ResultSet rs) throws Exception {
		ResultSetMetaData	rsmd=rs.getMetaData();
		int	row=1;
		while (rs.next()) {
			System.out.println(row+" {");
			for (int i=1; i<=rsmd.getColumnCount(); i++) {
				System.out.println("	"+
						rsmd.getColumnName(i)+": "+
						rs.getString(i));
			}
			System.out.println("}");
			row++;
		}
	}
}
