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

			... generate a unique filename ...

        		cur.cacheToFile(filename);
        		cur.setCacheTtl(600);
        		cur.sendQuery("select * from my_table");
        		con.endSession();
        		cur.cacheOff();

        		... pass the filename to the next page ...

		}
	}
}
