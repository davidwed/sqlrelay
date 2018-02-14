my $sth=$dbh->prepareQuery("exec exampleproc");
$sth->execute();
my $out1;
my $out2;
my $out3;
$sth->bind_columns(undef,\$out1,\$out2,\$out3);
while ($sth->fetch()) {
        print "$out1, $out2, $out3\n";
}
