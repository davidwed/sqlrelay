#!/bin/sh

#./db2.py localhost 8001 "" db2test db2test
#./freetds.py localhost 8002 "" freetdstest freetdstest
#./interbase.py localhost 8003 "" interbasetest interbasetest
#./lago.py localhost 8004 "" lagotest lagotest
#./msql.py localhost 8005 "" msqltest msqltest
#./mysql.py localhost 8006 "" mysqltest mysqltest
#./oracle7.py localhost 8008 "" oracle7test oracle7test
#./oracle8.py localhost 8009 "" oracle8test oracle8test
#./oracle8i.py localhost 8009 "" oracle8test oracle8test
./postgresql7.py localhost 8010 "" postgresqltest postgresqltest
#./postgresql6.py localhost 8010 "" postgresqltest postgresqltest
#./sqlite.py localhost 8011 "" sqlitetest sqlitetest
#./sybase.py localhost 8012 "" sybasetest sybasetest

rm -f cachefile* sqlnet.log
