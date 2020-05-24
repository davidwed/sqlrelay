// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

import java.sql.*;
import com.firstworks.sqlrelay.*;
import com.firstworks.sql.*;
import java.util.Properties;

class oracle {

	private static void checkSuccess(String value, String success, int length) {
	
		if (success==null) {
			if (value==null) {
				System.out.printf("success ");
				return;
			} else {
				System.out.printf(value+"!="+success+" ");
				System.out.printf("failure ");
				System.exit(1);
			}
		}
	
		if (value.regionMatches(0,success,0,length)) {
			System.out.printf("success ");
		} else {
			System.out.printf(value+"!="+success+" ");
			System.out.printf("failure ");
			System.exit(1);
		}
	}
	
	private static void checkSuccess(String value, String success) {
	
		if (success==null) {
			if (value==null) {
				System.out.printf("success ");
				return;
			} else {
				System.out.printf(value+"!="+success+" ");
				System.out.printf("failure ");
				System.exit(1);
			}
		}
	
		if (value.equals(success)) {
			System.out.printf("success ");
		} else {
			System.out.printf(value+"!="+success+" ");
			System.out.printf("failure ");
			System.exit(1);
		}
	}
	
	private static void checkSuccess(byte[] value, String success, int length) {
	
		if (success==null) {
			if (value==null) {
				System.out.printf("success ");
				return;
			} else {
				System.out.printf("failure ");
				
				
				System.exit(1);
			}
		}

		byte[]	successvalue=success.getBytes();
	
		for (int index=0; index<length; index++) {
			if (value[index]!=successvalue[index]) {
				System.out.printf("failure ");
				System.exit(1);
			}
		}

		System.out.printf("success ");
	}
	
	private static void checkSuccess(long value, int success) {
	
		if (value==success) {
			System.out.printf("success ");
		} else {
			System.out.printf("failure ");
			
			
			System.exit(1);
		}
	}
	
	private static void checkSuccess(double value, double success) {
	
		if (value==success) {
			System.out.printf("success ");
		} else {
			System.out.printf("failure ");
			
			
			System.exit(1);
		}
	}
	
	private static void checkSuccess(boolean value, int success) {
	
		if (((value)?1:0)==success) {
			System.out.printf("success ");
		} else {
			System.out.printf("failure ");
			
			
			System.exit(1);
		}
	}

