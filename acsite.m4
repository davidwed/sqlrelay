AC_DEFUN([FW_TRY_LINK],
[
SAVECPPFLAGS="$CPPFLAGS"
SAVELIBS="$LIBS"
SAVE_LD_LIBRARY_PATH="$LD_LIBRARY_PATH"
CPPFLAGS="$3"
LIBS="$4"
LD_LIBRARY_PATH="$5"
export LD_LIBRARY_PATH
AC_TRY_LINK([$1],[$2],[$6],[$7])
CPPFLAGS="$SAVECPPFLAGS"
LIBS="$SAVELIBS"
LD_LIBRARY_PATH="$SAVE_LD_LIBRARY_PATH"
export LD_LIBRARY_PATH
])


AC_DEFUN([FW_CHECK_LIB],
[
FOUNDLIB=""
AC_CHECK_FILE($1, FOUNDLIB="yes")
if ( test -n "$FOUNDLIB" )
then
	eval "$2"
else
	if ( test -n "$3" )
	then
		AC_CHECK_FILE($3, FOUNDLIB="yes")
		if ( test -n "$FOUNDLIB" )
		then
			eval "$4"
		fi
	fi
fi
])


AC_DEFUN([FW_CHECK_HEADER_LIB],
[
FOUNDHEADER=""
FOUNDLIB=""
AC_CHECK_FILE($1, FOUNDHEADER="yes")
AC_CHECK_FILE($3, FOUNDLIB="yes")
if ( test -n "$FOUNDLIB" )
then
	if ( test -n "$FOUNDHEADER" -a -n "$FOUNDLIB" )
	then
		eval "$2"
		eval "$4"
	fi
else
	if ( test -n "$5" -a -n "$6" )
	then
		AC_CHECK_FILE($5, FOUNDLIB="yes")
		if ( test -n "$FOUNDHEADER" -a -n "$FOUNDLIB" )
		then
			eval "$2"
			eval "$6"
		fi
	fi
fi
])


