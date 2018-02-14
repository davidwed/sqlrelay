load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

$cur setResultSetBufferSize 2

catch {$cur sendQuery "select * from exampletable"}

set row 0
while {true} {
        for {set col 0} {$col<[$cur colCount]} {incr col} {
                puts -nonewline [$cur getFieldByIndex $row $col]
                puts -nonewline ","
        }
        puts ""
        incr row
        if {[$cur endOfResultSet]==1 && $row>=[$cur rowCount]} {
                break
        }
}
