my $sth=$dth->prepare("exec exampleproc");
$sth->bind_param("in1",1);
$sth->bind_param("in2",1.1,2,1);
$sth->bind_param("in3","hello");
my $result;
$sth->bind_param_inout("out1",\$result,20);
$sth->execute();
