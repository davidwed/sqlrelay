#!/bin/sh

#./db2.rb localhost 8001 "" db2test db2test
./freetds.rb localhost 8002 "" freetdstest freetdstest
#./interbase.rb localhost 8003 "" interbasetest interbasetest
#./lago.rb localhost 8004 "" lagotest lagotest
#./msql.rb localhost 8005 "" msqltest msqltest
#./mysql.rb localhost 8006 "" mysqltest mysqltest
#./oracle7.rb localhost 8008 "" oracle7test oracle7test
#./oracle8.rb localhost 8009 "" oracle8test oracle8test
#./postgresql.rb localhost 8010 "" postgresqltest postgresqltest
#./sqlite.rb localhost 8011 "" sqlitetest sqlitetest
#./sybase.rb localhost 8012 "" sybasetest sybasetest

rm -f cachefile* sqlnet.log
