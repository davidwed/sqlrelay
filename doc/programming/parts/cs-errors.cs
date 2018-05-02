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


        		if (cur.sendQuery("select * from my_nonexistant_table") != true)
			{
                		Console.WriteLine(cur.errorMessage());
        		}
		}
	}
}
