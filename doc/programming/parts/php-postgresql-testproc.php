sqlrcur_prepareQuery($cur,"select examplefunc($1,$2,$3)");
sqlrcur_inputBind($cur,"1",1);
sqlrcur_inputBind($cur,"2",1.1,2,1);
sqlrcur_inputBind($cur,"3","hello");
sqlrcur_executeQuery($cur);
