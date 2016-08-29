SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();

sqlrcom.CommandText = "set @out1=0";
Int64 result = sqlrcom.ExecuteNonQuery();

sqlrcom.CommandText = "call testproc(@out1)";
Int64 result = sqlrcom.ExecuteNonQuery();

sqlrcom.CommandText = "select @out1";
Int64 result = sqlrcom.ExecuteScalar();
