my $sth=$dbh->prepare("begin exampleproc(:in1,:in2,:in3,:out1); end;");
$sth->bind_param("in1",1);
$sth->bind_param("in2",1.1,2,1);
$sth->bind_param("in3","hello");
my $result;
$sth->bind_param_inout("out1",\$result,20);
$sth->execute();
