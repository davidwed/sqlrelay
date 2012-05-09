using System;
using SQLRClient;

namespace SQLRClientTest
{
    class SQLRAdapterTest
    {

        private static void checkSuccess(string value, string success)
        {
            if (value == success)
            {
                Console.Write("success ");
            }
            else
            {
                Console.WriteLine("failure");
                Environment.Exit(1);
            }
        }

        private static void checkSuccess(Int64 value, Int64 success)
        {
            if (value == success)
            {
                Console.Write("success ");
            }
            else
            {
                Console.WriteLine("failure");
                Environment.Exit(1);
            }
        }

        private static void checkSuccess(bool value, bool success)
        {
            if (value == success)
            {
                Console.Write("success ");
            }
            else
            {
                Console.WriteLine("failure");
                Environment.Exit(1);
            }
        }


        public static void Main(string[] args)
        {

            // open connection and command
            SQLRelayConnection sqlrcon = new SQLRelayConnection("host=fedora;port=9000;socket=;user=test;password=test;retrytime=0;tries=1;debug=true");
            sqlrcon.Open();
            SQLRelayCommand sqlrcom = null;

            // execute scalar
            Console.WriteLine("EXECUTE SCALAR:");
            sqlrcom = new SQLRelayCommand("select 1 from dual", sqlrcon);
            Int64 value = Convert.ToInt64(sqlrcom.ExecuteScalar());
            checkSuccess(value, 1);
            Console.WriteLine("");

            // drop the table
            Console.WriteLine("DROP TABLE:");
            try
            {
                sqlrcom = new SQLRelayCommand("drop table testtable", sqlrcon);
                sqlrcom.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                // don't do anything, it's ok if this fails
            }
            Console.WriteLine("");

            // create the table
            Console.WriteLine("CREATE TABLE:");
            sqlrcom.CommandText = "create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long, testclob clob, testblob blob)";
            sqlrcom.ExecuteNonQuery();
            Console.WriteLine("");

            // insert
            Console.WriteLine("INSERT:");
            sqlrcom.CommandText = "insert into testtable values (1, 'testchar1', 'testvarchar1', '01-JAN-2001', 'testlong1', 'testclob1', empty_blob())";
            int affectedrows = sqlrcom.ExecuteNonQuery();
            Console.WriteLine("");

            // affected rows
            Console.WriteLine("AFFECTED ROWS:");
            checkSuccess((Int64)affectedrows, 1);
            Console.WriteLine("");

            // bind by position
            Console.WriteLine("BIND BY POSITION:");
            sqlrcom.CommandText = "insert into testtable values (:var1, :var2, :var3, :var4, :var5, :var6, :var7)";
            sqlrcom.Parameters.Add("1", 2);
            sqlrcom.Parameters.Add("2", "testchar2");
            sqlrcom.Parameters.Add("3", "testvarchar2");
            sqlrcom.Parameters.Add("4", "01-JAN-2002");
            sqlrcom.Parameters.Add("5", "testclob2");
            sqlrcom.Parameters.Add("6", "testblob2");
            checkSuccess((Int64)sqlrcom.ExecuteNonQuery(), 1);
            sqlrcom.Parameters.Clear();
            sqlrcom.Parameters.Add("1", 3);
            sqlrcom.Parameters.Add("2", "testchar3");
            sqlrcom.Parameters.Add("3", "testvarchar3");
            sqlrcom.Parameters.Add("4", "01-JAN-2003");
            sqlrcom.Parameters.Add("5", "testclob3");
            sqlrcom.Parameters.Add("6", "testblob3");
            checkSuccess((Int64)sqlrcom.ExecuteNonQuery(), 1);
            Console.WriteLine("");

            // bind by name
            sqlrcom.Parameters.Add("var1", 4);
            sqlrcom.Parameters.Add("var2", "testchar4");
            sqlrcom.Parameters.Add("var3", "testvarchar4");
            sqlrcom.Parameters.Add("var4", "01-JAN-2004");
            sqlrcom.Parameters.Add("var5", "testclob4");
            sqlrcom.Parameters.Add("var6", "testblob4");
            checkSuccess((Int64)sqlrcom.ExecuteNonQuery(), 1);
            sqlrcom.Parameters.Clear();
            sqlrcom.Parameters.Add("var1", 5);
            sqlrcom.Parameters.Add("var2", "testchar5");
            sqlrcom.Parameters.Add("var3", "testvarchar5");
            sqlrcom.Parameters.Add("var4", "01-JAN-2005");
            sqlrcom.Parameters.Add("var5", "testclob5");
            sqlrcom.Parameters.Add("var6", "testblob5");
            checkSuccess((Int64)sqlrcom.ExecuteNonQuery(), 1);
            Console.WriteLine("");

            // output bind by name

            // output bind by position

            // select
            Console.WriteLine("SELECT:");
            sqlrcom.CommandText = "select * from testtable order by testnumber";
            System.Data.IDataReader datareader = sqlrcom.ExecuteReader();
            checkSuccess(datareader != null, true);

            // column count
            Console.WriteLine("COLUMN COUNT:");
            checkSuccess(datareader.FieldCount, 6);
            Console.WriteLine("");

            // column names
            Console.WriteLine("COLUMN NAMES:");
            checkSuccess(datareader.GetName(0), "testnumber");
            checkSuccess(datareader.GetName(1), "testchar");
            checkSuccess(datareader.GetName(2), "testvarchar");
            checkSuccess(datareader.GetName(3), "testdate");
            checkSuccess(datareader.GetName(4), "testclob");
            checkSuccess(datareader.GetName(5), "testblob");
            Console.WriteLine("");

            // column types

            // column length

            // fields by index

            // field lengths by index

            // fields by name

            // field lengths by name

            // fields by array

            // commit and rollback

            // clob and blob output bind

            // null and empty clobs and blobs

            // long clob

            // long output bind

            // negative input bind

            // drop table

            // stored procedure

            // rebinding

            // invalid queries

            sqlrcon.Close();
        }
    }
}