$stmt=$dbh->prepare("execute procedure testproc ?, ?, ?");
$stmt->bindValue("1",1);
$stmt->bindValue("2","1.1");
$stmt->bindValue("3","hello");
$result=$stmt->execute();