	public static void main(String args[]) throws Exception {

		String	host="localhost";
		short	port=9000;
		String	socket=null;
		String	user="test";
		String	password="test";
		String	url="jdbc:sqlrelay://"+
				user+":"+password+"@"+host+":"+port;

		// Connection
		System.out.println("CONNECTION...");
		Class.forName("com.firstworks.sql.SQLRelayDriver");
		Connection	con=DriverManager.getConnection(
							url,"test","test");

		// close, isClosed, isValid
		System.out.println("CONNECTION - close");
		checkSuccess(con.isClosed(),0);
		checkSuccess(con.isValid(0),1);
		con.close();
		checkSuccess(con.isClosed(),1);
		checkSuccess(con.isValid(0),0);
		con=DriverManager.getConnection(url,"test","test");
		System.out.println();

		// setNetworkTimeout, getNetworkTimeout
		con.setNetworkTimeout(null,1);
		checkSuccess(con.getNetworkTimeout(),1);
		con.setNetworkTimeout(null,2);
		checkSuccess(con.getNetworkTimeout(),2);
		con.setNetworkTimeout(null,0);
		checkSuccess(con.getNetworkTimeout(),0);
		System.out.println();

		// SQLRelayConnection
		System.out.println("CONNECTION - SQLRelayConnection");
		SQLRelayConnection	sqlrcon=(SQLRelayConnection)con;
		checkSuccess(sqlrcon.getHost(),host);
		checkSuccess(sqlrcon.getPort(),port);
		checkSuccess(sqlrcon.getSocket(),socket);
		checkSuccess(sqlrcon.getUser(),user);
		checkSuccess(sqlrcon.getPassword(),password);
		System.out.println();

		// isWrapperFor, unwrap
		System.out.println("CONNECTION - unwrap");
		checkSuccess(con.isWrapperFor(SQLRConnection.class),1);
		checkSuccess((con.unwrap(SQLRConnection.class)!=null),1);
		System.out.println();

		// setCatalog, getCatalog
		System.out.println("CONNECTION - catalog");
		con.setCatalog("TESTUSER");
		checkSuccess(con.getCatalog(),"TESTUSER");
		System.out.println();

		// setSchema, getSchema
		System.out.println("CONNECTION - schema");
		con.setSchema("");
		checkSuccess(con.getSchema(),"");
		System.out.println();

		// setClientInfo, getClientInfo()
		System.out.println("CONNECTION - client info");
		Properties	inprop=new Properties();
		inprop.setProperty("key1","value1");
		inprop.setProperty("key2","value2");
		con.setClientInfo(inprop);
		con.setClientInfo("key3","value3");
		con.setClientInfo("key4","value4");
		checkSuccess(con.getClientInfo("key1"),"value1");
		checkSuccess(con.getClientInfo("key2"),"value2");
		checkSuccess(con.getClientInfo("key3"),"value3");
		checkSuccess(con.getClientInfo("key4"),"value4");
		Properties	outprop=con.getClientInfo();
		checkSuccess(outprop.getProperty("key1"),"value1");
		checkSuccess(outprop.getProperty("key2"),"value2");
		checkSuccess(outprop.getProperty("key3"),"value3");
		checkSuccess(outprop.getProperty("key4"),"value4");
		System.out.println();

		// setReadOnly, isReadOnly
		System.out.println("CONNECTION - readonly");
		con.setReadOnly(true);
		checkSuccess(con.isReadOnly(),1);
		con.setReadOnly(false);
		checkSuccess(!con.isReadOnly(),1);
		System.out.println();

		// setAutoCommit, getAutoCommit
		System.out.println("CONNECTION - autocommit");
		con.setAutoCommit(true);
		checkSuccess(con.getAutoCommit(),1);
		con.setAutoCommit(false);
		checkSuccess(!con.getAutoCommit(),1);
		System.out.println();

		// setHoldability, getHoldability
		System.out.println("CONNECTION - holdability");
		con.setHoldability(ResultSet.HOLD_CURSORS_OVER_COMMIT);
		checkSuccess(con.getHoldability()==
				ResultSet.HOLD_CURSORS_OVER_COMMIT,1);
		try {
			con.setHoldability(ResultSet.CLOSE_CURSORS_AT_COMMIT);
			checkSuccess(false,1);
		} catch (Exception ex) {
			checkSuccess(true,1);
		}
		System.out.println();

		// setTransactionIsolation, getTransactionIsolation
		System.out.println("CONNECTION - isolation");
		con.setTransactionIsolation(
			Connection.TRANSACTION_READ_UNCOMMITTED);
		checkSuccess(con.getTransactionIsolation(),
			Connection.TRANSACTION_READ_UNCOMMITTED);
		con.setTransactionIsolation(
			Connection.TRANSACTION_READ_COMMITTED);
		checkSuccess(con.getTransactionIsolation(),
			Connection.TRANSACTION_READ_COMMITTED);
		con.setTransactionIsolation(
			Connection.TRANSACTION_REPEATABLE_READ);
		checkSuccess(con.getTransactionIsolation(),
			Connection.TRANSACTION_REPEATABLE_READ);
		con.setTransactionIsolation(
			Connection.TRANSACTION_SERIALIZABLE);
		checkSuccess(con.getTransactionIsolation(),
			Connection.TRANSACTION_SERIALIZABLE);
		try {
			con.setTransactionIsolation(10);
			checkSuccess(false,1);
		} catch (Exception ex) {
			checkSuccess(true,1);
		}
		System.out.println();

		// setTypeMap, getTypeMap

		// getWarnings, clearWarnings
		System.out.println("CONNECTION - warnings");
		checkSuccess(con.getWarnings()==null,1);
		con.clearWarnings();
		System.out.println();

		// getMetaData

		// nativeSql

		// createBlob
		// createClob
		// createNClob

		// createArrayOf - unsupported
		// createSQLXML - unsupported
		// createStruct - unsupported

		// setSavepoint...
		// releaseSavepoint

		// commit
		// rollback...

		// createStatement
		System.out.println("CONNECTION - create statement");
		Statement	stmt=con.createStatement();
		checkSuccess((stmt!=null),1);
		stmt.close();
		stmt=con.createStatement(
				ResultSet.TYPE_FORWARD_ONLY,
				ResultSet.CONCUR_READ_ONLY,
				ResultSet.HOLD_CURSORS_OVER_COMMIT);
		checkSuccess((stmt!=null),1);
		stmt.close();
		stmt=con.createStatement(
				ResultSet.TYPE_SCROLL_INSENSITIVE,
				ResultSet.CONCUR_READ_ONLY,
				ResultSet.HOLD_CURSORS_OVER_COMMIT);
		checkSuccess((stmt!=null),1);
		stmt.close();
		try {
			stmt=con.createStatement(
				ResultSet.TYPE_SCROLL_SENSITIVE,
				ResultSet.CONCUR_UPDATABLE,
				ResultSet.HOLD_CURSORS_OVER_COMMIT);
			checkSuccess(false,1);
		} catch (Exception ex) {
			checkSuccess(true,1);
		}
		try {
			stmt=con.createStatement(
				ResultSet.TYPE_FORWARD_ONLY,
				ResultSet.CONCUR_UPDATABLE,
				ResultSet.HOLD_CURSORS_OVER_COMMIT);
			checkSuccess(false,1);
		} catch (Exception ex) {
			checkSuccess(true,1);
		}
		try {
			stmt=con.createStatement(
				ResultSet.TYPE_FORWARD_ONLY,
				ResultSet.CONCUR_READ_ONLY,
				ResultSet.CLOSE_CURSORS_AT_COMMIT);
			checkSuccess(false,1);
		} catch (Exception ex) {
			checkSuccess(true,1);
		}
		System.out.println();

		// prepareCall...

		// prepareStatement...
		System.out.println("\n");


		// Statement
		System.out.println("STATEMENT...");
		stmt=con.createStatement();
		System.out.println("\n");


		// ResultSet
		System.out.println("RESULTSET...");
		ResultSet	rs=stmt.executeQuery("select 1 from dual");
		checkSuccess((rs!=null),1);
		rs.next();
		checkSuccess(rs.getInt(1),1);
		rs.close();
		System.out.println("\n");


		stmt.close();
		con.close();

		System.exit(0);
	}
}
