#!/bin/sh

sqlr-stop
sleep 2

for DB in firebird freetds informix mysql oracle postgresql sap sqlite tls krb extensions router db2
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

	if ( test "$DB" = "router" )
	then
		sqlr-start -id routermaster
		sleep 2

		sqlr-start -id routerslave
		sleep 2
	fi

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

		# (this collapses whitespace)
		PING=`echo $PING`

		echo $PING

		if ( test "$PING" = "0: Couldn't connect to the listener." )
		then
			sleep 2
		else
			break
		fi
	done

	if ( test "$PING" = "The database is up." )
	then
		if ( test "$DB" = "krb" )
		then
			kdestroy
			kinit
		fi

		echo
		echo "success..."
		echo
		./test.sh $DB
	else
		echo
		echo "failed to start ${DB}test"
		echo
		echo "hit enter to continue or ctrl-c to stop..."
		if ( test "$DB" = "krb" )
		then
			read -t 20
		else
			read
		fi
	fi

	if ( test "$DB" = "router" )
	then
		sleep 2
		sqlr-stop -id routermaster
		sleep 2
		sqlr-stop -id routerslave
	fi

	sleep 2
	sqlr-stop -id ${DB}test
	sleep 2

	echo
	echo "================================================================================"
	echo
done
