$stmt=$dbh->prepare("select * from examplefunc($1,$2,$3) as (col1 int, col2 float, col3 char(20))");
$stmt->bindValue("$1",1);
$stmt->bindValue("$2","1.1");
$stmt->bindValue("$3","hello");
$result=$stmt->execute();
