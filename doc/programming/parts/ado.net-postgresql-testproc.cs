SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
sqlrcom.CommandText = "call examplefunc($1,$2,$3)";
sqlrcom.Parameters.Add("1",1);
sqlrcom.Parameters.Add("2",1.1);
sqlrcom.Parameters.Add("3","hello");
sqlrcom.ExecuteNonQuery();
