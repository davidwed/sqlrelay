SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
sqlrcom.CommandText = "begin exampleproc(:in1,:in2,:in3); end;";
sqlrcom.Parameters.Add("in1",1);
sqlrcom.Parameters.Add("in2",1.1);
sqlrcom.Parameters.Add("in3","hello");
sqlrcom.ExecuteNonQuery();
