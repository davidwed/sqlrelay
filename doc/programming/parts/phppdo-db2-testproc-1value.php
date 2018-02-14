$stmt=$dbh->prepare("call exampleproc(?,?,?,?)");
$stmt->bindValue("1",1);
$stmt->bindValue("2","1.1");
$stmt->bindValue("3","hello");
$out1=0;
$stmt->bindParam("4",$out1,PDO::PARAM_INT|PDO::PARAM_INPUT_OUTPUT);
$result=$stmt->execute();
