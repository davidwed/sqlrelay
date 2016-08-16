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
			SQLRelayConnection sqlrcon = new SQLRelayConnection("Data Source=sqlrserver:9000;User ID=test;Password=test;Retry Time=0;Tries=1;Debug=false");
			sqlrcon.Open();

			SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
			sqlrcom.CommandText = "insert into testtable values (:clobvar,:blobvar)";

			SQLRelayParameter clobvar = new SQLRelayParameter();
			clobvar.ParameterName = "clobvar";
			clobvar.Value = "testclob";
			clobvar.SQLRelayType = SQLRelayType.Clob;
			sqlrcom.Parameters.Add(clobvar);

			SQLRelayParameter blobvar = new SQLRelayParameter();
			blobvar.ParameterName = "blobvar";
			blobvar.Value = System.Text.Encoding.Default.GetBytes("testblob");
			blobvar.SQLRelayType = SQLRelayType.Blob;
			sqlrcom.Parameters.Add(blobvar);

			try
			{
				sqlrcom.ExecuteNonQuery();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
	}
}
