using System;
using SQLRClient;

namespace SQLRClientTest
{
    class SQLRTest
    {
        public static void Main()
        {
            SQLRConnection sqlrcon = new SQLRConnection("localhost", 9000, "", "test", "test", 0, 1);
            SQLRCursor sqlrcur = new SQLRCursor(sqlrcon);

            sqlrcon.debugOn();
            sqlrcur.sendQuery("select 1 from dual");
        }
    }
}
