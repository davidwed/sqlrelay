#!/bin/sh

# db2
echo "db2..."
echo "	db2as..."
su -l db2as -c "db2stop"
echo "	testinst..."
su -l testinst -c "db2stop"

# interbase
echo "interbase..."
sudo killall ibserver

# lago
echo "lago..."
killall lago

# msql
echo "msql..."
killall msql2d

# mysql
echo "mysql..."
echo "	sudo..."
sudo /etc/rc.d/init.d/mysqld stop

# oracle
echo "oracle..."
echo "	oracle..."
su -l oracle -c "dbshut; lsnrctl stop"

# postgresql
echo "postgresql..."
echo "	sudo..."
sudo /etc/rc.d/init.d/postgresql stop

# sybase
echo "sybase..."
echo "	sudo..."
sudo /etc/rc.d/init.d/sybase stop
