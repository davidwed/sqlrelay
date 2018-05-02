load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

$cur prepareQuery "select * from mytable $(whereclause)"
$cur substitution "whereclause" "where stringcol=:stringval and integercol>:integerval and floatcol>floatval"
$cur inputBind "stringval" "true"
$cur inputBind "integerval" 10
$cur inputBind "floatval" 1.1 2 1
$cur executeQuery

... process the result set ...
