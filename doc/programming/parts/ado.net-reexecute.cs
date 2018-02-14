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

			// Prepare the query...
			SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
			sqlrcom.CommandText = "insert into exampletable values (:var1,:var2,:var3)";
			sqlrcom.Prepare();


			// Execute once...
			sqlrcom.Parameters.Add("var1", 1);
			sqlrcom.Parameters.Add("var2", "hello");
			sqlrcom.Parameters.Add("var2", new DateTime(2001, 1, 1, 0, 0, 0));
			try
			{
				sqlrcom.ExecuteNonQuery();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
			sqlrcom.Parameters.Clear();


			// Execute again with new values...
			sqlrcom.Parameters.Add("var1", 2);
			sqlrcom.Parameters.Add("var2", "bye");
			sqlrcom.Parameters.Add("var2", new DateTime(2002, 2, 2, 0, 0, 0));
			try
			{
				sqlrcom.ExecuteNonQuery();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}


			... re-bind and re-execute again and again ...
		}
	}
}
