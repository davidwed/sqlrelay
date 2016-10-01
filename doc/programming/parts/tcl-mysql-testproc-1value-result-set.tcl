$cur sendQuery "select testfunc()"
set $result [$cur getField 0 0]
