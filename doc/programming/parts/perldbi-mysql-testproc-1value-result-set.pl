my $sth=$dbh->prepare("select testproc()");
$sth->execute();
$sth->bind_columns(undef,\$out1);
$sth->fetch();
