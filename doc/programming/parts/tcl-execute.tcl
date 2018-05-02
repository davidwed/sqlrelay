load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

catch {$cur sendQuery "select * from my_table"}

... do some stuff that takes a short time ...

catch {$cur sendFileQuery "/usr/local/myprogram/sql" "myquery.sql"}
$con endSession

... do some stuff that takes a long time ...

catch {$cur sendQuery "select * from my_other_table"}
$con endSession

... process the result set ...
