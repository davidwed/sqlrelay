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

			try
			{
				sqlrcom.ExecuteNonQuery();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			... do something with numvar.Value ...
			... do something with stringvar.Value ...
			... do something with floatvar.Value ...
			... do something with datevar.Value ...
		}
	}
}
