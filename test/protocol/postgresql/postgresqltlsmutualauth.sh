#!/bin/sh

# run with postgresqlprotocoltlstest instance


PGPASSFILE=`pwd`/pgpass \
PGSSLMODE=verify-ca \
PGSSLCERT=/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem \
PGSSLKEY=/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem \
PGSSLROOTCERT=/usr/local/firstworks/etc/sqlrelay.conf.d/ca.pem \
psql \
	-h sqlrelay \
	-U testuser \
	-w \
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
