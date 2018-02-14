my $sth=$dbh->prepare("execute procedure exampleproc ?, ?, ?");
$sth->bind_param("1",1);
$sth->bind_param("2",1.1,2,1);
$sth->bind_param("3","hello");
$sth->execute();
