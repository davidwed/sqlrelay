#!/bin/sh

# run with mysqlprotocoltest instance

mysql \
	-h sqlrelay \
	-u testuser \
	-ptestpassword \
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
