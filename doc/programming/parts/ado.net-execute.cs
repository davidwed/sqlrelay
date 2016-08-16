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


			// DML queries...
			SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
			sqlrcom.CommandText = "insert into testtable values (1,'hello')";
			try
			{
				sqlrcom.ExecuteNonQuery();
			}
			catch (Exception ex)
			{
				... handle exceptions ...
			}


			// Single values
			sqlrcom.CommandText = "select 1 from dual";
			try
			{
				Int64 value = sqlrcom.ExecuteScalar();

				... do something with the value ...
			}
			catch (Exception ex)
			{
				... handle exceptions ...
			}


			// Multiple rows
			sqlrcom.CommandText = "select * from testtable";
			try
			{
				System.Data.IDataReader datareader = sqlrcom.ExecuteReader();

				... do something with the result set ...
			}
			catch (Exception ex)
			{
				... handle exceptions ...
			}
		}
	}
}
