#!/bin/sh

#./db2.pl localhost 8001 "" db2test db2test
#./freetds.pl localhost 8002 "" freetdstest freetdstest
#./interbase.pl localhost 8003 "" interbasetest interbasetest
#./lago.pl localhost 8004 "" lagotest lagotest
#./msql.pl localhost 8005 "" msqltest msqltest
#./mysql.pl localhost 8006 "" mysqltest mysqltest
#./oracle7.pl localhost 8008 "" oracle7test oracle7test
./oracle8.pl localhost 8009 "" oracle8test oracle8test
#./postgresql.pl localhost 8010 "" postgresqltest postgresqltest
#./sqlite.pl localhost 8011 "" sqlitetest sqlitetest
#./sybase.pl localhost 8012 "" sybasetest sybasetest

rm -f cachefile* sqlnet.log
