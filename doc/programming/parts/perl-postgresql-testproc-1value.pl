$cur->prepareQuery("select * from testfunc($1,$2,$3)");
$cur->inputBind("1",1);
$cur->inputBind("2",1.1,4,2);
$cur->inputBind("3","hello");
$cur->executeQuery();
my $result=$cur->getField(0,0);
