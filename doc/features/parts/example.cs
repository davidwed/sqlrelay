using System;
using SQLRClient;
using System.IO;

namespace csexample
{
	class example
	{
		public static void Main()
		{
			SQLRConnection sqlrcon = new SQLRConnection(
							"examplehost", 9000,
							"/tmp/example.socket",
							"exampleuser",
							"examplepassword",
							0, 1);
			SQLRCursor sqlrcur = new SQLRCursor(sqlrcon);

			sqlrcur.sendQuery("select * from exampletable");
			for (UInt64 row=0; row<sqlrcur.rowCount(); row++) {
				for (UInt64 col=0; col<sqlrcur.colCount(); col++) {
					Console.WriteLine(sqlrcur.getField(row,col)+",");
				}
				Console.WriteLine();
			}
		}
	}
}
