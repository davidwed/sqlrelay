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
			sqlrcom.CommandText = "select col1,col2 from exampletable";

			try
			{
				System.Data.IDataReader datareader = sqlrcom.ExecuteReader();
				
				// read row...
				datareader.Read();

				// get the raw data of the first field in a variety of ways
				Object o1 = datareader.GetValue(0);
				o1 = datareader[0];
				o1 = datareader.GetValue(datareader.GetOrdinal("col1"));
				o1 = datareader["col1"];
				Object[] os1 = datareader.GetValues();
				o1 = os1[0];

				// get the first field as a string
				String s1 = datareader.GetString(0);


				// get the raw data of the second field in a variety of ways
				Object o2 = datareader.GetValue(1);
				o2 = datareader[1];
				o2 = datareader.GetValue(datareader.GetOrdinal("col2"));
				o2 = datareader["col2"];
				Object[] os2 = datareader.GetValues();
				o2 = os2[1];

				// get the second field as a string
				String s2 = datareader.GetString(1);
				
				// read another row...
				datareader.Read();

				... do something with this row ...

				... fetch more rows ...
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
	}
}
