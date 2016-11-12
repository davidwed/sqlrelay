#!/bin/sh

sqlr-stop
sleep 2

for DB in db2 firebird freetds informix mysql oracle postgresql sap sqlite tls krb extensions
do

	if ( test "$DB" = "tls" -o "$DB" = "krb" -o "$DB" = "extensions" )
	then
		if ( test -z "`ls /usr/local/firstworks/lib*/sqlrelay/sqlrconnection_oracle.so 2> /dev/null`" )
		then
			echo "skipping $DB..."
			echo
			echo "================================================================================"
			echo
			continue
		fi
	else
		if ( test -z "`ls /usr/local/firstworks/lib*/sqlrelay/sqlrconnection_$DB.so 2> /dev/null`" )
		then
			echo "skipping $DB..."
			echo
			echo "================================================================================"
			echo
			continue
		fi
	fi

	echo "testing $DB..."

	sqlr-start -id ${DB}test
	sleep 2

	PING=""
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

		if ( test "$PING" = "0: Couldn't connect to the listener" )
		then
			sleep 2
		else
			break
		fi
	done

	if ( test "$PING" = "        The database is up." )
	then
		echo
		echo "success..."
		echo
		./test.sh $DB
	else
		echo
		echo "failed to start ${DB}test"
		echo
		echo "hit enter to continue or ctrl-c to stop..."
		read
	fi

	sleep 2
	sqlr-stop -id ${DB}test
	sleep 2

	echo
	echo "================================================================================"
	echo
done
