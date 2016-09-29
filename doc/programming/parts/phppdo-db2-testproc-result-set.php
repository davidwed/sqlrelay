$stmt=$dbh->prepare("select * from testfunc() as (testint int, testfloat float, testchar char(40)");
$result=$stmt->execute();
