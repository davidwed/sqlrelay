using System;
using SQLRClient;

namespace SQLRExamples
{
	class SQLRExample
	{
		public static void Main()
		{
			SQLRConnection con = new SQLRConnection("sqlrserver", 9000, "/tmp/example.socket", "user", "password", 0, 1);
			SQLRCursor cur = new SQLRCursor(con);

			cur.sendQuery("insert into my_table values (1,2,3)");
        		cur.suspendResultSet();
        		con.suspendSession();
        		UInt16 rs = cur.getResultSetId();
        		UInt16 port = cur.getConnectionPort();
        		String socket = cur.getConnectionSocket();

        		... pass the rs, port and socket to the next page ...
		}
	}
}
