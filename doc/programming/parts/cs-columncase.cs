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

			// column names will be forced to upper case
        		cur.upperCaseColumnNames();
        		cur.sendQuery("select * from my_table");
        		con.endSession();

        		for (UInt32 i=0; i&lt;cur.colCount(); i++)
			{
                		Console.Write("Name:          ");
				Console.WriteLine(getColumnName(i));
                		Console.Write("\n");
        		}

        		// column names will be forced to lower case
        		cur.lowerCaseColumnNames();
        		cur.sendQuery("select * from my_table");
        		con.endSession();

        		for (UInt32 i=0; i&lt;cur.colCount(); i++)
			{
                		Console.Write("Name:          ");
				Console.WriteLine(cur.getColumnName(i));
                		Console.Write("\n");
        		}

        		// column names will be the same as they are in the database
        		cur.mixedCaseColumnNames();
        		cur.sendQuery("select * from my_table");
        		con.endSession();

        		for (UInt32 i=0; i&lt;cur.colCount(); i++)
			{
                		Console.Write("Name:          ");
				Console.WriteLine(cur.getColumnName(i));
                		Console.Write("\n");
        		}
		}
	}
}
