#!/bin/sh

sqlr-stop
sleep 2

#for DB in firebird freetds informix mysql oracle postgresql sap sqlite db2
for DB in informix mysql oracle postgresql sap sqlite db2
do

	echo "benching $DB..."

	sqlr-start -id ${DB}test
	sleep 2

	PING=""
	for attempts in 1 2 3 4 5 6 7 8 9 10
	do
		echo
		echo "pinging $DB..."
		PING=`sqlrsh -id ${DB}test -command ping`

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
		echo
		echo "success..."
		echo
		./sqlr-bench -db $DB
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
