SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
sqlrcom.CommandText = "exec exampleproc";
sqlrcom.Parameters.Add("in1",1);
sqlrcom.Parameters.Add("in2",1.1);
sqlrcom.Parameters.Add("in3","hello");
sqlrcom.ExecuteNonQuery();
