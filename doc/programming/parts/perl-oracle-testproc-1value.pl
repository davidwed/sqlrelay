$cur->prepareQuery("select testproc(:in1,:in2,:in3) from dual");
$cur->inputBind("in1",1);
$cur->inputBind("in2",1.1,2,1);
$cur->inputBind("in3","hello");
$cur->executeQuery();
my $result=$cur->getField(0,0);
