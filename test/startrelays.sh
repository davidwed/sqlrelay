#!/bin/sh

# db2
echo "db2..."
echo "	testinst..."
su -l testinst -c "sqlr-start -id db2test"

# freetds
echo "freetds..."
sqlr-start -id freetdstest

# interbase
echo "interbase..."
sqlr-start -id interbasetest

# lago
echo "lago..."
sqlr-start -id lagotest

# msql
echo "msql..."
sqlr-start -id msqltest

# mysql
echo "mysql..."
echo "	sudo..."
sqlr-start -id mysqltest

# oracle
echo "oracle..."
sqlr-start -id oracle7test
sqlr-start -id oracle8test

# postgresql
echo "postgresql..."
sqlr-start -id postgresqltest

#sqlite
echo "sqlite..."
sqlr-start -id sqlitetest

# sybase
echo "sybase..."
sqlr-start -id sybasetest
