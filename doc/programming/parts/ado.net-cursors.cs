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

			SQLRelayCommand sqlrcom1 = (SQLRelayComand)sqlrcon.CreateCommand();
			sqlrcom1.CommandText = "select * from my_first_table";


			SQLRelayCommand sqlrcom2 = (SQLRelayComand)sqlrcon.CreateCommand();
			sqlrcom2.CommandText = "insert into my_second_table values (:col1, :col2, :col3)";

			try
			{
				System.Data.IDataReader datareader = sqlrcom1.ExecuteReader();
				while (datareader.Read())
				{
					sqlrcom2.Parameters.Clear();
					sqlrcom2.Parameters.Add("col1",datareader.GetString(0));
					sqlrcom2.Parameters.Add("col2",datareader.GetString(1));
					sqlrcom2.Parameters.Add("col3",datareader.GetString(2));
					try
					{
						sqlrcom2.ExecuteNonQuery();
					}
					catch (Exception ex)
					{
						Console.WriteLine(ex.Message);
					}
				}
				
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
	}
}
