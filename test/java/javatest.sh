#!/bin/sh

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/firstworks/java/com/firstworks/sqlrelay
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/java/com/firstworks/sqlrelay
export LD_LIBRARY_PATH
CLASSPATH=$CLASSPATH:/usr/local/firstworks/java:./
#CLASSPATH=$CLASSPATH:/usr/java:./
export CLASSPATH
#java db2 localhost 8001 "" db2test db2test
#java freetds localhost 8002 "" freetdstest freetdstest
#java interbase localhost 8003 "" interbasetest interbasetest
#java lago localhost 8004 "" lagotest lagotest
#java msql localhost 8005 "" msqltest msqltest
#java mysql localhost 8006 "" mysqltest mysqltest
#java oracle7 localhost 8008 "" oracle7test oracle7test
#java oracle8 localhost 8009 "" oracle8test oracle8test
#java oracle8i localhost 8009 "" oracle8test oracle8test
java postgresql6 localhost 8010 "" postgresqltest postgresqltest
#java postgresql7 localhost 8010 "" postgresqltest postgresqltest
#java sqlite localhost 8011 "" sqlitetest sqlitetest
#java sybase localhost 8012 "" sybasetest sybasetest

rm -f hs_* cachefile* sqlnet.log
