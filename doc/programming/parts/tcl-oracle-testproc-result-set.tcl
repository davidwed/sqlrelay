$cur prepareQuery "begin  :curs:=exampleproc; end;"
$cur defineOutputBindCursor "curs"
$cur executeQuery
set bindcur [$cur getOutputBindCursor "curs"]
$bindcur fetchFromBindCursor
set $field00 [$bindcur getField 0 0]
set $field01 [$bindcur getField 0 1]
set $field02 [$bindcur getField 0 2]
set $field10 [$bindcur getField 1 0]
set $field11 [$bindcur getField 1 1]
set $field12 [$bindcur getField 1 2]
