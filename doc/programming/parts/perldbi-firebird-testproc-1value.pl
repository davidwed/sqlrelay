my $sth=$dth->prepare("select * from exampleproc(?,?,?)");
$sth->bind_param("1",1);
$sth->bind_param("2",1.1,2,1);
$sth->bind_param("3","hello");
$sth->execute();
my $result;
$sth->bind_columns(undef,\$result);
$sth->fetch();
