// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

import java.sql.*;

class oracle {

	public static void main(String args[]) throws Exception {

		Class.forName("com.firstworks.sql.SQLRelayDriver");

		Connection	con=DriverManager.getConnection(
					"jdbc:sqlrelay://test:test@localhost:9000",
					"test","test");

		Statement	stmt=con.createStatement();

		ResultSet	rs=stmt.executeQuery("select 1 from dual");
		rs.next();
		System.out.println(rs.getInt(1));
		rs.close();

		stmt.close();

		con.close();
	}
}
