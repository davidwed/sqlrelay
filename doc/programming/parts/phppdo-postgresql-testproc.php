$stmt=$dbh->prepare("call testfunc($1,$2,$3)");
$stmt->bindValue("$1",1);
$stmt->bindValue("$2","1.1");
$stmt->bindValue("$3","hello");
$result=$stmt->execute();
