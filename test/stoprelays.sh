#!/bin/sh

# db2
echo "db2..."
echo "	testinst..."
su -l testinst -c "sqlr-stop db2test"

# freetds
echo "freetds..."
sqlr-stop freetdstest

# interbase
echo "interbase..."
sqlr-stop interbasetest

# lago
echo "lago..."
sqlr-stop lagotest

# msql
echo "msql..."
sqlr-stop msqltest

# mysql
echo "mysql..."
echo "	sudo..."
sqlr-stop mysqltest

# oracle
echo "oracle..."
sqlr-stop oracle7test
sqlr-stop oracle8test

# postgresql
echo "postgresql..."
sqlr-stop postgresqltest

#sqlite
echo "sqlite..."
sqlr-stop sqlitetest

# sybase
echo "sybase..."
sqlr-stop sybasetest

echo "sudo..."
sudo killall sqlr-cachemanager
