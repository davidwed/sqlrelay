// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

using System;
using SQLRClient;
using System.Data;
using System.IO;
using System.Drawing;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace SQLRClientTest
{
    class SQLRAdapterTest
    {
        private static void checkSuccess(Object value, Object success)
        {
            if (value == success)
            {
                Console.Write("success ");
                Console.Out.Flush();
            }
            else
            {
                Console.WriteLine("failure");
                Console.WriteLine("\"" + value + "\" != \"" + success + "\"");
                Console.Out.Flush();
                Environment.Exit(1);
            }
        }

        private static void checkSuccess(String value, String success)
        {
            if (value == success)
            {
                Console.Write("success ");
                Console.Out.Flush();
            }
            else
            {
                Console.WriteLine("failure");
                Console.WriteLine("\"" + value + "\" != \"" + success + "\"");
                Console.Out.Flush();
                Environment.Exit(1);
            }
        }

        private static void checkSuccess(String value, String success, Int32 size)
        {
            if (value.Substring(0,size) == success.Substring(0,size))
            {
                Console.Write("success ");
                Console.Out.Flush();
            }
            else
            {
                Console.WriteLine("failure");
                Console.WriteLine("\"" + value + "\" != \"" + success + "\"");
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
                Console.WriteLine("\"" + value + "\" != \"" + success + "\"");
                Console.Out.Flush();
                Environment.Exit(1);
            }
        }

        private static void checkSuccess(Boolean  value, Boolean  success)
        {
            if (value == success)
            {
                Console.Write("success ");
                Console.Out.Flush();
            }
            else
            {
                Console.WriteLine("failure");
                Console.WriteLine("\"" + value + "\" != \"" + success + "\"");
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
                return -1;
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
                return -1;
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
                return null;
            }
        }


        public static void Main(String[] args)
        {
            SQLRelayConnection sqlrcon = new SQLRelayConnection("Data Source=sqlrelay:9000:/tmp/test.socket;User ID=test;Password=test;Retry Time=0;Tries=1;Debug=false");
            sqlrcon.Open();
            SQLRelayCommand sqlrcom = (SQLRelayCommand)sqlrcon.CreateCommand();
            Console.WriteLine("UNICODE:");
            sqlrcom.CommandText = "select unistr('abc\\00e5\\00f1\\00f6') from dual";
            System.Data.IDataReader datareader = ExecuteReader(sqlrcom);
            checkSuccess(datareader != null, true);
            checkSuccess(datareader.Read(), true);
            checkSuccess(datareader.GetString(0), "abcåñö");
            Console.WriteLine("\n");
            sqlrcon.Close();
        }
    }
}
