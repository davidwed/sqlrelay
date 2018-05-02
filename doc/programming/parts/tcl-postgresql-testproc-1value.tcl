$cur prepareQuery "select * from examplefunc($1,$2,$3)"
$cur inputBind "1" 1
$cur inputBind "2" 1.1 4 2
$cur inputBind "3" "hello"
$cur executeQuery
set result [$cur getField 0 0]
