$cur->prepareQuery("exec testproc");
$cur->inputBind("in1",1);
$cur->inputBind("in2",1.1,2,1);
$cur->inputBind("in3","hello");
$cur->defineOutputBindInteger("out1");
$cur->executeQuery();
my $result=$cur->getOutputBindInteger("out1");
