$cur sendQuery "set @out1=0"
$cur sendQuery "call testproc()"
$cur sendQuery "select @out1"
set $result [$cur getField 0 0]
