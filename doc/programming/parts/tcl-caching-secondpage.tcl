load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

        ... get the filename from the previous page ...

        ... get the page to display from the previous page ...

set con [sqlrcon -server "host" -port 9000 -user "user" -password "password"]
set cur [$con sqlrcur]

$cur openCachedResultSet $filename
$con endSession

for {set row [expr $pagetodisplay * 20]} {$row<[expr [expr $pagetodisplay + 1] * 20]} {incr row} {
        for {set col 0} {$col<[$cur colCount]} {incr col} {
                puts -nonewline $cur getField $row $col
                puts -nonewline ","
        }
        puts ""
}
