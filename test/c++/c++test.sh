#!/bin/sh

./db2 localhost 8001 "" db2test db2test
#./freetds localhost 8002 "" freetdstest freetdstest
#./interbase localhost 8003 "" interbasetest interbasetest
#./lago localhost 8004 "" lagotest lagotest
#./msql localhost 8005 "" msqltest msqltest
#./mysql localhost 8006 "" mysqltest mysqltest
#./oracle7 localhost 8008 "" oracle7test oracle7test
#./oracle8 localhost 8009 "" oracle8test oracle8test
#./postgresql localhost 8010 "" postgresqltest postgresqltest
#./sqlite localhost 8011 "" sqlitetest sqlitetest
#./sybase localhost 8012 "" sybasetest sybasetest

rm -f cachefile* sqlnet.log
