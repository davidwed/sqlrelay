#!/bin/sh

if ( test -z "$1" )
then
	echo "usage: test database"
	exit
fi

for DIR in  c c++ perl perldbi python pythondb ruby php phppdo java tcl cs nodejs
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
			mono --version > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="mono $1.exe"
				TESTFILE="$1.exe"
			fi
			;;
		java)
			java -h > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="./run $1"
				TESTFILE="$1.class"
			fi
			;;
		nodejs)
			node --version > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="node $1.js"
				TESTFILE="$1.js"
			fi
			;;
		perl)
			perl --version > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="perl $1.pl"
				TESTFILE="$1.pl"
			fi
			;;
		perldbi)
			perl --version > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="perl $1.pl"
				TESTFILE="$1.pl"
			fi
			;;
		php)
			php --version > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="php $1.php"
				TESTFILE="$1.php"
			fi
 			;;
		phppdo)
			php --version > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="php $1.php"
				TESTFILE="$1.php"
			fi
			;;
		python)
			python --version > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="python $1.py"
				TESTFILE="$1.py"
			fi
			;;
		pythondb)
			python --version > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="python $1.py"
				TESTFILE="$1.py"
			fi
			;;
		ruby)
			ruby --version > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="ruby $1.rb"
				TESTFILE="$1.rb"
			fi
			;;
		tcl)
			echo 'puts $tcl_version;exit 0'|tclsh > /dev/null 2>&1
			if ( test "$?" = "0" )
			then
				TEST="tclsh $1.tcl"
				TESTFILE="$1.tcl"
			fi
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
			read PROMPT
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
