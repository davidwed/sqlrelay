$cur prepareQuery "select * from exampleproc(?,?,?)"
$cur inputBind "in1" 1
$cur inputBind "in2" 1.1 4 2
$cur inputBind "in3" "hello"
$cur executeQuery
set out1 [$cur getField 0 0]
set out2 [$cur getField 0 1]
set out3 [$cur getField 0 2]
