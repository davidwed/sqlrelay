SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
sqlrcom.CommandText = "select testproc()";
Int64 result = sqlrcom.ExecuteScalar();
