my $sth->$dbh->prepare("begin exampleproc(:in1,:in2,:in3); end;");
$sth->bind_param("in1",1);
$sth->bind_param("in2",1.1,2,1);
$sth->bind_param("in3","hello");
$sth->execute();
