#!/bin/sh

sqlr-stop
sleep 2

for DB in db2 firebird freetds informix mysql oracle postgresql sap sqlite tls krb extensions
do
	if ( test -z "`ls /usr/local/firstworks/lib*/sqlrelay/sqlrconnection_$DB.so`" )
	then
		echo "skipping database $DB..."
		echo
		echo "================================================================================"
		echo
		continue
	fi

	echo "testing database $DB..."

	sqlr-start -id ${DB}test
	sleep 2

	UP=""
	for attempts in 1 2 3 4 5 6 7 8 9 10
	do
		echo
		echo "pinging $DB..."
		if ( test "$DB" = "tls" )
		then
			PING=`sqlrsh -host localhost -tls -tlscert /usr/local/firstworks/etc/client.pem -tlsca /usr/local/firstworks/etc/ca.pem -tlsvalidate ca -command ping`
		elif ( test "$DB" = "extensions" )
		then
			PING=`sqlrsh -host localhost -user test -password test -command ping`
		else
			PING=`sqlrsh -id ${DB}test -command ping`
		fi
		echo $PING

		UP=`echo "$PING" | grep "The database is up"`

		if ( test -n "$UP" )
		then
			break
		else
			sleep 2
		fi
	done

	if ( test -n "$UP" )
	then
		echo
		echo "success..."
		echo
		./test.sh $DB
	else
		echo
		echo "failed to start ${DB}test"
		read
	fi

	sleep 2
	sqlr-stop -id ${DB}test
	sleep 2

	echo
	echo "================================================================================"
	echo
done
