// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

import java.sql.*;

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

		Class.forName("com.firstworks.sql.SQLRelayDriver");

		String	host="localhost";
		short	port=9000;
		String	socket=null;
		String	user="test";
		String	password="test";

		System.out.println("CONNECTION");
		Connection	con=DriverManager.getConnection(
				"jdbc:sqlrelay://"+user+":"+password+"@"+
				host+":"+port,
				"test","test");
		// FIXME: how to get SQLRelayConnection?
		//checkSuccess(sqlrcon.getHost(),host);
		//checkSuccess(sqlrcon.getPort(),port);
		//checkSuccess(sqlrcon.getSocket(),socket);
		//checkSuccess(sqlrcon.getUser(),user);
		//checkSuccess(sqlrcon.getPassword(),password);
		Statement	stmt=con.createStatement();
		checkSuccess((stmt!=null),1);
		stmt.close();
		stmt=con.createStatement(
				ResultSet.TYPE_FORWARD_ONLY,
				ResultSet.CONCUR_READ_ONLY,
				ResultSet.CLOSE_CURSORS_AT_COMMIT);
		checkSuccess((stmt!=null),1);
		stmt.close();
		stmt=con.createStatement(
				ResultSet.TYPE_SCROLL_INSENSITIVE,
				ResultSet.CONCUR_READ_ONLY,
				ResultSet.CLOSE_CURSORS_AT_COMMIT);
		checkSuccess((stmt!=null),1);
		stmt.close();
		try {
			stmt=con.createStatement(
				ResultSet.TYPE_SCROLL_SENSITIVE,
				ResultSet.CONCUR_UPDATABLE,
				ResultSet.CLOSE_CURSORS_AT_COMMIT);
			checkSuccess(false,1);
		} catch (Exception ex) {
			checkSuccess(true,1);
		}
		try {
			stmt=con.createStatement(
				ResultSet.TYPE_FORWARD_ONLY,
				ResultSet.CONCUR_UPDATABLE,
				ResultSet.CLOSE_CURSORS_AT_COMMIT);
			checkSuccess(false,1);
		} catch (Exception ex) {
			checkSuccess(true,1);
		}
		try {
			stmt=con.createStatement(
				ResultSet.TYPE_FORWARD_ONLY,
				ResultSet.CONCUR_READ_ONLY,
				ResultSet.HOLD_CURSORS_OVER_COMMIT);
			checkSuccess(false,1);
		} catch (Exception ex) {
			checkSuccess(true,1);
		}
		// commit
		// createArrayOf - unsupported
		// createBlob
		// createClob
		// createNClob
		// createSQLXML - unsupported
		// createStruct - unsupported
		// getAutoCommit
		// getCatalog
		// getClientInfo...
		// getHoldability
		// getMetaData
		// getNetworkTimeout
		// getSchema
		// getTransactionIsolation
		// getTypeMap
		// getWarnings
		// isClosed
		// isValid
		// nativeSql
		// prepareCall...
		// prepareStatement...
		// releaseSavepoint
		// rollback...
		// setAutoCommit
		// setCatalog
		// setClientInfo...
		// setHoldability
		// setNetworkTimeout
		// setReadOnly
		// setSavepoint...
		// isWrapperFor
		// unwrap
		System.out.println();
		System.out.println();


		System.out.println("STATEMENT");
		stmt=con.createStatement();
		System.out.println();
		System.out.println();


		System.out.println("RESULTSET");
		ResultSet	rs=stmt.executeQuery("select 1 from dual");
		checkSuccess((rs!=null),1);
		rs.next();
		checkSuccess(rs.getInt(1),1);
		rs.close();
		System.out.println();
		System.out.println();


		stmt.close();
		con.close();

		System.exit(0);
	}
}
