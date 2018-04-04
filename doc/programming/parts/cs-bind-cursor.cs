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

			cur.prepareQuery("begin  :curs:=sp_mytable; end;");
        		cur.defineOutputBindCursor("curs");
        		cur.executeQuery();

        		SQLRCursor bindcur = cur.getOutputBindCursor("curs");
        		bindcur.fetchFromBindCursor();

        		// print fields from table
        		for (int i=0; i&lt;bindcur.rowCount(); i++)
			{
                		for (int j=0; j&lt;bindcur.colCount(); j++)
				{
                        		Console.Write(bindcur.getField(i, j));
					Console.Write(", ");
                		}
                		Console.Write("\n");
        		}
		}
	}
}
