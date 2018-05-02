load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

$cur sendQuery "select * from my_table"
$con endSession

for {set row 0} {$row<[$cur rowCount]} {incr row} {
        for {set col 0} {$col<[$cur colCount]} {incr col} {
                puts [$cur getFieldByIndex $row $col]
        }
}
