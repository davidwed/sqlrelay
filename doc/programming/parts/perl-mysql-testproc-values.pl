$cur->prepareQuery("call exampleproc(?,?,?)");
$cur->inputBind("1",1);
$cur->inputBind("2",1.1,4,2);
$cur->inputBind("3","hello");
$cur->executeQuery();
my $out1=$cur->getField(0,0);
my $out2=$cur->getField(0,1);
my $out3=$cur->getField(0,2);
