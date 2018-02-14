load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

catch {$cur sendQuery "select * from my_table"} error

if {$error != 1} {
        puts $error
}
