<?
        $con=sqlrcon_alloc("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
        $cur=sqlrcur_alloc($con);

        sqlrcur_prepareQuery($cur,"begin  :curs:=sp_mytable; end;");
        sqlrcur_defineOutputBindCursor($cur,"curs");
        sqlrcur_executeQuery($cur);

        sqlrcur         bindcur=sqlrcur_getOutputBindCursor($cur,"curs");
        sqlrcur_fetchFromBindCursor($bindcur);

        # print fields from table
        for ($i=0; $i<sqlrcur_rowCount($bindcur); $i++) {
                for ($j=0; $j<sqlrcur_colCount($bindcur); $j++) {
                        echo sqlrcur_getField($bindcur,$i,$j);
                        echo ", ";
                }
                echo "\n";
        }

        sqlrcur_free($bindcur);

        sqlrcur_free($cur);
        sqlrcon_free($con);
?>
