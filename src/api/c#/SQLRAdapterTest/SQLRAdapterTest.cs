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
                Console.Out.Flush();
            }
            else
            {
                Console.WriteLine("failure");
                Console.Out.Flush();
                Environment.Exit(1);
            }
        }

        private static void checkSuccess(Int64 value, Int64 success)
        {
            if (value == success)
            {
                Console.Write("success ");
                Console.Out.Flush();
            }
            else
            {
                Console.WriteLine("failure");
                Console.Out.Flush();
                Environment.Exit(1);
            }
        }

        private static void checkSuccess(bool value, bool success)
        {
            if (value == success)
            {
                Console.Write("success ");
                Console.Out.Flush();
            }
            else
            {
                Console.WriteLine("failure");
                Console.Out.Flush();
                Environment.Exit(1);
            }
        }

        private static Int64 ExecuteScalar(SQLRelayCommand cmd)
        {
            try
            {
                return Convert.ToInt64(cmd.ExecuteScalar());
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.Out.Flush();
                Environment.Exit(1);
                return 0;
            }
        }

        private static Int64 ExecuteNonQuery(SQLRelayCommand cmd)
        {
            try
            {
                return Convert.ToInt64(cmd.ExecuteNonQuery());
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.Out.Flush();
                Environment.Exit(1);
                return 0;
            }
        }

        private static SQLRelayDataReader ExecuteReader(SQLRelayCommand cmd)
        {
            try
            {
                return (SQLRelayDataReader)cmd.ExecuteReader();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.Out.Flush();
                Environment.Exit(1);
                return null;
            }
        }


        public static void Main(string[] args)
        {

            // open connection and command
            SQLRelayConnection sqlrcon = new SQLRelayConnection("host=fedora;port=9000;socket=;user=test;password=test;retrytime=0;tries=1;debug=false");
            sqlrcon.Open();
            SQLRelayCommand sqlrcom = null;

            // execute scalar
            Console.WriteLine("EXECUTE SCALAR:");
            sqlrcom = new SQLRelayCommand("select 1 from dual", sqlrcon);
            Int64 value = ExecuteScalar(sqlrcom);
            checkSuccess(value, 1);
            Console.WriteLine("\n");

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
            Console.WriteLine("\n");

            // create the table
            Console.WriteLine("CREATE TABLE:");
            sqlrcom.CommandText = "create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long, testclob clob, testblob blob)";
            ExecuteNonQuery(sqlrcom);
            Console.WriteLine("\n");

            // insert
            Console.WriteLine("INSERT:");
            sqlrcom.CommandText = "insert into testtable values (1, 'testchar1', 'testvarchar1', '01-JAN-2001', 'testlong1', 'testclob1', empty_blob())";
            Int64 affectedrows = ExecuteNonQuery(sqlrcom);
            Console.WriteLine("\n");

            // affected rows
            Console.WriteLine("AFFECTED ROWS:");
            checkSuccess(affectedrows, 1);
            Console.WriteLine("\n");

            // bind by position
            Console.WriteLine("BIND BY POSITION:");
            //sqlrcom.CommandText = "insert into testtable values (:var1, :var2, :var3, :var4, :var5, :var6, :var7)";
            sqlrcom.CommandText = "insert into testtable values (:var1, :var2, :var3, :var4, :var5, null, null)";
            sqlrcom.Parameters.Add("1", 2);
            sqlrcom.Parameters.Add("2", "testchar2");
            sqlrcom.Parameters.Add("3", "testvarchar2");
            sqlrcom.Parameters.Add("4", "01-JAN-2002");
            sqlrcom.Parameters.Add("5", "testlong2");
            //sqlrcom.Parameters.Add("6", "testclob2");
            //sqlrcom.Parameters.Add("7", "testblob2");
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            sqlrcom.Parameters.Add("1", 3);
            sqlrcom.Parameters.Add("2", "testchar3");
            sqlrcom.Parameters.Add("3", "testvarchar3");
            sqlrcom.Parameters.Add("4", "01-JAN-2003");
            sqlrcom.Parameters.Add("5", "testlong3");
            //sqlrcom.Parameters.Add("6", "testclob3");
            //sqlrcom.Parameters.Add("7", "testblob3");
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            Console.WriteLine("\n");

            // bind by name
            sqlrcom.Parameters.Add("var1", 4);
            sqlrcom.Parameters.Add("var2", "testchar4");
            sqlrcom.Parameters.Add("var3", "testvarchar4");
            sqlrcom.Parameters.Add("var4", "01-JAN-2004");
            sqlrcom.Parameters.Add("var5", "testlong4");
            //sqlrcom.Parameters.Add("var6", "testclob4");
            //sqlrcom.Parameters.Add("var7", "testblob4");
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            sqlrcom.Parameters.Add("var1", 5);
            sqlrcom.Parameters.Add("var2", "testchar5");
            sqlrcom.Parameters.Add("var3", "testvarchar5");
            sqlrcom.Parameters.Add("var4", "01-JAN-2005");
            sqlrcom.Parameters.Add("var5", "testlong5");
            //sqlrcom.Parameters.Add("var6", "testclob5");
            //sqlrcom.Parameters.Add("var7", "testblob5");
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            Console.WriteLine("\n");

            // output bind by name

            // output bind by position

            // select
            Console.WriteLine("SELECT:");
            sqlrcom.CommandText = "select * from testtable order by testnumber";
            System.Data.IDataReader datareader = ExecuteReader(sqlrcom);
            checkSuccess(datareader != null, true);
            Console.WriteLine("\n");

            // column count
            Console.WriteLine("COLUMN COUNT:");
            checkSuccess(datareader.FieldCount, 7);
            Console.WriteLine("\n");

            // column names
            Console.WriteLine("COLUMN NAMES:");
            checkSuccess(datareader.GetName(0), "TESTNUMBER");
            checkSuccess(datareader.GetName(1), "TESTCHAR");
            checkSuccess(datareader.GetName(2), "TESTVARCHAR");
            checkSuccess(datareader.GetName(3), "TESTDATE");
            checkSuccess(datareader.GetName(4), "TESTLONG");
            checkSuccess(datareader.GetName(5), "TESTCLOB");
            checkSuccess(datareader.GetName(6), "TESTBLOB");
            Console.WriteLine("\n");

            // column types
            Console.WriteLine("COLUMN TYPES:");
            checkSuccess(datareader.GetDataTypeName(0), "NUMBER");
            //Console.WriteLine("type: " + datareader.GetFieldType(0));
            checkSuccess(datareader.GetDataTypeName(1), "CHAR");
            checkSuccess(datareader.GetDataTypeName(2), "VARCHAR2");
            checkSuccess(datareader.GetDataTypeName(3), "DATE");
            checkSuccess(datareader.GetDataTypeName(4), "LONG");
            checkSuccess(datareader.GetDataTypeName(5), "CLOB");
            checkSuccess(datareader.GetDataTypeName(6), "BLOB");
            Console.WriteLine("\n");

            // column length

            // fields by index
            Console.WriteLine("FIELDS BY INDEX:");
            datareader.Read();
            //checkSuccess(datareader.GetData(0).GetType() == typeof(Int64), true);
            checkSuccess(datareader.GetInt16(0), 1);
            checkSuccess(datareader.GetInt32(0), 1);
            checkSuccess(datareader.GetInt64(0), 1);
            checkSuccess(Convert.ToInt64(datareader[0]), 1);
            checkSuccess(datareader.GetString(1), "testchar1                               ");
            checkSuccess(Convert.ToString(datareader[1]), "testchar1                               ");
            checkSuccess(datareader.GetString(2), "testvarchar1");
            checkSuccess(Convert.ToString(datareader[2]), "testvarchar1");
            checkSuccess(datareader.GetString(3), "01-JAN-01");
            //checkSuccess(Convert.ToString(datareader[3]), "01-JAN-01");
            checkSuccess(datareader.GetString(4), "testlong1");
            checkSuccess(System.Text.Encoding.Default.GetString((byte[])datareader[4]), "testlong1");
            //checkSuccess(datareader.GetString(5), "testclob1");
            //checkSuccess(datareader.GetString(6), "testblob1");
            Console.WriteLine("\n");

            // fields by name
            Console.WriteLine("FIELDS BY NAME:");
            checkSuccess(Convert.ToInt64(datareader["TESTNUMBER"]), 1);
            checkSuccess(Convert.ToString(datareader["TESTCHAR"]), "testchar1                               ");
            checkSuccess(Convert.ToString(datareader["TESTVARCHAR"]), "testvarchar1");
            //checkSuccess(Convert.ToString(datareader["TESTDATE"]), "01-JAN-01");
            checkSuccess(System.Text.Encoding.Default.GetString((byte[])datareader["TESTLONG"]), "testlong1");
            //checkSuccess(System.Text.Encoding.Default.GetString((byte[])datareader["TESTCLOB"]), "testclob1");
            //checkSuccess(System.Text.Encoding.Default.GetString((byte[])datareader["TESTBLOB"]), "testblob1");

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