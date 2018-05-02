my $sth=$dbh->prepareQuery("call exampleproc(?,?,?)");
$sth->bind_param("1",1);
$sth->bind_param("2",1.1,4,2);
$sth->bind_param("3","hello");
$sth->execute();
my $out1;
my $out2;
my $out3;
$sth->bind_columns(undef,\$out1,\$out2,\$out3);
while ($sth->fetch()) {
        print "$out1, $out2, $out3\n";
}
