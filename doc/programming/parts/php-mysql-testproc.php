sqlrcur_prepareQuery($cur,"call exampleproc(?,?,?)");
sqlrcur_inputBindLong($cur,"1",1);
sqlrcur_inputBindDouble($cur,"2",1.1,2,1);
sqlrcur_inputBindString($cur,"3","hello");
sqlrcur_executeQuery($cur);
