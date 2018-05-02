my $sth=$dbh->prepare("select exampleproc(:in1,:in2,:in3) from dual");
$sth->bind_param("in1",1);
$sth->bind_param("in2",1.1,2,1);
$sth->bind_param("in3","hello");
$sth->execute();
my $result;
$sth->bind_columns(undef,\$result);
$sth->fetch();
