// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information.

import java.sql.*;
import java.math.*;

class postgresqlbinds {

	public static void main(String args[]) throws Exception {

		Class.forName("org.postgresql.Driver");

		Connection	con=DriverManager.getConnection(
					"jdbc:postgresql://localhost/testdb",
					"testuser","testpassword");

		PreparedStatement	stmt=con.prepareStatement(
					"select " +
					"	?::bool, " +
					"	?::smallint, " +
					"	?::int, " +
					"	?::bigint, " +
					"	?::float4, " +
					"	?::float8, " +
					"	?::numeric, " +
					"	?::date, " +
					"	?::time, " +
					"	?::timestamp "
					);
		int	index=1;
		stmt.setBoolean(index++,true);
		stmt.setShort(index++,(short)1);
		stmt.setInt(index++,1);
		stmt.setLong(index++,1);
		stmt.setFloat(index++,(float)1.123);
		stmt.setDouble(index++,1.123);
		// the rest apparently use a text bind
		stmt.setBigDecimal(index++,new BigDecimal("-100.10"));
		long	current=System.currentTimeMillis();
		stmt.setDate(index++,new Date(current));
		stmt.setTime(index++,new Time(current));
		stmt.setTimestamp(index++,new Timestamp(current));

		ResultSet	rs=stmt.executeQuery();
		rs.next();
		for (int i=1; i<index; i++) {
			System.out.println(rs.getString(i));
		}
		rs.close();

		stmt.close();

		con.close();

		System.exit(0);
	}
}
