AC_DEFUN([FW_VERSION],
[
if ( test -n "$2" )
then
	echo "$1 version... $2"
fi
])

AC_DEFUN([FW_INCLUDES],
[
if ( test -n "$2" )
then
	echo "$1 includes... $2"
fi
])

AC_DEFUN([FW_LIBS],
[
if ( test -n "$2" )
then
	echo "$1 libs... $2"
fi
])

AC_DEFUN([FW_CHECK_FILE],
[
if ( test -r "$1" )
then
	eval "$2"
fi
])

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
FW_CHECK_FILE($1,[FOUNDLIB=\"yes\"])
if ( test -n "$FOUNDLIB" )
then
	eval "$2"
else
	if ( test -n "$3" )
	then
		FW_CHECK_FILE($3,[FOUNDLIB=\"yes\"])
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
FW_CHECK_FILE([$1],[FOUNDHEADER=\"yes\"])
FW_CHECK_FILE([$3],[FOUNDLIB=\"yes\"])
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
		FW_CHECK_FILE([$5],[FOUNDLIB=\"yes\"])
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

SEARCHPATH=$1
NAME=$2
HEADER=$3
LIBNAME=$4
LINKSTATIC=$5
LINKRPATH=$6
USEFULLLIBPATH=$12
INCLUDESTRING=""
LIBSTRING=""
LIBPATH=""
STATIC=""
HEADERSANDLIBSPATH=""

eval "$7=\"\""
eval "$8=\"\""
eval "$9=\"\""
eval "$10=\"\""
if ( test -n "$11" )
then
	eval "$11=\"\""
fi


for paths in "$SEARCHPATH" "/" "/usr" "/usr/local/$NAME" "/opt/$NAME" "/usr/$NAME" "/usr/local" "/usr/pkg" "/opt/sfw" "/opt/sfw/$NAME" "/usr/local/firstworks"
do
	if ( test -n "$paths" )
	then

		if ( test "$paths" = "/" )
		then
			dnl look in /usr/include and /lib
			if ( test "$USEFULLLIBPATH" = "yes" )
			then
				FW_CHECK_HEADER_LIB([/usr/include/$HEADER],[],[/lib/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"/lib\"; LIBSTRING=\"/lib/lib$LIBNAME.$SOSUFFIX\"],[/lib/lib$LIBNAME.a],[LIBSTRING=\"/lib/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
			else
				FW_CHECK_HEADER_LIB([/usr/include/$HEADER],[],[/lib/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"/lib\"; LIBSTRING=\"-l$LIBNAME\"],[/lib/lib$LIBNAME.a],[LIBSTRING=\"-l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi

			dnl set paths to "" so we won't get //'s from here on
			paths=""
		fi

		dnl look in $path/include and $path/lib
		if ( test "$USEFULLLIBPATH" = "yes" )
		then
			FW_CHECK_HEADER_LIB([$paths/include/$HEADER],[INCLUDESTRING=\"-I$paths/include\"],[$paths/lib/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib\"; LIBSTRING=\"$paths/lib/lib$LIBNAME.$SOSUFFIX\"],[$paths/lib/lib$LIBNAME.a],[LIBSTRING=\"$paths/lib/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
		else
			FW_CHECK_HEADER_LIB([$paths/include/$HEADER],[INCLUDESTRING=\"-I$paths/include\"],[$paths/lib/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib\"; LIBSTRING=\"-L$paths/lib -l$LIBNAME\"],[$paths/lib/lib$LIBNAME.a],[LIBSTRING=\"-L$paths/lib -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
		fi

		dnl look in $path/include/$NAME and $path/lib
		if ( test -z "$LIBSTRING" )
		then
			if ( test "$USEFULLLIBPATH" = "yes" )
			then
				FW_CHECK_HEADER_LIB([$paths/include/$NAME/$HEADER],[INCLUDESTRING=\"-I$paths/include/$NAME\"],[$paths/lib/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib\"; LIBSTRING=\"$paths/lib/lib$LIBNAME.$SOSUFFIX\"],[$paths/lib/lib$LIBNAME.a],[LIBSTRING=\"$paths/lib/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
			else
				FW_CHECK_HEADER_LIB([$paths/include/$NAME/$HEADER],[INCLUDESTRING=\"-I$paths/include/$NAME\"],[$paths/lib/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib\"; LIBSTRING=\"-L$paths/lib -l$LIBNAME\"],[$paths/lib/lib$LIBNAME.a],[LIBSTRING=\"-L$paths/lib -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi
		fi

		dnl look in $path/include and $path/lib/$NAME
		if ( test -z "$LIBSTRING" )
		then
			if ( test "$USEFULLLIBPATH" = "yes" )
			then
				FW_CHECK_HEADER_LIB([$paths/include/$HEADER],[INCLUDESTRING=\"-I$paths/include\"],[$paths/lib/$NAME/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib/$NAME\"; LIBSTRING=\"$paths/lib/$NAME/lib$LIBNAME.$SOSUFFIX\"],[$paths/lib/$NAME/lib$LIBNAME.a],[LIBSTRING=\"$paths/lib/$NAME/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
			else
				FW_CHECK_HEADER_LIB([$paths/include/$HEADER],[INCLUDESTRING=\"-I$paths/include\"],[$paths/lib/$NAME/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib/$NAME\"; LIBSTRING=\"-L$paths/lib/$NAME -l$LIBNAME\"],[$paths/lib/$NAME/lib$LIBNAME.a],[LIBSTRING=\"-L$paths/lib/$NAME -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi
		fi

		dnl look in $path/include/$NAME and $path/lib/$NAME
		if ( test -z "$LIBSTRING" )
		then
			if ( test "$USEFULLLIBPATH" = "yes" )
			then
				FW_CHECK_HEADER_LIB([$paths/include/$NAME/$HEADER],[INCLUDESTRING=\"-I$paths/include/$NAME\"],[$paths/lib/$NAME/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib/$NAME\"; LIBSTRING=\"$paths/lib/$NAME/lib$LIBNAME.$SOSUFFIX\"],[$paths/lib/$NAME/lib$LIBNAME.a],[LIBSTRING=\"$paths/lib/$NAME/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
			else
				FW_CHECK_HEADER_LIB([$paths/include/$NAME/$HEADER],[INCLUDESTRING=\"-I$paths/include/$NAME\"],[$paths/lib/$NAME/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib/$NAME\"; LIBSTRING=\"-L$paths/lib/$NAME -l$LIBNAME\"],[$paths/lib/$NAME/lib$LIBNAME.a],[LIBSTRING=\"-L$paths/lib/$NAME -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi
		fi

		dnl look in $path/include and $path/lib/opt
		if ( test -z "$LIBSTRING" )
		then
			if ( test "$USEFULLLIBPATH" = "yes" )
			then
				FW_CHECK_HEADER_LIB([$paths/include/$HEADER],[INCLUDESTRING=\"-I$paths/include\"],[$paths/lib/opt/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib/opt\"; LIBSTRING=\"$paths/lib/opt/lib$LIBNAME.$SOSUFFIX\"],[$paths/lib/opt/lib$LIBNAME.a],[LIBSTRING=\"$paths/lib/opt/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
			else
				FW_CHECK_HEADER_LIB([$paths/include/$HEADER],[INCLUDESTRING=\"-I$paths/include\"],[$paths/lib/opt/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib/opt\"; LIBSTRING=\"-L$paths/lib/opt -l$LIBNAME\"],[$paths/lib/opt/lib$LIBNAME.a],[LIBSTRING=\"-L$paths/lib/opt -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi
		fi

		dnl look in $path/include/$NAME and $path/lib/opt
		if ( test -z "$LIBSTRING" )
		then
			if ( test "$USEFULLLIBPATH" = "yes" )
			then
				FW_CHECK_HEADER_LIB([$paths/include/$NAME/$HEADER],[INCLUDESTRING=\"-I$paths/include/$NAME\"],[$paths/lib/opt/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib/opt\"; LIBSTRING=\"$paths/lib/opt/lib$LIBNAME.$SOSUFFIX\"],[$paths/lib/opt/lib$LIBNAME.a],[LIBSTRING=\"$paths/lib/opt/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
			else
				FW_CHECK_HEADER_LIB([$paths/include/$NAME/$HEADER],[INCLUDESTRING=\"-I$paths/include/$NAME\"],[$paths/lib/opt/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$paths/lib/opt\"; LIBSTRING=\"-L$paths/lib/opt -l$LIBNAME\"],[$paths/lib/opt/lib$LIBNAME.a],[LIBSTRING=\"-L$paths/lib/opt -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi
		fi
		if ( test -n "$LIBSTRING" )
		then
			HEADERSANDLIBSPATH="$paths"
			break
		fi
	fi
done

dnl remove -I/usr/include, -L/lib and -L/usr/lib
INCLUDESTRING=`echo $INCLUDESTRING | sed -e "s|-I/usr/include$||g" -e "s|-I/usr/include ||g"`
LIBSTRING=`echo $LIBSTRING | sed -e "s|-L/usr/lib$||g" -e "s|-L/lib$||g" -e "s|-L/usr/lib ||g" -e "s|-L/lib ||g"`

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
  LIBTOOLGCC="libtool"
else
  LIBTOOLGCC="$LIBTOOL.gcc"
fi
AC_SUBST(LIBTOOLGCC)
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


dnl figures out whether to run  ps -efal or ps aux
dnl sets the substitution variable PS
AC_DEFUN([FW_CHECK_PS],
[
AC_MSG_CHECKING(whether ps aux works)
INVALID="`ps aux 2>&1 | grep illegal | grep -v grep`"
DEPRECATED="`ps aux 2>&1 | grep deprecated | grep -v grep`"
USAGE="`ps aux 2>&1 | grep usage | grep -v grep`"
if ( test -n "$INVALID" -o -n "$DEPRECATED" -o -n "$USAGE" -o -n "$CYGWIN" )
then
	PS="ps\ \-efal"
	AC_MSG_RESULT(no)
else
	PS="ps\ aux"
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


dnl checks to see if -pipe option to gcc works or not
AC_DEFUN([FW_CHECK_PIPE],
[
AC_MSG_CHECKING(for -pipe option)
FW_TRY_LINK([#include <stdio.h>],[printf("hello");],[-pipe],[],[],[PIPE="-pipe"],[PIPE=""])
if ( test -n "$PIPE" )
then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AC_SUBST(PIPE)
])



dnl Checks for microsoft platform.
dnl sets the substitution variables MINGW32, CYGWIN and UWIN as appropriate
dnl also moves INSTALL to INSTALL.txt if we're using windows
dnl sets the enviroment variable MICROSOFT
AC_DEFUN([FW_CHECK_MICROSOFT],
[
CYGWIN=""
CYGWINDEFINES=""
MINGW32=""
UWIN=""
dnl AC_CANONICAL_HOST gets called when AC_PROG_LIBTOOL is called
case $host_os in
	*cygwin* ) CYGWIN="yes";;
	*mingw32* ) MINGW32="yes";;
	*uwin* ) UWIN="yes";;
esac
EXE=""
AC_SUBST(MINGW32)
AC_SUBST(CYGWIN)
AC_SUBST(UWIN)

dnl Hack so "make install" will work on windows.
MICROSOFT=""
EXE=""
if ( test "$UWIN" = "yes" -o "$MINGW32" = "yes" -o "$CYGWIN" = "yes" )
then
	if ( test -r "INSTALL" )
	then
		mv INSTALL INSTALL.txt
	fi
	MICROSOFT="yes"
	EXE=".exe"
fi
AC_SUBST(EXE)
AC_SUBST(MICROSOFT)
])


AC_DEFUN([FW_CHECK_OSX],
[
	PYTHONFRAMEWORK=""
	if ( test "`uname -s`" = "Darwin" -a -r "INSTALL" )
	then
		mv INSTALL INSTALL.txt
		PYTHONFRAMEWORK="-framework Python"
	fi
	AC_SUBST(PYTHONFRAMEWORK)
])


dnl Determines what extension shared object files have
AC_DEFUN([FW_CHECK_SO_EXT],
[
AC_MSG_CHECKING(for dynamic library extension)
if ( test -n "$CYGWIN" )
then
	SOSUFFIX="dll.a"
else
	if ( test "`uname -s`" = "Darwin" )
	then
		SOSUFFIX="dylib"
	else
		SOSUFFIX="so"
	fi
fi
AC_MSG_RESULT($SOSUFFIX)
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


AC_DEFUN([FW_CXX_NAMESPACES],
[
	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS
	SQLRELAY_NAMESPACE=""
	AC_MSG_CHECKING(namespace support)
	AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],[using namespace Outer::Inner; return i;],[SQLRELAY_NAMESPACE="yes"],[])
	AC_LANG_RESTORE
	if ( test "$SQLRELAY_NAMESPACE" = yes )
	then
		AC_MSG_RESULT(yes)
  		AC_DEFINE(SQLRELAY_NAMESPACE,1,Compiler supports namespaces)
	else
		AC_MSG_RESULT(no)
	fi
])


dnl checks for the pthreads library
dnl requires:  PTHREADPATH, RPATHFLAG, cross_compiling
dnl sets the substitution variable PTHREADLIBS
AC_DEFUN([FW_CHECK_PTHREAD],
[

HAVE_PTHREAD=""
PTHREADINCLUDES=""
PTHREADLIBS=""

if ( test "$cross_compiling" = "yes" )
then
	
	dnl cross compiling
	echo "cross compiling"
	if ( test -n "$PTHREADPATH" )
	then
		PTHREADINCLUDES="-I$PTHREADPATH/include"
		PTHREADLIBS="-L$PTHREADPATH/lib -lpthread"
	else
		PTHREADLIBS="-lpthread"
	fi
	HAVE_PTHREAD="yes"

else

	for i in "pthread" "c_r" "gthreads"
	do
		FW_CHECK_HEADERS_AND_LIBS([$PTHREADPATH],[pthread],[pthread.h],[$i],[""],[""],[PTHREADINCLUDES],[PTHREADLIBS],[PTHREADLIBPATH],[PTHREADSTATIC])
		if ( test -n "$PTHREADLIBS" )
		then
			if ( test "$i" = "c_r" )
			then
				PTHREADLIBS="$PTHREADLIBS -pthread"
			fi
			break
		fi
	done
	if ( test -n "$PTHREADLIBS" )
	then
		HAVE_PTHREAD="yes"
	fi

	dnl override PTHREADLIB on microsoft platforms
	if ( test -n "$PTHREADINCLUDES" -a "$MICROSOFT" = "yes" )
	then
		PTHREADLIBS="-pthread"
	fi
fi

FW_INCLUDES(pthreads,[$PTHREADINCLUDES])
FW_LIBS(pthreads,[$PTHREADLIBS])

AC_SUBST(PTHREADINCLUDES)
AC_SUBST(PTHREADLIBS)
if ( test -z "$HAVE_PTHREAD" )
then
	AC_MSG_ERROR(pthread library not found.  SQL-Relay requires this package.)
	exit
fi
])



dnl checks for the rudiments library
dnl requires:  MICROSOFT, RUDIMENTSPATH, RPATHFLAG, cross_compiling
dnl sets the substitution variables RUDIMENTSLIBS, RUDIMENTSLIBSPATH,
dnl RUDIMENTSLIBSINCLUDES
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
		RUDIMENTSCONFIG="$RUDIMENTSPATH/bin/rudiments-config"
		if ( test -r "$RUDIMENTSCONFIG" )
		then
			RUDIMENTSINCLUDES="`$RUDIMENTSCONFIG --cflags`"
			RUDIMENTSLIBS="`$RUDIMENTSCONFIG --libs`"
		else
			RUDIMENTSINCLUDES="-I$RUDIMENTSPATH/include"
			RUDIMENTSLIBS="-L$RUDIMENTSPATH/lib -lrudiments"
		fi
	fi

else

	for i in "$RUDIMENTSPATH" "/usr" "/usr/local" "/opt/sfw" "/usr/pkg" "/usr/local/firstworks"
	do
		if ( test -n "$i" )
		then
			RUDIMENTSCONFIG="$i/bin/rudiments-config"
			if ( test -r "$RUDIMENTSCONFIG" )
			then
				RUDIMENTSINCLUDES="`$RUDIMENTSCONFIG --cflags`"
				RUDIMENTSLIBS="`$RUDIMENTSCONFIG --libs`"
			fi
		fi
		if ( test -n "$RUDIMENTSLIBS" )
		then
			break
		fi
	done
fi

if ( test -z "$RUDIMENTSLIBS" )
then
	AC_MSG_ERROR(Rudiments not found.  SQL-Relay requires this package.)
	exit
fi

FW_INCLUDES(rudiments,[$RUDIMENTSINCLUDES])
FW_LIBS(rudiments,[$RUDIMENTSLIBS])

AC_SUBST(RUDIMENTSPATH)
AC_SUBST(RUDIMENTSINCLUDES)
AC_SUBST(RUDIMENTSLIBS)
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

		if ( test -n "$CYGWIN" )
		then
			for ORACLE_HOME in "`ls -d /cygdrive/c/oracle/product/*/*/OCI`"
			do
				instance=`dirname $ORACLE_HOME`
				home=`dirname $instance`
				version=`basename $home | cut -f1 -d'.'`
				if ( test "$version" = "10" )
				then
					ORACLEVERSION="10g"
				fi
				if ( test "$version" = "9" )
				then
					ORACLEVERSION="9i"
				fi
				if ( test -n "`echo $version | grep 8.1`" )
				then
					ORACLEVERSION="8i"
				fi
				if ( test -n "`echo $version | grep 8.0`" )
				then
					ORACLEVERSION="8"
				fi
				if ( test "$version" = "7" )
				then
					ORACLEVERSION="7"
				fi
				FW_CHECK_LIB([$ORACLE_HOME/lib/MSVC/oci.lib],[ORACLELIBSPATH=\"$ORACLE_HOME/lib/MSVC\"; ORACLELIBS=\"-L$ORACLE_HOME/lib/MSVC -loci\"])
				if ( test -n "$ORACLELIBS" )
				then
					break
				fi
			done
		else

			AC_MSG_CHECKING(for ORACLE_HOME)
		
			if ( test -n "$ORACLE_HOME" )
			then

				AC_MSG_RESULT(yes)

				dnl use sysliblist if it's there
				SYSLIBLIST="`cat $ORACLE_HOME/lib/sysliblist`"
				if ( test ! -n "$SYSLIBLIST" )
				then
					SYSLIBLIST="-lm $AIOLIB"
				fi
	
				FW_CHECK_LIB([$ORACLE_HOME/lib/libcore3.a],[ORACLEVERSION=\"7\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclient -lsqlnet -lncr -lsqlnet -lcommon -lgeneric -lnlsrtl3 -lcore3 -lnlsrtl3 -lcore3 -lc3v6 -lepc -lcore3 -lnsl $SYSLIBLIST\"])
				FW_CHECK_LIB([$ORACLE_HOME/lib/libcore4.a],[ORACLEVERSION=\"8.0\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclient -lncr -lcommon -lgeneric -lclntsh -lepcpt -lcore4 -lnlsrtl3 $SYSLIBLIST\"])
				FW_CHECK_LIB([$ORACLE_HOME/lib/libcore8.a],[ORACLEVERSION=\"8i\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclntsh $SYSLIBLIST\"])
				FW_CHECK_LIB([$ORACLE_HOME/lib/libcore9.a],[ORACLEVERSION=\"9i\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclntsh $SYSLIBLIST\"])
				FW_CHECK_LIB([$ORACLE_HOME/lib/libcore10.a],[ORACLEVERSION=\"10g\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclntsh $SYSLIBLIST\"])
				FW_CHECK_LIB([$ORACLE_HOME/lib/libclntsh.a],[ORACLESTATIC=\"$STATICFLAG\"])
			else
				AC_MSG_RESULT(no)
				AC_MSG_WARN(The ORACLE_HOME environment variable is not set.  Oracle support will not be built.)
			fi

		fi
		
		if ( test -n "$ORACLEVERSION" )
		then
			if ( test -n "$CYGWIN" )
			then
				ORACLEINCLUDES="-I$ORACLE_HOME/include"
			else
				ORACLEINCLUDES="-I$ORACLE_HOME/rdbms/demo -I$ORACLE_HOME/rdbms/public -I$ORACLE_HOME/network/public -I$ORACLE_HOME/plsql/public"
			fi
			echo "hmmm, looks like Oracle$ORACLEVERSION..."
		fi
		
		OCI_H=""
		if ( test -n "$ORACLELIBS" )
		then
			AC_MSG_CHECKING(if Oracle has oci.h)
			if ( test -n "$RPATHFLAG" -a -n "$ORACLELIBSPATH" )
			then
				FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <oci.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[exit(0)],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); OCI_H=\"yes\"],[AC_MSG_RESULT(no)])
			else
				FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <oci.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[exit(0)],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); OCI_H=\"yes\"],[AC_MSG_RESULT(no)])
			fi
		fi
		
		
		LINKFAIL=""
		if ( test -n "$ORACLESTATIC" -a -n "$ORACLELIBS" )
		then
			AC_MSG_CHECKING(if Oracle can be statically linked without $DLLIB)
			if ( test -n "$OCI_H" )
			then
				FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <oci.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			else
				FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <ociapr.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			fi
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Oracle can be statically linked with $DLLIB)
				if ( test -n "$OCI_H" )
				then
					FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <oci.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); ORACLESTATIC=""; LINKFAIL="yes"])
				else
					FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <ociapr.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES,$ORACLELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); ORACLESTATIC=""; LINKFAIL="yes"])
				fi
			fi
		fi
		
		if ( test -z "$ORACLESTATIC" -a -n "$ORACLELIBS" )
		then
			AC_MSG_CHECKING(if Oracle can be dynamically linked without $DLLIB)
			if ( test -n "$OCI_H" )
			then
				FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <oci.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,NULL,-1,NULL,-1,NULL,-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			else
				FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <ociapr.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,NULL,-1,NULL,-1,NULL,-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			fi
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Oracle can be dynamically linked with $DLLIB)
				if ( test -n "$OCI_H" )
				then
					FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <oci.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,NULL,-1,NULL,-1,NULL,-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); LINKFAIL="yes"])
				else
					FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <ociapr.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,NULL,-1,NULL,-1,NULL,-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); LINKFAIL="yes"])
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
		
	if ( test "$ORACLEVERSION" = "8i" -o "$ORACLEVERSION" = "9i" -o "$ORACLEVERSION" = "10g" )
	then
		AC_DEFINE(HAVE_ORACLE_8i,1,Oracle 8i or greater)
	fi

	if ( test -n $"ORACLEVERSION" )
	then
		FW_VERSION(oracle,[$ORACLEVERSION])
		FW_INCLUDES(oracle,[$ORACLEINCLUDES])
		FW_LIBS(oracle,[$ORACLELIBS])
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
			MYSQLLIBS="$MYSQLPATH/lib/mysql/libmysqlclient.$SOSUFFIX"
			MYSQLLIBSPATH="$MYSQLPATH/lib/mysql"
		fi

	else

		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi

		if ( test -n "$CYGWIN" )
		then
			FW_CHECK_HEADER_LIB([/cygdrive/c/mysql/include/mysql.h],[MYSQLINCLUDES=\"-I/cygdrive/c/mysql/include\"],[/cygdrive/c/mysql/lib/opt/libmysql.lib],[MYSQLLIBSPATH=\"/cygdrive/c/mysql/lib/opt\"; MYSQLLIBS=\"-L/cygdrive/c/mysql/lib/opt -lmysql\"],[/cygdrive/c/mysql/lib/opt/mysqlclient.lib],[MYSQLLIBS=\"/cygdrive/c/mysql/lib/opt/mysqlclient.lib\"; MYSQLSTATIC=\"$STATICFLAG\"])
		fi

		if ( test -z "$MYSQLLIBS" )
		then
			MYSQLINCLUDES=`mysql_config --cflags 2> /dev/null | sed -e "s|'||g"`
			MYSQLLIBS=`mysql_config --libs 2> /dev/null | sed -e "s|'||g"`
		fi

		if ( test -z "$MYSQLLIBS" )
		then
			for i in "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/usr/local/mysql/bin" "/opt/sfw/bin" "/opt/sfw/mysql/bin"
			do
				MYSQLINCLUDES=`$i/mysql_config --cflags 2> /dev/null | sed -e "s|'||g"`
				MYSQLLIBS=`$i/mysql_config --libs 2> /dev/null | sed -e "s|'||g"`
				if ( test -n "$MYSQLLIBS" )
				then
					break
				fi
			done
		fi

		if ( test -z "$MYSQLLIBS" )
		then
			FW_CHECK_HEADERS_AND_LIBS([$MYSQLPATH],[mysql],[mysql.h],[mysqlclient],[$STATICFLAG],[$RPATHFLAG],[MYSQLINCLUDES],[MYSQLLIBS],[MYSQLLIBSPATH],[MYSQLSTATIC],[dummy],[yes])

			if ( test -n "$MYSQLLIBS" -a -z "$MICROSOFT" )
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
		fi
		
		if ( test "$MYSQLINCLUDES" = "-I/usr/include" )
		then
			MYSQLINCLUDES=""
		fi
	
		if ( test -z "$MYSQLLIBS" )
		then
			AC_MSG_WARN(MySQL support will not be built.)
		else
			FW_CHECK_MYSQL_FUNCTIONS()
		fi
	fi

	FW_INCLUDES(mysql,[$MYSQLINCLUDES])
	FW_LIBS(mysql,[$MYSQLLIBS])
		
	AC_SUBST(MYSQLINCLUDES)
	AC_SUBST(MYSQLLIBS)
	AC_SUBST(MYSQLLIBSPATH)
	AC_SUBST(MYSQLSTATIC)
fi
])

AC_DEFUN([FW_CHECK_MYSQL_FUNCTIONS],
[
	AC_MSG_CHECKING(for mysql_real_connect)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_real_connect(NULL,NULL,NULL,NULL,NULL,0,NULL,0);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_REAL_CONNECT_FOR_SURE,1,MySQL supports mysql_real_connect)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_select_db)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_select_db(NULL,NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_SELECT_DB,1,MySQL supports mysql_select_db)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_ping)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_ping(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_PING,1,MySQL supports mysql_ping)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_change_user)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_change_user(NULL,NULL,NULL,NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_CHANGE_USER,1,MySQL supports mysql_change_user)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_commit)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_commit(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_COMMIT,1,MySQL supports mysql_commit)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_rollback)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_rollback(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_ROLLBACK,1,MySQL supports mysql_rollback)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_autocommit)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_autocommit(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_AUTOCOMMIT,1,MySQL supports mysql_autocommit)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_prepare)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_prepare(NULL,NULL,0);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_PREPARE,1,MySQL supports mysql_prepare)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for CR_SERVER_GONE_ERROR)
	FW_TRY_LINK([#include <mysql.h>
#include <errmsg.h>
#include <stdlib.h>],[int err=CR_SERVER_GONE_ERROR;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_CR_SERVER_GONE_ERROR,1,MySQL supports CR_SERVER_GONE_ERROR)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for CR_SERVER_LOST)
	FW_TRY_LINK([#include <mysql.h>
#include <errmsg.h>
#include <stdlib.h>],[int err=CR_SERVER_LOST;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_CR_SERVER_LOST,1,MySQL supports CR_SERVER_LOST)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for FIELD_TYPE_YEAR)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[MYSQL_FIELD field; field.type=FIELD_TYPE_YEAR;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_FIELD_TYPE_YEAR,1,MySQL supports FIELD_TYPE_YEAR)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for FIELD_TYPE_NEWDATE)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[MYSQL_FIELD field; field.type=FIELD_TYPE_NEWDATE;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_FIELD_TYPE_NEWDATE,1,MySQL supports FIELD_TYPE_NEWDATE)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for FIELD_TYPE_ENUM)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[MYSQL_FIELD field; field.type=FIELD_TYPE_ENUM;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_FIELD_TYPE_ENUM,1,MySQL supports FIELD_TYPE_ENUM)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for FIELD_TYPE_SET)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[MYSQL_FIELD field; field.type=FIELD_TYPE_SET;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_FIELD_TYPE_SET,1,MySQL supports FIELD_TYPE_SET)],[AC_MSG_RESULT(no)])

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
				FW_CHECK_HEADER_LIB([$i/include/msql.h],[MSQLINCLUDES=\"-I$i/include\"; MSQLPATH=\"$i\"],[$i/lib/libmsql.$SOSUFFIX],[MSQLLIBSPATH=\"$i/lib\"; MSQLLIBS=\"-L$i/lib -lmsql\"],[$i/lib/libmsql.a],[MSQLLIBS=\"-L$i/lib -lmsql\"])
				if ( test -n "$MSQLLIBS" )
				then
					break
				fi
			fi
		done
	
		FW_CHECK_HEADER_LIB([/usr/include/msql.h],[MSQLINCLUDES=\"\"; MSQLPATH=\"\"],[/usr/lib/libmsql.$SOSUFFIX],[MSQLLIBSPATH=\"\"; MSQLLIBS=\"-lmsql\"],[/usr/lib/libmsql.a],[MSQLLIBS=\"-lmsql\"])
		
		if ( test -z "$MSQLLIBS" )
		then
			AC_MSG_WARN(mSQL support will not be built.)
		fi
	fi

	FW_INCLUDES(msql,[$MSQLINCLUDES])
	FW_LIBS(msql,[$MSQLLIBS])
		
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

	if ( test -n "$POSTGRESQLLIBS" )
	then
		AC_MSG_CHECKING(if PostgreSQL has PQfmod)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQfmod(NULL,0);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQFMOD,1,Some versions of postgresql have PQfmod)],[AC_MSG_RESULT(no)])
		AC_MSG_CHECKING(if PostgreSQL has PQsetNoticeProcessor)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetNoticeProcessor(NULL,NULL,NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR,1,Some versions of postgresql have PQsetNoticeProcessor)],[AC_MSG_RESULT(no)])
	fi

	FW_INCLUDES(postgresql,[$POSTGRESQLINCLUDES])
	FW_LIBS(postgresql,[$POSTGRESQLLIBS])
		
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

FW_LIBS(gdbm,[$GDBMLIBS])

AC_SUBST(GDBMLIBS)
])


AC_DEFUN([FW_CHECK_GLIB],
[

if ( test "$cross_compiling" = "yes" )
then
	dnl cross compiling ...
	echo "cross compiling"
	if ( test -n "$GLIBPATH" )
	then
		GLIBINCLUDES="-I$GLIBPATH/include/glib-2.0 -I$GLIBPATH/lib/glib/include"
		GLIBLIBS="-L$GLIBPATH/lib -lglib-2.0"
		GLIBLIBSPATH="$GLIBPATH/lib"
	fi
else
	GLIBINCLUDES="`pkg-config glib-2.0 --cflags 2> /dev/null`"
	GLIBLIBS="`pkg-config glib-2.0 --libs 2> /dev/null`"
	if ( test -z "$GLIBINCLUDES" -a -z "$GLIBLIBS" )
	then
		FW_CHECK_HEADERS_AND_LIBS([$LIBGLIBPATH],[glib-2.0],[glib.h],[glib-2.0],[$STATICFLAG],[$RPATHFLAG],[GLIBINCLUDES],[GLIBLIBS],[GLIBLIBSPATH],[GLIBSTATIC])
		if ( test -n "$GLIBINCLUDES" )
		then
			DIRNAME1=`dirname $GLIBINCLUDES`
			DIRNAME2=`dirname $DIRNAME1`
			GLIBINCLUDES="$GLIBINCLUDES $DIRNAME2/lib/glib/include"
		fi
	fi
fi

FW_INCLUDES(glib,[$GLIBINCLUDES])
FW_LIBS(glib,[$GLIBLIBS])

AC_SUBST(GLIBLIBS)
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
	SQLITEVERSION=""

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
		
		if ( test -n "$PTHREADLIBS" )
		then
			FW_CHECK_HEADERS_AND_LIBS([$SQLITEPATH],[sqlite],[sqlite.h],[sqlite],[$STATICFLAG],[$RPATHFLAG],[SQLITEINCLUDES],[SQLITELIBS],[SQLITELIBSPATH],[SQLITESTATIC])
			if ( test -z "$SQLITELIBS" )
			then
				FW_CHECK_HEADERS_AND_LIBS([$SQLITEPATH],[sqlite],[sqlite3.h],[sqlite3],[$STATICFLAG],[$RPATHFLAG],[SQLITEINCLUDES],[SQLITELIBS],[SQLITELIBSPATH],[SQLITESTATIC])
				if ( test -n "$SQLITELIBS" )
				then
					SQLITEVERSION="3"
				fi
			fi
		else
			AC_MSG_WARN(pthreads was not found.)
		fi
			
		if ( test -z "$SQLITELIBS" )
		then
			AC_MSG_WARN(SQLite support will not be built.)
		else
			if ( test -z "$SQLITEVERSION" )
			then
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
	fi

	if ( test "$SQLITEVERSION" = "3" )
	then
		SQLITEINCLUDES="-DSQLITE3 $SQLITEINCLUDES"
		AC_DEFINE_UNQUOTED(SQLITE_TRANSACTIONAL,1,Some versions of sqlite are transactional)
	fi

	FW_INCLUDES(sqlite,[$SQLITEINCLUDES])
	FW_LIBS(sqlite,[$SQLITELIBS])
		
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
			FW_TRY_LINK([#include <lago.h>],[LCTX lctx; lctx=Lnewctx(); Ldelctx(lctx);],[$LAGOSTATIC $LAGOINCLUDES],[$LAGOLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$LAGOLIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); LAGOLIBS="$LAGOLIBS $PTHREADLIBS"])
		fi

	fi

	FW_INCLUDES(lago,[$LAGOINCLUDES])
	FW_LIBS(lago,[$LAGOLIBS])

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
		
		if ( test -n "$FREETDSLIBS" )
		then

			dnl some versions of freetds need libiconv, see if
			dnl a simple test will link
			LINKFAILED=""
			FW_TRY_LINK([],[],[$FREETDSINCLUDES],[$FREETDSLIBS],[$LD_LIBRARY_PATH],[],[LINKFAILED="yes"])

			dnl if not, search for iconv
			if ( test -n "$LINKFAILED" )
			then

				FW_CHECK_HEADERS_AND_LIBS([/usr],[iconv],[iconv.h],[iconv],[$STATICFLAG],[$RPATHFLAG],[ICONVINCLUDES],[ICONVLIBS],[ICONVLIBPATH],[ICONVSTATIC])

				dnl if iconv was found, try the test again,
				dnl using it
				dnl if it was not found, then freetds just
				dnl doesn't work
				if ( test -n "$ICONVLIBS" )
				then
					AC_MSG_CHECKING(whether freetds requires libiconv)

					FW_TRY_LINK([],[],[$FREETDSINCLUDES $ICONVINCLUDES],[$FREETDSLIBS $ICONVLIBS],[$LD_LIBRARY_PATH],[FREETDSINCLUDES="$FREETDSINCLUDES $ICONVINCLUDES"; FREETDSLIBS="$FREETDSLIBS $ICONVLIBS"; AC_MSG_RESULT(yes)],[FREETDSLIBS=""; FREETDSINCLUDES=""; AC_MSG_RESULT(no)])
				else
					FREETDSLIBS=""
					FREETDSINCLUDES=""
				fi
			fi

			dnl if we've figured out how to link a simple freetds
			dnl program, then try using c++ to see if we need
			dnl function definitions or not
			if ( test -n "$FREETDSLIBS" )
			then
				AC_LANG(C++)
				AC_MSG_CHECKING(whether ctpublic.h contains function definitions)
				FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$FREETDSINCLUDES],[$FREETDSLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_FREETDS_FUNCTION_DEFINITIONS,1,Some versions of FreeTDS have function definitions)],[AC_MSG_RESULT(no)])
				AC_LANG(C)
			fi
		fi

		dnl if FREETDSLIBS isn't defined at this point, then freetds
		dnl isn't installed or doesn't work
		if ( test -z "$FREETDSLIBS" )
		then
			AC_MSG_WARN(FreeTDS support will not be built.)
		fi
	fi

	FREETDSSYSTEMWARNING="no"
	if ( test "$FREETDSLIBS" = "-lct" -a -z "$FREETDSINCLUDES" )
	then
		FREETDSSYSTEMWARNING="yes"
	fi

	FW_INCLUDES(freetds,[$FREETDSINCLUDES])
	FW_LIBS(freetds,[$FREETDSLIBS])
		
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
			if ( test -n "$CYGWIN" )
			then
				FW_CHECK_HEADER_LIB([$SYBASEPATH/include/ctpublic.h],[SYBASEINCLUDES=\"-I$SYBASEPATH/include\"],[$SYBASEPATH/dll/libct.dll],[SYBASELIBSPATH=\"$SYBASEPATH/dll\"; SYBASELIBS=\"-L$SYBASEPATH/dll -llibcs -llibct\"],[],[])
			else
				FW_CHECK_HEADER_LIB([$SYBASEPATH/include/ctpublic.h],[SYBASEINCLUDES=\"-I$SYBASEPATH/include\"],[$SYBASEPATH/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"$SYBASEPATH/lib\"; SYBASELIBS=\"-L$SYBASEPATH/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"],[$SYBASEPATH/lib/libct.a],[SYBASELIBS=\"-L$SYBASEPATH/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"])
			fi
		else
		
			if ( test -n "$CYGWIN" )
			then
				FW_CHECK_HEADER_LIB([/cygdrive/c/sybase/OCS-12_5/include/ctpublic.h],[SYBASEINCLUDES=\"-I/cygdrive/c/sybase/OCS-12_5/include\"],[/cygdrive/c/sybase/OCS-12_5/dll/libct.dll],[SYBASELIBSPATH=\"/cygdrive/c/sybase/OCS-12_5/dll\"; SYBASELIBS=\"-L/cygdrive/c/sybase/OCS-12_5/dll -llibct -llibcs\"],[],[])
			else
				FW_CHECK_HEADER_LIB([/usr/local/sybase/include/ctpublic.h],[SYBASEINCLUDES=\"-I/usr/local/sybase/include\"],[/usr/local/sybase/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"/usr/local/sybase/lib\"; SYBASELIBS=\"-L/usr/local/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"],[/usr/local/sybase/lib/libct.a],[SYBASELIBS=\"-L/usr/local/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"])
		
				FW_CHECK_HEADER_LIB([/opt/sybase/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase/include\"],[/opt/sybase/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"/opt/sybase/lib\"; SYBASELIBS=\"-L/opt/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"],[/opt/sybase/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"])
	
				FW_CHECK_HEADER_LIB([/opt/sybase-12.5/OCS-12_5/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase-12.5/OCS-12_5/include\"],[/opt/sybase-12.5/OCS-12_5/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"/opt/sybase-12.5/OCS-12_5/lib\"; SYBASELIBS=\"-L/opt/sybase-12.5/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"],[/opt/sybase-12.5/OCS-12_5/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase-12.5/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"; SYBASESTATIC=\"$STATICFLAG\"])
	
				FW_CHECK_HEADER_LIB([/opt/sybase/OCS-12_5/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase/OCS-12_5/include\"],[/opt/sybase/OCS-12_5/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"/opt/sybase/OCS-12_5/lib\"; SYBASELIBS=\"-L/opt/sybase/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"],[/opt/sybase/OCS-12_5/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"; SYBASESTATIC=\"$STATICFLAG\"])
			fi
		
			if ( test -z "$SYBASELIBS" )
			then
				for i in "11.9.2" "11.0.3.3"
				do
					FW_CHECK_HEADER_LIB([/opt/sybase-$i/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase-$i/include\"],[/opt/sybase-$i/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"/opt/sybase-$i/lib\"; SYBASELIBS=\"-L/opt/sybase-$i/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl -linsck\"],[/opt/sybase-$i/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase-$i/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"])
					if ( test -n "$SYBASELIBS" )
					then
						break
					fi
				done
			fi
		fi
		
		LINKFAIL=""
		if ( test -n "$DLLIB" -a -n "$SYBASESTATIC" -a -n "$SYBASELIBS" )
		then
			AC_MSG_CHECKING(if Sybase can be statically linked without $DLLIB)
			FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASESTATIC $SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Sybase can be statically linked with $DLLIB)
				FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASESTATIC $SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); SYBASELIBS="$SYBASELIBS $DLLIB"; LINKFAIL="";],[AC_MSG_RESULT(no); SYBASESTATIC=""])
			fi
		fi
		
		if ( test -n "$DLLIB" -a -z "$SYBASESTATIC" -a -n "$SYBASELIBS" )
		then
			AC_MSG_CHECKING(if Sybase can be dynamically linked without $DLLIB)
			FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$SYBASELIBSPATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Sybase can be dynamically linked with $DLLIB)
				FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIB $DLLIB],[$LD_LIBRARY_PATH:$SYBASELIBSPATH],[AC_MSG_RESULT(yes); SYBASELIBS="$SYBASELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no)])
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

	FW_INCLUDES(sybase,[$SYBASEINCLUDES])
	FW_LIBS(sybase,[$SYBASELIBS])
		
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

		if ( test -n "$CYGWIN" -a -z "$ODBCLIBS" )
		then
			FW_CHECK_HEADER_LIB([/usr/include/w32api/sql.h],[],[/usr/lib/w32api/libodbc32.dll.a],[ODBCLIBSPATH=\"/usr/lib/w32api\"; ODBCLIBS=\"-L/usr/lib/w32api -lodbc32\"],[/usr/lib/w32api/libodbc32.a],[ODBCLIBSPATH=\"/usr/lib/w32api\"; ODBCLIBS=\"-L/usr/lib/w32api -lodbc32\"; STATIC=\"$STATICFLAG\"])
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
#include <stdlib.h>],[SQLHENV env; SQLHDBC dbc; SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env); SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc); SQLFreeHandle(SQL_HANDLE_DBC,dbc); SQLFreeHandle(SQL_HANDLE_ENV,env);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); ODBCLIBS="$ODBCLIBS $PTHREADLIBS"])
		fi
		if ( test -n "$HAVE_IODBC" )
		then
			AC_DEFINE(HAVE_IODBC,1,iODBC)
			AC_MSG_CHECKING(if iODBC needs threads)
			FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLHENV env; SQLHDBC dbc; SQLAllocEnv(&env); SQLAllocConnect(env,&dbc); SQLFreeConnect(&dbc); SQLFreeEnv(&env);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); ODBCLIBS=\"$ODBCLIBS $PTHREADLIBS\"])
		fi
		if ( test -z "$ODBCLIBS" )
		then
			AC_MSG_WARN(ODBC support will not be built.)
		fi

	fi

	if ( test -n "$ODBCLIBS" )
	then
		AC_MSG_CHECKING(if SQLBindParameter takes SQLLEN * argument)
		FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLBindParameter(0,0,0,0,0,0,0,0,0,(SQLLEN *)NULL);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(SQLBINDPARAMETER_SQLLEN,1,Some systems use SQLLEN * in SQLBINDPARAMETER)],[AC_MSG_RESULT(no)])

		AC_MSG_CHECKING(if SQLColAttribute takes SQLLEN * argument)
		FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLColAttribute(0,0,0,0,0,0,(SQLLEN *)NULL);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(SQLCOLATTRIBUTE_SQLLEN,1,Some systems use SQLLEN * in SQLColAttribute)],[AC_MSG_RESULT(no)])
		
		AC_MSG_CHECKING(if SQLRowCount takes SQLLEN * argument)
		FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLRowCount(0,(SQLLEN *)0);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIB],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(SQLROWCOUNT_SQLLEN,1,Some systems use SQLLEN * in SQLRowCount)],[AC_MSG_RESULT(no)])
	fi

	FW_INCLUDES(odbc,[$ODBCINCLUDES])
	FW_LIBS(odbc,[$ODBCLIBS])
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
			if ( test -n "$CYGWIN" )
			then
				FW_CHECK_HEADER_LIB([$DB2PATH/include/sql.h],[DB2INCLUDES=\"-I$DB2PATH/include\"],[$DB2PATH/lib/db2cli.lib],[DB2LIBSPATH=\"$DB2PATH/lib\"; DB2LIBS=\"$DB2PATH/lib/db2cli.lib\"],[],[])
			else
				FW_CHECK_HEADER_LIB([$DB2PATH/include/sql.h],[DB2INCLUDES=\"-I$DB2PATH/include\"],[$DB2PATH/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"$DB2PATH/lib\"; DB2LIBS=\"-L$DB2PATH/lib -ldb2\"],[$DB2PATH/lib/libdb2.a],[DB2LIBS=\"-L$DB2PATH/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"])
			fi
		
		else

			if ( test -n "$CYGWIN" )
			then

				FW_CHECK_HEADER_LIB([/cygdrive/c/Program Files/IBM/SQLLIB/include/sql.h],[DB2INCLUDES=\"-I/cygdrive/c/Program\ Files/IBM/SQLLIB/include\"; DB2VERSION=\"8\"],[/cygdrive/c/Program Files/IBM/SQLLIB/lib/db2cli.lib],[DB2LIBSPATH=\"/cygdrive/c/Program\ Files/IBM/SQLLIB/lib\"; DB2LIBS=\"/cygdrive/c/Program\ Files/IBM/SQLLIB/lib/db2cli.lib\"; DB2VERSION=\"8\"],[],[])

			else
		
				dnl check /opt for 7.2
				FW_CHECK_HEADER_LIB([/opt/IBMdb2/V7.1/include/sql.h],[DB2INCLUDES=\"-I/opt/IBMdb2/V7.1/include\"; DB2VERSION=\"7\"],[/opt/IBMdb2/V7.1/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/IBMdb2/V7.1/lib\"; DB2LIBS=\"-L/opt/IBMdb2/V7.1/lib -ldb2\"; DB2VERSION=\"7\"],[/opt/IBMdb2/V7.1/lib/libdb2.a],[DB2LIBS=\"-L/opt/IBMdb2/V7.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"7\"])
		
				dnl check /usr for 7.2
				FW_CHECK_HEADER_LIB([/usr/IBMdb2/V7.1/include/sql.h],[DB2INCLUDES=\"-I/usr/IBMdb2/V7.1/include\"; DB2VERSION=\"7\"],[/usr/IBMdb2/V7.1/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/usr/IBMdb2/V7.1/lib\"; DB2LIBS=\"-L/usr/IBMdb2/V7.1/lib -ldb2\"; DB2VERSION=\"7\"],[/usr/IBMdb2/V7.1/lib/libdb2.a],[DB2LIBS=\"-L/usr/IBMdb2/V7.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"7\"])
		
				dnl check /opt for 8.1
				FW_CHECK_HEADER_LIB([/opt/IBM/db2/V8.1/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V8.1/include\"; DB2VERSION=\"8\"],[/opt/IBM/db2/V8.1/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/IBM/db2/V8.1/lib\"; DB2LIBS=\"-L/opt/IBM/db2/V8.1/lib -ldb2\"; DB2VERSION=\"8\"],[/opt/IBM/db2/V8.1/lib/libdb2.a],[DB2LIBS=\"-L/opt/IBM/db2/V8.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"8\"])
			fi
		fi
		
		if ( test -z "$DB2LIBS" )
		then
			AC_MSG_WARN(DB2 support will not be built.)
		fi
	fi

	FW_VERSION(db2,[$DB2VERSION])
	FW_INCLUDES(db2,[$DB2INCLUDES])
	FW_LIBS(db2,[$DB2LIBS])
		
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
			if ( test -n "$CYGWIN" )
			then
				FW_CHECK_HEADER_LIB([$INTERBASEPATH/include/ibase.h],[INTERBASEINCLUDES=\"-I$INTERBASEPATH/include\"],[$INTERBASEPATH/bin/fbclient.dll],[INTERBASELIBSPATH=\"$INTERBASPATH/bin\"; INTERBASELIBS=\"-L$INTERBASEPATH/bin -lfbclient\"],[],[])
			else
				FW_CHECK_HEADER_LIB([$INTERBASEPATH/include/ibase.h],[INTERBASEINCLUDES=\"-I$INTERBASEPATH/include\"],[$INTERBASEPATH/lib/libgds.$SOSUFFIX],[INTERBASELIBSPATH=\"$INTERBASPATH/lib\"; INTERBASELIBS=\"-L$INTERBASEPATH/lib -lgds -lcrypt\"],[$INTERBASEPATH/lib/libgds.a],[INTERBASELIBS=\"-L$INTERBASEPATH/lib -lgds -lcrypt\"; INTERBASESTATIC=\"$STATICFLAG\"])
			fi
		else
			if ( test -z "$MICROSOFT" )
			then
				FW_CHECK_HEADER_LIB([/usr/include/ibase.h],[INTERBASEINCLUDES=\"\"],[/usr/lib/libgds.$SOSUFFIX],[INTERBASELIBS=\"-lgds -lcrypt\"],[/usr/lib/libgds.a],[INTERBASELIBS=\"-lgds -lcrypt\"; INTERBASESTATIC=\"$STATICFLAG\"])
			fi
		fi

		if ( test -z "$INTERBASELIBS" )
		then
			if ( test -n "$CYGWIN" )
			then
				for dir in "`ls -d /cygdrive/c/Firebird*`"
				do
					FW_CHECK_HEADER_LIB([$dir/include/ibase.h],[INTERBASEINCLUDES=\"-I$dir/include\"],[$dir/bin/fbclient.dll],[INTERBASELIBS=\"-L$dir/bin -lfbclient\"],[],[])
				done
			else
				FW_CHECK_HEADER_LIB([/usr/local/firebird/include/ibase.h],[INTERBASEINCLUDES=\"-I/usr/local/firebird/include\"],[/usr/local/firebird/lib/libgds.$SOSUFFIX],[INTERBASELIBSPATH=\"/usr/local/firebird/lib\"; INTERBASELIBS=\"-L/usr/local/firebird/lib -lgds -lcrypt\"],[/usr/local/firebird/lib/libgds.a],[INTERBASELIBS=\"-L/usr/local/firebird/lib -lgds -lcrypt\"; INTERBASESTATIC=\"$STATICFLAG\"])
			fi
		fi
		
		LINKFAIL=""
		if ( test -n "$DLLIB" -a -n "$INTERBASESTATIC" -a -n "$INTERBASELIBS" )
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
		
		if ( test -n "$DLLIB" -a -z "$INTERBASESTATIC" -a -n "$INTERBASELIBS" )
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

	FW_INCLUDES(interbase,[$INTERBASEINCLUDES])
	FW_LIBS(interbase,[$INTERBASELIBS])
	
	AC_SUBST(INTERBASEINCLUDES)
	AC_SUBST(INTERBASELIBS)
	AC_SUBST(INTERBASELIBSPATH)
	AC_SUBST(INTERBASESTATIC)
fi
])



AC_DEFUN([FW_CHECK_MDBTOOLS],
[
if ( test "$ENABLE_MDBTOOLS" = "yes" )
then
	FW_CHECK_GLIB

	MDBTOOLSINCLUDES=""
	MDBTOOLSLIBS=""
	MDBTOOLSLIBSPATH=""
	MDBTOOLSSTATIC=""

	if ( test -n "$GLIBINCLUDES" -a -n "$GLIBLIBS" )
	then

		if ( test "$cross_compiling" = "yes" )
		then

			dnl cross compiling ...
			echo "cross compiling"
			if ( test -n "$MDBTOOLSPATH" )
			then
				MDBTOOLSINCLUDES="-I$MDBTOOLSPATH/include $GLIBINCLUDES"
				MDBTOOLSLIBS="-L$MDBTOOLSPATH/lib -lmdbsql -lmdb $GLIBLIBS"
				MDBTOOLSLIBSPATH="$MDBTOOLSPATH/lib"
			fi

		else

			STATICFLAG=""
			if ( test -n "$STATICLINK" )
			then
				STATICFLAG="-static"
			fi

			FW_CHECK_HEADERS_AND_LIBS([$MDBTOOLSPATH],[mdbsql],[mdbsql.h],[mdbsql],[$STATICFLAG],[$RPATHFLAG],[MDBSQLINCLUDES],[MDBSQLLIBS],[MDBSQLLIBSPATH],[MDBSQLSTATIC])
			FW_CHECK_HEADERS_AND_LIBS([$MDBTOOLSPATH],[mdb],[mdbtools.h],[mdb],[$STATICFLAG],[$RPATHFLAG],[MDBINCLUDES],[MDBLIBS],[MDBTOOLSLIBSPATH],[MDBTOOLSSTATIC])

			if ( test -n "$MDBSQLINCLUDES" -o -n "$MDBSQLLIBS" -o -n "$MDBINCLUDES" -o -n "$MDBLIBS" )
			then
				LINKFAIL=""
				MDBTOOLSINCLUDES="$MDBINCLUDES $MDBSQLINCLUDES $GLIBINCLUDES"
				MDBTOOLSLIBS="$MDBSQLLIBS $MDBLIBS $GLIBLIBS"
				AC_MSG_CHECKING(if MDB Tools has mdb_run_query)
				FW_TRY_LINK([#include <mdbsql.h>
#include <stdlib.h>],[mdb_run_query(NULL,NULL);],[$MDBTOOLSINCLUDES],[$MDBTOOLSLIBS $SOCKETLIB $DLLIB -lm],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MDB_RUN_QUERY,1,Some versions of mdbtools define mdb_run_query)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			fi
		
		fi

		if ( test -z "$MDBTOOLSLIBS" -o -n "$LINKFAIL" )
		then
			AC_MSG_WARN(MDB Tools support will not be built.)
		fi
	fi

	FW_INCLUDES(mdbtools,[$MDBTOOLSINCLUDES])
	FW_LIBS(mdbtools,[$MDBTOOLSLIBS])

	AC_SUBST(MDBTOOLSINCLUDES)
	AC_SUBST(MDBTOOLSLIBS)
	AC_SUBST(MDBTOOLSLIBSPATH)
	AC_SUBST(MDBTOOLSSTATIC)
fi
])



AC_DEFUN([FW_CHECK_PERL],
[
if ( test "$ENABLE_PERL" = "yes" )
then

	HAVE_PERL=""
	PERL=""
	PERLPREFIX=""
	PERLCYGDRIVEPREFIX=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else

		if ( test -n "$PERLPATH" )
		then
			FW_CHECK_FILE("$PERLPATH/bin/perl",[PERL=\"$PERLPATH/bin/perl\"; HAVE_PERL=\"yes\"])
		else
			AC_CHECK_PROG(PERL,perl,"perl")
			if ( test -z "$PERL" )
			then
				for i in "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/usr/local/perl/bin" "/opt/sfw/bin"
				do
					FW_CHECK_FILE("$i/perl5",[PERL=\"$i/perl5\"])
					FW_CHECK_FILE("$i/perl",[PERL=\"$i/perl\"])
					if ( test -n "$PERL" )
					then
						break
					fi
				done
			fi
			if ( test -z "$PERL" -a -n "$CYGWIN" )
			then
				FW_CHECK_FILE("/cygdrive/c/Perl/bin/perl",[PERL=\"/cygdrive/c/Perl/bin/perl\"; PERLCYGDRIVEPREFIX=\"/cygdrive/c\"])
			fi
		fi
		if ( test -n "$PERL" )
		then
			HAVE_PERL="yes"
			PERLPREFIXCMD=`$PERL -V:prefix`
			PERLPREFIX=`eval "$PERLPREFIXCMD"; echo $prefix`
			if ( test -n "`pod2man --help 2>&1 | grep Usage`" )
			then
				POD2MAN="pod2man"
			fi
		else
			AC_MSG_WARN(The Perl API will not be built.)
		fi
	fi

	AC_SUBST(HAVE_PERL)
	AC_SUBST(PERL)
	AC_SUBST(PERLPREFIX)
	AC_SUBST(PERLCYGDRIVEPREFIX)
	AC_SUBST(POD2MAN)
fi
])



AC_DEFUN([FW_CHECK_PYTHON],
[
if ( test "$ENABLE_PYTHON" = "yes" )
then

	HAVE_PYTHON=""
	PYTHONINCLUDES=""
	PYTHONDIR=""
	PYTHONLIB=""
	PYTHONVERSION=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else

		if ( test -n "$PYTHONPATH" )
		then
		
			for i in "2.3" "2.2" "2.1" "2.0" "1.6" "1.5"
			do
				if ( test -d "$PYTHONPATH/include/python$i" -a -d "$PYTHONPATH/lib/python$i/config" )
				then
					PYTHONINCLUDES="-I$PYTHONPATH/include/python$i"
					PYTHONDIR="$PYTHONPATH/lib/python$i"
					if ( test -n "$CYGWIN" )
					then
						if ( test -r "$PYTHONPATH/lib/python$i/config/libpython$i.dll.a" )
						then
							PYTHONDIR="$PYTHONPATH/lib/python$i"
							PYTHONLIB="-L$PYTHONDIR/config -lpython$i"
						fi
					else
						PYTHONDIR="$PYTHONPATH/lib/python$i"
					fi
				fi
		
				if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" )
				then
					PYTHONVERSION=`echo $i | sed -e "s|\.||"`
					break
				fi
			done

			if ( test -z "$PYTHONDIR" -a -n "$CYGWIN" )
			then

				for i in "23" "22" "21" "20" "16" "15"
				do

					FW_CHECK_HEADER_LIB([$PYTHONPATH/include/Python.h],[PYTHONINCLUDES=\"-I$PYTHONPATH/include\"],[$PYTHONPATH/libs/libpython$j.lib],[PYTHONDIR=\"$PYTHONPATH/Lib\"; PYTHONLIB=\"-L$PYTHONPATH/libs -lpython$j\"],[],[])
					if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" )
					then
						PYTHONVERSION=$i
						break
					fi

				done
			fi
		
		else
		
			for j in "2.3" "2.2" "2.1" "2.0" "1.6" "1.5"
			do
				for i in "/usr/include/python$j" "/usr/local/include/python$j" "/usr/pkg/include/python$j" "/usr/local/python$j/include/python$j" "/opt/sfw/include/python$j"
				do
					if ( test -d "$i" )
					then
						PYTHONINCLUDES="-I$i"
					fi
					if ( test -n "$PYTHONINCLUDES" )
					then
						PYTHONVERSION=`echo $j | sed -e "s|\.||"`
						break
					fi
				done
			
				for i in "/usr/lib/python$j" "/usr/local/lib/python$j" "/usr/pkg/lib/python$j" "/usr/local/python$j/lib/python$j" "/opt/sfw/lib/python$j"
				do
					if ( test -d "$i/config" )
					then
						if ( test -n "$CYGWIN" -a -r "$i/config/libpython$j.dll.a" )
						then
							PYTHONDIR="$i"
							PYTHONLIB="-L$PYTHONDIR/config -lpython$j"
						else
							PYTHONDIR="$i"
						fi
					fi
					if ( test -n "$PYTHONDIR" )
					then
						break
					fi
				done
				if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" )
				then
					break
				fi
			done

			if ( test -z "$PYTHONDIR" -a -n "$CYGWIN" )
			then

				for j in "23" "22" "21" "20" "16" "15"
				do
					FW_CHECK_HEADER_LIB([/cygdrive/c/Python$j/include/Python.h],[PYTHONINCLUDES=\"-I/cygdrive/c/Python$j/include\"],[/cygdrive/c/Python$j/libs/python$j.lib],[PYTHONDIR=\"/cygdrive/c/Python$j/Lib\"; PYTHONLIB=\"-L/cygdrive/c/Python$j/libs -lpython$j\"],[],[])
					if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" )
					then
						PYTHONVERSION=$j
						break
					fi
				done
			fi
		
		fi
		
		if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" )
		then
			HAVE_PYTHON="yes"
		else
			AC_MSG_WARN(The Python API will not be built.)
		fi
	fi

	FW_INCLUDES(python,[$PYTHONINCLUDES])

	PYTHON_HAVE_WEAKREF=""
	if ( test "$PYTHONVERSION" -ge "21" )
	then
		PYTHON_HAVE_WEAKREF="yes"
	fi

	AC_SUBST(HAVE_PYTHON)
	AC_SUBST(PYTHONINCLUDES)
	AC_SUBST(PYTHONDIR)
	AC_SUBST(PYTHONLIB)
	AC_SUBST(PYTHON_HAVE_WEAKREF)
fi
])



AC_DEFUN([FW_CHECK_ZOPE],
[
if ( test "$ENABLE_ZOPE" = "yes" )
then

	AC_MSG_CHECKING(for Zope)

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
				HAVE_ZOPE="yes"
			else
		
				if ( test -z "$ZOPEDIR" )
				then
					for i in "/usr/local/www" "/usr/share" "/usr/local" "/usr" "/usr/lib" "/opt"
					do
						for j in "zope" "Zope"
						do
							FW_CHECK_FILE("$i/$j/lib/python/Products/__init__.py",[HAVE_ZOPE=\"yes\"; ZOPEDIR=\"$i/$j/lib/python/Products\"])
							if ( test -n "$ZOPEDIR" )
							then
								break
							fi
							for k in "2.2" "2.3" "2.4" "2.5" "2.6" "2.7"
							do
								FW_CHECK_FILE("$i/$j-$k/lib/python/Products/__init__.py",[HAVE_ZOPE=\"yes\"; ZOPEDIR=\"$i/$j-$k/lib/python/Products\"])
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

	AC_MSG_RESULT($ZOPEDIR)

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
	RUBYCYGDRIVEPREFIX=""
	RUBYLIB=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else
	
		if ( test -n "$RUBYPATH" )
		then
			FW_CHECK_FILE("$RUBYPATH/bin/ruby",[RUBY=\"$RUBYPATH/bin/ruby\"])
		else
			AC_CHECK_PROG(RUBY,"ruby","ruby")
			if ( test -z "$RUBY" )
			then
				for i in "/usr/local/ruby/bin" "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/opt/sfw/bin"
				do
					FW_CHECK_FILE("$i/ruby",[RUBY=\"$i/ruby\"])
					if ( test -n "$RUBY" )
					then
						break
					fi
				done
			fi
			if ( test -z "$RUBY" -a -n "$CYGWIN" )
			then
				FW_CHECK_FILE("/cygdrive/c/ruby/bin/ruby",[RUBY=\"/cygdrive/c/ruby/bin/ruby\"; RUBYCYGDRIVEPREFIX=\"/cygdrive/c\"])
			fi
		fi
		
		if ( test -n "$RUBY" )
		then
			HAVE_RUBY="yes"
			if ( test -n "$CYGWIN" )
			then
				RUBYLIB="-lruby"
			fi
		else
			HAVE_RUBY=""
			AC_MSG_WARN(The Ruby API will not be built.)
		fi
	fi

	AC_SUBST(HAVE_RUBY)
	AC_SUBST(RUBY)
	AC_SUBST(RUBYCYGDRIVEPREFIX)
	AC_SUBST(RUBYLIB)
fi
])



AC_DEFUN([FW_CHECK_JAVA],
[
if ( test "$ENABLE_JAVA" = "yes" )
then

	HAVE_JAVA=""
	JAVAC=""
	JAVAINCLUDES=""
	JAVALIB=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else
		
		if ( test -z "$JAVAPATH" )
		then
			for i in `ls -d /usr/java/jdk* /usr/java/j2sdk* /usr/local/jdk* /usr/java /usr/local/java /cygdrive/c/jdk* /cygdrive/c/j2sdk* 2>/dev/null`
			do
				if ( test -z "$JAVAPATH" )
				then
					FW_CHECK_FILE("$i/include/jni.h",[JAVAPATH=\"$i\"])
				else
					break
				fi
			done
		fi
		
		if ( test -n "$JAVAPATH" )
		then
			FW_CHECK_FILE("$JAVAPATH/bin/javac$EXE",[JAVAC=\"$JAVAPATH/bin/javac$EXE\"])
			FW_CHECK_FILE("$JAVAPATH/include/jni.h",[JAVAINCLUDES=\"-I$JAVAPATH/include\"])
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

	if ( test -n "$CYGWIN" )
	then
		JAVAINCLUDES="$JAVAINCLUDES -I$JAVAPATH/include/win32"
		JAVALIB="-L$JAVAPATH/lib -ljni"
	fi

	FW_INCLUDES(java,[$JAVAINCLUDES])
	FW_LIBS(java,[$JAVALIB])
		
	AC_SUBST(HAVE_JAVA)
	AC_SUBST(JAVAC)
	AC_SUBST(JAVAINCLUDES)
	AC_SUBST(JAVALIB)
fi
])


AC_DEFUN([FW_CHECK_PHP],
[
if ( test "$ENABLE_PHP" = "yes" )
then

	HAVE_PHP=""
	PHPINCLUDES=""
	PHPEXTDIR=""
	PHPLIB=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else
	
		if ( test -n "$CYGWIN" )
		then
			dnl Windows stuff here...
			echo "windows..."
		else
			PHPCONFIG=""
			if ( test -n "$PHPPATH" )
			then
				FW_CHECK_FILE("$PHPPATH/bin/php-config",[PHPCONFIG=\"$PHPPATH/bin/php-config\"])
			else
				AC_CHECK_PROG(PHPCONFIG,"php-config","php-config")
				if ( test -z "$PHPCONFIG" )
				then
					for i in "/usr/local/php/bin" "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/opt/sfw/bin"
					do
						FW_CHECK_FILE("$i/php-config",[PHPCONFIG=\"$i/php-config\"])
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
				dnl some php's fail to replace ${prefix} with
				dnl their prefix when you run php-config
				dnl --includes, but php-config --prefix usually
				dnl works so we fake it here
				PHPINCLUDES=`$PHPCONFIG --includes | sed -e "s|\\${prefix}|$PHPPREFIX|" | sed -e "s|\\${prefix}|$PHPPREFIX|" | sed -e "s|\\${prefix}|$PHPPREFIX|" | sed -e "s|\\${prefix}|$PHPPREFIX|"`
				PHPEXTDIR=`$PHPCONFIG --extension-dir`
			else
				HAVE_PHP=""
				AC_MSG_WARN(The PHP API will not be built.)
			fi
		fi
	fi

	FW_INCLUDES(php,[$PHPINCLUDES])

	AC_SUBST(HAVE_PHP)
	AC_SUBST(PHPINCLUDES)
	AC_SUBST(PHPEXTDIR)
	AC_SUBST(PHPLIB)
fi
])

AC_DEFUN([FW_CHECK_PHP_PEAR_DB],
[

	if ( test -z "$PHPPEARDBDIR" )
	then

		for i in "/usr" "/usr/local" "/usr/pkg" "/opt/sfw"
		do
			for j in "pear/DB" "pear/bootstrap/DB"
			do
				if ( test -d "$i/$j" -a -r "$i/$j/common.php" )
				then
					PHPPEARDBDIR="$i/$j"
					break
				fi
			done
			if ( test -n "$PHPPEARDBDIR" )
			then
				break
			fi
		done

	fi

	if ( test -z "$PHPPEARDBDIR" -a "$HAVE_PHP" )
	then
		PHPPEARDBDIR="$PHPPREFIX/share/pear/DB"
	fi

	if ( test -z "$PHPPEARDBDIR" )
	then
		PHPPEARDBDIR="\${datadir}/pear/DB"
	fi

	AC_SUBST(PHPPEARDBDIR)
])


AC_DEFUN([FW_CHECK_TCL],
[
if ( test "$ENABLE_TCL" = "yes" )
then

	HAVE_TCL=""
	TCLINCLUDE=""
	TCLLIB=""
	HAVETCLGETSTRING=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else

		dnl Checks for TCL.
		if ( test -n "$TCLINCLUDEPATH" )
		then
			FW_CHECK_FILE($TCLINCLUDEPATH/tcl.h,[TCLINCLUDE=\"-I$TCLINCLUDEPATH\"])
		else
			for i in "/usr/include" "$prefix/include" "/usr/local/include" "/usr/pkg/include" "/opt/sfw/include"
			do
				FW_CHECK_FILE($i/tcl.h,[TCLINCLUDE=\"-I$i\"])
				for j in "tcl8.0" "tcl8.1" "tcl8.2" "tcl8.3" "tcl8.4" "tcl8.5"
				do
					FW_CHECK_FILE($i/$j/tcl.h,[TCLINCLUDE=\"-I$i/$j\"])
				done
			done
		fi
		if ( test -z "$TCLINCLUDE" )
		then
			AC_MSG_WARN("The TCL API will not be installed.")
		else
			if ( test -n "$TCLLIBSPATH" )
			then
				if ( test -z "$MICROSOFT" )
				then
					dnl FW_CHECK_FILE($TCLLIBSPATH/libtclstub.a,[TCLLIB=\"-L$TCLLIBSPATH -ltclstub\"; TCLINCLUDE=\"-DUSE_TCL_STUBS $TCLINCLUDE\"])
					FW_CHECK_FILE($TCLLIBSPATH/libtclstub.a,[TCLLIB=\"$TCLLIBSPATH/libtclstub.a\"; TCLINCLUDE=\"-DUSE_TCL_STUBS $TCLINCLUDE\"])
					if ( test -z "$TCLLIB" )
					then
						for i in "8.0" "8.1" "8.2" "8.3" "8.4" "8.5" "80" "81" "82" "83" "84" "85"
						do
							dnl FW_CHECK_FILE($TCLLIBSPATH/libtclstub$i.a,[TCLLIB=\"-L$TCLLIBSPATH -ltclstub$i\"; TCLINCLUDE=\"-DUSE_TCL_STUBS $TCLINCLUDE\"])
							FW_CHECK_FILE($TCLLIBSPATH/libtclstub$i.a,[TCLLIB=\"$TCLLIBSPATH/libtclstub$i.a\"; TCLINCLUDE=\"-DUSE_TCL_STUBS $TCLINCLUDE\"])
						done
					fi
				fi
				if ( test -z "$TCLLIB" )
				then
					FW_CHECK_FILE($TCLLIBSPATH/libtcl.$SOSUFFIX,[TCLLIB=\"-L$TCLLIBSPATH -ltcl\"])
					if ( test -z "$TCLLIB" )
					then
						for i in "8.0" "8.1" "8.2" "8.3" "8.4" "8.5" "80" "81" "82" "83" "84" "85"
						do
							FW_CHECK_FILE($TCLLIBSPATH/libtcl$i.$SOSUFFIX,[TCLLIB=\"-L$TCLLIBSPATH -ltcl$i\"])
						done
					fi
				fi
			else
				if ( test -z "$MICROSOFT" )
				then
					for i in "/usr/lib" "$prefix/lib" "/usr/local/lib" "/usr/pkg/lib" "/opt/sfw/lib"
					do
						for j in "" "8.0" "8.1" "8.2" "8.3" "8.4" "8.5" "80" "81" "82" "83" "84" "85"
						do
							dnl FW_CHECK_FILE($i/libtclstub$j.a,[TCLLIB=\"-L$i -ltclstub$j\"; TCLLIBSPATH=\"$i\"; TCLINCLUDE=\"-DUSE_TCL_STUBS $TCLINCLUDE\"])
							FW_CHECK_FILE($i/libtclstub$j.a,[TCLLIB=\"$i/libtclstub$j.a\"; TCLLIBSPATH=\"$i\"; TCLINCLUDE=\"-DUSE_TCL_STUBS $TCLINCLUDE\"])
						done
					done
				fi
				if ( test -z "$TCLLIB" )
				then
					for i in "/usr/lib" "$prefix/lib" "/usr/local/lib" "/usr/pkg/lib" "/opt/sfw/lib"
					do
						for j in "" "8.0" "8.1" "8.2" "8.3" "8.4" "8.5" "80" "81" "82" "83" "84" "85"
						do
							FW_CHECK_FILE($i/libtcl$j.$SOSUFFIX,[TCLLIB=\"-L$i -ltcl$j\"; TCLLIBSPATH=\"$i\"])
							dnl for some reason, the
							dnl tcl dll stub library
							dnl is called libtcl.a
							dnl instead of
							dnl libtcl.dll.a on
							dnl cygwin
							if ( test -z "$TCLLIB" -a -n "$CYGWIN" )
							then
								FW_CHECK_FILE($i/libtcl$j.a,[TCLLIB=\"-L$i -ltcl$j\"; TCLLIBSPATH=\"$i\"])
							fi
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
			AC_MSG_CHECKING(for Tcl_GetString)
			FW_TRY_LINK([#include <tcl.h>],[Tcl_GetString(NULL);],[$TCLINCLUDE],[$TCLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); HAVETCLGETSTRING="yes"],[AC_MSG_RESULT(no)])
		fi
	fi

	if ( test -n "$HAVETCLGETSTRING" )
	then
		AC_DEFINE_UNQUOTED(HAVE_TCL_GETSTRING,1,Some versions of TCL don't have Tcl_GetString)
	fi

	FW_INCLUDES(tcl,[$TCLINCLUDE])
	FW_LIBS(tcl,[$TCLLIB])

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
			FW_CHECK_FILE("$GTKPATH/bin/gtk-config",[GTKCONFIG=\"$GTKPATH/bin/gtk-config\"])
		else
			AC_CHECK_PROG(GTKCONFIG,"gtk-config","gtk-config")
			if ( test -z "$GTKCONFIG" )
			then
				for i in "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/usr/local/gtk/bin" "/usr/local/gtk+/bin" "/opt/sfw/bin"
				do
					for j in "gtk-config" "gtk12-config" "gtk10-config"
					do
						FW_CHECK_FILE("$i/$j",[GTKCONFIG=\"$i/$j\"])
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

AC_DEFUN([FW_CHECK_NEED_REDHAT_9_GLIBC_2_3_2_HACK],
[
	dnl if there's no features.h then we're not using glibc (hopefully)
	AC_CHECK_HEADER([features.h],[USING_GLIBC=yes],[USING_GLIBC=no])

	if ( test "$USING_GLIBC" = "yes" )
	then
		AC_MSG_CHECKING(for broken glibc-2.3)
		GLIBC23HACKCODE=""
		GLIBC23HACKINCLUDE=""
		AC_TRY_LINK([#include <ctype.h>
#include <features.h>],[#if __GLIBC__==2 && __GLIBC_MINOR__==3
	__ctype_toupper('a');
#endif],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes) AC_DEFINE_UNQUOTED(NEED_REDHAT_9_GLIBC_2_3_2_HACK,1,Some versions of glibc-2.3 need a fixup) GLIBC23HACKINCLUDE="#include <ctype.h>"; GLIBC23HACKCODE=" const unsigned short int **__ctype_b() { return __ctype_b_loc(); } const __int32_t **__ctype_toupper() { return __ctype_toupper_loc(); } const __int32_t **__ctype_tolower() { return __ctype_tolower_loc(); }"])
	fi
])

AC_DEFUN([FW_CHECK_SIGNALS],
[
AC_MSG_CHECKING(for unsupported signals)
UNSUPPORTEDSIGNALS=""
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGHUP;, AC_DEFINE(HAVE_SIGHUP,1,Do we have SIGHUP), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGHUP")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGINT;, AC_DEFINE(HAVE_SIGINT,1,Do we have SIGINT), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGINT")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGQUIT;, AC_DEFINE(HAVE_SIGQUIT,1,Do we have SIGQUIT), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGQUIT")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGILL;, AC_DEFINE(HAVE_SIGILL,1,Do we have SIGILL), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGILL")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGTRAP;, AC_DEFINE(HAVE_SIGTRAP,1,Do we have SIGTRAP), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGTRAP")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGABRT;, AC_DEFINE(HAVE_SIGABRT,1,Do we have SIGABRT), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGABRT")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGIOT;, AC_DEFINE(HAVE_SIGIOT,1,Do we have SIGIOT), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGIOT")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGBUS;, AC_DEFINE(HAVE_SIGBUS,1,Do we have SIGBUS), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGBUS")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGFPE;, AC_DEFINE(HAVE_SIGFPE,1,Do we have SIGFPE), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGFPE")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGKILL;, AC_DEFINE(HAVE_SIGKILL,1,Do we have SIGKILL), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGKILL")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGUSR1;, AC_DEFINE(HAVE_SIGUSR1,1,Do we have SIGUSR1), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGUSR1")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGSEGV;, AC_DEFINE(HAVE_SIGSEGV,1,Do we have SIGSEGV), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGSEGV")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGUSR2;, AC_DEFINE(HAVE_SIGUSR2,1,Do we have SIGUSR2), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGUSR2")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGPIPE;, AC_DEFINE(HAVE_SIGPIPE,1,Do we have SIGPIPE), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGPIPE")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGALRM;, AC_DEFINE(HAVE_SIGALRM,1,Do we have SIGALRM), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGALRM")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGTERM;, AC_DEFINE(HAVE_SIGTERM,1,Do we have SIGTERM), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGTERM")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGSTKFLT;, AC_DEFINE(HAVE_SIGSTKFLT,1,Do we have SIGSTKFLT), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGSTKFLT")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGCHLD;, AC_DEFINE(HAVE_SIGCHLD,1,Do we have SIGCHLD), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGCHLD")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGCONT;, AC_DEFINE(HAVE_SIGCONT,1,Do we have SIGCONT), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGCONT")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGSTOP;, AC_DEFINE(HAVE_SIGSTOP,1,Do we have SIGSTOP), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGSTOP")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGTSTP;, AC_DEFINE(HAVE_SIGTSTP,1,Do we have SIGTSTP), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGTSTP")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGTTIN;, AC_DEFINE(HAVE_SIGTTIN,1,Do we have SIGTTIN), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGTTIN")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGTTOU;, AC_DEFINE(HAVE_SIGTTOU,1,Do we have SIGTTOU), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGTTOU")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGURG;, AC_DEFINE(HAVE_SIGURG,1,Do we have SIGURG), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGURG")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGXCPU;, AC_DEFINE(HAVE_SIGXCPU,1,Do we have SIGXCPU), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGXCPU")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGXFSZ;, AC_DEFINE(HAVE_SIGXFSZ,1,Do we have SIGXFSZ), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGXFSZ")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGVTALRM;, AC_DEFINE(HAVE_SIGVTALRM,1,Do we have SIGVTALRM), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGVTALRM")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGPROF;, AC_DEFINE(HAVE_SIGPROF,1,Do we have SIGPROF), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGPROF")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGWINCH;, AC_DEFINE(HAVE_SIGWINCH,1,Do we have SIGWINCH), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGWINCH")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGIO;, AC_DEFINE(HAVE_SIGIO,1,Do we have SIGIO), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGIO")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGPOLL;, AC_DEFINE(HAVE_SIGPOLL,1,Do we have SIGPOLL), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGPOLL")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGPWR;, AC_DEFINE(HAVE_SIGPWR,1,Do we have SIGPWR), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGPWR")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGUNUSED;, AC_DEFINE(HAVE_SIGUNUSED,1,Do we have SIGUNUSED), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGUNUSED")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGEMT;, AC_DEFINE(HAVE_SIGEMT,1,Do we have SIGEMT), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGEMT")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGSYS;, AC_DEFINE(HAVE_SIGSYS,1,Do we have SIGSYS), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGSYS")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGCLD;, AC_DEFINE(HAVE_SIGCLD,1,Do we have SIGCLD), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGCLD")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGWAITING;, AC_DEFINE(HAVE_SIGWAITING,1,Do we have SIGWAITING), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGWAITING")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGLWP;, AC_DEFINE(HAVE_SIGLWP,1,Do we have SIGLWP), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGLWP")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGFREEZE;, AC_DEFINE(HAVE_SIGFREEZE,1,Do we have SIGFREEZE), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGFREEZE")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGTHAW;, AC_DEFINE(HAVE_SIGTHAW,1,Do we have SIGTHAW), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGTHAW")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGCANCEL;, AC_DEFINE(HAVE_SIGCANCEL,1,Do we have SIGCANCEL), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGCANCEL")
AC_TRY_COMPILE([#include <signal.h>
#include <stdlib.h>],
int a=SIGLOST;, AC_DEFINE(HAVE_SIGLOST,1,Do we have SIGLOST), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGLOST")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=_SIGRTMIN;, AC_DEFINE(HAVE__SIGRTMIN,1,Do we have _SIGRTMIN), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGRTMIN")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=_SIGRTMAX;, AC_DEFINE(HAVE__SIGRTMAX,1,Do we have _SIGRTMAX), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGRTMAX")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGRTMIN;, AC_DEFINE(HAVE_SIGRTMIN,1,Do we have SIGRTMIN), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGTMIN")
AC_TRY_COMPILE([#include <signal.h> 
#include <stdlib.h>],
int a=SIGRTMAX;, AC_DEFINE(HAVE_SIGRTMAX,1,Do we have SIGRTMAX), UNSUPPORTEDSIGNALS="$UNSUPPORTEDSIGNALS SIGTMAX")

if ( test -n "$UNSUPPORTEDSIGNALS" )
then
	AC_MSG_RESULT($UNSUPPORTEDSIGNALS)
fi
])

dnl check to see which should be used of -lsocket, -lnsl and -lxnet
AC_DEFUN([FW_CHECK_SOCKET_LIBS],
[
	AC_LANG(C)
	SOCKETLIBS=""
	DONE=""
	for i in "" "-lnsl" "-lsocket" "-lsocket -lnsl" "-lxnet"
	do
		if ( test -n "$i" )
		then
			AC_MSG_CHECKING($i is required for socket-related calls)
		else
			AC_MSG_CHECKING(no extra libraries are required for socket-related calls)
		fi
		FW_TRY_LINK([#include <stdlib.h>],[connect(0,NULL,0); listen(0,0); bind(0,NULL,0); accept(0,NULL,0); send(0,NULL,0,0); sendto(0,NULL,0,0,NULL,0); sendmsg(0,NULL,0); gethostbyname(NULL);],[],[$i],[],[SOCKETLIBS="$i"; DONE="yes"; AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])
		if ( test -n "$DONE" )
		then
			break
		fi
	done
	AC_LANG(C++)

	if ( test -z "$DONE" )
	then
		AC_MSG_ERROR(no combination of networking libraries was found.)
	fi

	AC_SUBST(SOCKETLIBS)
])
