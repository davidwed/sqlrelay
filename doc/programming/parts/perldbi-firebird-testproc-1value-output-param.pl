my $sth=$dbh->prepare("execute procedure testproc ?, ?, ?");
$sth->bind_param("1",1);
$sth->bind_param("2",1.1,2,1);
$sth->bind_param("3","hello");
my $result;
$sth->bind_inout_param("1",\$result,20);
$sth->execute();
