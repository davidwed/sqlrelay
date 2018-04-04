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

			cur.prepareQuery("begin  select image into :image from images;  select description into :desc from images;  end;");
        		cur.defineOutputBindBlob("image");
        		cur.defineOutputBindClob("desc");
        		cur.executeQuery();

        		String image = cur.getOutputBindBlob("image");
        		UInt32 imagelength = cur.getOutputBindLength("image");

        		String desc = cur.getOutputBindClob("desc");
        		UInt32 desclength = cur.getOutputBindLength("desc");

        		con.endSession();

        		... do something with image and desc ...
		}
	}
}
