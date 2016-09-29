$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0","testuser","testpassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->query("select int_col, float_col, string_col, blob_col from testtable");
$result=$stmt->fetch();
echo($result[0].",".$result[1].",".$result[2].",".stream_get_contents($result[3])."\n");
echo($result["int_col"].",".$result["float_col"].",".$result["string_col"].",".stream_get_contents("blob_col")."\n");
