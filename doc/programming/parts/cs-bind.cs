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

        		cur.prepareQuery("select * from mytable $(whereclause)")
        		cur.substitution("whereclause", "where stringcol=:stringval and integercol&gt;:integerval and floatcol&gt;floatval");
        		cur.inputBind("stringval", "true");
        		cur.inputBind("integerval", 10);
        		cur.inputBind("floatval", 1.1, 2, 1);
        		cur.executeQuery();

        		... process the result set ...
		}
	}
}
