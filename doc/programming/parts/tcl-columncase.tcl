load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

# column names will be forced to upper case
$cur upperCaseColumnNames
$cur sendQuery "select * from my_table"
$con endSession

for {set i 0} {$i<[$cur colCount]} {incr i} {
        puts -nonewline "Name:          "
        puts [$cur getColumnName $i]
}

# column names will be forced to lower case
$cur lowerCaseColumnNames
$cur sendQuery "select * from my_table"
$con endSession

for {set i 0} {$i<[$cur colCount]} {incr i} {
        puts -nonewline "Name:          "
        puts [$cur getColumnName $i]
}

# column names will be the same as they are in the database
$cur mixedCaseColumnNames
$cur sendQuery "select * from my_table"
$con endSession

for {set i 0} {$i<[$cur colCount]} {incr i} {
        puts -nonewline "Name:          "
        puts [$cur getColumnName $i]
}
