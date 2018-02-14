SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
sqlrcom.CommandText = "begin :curs=exampleproc; end;";

SQLRelayParameter curs = new SQLRelayParameter();
curs.ParameterName = "curs";
curs.Direction = ParameterDirection.Output;
curs.SQLRelayType = SQLRelayType.Cursor;
sqlrcom.Parameters.Add(curs);

System.Data.IDataReader datareader = sqlrcom.ExecuteReader();

// the main result set will be empty, skip to the result set of the cursor
datareader.NextResult();

datareader.Read();
Int64 out1 = datareader.GetInt64(0);
Double out2 = datareader.GetDouble(1);
String out3 = datareader.GetString(2);
