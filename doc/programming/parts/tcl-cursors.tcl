load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cursor1 [$con sqlrcur]
set cursor2 [$con sqlrcur]

$cursor1 setResultSetBufferSize 10
$cursor1 sendQuery "select * from my_huge_table"

set $index 0
while {[$cursor1 endOfResultSet] != 0} {
        $cursor2 prepareQuery "insert into my_other_table values (:1,:2,:3)"
        $cursor2 inputBind "1" [$cursor1 getField $index 1]
        $cursor2 inputBind "2" [$cursor1 getField $index 2]
        $cursor2 inputBind "3" [$cursor1 getField $index 3]
        $cursor2 executeQuery
}
