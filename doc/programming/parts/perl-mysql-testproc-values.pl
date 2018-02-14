$cur->prepareQuery("call exampleproc(:in1,:in2,:in3)");
$cur->inputBind("in1",1);
$cur->inputBind("in2",1.1,4,2);
$cur->inputBind("in3","hello");
$cur->executeQuery();
my $out1=$cur->getField(0,0);
my $out2=$cur->getField(0,1);
my $out3=$cur->getField(0,2);
