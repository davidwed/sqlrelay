#!/bin/sh

# db2
echo "db2..."
echo "	db2as..."
su -l db2as -c "db2start"
echo "	testinst..."
su -l testinst -c "db2start"

# interbase
echo "interbase..."
echo "	sudo..."
sudo /opt/interbase/bin/ibserver&

# lago
echo "lago..."
/usr/local/lago/bin/lago &

# msql
echo "msql..."
/usr/local/Hughes/bin/msql2d &

# mysql
echo "mysql..."
echo "	sudo..."
sudo /etc/rc.d/init.d/mysqld start

# oracle
echo "oracle..."
echo "	oracle..."
su -l oracle -c "dbstart; lsnrctl start"

# postgresql
echo "postgresql..."
echo "	sudo..."
sudo /etc/rc.d/init.d/postgresql start

# sybase
echo "sybase..."
echo "	sudo..."
sudo /etc/rc.d/init.d/sybase start
