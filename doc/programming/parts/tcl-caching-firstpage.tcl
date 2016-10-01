load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -user "user" -password "password"]
set cur [$con sqlrcur]

        ... generate a unique file name ...

$cur cacheToFile $filename
$cur setCacheTtl 600
$cur sendQuery "select * from my_table"
$con endSession
$cur cacheOff

        ... pass the filename to the next page ...
