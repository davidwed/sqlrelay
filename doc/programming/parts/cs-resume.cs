using System;
using SQLRClient;

namespace SQLRExamples
{
	class SQLRExample
	{
		public static void Main()
		{
			... get rs, port and socket from previous page ...

			SQLRConnection con = new SQLRConnection("sqlrserver", 9000, "/tmp/example.socket", "user", "password", 0, 1);
			SQLRCursor cur = new SQLRCursor(con);

			con.resumeSession(port, socket);
        		cur.resumeResultSet(rs);
        		cur.sendQuery("commit");
        		con.endSession();
		}
	}
}
