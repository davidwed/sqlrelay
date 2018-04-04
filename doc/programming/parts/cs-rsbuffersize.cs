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

			cur.setResultSetBufferSize(5);

        		cur.sendQuery("select * from my_table");

        		Boolean done = false;
        		UInt64 row = 0;
        		String field;
        		while (!done)
			{
                		for (UInt32 col=0; col&lt;cur.colCount(); col++)
				{
                        		field = cur.getField(row, col);
                        		if (field != null)
					{
						Console.Write(field);
						Console.Write(",");
                        		}
					else
					{
                                		done = true;
                        		}
                		}
                		Console.Write("\n");
                		row++;
        		}

        		cur.sendQuery("select * from my_other_table");

        		... process this query's result set in chunks also ...

        		cur.setResultSetBufferSize(0);

        		cur.sendQuery("select * from my_third_table");

        		... process this query's result set all at once ...

        		con.endSession();
		}
	}
}
