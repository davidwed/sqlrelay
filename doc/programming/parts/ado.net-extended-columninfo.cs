using System;
using SQLRClient;
using System.Data;
using System.IO;

namespace SQLRExamples
{
	class SQLRExample
	{
		public static void Main()
		{
			SQLRelayConnection sqlrcon = new SQLRelayConnection("Data Source=sqlrserver:9000;User ID=user;Password=password;Retry Time=0;Tries=1;Debug=false");
			sqlrcon.Open();

			SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
			sqlrcom.CommandText = "select * from my_first_table";

			try
			{
				System.Data.IDataReader datareader = sqlrcom.ExecuteReader();

				DataTable schematable = datareader.GetSchemaTable();
				for (UInt32 index = 0; index &lt; sqlrcom.FieldCount; index++)
				{
					String columnname = Convert.ToString(schematable.Rows[index]["ColumnName"]);
					Int64 columnordinal = Convert.ToInt64(schematable.Rows[index]["ColumnOrdinal"]);
            				Int64 columnsize = Convert.ToInt64(schematable.Rows[index]["ColumnSize"]);
            				Int64 numericprecision = Convert.ToInt64(schematable.Rows[index]["NumericPrecision"]);
            				Int64 numericscale = Convert.ToInt64(schematable.Rows[index]["NumericScale"]);
            				Boolean isunique = Convert.ToBoolean(schematable.Rows[index]["IsUnique"]);
            				Boolean iskey = Convert.ToBoolean(schematable.Rows[index]["IsKey"]);
            				String baseservername = Convert.ToString(schematable.Rows[index]["BaseServerName"]);
            				String basecatalogname = Convert.ToString(schematable.Rows[index]["BaseCatalogName"]);
            				String basecolumnname = Convert.ToString(schematable.Rows[index]["BaseColumnName"]);
            				String baseschemaname = Convert.ToString(schematable.Rows[index]["BaseSchemaName"]);
            				String basetablename = Convert.ToString(schematable.Rows[index]["BaseTableName"]);
            				String datatype = Convert.ToString(schematable.Rows[index]["DataType"]);
            				Boolean allowdbnull = Convert.ToBoolean(schematable.Rows[index]["AllowDBNull"]);
            				String providertype = Convert.ToString(schematable.Rows[index]["ProviderType"]);
            				Boolean isaliased = Convert.ToBoolean(schematable.Rows[index]["IsAliased"]);
            				Boolean isexpression = Convert.ToBoolean(schematable.Rows[index]["IsExpression"]);
            				Boolean isidentity = Convert.ToBoolean(schematable.Rows[index]["IsIdentity"]);
            				Boolean isautoincrement = Convert.ToBoolean(schematable.Rows[index]["IsAutoIncrement"]);
            				Boolean isrowversion = Convert.ToBoolean(schematable.Rows[index]["IsRowVersion"]);
            				Boolean ishidden = Convert.ToBoolean(schematable.Rows[index]["IsHidden"]);
            				Boolean islong = Convert.ToBoolean(schematable.Rows[index]["IsLong"]);
            				Boolean isreadonly = Convert.ToBoolean(schematable.Rows[index]["IsReadOnly"]);
            				String providerspecificdatatype = Convert.ToString(schematable.Rows[index]["ProviderSpecificDataType"]);
            				String datatypename = Convert.ToString(schematable.Rows[index]["DataTypeName"]);
            				String xmlschemacollectiondatabase = Convert.ToString(schematable.Rows[index]["XmlSchemaCollectionDatabase"]);
            				String xmlschemacollectionowningschema = Convert.ToString(schematable.Rows[index]["XmlSchemaCollectionOwningSchema"]);
            				String xmlschemacollectionname = Convert.ToString(schematable.Rows[index]["XmlSchemaCollectionName"]);

					... do something with all these bits of information ...
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
	}
}
