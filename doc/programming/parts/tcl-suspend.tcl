load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -user "user" -password "password"]
set cur [$con sqlrcur]

$cur sendQuery "insert into my_table values (1,2,3)"
$cur suspendResultSet
$con suspendSession
set $rs [$cur getResultSetId]
set $port [$cur getConnectionPort]
set $cusocket [$cur getConnectionSocket]

        ... pass the rs, port and socket to the next page ...
