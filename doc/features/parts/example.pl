#!/usr/bin/perl

use SQLRelay::Connection;
use SQLRelay::Cursor;

$sqlrcon=SQLRelay::Connection->new("examplehost",9000,
					"/tmp/example.socket",
					"exampleuser",
					"examplepassword",0,1);
$sqlrcur=SQLRelay::Cursor->new($sqlrcon);

$sqlrcur->sendQuery("select * from exampletable");
for ($row=0; $row<$sqlrcur->rowCount(); $row++) {
	for ($col=0; $col<$sqlrcur->colCount(); $col++) {
		print($sqlrcur->getField($row,$col).",");
	}
	print("\n");
}
