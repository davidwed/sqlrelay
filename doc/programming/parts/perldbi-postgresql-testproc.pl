my $sth=$dbh->prepare("select examplefunc($1,$2,$3)");
$sth->bind_param("1",1);
$sth->bind_param("2",1.1,2,1);
$sth->bind_param("3","hello");
$sth->execute();
