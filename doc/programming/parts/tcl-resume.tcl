load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

        ... get rs, port and socket from previous page ...

set con [sqlrcon -server "host" -port 9000 -user "user" -password "password"]
set cur [$con sqlrcur]

$con resumeSession $port $socket
$cur resumeResultSet $rs
$cur sendQuery "commit"
$con endSession
