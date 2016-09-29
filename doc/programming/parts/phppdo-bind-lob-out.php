$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0","testuser","testpassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->prepare("select blob_col into :blobvar from testtable");
$blobvar="";
$stmt->bindParam(":blobvar",$blobvar,PDO::PARAM_LOB|PDO::PARAM_INPUT_OUTPUT);
$stmt->execute();
echo(stream_get_contents($blobvar));
