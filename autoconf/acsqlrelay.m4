AC_DEFUN([FW_CHECK_SQLRELAY],
[

if ( test -z "$SQLR" )
then
	SQLR=sqlr
fi

if ( test -n "$SQLRELAYPREFIX" )
then
	SQLRSERVERCONFIG="$SQLRELAYPREFIX/bin/${SQLR}server-config"
	SQLRCLIENTCONFIG="$SQLRELAYPREFIX/bin/${SQLR}client-config"
else
	AC_PATH_PROG(SQLRSERVERCONFIG,${SQLR}server-config,"","/bin:/usr/bin:/usr/local/bin:/opt/sfw/bin:/usr/pkg/bin:/sw/bin:/usr/local/firstworks/bin:/boot/common/bin:/resources/index/bin:/resources/firstworks/bin")
	AC_PATH_PROG(SQLRCLIENTCONFIG,${SQLR}client-config,"","/bin:/usr/bin:/usr/local/bin:/opt/sfw/bin:/usr/pkg/bin:/sw/bin:/usr/local/firstworks/bin:/boot/common/bin:/resources/index/bin:/resources/firstworks/bin")
fi

SQLRSERVERVERSION="`$SQLRSERVERCONFIG --version 2> /dev/null`"
SQLRSERVERCFLAGS="`$SQLRSERVERCONFIG --cflags 2> /dev/null`"
SQLRSERVERLIBS="`$SQLRSERVERCONFIG --libs 2> /dev/null`"
if ( test -z "$SQLRSERVERLIBS" )
then
	AC_MSG_ERROR("${SQLR}server-config not found.")
fi

SQLRCLIENTVERSION="`$SQLRCLIENTCONFIG --version 2> /dev/null`"
SQLRCLIENTCFLAGS="`$SQLRCLIENTCONFIG --cflags 2> /dev/null`"
SQLRCLIENTLIBS="`$SQLRCLIENTCONFIG --libs 2> /dev/null`"
if ( test -z "$SQLRCLIENTLIBS" )
then
	AC_MSG_ERROR("${SQLR}client-config not found.")
fi

AC_SUBST(SQLRSERVERCFLAGS)
AC_SUBST(SQLRSERVERLIBS)
AC_SUBST(SQLRCLIENTCFLAGS)
AC_SUBST(SQLRCLIENTLIBS)
])
