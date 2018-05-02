$cur prepareQuery "call exampleproc(?,?,?,?,?,?)"
$cur inputBind "1" 1
$cur inputBind "2" 1.1 2 1
$cur inputBind "3" "hello"
$cur defineOutputBindInteger "4" 25
$cur defineOutputBindDouble "5" 25
$cur defineOutputBindString "6" 25
$cur executeQuery
set out1 [$cur getOutputBindInteger "4"]
set out2 [$cur getOutputBindDouble "5"]
set out3 [$cur getOutputBindString "6"]
