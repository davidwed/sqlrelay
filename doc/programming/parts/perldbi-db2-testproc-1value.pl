my $sth=$dbh->prepare("call exampleproc(?,?,?,?)");
$sth->bind_param("1",1);
$sth->bind_param("2",1.1,2,1);
$sth->bind_param("3","hello");
my $result;
$sth->bind_param_inout("4",\$result,25);
$sth->execute();
