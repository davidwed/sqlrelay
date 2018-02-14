$cur prepareQuery "execute procedure exampleproc ?, ?, ?"
$cur inputBind "1" 1
$cur inputBind "2" 1.1 2 1
$cur inputBind "3" "hello"
$cur defineOutputBindInteger "1" 20
$cur defineOutputBindDouble "2" 20
$cur defineOutputBindString "3" 20
$cur executeQuery
set out1 [$cur getOutputBindInteger "1"]
set out2 [$cur getOutputBindDouble "2"]
set out3 [$cur getOutputBindString "3"]
