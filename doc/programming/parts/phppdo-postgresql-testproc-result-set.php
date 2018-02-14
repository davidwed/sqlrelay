$stmt=$dbh->prepare("select * from examplefunc() as (exampleint int, examplefloat float, examplechar char(40)");
$result=$stmt->execute();
