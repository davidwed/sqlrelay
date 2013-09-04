<html>
<body>
<?php

dl("sql_relay.so");

$con=sqlrcon_alloc("examplehost",9000,
			"/tmp/example.socket",
			"exampleuser",
			"examplepassword",0,1);
$cur=sqlrcur_alloc($con);

sqlrcur_sendQuery($cur,"select * from exampletable");
for ($row=0; $row<sqlrcur_rowCount($cur); $row++) {
	for ($col=0; $col<sqlrcur_colCount($cur); $col++) {
		echo(sqlrcur_getField($cur,$row,$col).",");
	}
	echo("<br>\n");
}

?>
</body>
</html>
