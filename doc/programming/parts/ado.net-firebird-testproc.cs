SQLRelayCommand sqlrcom = (SQLRelayComand)sqlrcon.CreateCommand();
sqlrcom.CommandText = "execute procedure exampleproc ?, ?, ?";
sqlrcom.Parameters.Add("1",1);
sqlrcom.Parameters.Add("2",1.1);
sqlrcom.Parameters.Add("3","hello");
sqlrcom.ExecuteNonQuery();
