my $sth=$dbh->prepare("select exampleproc()");
$sth->execute();
$sth->bind_columns(undef,\$out1);
$sth->fetch();
