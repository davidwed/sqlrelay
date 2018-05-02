SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
sqlrcom.CommandText = "select exampleproc()";
Int64 result = sqlrcom.ExecuteScalar();
