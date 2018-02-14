load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

$cur prepareQuery "select * from mytable where mycolumn>:value"
$cur inputBind "value" 1
$cur executeQuery

... process the result set ...

$cur clearBinds
$cur inputBind "value" 5
$cur executeQuery

... process the result set ...

$cur clearBinds
$cur inputBind "value" 10
$cur executeQuery

... process the result set ...
