load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

$cur prepareQuery "begin  :curs:=sp_mytable; end;"
$cur defineOutputBindCursor "curs"
$cur executeQuery

set bindcur [$cur getOutputBindCursor "curs"]
$bindcur fetchFromBindCursor

# print fields from table
for {set i 0} {$i<[$bindcur rowCount]} {incr i} {
        for {set j 0} {$j<[$bindcur colCount]} {incr j} {
                puts [$bindcur getField $i $j]
        }
}
