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

        		for (UInt32 i=0; i&lt;cur.colCount(); i++)
			{
                		Console.Write("Name:          ");
				Console.WriteLine(cur.getColumnName(i));
                		Console.Write("Type:          ");
				Console.WriteLine(cur.getColumnType(i));
                		Console.Write("Length:        ");
				Console.WriteLine(cur.getColumnLength(i));
                		Console.Write("Precision:     ");
				Console.WriteLine(cur.getColumnPrecision(i));
                		Console.Write("Scale:         ");
				Console.WriteLine(cur.getColumnScale(i));
                		Console.Write("Longest Field: ");
				Console.WriteLine(cur.getLongest(i));
                		Console.Write("Nullable:      ");
				Console.WriteLine(cur.getColumnIsNullable(i));
                		Console.Write("Primary Key:   ");
				Console.WriteLine(cur.getColumnIsPrimaryKey(i));
                		Console.Write("Unique:        ");
				Console.WriteLine(cur.getColumnIsUnique(i));
                		Console.Write("Part of Key:   ");
				Console.WriteLine(cur.getColumnIsPartOfKey(i));
                		Console.Write("Unsigned:      ");
				Console.WriteLine(cur.getColumnIsUnsigned(i));
                		Console.Write("Zero Filled:   ");
				Console.WriteLine(cur.getColumnIsZeroFilled(i));
                		Console.Write("Binary:        ");
				Console.WriteLine(cur.getColumnIsBinary(i));
                		Console.Write("Auto Increment:");
				Console.WriteLine(cur.getColumnIsAutoIncrement(i));
                		Console.Write("\n");
        		}
		}
	}
}
