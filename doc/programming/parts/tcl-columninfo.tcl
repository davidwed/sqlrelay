load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

$cur sendQuery "select * from my_table"
$con endSession

for {set i 0} {$i<[$cur colCount]} {incr i} {
        puts -nonewline "Name:          "
        puts [$cur getColumnName $i]
        puts -nonewline "Type:          "
        puts [$cur getColumnTypeByIndex $i]
        puts -nonewline "Length:        "
        puts [$cur getColumnLengthByIndex $i]
        puts -nonewline "Precision:     "
        puts [$cur getColumnPrecisionByIndex $i]
        puts -nonewline "Scale:         "
        puts [$cur getColumnScaleByIndex $i]
        puts -nonewline "Longest Field: "
        puts [$cur getLongestByIndex $i]
        puts -nonewline "Nullable:      "
        puts [$cur getColumnIsNullableByIndex $i]
        puts -nonewline "Primary Key:   "
        puts [$cur getColumnIsPrimaryKeyByIndex $i]
        puts -nonewline "Unique:        "
        puts [$cur getColumnIsUniqueByIndex $i]
        puts -nonewline "Part Of Key:   "
        puts [$cur getColumnIsPartOfKeyByIndex $i]
        puts -nonewline "Unsigned:      "
        puts [$cur getColumnIsUnsignedByIndex $i]
        puts -nonewline "Zero Filled:   "
        puts [$cur getColumnIsZeroFilledByIndex $i]
        puts -nonewline "Binary:        "
        puts [$cur getColumnIsBinaryByIndex $i]
        puts -nonewline "Auto Increment:"
        puts [$cur getColumnIsAutoIncrementByIndex $i]
}
