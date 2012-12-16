#!/bin/sh

MODULE=$1
SOSUFFIX=$2
MODULESUFFIX=$3

# exit if the module and so suffix are the same
if ( test "$SOSUFFIX" = "$MODULESUFFIX" )
then
	exit 0
fi

BASENAME="`basename $MODULE $SOSUFFIX`"
DIRNAME="`dirname $MODULE`"
echo mv $MODULE $DIRNAME/$BASENAME$MODULESUFFIX
