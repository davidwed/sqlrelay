$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
if (!$dbh) {
	die("connection failed");
}

$stmt=$dbh->prepare("select int_col, float_col, string_col, blob_col from exampletable");

$col1=0;
$col2="";
$col3="";
$col4="";
$stmt->bindColumn(1,$col1);
$stmt->bindColumn(2,$col2);
$stmt->bindColumn(3,$col3);
$stmt->bindColumn(4,$col4);

$stmt->execute();

$result=$stmt->fetch(PDO::FETCH_BOUND);
echo($col1.",".$col2.",".$col3.",".stream_get_contents($col4)."\n");

$result=$stmt->fetch(PDO::FETCH_BOUND);
echo($col1.",".$col2.",".$col3.",".stream_get_contents($col4)."\n");
