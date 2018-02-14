my $sth=$dbh->prepare("select * from examplefunc() as (exampleint int, examplefloat float, examplechar char(40))");
$sth->execute();
my $col1;
my $col2;
my $col3;
$sth->bind_columns(undef,\$col1,\$col2,\$col3);
while ($sth->fetch()) {
        print "$col1, $col2, $col3\n";
}
