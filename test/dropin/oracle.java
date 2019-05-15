// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

import java.sql.*;

class oracle {

	public static void main(String args[]) throws Exception {

		Class.forName("oracle.jdbc.OracleDriver");

		Connection	con=DriverManager.getConnection(
					"jdbc:oracle:thin:@localhost:1522:ora1",
					"testuser","testpassword");

		Statement	stmt=con.createStatement();

		ResultSet	rs=stmt.executeQuery("select 1 from dual");
		rs.next();
		rs.getInt(1);
		rs.close();

		stmt.close();

		con.close();
	}
}
