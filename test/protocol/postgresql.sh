#!/bin/sh

# run with postgresqlprotocoltest instance


PGPASSFILE=`pwd`/pgpass \
PGSSLMODE=disable \
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
