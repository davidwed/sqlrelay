#!/bin/sh

sudo /etc/rc.d/init.d/httpd start
sleep 1

#chmod 755 db2.php
#sudo cp db2.php /var/www/html
#lynx -dump "http://localhost/db2.php?host=localhost&port=8001&socket=&user=db2test&password=db2test"

#chmod 755 freetds.php
#sudo cp freetds.php /var/www/html
#lynx -dump "http://localhost/freetds.php?host=localhost&port=8002&socket=&user=freetdstest&password=freetdstest"

#chmod 755 interbase.php
#sudo cp interbase.php /var/www/html
#lynx -dump "http://localhost/interbase.php?host=localhost&port=8003&socket=&user=interbasetest&password=interbasetest"

#chmod 755 lago.php
#sudo cp lago.php /var/www/html
#lynx -dump "http://localhost/lago.php?host=localhost&port=8004&socket=&user=lagotest&password=lagotest"

#chmod 755 msql.php
#sudo cp msql.php /var/www/html
#lynx -dump "http://localhost/msql.php?host=localhost&port=8005&socket=&user=msqltest&password=msqltest"

#chmod 755 mysql.php
#sudo cp mysql.php /var/www/html
#lynx -dump "http://localhost/mysql.php?host=localhost&port=8006&socket=&user=mysqltest&password=mysqltest"

#chmod 755 oracle7.php
#sudo cp oracle7.php /var/www/html
#lynx -dump "http://localhost/oracle7.php?host=localhost&port=8008&socket=&user=oracle7test&password=oracle7test"

#chmod 755 oracle8.php
#sudo cp oracle8.php /var/www/html
#lynx -dump "http://localhost/oracle8.php?host=localhost&port=8009&socket=&user=oracle8test&password=oracle8test"

#chmod 755 oracle8i.php
#sudo cp oracle8i.php /var/www/html
#lynx -dump "http://localhost/oracle8i.php?host=localhost&port=8009&socket=&user=oracle8test&password=oracle8test"

chmod 755 postgresql.php
sudo cp postgresql.php /var/www/html
lynx -dump "http://localhost/postgresql.php?host=localhost&port=8010&socket=&user=postgresqltest&password=postgresqltest"

#chmod 755 sqlite.php
#sudo cp sqlite.php /var/www/html
#lynx -dump "http://localhost/sqlite.php?host=localhost&port=8011&socket=&user=sqlitetest&password=sqlitetest"

#chmod 755 sybase.php
#sudo cp sybase.php /var/www/html
#lynx -dump "http://localhost/sybase.php?host=localhost&port=8012&socket=&user=sybasetest&password=sybasetest"

sudo /etc/rc.d/init.d/httpd stop

sudo rm -f /tmp/cachefile* sqlnet.log
