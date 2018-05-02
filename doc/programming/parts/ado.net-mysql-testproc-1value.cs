SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
sqlrcom.CommandText = "select * from examplefunc(?,?,?)";
sqlrcom.Parameters.Add("1",1);
sqlrcom.Parameters.Add("2",1.1);
sqlrcom.Parameters.Add("3","hello");
Int64 result = sqlrcom.ExecuteScalar();
