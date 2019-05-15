// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

import java.sql.*;

class odbc {

	public static void main(String args[]) throws Exception {

		Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");

		Connection	con=DriverManager.getConnection(
					"jdbc:odbc:sqlrodbc","test","test");

		DatabaseMetaData	dmd=con.getMetaData();
		ResultSet		rs=dmd.getSchemas();
		while (rs.next()) {
		}
		rs.close();
		//rs=dmd.getTables(null,null,null, new String[] {"TABLE"});
		rs=dmd.getTables(null,null,"%",null);
		while (rs.next()) {
		}
		rs.close();

		Statement	stmt=con.createStatement();
		rs=stmt.executeQuery("select 1");
		rs.next();
		rs.getInt(1);
		rs.close();

		stmt.close();

		con.close();
	}
}
