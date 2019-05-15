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

        private static String tlscert = "";
        private static String tlsca = "";

        public static void Main(String[] args)
        {

            tlscert = "/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem";
            tlsca = "/usr/local/firstworks/etc/sqlrelay.conf.d/ca.pem";
            if (Environment.OSVersion.ToString().Contains("Windows"))
            {
                tlscert = "C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\client.pfx";
                tlsca = "C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\ca.pfx";
            }

            // open connection and command
            SQLRelayConnection sqlrcon = new SQLRelayConnection("Data Source=sqlrelay:9000:/tmp/test.socket;Retry Time=0;Tries=1;Tls=yes;Tlscert=" + tlscert + ";Tlsvalidate=ca;Tlsca=" + tlsca + ";Debug=false");
            sqlrcon.Open();


            // execute scalar
            SQLRelayCommand sqlrcom = (SQLRelayCommand)sqlrcon.CreateCommand();
            Console.WriteLine("EXECUTE SCALAR:");
            sqlrcom.CommandText = "select 1 from dual";
            Int64 value = ExecuteScalar(sqlrcom);
            checkSuccess(value, 1);
            Console.WriteLine("\n");

            // drop the table
            Console.WriteLine("DROP TABLE:");
            sqlrcom = new SQLRelayCommand("drop table testtable");
            sqlrcom.Connection = sqlrcon;
            ExecuteNonQuery(sqlrcom);
            Console.WriteLine("\n");

            // create the table
            Console.WriteLine("CREATE TABLE:");
            sqlrcom = new SQLRelayCommand("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long, testclob clob, testblob blob)", sqlrcon);
            ExecuteNonQuery(sqlrcom);
            Console.WriteLine("\n");

            // insert
            Console.WriteLine("INSERT:");
            sqlrcom.CommandText = "insert into testtable values (1, 'testchar1', 'testvarchar1', '01-JAN-2001', 'testlong1', 'testclob1', empty_blob())";
            sqlrcom.Prepare();
            Int64 affectedrows = ExecuteNonQuery(sqlrcom);
            Console.WriteLine("\n");

            // affected rows
            Console.WriteLine("AFFECTED ROWS:");
            checkSuccess(affectedrows, 1);
            Console.WriteLine("\n");

            // bind by position
            Console.WriteLine("BIND BY POSITION:");
            sqlrcom.CommandText = "insert into testtable values (:var1, :var2, :var3, :var4, :var5, :var6, :var7)";
            sqlrcom.Parameters.Add("1", 2);
            sqlrcom.Parameters.Add("2", "testchar2");
            sqlrcom.Parameters.Add("3", "testvarchar2");
            sqlrcom.Parameters.Add("4", new DateTime(2001,1,1,0,0,0,0));
            sqlrcom.Parameters.Add("5", "testlong2");
            SQLRelayParameter var6 = new SQLRelayParameter();
            var6.ParameterName = "6";
            var6.Value = "testclob2";
            var6.SQLRelayType = SQLRelayType.Clob;
            sqlrcom.Parameters.Add(var6);
            SQLRelayParameter var7 = new SQLRelayParameter();
            var7.ParameterName = "7";
            var7.Value = System.Text.Encoding.Default.GetBytes("testblob2");
            var7.SQLRelayType = SQLRelayType.Blob;
            sqlrcom.Parameters.Add(var7);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            sqlrcom.Parameters.Add("1", 3);
            sqlrcom.Parameters.Add("2", "testchar3");
            sqlrcom.Parameters.Add("3", "testvarchar3");
            sqlrcom.Parameters.Add("4", new DateTime(2003,1,1,0,0,0,0));
            sqlrcom.Parameters.Add("5", "testlong3");
            var6.Value = "testclob3";
            sqlrcom.Parameters.Add(var6);
            var7.Value = System.Text.Encoding.Default.GetBytes("testblob3");
            var7.SQLRelayType = SQLRelayType.Object;
            var7.DbType = DbType.Binary;
            sqlrcom.Parameters.Add(var7);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            Console.WriteLine("\n");

            // bind by name
            sqlrcom.Parameters.Add("var1", 4);
            sqlrcom.Parameters.Add("var2", "testchar4");
            sqlrcom.Parameters.Add("var3", "testvarchar4");
            sqlrcom.Parameters.Add("var4", new DateTime(2004,1,1,0,0,0,0));
            sqlrcom.Parameters.Add("var5", "testlong4");
            var6.Value = "testclob4";
            sqlrcom.Parameters.Add(var6);
            var7.Value = System.Text.Encoding.Default.GetBytes("testblob4");
            sqlrcom.Parameters.Add(var7);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            sqlrcom.Parameters.Add("var1", 5);
            sqlrcom.Parameters.Add("var2", "testchar5");
            sqlrcom.Parameters.Add("var3", "testvarchar5");
            sqlrcom.Parameters.Add("var4", new DateTime(2005,1,1,0,0,0,0));
            sqlrcom.Parameters.Add("var5", "testlong5");
            var6.Value = "testclob5";
            sqlrcom.Parameters.Add(var6);
            var7.Value = System.Text.Encoding.Default.GetBytes("testblob5");
            sqlrcom.Parameters.Add(var7);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            Console.WriteLine("\n");

            // null and empty binds
            Console.WriteLine("NULL BINDS:");
            sqlrcom.Parameters.Add("1", null);
            sqlrcom.Parameters.Add("2", null);
            sqlrcom.Parameters.Add("3", null);
            sqlrcom.Parameters.Add("4", null);
            sqlrcom.Parameters.Add("5", null);
            sqlrcom.Parameters.Add("6", null);
            sqlrcom.Parameters.Add("7", null);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            Console.WriteLine("\n");

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
            checkSuccess(datareader.GetFieldType(0).ToString(), "System.Int64");
            checkSuccess(datareader.GetDataTypeName(1), "CHAR");
            checkSuccess(datareader.GetFieldType(1).ToString(), "System.String");
            checkSuccess(datareader.GetDataTypeName(2), "VARCHAR2");
            checkSuccess(datareader.GetFieldType(2).ToString(), "System.String");
            checkSuccess(datareader.GetDataTypeName(3), "DATE");
            checkSuccess(datareader.GetFieldType(3).ToString(), "System.DateTime");
            checkSuccess(datareader.GetDataTypeName(4), "LONG");
            checkSuccess(datareader.GetFieldType(4).ToString(), "System.Byte[]");
            checkSuccess(datareader.GetDataTypeName(5), "CLOB");
            checkSuccess(datareader.GetFieldType(5).ToString(), "System.String");
            checkSuccess(datareader.GetDataTypeName(6), "BLOB");
            checkSuccess(datareader.GetFieldType(6).ToString(), "System.Byte[]");
            Console.WriteLine("\n");

            // schema table
            Console.WriteLine("SCHEMA TABLE:");
            DataTable schematable = datareader.GetSchemaTable();
            checkSuccess(Convert.ToString(schematable.Rows[0]["ColumnName"]), "TESTNUMBER");
            checkSuccess(Convert.ToInt64(schematable.Rows[0]["ColumnOrdinal"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[0]["ColumnSize"]), 22);
            checkSuccess(Convert.ToInt64(schematable.Rows[0]["NumericPrecision"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[0]["NumericScale"]), 129);
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsUnique"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsKey"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[0]["BaseServerName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[0]["BaseCatalogName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[0]["BaseColumnName"]), "TESTNUMBER");
            checkSuccess(Convert.ToString(schematable.Rows[0]["BaseSchemaName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[0]["BaseTableName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[0]["DataType"]), "System.Int64");
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["AllowDBNull"]), true);
            checkSuccess(Convert.ToString(schematable.Rows[0]["ProviderType"]), "NUMBER");
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsAliased"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsExpression"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsIdentity"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsAutoIncrement"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsRowVersion"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsHidden"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsLong"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[0]["IsReadOnly"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[0]["ProviderSpecificDataType"]), "NUMBER");
            checkSuccess(Convert.ToString(schematable.Rows[0]["DataTypeName"]), "NUMBER");
            checkSuccess(Convert.ToString(schematable.Rows[0]["XmlSchemaCollectionDatabase"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[0]["XmlSchemaCollectionOwningSchema"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[0]["XmlSchemaCollectionName"]), "");

            checkSuccess(Convert.ToString(schematable.Rows[1]["ColumnName"]), "TESTCHAR");
            checkSuccess(Convert.ToInt64(schematable.Rows[1]["ColumnOrdinal"]), 1);
            checkSuccess(Convert.ToInt64(schematable.Rows[1]["ColumnSize"]), 40);
            checkSuccess(Convert.ToInt64(schematable.Rows[1]["NumericPrecision"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[1]["NumericScale"]), 0);
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsUnique"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsKey"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[1]["BaseServerName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[1]["BaseCatalogName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[1]["BaseColumnName"]), "TESTCHAR");
            checkSuccess(Convert.ToString(schematable.Rows[1]["BaseSchemaName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[1]["BaseTableName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[1]["DataType"]), "System.String");
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["AllowDBNull"]), true);
            checkSuccess(Convert.ToString(schematable.Rows[1]["ProviderType"]), "CHAR");
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsAliased"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsExpression"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsIdentity"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsAutoIncrement"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsRowVersion"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsHidden"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsLong"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[1]["IsReadOnly"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[1]["ProviderSpecificDataType"]), "CHAR");
            checkSuccess(Convert.ToString(schematable.Rows[1]["DataTypeName"]), "CHAR");
            checkSuccess(Convert.ToString(schematable.Rows[1]["XmlSchemaCollectionDatabase"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[1]["XmlSchemaCollectionOwningSchema"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[1]["XmlSchemaCollectionName"]), "");

            checkSuccess(Convert.ToString(schematable.Rows[2]["ColumnName"]), "TESTVARCHAR");
            checkSuccess(Convert.ToInt64(schematable.Rows[2]["ColumnOrdinal"]), 2);
            checkSuccess(Convert.ToInt64(schematable.Rows[2]["ColumnSize"]), 40);
            checkSuccess(Convert.ToInt64(schematable.Rows[2]["NumericPrecision"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[2]["NumericScale"]), 0);
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsUnique"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsKey"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[2]["BaseServerName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[2]["BaseCatalogName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[2]["BaseColumnName"]), "TESTVARCHAR");
            checkSuccess(Convert.ToString(schematable.Rows[2]["BaseSchemaName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[2]["BaseTableName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[2]["DataType"]), "System.String");
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["AllowDBNull"]), true);
            checkSuccess(Convert.ToString(schematable.Rows[2]["ProviderType"]), "VARCHAR2");
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsAliased"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsExpression"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsIdentity"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsAutoIncrement"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsRowVersion"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsHidden"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsLong"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[2]["IsReadOnly"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[2]["ProviderSpecificDataType"]), "VARCHAR2");
            checkSuccess(Convert.ToString(schematable.Rows[2]["DataTypeName"]), "VARCHAR2");
            checkSuccess(Convert.ToString(schematable.Rows[2]["XmlSchemaCollectionDatabase"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[2]["XmlSchemaCollectionOwningSchema"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[2]["XmlSchemaCollectionName"]), "");

            checkSuccess(Convert.ToString(schematable.Rows[3]["ColumnName"]), "TESTDATE");
            checkSuccess(Convert.ToInt64(schematable.Rows[3]["ColumnOrdinal"]), 3);
            checkSuccess(Convert.ToInt64(schematable.Rows[3]["ColumnSize"]), 7);
            checkSuccess(Convert.ToInt64(schematable.Rows[3]["NumericPrecision"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[3]["NumericScale"]), 0);
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsUnique"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsKey"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[3]["BaseServerName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[3]["BaseCatalogName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[3]["BaseColumnName"]), "TESTDATE");
            checkSuccess(Convert.ToString(schematable.Rows[3]["BaseSchemaName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[3]["BaseTableName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[3]["DataType"]), "System.DateTime");
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["AllowDBNull"]), true);
            checkSuccess(Convert.ToString(schematable.Rows[3]["ProviderType"]), "DATE");
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsAliased"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsExpression"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsIdentity"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsAutoIncrement"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsRowVersion"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsHidden"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsLong"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[3]["IsReadOnly"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[3]["ProviderSpecificDataType"]), "DATE");
            checkSuccess(Convert.ToString(schematable.Rows[3]["DataTypeName"]), "DATE");
            checkSuccess(Convert.ToString(schematable.Rows[3]["XmlSchemaCollectionDatabase"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[3]["XmlSchemaCollectionOwningSchema"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[3]["XmlSchemaCollectionName"]), "");

            checkSuccess(Convert.ToString(schematable.Rows[4]["ColumnName"]), "TESTLONG");
            checkSuccess(Convert.ToInt64(schematable.Rows[4]["ColumnOrdinal"]), 4);
            checkSuccess(Convert.ToInt64(schematable.Rows[4]["ColumnSize"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[4]["NumericPrecision"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[4]["NumericScale"]), 0);
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsUnique"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsKey"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[4]["BaseServerName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[4]["BaseCatalogName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[4]["BaseColumnName"]), "TESTLONG");
            checkSuccess(Convert.ToString(schematable.Rows[4]["BaseSchemaName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[4]["BaseTableName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[4]["DataType"]), "System.Byte[]");
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["AllowDBNull"]), true);
            checkSuccess(Convert.ToString(schematable.Rows[4]["ProviderType"]), "LONG");
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsAliased"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsExpression"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsIdentity"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsAutoIncrement"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsRowVersion"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsHidden"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsLong"]), true);
            checkSuccess(Convert.ToBoolean(schematable.Rows[4]["IsReadOnly"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[4]["ProviderSpecificDataType"]), "LONG");
            checkSuccess(Convert.ToString(schematable.Rows[4]["DataTypeName"]), "LONG");
            checkSuccess(Convert.ToString(schematable.Rows[4]["XmlSchemaCollectionDatabase"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[4]["XmlSchemaCollectionOwningSchema"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[4]["XmlSchemaCollectionName"]), "");

            checkSuccess(Convert.ToString(schematable.Rows[5]["ColumnName"]), "TESTCLOB");
            checkSuccess(Convert.ToInt64(schematable.Rows[5]["ColumnOrdinal"]), 5);
            checkSuccess(Convert.ToInt64(schematable.Rows[5]["ColumnSize"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[5]["NumericPrecision"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[5]["NumericScale"]), 0);
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsUnique"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsKey"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[5]["BaseServerName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[5]["BaseCatalogName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[5]["BaseColumnName"]), "TESTCLOB");
            checkSuccess(Convert.ToString(schematable.Rows[5]["BaseSchemaName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[5]["BaseTableName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[5]["DataType"]), "System.String");
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["AllowDBNull"]), true);
            checkSuccess(Convert.ToString(schematable.Rows[5]["ProviderType"]), "CLOB");
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsAliased"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsExpression"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsIdentity"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsAutoIncrement"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsRowVersion"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsHidden"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsLong"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[5]["IsReadOnly"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[5]["ProviderSpecificDataType"]), "CLOB");
            checkSuccess(Convert.ToString(schematable.Rows[5]["DataTypeName"]), "CLOB");
            checkSuccess(Convert.ToString(schematable.Rows[5]["XmlSchemaCollectionDatabase"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[5]["XmlSchemaCollectionOwningSchema"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[5]["XmlSchemaCollectionName"]), "");

            checkSuccess(Convert.ToString(schematable.Rows[6]["ColumnName"]), "TESTBLOB");
            checkSuccess(Convert.ToInt64(schematable.Rows[6]["ColumnOrdinal"]), 6);
            checkSuccess(Convert.ToInt64(schematable.Rows[6]["ColumnSize"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[6]["NumericPrecision"]), 0);
            checkSuccess(Convert.ToInt64(schematable.Rows[6]["NumericScale"]), 0);
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsUnique"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsKey"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[6]["BaseServerName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[6]["BaseCatalogName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[6]["BaseColumnName"]), "TESTBLOB");
            checkSuccess(Convert.ToString(schematable.Rows[6]["BaseSchemaName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[6]["BaseTableName"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[6]["DataType"]), "System.Byte[]");
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["AllowDBNull"]), true);
            checkSuccess(Convert.ToString(schematable.Rows[6]["ProviderType"]), "BLOB");
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsAliased"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsExpression"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsIdentity"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsAutoIncrement"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsRowVersion"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsHidden"]), false);
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsLong"]), true);
            checkSuccess(Convert.ToBoolean(schematable.Rows[6]["IsReadOnly"]), false);
            checkSuccess(Convert.ToString(schematable.Rows[6]["ProviderSpecificDataType"]), "BLOB");
            checkSuccess(Convert.ToString(schematable.Rows[6]["DataTypeName"]), "BLOB");
            checkSuccess(Convert.ToString(schematable.Rows[6]["XmlSchemaCollectionDatabase"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[6]["XmlSchemaCollectionOwningSchema"]), "");
            checkSuccess(Convert.ToString(schematable.Rows[6]["XmlSchemaCollectionName"]), "");

            Console.WriteLine("\n");

            // fields by index
            Console.WriteLine("FIELDS BY INDEX:");
            checkSuccess(datareader.Read(), true);
            checkSuccess(datareader.GetInt16(0), 1);
            checkSuccess(datareader.GetInt32(0), 1);
            checkSuccess(datareader.GetInt64(0), 1);
            checkSuccess(Convert.ToInt64(datareader[0]), 1);
            checkSuccess(datareader.GetString(1), "testchar1                               ");
            checkSuccess(Convert.ToString(datareader[1]), "testchar1                               ");
            checkSuccess(datareader.GetString(2), "testvarchar1");
            checkSuccess(Convert.ToString(datareader[2]), "testvarchar1");
            checkSuccess(datareader.GetString(3), "01-JAN-01");
            checkSuccess(Convert.ToString(datareader[3]), "1/1/2001 12:00:00 AM");
            checkSuccess(datareader.GetString(4), "testlong1");
            checkSuccess(System.Text.Encoding.Default.GetString((Byte[])datareader[4]), "testlong1");
            checkSuccess(datareader.GetString(5), "testclob1");
            checkSuccess(Convert.ToString(datareader[5]), "testclob1");
            checkSuccess(datareader[6], null);
            Console.WriteLine("\n");

            // fields by name
            Console.WriteLine("FIELDS BY NAME:");
            checkSuccess(Convert.ToInt64(datareader["TESTNUMBER"]), 1);
            checkSuccess(datareader.GetInt16(datareader.GetOrdinal("TESTNUMBER")), 1);
            checkSuccess(datareader.GetInt32(datareader.GetOrdinal("TESTNUMBER")), 1);
            checkSuccess(datareader.GetInt64(datareader.GetOrdinal("TESTNUMBER")), 1);
            checkSuccess(Convert.ToString(datareader["TESTCHAR"]), "testchar1                               ");
            checkSuccess(datareader.GetString(datareader.GetOrdinal("TESTCHAR")), "testchar1                               ");
            checkSuccess(Convert.ToString(datareader["TESTVARCHAR"]), "testvarchar1");
            checkSuccess(datareader.GetString(datareader.GetOrdinal("TESTVARCHAR")), "testvarchar1");
            checkSuccess(Convert.ToString(datareader["TESTDATE"]), "1/1/2001 12:00:00 AM");
            checkSuccess(datareader.GetString(datareader.GetOrdinal("TESTDATE")), "01-JAN-01");
            checkSuccess(System.Text.Encoding.Default.GetString((Byte[])datareader["TESTLONG"]), "testlong1");
            checkSuccess(datareader.GetString(datareader.GetOrdinal("TESTLONG")), "testlong1");
            checkSuccess(Convert.ToString(datareader["TESTCLOB"]), "testclob1");
            checkSuccess(datareader.GetString(datareader.GetOrdinal("TESTCLOB")), "testclob1");
            checkSuccess(datareader["TESTBLOB"], null);
            checkSuccess(datareader.GetString(datareader.GetOrdinal("TESTBLOB")), "");
            Console.WriteLine("\n");

            // fields by array
            Console.WriteLine("FIELDS BY ARRAY:");
            Object[] fields = new Object[datareader.FieldCount];
            checkSuccess((Int64)datareader.GetValues(fields), datareader.FieldCount);
            checkSuccess(Convert.ToInt64(fields[0]), 1);
            checkSuccess(Convert.ToString(fields[1]), "testchar1                               ");
            checkSuccess(Convert.ToString(fields[2]), "testvarchar1");
            checkSuccess(Convert.ToString(fields[3]), "1/1/2001 12:00:00 AM");
            checkSuccess(System.Text.Encoding.Default.GetString((Byte[])fields[4]), "testlong1");
            checkSuccess(Convert.ToString(fields[5]), "testclob1");
            checkSuccess(fields[6], null);
            Console.WriteLine("\n");

            // more rows
            Console.WriteLine("MORE ROWS:");
            checkSuccess(datareader.Read(), true);
            checkSuccess(datareader.GetInt64(0), 2);
            checkSuccess(System.Text.Encoding.Default.GetString((Byte[])datareader[6]), "testblob2");
            checkSuccess(datareader.Read(), true);
            checkSuccess(datareader.GetInt64(0), 3);
            checkSuccess(datareader.Read(), true);
            checkSuccess(datareader.GetInt64(0), 4);
            checkSuccess(datareader.Read(), true);
            checkSuccess(datareader.GetInt64(0), 5);
            checkSuccess(datareader.Read(), true);
            checkSuccess(datareader.GetString(0), "");
            checkSuccess(datareader.Read(), false);
            checkSuccess(datareader.GetString(0), null);
            Console.WriteLine("\n");

            // commit and rollback
            Console.WriteLine("COMMIT AND ROLLBACK:");
            SQLRelayConnection sqlrcon2 = new SQLRelayConnection("Data Source=sqlrelay:9000:/tmp/test.socket;Retry Time=0;Tries=1;Tls=yes;Tlscert=" + tlscert + ";Tlsvalidate=ca;Tlsca=" + tlsca + ";Debug=false");
            sqlrcon2.Open();
            SQLRelayCommand sqlrcom2 = new SQLRelayCommand("select count(*) from testtable", sqlrcon2);
            checkSuccess(Convert.ToInt64(sqlrcom2.ExecuteScalar()), 0);
            SQLRelayTransaction sqlrtran = sqlrcon.BeginTransaction();
            sqlrtran.Commit();
            checkSuccess(Convert.ToInt64(sqlrcom2.ExecuteScalar()), 6);
            sqlrtran = sqlrcon.BeginTransaction();
            sqlrcom.CommandText = "insert into testtable values (6, 'testchar6', 'testvarchar6', '01-JAN-2006', 'testlong6', 'testclob6', empty_blob())";
            checkSuccess(sqlrcom.ExecuteNonQuery(), 1);
            sqlrcom.CommandText = "select count(*) from testtable";
            checkSuccess(Convert.ToInt64(sqlrcom.ExecuteScalar()), 7);
            sqlrtran.Rollback();
            checkSuccess(Convert.ToInt64(sqlrcom2.ExecuteScalar()), 6);
            sqlrcon2.Close();
            Console.WriteLine("\n");

            // output bind by name
            Console.WriteLine("OUTPUT BINDS BY NAME:");
            sqlrcom.CommandText = "begin  :numvar:=1;  :stringvar:='hello';  :floatvar:=2.5;  :datevar:='03-FEB-2001'; end;";
            SQLRelayParameter numvar = new SQLRelayParameter();
            numvar.ParameterName = "numvar";
            numvar.Direction = ParameterDirection.Output;
            numvar.DbType = DbType.Int64;
            sqlrcom.Parameters.Add(numvar);
            SQLRelayParameter stringvar = new SQLRelayParameter();
            stringvar.ParameterName = "stringvar";
            stringvar.Direction = ParameterDirection.Output;
            stringvar.DbType = DbType.String;
            stringvar.Size = 20;
            sqlrcom.Parameters.Add(stringvar);
            SQLRelayParameter floatvar = new SQLRelayParameter();
            floatvar.ParameterName = "floatvar";
            floatvar.Direction = ParameterDirection.Output;
            floatvar.DbType = DbType.Double;
            sqlrcom.Parameters.Add(floatvar);
            SQLRelayParameter datevar = new SQLRelayParameter();
            datevar.ParameterName = "datevar";
            datevar.Direction = ParameterDirection.Output;
            datevar.DbType = DbType.DateTime;
            sqlrcom.Parameters.Add(datevar);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            checkSuccess(Convert.ToInt64(numvar.Value), 1);
            checkSuccess(Convert.ToString(stringvar.Value), "hello");
            checkSuccess(Convert.ToInt64(stringvar.Size), 5);
            checkSuccess(Convert.ToString(floatvar.Value), "2.5");
            checkSuccess(Convert.ToInt64(Convert.ToDateTime(datevar.Value).Year), 2001);
            checkSuccess(Convert.ToInt64(Convert.ToDateTime(datevar.Value).Month), 2);
            checkSuccess(Convert.ToInt64(Convert.ToDateTime(datevar.Value).Day), 3);
            sqlrcom.Parameters.Clear();
            Console.WriteLine("\n");

            // output bind by position
            Console.WriteLine("OUTPUT BINDS BY POSITION:");
            sqlrcom.CommandText = "begin  :numvar:=1;  :stringvar:='hello';  :floatvar:=2.5;  :datevar:='03-FEB-2001'; end;";
            numvar = new SQLRelayParameter();
            numvar.ParameterName = "1";
            numvar.Direction = ParameterDirection.Output;
            numvar.DbType = DbType.Int64;
            sqlrcom.Parameters.Add(numvar);
            stringvar = new SQLRelayParameter();
            stringvar.ParameterName = "2";
            stringvar.Direction = ParameterDirection.Output;
            stringvar.DbType = DbType.String;
            stringvar.Size = 20;
            sqlrcom.Parameters.Add(stringvar);
            floatvar = new SQLRelayParameter();
            floatvar.ParameterName = "3";
            floatvar.Direction = ParameterDirection.Output;
            floatvar.DbType = DbType.Double;
            sqlrcom.Parameters.Add(floatvar);
            datevar = new SQLRelayParameter();
            datevar.ParameterName = "4";
            datevar.Direction = ParameterDirection.Output;
            datevar.DbType = DbType.DateTime;
            sqlrcom.Parameters.Add(datevar);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            checkSuccess(Convert.ToInt64(numvar.Value), 1);
            checkSuccess(Convert.ToString(stringvar.Value), "hello");
            checkSuccess(Convert.ToInt64(stringvar.Size), 5);
            checkSuccess(Convert.ToString(floatvar.Value), "2.5");
            checkSuccess(Convert.ToInt64(Convert.ToDateTime(datevar.Value).Year), 2001);
            checkSuccess(Convert.ToInt64(Convert.ToDateTime(datevar.Value).Month), 2);
            checkSuccess(Convert.ToInt64(Convert.ToDateTime(datevar.Value).Day), 3);
            sqlrcom.Parameters.Clear();
            Console.WriteLine("\n");



            // cursor binds using NextResult
            Console.WriteLine("CURSOR BINDS USING NEXTRESULT:");
            sqlrcom.CommandText = "create or replace package types is type cursorType is ref cursor; end;";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            sqlrcom.CommandText = "create or replace function sp_testtable(value in number) return types.cursortype is l_cursor    types.cursorType; begin open l_cursor for select * from testtable where testnumber>value; return l_cursor; end;";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            sqlrcom.CommandText = "begin  :curs1:=sp_testtable(2);  :curs2:=sp_testtable(0); end;";
            SQLRelayParameter curs1 = new SQLRelayParameter();
            curs1.ParameterName = "curs1";
            curs1.SQLRelayType = SQLRelayType.Cursor;
            curs1.Direction = ParameterDirection.Output;
            curs1.Value = null;
            sqlrcom.Parameters.Add(curs1);
            SQLRelayParameter curs2 = new SQLRelayParameter();
            curs2.ParameterName = "curs2";
            curs2.SQLRelayType = SQLRelayType.Cursor;
            curs2.Direction = ParameterDirection.Output;
            curs2.Value = null;
            sqlrcom.Parameters.Add(curs2);
            datareader = ExecuteReader(sqlrcom);
            checkSuccess(datareader != null, true);
            sqlrcom.Parameters.Clear();
            datareader.Read();
            checkSuccess(datareader.GetInt64(0), 3);
            datareader.Read();
            checkSuccess(datareader.GetInt64(0), 4);
            datareader.Read();
            checkSuccess(datareader.GetInt64(0), 5);
            datareader.Close();
            checkSuccess(datareader.NextResult(), true);
            datareader.Read();
            checkSuccess(datareader.GetInt64(0), 1);
            datareader.Read();
            checkSuccess(datareader.GetInt64(0), 2);
            datareader.Read();
            checkSuccess(datareader.GetInt64(0), 3);
            datareader.Close();
            checkSuccess(datareader.NextResult(), false);
            sqlrcom.CommandText = "drop package types";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            Console.WriteLine("\n");



            // cursor binds
            Console.WriteLine("CURSOR BINDS:");
            sqlrcom.CommandText = "create or replace package types is type cursorType is ref cursor; end;";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            sqlrcom.CommandText = "create or replace function sp_testtable(value in number) return types.cursortype is l_cursor    types.cursorType; begin open l_cursor for select * from testtable where testnumber>value; return l_cursor; end;";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            sqlrcom.CommandText = "begin  :curs1:=sp_testtable(2);  :curs2:=sp_testtable(0); end;";
            curs1 = new SQLRelayParameter();
            curs1.ParameterName = "curs1";
            curs1.SQLRelayType = SQLRelayType.Cursor;
            curs1.Direction = ParameterDirection.Output;
            curs1.Value = null;
            sqlrcom.Parameters.Add(curs1);
            curs2 = new SQLRelayParameter();
            curs2.ParameterName = "curs2";
            curs2.SQLRelayType = SQLRelayType.Cursor;
            curs2.Direction = ParameterDirection.Output;
            curs2.Value = null;
            sqlrcom.Parameters.Add(curs2);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            SQLRelayDataReader curs1reader = (SQLRelayDataReader)curs1.Value;
            curs1reader.Read();
            checkSuccess(curs1reader.GetInt64(0), 3);
            curs1reader.Read();
            checkSuccess(curs1reader.GetInt64(0), 4);
            curs1reader.Read();
            checkSuccess(curs1reader.GetInt64(0), 5);
            curs1reader.Close();
            SQLRelayDataReader curs2reader = (SQLRelayDataReader)curs2.Value;
            curs2reader.Read();
            checkSuccess(curs2reader.GetInt64(0), 1);
            curs2reader.Read();
            checkSuccess(curs2reader.GetInt64(0), 2);
            curs2reader.Read();
            checkSuccess(curs2reader.GetInt64(0), 3);
            curs2reader.Close();
            sqlrcom.CommandText = "drop package types";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            Console.WriteLine("\n");



            // clob and blob output bind
            Console.WriteLine("CLOB AND BLOB OUTPUT BINDS:");
            sqlrcom.CommandText = "drop table testtable1";
            ExecuteNonQuery(sqlrcom);
            sqlrcom.CommandText = "create table testtable1 (testclob clob, testblob blob)";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            sqlrcom.CommandText = "insert into testtable1 values ('hello', :var1)";
            SQLRelayParameter var1 = new SQLRelayParameter();
            var1.ParameterName = "var1";
            var1.Value = System.Text.Encoding.Default.GetBytes("hello");
            var1.SQLRelayType = SQLRelayType.Blob;
            sqlrcom.Parameters.Add(var1);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            sqlrcom.CommandText = "begin select testclob into :clobvar from testtable1; select testblob into :blobvar from testtable1; end;";
            SQLRelayParameter clobvar = new SQLRelayParameter();
            clobvar.Direction = ParameterDirection.Output;
            clobvar.ParameterName = "clobvar";
            clobvar.SQLRelayType = SQLRelayType.Clob;
            sqlrcom.Parameters.Add(clobvar);
            SQLRelayParameter blobvar = new SQLRelayParameter();
            blobvar.ParameterName = "blobvar";
            blobvar.SQLRelayType = SQLRelayType.Blob;
            blobvar.Direction = ParameterDirection.Output;
            sqlrcom.Parameters.Add(blobvar);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            checkSuccess(Convert.ToString(clobvar.Value), "hello", 5);
            checkSuccess(clobvar.Size, 5);
            checkSuccess(System.Text.Encoding.Default.GetString((byte[])blobvar.Value), "hello");
            checkSuccess(blobvar.Size, 5);
            sqlrcom.CommandText = "drop table testtable1";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            Console.WriteLine("\n");

            // null and empty clobs and blobs
            Console.WriteLine("NULL AND EMPTY CLOBS AND BLOBS:");
            sqlrcom.CommandText = "create table testtable1 (testclob1 clob, testclob2 clob, testblob1 blob, testblob2 blob)";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            sqlrcom.CommandText = "insert into testtable1 values (:testclob1, :testclob2, :testblob1, :testblob2)";
            SQLRelayParameter testclob1 = new SQLRelayParameter();
            testclob1.ParameterName = "testclob1";
            testclob1.SQLRelayType = SQLRelayType.Clob;
            testclob1.Value = "";
            sqlrcom.Parameters.Add(testclob1);
            SQLRelayParameter testclob2 = new SQLRelayParameter();
            testclob2.ParameterName = "testclob2";
            testclob2.SQLRelayType = SQLRelayType.Clob;
            testclob2.Value = null;
            sqlrcom.Parameters.Add(testclob2);
            SQLRelayParameter testblob1 = new SQLRelayParameter();
            testblob1.ParameterName = "testblob1";
            testblob1.SQLRelayType = SQLRelayType.Blob;
            testblob1.Value = System.Text.Encoding.Default.GetBytes("");
            sqlrcom.Parameters.Add(testblob1);
            SQLRelayParameter testblob2 = new SQLRelayParameter();
            testblob2.ParameterName = "testblob2";
            testblob2.SQLRelayType = SQLRelayType.Blob;
            testblob2.Value = null;
            sqlrcom.Parameters.Add(testblob2);
            checkSuccess(ExecuteNonQuery(sqlrcom), 1);
            sqlrcom.Parameters.Clear();
            sqlrcom.CommandText = "select * from testtable1";
            datareader = ExecuteReader(sqlrcom);
            checkSuccess(datareader != null, true);
            checkSuccess(datareader.Read(), true);
            // FIXME: I'd expect these to come out as empty strings, not null's
            checkSuccess(datareader.GetString(0), "");
            checkSuccess(datareader.GetString(1), "");
            checkSuccess(datareader.GetString(2), "");
            checkSuccess(datareader.GetString(3), "");
            sqlrcom.CommandText = "drop table testtable1";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            Console.WriteLine("\n");

            // switching connection of command
            Console.WriteLine("SWITCHING CONNECTION OF COMMAND:");
            sqlrcom.Connection = sqlrcon2;
            sqlrcom.CommandText = "select count(*) from testtable";
            try
            {
                sqlrcom.ExecuteScalar();
                checkSuccess(false, true);
            }
            catch
            {
                // this should fail because sqlrcon2 was closed earlier
                checkSuccess(true, true);
            }
            sqlrcom.Connection = sqlrcon;
            checkSuccess(ExecuteScalar(sqlrcom), 6);
            Console.WriteLine("\n");

            // closed datareader
            Console.WriteLine("CLOSED DATAREADER:");
            sqlrcom.CommandText = "select * from testtable";
            datareader = sqlrcom.ExecuteReader();
            checkSuccess(datareader != null, true);
            datareader.Read();
            datareader.Close();
            checkSuccess(datareader.IsClosed, true);
            try
            {
                datareader.Read();
                checkSuccess(false, true);
            }
            catch
            {
                // this should fail because datareader was closed earlier
                checkSuccess(true, true);
            }
            Console.WriteLine("\n");

            // has rows
            Console.WriteLine("HAS ROWS:");
            sqlrcom.CommandText = "select * from testtable";
            datareader = ExecuteReader(sqlrcom);
            checkSuccess(datareader != null, true);
            checkSuccess(((SQLRelayDataReader)datareader).HasRows, true);
            sqlrcom.CommandText = "delete from testtable";
            checkSuccess(ExecuteNonQuery(sqlrcom), 6);
            sqlrcom.CommandText = "select * from testtable";
            datareader = ExecuteReader(sqlrcom);
            checkSuccess(datareader != null, true);
            checkSuccess(((SQLRelayDataReader)datareader).HasRows, false);
            Console.WriteLine("\n");

            // drop table
            Console.WriteLine("DROP TABLE:");
            sqlrcom.CommandText = "drop table testtable";
            checkSuccess(ExecuteNonQuery(sqlrcom), 0);
            Console.WriteLine("\n");



            // invalid queries

            sqlrcon.Close();
        }
    }
}
