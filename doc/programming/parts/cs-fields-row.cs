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

			cur.sendQuery("select * from my_table");
        		con.endSession();

        		for (UInt64 row=0; row&lt;cur.rowCount(); row++)
			{
                		String[] rowarray=cur.getRow(row);
                		for (UInt32 col=0; col&lt;cur.colCount(); col++)
				{
                        		Console.Write(rowarray[col]);
					Console.Write(",");
                		}
				Console.Write("\n");
        		}
		}
	}
}
