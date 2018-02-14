$dbh->query("set @out1=0, @out2=0.0, @out3=''");

$dbh->query("call exampleproc(@out1,@out2,@out3)");

$stmt=$dbh->prepare("select @out1,@out2,@out3");
$result=$stmt->execute();
