$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->prepare("select int_col, float_col, string_col, blob_col from exampletable");
$stmt->execute();

$result=$stmt->fetchColumn(0);

echo($result[0]."\n");
echo($result[1]."\n");
echo($result[2]."\n");
... and so on ...
