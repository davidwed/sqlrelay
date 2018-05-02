$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->prepare("select int_col, float_col, string_col, blob_col from exampletable");
$stmt->execute();

$result=$stmt->fetchAll();

echo($result[0][0].",".$result[0][1].",".$result[0][2].",".stream_get_contents($result[0][3])."\n");
echo($result[1][0].",".$result[1][1].",".$result[1][2].",".stream_get_contents($result[1][3])."\n");
echo($result[2][0].",".$result[2][1].",".$result[2][2].",".stream_get_contents($result[2][3])."\n");
... and so on ...
