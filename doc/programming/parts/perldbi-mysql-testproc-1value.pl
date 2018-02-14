my $sth=$dbh->prepare("select examplefunc(?,?,?)");
$sth->bind_param("1",1);
$sth->bind_param("2",1.1,4,2);
$sth->bind_param("3","hello");
$sth->execute();
my $out1;
$sth->bind_columns(undef,\$out1);
$sth->fetch();