AC_DEFUN([FW_CHECK_HEADERS_AND_LIBS],
[

eval "$7=\"\""
eval "$8=\"\""
eval "$9=\"\""
eval "$10=\"\""
if ( test -n "$11" )
then
	eval "$11=\"\""
fi

SEARCHPATH=$1
NAME=$2
HEADER=$3
LIBNAME=$4
LINKSTATIC=$5
LINKRPATH=$6
INCLUDESTRING=""
LIBSTRING=""
LIBPATH=""
STATIC=""
HEADERSANDLIBSPATH=""

FW_CHECK_HEADER_LIB([/usr/include/$HEADER],[INCLUDESTRING=\"\"],[/usr/lib/lib$LIBNAME.so],[LIBPATH=\"\"; LIBSTRING=\"-l$LIBNAME\"],[/usr/lib/lib$LIBNAME.a],[LIBSTRING=\"-l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
if ( test -z "$LIBSTRING" )
then
	FW_CHECK_HEADER_LIB([/usr/include/$NAME/$HEADER],[INCLUDESTRING=\"-I/usr/include/$NAME\"],[/usr/lib/lib$LIBNAME.so],[LIBPATH=\"\"; LIBSTRING=\"-l$LIBNAME\"],[/usr/lib/lib$LIBNAME.a],[LIBSTRING=\"-l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
fi
if ( test -z "$LIBSTRING" )
then
	FW_CHECK_HEADER_LIB([/usr/include/$HEADER],[INCLUDESTRING=\"\"],[/usr/lib/$NAME/lib$LIBNAME.so],[LIBPATH=\"/usr/lib/$NAME\"; LIBSTRING=\"-L/usr/lib/$NAME -l$LIBNAME\"],[/usr/lib/$NAME/lib$LIBNAME.a],[LIBSTRING=\"-L/usr/lib/$NAME -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
fi
if ( test -z "$LIBSTRING" )
then
	FW_CHECK_HEADER_LIB([/usr/include/$NAME/$HEADER],[INCLUDESTRING=\"-I/usr/include/$NAME\"],[/usr/lib/$NAME/lib$LIBNAME.so],[LIBPATH=\"/usr/lib/$NAME\"; LIBSTRING=\"-L/usr/lib/$NAME -l$LIBNAME\"],[/usr/lib/$NAME/lib$LIBNAME.a],[LIBSTRING=\"-L/usr/lib/$NAME -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
fi

if ( test -z "$LIBSTRING" )
then

	for i in "$SEARCHPATH" "/usr/local/$NAME" "/opt/$NAME" "/usr/$NAME" "/usr/local" "/usr/pkg" "/opt/sfw" "/usr/local/firstworks"
	do
		if ( test -n "$i" )
		then
			FW_CHECK_HEADER_LIB([$i/include/$HEADER],[INCLUDESTRING=\"-I$i/include\"],[$i/lib/lib$LIBNAME.so],[LIBPATH=\"$i/lib\"; LIBSTRING=\"-L$i/lib -l$LIBNAME\"],[$i/lib/lib$LIBNAME.a],[LIBSTRING=\"-L$i/lib -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			if ( test -z "$LIBSTRING" )
			then
				FW_CHECK_HEADER_LIB([$i/include/$NAME/$HEADER],[INCLUDESTRING=\"-I$i/include/$NAME\"],[$i/lib/lib$LIBNAME.so],[LIBPATH=\"$i/lib\"; LIBSTRING=\"-L$i/lib -l$LIBNAME\"],[$i/lib/lib$LIBNAME.a],[LIBSTRING=\"-L$i/lib -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi
			if ( test -z "$LIBSTRING" )
			then
				FW_CHECK_HEADER_LIB([$i/include/$HEADER],[INCLUDESTRING=\"-I$i/include\"],[$i/lib/$NAME/lib$LIBNAME.so],[LIBPATH=\"$i/lib/$NAME\"; LIBSTRING=\"-L$i/lib/$NAME -l$LIBNAME\"],[$i/lib/$NAME/lib$LIBNAME.a],[LIBSTRING=\"-L$i/lib/$NAME -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi
			if ( test -z "$LIBSTRING" )
			then
				FW_CHECK_HEADER_LIB([$i/include/$NAME/$HEADER],[INCLUDESTRING=\"-I$i/include/$NAME\"],[$i/lib/$NAME/lib$LIBNAME.so],[LIBPATH=\"$i/lib/$NAME\"; LIBSTRING=\"-L$i/lib/$NAME -l$LIBNAME\"],[$i/lib/$NAME/lib$LIBNAME.a],[LIBSTRING=\"-L$i/lib/$NAME -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi
			if ( test -n "$LIBSTRING" )
			then
				HEADERSANDLIBSPATH="$i"
				break
			fi
		fi
	done
else
	HEADERSANDLIBSPATH="/usr"
fi

eval "$7=\"$INCLUDESTRING\""
eval "$8=\"$LIBSTRING\""
eval "$9=\"$LIBPATH\""
eval "$10=\"$STATIC\""
if ( test -n "$11" )
then
	eval "$11=\"$HEADERSANDLIBSPATH\""
fi
])



dnl override libtool if so desired
dnl a bit crude, but AC_PROG_LIBTOOL sets vital
dnl environment variables, it seems
AC_DEFUN([FW_CHECK_USE_SYSTEM_LIBTOOL],
[
if ( test "$USE_SYSTEM_LIBTOOL" = "yes" )
then
  LIBTOOL="libtool"
fi
])


dnl checks if the linker supports -rpath
dnl sets the enviroment variable RPATHFLAG
AC_DEFUN([FW_CHECK_LD_RPATH],
[
AC_MSG_CHECKING(whether ld -rpath works)
ld -rpath /usr/lib 2> conftest
INVALID="`grep 'no input files' conftest`"
if ( test -n "$INVALID" )
then
	RPATHFLAG="yes"
	AC_MSG_RESULT(yes)
else
	RPATHFLAG=""
	AC_MSG_RESULT(no)
fi
rm conftest
])


dnl figures out whether to run  ps -efa  or  ps aux
dnl sets the substitution variable PS
AC_DEFUN([FW_CHECK_PS],
[
AC_MSG_CHECKING(whether ps -efa works)
INVALID="`ps -efal 2>&1 | grep illegal | grep -v grep`"
if ( test -n "$INVALID" )
then
	PS="ps\ aux"
	AC_MSG_RESULT(no)
else
	PS="ps\ \-efal"
	AC_MSG_RESULT(yes)
fi
AC_SUBST(PS)
])


dnl sets the substitution variable UNAME with the uname of the machine
AC_DEFUN([FW_CHECK_UNAME],
[
UNAME=`uname -s`
AC_SUBST(UNAME)
])


dnl determines whether we're using GNU strip or not
dnl sets the substitution variable STRIP to "touch" if we're not
AC_DEFUN([FW_CHECK_GNU_STRIP],
[
GNUSTRIP=`$STRIP --version 2> /dev/null | grep "GNU strip"`
AC_MSG_CHECKING(for GNU strip)
if ( test -n "$GNUSTRIP" )
then
	AC_MSG_RESULT(yes)
else
	STRIP="touch"
	AC_MSG_RESULT(no)
fi
AC_SUBST(STRIP)
])


dnl Checks for microsoft platform.
dnl sets the substitution variables MINGW32, CYGWIN and UWIN as appropriate
dnl also moves INSTALL to INSTALL.txt if we're using windows
dnl sets the enviroment variable MICROSOFT
AC_DEFUN([FW_CHECK_MICROSOFT],
[
CYGWIN=""
MINGW32=""
UWIN=""
dnl AC_CANONICAL_HOST gets called when AC_PROG_LIBTOOL is called
case $host_os in
	*cygwin* ) CYGWIN="yes";;
	*mingw32* ) MINGW32="yes";;
	*uwin* ) UWIN="yes";;
esac
AC_SUBST(MINGW32)
AC_SUBST(CYGWIN)
AC_SUBST(UWIN)

dnl Hack so "make install" will work on windows.
MICROSOFT=""
if ( test "$UWIN" = "yes" -o "$MINGW32" = "yes" -o "$CYGWIN" = "yes" )
then
	if ( test -r "INSTALL" )
	then
		mv INSTALL INSTALL.txt
	fi
	MICROSOFT="yes"
fi
])


dnl checks if the compiler supports the inline keyword
dnl defines the macro INLINE
AC_DEFUN([FW_CHECK_INLINE],
[
AC_MSG_CHECKING(inline)
INLINE="inline"
dnl intel optimizing compiler doesn't have inlines, assume that CC doesn't
dnl either even though it might, this test needs to be more robust
if ( test "$CXX" = "icc" -o "$CXX" = "CC" )
then
	INLINE=""
else 
	dnl redhat's gcc 2.96 has problems with inlines
	CXX_VERSION=`$CXX --version`
	if ( test "$CXX_VERSION" = "2.96" )
	then
		INLINE=""
	fi
fi
if ( test "$INLINE" = "inline" )
then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AC_DEFINE_UNQUOTED(INLINE,$INLINE,Some compliers don't support the inline keyword)
])


dnl checks for the pthreads library
dnl requires:  PTHREADSPATH, RPATHFLAG, cross_compiling
dnl sets the substitution variable PTHREADSLIB
dnl sets the environment variable PTHREADSLIBPATH
AC_DEFUN([FW_CHECK_PTHREADS],
[

PTHREADSLIB=""
PTHREADSLIBPATH=""

if ( test "$cross_compiling" = "yes" )
then
	
	dnl cross compiling
	echo "cross compiling"
	if ( test -n "$PTHREADSPATH" )
	then
		PTHREADSLIB="-L$PTHREADSPATH/lib -lpthread"
	else
		PTHREADSLIB="-lpthread"
	fi

else

	for i in "pthread" "pth" "c_r" "gthreads"
	do
		for j in "$PTHREADSPATH" "/usr/local/lib" "/usr/pkg/lib" "/usr/local/lib/pthread" "/usr/local/lib/pthreads" "/usr/local/lib/pth" "/usr/local/pthread/lib" "/usr/local/pthreads/lib" "/opt/sfw/lib"
		do
			if ( test -n "$j" )
			then
				FW_CHECK_LIB([$j/lib$i.so],[PTHREADSLIBPATH=\"$j\"; PTHREADSLIB=\"-L$j -l$i\"],[$j/lib$i.a],[PTHREADSLIB=\"-L$j -l$i\"])
				if ( test -n "$PTHREADSLIB" )
				then
					break
				fi
			fi
		done
	
		FW_CHECK_LIB([/usr/lib/lib$i.so],[PTHREADSLIBPATH=\"\"; PTHREADSLIB=\"-l$i\"],[/usr/lib/lib$i.a],[PTHREADSLIB=\"-l$i\"])
		if ( test -n "$PTHREADSLIB" )
		then
			break
		fi
	done
fi
AC_SUBST(PTHREADSLIB)
])










dnl checks for the rudiments library
dnl requires:  MICROSOFT, RUDIMENTSPATH, RPATHFLAG, cross_compiling
dnl sets the substitution variables RUDIMENTSLIBS, RUDIMENTSDEBUGLIBS,
dnl    RUDIMENTSLIBSPATH, RUDIMENTSLIBSINCLUDES
AC_DEFUN([FW_CHECK_RUDIMENTS],
[

RUDIMENTSLIBS=""
RUDIMENTSLIBSPATH=""
RUDIMENTSINCLUDES=""

if ( test "$cross_compiling" = "yes" )
then

	dnl cross compiling
	echo "cross compiling"
	if ( test -n "$RUDIMENTSPATH" )
	then
		RUDIMENTSINCLUDES="-I$RUDIMENTSPATH/include"
		RUDIMENTSLIBS="-L$RUDIMENTSPATH/lib -lrudiments"
		RUDIMENTSDEBUGLIBS="-L$RUDIMENTSPATH/lib -lrudiments_p"
	fi

else

	if ( test -z "$MICROSOFT" )
	then
		FW_CHECK_HEADERS_AND_LIBS([$RUDIMENTSPATH],[rudiments],[rudiments/daemonprocess.h],[rudiments_p],[$STATICFLAG],[$RPATHFLAG],[RUDIMENTSINCLUDES],[RUDIMENTSDEBUGLIBS],[RUDIMENTSLIBSPATH],[RUDIMENTSSTATIC],[RUDIMENTSPATH])

		FW_CHECK_HEADERS_AND_LIBS([$RUDIMENTSPATH],[rudiments],[rudiments/daemonprocess.h],[rudiments],[$STATICFLAG],[$RPATHFLAG],[RUDIMENTSINCLUDES],[RUDIMENTSLIBS],[RUDIMENTSLIBSPATH],[RUDIMENTSSTATIC],[RUDIMENTSPATH])

	else
		for i in "$RUDIMENTSPATH" "/usr/local/firstworks"
		do
			if ( test -n "$i" )
			then
				FW_CHECK_HEADER_LIB([$i/include/rudiments/daemonprocess.h],[RUDIMENTSINCLUDES=\"-I$i/include\"; RUDIMENTSPATH=\"$i\"],[$i/lib/librudiments.dll],[RUDIMENTSLIBS=\"$i/lib/librudiments.dll\"; RUDIMENTSDEBUGLIBS=\"$i/lib/librudiments_p.dll\"])
			fi
			if ( test -n "$RUDIMENTSLIBS" )
			then
				break
			fi
		done
	fi
fi

if ( test -z "$RUDIMENTSLIBS" )
then
	AC_MSG_ERROR(Rudiments not found.  SQL-Relay requires this package.)
	exit
fi

AC_SUBST(RUDIMENTSPATH)
AC_SUBST(RUDIMENTSINCLUDES)
AC_SUBST(RUDIMENTSLIBS)
AC_SUBST(RUDIMENTSDEBUGLIBS)
AC_SUBST(RUDIMENTSLIBSPATH)
])




AC_DEFUN([FW_CHECK_ORACLE],
[
if ( test "$ENABLE_ORACLE" = "yes" )
then

	ORACLEVERSION=""
	ORACLEINCLUDES=""
	ORACLELIBS=""
	ORACLELIBSPATH=""
	ORACLESTATIC=""
	STATICFLAG=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling
		echo "cross compiling"

	else

		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		AC_MSG_CHECKING(for ORACLE_HOME)
		
		if ( test -n "$ORACLE_HOME" )
		then
			AC_MSG_RESULT(yes)
			FW_CHECK_LIB([$ORACLE_HOME/lib/libcore3.a],[ORACLEVERSION=\"7\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclient -lsqlnet -lncr -lsqlnet -lcommon -lgeneric -lnlsrtl3 -lcore3 -lnlsrtl3 -lcore3 -lc3v6 -lepc -lcore3 -lnsl -lm $AIOLIB\"])
			FW_CHECK_LIB([$ORACLE_HOME/lib/libcore4.a],[ORACLEVERSION=\"8.0\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclient -lncr -lcommon -lgeneric -lclntsh -lepcpt -lcore4 -lnlsrtl3 -lm $AIOLIB\"])
			WTC8=""
			FW_CHECK_LIB([$ORACLE_HOME/lib/libwtc8.a],[WTC8=\"-lwtc8\"])
			FW_CHECK_LIB([$ORACLE_HOME/lib/libcore8.a],[ORACLEVERSION=\"8.1\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclient8 -lcommon8 -lgeneric8 -lclntsh -lcore8 -lnls8 $WTC8 -lm $AIOLIB\"])
			FW_CHECK_LIB([$ORACLE_HOME/lib/libcore9.a],[ORACLEVERSION=\"9.0\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclient9 -lcommon9 -lgeneric9 -lclntsh -lcore9 -lnls9 -lwtc9 -lm $AIOLIB\"])
			if ( test -n "$ORACLEVERSION" )
		then
				ORACLEINCLUDES="-I$ORACLE_HOME/rdbms/demo -I$ORACLE_HOME/rdbms/public -I$ORACLE_HOME/network/public -I$ORACLE_HOME/plsql/public"
				echo "hmmm, looks like Oracle$ORACLEVERSION..."
			fi
			FW_CHECK_LIB([$ORACLE_HOME/lib/libclntsh.a],[ORACLESTATIC=\"$STATICFLAG\"])
		else
			AC_MSG_RESULT(no)
			AC_MSG_WARN(The ORACLE_HOME environment variable is not set.  Oracle support will not be built.)
		fi
		
		
		OCI_H=""
		if ( test -n "$ORACLELIBS" )
		then
			AC_MSG_CHECKING(if Oracle has oci.h)
			if ( test -n "$RPATHFLAG" -a -n "$ORACLELIBSPATH" )
			then
				FW_TRY_LINK([#include <oci.h>
#include <stdlib.h>],[exit(0)],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); OCI_H=\"yes\"],[AC_MSG_RESULT(no)])
			else
			FW_TRY_LINK([#include <oci.h>
#include <stdlib.h>],[exit(0)],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); OCI_H=\"yes\"],[AC_MSG_RESULT(no)])
			fi
		fi
		
		
		LINKFAIL=""
		if ( test -n "$ORACLESTATIC" -a -n "$ORACLELIBS" )
		then
			AC_MSG_CHECKING(if Oracle can be statically linked without $DLLIB)
			if ( test -n "$OCI_H" )
			then
				FW_TRY_LINK([#include <oci.h>
#include <stdlib.h>],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			else
				FW_TRY_LINK([#include <ociapr.h>
#include <stdlib.h>],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			fi
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Oracle can be statically linked with $DLLIB)
				if ( test -n "$OCI_H" )
				then
					FW_TRY_LINK([#include <oci.h>
#include <stdlib.h>],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); ORACLESTATIC=""; LINKFAIL="yes"])
				else
					FW_TRY_LINK([#include <ociapr.h>
#include <stdlib.h>],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES,$ORACLELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); ORACLESTATIC=""; LINKFAIL="yes"])
				fi
			fi
		fi
		
		if ( test -z "$ORACLESTATIC" -a -n "$ORACLELIBS" )
		then
			AC_MSG_CHECKING(if Oracle can be dynamically linked without $DLLIB)
			if ( test -n "$OCI_H" )
			then
				FW_TRY_LINK([#include <oci.h>
#include <stdlib.h>],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			else
				FW_TRY_LINK([#include <ociapr.h>
#include <stdlib.h>],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			fi
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Oracle can be dynamically linked with $DLLIB)
				if ( test -n "$OCI_H" )
				then
					FW_TRY_LINK([#include <oci.h>
#include <stdlib.h>],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); LINKFAIL="yes"])
				else
					FW_TRY_LINK([#include <ociapr.h>
#include <stdlib.h>],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); LINKFAIL="yes"])
				fi
			fi
		fi
		
		if ( test -n "$LINKFAIL" )
		then
			AC_MSG_WARN(No Oracle link configuration could be found.)
			ORACLEVERSION=""
			ORACLEINCLUDES=""
			ORACLELIBS=""
			ORACLESTATIC=""
		fi
		
		if ( test -n "$OCI_H" )
		then
			AC_DEFINE(HAVE_OCI_H,1,oci.h)
		fi
	fi
		
	AC_SUBST(ORACLEVERSION)
	AC_SUBST(ORACLEINCLUDES)
	AC_SUBST(ORACLELIBS)
	AC_SUBST(ORACLELIBSPATH)
	AC_SUBST(ORACLESTATIC)
fi
])



AC_DEFUN([FW_CHECK_MYSQL],
[
if ( test "$ENABLE_MYSQL" = "yes" )
then

	MYSQLINCLUDES=""
	MYSQLLIBS=""
	MYSQLLIBSPATH=""
	MYSQLSTATIC=""
	STATICFLAG=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$MYSQLPATH" )
		then
			MYSQLINCLUDES="-I$MYSQLPATH/include/mysql"
			MYSQLLIBS="-L$MYSQLPATH/lib/mysql -lmysqlclient"
			MYSQLLIBSPATH="$MYSQLPATH/lib/mysql"
		fi

	else

		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		
		if ( test -z "$MICROSOFT" )
		then
	
			FW_CHECK_HEADERS_AND_LIBS([$MYSQLPATH],[mysql],[mysql.h],[mysqlclient],[$STATICFLAG],[$RPATHFLAG],[MYSQLINCLUDES],[MYSQLLIBS],[MYSQLLIBSPATH],[MYSQLSTATIC])
	
		else
			if ( test -n "$MYSQLPATH" )
			then
				AC_CHECK_FILE($MYSQLPATH/include/mysql.h,MYSQLINCLUDES="-I$MYSQLPATH/include")
				AC_CHECK_FILE($MYSQLPATH/lib/opt/libmysqlclient.a,MYSQLLIBS="-L$MYSQLPATH/lib/opt -lmysqlclient"; MYSQLSTATIC="$STATICFLAG")
				AC_CHECK_FILE($MYSQLPATH/lib/opt/libmysqlclient.so,MYSQLLIBSPATH="$MYSQLPATH/lib/opt"; MYSQLLIBS="-L$MYSQLPATH/lib/opt -lmysqlclient")
			else
				AC_CHECK_FILE("/cygdrive/c/mysql/include/mysql.h",MYSQLINCLUDES="-I/cygdrive/c/mysql/include")
				AC_CHECK_FILE("/cygdrive/c/mysql/lib/opt/libmySQL.dll",MYSQLLIBS="/cygdrive/c/mysql/lib/opt/libmySQL.dll")
			fi
		fi
		
		if ( test -n "$MYSQLLIBS" )
		then
		
			AC_MSG_CHECKING(if MySQL requires -lz)
			FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_real_connect(NULL,NULL,NULL,NULL,NULL,0,NULL,0); mysql_real_query(NULL,NULL,0); mysql_store_result(NULL); mysql_num_fields(NULL); mysql_fetch_row(NULL); mysql_free_result(NULL); mysql_close(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); MYSQLLIBS="$MYSQLLIBS -lz"])
		
			NEEDSLIBZ=`echo "$MYSQLLIBS" | grep "\-lz"`
			if ( test -n "$NEEDSLIBZ" )
			then
				AC_CHECK_LIB(z,gzopen,,MYSQLINCLUDES=""; MYSQLLIBS=""; AC_MSG_WARN(MySQL requires libz but libz was not found.))
			fi
		fi
		
		if ( test "$MYSQLINCLUDES" = "-I/usr/include" )
		then
			MYSQLINCLUDES=""
		fi
	
		if ( test -z "$MYSQLLIBS" )
		then
			AC_MSG_WARN(MySQL support will not be built.)
		fi
	fi
		
	AC_SUBST(MYSQLINCLUDES)
	AC_SUBST(MYSQLLIBS)
	AC_SUBST(MYSQLLIBSPATH)
	AC_SUBST(MYSQLSTATIC)
fi
])
	


AC_DEFUN([FW_CHECK_MSQL],
[
if ( test "$ENABLE_MSQL" = "yes" )
then

	MSQLINCLUDES=""
	MSQLLIBS=""
	MSQLLIBSPATH=""
	MSQLSTATIC=""
	STATICFLAG=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$MSQLPATH" )
		then
			MSQLINCLUDES="-I$MSQLPATH/include"
			MSQLLIBS="-L$MSQLPATH/lib -lmsql"
			MSQLLIBSPATH="$MSQLPATH/lib"
		fi

	else

		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		
		for i in "$MSQLPATH" "/usr/local/Hughes"
		do
			if ( test -n "$i" )
			then
				FW_CHECK_HEADER_LIB([$i/include/msql.h],[MSQLINCLUDES=\"-I$i/include\"; MSQLPATH=\"$i\"],[$i/lib/libmsql.so],[MSQLLIBSPATH=\"$i/lib\"; MSQLLIBS=\"-L$i/lib -lmsql\"],[$i/lib/libmsql.a],[MSQLLIBS=\"-L$i/lib -lmsql\"])
				if ( test -n "$MSQLLIBS" )
				then
					break
				fi
			fi
		done
	
		FW_CHECK_HEADER_LIB([/usr/include/msql.h],[MSQLINCLUDES=\"\"; MSQLPATH=\"\"],[/usr/lib/libmsql.so],[MSQLLIBSPATH=\"\"; MSQLLIBS=\"-lmsql\"],[/usr/lib/libmsql.a],[MSQLLIBS=\"-lmsql\"])
		
		if ( test -z "$MSQLLIBS" )
		then
			AC_MSG_WARN(mSQL support will not be built.)
		fi
	fi
		
	AC_SUBST(MSQLINCLUDES)
	AC_SUBST(MSQLLIBS)
	AC_SUBST(MSQLLIBSPATH)
	AC_SUBST(MSQLSTATIC)
fi
])



AC_DEFUN([FW_CHECK_POSTGRESQL],
[
if ( test "$ENABLE_POSTGRESQL" = "yes" )
then
	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$POSTGRESQLPATH" )
		then
			POSTGRESQLINCLUDES="-I$POSTGRESQLPATH/include"
			POSTGRESQLLIBS="-L$POSTGRESQLPATH/lib -lpq"
			POSTGRESQLLIBSPATH="$POSTGRESQLPATH/lib"
		fi

	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
	
		FW_CHECK_HEADERS_AND_LIBS([$POSTGRESQLPATH],[pgsql],[libpq-fe.h],[pq],[$STATICFLAG],[$RPATHFLAG],[POSTGRESQLINCLUDES],[POSTGRESQLLIBS],[POSTGRESQLLIBSPATH],[POSTGRESQLSTATIC])
	
		if ( test -z "$POSTGRESQLLIBS" )
		then
			FW_CHECK_HEADERS_AND_LIBS([$POSTGRESQLPATH],[postgresql],[libpq-fe.h],[pq],[$STATICFLAG],[$RPATHFLAG],[POSTGRESQLINCLUDES],[POSTGRESQLLIBS],[POSTGRESQLLIBSPATH],[POSTGRESQLSTATIC])
		fi
	
		if ( test -z "$POSTGRESQLLIBS" )
		then
			FW_CHECK_HEADERS_AND_LIBS([$POSTGRESQLPATH],[postgres],[libpq-fe.h],[pq],[$STATICFLAG],[$RPATHFLAG],[POSTGRESQLINCLUDES],[POSTGRESQLLIBS],[POSTGRESQLLIBSPATH],[POSTGRESQLSTATIC])
		fi
		
		LINKFAIL=""
		if ( test -n "$POSTGRESQLSTATIC" -a -n "$POSTGRESQLLIBS" )
		then
			AC_MSG_CHECKING(if PostgreSQL can be statically linked without -lcrypt)
			FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetdbLogin(NULL,NULL,NULL,NULL,NULL,NULL,NULL);],[$POSTGRESQLSTATIC $POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" )
			then
				AC_MSG_CHECKING(if PostgreSQL can be statically linked with -lcrypt)
				FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetdbLogin(NULL,NULL,NULL,NULL,NULL,NULL,NULL);],[$POSTGRESQLSTATIC $POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIB -lcrypt],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); POSTGRESQLLIBS="$POSTGRESQLLIBS -lcrypt"; LINKFAIL=""],[AC_MSG_RESULT(no); POSTGRESQLSTATIC=""; LINKFAIL="yes"])
			fi
		fi
		
		if ( test -z "$POSTGRESQLSTATIC" -a -n "$POSTGRESQLLIBS" )
		then
			AC_MSG_CHECKING(if PostgreSQL can be dynamically linked without -lcrypt)
			FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetdbLogin(NULL,NULL,NULL,NULL,NULL,NULL,NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" )
			then
				AC_MSG_CHECKING(if PostgreSQL can be dynamically linked with -lcrypt)
				FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetdbLogin(NULL,NULL,NULL,NULL,NULL,NULL,NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIB -lcrypt],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); POSTGRESQLLIBS="$POSTGRESQLLIBS -lcrypt"; LINKFAIL=""],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			fi
		fi
		
		if ( test -n "$LINKFAIL" )
		then
			AC_MSG_WARN(No PostgreSQL link configuration could be found.)
			POSTGRESQLINCLUDES=""
			POSTGRESQLLIBS=""
			POSTGRESQLSTATIC=""
		fi
		
		if ( test "$POSTGRESQLINCLUDES" = "-I/usr/include" )
		then
			POSTGRESQLINCLUDES=""
		fi
	
		if ( test -z "$POSTGRESQLLIBS" )
		then
			AC_MSG_WARN(PostgreSQL support will not be built.)
		fi
	fi
		
	AC_SUBST(POSTGRESQLINCLUDES)
	AC_SUBST(POSTGRESQLLIBS)
	AC_SUBST(POSTGRESQLLIBSPATH)
	AC_SUBST(POSTGRESQLSTATIC)
fi
])


AC_DEFUN([FW_CHECK_GDBM],
[
if ( test "$cross_compiling" = "yes" )
then
	dnl cross compiling ...
	echo "cross compiling"
	if ( test -n "$GDBMPATH" )
	then
		GDBMINCLUDES="-I$GDBMPATH/include"
		GDBMLIBS="-L$GDBMPATH/lib -lgdbm"
		GDBMLIBSPATH="$GDBMPATH/lib"
	fi
else
	FW_CHECK_HEADERS_AND_LIBS([$LIBGDBMPATH],[gdbm],[gdbm.h],[gdbm],[$STATICFLAG],[$RPATHFLAG],[GDBMINCLUDES],[GDBMLIBS],[GDBMLIBSPATH],[GDBMSTATIC])
fi
AC_SUBST(GDBMLIBS)
])


AC_DEFUN([FW_CHECK_SQLITE],
[
if ( test "$ENABLE_SQLITE" = "yes" )
then

	FW_CHECK_GDBM
		
	SQLITEINCLUDES=""
	SQLITELIBS=""
	SQLITELIBSPATH=""
	SQLITESTATIC=""

	if ( test "$cross_compiling" = "yes" )
	then
		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$SQLITEPATH" )
		then
			SQLITEINCLUDES="-I$SQLITEPATH/include"
			SQLITELIBS="-L$SQLITEPATH/lib -lsqlite"
			SQLITELIBSPATH="$SQLITEPATH/lib"
		fi
	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		
		if ( test -n "$PTHREADSLIB" )
		then
			FW_CHECK_HEADERS_AND_LIBS([$SQLITEPATH],[sqlite],[sqlite.h],[sqlite],[$STATICFLAG],[$RPATHFLAG],[SQLITEINCLUDES],[SQLITELIBS],[SQLITELIBSPATH],[SQLITESTATIC])
		else
			AC_MSG_WARN(pthreads was not found.)
		fi
			
		if ( test -z "$SQLITELIBS" )
		then
			AC_MSG_WARN(SQLite support will not be built.)
		else
			AC_MSG_CHECKING(if SQLite needs gdbm)
			SQLITENEEDGDBM=""
			FW_TRY_LINK([#include <sqlite.h>],[sqlite *sqliteptr; char *errmesg; sqliteptr=sqlite_open("/tmp/testfile",666,&errmesg); sqlite_close(sqliteptr);],[$SQLITESTATIC $SQLITEINCLUDES],[$SQLITELIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$SQLITELIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); SQLITENEEDGDBM="yes"])
		
			if ( test -n "$SQLITENEEDGDBM" )
			then
				if ( test -z "$GDBMLIBS" )
				then
					AC_MSG_WARN(SQLite needs GDBM but GDBM was not found. SQLite support will not be built.)
					SQLITELIBS=""
					SQLITELIBSPATH=""
					SQLITEINCLUDES=""
					SQLITESTATIC=""
				else 
					SQLITELIBS="$SQLITELIBS $GDBMLIBS"
				fi
			else
				AC_DEFINE_UNQUOTED(SQLITE_TRANSACTIONAL,1,Some versions of sqlite are transactional)
			fi
		
		fi
	fi
		
	AC_SUBST(SQLITEINCLUDES)
	AC_SUBST(SQLITELIBS)
	AC_SUBST(SQLITELIBSPATH)
	AC_SUBST(SQLITESTATIC)
fi
])



AC_DEFUN([FW_CHECK_LAGO],
[
if ( test "$ENABLE_LAGO" = "yes" )
then
	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$LAGOPATH" )
		then
			LAGOINCLUDES="-I$LAGOPATH/include"
			LAGOLIBS="-L$LAGOPATH/lib -llago"
			LAGOLIBSPATH="$LAGOPATH/lib"
		fi

	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
	
		FW_CHECK_HEADERS_AND_LIBS([$LAGOPATH],[lago],[lago.h],[lago],[$STATICFLAG],[$RPATHFLAG],[LAGOINCLUDES],[LAGOLIBS],[LAGOLIBSPATH],[LAGOSTATIC])
		
		if ( test -z "$LAGOLIBS" )
		then
			AC_MSG_WARN(Lago support will not be built.)
		else
			AC_MSG_CHECKING(if Lago needs threads)
			FW_TRY_LINK([#include <lago.h>],[LCTX lctx; lctx=Lnewctx(); Ldelctx(lctx);],[$LAGOSTATIC $LAGOINCLUDES],[$LAGOLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$LAGOLIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); LAGOLIBS="$LAGOLIBS $PTHREADSLIB"])
		fi

	fi

	AC_SUBST(LAGOINCLUDES)
	AC_SUBST(LAGOLIBS)
	AC_SUBST(LAGOLIBSPATH)
	AC_SUBST(LAGOSTATIC)
fi
])



AC_DEFUN([FW_CHECK_FREETDS],
[
if ( test "$ENABLE_FREETDS" = "yes" )
then

	FREETDSINCLUDES=""
	FREETDSLIBS=""
	FREETDSLIBSPATH=""
	FREETDSSTATIC=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$FREETDSPATH" )
		then
			FREETDSINCLUDES="-I$FREETDSPATH/include"
			FREETDSLIBS="-L$FREETDSPATH/lib -lct"
			FREETDSLIBSPATH="$FREETDSPATH/lib"
		fi

	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
	
		FW_CHECK_HEADERS_AND_LIBS([$FREETDSPATH],[freetds],[ctpublic.h],[ct],[$STATICFLAG],[$RPATHFLAG],[FREETDSINCLUDES],[FREETDSLIBS],[FREETDSLIBPATH],[FREETDSSTATIC])
		
		if ( test -z "$FREETDSLIBS" )
		then
			AC_MSG_WARN(FreeTDS support will not be built.)
		else
			AC_LANG(C++)
			AC_MSG_CHECKING(whether ctpublic.h contains function definitions)
			FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$FREETDSINCLUDES],[$FREETDSLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_FREETDS_FUNCTION_DEFINITIONS,1,Some versions of FreeTDS have function definitions)],[AC_MSG_RESULT(no)])
			AC_LANG(C)
		fi
	fi
		
	AC_SUBST(FREETDSINCLUDES)
	AC_SUBST(FREETDSLIBS)
	AC_SUBST(FREETDSLIBSPATH)
	AC_SUBST(FREETDSSTATIC)
fi
])



AC_DEFUN([FW_CHECK_SYBASE],
[
if ( test "$ENABLE_SYBASE" = "yes" )
then

	SYBASEINCLUDES=""
	SYBASELIBS=""
	SYBASESTATIC=""
	SYBASELIBSPATH=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$SYBASEPATH" )
		then
			SYBASEINCLUDES="-I$SYBASEPATH/include"
			SYBASELIBS="-L$SYBASEPATH/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck"
			SYBASELIBSPATH="$SYBASEPATH/lib"
		fi

	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		
		if ( test -n "$SYBASEPATH" )
		then
			FW_CHECK_HEADER_LIB([$SYBASEPATH/include/ctpublic.h],[SYBASEINCLUDES=\"-I$SYBASEPATH/include\"],[$SYBASEPATH/lib/libct.a],[SYBASELIBS=\"-L$SYBASEPATH/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"],[$SYBASEPATH/lib/libct.so],[SYBASELIBSPATH=\"$SYBASEPATH/lib\"; SYBASELIBS=\"-L$SYBASEPATH/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"])
		else
		
			FW_CHECK_HEADER_LIB([/usr/local/sybase/include/ctpublic.h],[SYBASEINCLUDES=\"-I/usr/local/sybase/include\"],[/usr/local/sybase/lib/libct.a],[SYBASELIBS=\"-L/usr/local/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"],[/usr/local/sybase/lib/libct.so],[SYBASELIBSPATH=\"/usr/local/sybase/lib\"; SYBASELIBS=\"-L/usr/local/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"])
		
			FW_CHECK_HEADER_LIB([/opt/sybase/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase/include\"],[/opt/sybase/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"],[/opt/sybase/lib/libct.so],[SYBASELIBSPATH=\"/opt/sybase/lib\"; SYBASELIBS=\"-L/opt/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"])
	
			FW_CHECK_HEADER_LIB([/opt/sybase-12.5/OCS-12_5/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase-12.5/OCS-12_5/include\"],[/opt/sybase-12.5/OCS-12_5/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase-12.5/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"; SYBASESTATIC=\"$STATICFLAG\"],[/opt/sybase-12.5/OCS-12_5/lib/libct.so],[SYBASELIBSPATH=\"/opt/sybase-12.5/OCS-12_5/lib\"; SYBASELIBS=\"-L/opt/sybase-12.5/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"])
		
			if ( test -z "$SYBASELIBS" )
			then
				for i in "11.9.2" "11.0.3.3"
				do
					FW_CHECK_HEADER_LIB([/opt/sybase-$i/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase-$i/include\"],[/opt/sybase-$i/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase-$i/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"],[/opt/sybase-$i/lib/libct.so],[SYBASELIBSPATH=\"/opt/sybase-$i/lib\"; SYBASELIBS=\"-L/opt/sybase-$i/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl -linsck\"])
					if ( test -n "$SYBASELIBS" )
					then
						break
					fi
				done
			fi
		fi
		
		LINKFAIL=""
		if ( test -n "$SYBASESTATIC" -a -n "$SYBASELIBS" )
		then
			AC_MSG_CHECKING(if Sybase can be statically linked without $DLLIB)
			FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASESTATIC $SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Sybase can be statically linked with $DLLIB)
				FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASESTATIC $SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); SYBASELIBS="$SYBASELIBS $DLLIB"; LINKFAIL="";],[AC_MSG_RESULT(no); SYBASESTATIC=""])
			fi
		fi
		
		if ( test -z "$SYBASESTATIC" -a -n "$SYBASELIBS" )
		then
			AC_MSG_CHECKING(if Sybase can be dynamically linked without $DLLIB)
			FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$SYBASELIBSPATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Sybase can be dynamically linked with $DLLIB)
				FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH:$SYBASELIBSPATH],[AC_MSG_RESULT(yes); SYBASELIBS="$SYBASELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no)])
			fi
		fi
	
		if ( test -n "$LINKFAIL" )
		then
			AC_MSG_WARN(No Sybase link configuration could be found.)
			SYBASEINCLUDES=""
			SYBASELIBS=""
			SYBASELIBSPATH=""
			SYBASESTATIC=""
		fi
	
		if ( test -z "$SYBASELIBS" )
		then
			AC_MSG_WARN(Sybase support will not be built.)
		fi
	fi
		
	AC_SUBST(SYBASEINCLUDES)
	AC_SUBST(SYBASELIBS)
	AC_SUBST(SYBASELIBSPATH)
	AC_SUBST(SYBASESTATIC)
fi
])



AC_DEFUN([FW_CHECK_ODBC],
[
if ( test "$ENABLE_ODBC" = "yes" )
then

	ODBCSTATIC=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$ODBCPATH" )
		then
			ODBCINCLUDES="-I$ODBCPATH/include"
			ODBCLIBS="-L$ODBCPATH/lib -lodbc"
			ODBCLIBSPATH="$ODBCPATH/lib"
		fi

	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		HAVE_IODBC=""
		HAVE_UNIXODBC=""
	
		FW_CHECK_HEADERS_AND_LIBS([$ODBCPATH],[unixodbc],[sql.h],[odbc],[$STATICFLAG],[$RPATHFLAG],[ODBCINCLUDES],[ODBCLIBS],[ODBCLIBSPATH],[UNIXODBCSTATIC])
	
		if ( test -n "$ODBCLIBS" )
		then
			HAVE_UNIXODBC="yes"
		else
			FW_CHECK_HEADERS_AND_LIBS([$ODBCPATH],[iodbc],[sql.h],[iodbc],[$STATICFLAG],[$RPATHFLAG],[ODBCINCLUDES],[ODBCLIBS],[ODBCLIBSPATH],[IODBCSTATIC])
			if ( test -n "$ODBCLIBS" )
			then
				HAVE_IODBC="yes"
			fi
		fi
		
		AC_SUBST(ODBCINCLUDES)
		AC_SUBST(ODBCLIBS)
		AC_SUBST(ODBCLIBSPATH)
	
		if ( test -n "`echo $ODBCLIBS | grep iodbc`" )
		then
			ODBCSTATIC="$IODBCSTATIC"
		else
			ODBCSTATIC="$UNIXODBCSTATIC"
		fi
		AC_SUBST(ODBCSTATIC)
		
		if ( test -n "$HAVE_UNIXODBC" )
		then
			AC_DEFINE(HAVE_UNIXODBC,1,UnixODBC)
			AC_MSG_CHECKING(if UnixODBC needs threads)
			FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLHENV env; SQLHDBC dbc; SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env); SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc); SQLFreeHandle(SQL_HANDLE_DBC,dbc); SQLFreeHandle(SQL_HANDLE_ENV,env);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); ODBCLIBS="$ODBCLIBS $PTHREADSLIB"])
		fi
		if ( test -n "$HAVE_IODBC" )
		then
			AC_DEFINE(HAVE_IODBC,1,iODBC)
			AC_MSG_CHECKING(if iODBC needs threads)
			FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLHENV env; SQLHDBC dbc; SQLAllocEnv(&env); SQLAllocConnect(env,&dbc); SQLFreeConnect(&dbc); SQLFreeEnv(&env);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); ODBCLIBS=\"$ODBCLIBS $PTHREADSLIB\"])
		fi
		if ( test -z "$ODBCLIBS" )
		then
			AC_MSG_WARN(ODBC support will not be built.)
		fi
	fi
fi
])



AC_DEFUN([FW_CHECK_DB2],
[
if ( test "$ENABLE_DB2" = "yes" )
then

	DB2INCLUDES=""
	DB2LIBS=""
	DB2LIBSPATH=""
	DB2STATIC=""
	DB2VERSION="7"

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$DB2PATH" )
		then
			DB2INCLUDES="-I$DB2PATH/include"
			DB2LIBS="-L$DB2PATH/lib -ldb2"
			DB2LIBSPATH="$DB2PATH/lib"
		fi

	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		
		if ( test -n "$DB2PATH" )
		then
			FW_CHECK_HEADER_LIB([$DB2PATH/include/sql.h],[DB2INCLUDES=\"-I$DB2PATH/include\"],[$DB2PATH/lib/libdb2.so],[DB2LIBSPATH=\"$DB2PATH/lib\"; DB2LIBS=\"-L$DB2PATH/lib -ldb2\"],[$DB2PATH/lib/libdb2.a],[DB2LIBS=\"-L$DB2PATH/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"])
		
		else
		
			dnl check /opt for 7.2
			FW_CHECK_HEADER_LIB([/opt/IBMdb2/V7.1/include/sql.h],[DB2INCLUDES=\"-I/opt/IBMdb2/V7.1/include\"],[/opt/IBMdb2/V7.1/lib/libdb2.so],[DB2LIBSPATH=\"/opt/IBMdb2/V7.1/lib\"; DB2LIBS=\"-L/opt/IBMdb2/V7.1/lib -ldb2\"],[/opt/IBMdb2/V7.1/lib/libdb2.a],[DB2LIBS=\"-L/opt/IBMdb2/V7.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"])
		
			dnl check /usr for 7.2
			FW_CHECK_HEADER_LIB([/usr/IBMdb2/V7.1/include/sql.h],[DB2INCLUDES=\"-I/usr/IBMdb2/V7.1/include\"],[/usr/IBMdb2/V7.1/lib/libdb2.so],[DB2LIBSPATH=\"/usr/IBMdb2/V7.1/lib\"; DB2LIBS=\"-L/usr/IBMdb2/V7.1/lib -ldb2\"],[/usr/IBMdb2/V7.1/lib/libdb2.a],[DB2LIBS=\"-L/usr/IBMdb2/V7.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"])
		
			dnl check /opt for 8.1
			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V8.1/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V8.1/include\"; DB2VERSION=\"8\"],[/opt/IBM/db2/V8.1/lib/libdb2.so],[DB2LIBSPATH=\"/opt/IBM/db2/V8.1/lib\"; DB2LIBS=\"-L/opt/IBM/db2/V8.1/lib -ldb2\"; DB2VERSION=\"8\"],[/opt/IBM/db2/V8.1/lib/libdb2.a],[DB2LIBS=\"-L/opt/IBM/db2/V8.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"8\"])
		fi
		
		if ( test -z "$DB2LIBS" )
		then
			AC_MSG_WARN(DB2 support will not be built.)
		fi
	fi
		
	AC_SUBST(DB2INCLUDES)
	AC_SUBST(DB2LIBS)
	AC_SUBST(DB2LIBSPATH)
	AC_SUBST(DB2STATIC)
	AC_DEFINE_UNQUOTED(DB2VERSION,$DB2VERSION,Version of DB2)
fi
])



AC_DEFUN([FW_CHECK_INTERBASE],
[
if ( test "$ENABLE_INTERBASE" = "yes" )
then

	INTERBASEINCLUDES=""
	INTERBASELIBS=""
	INTERBASELIBSPATH=""
	INTERBASESTATIC=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$INTERBASEPATH" )
		then
			INTERBASEINCLUDES="-I$INTERBASEPATH/include"
			INTERBASELIBS="-L$INTERBASEPATH/lib -lgds"
			INTERBASELIBSPATH="$INTERBASEPATH/lib"
		fi

	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		
		if ( test -n "$INTERBASEPATH" )
		then
			FW_CHECK_HEADER_LIB([$INTERBASEPATH/include/ibase.h],[INTERBASEINCLUDES=\"-I$INTERBASEPATH/include\"],[$INTERBASEPATH/lib/libgds.so],[INTERBASELIBSPATH=\"$INTERBASPATH/lib\"; INTERBASELIBS=\"-L$INTERBASEPATH/lib -lgds -lcrypt\"],[$INTERBASEPATH/lib/libgds.a],[INTERBASELIBS=\"-L$INTERBASEPATH/lib -lgds -lcrypt\"; INTERBASESTATIC=\"$STATICFLAG\"])
		
		else
		
			dnl includes
			FW_CHECK_HEADER_LIB([/usr/include/ibase.h],[INTERBASEINCLUDES=\"\"],[/usr/lib/libgds.so],[INTERBASELIBS=\"-lgds -lcrypt\"],[/usr/lib/libgds.a],[INTERBASELIBS=\"-lgds -lcrypt\"; INTERBASESTATIC=\"$STATICFLAG\"])
		fi
		
		LINKFAIL=""
		if ( test -n "$INTERBASESTATIC" -a -n "$INTERBASELIBS" )
		then
			AC_MSG_CHECKING(if Interbase can be statically linked without $DLLIB)
			FW_TRY_LINK([#include <ibase.h>
	#include <stdlib.h>],[isc_db_handle db=0; isc_attach_database(NULL,0,"",&db,0,NULL);],[$INTERBASESTATIC $INTERBASEINCLUDES],[$INTERBASELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Interbase can be statically linked with $DLLIB)
				FW_TRY_LINK([#include <ibase.h>
#include <stdlib.h>],[isc_db_handle db=0; isc_attach_database(NULL,0,"",&db,0,NULL);],[$INTERBASESTATIC $INTERBASEINCLUDES],[$INTERBASELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); INTERBASELIBS="$INTERBASELIBS $DLLIB"; LINKFAIL="";],[AC_MSG_RESULT(no); INTERBASESTATIC=""])
			fi
		fi
		
		if ( test -z "$INTERBASESTATIC" -a -n "$INTERBASELIBS" )
		then
			AC_MSG_CHECKING(if Interbase can be dynamically linked without $DLLIB)
			FW_TRY_LINK([#include <ibase.h>
#include <stdlib.h>],[isc_db_handle db=0; isc_attach_database(NULL,0,"",&db,0,NULL);],[$INTERBASEINCLUDES],[$INTERBASELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Interbase can be dynamically linked with $DLLIB)
				FW_TRY_LINK([#include <ibase.h>
#include <stdlib.h>],[isc_db_handle db=0; isc_attach_database(NULL,0,"",&db,0,NULL);],[$INTERBASEINCLUDES],[$INTERBASELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); INTERBASELIBS="$INTERBASELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no)])
			fi
		fi
		
		if ( test -n "$LINKFAIL" )
		then
			AC_MSG_WARN(No Interbase link configuration could be found.)
			INTERBASEINCLUDES=""
			INTERBASELIBS=""
			INTERBASESTATIC=""
		fi
		
		if ( test -z "$INTERBASELIBS" )
		then
			AC_MSG_WARN(Interbase support will not be built.)
		fi
	fi
	
	AC_SUBST(INTERBASEINCLUDES)
	AC_SUBST(INTERBASELIBS)
	AC_SUBST(INTERBASELIBSPATH)
	AC_SUBST(INTERBASESTATIC)
fi
])



AC_DEFUN([FW_CHECK_PERL],
[
if ( test "$ENABLE_PERL" = "yes" )
then

	HAVE_PERL=""
	PERL=""
	PERLPREFIX=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else

		if ( test -n "$PERLPATH" )
		then
			AC_CHECK_FILE("$PERLPATH/bin/perl",PERL="$PERLPATH/bin/perl"; HAVE_PERL="yes")
		else
			AC_CHECK_PROG(PERL,perl,"perl")
			if ( test -z "$PERL" )
			then
				for i in "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/usr/local/perl/bin" "/opt/sfw/bin"
				do
					AC_CHECK_FILE("$i/perl5",PERL="$i/perl5")
					AC_CHECK_FILE("$i/perl",PERL="$i/perl")
					if ( test -n "$PERL" )
					then
						break
					fi
				done
			fi
		fi
		if ( test -n "$PERL" )
		then
			HAVE_PERL="yes"
			PERLPREFIXCMD=`$PERL -V:prefix`
			PERLPREFIX=`eval "$PERLPREFIXCMD"; echo $prefix`
		else
			AC_MSG_WARN(The Perl API will not be built.)
		fi
	fi

	AC_SUBST(HAVE_PERL)
	AC_SUBST(PERL)
	AC_SUBST(PERLPREFIX)
fi
])



AC_DEFUN([FW_CHECK_PYTHON],
[
if ( test "$ENABLE_PYTHON" = "yes" )
then

	HAVE_PYTHON=""
	PYTHONINCLUDES=""
	PYTHONLIBS=""
	PYTHONOBJ=""
	PYTHONDIR=""
	PYTHONVERSION=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else

		AC_MSG_CHECKING(for Python)
		
		if ( test -n "$PYTHONPATH" )
		then
		
			for i in "1.5" "1.6" "2.0" "2.1" "2.2"
			do
				if ( test -d "$PYTHONPATH/include/python$i" )
				then
					PYTHONINCLUDES="-I$PYTHONPATH/include/python$i"
				fi
		
				if ( test -r "$PYTHONPATH/lib/python$i/config/python.o" -a -r "$PYTHONPATH/lib/python$i/config/libpython$i.a" )
				then
					PYTHONLIBS="-L$PYTHONPATH/lib/python$i/config -lpython$i"
					PYTHONOBJ="$PYTHONPATH/lib/python$i/config/python.o"
		
				else
					if ( test -r "$PYTHONPATH/lib/python$i/config/ccpython.o" -a -r "$PYTHONPATH/lib/python$i/config/libpython$i.a" )
					then
					PYTHONLIBS="-L$PYTHONPATH/lib/python$i/config -lpython$i"
					PYTHONOBJ="$PYTHONPATH/lib/python$i/config/ccpython.o"
					fi
				fi
			
				if ( test -d "$PYTHONPATH/lib/python$i" )
				then
					PYTHONDIR="$PYTHONPATH/lib/python$i"
				fi
		
				if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONLIBS" -a -n "$PYTHONDIR" )
				then
					PYTHONVERSION="$i"
					break
				fi
			done
		
		else
		
			for j in "1.5" "1.6" "2.0" "2.1" "2.2"
			do
				for i in "/usr/include/python$j" "/usr/local/include/python$j" "/usr/pkg/include/python$j" "/usr/local/python$j/include/python$j" "/opt/sfw/include/python$j"
				do
					if ( test -d "$i" )
					then
						PYTHONINCLUDES="-I$i"
					fi
					if ( test -n "$PYTHONINCLUDES" )
					then
						break
					fi
				done
			
				for i in "/usr/lib/python$j/config" "/usr/local/lib/python$j/config" "/usr/pkg/lib/python$j/config" "/usr/local/python$j/lib/python$j/config" "/opt/sfw/lib/python$j/config"
				do
					if ( test -r "$i/python.o" -a -r "$i/libpython$j.a" )
					then
						PYTHONLIBS="-L$i -lpython$j"
						PYTHONOBJ="$i/python.o"
					else
						if ( test -r "$i/ccpython.o" -a -r "$i/libpython$j.a" )
						then
							PYTHONLIBS="-L$i/config -lpython$j"
							PYTHONOBJ="$i/config/ccpython.o"
						fi
					fi
					if ( test -n "$PYTHONLIBS" )
					then
						break
					fi
				done
			
				for i in "/usr/lib/python$j" "/usr/local/lib/python$j" "/usr/pkg/lib/python$j" "/usr/local/python$j/lib/python$j" "/opt/sfw/lib/python$j"
				do
					if ( test -d "$i" )
					then
						PYTHONDIR="$i"
					fi
					if ( test -n "$PYTHONDIR" )
					then
						break
					fi
				done
				if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONLIBS" -a -n "$PYTHONDIR" )
				then
					PYTHONVERSION="$j"
					break
				fi
			done
		
		fi
		
		if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONLIBS" -a -n "$PYTHONOBJ" -a -n "$PYTHONDIR" )
		then
			HAVE_PYTHON="yes"
			AC_MSG_RESULT(yes)
		else
			AC_MSG_WARN(The Python API will not be built.)
			AC_MSG_RESULT(no)
		fi
	fi

	AC_SUBST(HAVE_PYTHON)
	AC_SUBST(PYTHONINCLUDES)
	AC_SUBST(PYTHONLIBS)
	AC_SUBST(PYTHONOBJ)
	AC_SUBST(PYTHONDIR)
	AC_SUBST(PYTHONVERSION)
fi
])



AC_DEFUN([FW_CHECK_ZOPE],
[
if ( test "$ENABLE_ZOPE" = "yes" )
then

	HAVE_ZOPE=""
	ZOPEDIR=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else
	
		if ( test -n "$HAVE_PYTHON" )
		then
			if ( test -n "$ZOPEPATH" )
			then
				ZOPEDIR="$ZOPEPATH/lib/python/Products"
			else
		
				if ( test -z "$ZOPEDIR" )
				then
					for i in "/usr/local/www" "/usr/share" "/usr/local" "/usr" "/opt"
					do
						for j in "zope" "Zope"
						do
							AC_CHECK_FILE("$i/$j/lib/python/Products/__init__.py",HAVE_ZOPE="yes"; ZOPEDIR="$i/$j/lib/python/Products")
							if ( test -n "$ZOPEDIR" )
							then
								break
							fi
							for k in "2.2" "2.3" "2.4"
							do
								AC_CHECK_FILE("$i/$j-$k/lib/python/Products/__init__.py",HAVE_ZOPE="yes"; ZOPEDIR="$i/$j-$k/lib/python/Products")
								if ( test -n "$ZOPEDIR" )
								then
									break
								fi
							done
						done
						if ( test -n "$ZOPEDIR" )
						then
							break
						fi
					done
				fi
			fi
		fi
		
		if ( test -z "$HAVE_ZOPE" )
		then
			AC_MSG_WARN(The Zope API will not be installed.)
		fi
	fi

	AC_SUBST(HAVE_ZOPE)
	AC_SUBST(ZOPEDIR)
fi
])



AC_DEFUN([FW_CHECK_RUBY],
[
if ( test "$ENABLE_RUBY" = "yes" )
then

	HAVE_RUBY=""
	RUBY=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else
	
		if ( test -n "$RUBYPATH" )
		then
			AC_CHECK_FILE("$RUBYPATH/bin/ruby",RUBY="$RUBYPATH/bin/ruby")
		else
			AC_CHECK_PROG(RUBY,"ruby","ruby")
			if ( test -z "$RUBY" )
			then
				for i in "/usr/local/ruby/bin" "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/opt/sfw/bin"
				do
					AC_CHECK_FILE("$i/ruby",RUBY="$i/ruby")
					if ( test -n "$RUBY" )
					then
						break
					fi
				done
			fi
		fi
		
		if ( test -n "$RUBY" )
		then
			HAVE_RUBY="yes"
		else
			HAVE_RUBY=""
			AC_MSG_WARN(The Ruby API will not be built.)
		fi
	fi

	AC_SUBST(HAVE_RUBY)
	AC_SUBST(RUBY)
fi
])



AC_DEFUN([FW_CHECK_JAVA],
[
if ( test "$ENABLE_JAVA" = "yes" )
then

	HAVE_JAVA=""
	JAVAC=""
	JAVAINCLUDES=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else
		
		if ( test -z "$JAVAPATH" )
		then
			for i in `ls -d /usr/java/jdk* /usr/java/j2sdk* /usr/local/jdk* /usr/local/java 2>/dev/null`
			do
				if ( test -z "$JAVAPATH" )
				then
					AC_CHECK_FILE("$i/include/jni.h",JAVAPATH="$i")
				else
					break
				fi
			done
		fi
		
		if ( test -z "$JAVAPATH" )
		then
			AC_CHECK_FILE("/usr/local/java/include/jni.h",JAVAPATH="/usr/local/java")
		fi
		
		if ( test -z "$JAVAPATH" )
		then
			AC_CHECK_FILE("/usr/java/include/jni.h",JAVAPATH="/usr/java")
		fi
		
		if ( test -n "$JAVAPATH" )
		then
			AC_CHECK_FILE("$JAVAPATH/bin/javac",JAVAC="$JAVAPATH/bin/javac")
			AC_CHECK_FILE("$JAVAPATH/include/jni.h",JAVAINCLUDES="-I$JAVAPATH/include")
			if ( test -n "$JAVAINCLUDES" )
			then
				for i in `ls -d $JAVAPATH/include/*`
				do
					if ( test -d "$i" )
					then
						JAVAINCLUDES="$JAVAINCLUDES -I$i"
					fi
				done
			fi
		fi
		
		if ( test -n "$JAVAC" -a -n "$JAVAINCLUDES" )
		then
			HAVE_JAVA="yes"
		else
			HAVE_JAVA=""
			AC_MSG_WARN(The Java API will not be built.)
		fi
	fi
		
	AC_SUBST(HAVE_JAVA)
	AC_SUBST(JAVAC)
	AC_SUBST(JAVAINCLUDES)
fi
])


AC_DEFUN([FW_CHECK_PHP],
[
if ( test "$ENABLE_PHP" = "yes" )
then

	HAVE_PHP=""
	PHPINCLUDES=""
	PHPEXTDIR=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else
	
		PHPCONFIG=""
		if ( test -n "$PHPPATH" )
		then
			AC_CHECK_FILE("$PHPPATH/bin/php-config",PHPCONFIG="$PHPPATH/bin/php-config")
		else
			AC_CHECK_PROG(PHPCONFIG,"php-config","php-config")
			if ( test -z "$PHPCONFIG" )
			then
				for i in "/usr/local/php/bin" "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/opt/sfw/bin"
				do
					AC_CHECK_FILE("$i/php-config",PHPCONFIG="$i/php-config")
					if ( test -n "$PHPCONFIG" )
					then
						break
					fi
				done
			fi
		fi
		
		if ( test -n "$PHPCONFIG" )
		then
			HAVE_PHP="yes"
			PHPPREFIX=`$PHPCONFIG --prefix`
			dnl some php's fail to replace ${prefix} with their prefix when
			dnl you run php-config --includes, but php-config --prefix
			dnl usually works so we fake it here
			PHPINCLUDES=`$PHPCONFIG --includes | sed -e "s|\\${prefix}|$PHPPREFIX|" | sed -e "s|\\${prefix}|$PHPPREFIX|" | sed -e "s|\\${prefix}|$PHPPREFIX|" | sed -e "s|\\${prefix}|$PHPPREFIX|"`
			PHPEXTDIR=`$PHPCONFIG --extension-dir`
		else
			HAVE_PHP=""
			AC_MSG_WARN(The PHP API will not be built.)
		fi
		
	fi

	AC_SUBST(HAVE_PHP)
	AC_SUBST(PHPINCLUDES)
	AC_SUBST(PHPEXTDIR)
fi
])



AC_DEFUN([FW_CHECK_TCL],
[
if ( test "$ENABLE_TCL" = "yes" )
then

	HAVE_TCL=""
	TCLINCLUDE=""
	TCLLIB=""
	TCLLIBSPATH=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else

		dnl Checks for TCL.
		if ( test -n "$TCLINCLUDEPATH" )
		then
			AC_CHECK_FILE($TCLINCLUDEPATH/tcl.h,TCLINCLUDE="-I$TCLINCLUDEPATH")
		else
			for i in "/usr/include" "$prefix/include" "/usr/local/include" "/usr/pkg/include" "/opt/sfw/include"
			do
				AC_CHECK_FILE($i/tcl.h,TCLINCLUDE="-I$i")
				for j in "tcl8.2" "tcl8.3" "tcl8.4" "tcl8.5"
				do
					AC_CHECK_FILE($i/$j/tcl.h,TCLINCLUDE="-I$i/$j")
				done
			done
		fi
		if ( test -z "$TCLINCLUDE" )
		then
			AC_MSG_WARN("The TCL API will not be installed.")
		else
			if ( test -n "$TCLLIBSPATH" )
			then
				AC_CHECK_FILE($TCLLIBSPATH/libtclstub.a,TCLLIB="-L$TCLLIBSPATH -ltclstub"; TCLINCLUDE="-DUSE_TCL_STUBS $TCLINCLUDE")
				for i in "8.2" "8.3" "8.4" "8.5" "82" "83" "84" "85"
				do
					AC_CHECK_FILE($TCLLIBSPATH/libtclstub$i.a, TCLLIB="-L$TCLLIBSPATH -ltclstub$i"; TCLINCLUDE="-DUSE_TCL_STUBS $TCLINCLUDE")
				done
				if ( test -z "$TCLLIB" )
				then
					AC_CHECK_FILE($TCLLIBSPATH/libtcl.so,TCLLIB="-L$TCLLIBSPATH -ltcl")
					for i in "8.2" "8.3" "8.4" "8.5" "82" "83" "84" "85"
					do
						AC_CHECK_FILE($TCLLIBSPATH/libtcl$i.so,TCLLIB="-L$TCLLIBSPATH -ltcl$i")
					done
				fi
			else
				for i in "/usr/lib" "$prefix/lib" "/usr/local/lib" "/usr/pkg/lib" "/opt/sfw/lib"
				do
					for j in "" "8.2" "8.3" "8.4" "8.5" "82" "83" "84" "85"
					do
						AC_CHECK_FILE($i/libtclstub$j.a,TCLLIB="-L$i -ltclstub$j"; TCLLIBSPATH="$i"; TCLINCLUDE="-DUSE_TCL_STUBS $TCLINCLUDE")
					done
				done
				if ( test -z "$TCLLIB" )
				then
					for i in "/usr/lib" "$prefix/lib" "/usr/local/lib" "/usr/pkg/lib" "/opt/sfw/lib"
					do
						for j in "" "8.2" "8.3" "8.4" "8.5" "82" "83" "84" "85"
						do
							AC_CHECK_FILE($i/libtcl$j.so,TCLLIB="-L$i -ltcl$j"; TCLLIBSPATH="$i")
						done
					done
				fi
			fi
		fi
		if ( test -z "$TCLLIB" )
		then
			AC_MSG_WARN("The TCL API will not be installed.")
		else
			HAVE_TCL="yes"
		fi
	fi

	AC_SUBST(HAVE_TCL)
	AC_SUBST(TCLINCLUDE)
	AC_SUBST(TCLLIB)
	AC_SUBST(TCLLIBSPATH)
fi
])


AC_DEFUN([FW_CHECK_GTK],
[
if ( test "$ENABLE_GTK" = "yes" )
then

	HAVE_GTK=""
	GTKVER=""
	GTKCONFIG=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else
		
		if ( test -n "$GTKPATH" )
		then
			AC_CHECK_FILE("$GTKPATH/bin/gtk-config",GTKCONFIG="$GTKPATH/bin/gtk-config")
		else
			AC_CHECK_PROG(GTKCONFIG,"gtk-config","gtk-config")
			if ( test -z "$GTKCONFIG" )
			then
				for i in "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/usr/local/gtk/bin" "/usr/local/gtk+/bin" "/opt/sfw/bin"
				do
					for j in "gtk-config" "gtk12-config" "gtk10-config"
					do
						AC_CHECK_FILE("$i/$j",GTKCONFIG="$i/$j")
						if ( test -n "$GTKCONFIG" )
						then	
							break
						fi
					done
				done
			fi
		fi
		
		if ( test -n "$GTKCONFIG" )
		then
			GTKVER=`$GTKCONFIG --version | cut -d'.' -f2`
			HAVE_GTK="yes"
		else
			HAVE_GTK=""
			AC_MSG_WARN(GTK config tool will not be built.)
		fi
	fi
		
	AC_SUBST(HAVE_GTK)
	AC_DEFINE_UNQUOTED(GTK_VERSION,$GTKVER,Second decimal of the version of gtk)
	AC_SUBST(GTKCONFIG)
fi
])
