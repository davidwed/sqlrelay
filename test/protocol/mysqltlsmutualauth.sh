#!/bin/sh

# run with mysqlprotocoltlsmutualauthtest instance

mysql \
	-h sqlrelay \
	-u testuser \
	-ptestpassword \
	--ssl-cert=/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem \
	--ssl-ca=/usr/local/firstworks/etc/sqlrelay.conf.d/ca.pem \
> /dev/null << EOF
select 1
EOF

if ( test "$?" = "0" )
then
	echo success
	exit 0
else
	echo failed
	exit 1
fi
