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
				for (UInt32 index = 0; index &lt; sqlrcom.FieldCount; index++)
				{
					String name = datareader.GetName(index);
					String datatype = datareader.GetDataTypename(index);
					String nativedatatype = datareader.GetFieldType(index).ToString();

					... do someting with the column data ...
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
	}
}
