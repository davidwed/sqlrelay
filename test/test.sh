#!/bin/sh

if ( test -z "$1" )
then
	echo "usage: test database"
	exit
fi

for DIR in c c++ java nodejs perl perldbi php phppdo python pythondb ruby tcl
do
	echo "testing in $DIR"
	echo
	cd $DIR

	TEST=""
	TESTFILE=""
	case "$DIR" in
		c)
			TEST="./$1"
			TESTFILE="$1"
			;;
		c++)
			TEST="./$1"
			TESTFILE="$1"
			;;
		cs)
			TEST="mono $1.exe"
			TESTFILE="$1.exe"
			;;
		java)
			TEST="./run $1"
			TESTFILE="$1.class"
			;;
		nodejs)
			TEST="node $1.js"
			TESTFILE="$1.js"
			;;
		perl)
			TEST="perl $1.pl"
			TESTFILE="$1.pl"
			;;
		perldbi)
			TEST="perl $1.pl"
			TESTFILE="$1.pl"
			;;
		php)
			TEST="php $1.php"
			TESTFILE="$1.php"
 			;;
		phppdo)
			TEST="php $1.php"
			TESTFILE="$1.php"
			;;
		python)
			TEST="python $1.py"
			TESTFILE="$1.py"
			;;
		pythondb)
			TEST="python $1.py"
			TESTFILE="$1.py"
			;;
		ruby)
			TEST="ruby $1.rb"
			TESTFILE="$1.rb"
			;;
		tcl)
			TEST="tclsh $1.tcl"
			TESTFILE="$1.tcl"
			;;
	esac


	if ( test -r "$TESTFILE" )
	then
		$TEST
		if ( test "$?" = "1" )
		then
			echo
			echo
			echo "$1 failed in $DIR"
			echo
			echo "hit enter to continue or ctrl-c to stop..."
			read
		else
			echo
			echo "test complete"
		fi
	else
		echo "no test found for $1 in $DIR"
	fi

	echo
	echo "================================================================================"
	echo

	cd ..
done
