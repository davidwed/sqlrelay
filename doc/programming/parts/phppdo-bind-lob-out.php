$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->prepare("select blob_col into :blobvar from exampletable");
$blobvar="";
$stmt->bindParam(":blobvar",$blobvar,PDO::PARAM_LOB|PDO::PARAM_INPUT_OUTPUT);
$stmt->execute();
echo(stream_get_contents($blobvar));
