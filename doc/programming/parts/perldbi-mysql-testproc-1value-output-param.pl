my $sth=$dbh->prepare("select \@out1=0");
$sth->execute();
my $sth=$dbh->prepare("call testproc(\@out1)");
$sth->execute();
my $sth=$dbh->prepare("select \@out1");
$sth->execute();
$sth->bind_columns(undef,\$out1);
$sth->fetch();
