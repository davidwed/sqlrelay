#!/bin/sh

# This is mainly for OSX, where gcc likes to build modules with a .so extension
# but they should have a .bundle extension.

MODULE=$1
SOSUFFIX=$2
MODULESUFFIX=$3

# exit silently if the module isn't found
if ( test ! -r "$MODULE" )
then
	exit 0
fi

# exit silently if the module and so suffixes are the same
if ( test "$SOSUFFIX" = "$MODULESUFFIX" )
then
	exit 0
fi

# rename the module
BASENAME="`basename $MODULE $SOSUFFIX`"
DIRNAME="`dirname $MODULE`"
mv $MODULE $DIRNAME/$BASENAME$MODULESUFFIX
