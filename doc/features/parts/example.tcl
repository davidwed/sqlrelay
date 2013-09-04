#!/usr/bin/tclsh

load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "examplehost"
			-port 9000
			-socket "/tmp/example.socket"
			-user "exampleuser"
			-password "examplepassword"
			-retrytime 0 -tries 1]
set cur [$con sqlrcur]

$cur sendQuery "select * from exampletable"
for {set row 0} {$row<[$cur rowCount]} {incr row} {
	for {set col 0} {$col<[$cur colCount]} {incr col} {
		puts -nonewline [$cur getFieldByIndex $row $col]
		puts -nonewline ","
	}
	puts ""
}
