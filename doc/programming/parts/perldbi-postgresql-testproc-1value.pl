my $sth=$dbh->prepare("select * from examplefunc($1,$2,$3)");
$sth->bind_param("1",1);
$sth->bind_param("2",1.1,4,2);
$sth->bind_param("3","hello");
$sth->execute();
my $result;
$sth->bind_columns(undef,\$result);
$sth->fetch();
