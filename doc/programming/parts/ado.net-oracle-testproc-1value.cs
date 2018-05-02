SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
sqlrcom.CommandText = "select exampleproc(:in1,:in2,:in3) from dual";
sqlrcom.Parameters.Add("in1",1);
sqlrcom.Parameters.Add("in2",1.1);
sqlrcom.Parameters.Add("in3","hello");
Int64 result=sqlrcom.ExecuteScalar();
