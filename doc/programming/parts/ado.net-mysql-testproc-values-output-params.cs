SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();

sqlrcom.CommandText = "set @out1=0, @out2=0.0, @out3=''";
Int64 result = sqlrcom.ExecuteNonQuery();

sqlrcom.CommandText = "call exampleproc(@out1,@out2,@out3)";
Int64 result = sqlrcom.ExecuteNonQuery();

sqlrcom.CommandText = "select @out1,@out2,@out3";
System.Data.IDataReader datareader = sqlrcom.ExecuteReader();
datareader.Read();
Int64 out1 = datareader.GetInt64(0);
Double out2 = datareader.GetDouble(1);
String out3 = datareader.GetString(2);
