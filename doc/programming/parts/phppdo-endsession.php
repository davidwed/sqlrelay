$dbh=new PDO("sqlrelay:host=sqlrserver;port=9000;socket=/tmp/example.socket;tries=0;retrytime=1;debug=0","exampleuser","examplepassword");
$stmt=$dbh->query("select * from my_table");

... do some stuff that takes a short time ...

$stmt=$dbh->query("select * from another_table");
$dbh->endSession();

... do some stuff that takes a long time ...

$stmt=$dbh->query("select * from yet_another_table");
$dbh->endSession();

... process the result set ...
