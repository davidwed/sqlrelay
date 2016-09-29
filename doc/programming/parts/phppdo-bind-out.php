$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0","testuser","testpassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->prepare("begin  :numvar:=1; :stringvar:='hello'; end;");
$param1=0;
$param2="";
$stmt->bindParam(":numvar",$param1,PDO::PARAM_INT|PDO::PARAM_INPUT_OUTPUT);
$stmt->bindParam(":stringvar",$param2,PDO::PARAM_STR|PDO::PARAM_INPUT_OUTPUT);
$stmt->execute();
echo($param1.",".$param2);
