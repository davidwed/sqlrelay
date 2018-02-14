$cur prepareQuery "exec exampleproc"
$cur inputBind "in1" 1
$cur inputBind "in2" 1.1 2 1
$cur inputBind "in3" "hello"
$cur executeQuery
