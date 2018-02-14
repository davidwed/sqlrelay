<?
     $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
     $cur=sqlrcur_alloc($con);

     sqlrcur_sendQuery($cur,"select * from my_table");
     sqlrcon_endSession($con);

     for ($i=0; $i<sqlrcur_colCount($cur); $i++) {
             echo "Name: ";
             echo sqlrcur_getColumnName($cur,$i);
             echo "\n";
             echo "Type: ";
             echo sqlrcur_getColumnType($cur,$i);
             echo "\n";
             echo "Length: ";
             echo sqlrcur_getColumnLength($cur,$i);
             echo "\n";
             echo "Precision: ";
             echo sqlrcur_getColumnPrecision($cur,$i);
             echo "\n";
             echo "Scale: ";
             echo sqlrcur_getColumnScale($cur,$i);
             echo "\n";
             echo "Longest Field: ";
             echo sqlrcur_getLongest($cur,$i));
             echo "\n";
             echo "Nullable: ";
             echo sqlrcur_getColumnIsNullable($cur,$i);
             echo "\n";
             echo "Primary Key: ";
             echo sqlrcur_getColumnIsPrimaryKey($cur,$i);
             echo "\n";
             echo "Unique: ";
             echo sqlrcur_getColumnIsUnique($cur,$i);
             echo "\n";
             echo "Part Of Key: ";
             echo sqlrcur_getColumnIsPartOfKey($cur,$i);
             echo "\n";
             echo "Unsigned: ";
             echo sqlrcur_getColumnIsUnsigned($cur,$i);
             echo "\n";
             echo "Zero Filled: ";
             echo sqlrcur_getColumnIsZeroFilled($cur,$i);
             echo "\n";
             echo "Binary: ";
             echo sqlrcur_getColumnIsBinary($cur,$i);
             echo "\n";
             echo "Auto Increment: ";
             echo sqlrcur_getColumnIsAutoIncrement($cur,$i);
             echo "\n\n";
     }

     sqlrcur_free($cur);
     sqlrcon_free($con);
?>
