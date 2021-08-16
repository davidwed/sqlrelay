#!/bin/sh

# run with mysqlprotocoltlstest instance

mysql \
	-h sqlrelay \
	-u testuser \
	-ptestpassword \
	--ssl \
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
