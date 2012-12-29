AC_DEFUN([FW_GMAKE],
[
AC_MSG_CHECKING(for GNU Make)
if ( test -n "make -v | grep 'GNU Make'" )
then
	MAKE="make"
	AC_MSG_RESULT(yes)
else
	if ( test -n "gmake -v | grep 'GNU Make'" )
	then
		MAKE="gmake"
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
		AC_MSG_ERROR(GNU make not found.  SQL-Relay requires GNU make.)
	fi
fi
])

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
dnl echo "check: $1"
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


AC_DEFUN([FW_TRY_COMPILE],
[
SAVECPPFLAGS="$CPPFLAGS"
CPPFLAGS="$3"
AC_TRY_COMPILE([$1],[$2],[$4],[$5])
CPPFLAGS="$SAVECPPFLAGS"
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


for path in "$SEARCHPATH" "/" "/usr" "/usr/local/$NAME" "/opt/$NAME" "/usr/$NAME" "/usr/local" "/usr/pkg" "/usr/pkg/$NAME" "/opt/sfw" "/opt/sfw/$NAME" "/usr/sfw" "/usr/sfw/$NAME" "/opt/csw" "/sw" "/boot/common" "/Library/$NAME" "/usr/local/firstworks"
do
	if ( test -n "$path" -a -d "$path" )
	then

		if ( test "$path" = "/" )
		then
			dnl look in /usr/include and /lib and /lib64
			if ( test "$USEFULLLIBPATH" = "yes" )
			then
				FW_CHECK_HEADER_LIB([/usr/include/$HEADER],[],[/lib/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"/lib\"; LIBSTRING=\"-Wl,/lib/lib$LIBNAME.$SOSUFFIX\"],[/lib/lib$LIBNAME.a],[LIBSTRING=\"/lib/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
			else
				FW_CHECK_HEADER_LIB([/usr/include/$HEADER],[],[/lib/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"/lib\"; LIBSTRING=\"-l$LIBNAME\"],[/lib/lib$LIBNAME.a],[LIBSTRING=\"-l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi

			if ( test "$USEFULLLIBPATH" = "yes" )
			then
				FW_CHECK_HEADER_LIB([/usr/include/$HEADER],[],[/lib64/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"/lib64\"; LIBSTRING=\"-Wl,/lib64/lib$LIBNAME.$SOSUFFIX\"],[/lib64/lib$LIBNAME.a],[LIBSTRING=\"/lib64/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
			else
				FW_CHECK_HEADER_LIB([/usr/include/$HEADER],[],[/lib64/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"/lib64\"; LIBSTRING=\"-l$LIBNAME\"],[/lib64/lib$LIBNAME.a],[LIBSTRING=\"-l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
			fi
			

			dnl set path to "" so we won't get //'s from here on
			path=""
		fi


		for libpath in "$path/lib64" "$path/lib64/$NAME" "$path/lib64/opt" "$path/lib" "$path/lib/$NAME" "$path/lib/opt"
		do

			if ( test -n "$LIBSTRING" )
			then
				break
			fi

			for includepath in "$path/include" "$path/include/$NAME"
			do

				if ( test -n "$LIBSTRING" )
				then
					break
				fi

				if ( test "$USEFULLLIBPATH" = "yes" )
				then
					FW_CHECK_HEADER_LIB([$includepath/$HEADER],[INCLUDESTRING=\"-I$includepath\"],[$libpath/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$libpath\"; LIBSTRING=\"-Wl,$libpath/lib$LIBNAME.$SOSUFFIX\"],[$libpath/lib$LIBNAME.a],[LIBSTRING=\"$libpath/lib$LIBNAME.a\"; STATIC=\"$LINKSTATIC\"])
				else
					FW_CHECK_HEADER_LIB([$includepath/$HEADER],[INCLUDESTRING=\"-I$includepath\"],[$libpath/lib$LIBNAME.$SOSUFFIX],[LIBPATH=\"$libpath\"; LIBSTRING=\"-L$libpath -l$LIBNAME\"],[$libpath/lib$LIBNAME.a],[LIBSTRING=\"-L$libpath -l$LIBNAME\"; STATIC=\"$LINKSTATIC\"])
				fi
			done
		done

		if ( test -n "$LIBSTRING" )
		then
			HEADERSANDLIBSPATH="$path"
			break
		fi
	fi
done

dnl remove -I/usr/include, -L/lib, -L/usr/lib, -L/lib64 and -L/usr/lib64
INCLUDESTRING=`echo $INCLUDESTRING | sed -e "s|-I/usr/include$||g" -e "s|-I/usr/include ||g"`
LIBSTRING=`echo $LIBSTRING | sed -e "s|-L/usr/lib$||g" -e "s|-L/lib$||g" -e "s|-L/usr/lib ||g" -e "s|-L/lib ||g"`
LIBSTRING=`echo $LIBSTRING | sed -e "s|-L/usr/lib64$||g" -e "s|-L/lib64$||g" -e "s|-L/usr/lib64 ||g" -e "s|-L/lib64 ||g"`

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
USAGE="`ps aux 2>&1 | grep -i usage | grep -v grep`"
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


dnl checks to see if -Wno-long-double option to gcc works or not
AC_DEFUN([FW_CHECK_WNOLONGDOUBLE],
[
AC_MSG_CHECKING(for -Wno-long-double option)
FW_TRY_LINK([#include <stdio.h>],[printf("hello");],[-Wno-long-double],[],[],[WNOLONGDOUBLE="-Wno-long-double"],[WNOLONGDOUBLE=""])
if ( test -n "$WNOLONGDOUBLE" )
then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AC_SUBST(WNOLONGDOUBLE)
])


dnl checks to see if -Wall option works or not
AC_DEFUN([FW_CHECK_WALL],
[
AC_MSG_CHECKING(for -Wall)
FW_TRY_LINK([#include <stdio.h>],[printf("hello");],[-Wall],[],[],[WALL="-Wall"],[WALL=""])
if ( test -n "$WALL" )
then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AC_SUBST(WALL)
])


dnl checks to see if -Werror option works or not
AC_DEFUN([FW_CHECK_WERROR],
[
AC_MSG_CHECKING(for -Werror)
FW_TRY_LINK([#include <stdio.h>],[printf("hello");],[-Werror],[],[],[WERROR="-Werror"],[WERROR=""])
if ( test -n "$WERROR" )
then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AC_SUBST(WERROR)
])



dnl checks to see if -g3 option works or not
AC_DEFUN([FW_CHECK_DEBUG],
[
if ( test "$DEBUG" = "yes" )
then
	AC_MSG_CHECKING(for -g3)
	FW_TRY_LINK([#include <stdio.h>],[printf("hello");],[-g3],[],[],[DBG="-g3"],[DBG="-g"])
	if ( test "$DBG" = "-g3" )
	then
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
	CFLAGS="$CFLAGS $DBG"
	CXXFLAGS="$CXXFLAGS $DBG"
fi
])


dnl Checks for microsoft platform.
dnl sets the substitution variables MINGW32, CYGWIN and UWIN as appropriate
dnl sets the enviroment variable MICROSOFT
AC_DEFUN([FW_CHECK_MICROSOFT],
[
AC_MSG_CHECKING(for microsoft platform)
CYGWIN=""
MINGW32=""
UWIN=""
case $host_os in
	*cygwin* )
		CYGWIN="yes"
		AC_MSG_RESULT(cygwin)
		;;
	*mingw32* )
		MINGW32="yes"
		AC_MSG_RESULT(mingw32)
		;;
	*uwin* )
		UWIN="yes"
		AC_MSG_RESULT(uwin)
		;;
	* )
		AC_MSG_RESULT(no)
		;;
esac
EXE=""
AC_SUBST(MINGW32)
AC_SUBST(CYGWIN)
AC_SUBST(UWIN)

MICROSOFT=""
if ( test "$UWIN" = "yes" -o "$MINGW32" = "yes" -o "$CYGWIN" = "yes" )
then
	MICROSOFT="yes"
	EXE=".exe"
fi

AC_SUBST(EXE)
AC_SUBST(MICROSOFT)

if ( test "$MINGW32" )
then
	AC_DEFINE(MINGW32,1,Mingw32 environment)

	dnl if we're building mingw32, we're cross-compiling by definition
	cross_compiling="yes"
fi
])


AC_DEFUN([FW_CHECK_OSX],
[
DARWIN=""
AC_MSG_CHECKING(for OSX)
case $host_os in
	*darwin* )
		DARWIN="yes"
		AC_MSG_RESULT(yes)
		;;
	* )
		AC_MSG_RESULT(no)
		;;
esac
])

dnl Checks for minix and adds some macros if it is
AC_DEFUN([FW_CHECK_MINIX],
[
AC_MSG_CHECKING(for minix)
case $host_os in
	*minix* )
		CPPFLAGS="$CPPFLAGS -D_MINIX -D_POSIX_SOURCE"
		AC_MSG_RESULT(yes)
		;;
	* )
		AC_MSG_RESULT(no)
		;;
esac
])

dnl Checks for haiku and adds some macros if it is
AC_DEFUN([FW_CHECK_HAIKU],
[
AC_MSG_CHECKING(for haiku)
case $host_os in
	*haiku* )
		if ( test "$prefix" = "NONE" )
		then
			prefix="/boot/common"
		fi
  		AC_DEFINE(ADD_NEWLINE_AFTER_READ_FROM_STDIN,1,On some platforms, you have to add a newline after reading from stdin)
		AC_MSG_RESULT(yes)
		;;
	* )
		AC_MSG_RESULT(no)
		;;
esac
])

dnl Determines what extension shared object files have
AC_DEFUN([FW_CHECK_SO_EXT],
[
AC_MSG_CHECKING(for dynamic library extensions)
SOSUFFIX="so"
MODULESUFFIX="so"
JNISUFFIX="so"
if ( test -n "$CYGWIN" )
then
	SOSUFFIX="dll.a"
fi
if ( test -n "$DARWIN" )
then
	SOSUFFIX="dylib"
	MODULESUFFIX="bundle"
	JNISUFFIX="jnilib"
fi
AC_MSG_RESULT(so=>$SOSUFFIX module=>$MODULESUFFIX jni=>$JNISUFFIX)
AC_SUBST(SOSUFFIX)
AC_SUBST(MODULESUFFIX)
AC_SUBST(JNISUFFIX)
AC_DEFINE_UNQUOTED(SQLRELAY_MODULESUFFIX,"$MODULESUFFIX",Suffix for loadable modules)
])

dnl Determines what extension shared object files have
AC_DEFUN([FW_CHECK_PLUGIN_DEPENDENCIES],
[
AC_MSG_CHECKING(for plugin dependencies)
PYTHONFRAMEWORK=""
TRIGGERPLUGINLIBS=""
TRANSLATIONPLUGINLIBS=""
if ( test -n "$CYGWIN" )
then
	TRIGGERPLUGINLIBS="-L./ -lsqlrconnection"
	TRANSLATIONPLUGINLIBS="-L./ -lsqlrconnection"
	AC_MSG_RESULT(Cygwin style)
elif (test -n "$DARWIN" )
then
	PYTHONFRAMEWORK="-framework Python"
	TRIGGERPLUGINLIBS="-L./ -lsqlrconnection"
	TRANSLATIONPLUGINLIBS="-L./ -lsqlrconnection"
	AC_MSG_RESULT(OSX style)
else
	AC_MSG_RESULT(standard unix style)
fi
AC_SUBST(PYTHONFRAMEWORK)
AC_SUBST(TRIGGERPLUGINLIBS)
AC_SUBST(TRANSLATIONPLUGINLIBS)
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
	SQLRELAY_NAMESPACE=""
	AC_MSG_CHECKING(namespace support)
	AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],[using namespace Outer::Inner; return i;],[SQLRELAY_NAMESPACE="yes"],[])
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
dnl sets the substitution variable PTHREADLIB
AC_DEFUN([FW_CHECK_PTHREAD],
[

AC_MSG_CHECKING(if -pthread works during compile phase)
if ( test -n "`$CXX -pthread 2>&1 | grep 'unrecognized option' | grep pthread`" )
then
	PTHREAD_COMPILE=""
else
	PTHREAD_COMPILE="-pthread"
fi
if ( test -n "$PTHREAD_COMPILE" )
then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

HAVE_PTHREAD=""
PTHREADINCLUDES=""
PTHREADLIB=""

if ( test "$cross_compiling" = "yes" )
then

	dnl cross compiling
	echo "cross compiling"

	if ( test -n "$PTHREADPATH" )
	then
		PTHREADINCLUDES="$PTHREAD_COMPILE -I$PTHREADPATH/include"
		PTHREADLIB="-L$PTHREADPATH/lib -lpthread -pthread"
	else
		PTHREADINCLUDES="$PTHREAD_COMPILE"
		PTHREADLIB="-lpthread -pthread"
	fi
	HAVE_PTHREAD="yes"

else

	dnl check pthread.h and standard thread libraries
	for i in "pthread" "c_r" "thread" "pthreads" "gthreads" ""
	do
		if ( test -n "$i" )
		then
			AC_MSG_CHECKING(for lib$i)
		else
			AC_MSG_CHECKING(for no library)
		fi

		INCLUDEDIR="pthread"
		if ( test "$i" = "gthreads" )
		then
			INCLUDEDIR="FSU"
		fi

		if ( test -n "$i" )
		then
			FW_CHECK_HEADERS_AND_LIBS([$PTHREADPATH],[$INCLUDEDIR],[pthread.h],[$i],[""],[""],[PTHREADINCLUDES],[PTHREADLIB],[PTHREADLIBPATH],[PTHREADSTATIC])
		fi

		if ( test -n "$PTHREADLIB" -o -z "$i" )
		then

			AC_MSG_RESULT(yes)

			if ( test -n "$i" )
			then
				AC_MSG_CHECKING(whether lib$i works)
			else
				AC_MSG_CHECKING(whether no library works)
			fi

			dnl  If we found a set of headers and libs, try
			dnl  linking with them.  We'll try six times,
			dnl  first with just the header and lib that we
			dnl  found, then with -pthread added to one,
			dnl  the other and both, and then finally
			dnl  without any libs, just -pthread
			for try in 1 2 3 4 5 6
			do

				if ( test "$try" = "1" )
				then
					TESTINCLUDES="$PTHREADINCLUDES"
					TESTLIB="$PTHREADLIB"
				elif ( test "$try" = "2" )
				then
					TESTINCLUDES="$PTHREADINCLUDES"
					TESTLIB="$PTHREADLIB -pthread"
				elif ( test "$try" = "3" )
				then
					TESTINCLUDES="$PTHREAD_COMPILE $PTHREADINCLUDES"
					TESTLIB="$PTHREADLIB"
				elif ( test "$try" = "4" )
				then
					TESTINCLUDES="$PTHREAD_COMPILE $PTHREADINCLUDESS"
					TESTLIB="$PTHREADLIB -pthread"
				elif ( test "$try" = "5" )
				then
					TESTINCLUDES="$PTHREADINCLUDESS"
					TESTLIB="-pthread"
				elif ( test "$try" = "6" )
				then
					TESTINCLUDES="$PTHREAD_COMPILE $PTHREADINCLUDESS"
					TESTLIB="-pthread"
				fi

				HAVE_PTHREAD=""
				dnl try to link
				FW_TRY_LINK([#include <pthread.h>],[pthread_create(NULL,NULL,NULL,NULL);],[$PTHREAD_COMPILE $CPPFLAGS $PTHREADINCLUDES],[$PTHREADLIB],[],[HAVE_PTHREAD="yes"],[])
				if ( test -z "$HAVE_PTHREAD" )
				then
					dnl try link again, some older
					dnl thread implementations have
					dnl non-pointer 2nd parameters
					FW_TRY_LINK([#include <pthread.h>],[pthread_create(NULL,pthread_attr_default,NULL,NULL);],[$PTHREAD_COMPILE $CPPFLAGS],[-pthread],[],[HAVE_PTHREAD="yes"],[])
				fi

				dnl  If the link succeeded then keep
				dnl  the flags.
				if ( test -n "$HAVE_PTHREAD" )
				then
					PTHREADINCLUDES="$TESTINCLUDES"
					PTHREADLIB="$TESTLIB"
					break
				fi

				dnl  If the link failed, reset the flags
				PTHREADINCLUDES=""
				PTHREADLIB=""
			done

			if ( test -n "$HAVE_PTHREAD" )
			then
				AC_MSG_RESULT(yes)
				break
			else
				AC_MSG_RESULT(no)
			fi

		else
			AC_MSG_RESULT(no)
		fi
	done
fi

FW_INCLUDES(pthreads,[$PTHREADINCLUDES])
FW_LIBS(pthreads,[$PTHREADLIB])

AC_SUBST(PTHREADINCLUDES)
AC_SUBST(PTHREADLIB)
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

	for i in "$RUDIMENTSPATH" "/usr" "/usr/local" "/opt/sfw" "/usr/sfw" "/opt/csw" "/usr/pkg" "/sw" "/usr/local/firstworks" "/boot/common"
	do
		if ( test -n "$i" -a -d "$i" )
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
	LINKFAIL=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling
		echo "cross compiling"

	else

		AC_MSG_CHECKING(for oracle includes and libraries)

		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi

		if ( test -n "$ORACLE_HOME" )
		then

			dnl use sysliblist if it's there
			SYSLIBLIST="`cat $ORACLE_HOME/lib/sysliblist 2> /dev/null`"
			if ( test ! -n "$SYSLIBLIST" )
			then
				SYSLIBLIST="-lm $AIOLIB"
			fi

			FW_CHECK_LIB([$ORACLE_HOME/lib/libcore4.a],[ORACLEVERSION=\"8.0\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclient -lncr -lcommon -lgeneric -lclntsh -lepcpt -lcore4 -lnlsrtl3 $SYSLIBLIST\"])
			FW_CHECK_LIB([$ORACLE_HOME/lib/libcore8.a],[ORACLEVERSION=\"8i\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclntsh $SYSLIBLIST\"])
			FW_CHECK_LIB([$ORACLE_HOME/lib/libcore9.a],[ORACLEVERSION=\"9i\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclntsh $SYSLIBLIST\"])
			FW_CHECK_LIB([$ORACLE_HOME/lib/libcore10.a],[ORACLEVERSION=\"10g\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclntsh $SYSLIBLIST\"])
			FW_CHECK_LIB([$ORACLE_HOME/lib/libcore11.a],[ORACLEVERSION=\"11g\"; ORACLELIBSPATH=\"$ORACLE_HOME/lib\"; ORACLELIBS=\"-L$ORACLE_HOME/lib -lclntsh $SYSLIBLIST\"])
			FW_CHECK_LIB([$ORACLE_HOME/lib/libclntsh.a],[ORACLESTATIC=\"$STATICFLAG\"])
		fi

		dnl if we didn't find anything yet,
		dnl look for non-RPM-based instantclient
		if ( test -z "$ORACLELIBS" )
		then

			dnl if a prefix wasn't passed in,
			dnl look in some common places
			if ( test -z "$ORACLE_INSTANTCLIENT_PREFIX" )
			then
				for i in "/usr" "/usr/lib" "/usr/local" "/opt"
				do
					INSTCLNT=`ls -d $i/instantclient* 2> /dev/null | tail -1`
					if ( test -n "$INSTCLNT" )
					then
						ORACLE_INSTANTCLIENT_PREFIX=$INSTCLNT
						break
					fi
				done
			fi

			dnl For some reason libclntsh.so is not included in the
			dnl non-RPM versions, so we have to look for and use
			dnl the file with a version number tacked on to the end.
			if ( test -n "$ORACLE_INSTANTCLIENT_PREFIX" -a -r "`ls $ORACLE_INSTANTCLIENT_PREFIX/libclntsh.$SOSUFFIX.* 2> /dev/null | tail -1`" -a -r "$ORACLE_INSTANTCLIENT_PREFIX/sdk/include/oci.h" )
			then
				ORACLEVERSION="10g"
				if ( test -n `echo $ORACLE_INSTANTCLIENT_PREFIX | grep 11` )
				then
					ORACLEVERSION="11g"
				fi
				ORACLELIBSPATH="$ORACLE_INSTANTCLIENT_PREFIX"
				CLNTSH="`ls $ORACLE_INSTANTCLIENT_PREFIX/libclntsh.$SOSUFFIX.* 2> /dev/null | tail -1`"
				NNZ=`basename $ORACLELIBSPATH/libnnz*.$SOSUFFIX | sed -e "s|lib||" -e "s|.$SOSUFFIX||"`
				ORACLELIBS="-Wl,$CLNTSH -L$ORACLE_INSTANTCLIENT_PREFIX -l$NNZ"
				ORACLEINCLUDES="-I$ORACLE_INSTANTCLIENT_PREFIX/sdk/include"
			fi
		fi

		dnl if we didn't find anything yet, look for RPM-based
		dnl instantclient, which, oddly enough, does contain
		dnl libclntsh.so
		if ( test -z "$ORACLELIBS" )
		then
			for version in `cd /usr/lib/oracle 2> /dev/null; ls -d * 2> /dev/null`
			do
				if ( test -r "/usr/lib/oracle/$version/client/lib/libclntsh.$SOSUFFIX" -a -r "/usr/include/oracle/$version/client/oci.h" )
				then
					ORACLEVERSION="10g"
					if ( test -n `echo $version | grep 11` )
					then
						ORACLEVERSION="11g"
					fi
					ORACLELIBSPATH="/usr/lib/oracle/$version/client/lib"
					NNZ=`basename $ORACLELIBSPATH/libnnz*.$SOSUFFIX | sed -e "s|lib||" -e "s|.$SOSUFFIX||"`
					ORACLELIBS="-L/usr/lib/oracle/$version/client/lib -lclntsh -l$NNZ"
					ORACLEINCLUDES="-I/usr/include/oracle/$version/client"
				fi

				dnl x86_64 uses client64 rather than client
				if ( test -r "/usr/lib/oracle/$version/client64/lib/libclntsh.$SOSUFFIX" -a -r "/usr/include/oracle/$version/client64/oci.h" )
				then
					ORACLEVERSION="10g"
					if ( test -n `echo $version | grep 11` )
					then
						ORACLEVERSION="11g"
					fi
					ORACLELIBSPATH="/usr/lib/oracle/$version/client64/lib"
					NNZ=`basename $ORACLELIBSPATH/libnnz*.$SOSUFFIX | sed -e "s|lib||" -e "s|.$SOSUFFIX||"`
					ORACLELIBS="-L/usr/lib/oracle/$version/client64/lib -lclntsh -l$NNZ"
					ORACLEINCLUDES="-I/usr/include/oracle/$version/client64"
				fi
			done
		fi
		
		if ( test -n "$ORACLEVERSION" )
		then
			if ( test -z "$ORACLEINCLUDES" )
			then
				ORACLEINCLUDES="-I$ORACLE_HOME/rdbms/demo -I$ORACLE_HOME/rdbms/public -I$ORACLE_HOME/network/public -I$ORACLE_HOME/plsql/public"
			fi
		fi

		if ( test -n "$ORACLELIBS" -a -n "$ORACLEINCLUDES" )
		then
			AC_MSG_RESULT(yes)
		else
			AC_MSG_RESULT(no)
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
$GLIBC23HACKCODE],[exit(0)],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); OCI_H=\"yes\"],[AC_MSG_RESULT(no)])
			else
				FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <oci.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[exit(0)],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); OCI_H=\"yes\"],[AC_MSG_RESULT(no)])
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
$GLIBC23HACKCODE],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			else
				FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <ociapr.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
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
$GLIBC23HACKCODE],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIBS $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); ORACLESTATIC=""; LINKFAIL="yes"])
				else
					FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <ociapr.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,"",-1,"",-1,"",-1,OCI_LM_DEF);],[$ORACLESTATIC $ORACLEINCLUDES,$ORACLELIBS $SOCKETLIBS $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); ORACLESTATIC=""; LINKFAIL="yes"])
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
$GLIBC23HACKCODE],[olog(NULL,NULL,NULL,-1,NULL,-1,NULL,-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			else
				FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <ociapr.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,NULL,-1,NULL,-1,NULL,-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
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
$GLIBC23HACKCODE],[olog(NULL,NULL,NULL,-1,NULL,-1,NULL,-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIBS $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); LINKFAIL="yes"])
				else
					FW_TRY_LINK([#ifdef __CYGWIN__
	#define _int64 long long
#endif
#include <ociapr.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[olog(NULL,NULL,NULL,-1,NULL,-1,NULL,-1,OCI_LM_DEF);],[$ORACLEINCLUDES],[$ORACLELIBS $SOCKETLIBS $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); ORACLELIBS="$ORACLELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no); LINKFAIL="yes"])
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
		
	if ( test "$ORACLEVERSION" = "8i" -o "$ORACLEVERSION" = "9i" -o "$ORACLEVERSION" = "10g" -o "$ORACLEVERSION" = "11g" )
	then
		AC_DEFINE(HAVE_ORACLE_8i,1,Oracle 8i or greater)
	fi

	if ( test -n $"ORACLEVERSION" )
	then
		FW_VERSION(oracle,[$ORACLEVERSION])
		FW_INCLUDES(oracle,[$ORACLEINCLUDES])
		FW_LIBS(oracle,[$ORACLELIBS])
	fi

	if ( test -z "$ORACLELIBS" -o -n "$LINKFAIL" )
	then
		AC_MSG_WARN(Oracle support will not be built.)
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

		if ( test -z "$MYSQLLIBS" )
		then
			MYSQLINCLUDES=`mysql_config --cflags 2> /dev/null | sed -e "s|'||g"`
			dnl on some platforms, mysql_config returns options
			dnl that the native compiler likes but g++ does not
			if ( test -n "`echo $CXX | grep g++`" )
			then
				MYSQLINCLUDES=`echo $MYSQLINCLUDES | sed -e "s|-x.* ||g" -e "s|-x.*$||g" -e "s|-nofstore ||g" -e "s|-nofstore$||g" -e "s|-f.* ||g" -e "s|-f.*$||g" -e "s|-mt ||g" -e "s|-mt$||g"`
			fi
			MYSQLLIBS=`mysql_config --libs 2> /dev/null | sed -e "s|'||g"`

			if ( test -n "$MYSQLLIBS" )
			then
				dnl sanity check
				AC_MSG_CHECKING(for valid mysql_config output)
				FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_close(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); MYSQLINCLUDES=""; MYSQLLIBS=""])
			fi
		fi

		if ( test -z "$MYSQLLIBS" )
		then
			for i in "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/usr/local/mysql/bin" "/opt/sfw/bin" "/opt/sfw/mysql/bin" "/usr/sfw/bin" "/usr/sfw/mysql/bin" "/opt/csw/bin" "/sw/bin"
			do
				if ( test -d "$i" )
				then
					MYSQLINCLUDES=`$i/mysql_config --cflags 2> /dev/null | sed -e "s|'||g"`
					MYSQLLIBS=`$i/mysql_config --libs 2> /dev/null | sed -e "s|'||g"`
					if ( test -n "$MYSQLLIBS" )
					then
						break
					fi
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
#include <stdlib.h>],[mysql_real_connect(NULL,NULL,NULL,NULL,NULL,0,NULL,0); mysql_real_query(NULL,NULL,0); mysql_store_result(NULL); mysql_num_fields(NULL); mysql_fetch_row(NULL); mysql_free_result(NULL); mysql_close(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); MYSQLLIBS="$MYSQLLIBS -lz"])
		
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
	fi

	if ( test -z "$MYSQLLIBS" )
	then
		AC_MSG_WARN(MySQL support will not be built.)
	else
		FW_CHECK_MYSQL_FUNCTIONS()
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
#include <stdlib.h>],[mysql_real_connect(NULL,NULL,NULL,NULL,NULL,0,NULL,0);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_REAL_CONNECT_FOR_SURE,1,MySQL supports mysql_real_connect)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_select_db)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_select_db(NULL,NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_SELECT_DB,1,MySQL supports mysql_select_db)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_ping)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_ping(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_PING,1,MySQL supports mysql_ping)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_change_user)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_change_user(NULL,NULL,NULL,NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_CHANGE_USER,1,MySQL supports mysql_change_user)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_commit)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_commit(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_COMMIT,1,MySQL supports mysql_commit)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_rollback)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_rollback(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_ROLLBACK,1,MySQL supports mysql_rollback)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_autocommit)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_autocommit(NULL,0);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_AUTOCOMMIT,1,MySQL supports mysql_autocommit)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_prepare)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_stmt_prepare(NULL,NULL,0);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_STMT_PREPARE,1,MySQL supports mysql_stmt_prepare)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_next_result)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_next_result(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_NEXT_RESULT,1,MySQL supports mysql_next_result)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for mysql_set_character_set)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_set_character_set(NULL,"");],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_SET_CHARACTER_SET,1,MySQL supports mysql_set_character_set)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for CR_SERVER_GONE_ERROR)
	FW_TRY_LINK([#include <mysql.h>
#include <errmsg.h>
#include <stdlib.h>],[int err=CR_SERVER_GONE_ERROR;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_CR_SERVER_GONE_ERROR,1,MySQL supports CR_SERVER_GONE_ERROR)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for CR_SERVER_LOST)
	FW_TRY_LINK([#include <mysql.h>
#include <errmsg.h>
#include <stdlib.h>],[int err=CR_SERVER_LOST;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_CR_SERVER_LOST,1,MySQL supports CR_SERVER_LOST)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for FIELD_TYPE_YEAR)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[MYSQL_FIELD field; field.type=FIELD_TYPE_YEAR;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_FIELD_TYPE_YEAR,1,MySQL supports FIELD_TYPE_YEAR)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for FIELD_TYPE_NEWDATE)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[MYSQL_FIELD field; field.type=FIELD_TYPE_NEWDATE;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_FIELD_TYPE_NEWDATE,1,MySQL supports FIELD_TYPE_NEWDATE)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for FIELD_TYPE_ENUM)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[MYSQL_FIELD field; field.type=FIELD_TYPE_ENUM;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_FIELD_TYPE_ENUM,1,MySQL supports FIELD_TYPE_ENUM)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for FIELD_TYPE_SET)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[MYSQL_FIELD field; field.type=FIELD_TYPE_SET;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_FIELD_TYPE_SET,1,MySQL supports FIELD_TYPE_SET)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for FIELD_TYPE_NEWDECIMAL)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[MYSQL_FIELD field; field.type=FIELD_TYPE_NEWDECIMAL;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_FIELD_TYPE_NEWDECIMAL,1,MySQL supports FIELD_TYPE_NEWDECIMAL)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for MYSQL_OPT_RECONNECT)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_option a; a=MYSQL_OPT_RECONNECT;],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_OPT_RECONNECT,1,MySQL supports MYSQL_OPT_RECONNECT)],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(for MYSQL_GET_SERVER_VERSION)
	FW_TRY_LINK([#include <mysql.h>
#include <stdlib.h>],[mysql_get_server_version(NULL);],[$MYSQLSTATIC $MYSQLINCLUDES],[$MYSQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MYSQL_GET_SERVER_VERSION,1,MySQL supports MYSQL_GET_SERVER_VERSION)],[AC_MSG_RESULT(no)])
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

		if ( test -z "$POSTGRESQLLIBS" )
		then
			for dir in `ls /Library/PostgreSQL 2> /dev/null`
			do
				FW_CHECK_HEADERS_AND_LIBS([$POSTGRESQLPATH],[PostgreSQL/$dir],[libpq-fe.h],[pq],[$STATICFLAG],[$RPATHFLAG],[POSTGRESQLINCLUDES],[POSTGRESQLLIBS],[POSTGRESQLLIBSPATH],[POSTGRESQLSTATIC])
			done
		fi
		
		LINKFAIL=""
		if ( test -n "$POSTGRESQLSTATIC" -a -n "$POSTGRESQLLIBS" )
		then
			AC_MSG_CHECKING(if PostgreSQL can be statically linked without -lcrypt)
			FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetdbLogin(NULL,NULL,NULL,NULL,NULL,NULL,NULL);],[$POSTGRESQLSTATIC $POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" )
			then
				AC_MSG_CHECKING(if PostgreSQL can be statically linked with -lcrypt)
				FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetdbLogin(NULL,NULL,NULL,NULL,NULL,NULL,NULL);],[$POSTGRESQLSTATIC $POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS -lcrypt],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); POSTGRESQLLIBS="$POSTGRESQLLIBS -lcrypt"; LINKFAIL=""],[AC_MSG_RESULT(no); POSTGRESQLSTATIC=""; LINKFAIL="yes"])
			fi
		fi
		
		if ( test -z "$POSTGRESQLSTATIC" -a -n "$POSTGRESQLLIBS" )
		then
			AC_MSG_CHECKING(if PostgreSQL can be dynamically linked without -lcrypt)
			FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetdbLogin(NULL,NULL,NULL,NULL,NULL,NULL,NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" )
			then
				AC_MSG_CHECKING(if PostgreSQL can be dynamically linked with -lcrypt)
				FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetdbLogin(NULL,NULL,NULL,NULL,NULL,NULL,NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS -lcrypt],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); POSTGRESQLLIBS="$POSTGRESQLLIBS -lcrypt"; LINKFAIL=""],[AC_MSG_RESULT(no); LINKFAIL="yes"])
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
#include <stdlib.h>],[PQfmod(NULL,0);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQFMOD,1,Some versions of postgresql have PQfmod)],[AC_MSG_RESULT(no)])
		AC_MSG_CHECKING(if PostgreSQL has PQsetNoticeProcessor)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetNoticeProcessor(NULL,NULL,NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR,1,Some versions of postgresql have PQsetNoticeProcessor)],[AC_MSG_RESULT(no)])
		AC_MSG_CHECKING(if PostgreSQL has PQprepare)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQprepare(NULL,NULL,NULL,NULL,NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQPREPARE,1,Some versions of postgresql have PQprepare)],[AC_MSG_RESULT(no)])
		AC_MSG_CHECKING(if PostgreSQL has PQexecPrepared)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQexecPrepared(NULL,NULL,0,NULL,NULL,NULL,0);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQEXECPREPARED,1,Some versions of postgresql have PQexecPrepared)],[AC_MSG_RESULT(no)])
		AC_MSG_CHECKING(if PostgreSQL has PQserverVersion)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQserverVersion(NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQSERVERVERSION,1,Some versions of postgresql have PQserverVersion)],[AC_MSG_RESULT(no)])
		AC_MSG_CHECKING(if PostgreSQL has PQparameterStatus)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQparameterStatus(NULL,NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQPARAMETERSTATUS,1,Some versions of postgresql have PQparameterStatus)],[AC_MSG_RESULT(no)])
		AC_MSG_CHECKING(if PostgreSQL has PQsetClientEncoding)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQsetClientEncoding(NULL,NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQSETCLIENTENCODING,1,Some versions of postgresql have PQsetClientEncoding)],[AC_MSG_RESULT(no)])
		AC_MSG_CHECKING(if PostgreSQL has PQoidValue)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQoidValue(NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQOIDVALUE,1,Some versions of postgresql have PQoidValue)],[AC_MSG_RESULT(no)])
		AC_MSG_CHECKING(if PostgreSQL has PQbinaryTuples)
		FW_TRY_LINK([#include <libpq-fe.h>
#include <stdlib.h>],[PQbinaryTuples(NULL);],[$POSTGRESQLINCLUDES],[$POSTGRESQLLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$POSTGRESQLLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_POSTGRESQL_PQBINARYTUPLES,1,Some versions of postgresql have PQbinaryTuples)],[AC_MSG_RESULT(no)])
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
			DIRNAME1=`dirname $GLIBINCLUDES 2> /dev/null`
			DIRNAME2=`dirname $DIRNAME1 2> /dev/null`
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
			SQLITELIBS="-L$SQLITEPATH/lib -lsqlite3"
			SQLITELIBSPATH="$SQLITEPATH/lib"
			SQLITEVERSION="3"
		fi
	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		
		if ( test -n "$HAVE_PTHREAD" )
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
				FW_TRY_LINK([#include <sqlite.h>],[sqlite *sqliteptr; char *errmesg; sqliteptr=sqlite_open("/tmp/testfile",666,&errmesg); sqlite_close(sqliteptr);],[$SQLITESTATIC $SQLITEINCLUDES],[$SQLITELIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$SQLITELIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); SQLITENEEDGDBM="yes"])
			
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

	AC_MSG_CHECKING(for sqlite3_bind_int)
	FW_TRY_LINK([#include <sqlite3.h>
#include <stdlib.h>],[sqlite3_bind_int(NULL,0,0);],[$SQLITESTATIC $SQLITEINCLUDES],[$SQLITELIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_SQLITE3_BIND_INT,1,SQLite supports sqlite3_bind_int)],[AC_MSG_RESULT(no)])
fi
])

AC_DEFUN([FW_CHECK_ICONV],
[
	FW_CHECK_HEADERS_AND_LIBS([/usr],[iconv],[iconv.h],[iconv],[$STATICFLAG],[$RPATHFLAG],[ICONVINCLUDES],[ICONVLIBS],[ICONVLIBPATH],[ICONVSTATIC])

	HAVE_ICONV=""
	AC_MSG_CHECKING(for iconv)
	FW_TRY_LINK([#include <iconv.h>
#include <stdlib.h>],[iconv(0,NULL,NULL,NULL,NULL);],[$ICONVINCLUDES],[$ICONVLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); HAVE_ICONV="yes"],[AC_MSG_RESULT(no)])

	AC_MSG_CHECKING(if iconv takes const char ** argument)
	FW_TRY_LINK([#include <iconv.h>
#include <stdlib.h>],[const char *t; iconv(0,&t,NULL,NULL,NULL);],[$ICONVINCLUDES],[$ICONVLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(ICONV_CONST_CHAR,1,Some iconv implementations use a const char ** parameter)],[AC_MSG_RESULT(no)])

	AC_SUBST(HAVE_ICONV)
	AC_SUBST(ICONVINCLUDES)
	AC_SUBST(ICONVLIBS)
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
			FW_TRY_LINK([],[],[$FREETDSINCLUDES $PTHREADINCLUDES],[$FREETDSLIBS $PTHREADLIBS],[$LD_LIBRARY_PATH],[],[LINKFAILED="yes"])

			dnl if not, search for iconv
			if ( test -n "$LINKFAILED" )
			then

				dnl if iconv was found, try the test again,
				dnl using it
				dnl if it was not found, then freetds just
				dnl doesn't work
				if ( test -n "$ICONVLIBS" )
				then
					AC_MSG_CHECKING(whether freetds requires libiconv)

					FW_TRY_LINK([],[],[$FREETDSINCLUDES $ICONVINCLUDES $PTHREADINCLUDES],[$FREETDSLIBS $ICONVLIBS $PTHREADLIBS],[$LD_LIBRARY_PATH],[FREETDSINCLUDES="$FREETDSINCLUDES $ICONVINCLUDES"; FREETDSLIBS="$FREETDSLIBS $ICONVLIBS"; AC_MSG_RESULT(yes)],[FREETDSLIBS=""; FREETDSINCLUDES=""; AC_MSG_RESULT(no)])
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
				AC_MSG_CHECKING(whether ctpublic.h contains function definitions)
				FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$FREETDSINCLUDES $PTHREADINCLUDES],[$FREETDSLIBS $PTHREADLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_FREETDS_FUNCTION_DEFINITIONS,1,Some versions of FreeTDS have function definitions)],[AC_MSG_RESULT(no)])
			fi
		fi

		dnl if FREETDSLIBS is defined, check for tdsver.h
		if ( test -n "$FREETDSLIBS" )
		then
			AC_MSG_CHECKING(whether tdsver.h exists)
			FW_TRY_LINK([#include <tdsver.h>],[],[$FREETDSINCLUDES $PTHREADINCLUDES],[$FREETDSLIBS $PTHREADLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_FREETDS_H,1,Some versions of FreeTDS have tdsver.h)],[AC_MSG_RESULT(no)])
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
			FW_CHECK_HEADER_LIB([$SYBASEPATH/include/ctpublic.h],[SYBASEINCLUDES=\"-I$SYBASEPATH/include\"],[$SYBASEPATH/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"$SYBASEPATH/lib\"; SYBASELIBS=\"-L$SYBASEPATH/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"],[$SYBASEPATH/lib/libct.a],[SYBASELIBS=\"-L$SYBASEPATH/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"])

			FW_CHECK_HEADER_LIB([$SYBASEPATH/include/ctpublic.h],[SYBASEINCLUDES=\"-DSYB_LP64 -I$SYBASEPATH/include\"],[$SYBASEPATH/lib/libsybct64.$SOSUFFIX],[SYBASELIBSPATH=\"$SYBASEPATH/lib\"; SYBASELIBS=\"-L$SYBASEPATH/lib -lsybblk64 -lsybct64 -lsybcs64 -lsybcomn64 -lsybtcl64 -lsybdb64 -lsybintl64\"],[$SYBASEPATH/lib/libsybct64.a],[SYBASELIBS=\"-L$SYBASEPATH/lib -lsybblk64 -lsybct64 -lsybcs64 -lsybcomn64 -lsybsybtcl64 -lsybsybdb64 -lsybintl64\"; SYBASESTATIC=\"$STATICFLAG\"])
		else
		
			FW_CHECK_HEADER_LIB([/usr/local/sybase/include/ctpublic.h],[SYBASEINCLUDES=\"-I/usr/local/sybase/include\"],[/usr/local/sybase/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"/usr/local/sybase/lib\"; SYBASELIBS=\"-L/usr/local/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"],[/usr/local/sybase/lib/libct.a],[SYBASELIBS=\"-L/usr/local/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"])
		
			FW_CHECK_HEADER_LIB([/opt/sybase/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase/include\"],[/opt/sybase/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"/opt/sybase/lib\"; SYBASELIBS=\"-L/opt/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"],[/opt/sybase/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase/lib -lblk -lcs -lct -lcomn -lsybtcl -lsybdb -lintl -linsck\"; SYBASESTATIC=\"$STATICFLAG\"])

			FW_CHECK_HEADER_LIB([/opt/sybase-12.5/OCS-12_5/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase-12.5/OCS-12_5/include\"],[/opt/sybase-12.5/OCS-12_5/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"/opt/sybase-12.5/OCS-12_5/lib\"; SYBASELIBS=\"-L/opt/sybase-12.5/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"],[/opt/sybase-12.5/OCS-12_5/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase-12.5/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"; SYBASESTATIC=\"$STATICFLAG\"])

			FW_CHECK_HEADER_LIB([/opt/sybase/OCS-12_5/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase/OCS-12_5/include\"],[/opt/sybase/OCS-12_5/lib/libct.$SOSUFFIX],[SYBASELIBSPATH=\"/opt/sybase/OCS-12_5/lib\"; SYBASELIBS=\"-L/opt/sybase/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"],[/opt/sybase/OCS-12_5/lib/libct.a],[SYBASELIBS=\"-L/opt/sybase/OCS-12_5/lib -lblk -lct -lcs -lcomn -lsybtcl -lsybdb -lintl\"; SYBASESTATIC=\"$STATICFLAG\"])

			FW_CHECK_HEADER_LIB([/opt/sybase/OCS-15_0/include/ctpublic.h],[SYBASEINCLUDES=\"-I/opt/sybase/OCS-15_0/include\"],[/opt/sybase/OCS-15_0/lib/libsybct.$SOSUFFIX],[SYBASELIBSPATH=\"/opt/sybase/OCS-15_0/lib\"; SYBASELIBS=\"-L/opt/sybase/OCS-15_0/lib -lsybblk -lsybct -lsybcs -lsybcomn -lsybtcl -lsybdb -lsybintl\"],[/opt/sybase/OCS-15_0/lib/libsybct.a],[SYBASELIBS=\"-L/opt/sybase/OCS-15_0/lib -lsybblk -lsybct -lsybcs -lsybcomn -lsybsybtcl -lsybsybdb -lsybintl\"; SYBASESTATIC=\"$STATICFLAG\"])

			FW_CHECK_HEADER_LIB([/opt/sybase/OCS-15_0/include/ctpublic.h],[SYBASEINCLUDES=\"-DSYB_LP64 -I/opt/sybase/OCS-15_0/include\"],[/opt/sybase/OCS-15_0/lib/libsybct64.$SOSUFFIX],[SYBASELIBSPATH=\"/opt/sybase/OCS-15_0/lib\"; SYBASELIBS=\"-L/opt/sybase/OCS-15_0/lib -lsybblk64 -lsybct64 -lsybcs64 -lsybcomn64 -lsybtcl64 -lsybdb64 -lsybintl64\"],[/opt/sybase/OCS-15_0/lib/libsybct64.a],[SYBASELIBS=\"-L/opt/sybase/OCS-15_0/lib -lsybblk64 -lsybct64 -lsybcs64 -lsybcomn64 -lsybsybtcl64 -lsybsybdb64 -lsybintl64\"; SYBASESTATIC=\"$STATICFLAG\"])
		
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
$GLIBC23HACKCODE],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASESTATIC $SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Sybase can be statically linked with $DLLIB)
				FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASESTATIC $SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIBS $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); SYBASELIBS="$SYBASELIBS $DLLIB"; LINKFAIL="";],[AC_MSG_RESULT(no); SYBASESTATIC=""])
			fi
		fi
		
		if ( test -n "$DLLIB" -a -z "$SYBASESTATIC" -a -n "$SYBASELIBS" )
		then
			AC_MSG_CHECKING(if Sybase can be dynamically linked without $DLLIB)
			FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$SYBASELIBSPATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Sybase can be dynamically linked with $DLLIB)
				FW_TRY_LINK([#include <ctpublic.h>
#include <stdlib.h>
$GLIBC23HACKINCLUDE
$GLIBC23HACKCODE],[CS_CONTEXT *context; cs_ctx_alloc(CS_VERSION_100,&context);],[$SYBASEINCLUDES],[$SYBASELIBS $SOCKETLIBS $DLLIB],[$LD_LIBRARY_PATH:$SYBASELIBSPATH],[AC_MSG_RESULT(yes); SYBASELIBS="$SYBASELIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no)])
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

	ODBCINCLUDES=""
	ODBCLIBS=""
	ODBCLIBSPATH=""
	ODBCSTATIC=""
	ODBCUNICODE=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$ODBCPATH" )
		then
			ODBCINCLUDES="-I$ODBCPATH/include"
			ODBCLIBS="-L$ODBCPATH/lib -lodbc -lodbcinst"
			ODBCLIBSPATH="$ODBCPATH/lib"
			ODBCUNICODE="yes"
		fi

	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		HAVE_IODBC=""
		HAVE_UNIXODBC=""

		UNIXODBCLIBS=""
		UNIXODBCLIBSPATH=""
		UNIXODBCSTATIC=""
		UNIXODBCINCLUDES=""

		IODBCLIBS=""
		IODBCLIBSPATH=""
		IODBCSTATIC=""
		IODBCINCLUDES=""

		IODBCINSTLIBS=""
		IODBCINSTLIBSPATH=""
		IODBCINSTSTATIC=""
		IODBCINSTINCLUDES=""

		FW_CHECK_HEADERS_AND_LIBS([$ODBCPATH],[unixodbc],[sql.h],[odbc],[$STATICFLAG],[$RPATHFLAG],[UNIXODBCINCLUDES],[UNIXODBCLIBS],[UNIXODBCLIBSPATH],[UNIXODBCSTATIC])

		if ( test -n "$UNIXODBCLIBS" )
		then
			FW_CHECK_HEADERS_AND_LIBS([$ODBCPATH],[odbcinst],[sql.h],[odbcinst],[$STATICFLAG],[$RPATHFLAG],[UNIXODBCINSTINCLUDES],[UNIXODBCINSTLIBS],[UNIXODBCINSTLIBSPATH],[UNIXODBCINSTSTATIC])
			HAVE_UNIXODBC="yes"
		fi

		FW_CHECK_HEADERS_AND_LIBS([$ODBCPATH],[iodbc],[sql.h],[iodbc],[$STATICFLAG],[$RPATHFLAG],[IODBCINCLUDES],[IODBCLIBS],[IODBCLIBSPATH],[IODBCSTATIC])
		if ( test -n "$IODBCLIBS" )
		then
			FW_CHECK_HEADERS_AND_LIBS([$ODBCPATH],[iodbcinst],[sql.h],[iodbcinst],[$STATICFLAG],[$RPATHFLAG],[IODBCINSTINCLUDES],[IODBCINSTLIBS],[IODBCINSTLIBSPATH],[IODBCINSTSTATIC])
			HAVE_IODBC="yes"
		fi

		if ( test -n "$MICROSOFT" -a -z "$ODBCLIBS" )
		then
			FW_CHECK_HEADER_LIB([/usr/include/w32api/sql.h],[],[/usr/lib/w32api/libodbc32.$SOSUFFIX],[ODBCLIBSPATH=\"/usr/lib/w32api\"; ODBCLIBS=\"-L/usr/lib/w32api -lodbc32\"],[/usr/lib/w32api/libodbc32.a],[ODBCLIBSPATH=\"/usr/lib/w32api\"; ODBCLIBS=\"-L/usr/lib/w32api -lodbc32\"; STATIC=\"$STATICFLAG\"])
		fi

		if ( test -n "$HAVE_IODBC" )
		then
			ODBCLIBS="$IODBCLIBS $IODBCINSTLIBS"
			ODBCINCLUDES="$IODBCINCLUDES $IODBCINSTINCLUDES"
			ODBCLIBSPATH="$IODBCLIBSPATH"
			ODBCSTATIC="$IODBCSTATIC"
		elif ( test -n "$HAVE_UNIXODBC" )
		then
			ODBCLIBS="$UNIXODBCLIBS $UNIXODBCINSTLIBS"
			ODBCINCLUDES="$UNIXODBCINCLUDES $UNIXODBCINSTINCLUDES"
			ODBCLIBSPATH="$UNIXODBCLIBSPATH"
			ODBCSTATIC="$UNIXODBCSTATIC"
		fi
		
		if ( test -n "$HAVE_UNIXODBC" )
		then
			AC_DEFINE(HAVE_UNIXODBC,1,UnixODBC)
			AC_MSG_CHECKING(if UnixODBC needs threads)
			FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLHENV env; SQLHDBC dbc; SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env); SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc); SQLFreeHandle(SQL_HANDLE_DBC,dbc); SQLFreeHandle(SQL_HANDLE_ENV,env);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); ODBCINCLUDES="$ODBCINCLUDES $PTHREADINCLUDES"; ODBCLIBS="$ODBCLIBS $PTHREADLIB"])
		fi

		if ( test -n "$HAVE_IODBC" )
		then
			AC_DEFINE(HAVE_IODBC,1,iODBC)
			AC_MSG_CHECKING(if iODBC needs threads)
			FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLHENV env; SQLHDBC dbc; SQLAllocEnv(&env); SQLAllocConnect(env,&dbc); SQLFreeConnect(&dbc); SQLFreeEnv(&env);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(no)],[AC_MSG_RESULT(yes); ODBCINCLUDES="$ODBCINCLUDES $PTHREADINCLUDES"; ODBCLIBS="$ODBCLIBS $PTHREADLIB"])
		fi
	fi

	if ( test -n "$ODBCLIBS" )
	then
		AC_MSG_CHECKING(if SQLBindParameter takes SQLLEN * argument)
		FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLBindParameter(0,0,0,0,0,0,0,0,0,(SQLLEN *)1);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(SQLBINDPARAMETER_SQLLEN,1,Some systems use SQLLEN * in SQLBINDPARAMETER)],[AC_MSG_RESULT(no)])

		AC_MSG_CHECKING(if SQLColAttribute takes SQLLEN * argument)
		FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>

extern "C" SQLRETURN SQL_API SQLColAttribute(SQLHSTMT statementhandle,
					SQLUSMALLINT columnnumber,
					SQLUSMALLINT fieldidentifier,
					SQLPOINTER characterattribute,
					SQLSMALLINT bufferlength,
					SQLSMALLINT *stringlength,
					SQLLEN *numericattribute) {
	return 1;
}],[],[$ODBCSTATIC $ODBCINCLUDES],[],[],[AC_MSG_RESULT(yes); AC_DEFINE(SQLCOLATTRIBUTE_SQLLEN,1,Some systems use SQLLEN * in SQLColAttribute)],[AC_MSG_RESULT(no)])

		AC_MSG_CHECKING(if SQLRowCount takes SQLLEN * argument)
		FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>

extern "C" SQLRETURN SQL_API SQLRowCount(SQLHSTMT statementhandle,
					SQLLEN *rowcount) {
	return 1;
}],[],[$ODBCSTATIC $ODBCINCLUDES],[],[],[AC_MSG_RESULT(yes); AC_DEFINE(SQLROWCOUNT_SQLLEN,1,Some systems use SQLLEN * in SQLRowCount)],[AC_MSG_RESULT(no)])
		
		AC_MSG_CHECKING(if SQLBindCol takes SQLLEN * argument)
		FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLBindCol(0,0,0,0,0,(SQLLEN *)0);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(SQLBINDCOL_SQLLEN,1,Some systems use SQLLEN * in SQLBindCol)],[AC_MSG_RESULT(no)])
		
		AC_MSG_CHECKING(for SQLConnectW)
		FW_TRY_LINK([#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>],[SQLConnectW(0,NULL,0,NULL,0,NULL,0);],[$ODBCSTATIC $ODBCINCLUDES],[$ODBCLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH:$ODBCLIBSPATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_SQLCONNECTW,1,Some systems have SQLConnectW) ODBCUNICODE="yes"],[AC_MSG_RESULT(no)])
	fi

	if ( test -z "$ODBCLIBS" -o -z "$ODBCUNICODE" )
	then
		AC_MSG_WARN(ODBC connection support will not be built.)
	fi

	if ( test -n "$ODBCLIBS" -a -n "$OBCUNICODE" -a -z "$HAVE_ICONV" )
	then
		AC_MSG_WARN(iconv support missing... ODBC connection support will not be built.)
		ODBCLIBS=""
	fi

	if ( test -z "$ODBCLIBS" )
	then
		AC_MSG_WARN(ODBC driver will not be built.)
	fi

	if ( test -n "$ODBCLIBS" )
	then
		FW_INCLUDES(odbc,[$ODBCINCLUDES])
	fi
	if ( test -n "$ODBCLIBS" )
	then
		FW_LIBS(odbc,[$ODBCLIBS])
	fi
		
	AC_SUBST(ODBCINCLUDES)
	AC_SUBST(ODBCLIBS)
	AC_SUBST(ODBCLIBSPATH)
	AC_SUBST(ODBCSTATIC)
	AC_SUBST(ODBCUNICODE)
fi
])



AC_DEFUN([FW_CHECK_DB2],
[
if ( test "$ENABLE_DB2" = "yes" )
then

	DB2INCLUDES=""
	DB2LIBS=""
	DB2LIBSPATH=""
	DB2LIBS32=""
	DB2LIBSPATH32=""
	DB2LIBS64=""
	DB2LIBSPATH64=""
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
			FW_CHECK_HEADER_LIB([$DB2PATH/include/sql.h],[DB2INCLUDES=\"-I$DB2PATH/include\"],[$DB2PATH/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"$DB2PATH/lib\"; DB2LIBS=\"-L$DB2PATH/lib -ldb2\"],[$DB2PATH/lib/libdb2.a],[DB2LIBS=\"-L$DB2PATH/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"])
		
		else

			dnl check /opt for 7.2
			FW_CHECK_HEADER_LIB([/opt/IBMdb2/V7.1/include/sql.h],[DB2INCLUDES=\"-I/opt/IBMdb2/V7.1/include\"; DB2VERSION=\"7\"],[/opt/IBMdb2/V7.1/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/IBMdb2/V7.1/lib\"; DB2LIBS=\"-L/opt/IBMdb2/V7.1/lib -ldb2\"; DB2VERSION=\"7\"],[/opt/IBMdb2/V7.1/lib/libdb2.a],[DB2LIBS=\"-L/opt/IBMdb2/V7.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"7\"])
		
			dnl check /usr for 7.2
			FW_CHECK_HEADER_LIB([/usr/IBMdb2/V7.1/include/sql.h],[DB2INCLUDES=\"-I/usr/IBMdb2/V7.1/include\"; DB2VERSION=\"7\"],[/usr/IBMdb2/V7.1/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/usr/IBMdb2/V7.1/lib\"; DB2LIBS=\"-L/usr/IBMdb2/V7.1/lib -ldb2\"; DB2VERSION=\"7\"],[/usr/IBMdb2/V7.1/lib/libdb2.a],[DB2LIBS=\"-L/usr/IBMdb2/V7.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"7\"])
	
			dnl check /opt for 8.1
			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V8.1/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V8.1/include\"; DB2VERSION=\"8\"],[/opt/IBM/db2/V8.1/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/IBM/db2/V8.1/lib\"; DB2LIBS=\"-L/opt/IBM/db2/V8.1/lib -ldb2\"; DB2VERSION=\"8\"],[/opt/IBM/db2/V8.1/lib/libdb2.a],[DB2LIBS=\"-L/opt/IBM/db2/V8.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"8\"])
	
			dnl check /opt for 9.1
			FW_CHECK_HEADER_LIB([/opt/ibm/db2/V9.1/include/sql.h],[DB2INCLUDES=\"-I/opt/ibm/db2/V9.1/include\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.1/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/ibm/db2/V9.1/lib\"; DB2LIBS=\"-L/opt/ibm/db2/V9.1/lib -ldb2\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.1/lib/libdb2.a],[DB2LIBS=\"-L/opt/ibm/db2/V9.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V9.1/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V9.1/include\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.1/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/IBM/db2/V9.1/lib\"; DB2LIBS=\"-L/opt/IBM/db2/V9.1/lib -ldb2\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.1/lib/libdb2.a],[DB2LIBS=\"-L/opt/IBM/db2/V9.1/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/ibm/db2/V9.1/include/sql.h],[DB2INCLUDES=\"-I/opt/ibm/db2/V9.1/include\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.1/lib32/libdb2.$SOSUFFIX],[DB2LIBSPATH32=\"/opt/ibm/db2/V9.1/lib32\"; DB2LIBS32=\"-L/opt/ibm/db2/V9.1/lib32 -ldb2\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.1/lib32/libdb2.a],[DB2LIBS32=\"-L/opt/ibm/db2/V9.1/lib32 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V9.1/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V9.1/include\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.1/lib64/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/IBM/db2/V9.1/lib64\"; DB2LIBS=\"-L/opt/IBM/db2/V9.1/lib64 -ldb2\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.1/lib64/libdb2.a],[DB2LIBS=\"-L/opt/IBM/db2/V9.1/lib64 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
	
			dnl check /opt for 9.5
			FW_CHECK_HEADER_LIB([/opt/ibm/db2/V9.5/include/sql.h],[DB2INCLUDES=\"-I/opt/ibm/db2/V9.5/include\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.5/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/ibm/db2/V9.5/lib\"; DB2LIBS=\"-L/opt/ibm/db2/V9.5/lib -ldb2\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.5/lib/libdb2.a],[DB2LIBS=\"-L/opt/ibm/db2/V9.5/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])

			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V9.5/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V9.5/include\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.5/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/IBM/db2/V9.5/lib\"; DB2LIBS=\"-L/opt/IBM/db2/V9.5/lib -ldb2\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.5/lib/libdb2.a],[DB2LIBS=\"-L/opt/IBM/db2/V9.5/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/ibm/db2/V9.5/include/sql.h],[DB2INCLUDES=\"-I/opt/ibm/db2/V9.5/include\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.5/lib32/libdb2.$SOSUFFIX],[DB2LIBSPATH32=\"/opt/ibm/db2/V9.5/lib32\"; DB2LIBS32=\"-L/opt/ibm/db2/V9.5/lib32 -ldb2\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.5/lib32/libdb2.a],[DB2LIBS32=\"-L/opt/ibm/db2/V9.5/lib32 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V9.5/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V9.5/include\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.5/lib32/libdb2.$SOSUFFIX],[DB2LIBSPATH32=\"/opt/IBM/db2/V9.5/lib32\"; DB2LIBS32=\"-L/opt/IBM/db2/V9.5/lib32 -ldb2\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.5/lib32/libdb2.a],[DB2LIBS32=\"-L/opt/IBM/db2/V9.5/lib32 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/ibm/db2/V9.5/include/sql.h],[DB2INCLUDES=\"-I/opt/ibm/db2/V9.5/include\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.5/lib64/libdb2.$SOSUFFIX],[DB2LIBSPATH64=\"/opt/ibm/db2/V9.5/lib64\"; DB2LIBS64=\"-L/opt/ibm/db2/V9.5/lib64 -ldb2\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.5/lib64/libdb2.a],[DB2LIBS64=\"-L/opt/ibm/db2/V9.5/lib64 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V9.5/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V9.5/include\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.5/lib64/libdb2.$SOSUFFIX],[DB2LIBSPATH64=\"/opt/IBM/db2/V9.5/lib64\"; DB2LIBS64=\"-L/opt/IBM/db2/V9.5/lib64 -ldb2\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.5/lib64/libdb2.a],[DB2LIBS64=\"-L/opt/IBM/db2/V9.5/lib64 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
	
			dnl check /opt for 9.7
			FW_CHECK_HEADER_LIB([/opt/ibm/db2/V9.7/include/sql.h],[DB2INCLUDES=\"-I/opt/ibm/db2/V9.7/include\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.7/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/ibm/db2/V9.7/lib\"; DB2LIBS=\"-L/opt/ibm/db2/V9.7/lib -ldb2\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.7/lib/libdb2.a],[DB2LIBS=\"-L/opt/ibm/db2/V9.7/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V9.7/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V9.7/include\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.7/lib/libdb2.$SOSUFFIX],[DB2LIBSPATH=\"/opt/IBM/db2/V9.7/lib\"; DB2LIBS=\"-L/opt/IBM/db2/V9.7/lib -ldb2\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.7/lib/libdb2.a],[DB2LIBS=\"-L/opt/IBM/db2/V9.7/lib -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/ibm/db2/V9.7/include/sql.h],[DB2INCLUDES=\"-I/opt/ibm/db2/V9.7/include\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.7/lib32/libdb2.$SOSUFFIX],[DB2LIBSPATH32=\"/opt/ibm/db2/V9.7/lib32\"; DB2LIBS32=\"-L/opt/ibm/db2/V9.7/lib32 -ldb2\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.7/lib32/libdb2.a],[DB2LIBS32=\"-L/opt/ibm/db2/V9.7/lib32 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V9.7/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V9.7/include\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.7/lib32/libdb2.$SOSUFFIX],[DB2LIBSPATH32=\"/opt/IBM/db2/V9.7/lib32\"; DB2LIBS32=\"-L/opt/IBM/db2/V9.7/lib32 -ldb2\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.7/lib32/libdb2.a],[DB2LIBS32=\"-L/opt/IBM/db2/V9.7/lib32 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/ibm/db2/V9.7/include/sql.h],[DB2INCLUDES=\"-I/opt/ibm/db2/V9.7/include\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.7/lib64/libdb2.$SOSUFFIX],[DB2LIBSPATH64=\"/opt/ibm/db2/V9.7/lib64\"; DB2LIBS64=\"-L/opt/ibm/db2/V9.7/lib64 -ldb2\"; DB2VERSION=\"9\"],[/opt/ibm/db2/V9.7/lib64/libdb2.a],[DB2LIBS64=\"-L/opt/ibm/db2/V9.7/lib64 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
			FW_CHECK_HEADER_LIB([/opt/IBM/db2/V9.7/include/sql.h],[DB2INCLUDES=\"-I/opt/IBM/db2/V9.7/include\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.7/lib64/libdb2.$SOSUFFIX],[DB2LIBSPATH64=\"/opt/IBM/db2/V9.7/lib64\"; DB2LIBS64=\"-L/opt/IBM/db2/V9.7/lib64 -ldb2\"; DB2VERSION=\"9\"],[/opt/IBM/db2/V9.7/lib64/libdb2.a],[DB2LIBS64=\"-L/opt/IBM/db2/V9.7/lib64 -ldb2\"; DB2STATIC=\"$STATICFLAG\"; DB2VERSION=\"9\"])
		fi

		dnl determine if we need to use the 64 or 32 bit libs
		if ( test -n "$DB2LIBS64" )
		then
			AC_MSG_CHECKING(for 64-bit build)
			FW_TRY_LINK([#include <sqlcli1.h>],[SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,NULL);],[$DB2INCLUDES],[$DB2LIBS64],[$DB2LIBSPATH64],[AC_MSG_RESULT(yes); DB2LIBS="$DB2LIBS64"; DB2LIBSPATH="$DB2LIBSPATH64"],[AC_MSG_RESULT(no)])
		fi
		if ( test -n "$DB2LIBS32" )
		then
			AC_MSG_CHECKING(for 32-bit build)
			FW_TRY_LINK([#include <sqlcli1.h>],[SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,NULL);],[$DB2INCLUDES],[$DB2LIBS32],[$DB2LIBSPATH32],[AC_MSG_RESULT(yes); DB2LIBS="$DB2LIBS32"; DB2LIBSPATH="$DB2LIBSPATH32"],[AC_MSG_RESULT(no)])
		fi
		if ( test -n "$DB2LIBS" )
		then
			AC_MSG_CHECKING(for build with $DB2LIBS)
			FW_TRY_LINK([#include <sqlcli1.h>],[SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,NULL);],[$DB2INCLUDES],[$DB2LIBS],[$DB2LIBSPATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); DB2INCLUDES=""; DB2LIBS=""; DB2LIBSPATH=""])
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



AC_DEFUN([FW_CHECK_FIREBIRD],
[
if ( test "$ENABLE_FIREBIRD" = "yes" )
then

	FIREBIRDINCLUDES=""
	FIREBIRDLIBS=""
	FIREBIRDLIBSPATH=""
	FIREBIRDSTATIC=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling"
		if ( test -n "$FIREBIRDPATH" )
		then
			FIREBIRDINCLUDES="-I$FIREBIRDPATH/include"
			FIREBIRDLIBS="-L$FIREBIRDPATH/lib -lgds"
			FIREBIRDLIBSPATH="$FIREBIRDPATH/lib"
		fi

	else

		STATICFLAG=""
		if ( test -n "$STATICLINK" )
		then
			STATICFLAG="-static"
		fi
		
		FW_CHECK_HEADERS_AND_LIBS([$FIREBIRDPATH],[interbase],[ibase.h],[gds],[$STATICFLAG],[$RPATHFLAG],[FIREBIRDINCLUDES],[FIREBIRDLIBS],[FIREBIRDLIBSPATH],[FIREBIRDSQLSTATIC])
		FW_CHECK_HEADERS_AND_LIBS([$FIREBIRDPATH],[firebird],[ibase.h],[gds],[$STATICFLAG],[$RPATHFLAG],[FIREBIRDINCLUDES],[FIREBIRDLIBS],[FIREBIRDLIBSPATH],[FIREBIRDSQLSTATIC])
		FW_CHECK_HEADERS_AND_LIBS([$FIREBIRDPATH],[firebird],[ibase.h],[fbclient],[$STATICFLAG],[$RPATHFLAG],[FIREBIRDINCLUDES],[FIREBIRDLIBS],[FIREBIRDLIBSPATH],[FIREBIRDSQLSTATIC])
		if ( test -n "$FIREBIRDLIBS" )
		then
			FIREBIRDLIBS="$FIREBIRDLIBS -lcrypt"
		fi

		if ( test -z "$FIREBIRDLIBS" )
		then
			FW_CHECK_HEADER_LIB([/Library/Frameworks/Firebird.framework/Versions/Current/Headers/ibase.h],[FIREBIRDINCLUDES=\"-I/Library/Frameworks/Firebird.framework/Versions/Current/Headers\"],[/Library/Frameworks/Firebird.framework/Versions/Current/Firebird],[FIREBIRDLIBS=\"-framework Firebird\"],[],[])
		fi
		
		LINKFAIL=""
		if ( test -n "$DLLIB" -a -n "$FIREBIRDSTATIC" -a -n "$FIREBIRDLIBS" )
		then
			AC_MSG_CHECKING(if Firebird can be statically linked without $DLLIB)
			FW_TRY_LINK([#include <ibase.h>
	#include <stdlib.h>],[isc_db_handle db=0; isc_attach_database(NULL,0,"",&db,0,NULL);],[$FIREBIRDSTATIC $FIREBIRDINCLUDES],[$FIREBIRDLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Firebird can be statically linked with $DLLIB)
				FW_TRY_LINK([#include <ibase.h>
#include <stdlib.h>],[isc_db_handle db=0; isc_attach_database(NULL,0,"",&db,0,NULL);],[$FIREBIRDSTATIC $FIREBIRDINCLUDES],[$FIREBIRDLIBS $SOCKETLIBS $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); FIREBIRDLIBS="$FIREBIRDLIBS $DLLIB"; LINKFAIL="";],[AC_MSG_RESULT(no); FIREBIRDSTATIC=""])
			fi
		fi
		
		if ( test -n "$DLLIB" -a -z "$FIREBIRDSTATIC" -a -n "$FIREBIRDLIBS" )
		then
			AC_MSG_CHECKING(if Firebird can be dynamically linked without $DLLIB)
			FW_TRY_LINK([#include <ibase.h>
#include <stdlib.h>],[isc_db_handle db=0; isc_attach_database(NULL,0,"",&db,0,NULL);],[$FIREBIRDINCLUDES],[$FIREBIRDLIBS $SOCKETLIBS],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); LINKFAIL="yes"])
			if ( test -n "$LINKFAIL" -a -n "$DLLIB" )
			then
				AC_MSG_CHECKING(if Firebird can be dynamically linked with $DLLIB)
				FW_TRY_LINK([#include <ibase.h>
#include <stdlib.h>],[isc_db_handle db=0; isc_attach_database(NULL,0,"",&db,0,NULL);],[$FIREBIRDINCLUDES],[$FIREBIRDLIBS $SOCKETLIBS $DLLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); FIREBIRDLIBS="$FIREBIRDLIBS $DLLIB"; LINKFAIL=""],[AC_MSG_RESULT(no)])
			fi
		fi
		
		if ( test -n "$LINKFAIL" )
		then
			AC_MSG_WARN(No Firebird link configuration could be found.)
			FIREBIRDINCLUDES=""
			FIREBIRDLIBS=""
			FIREBIRDSTATIC=""
		fi
		
		if ( test -z "$FIREBIRDLIBS" )
		then
			AC_MSG_WARN(Firebird support will not be built.)
		fi
	fi

	FW_INCLUDES(firebird,[$FIREBIRDINCLUDES])
	FW_LIBS(firebird,[$FIREBIRDLIBS])

	AC_SUBST(FIREBIRDINCLUDES)
	AC_SUBST(FIREBIRDLIBS)
	AC_SUBST(FIREBIRDLIBSPATH)
	AC_SUBST(FIREBIRDSTATIC)
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

			FW_CHECK_HEADERS_AND_LIBS([$MDBTOOLSPATH],[mdb],[mdbsql.h],[mdbsql],[$STATICFLAG],[$RPATHFLAG],[MDBSQLINCLUDES],[MDBSQLLIBS],[MDBSQLLIBSPATH],[MDBSQLSTATIC])
			FW_CHECK_HEADERS_AND_LIBS([$MDBTOOLSPATH],[mdb],[mdbtools.h],[mdb],[$STATICFLAG],[$RPATHFLAG],[MDBINCLUDES],[MDBLIBS],[MDBTOOLSLIBSPATH],[MDBTOOLSSTATIC])

			if ( test -n "$MDBSQLINCLUDES" -o -n "$MDBSQLLIBS" -o -n "$MDBINCLUDES" -o -n "$MDBLIBS" )
			then
				MDBTOOLSINCLUDES="$MDBINCLUDES $MDBSQLINCLUDES $GLIBINCLUDES"
				MDBTOOLSLIBS="$MDBSQLLIBS $MDBLIBS $GLIBLIBS"
				AC_MSG_CHECKING(if MDB Tools has mdb_run_query)
				FW_TRY_LINK([extern "C" {
#include <mdbsql.h>
}
#include <stdlib.h>],[mdb_run_query(NULL,NULL);],[$MDBTOOLSINCLUDES],[$MDBTOOLSLIBS $SOCKETLIBS $DLLIB -lm],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MDB_RUN_QUERY,1,Some versions of mdbtools define mdb_run_query)],[AC_MSG_RESULT(no)])
				AC_MSG_CHECKING(if MDB Tools has mdb_sql_run_query)
				FW_TRY_LINK([extern "C" {
#include <mdbsql.h>
}
#include <stdlib.h>],[mdb_sql_run_query(NULL,NULL);],[$MDBTOOLSINCLUDES],[$MDBTOOLSLIBS $SOCKETLIBS $DLLIB -lm],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MDB_SQL_RUN_QUERY,1,Some versions of mdbtools define mdb_sql_run_query)],[AC_MSG_RESULT(no)])
				AC_MSG_CHECKING(if MDB Tools has mdb_sql_fetch_row)
				FW_TRY_LINK([extern "C" {
#include <mdbsql.h>
}
#include <stdlib.h>],[mdb_sql_fetch_row(NULL,NULL);],[$MDBTOOLSINCLUDES],[$MDBTOOLSLIBS $SOCKETLIBS $DLLIB -lm],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MDB_SQL_FETCH_ROW,1,Some versions of mdbtools define mdb_sql_fetch_row)],[AC_MSG_RESULT(no)])
				AC_MSG_CHECKING(if mdb_col_to_string has 5 parameters)
				FW_TRY_LINK([extern "C" {
#include <mdbsql.h>
}
#include <stdlib.h>],[mdb_col_to_string(NULL,NULL,0,0,0);],[$MDBTOOLSINCLUDES],[$MDBTOOLSLIBS $SOCKETLIBS $DLLIB -lm],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MDB_COL_TO_STRING_5_PARAM,1,Some versions of mdbtools have 5 param mdb_col_to_string)],[AC_MSG_RESULT(no)])

				AC_MSG_CHECKING(if MDB Tools has mdb_remove_backends)
				FW_TRY_LINK([extern "C" {
#include <mdbtools.h>
}
#include <stdlib.h>],[mdb_remove_backends();],[$MDBTOOLSINCLUDES],[$MDBTOOLSLIBS $SOCKETLIBS $DLLIB -lm],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MDB_REMOVE_BACKENDS,1,Some versions of mdbtools have mdb_remove_backends())],[AC_MSG_RESULT(no)])

				AC_MSG_CHECKING(if MDB Tools has mdb_open with 2 params)
				FW_TRY_LINK([extern "C" {
#include <mdbtools.h>
}
#include <stdlib.h>],[mdb_open(0,MDB_NOFLAGS);],[$MDBTOOLSINCLUDES],[$MDBTOOLSLIBS $SOCKETLIBS $DLLIB -lm],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MDB_OPEN_2_PARAM,1,Some versions of mdbtools have mdb_open() with 2 parameters)],[AC_MSG_RESULT(no)])

				AC_MSG_CHECKING(if MDB Tools has mdb_close)
				FW_TRY_LINK([extern "C" {
#include <mdbtools.h>
}
#include <stdlib.h>],[mdb_close(0);],[$MDBTOOLSINCLUDES],[$MDBTOOLSLIBS $SOCKETLIBS $DLLIB -lm],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_MDB_CLOSE,1,Some versions of mdbtools have mdb_close())],[AC_MSG_RESULT(no)])
			fi
		
		fi
	fi

	if ( test -z "$MDBTOOLSLIBS" )
	then
		AC_MSG_WARN(MDB Tools support will not be built.)
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
	PERLLIB=""
	PERLPREFIX=""

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
				for i in "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/usr/local/perl/bin" "/opt/sfw/bin" "/usr/sfw/bin" "/opt/csw/bin" "/sw/bin"
				do
					if ( test -d "$i" )
					then
						PERL=""
						FW_CHECK_FILE("$i/perl5",[PERL=\"$i/perl5\"])
						FW_CHECK_FILE("$i/perl",[PERL=\"$i/perl\"])
						if ( test -n "$PERL" )
						then
							break
						fi
					fi
				done
			fi
		fi

		dnl for cygwin and mac os x add -lperl
		if ( test -n "$PERL" )
		then
			if ( test -n "$CYGWIN" -o -n "$DARWIN" )
			then
				DIRS=`perl -e 'foreach (@INC) { print("$_\n"); }'`
				for dir in $DIRS
				do
					FW_CHECK_FILE("$dir/CORE/libperl.$SOSUFFIX",[PERLLIB=\"-L$dir/CORE -lperl\"])
				done
			fi
		fi

		XSUBPP=""
		if ( test -n "$PERL" )
		then
			PERLPREFIXCMD=`$PERL -V:prefix`
			PERLLIBCMD=`$PERL -V:privlibexp`
			PERLPREFIX=`eval "$PERLPREFIXCMD"; echo $prefix`
			PERL_LIB=`eval "$PERLLIBCMD"; echo $privlibexp`
			XSUBPP=$PERL_LIB/ExtUtils/xsubpp
			if ( test -n "`pod2man --help 2>&1 | grep Usage`" )
			then
				POD2MAN="pod2man"
			fi
		fi

		if ( test -r "$XSUBPP" )
		then
			HAVE_PERL="yes"
		else
			AC_MSG_WARN(xsubpp not found)
			AC_MSG_WARN(The Perl API will not be built.)
		fi
	fi

	AC_SUBST(HAVE_PERL)
	AC_SUBST(PERL)
	AC_SUBST(PERLLIB)
	AC_SUBST(PERLPREFIX)
	AC_SUBST(POD2MAN)
fi
])



AC_DEFUN([FW_CHECK_PYTHON],
[
if ( test "$ENABLE_PYTHON" = "yes" )
then

	OVERRIDEPYTHONDIR="$PYTHONDIR"

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
		
			dnl for i in "3.9" "3.8" "3.7" "3.6" "3.5" "3.4" "3.2" "3.1" "3.0" "2.9" "2.8" "2.7" "2.6" "2.5" "2.4" "2.3" "2.2" "2.1"
			for i in "2.9" "2.8" "2.7" "2.6" "2.5" "2.4" "2.3" "2.2" "2.1"
			do
				if ( test -d "$PYTHONPATH/include/python$i" -a -d "$PYTHONPATH/lib64/python$i/config" )
				then
					PYTHONINCLUDES="-I$PYTHONPATH/include/python$i"
					PYTHONDIR="$PYTHONPATH/lib64/python$i"
				else
					if ( test -d "$PYTHONPATH/include/python$i" -a -d "$PYTHONPATH/lib/python$i/config" )
					then
						PYTHONINCLUDES="-I$PYTHONPATH/include/python$i"
						PYTHONDIR="$PYTHONPATH/lib/python$i"
					fi
				fi
			
				if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" )
				then
					PYTHONVERSION=`echo $i | sed -e "s|\.||"`
					break
				fi
			done

		else
		
			dnl for j in "3.9" "3.8" "3.7" "3.6" "3.5" "3.4" "3.2" "3.1" "3.0" "2.9" "2.8" "2.7" "2.6" "2.5" "2.4" "2.3" "2.2" "2.1"
			for j in "2.9" "2.8" "2.7" "2.6" "2.5" "2.4" "2.3" "2.2" "2.1"
			do
				for i in "/usr/include/python$j" "/usr/local/include/python$j" "/usr/pkg/include/python$j" "/usr/local/python$j/include/python$j" "/opt/sfw/include/python$j" "/usr/sfw/include/python$j" "/opt/csw/include/python$j" "/sw/include/python$j" "/System/Library/Frameworks/Python.framework/Versions/Current/include/python$j"
				do
					PYTHONINCLUDES=""
					for k in "mu" "m" "u" ""
					do
						if ( test -d "$i$k" )
						then
							PYTHONINCLUDES="-I$i$k"
							if ( test -n "$PYTHONINCLUDES" )
							then
								PYTHONVERSION=`echo $j | sed -e "s|\.||"`
								break
							fi
						fi
					done
					if ( test -n "$PYTHONINCLUDES" )
					then
						break
					fi
				done

				for i in "/usr/lib64/python$j" "/usr/lib/python$j" "/usr/local/lib64/python$j" "/usr/local/lib/python$j" "/usr/pkg/lib/python$j" "/usr/local/python$j/lib64/python$j" "/usr/local/python$j/lib/python$j" "/opt/sfw/lib/python$j" "/usr/sfw/lib/python$j" "/sfw/lib/python$j" "/opt/csw/lib/python$j" "/sw/lib/python$j" "/System/Library/Frameworks/Python.framework/Versions/Current/lib/python$j"
				do

					PYTHONDIR=""
					for k in "config" "config-$j" "config-${j}mu" "config-${j}m" "config-${j}u"
					do

						if ( test -d "$i/$k" )
						then
							dnl for cygwin and mac os x
							dnl add -lpython
							if ( test -n "$CYGWIN" -a -r "$i/$k/libpython$j.dll.a" )
							then
								PYTHONDIR="$i"
								PYTHONLIB="-L$PYTHONDIR/$k -lpython$j"
							elif ( test -n "$DARWIN" )
							then
								PYTHONDIR="$i"
								PYTHONLIB="-lpython$j"
							else
								PYTHONDIR="$i"
							fi
							if ( test -n "$PYTHONDIR" )
							then
								break
							fi
						fi
					done
					if ( test -n "$PYTHONDIR" )
					then
						break
					fi
				done

				if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" )
				then
					dnl override PYTHONDIR on osx in some cases
					if ( test -d "/Library/Python/$j/site-packages" )
					then
						PYTHONDIR="/Library/Python/$j"
					fi
					break
				fi
			done

		fi

		if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" )
		then
			AC_MSG_CHECKING(for Python.h)
			FW_TRY_COMPILE([#include <Python.h>],[PyArg_ParseTuple(NULL,NULL,NULL,NULL,NULL);],[$PYTHONINCLUDES],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no); PYTHONINCLUDES=""; PYTHONDIR=""])
		fi
		
		if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" )
		then
			HAVE_PYTHON="yes"
		else
			AC_MSG_WARN(The Python API will not be built.)
		fi
	fi

	FW_INCLUDES(python,[$PYTHONINCLUDES])

	if ( test -n "$OVERRIDEPYTHONDIR" )
	then
		PYTHONDIR="$OVERRIDEPYTHONDIR"
	fi

	AC_SUBST(HAVE_PYTHON)
	AC_SUBST(PYTHONINCLUDES)
	AC_SUBST(PYTHONDIR)
	AC_SUBST(PYTHONLIB)
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
					for i in "/usr/local/www" "/usr/share" "/usr/local" "/usr/local/lib" "/usr/local/lib64" "/usr" "/usr/lib" "/usr/lib64" "/opt" "/opt/csw" "/sw" "/usr/pkg" "/usr/pkg/share" "/usr/pkg/lib"
					do
						for j in "zope" "Zope" "zope2" "Zope2" "zope3" "Zope3"
						do
							FW_CHECK_FILE("$i/$j/lib/python/Products/__init__.py",[HAVE_ZOPE=\"yes\"; ZOPEDIR=\"$i/$j/lib/python/Products\"])
							FW_CHECK_FILE("$i/$j/lib64/python/Products/__init__.py",[HAVE_ZOPE=\"yes\"; ZOPEDIR=\"$i/$j/lib64/python/Products\"])
							if ( test -n "$ZOPEDIR" )
							then
								break
							fi
							for k in "2.2" "2.3" "2.4" "2.5" "2.6" "2.7" "2.8" "2.9" "3.0" "3.1" "3.2" "3.3" "3.4"
							do
								FW_CHECK_FILE("$i/$j-$k/lib/python/Products/__init__.py",[HAVE_ZOPE=\"yes\"; ZOPEDIR=\"$i/$j-$k/lib/python/Products\"])
								FW_CHECK_FILE("$i/$j-$k/lib64/python/Products/__init__.py",[HAVE_ZOPE=\"yes\"; ZOPEDIR=\"$i/$j-$k/lib64/python/Products\"])
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
	RUBYLIB=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else

		found="no"

		for major in "" "1" "2"
		do

			for minor in "" "9" "8" "7" "6" "5" "4" "3" "2" "1" "0"
			do

				for patchlevel in "" "0" "1" "2" "3" "4" "5" "6" "7" "8" "9"
				do

					for separator in "" "."
					do

			ruby="ruby$major$separator$minor"
			if ( test -n "$patchlevel" )
			then
				ruby="ruby$major$separator$minor$separator$patchlevel"
			fi

			HAVE_RUBY=""
			RUBY=""
			RUBYLIB=""

			if ( test -n "$RUBYPATH" )
			then
				FW_CHECK_FILE("$RUBYPATH/bin/$ruby",[RUBY=\"$RUBYPATH/bin/$ruby\"])
			else
				AC_CHECK_PROG(RUBY,"$ruby","$ruby")
				if ( test -z "$RUBY" )
				then
					for i in "/usr/local/ruby/bin" "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/opt/sfw/bin" "/usr/sfw/bin" "/opt/csw/bin" "/sw/bin"
					do
						FW_CHECK_FILE("$i/$ruby",[RUBY=\"$i/$ruby\"])
						if ( test -n "$RUBY" )
						then
							found="yes"
							break
						fi
					done
				fi
			fi

			if ( test -n "$RUBY" )
			then
				HAVE_RUBY="yes"
				dnl for cygwin and OSX include -lruby
				if ( test -n "$CYGWIN" -o -n "$DARWIN" )
				then
					RUBYLIB="-lruby"
				fi
				found="yes"
				break
			fi
						if ( test -z "$major" -o -z "$minor" -o -z "$patchlevel" )
						then
							break
						fi

					done
					if ( test "$found" = "yes" )
					then
						break
					fi
					if ( test -z "$minor" )
					then
						break
					fi
				done
				if ( test "$found" = "yes" )
				then
						break
				fi
				if ( test -z "$major" )
				then
					break
				fi
			done
			if ( test "$found" = "yes" )
			then
				break
			fi
		done

		if ( test -n "$HAVE_RUBY" )
		then
			AC_MSG_CHECKING(for ruby.h)
			rm -f conftest.rb
			cat > conftest.rb << END
require "mkmf"
drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/
print "\n"
print "arch = "
print CONFIG@<:@"arch"@:>@
print "\n"
print "ruby_version = "
print Config::CONFIG@<:@"ruby_version"@:>@
print "\n"
print "prefix = "
print CONFIG@<:@"prefix"@:>@.sub(drive, "")
print "\n"
print "exec_prefix = "
print CONFIG@<:@"exec_prefix"@:>@.sub(drive, "")
print "\n"
print "libdir = "
print \$libdir.sub(drive, "")
print "\n"
print "rubylibdir = "
print \$rubylibdir.sub(drive, "")
print "\n"
print "topdir = "
print \$topdir
print "\n"
print "hdrdir = "
print \$hdrdir
print "\n\n"
print "all:\n"
print "	echo \$(hdrdir)\n"
END

			HAVE_RUBY_H=""
			for dir in `eval $RUBY conftest.rb | sed -e "s|-x.* | |g" -e "s|-belf||g" -e "s|-mtune=.* | |g" | $MAKE -s -f - | grep -v Entering | grep -v Leaving`
			do
				if ( test -r "$dir/ruby.h" )
				then
					HAVE_RUBY_H="yes"
				fi
			done
			rm -f conftest.rb

			dnl if we didn't have ruby.h then we don't have ruby
			if ( test -z "$HAVE_RUBY_H" )
			then
				AC_MSG_RESULT(no)
				HAVE_RUBY=""
			else
				AC_MSG_RESULT(yes)
			fi
		fi
	fi

	if ( test -z "$HAVE_RUBY" )
	then
		AC_MSG_WARN(The Ruby API will not be built.)
	fi

	AC_SUBST(HAVE_RUBY)
	AC_SUBST(RUBY)
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

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else
		
		if ( test -z "$JAVAPATH" )
		then
			for i in `ls -d /usr/java/jdk* /usr/java/j2sdk* /usr/local/jdk* 2> /dev/null` /usr/java /usr/local/java `ls -d /usr/local/openjdk* /usr/pkg/java/openjdk* 2> /dev/null` `ls -d /usr/lib/jvm/java 2> /dev/null` /System/Library/Frameworks/JavaVM.framework/Versions/Current /usr /usr/local
			do
				FW_CHECK_FILE("$i/bin/javac$EXE",[JAVAPATH=\"$i\"])
				if ( test -z "$JAVAPATH" )
				then
					FW_CHECK_FILE("$i/Commands/javac$EXE",[JAVAPATH=\"$i\"])
				fi
				if ( test -n "$JAVAPATH" )
				then
					break
				fi
			done
		fi
		
		if ( test -n "$JAVAPATH" )
		then
			FW_CHECK_FILE("$JAVAPATH/bin/javac$EXE",[JAVAC=\"$JAVAPATH/bin/javac$EXE\"])
			FW_CHECK_FILE("$JAVAPATH/Commands/javac$EXE",[JAVAC=\"$JAVAPATH/Commands/javac$EXE\"])
			FW_CHECK_FILE("$JAVAPATH/bin/jar$EXE",[JAR=\"$JAVAPATH/bin/jar$EXE\"])
			FW_CHECK_FILE("$JAVAPATH/Commands/jar$EXE",[JAR=\"$JAVAPATH/Commands/jar$EXE\"])
			FW_CHECK_FILE("$JAVAPATH/include/jni.h",[JAVAINCLUDES=\"-I$JAVAPATH/include\"])
			FW_CHECK_FILE("$JAVAPATH/Headers/jni.h",[JAVAINCLUDES=\"-I$JAVAPATH/Headers\"])
			if ( test -n "$JAVAINCLUDES" )
			then
				for i in `ls -d $JAVAPATH/include/* $JAVAPATH/Headers/* 2> /dev/null`
				do
					if ( test -d "$i" )
					then
						JAVAINCLUDES="$JAVAINCLUDES -I$i"
					fi
				done
			fi
		fi

		if ( test -n "$JAVAC" -a -n "$JAR" )
		then
			HAVE_JAVA="yes"
		fi
	fi

	if ( test -n "$HAVE_JAVA" )
	then
		AC_MSG_CHECKING(whether $JAVAC works)
		cat << EOF > conftest.java
public class conftest {
	public static void main(String[] args) {
		System.out.println("hello world");
	}
}
EOF
		$JAVAC conftest.java 2> /dev/null
		if ( test -r "conftest.class" )
		then
			AC_MSG_RESULT(yes)
			rm -f conftest.class
		else
			AC_MSG_RESULT(no)
			HAVE_JAVA=""
		fi
	fi

	if ( test -n "$HAVE_JAVA" )
	then
		FW_TRY_COMPILE([#ifdef __CYGWIN__
#include <windows.h>
#endif
#include <jni.h>],[JNIEnv *env; jobjectArray a=(jobjectArray)env->NewObjectArray(0,NULL,NULL);],[$JAVAINCLUDES],[AC_DEFINE(CAST_NEW_OBJECT_ARRAY,1,On some platforms NewObjectArray requires a cast)],[])
	else
		AC_MSG_WARN(The Java API will not be built.)
	fi

	FW_INCLUDES(java,[$JAVAINCLUDES])
		
	AC_SUBST(HAVE_JAVA)
	AC_SUBST(JAVAC)
	AC_SUBST(JAR)
	AC_SUBST(JAVAINCLUDES)
fi
])


AC_DEFUN([FW_CHECK_PHP],
[
if ( test "$ENABLE_PHP" = "yes" )
then

	OVERRIDEPHPEXTDIR="$PHPEXTDIR"

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
			FW_CHECK_FILE("$PHPPATH/bin/php-config",[PHPCONFIG=\"$PHPPATH/bin/php-config\"])
		else
			AC_CHECK_PROG(PHPCONFIG,"php-config","php-config")
			if ( test -z "$PHPCONFIG" )
			then
				for i in "/usr/local/php/bin" "/usr/bin" "/usr/local/bin" "/usr/pkg/bin" "/opt/sfw/bin" "/usr/sfw/bin" "/opt/csw/bin" "/opt/csw/php4/bin" "/opt/csw/php5/bin" "/sw/bin"
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
			PHPVERSION=`$PHPCONFIG --version`
			PHPMAJORVERSION=`echo "$PHPVERSION" | cut -d'.' -f1`
		else
			HAVE_PHP=""
			AC_MSG_WARN(The PHP API will not be built.)
		fi

		dnl on os x add -lphp - this isn't necessary any more
		dnl and I can't figure out what platforms it was required on
		dnl any more either
		dnl if ( test -n "$PHPCONFIG" -a -n "$DARWIN" )
		dnl then
			dnl PHPLIB="-lphp"
		dnl fi
	fi

	FW_INCLUDES(php,[$PHPINCLUDES])

	if ( test -n "$OVERRIDEPHPEXTDIR" )
	then
		PHPEXTDIR="$OVERRIDEPHPEXTDIR"
	fi

	AC_SUBST(HAVE_PHP)
	AC_SUBST(PHPINCLUDES)
	AC_SUBST(PHPEXTDIR)
	AC_SUBST(PHPVERSION)
	AC_SUBST(PHPMAJORVERSION)
	AC_SUBST(PHPLIB)
fi
])

AC_DEFUN([FW_CHECK_PHP_PEAR_DB],
[

	if ( test -z "$PHPPEARDBDIR" )
	then

		for i in "/usr" "/usr/local" "/usr/pkg" "/opt/sfw" "/usr/sfw" "/opt/csw" "/sw"
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


AC_DEFUN([FW_CHECK_ERLANG],
[
if ( test "$ENABLE_ERLANG" = "yes" )
then

	HAVE_ERLANG=""
	ERLANGINCLUDES=""
	ERLANGLIBS=""

	if ( test "$cross_compiling" = "yes" )
	then

		dnl cross compiling ...
		echo "cross compiling..."

	else

		AC_ERLANG_PATH_ERLC
		AC_ERLANG_PATH_ERL
		if ( test -n "$ERLC" -a -n "$ERL" )
		then

			dnl get ERLANG_ROOT_DIR
			if ( test -z "$ERLANG_ROOT_DIR" )
			then
				rm -f conftest.erl
				cat > conftest.erl << END
-module(conftest).
-export([[start/0]]).
start() ->
	RootDir = code:root_dir(),
	file:write_file("conftest.out", RootDir),
	ReturnValue = 0,
	halt(ReturnValue)
.
END
				cat > conftest << END
#!/bin/sh
$ERLC $ERLCFLAGS -b beam conftest.erl
$ERL -run conftest start -run init stop -noshell
END
				chmod 755 conftest
				./conftest
				ERLANG_ROOT_DIR=`cat conftest.out`
				rm -f conftest.out
				rm -f conftest.erl
				rm -f conftest.beam
			fi

			dnl get ERLANG_LIB_DIR
			if ( test -z "$ERLANG_LIB_DIR" )
			then
				rm -f conftest.erl
				cat > conftest.erl << END
-module(conftest).
-export([[start/0]]).
start() ->
	LibDir = code:lib_dir(),
	file:write_file("conftest.out", LibDir),
	ReturnValue = 0,
	halt(ReturnValue)
.
END
				cat > conftest << END
#!/bin/sh
$ERLC $ERLCFLAGS -b beam conftest.erl
$ERL -run conftest start -run init stop -noshell
END
				chmod 755 conftest
				./conftest
				ERLANG_LIB_DIR=`cat conftest.out`
				rm -f conftest.out
				rm -f conftest.erl
				rm -f conftest.beam
			fi

			if ( test -z "$ERLANG_INSTALL_LIB_DIR" )
			then
				ERLANG_INSTALL_LIB_DIR='%{libdir}/erlang/lib'
			fi
			


			ERLANG_INCLUDE_DIR=`ls -d $ERLANG_LIB_DIR/erl_interface*/include 2> /dev/null`
			ERLANG_LIB_DIR=`ls -d $ERLANG_LIB_DIR/erl_interface*/lib 2> /dev/null`


			if ( test -n "$ERLANG_INCLUDE_DIR" -a -n "$ERLANG_LIB_DIR" )
			then

				if ( test "$ERLANG_INCLUDE_DIR" != "/usr" )
				then
					ERLANGINCLUDES="-I$ERLANG_INCLUDE_DIR"
				fi

				if ( test "$ERLANG_LIB_DIR" != "/usr" )
				then
					ERLANGLIBS="-L$ERLANG_LIB_DIR"
				fi
				ERLANGLIBS="$ERLANGLIBS -lerl_interface -lei"

				HAVE_ERLANG="yes"
			else
				AC_MSG_WARN(The Erlang API will not be built.)
			fi
		else
			AC_MSG_WARN(The Erlang API will not be built.)
		fi
	fi

	FW_INCLUDES(erlang,[$ERLANGINCLUDES])
	FW_LIBS(erlang,[$ERLANGLIBS])

	AC_SUBST(HAVE_ERLANG)
	AC_SUBST(ERLANGINCLUDES)
	AC_SUBST(ERLANGLIBS)
	AC_SUBST(ERLANG_ROOT_DIR)
	AC_SUBST(ERLANG_LIB_DIR)
	AC_SUBST(ERLANG_INSTALL_LIB_DIR)
fi
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
		for i in "/sw/include" "/opt/csw/include" "/usr/sfw/include" "/opt/sfw/include" "/usr/pkg/include" "/usr/local/include" "$prefix/include" "/usr/include" "$TCLINCLUDEPATH"
		do
			for j in "" "/tcl8.0" "/tcl8.1" "/tcl8.2" "/tcl8.3" "/tcl8.4" "/tcl8.5" "/tcl8.6" "/tcl8.7" "/tcl8.8" "/tcl8.9" "/tcl8.10" "/tcl8.11" "/tcl8.12" "/tcl8.13" "/tcl8.14" "/tcl8.15" "/tcl8.16" "/tcl8.17" "/tcl8.18" "/tcl8.19" "/tcl9.0" "/tcl9.1" "/tcl9.2" "/tcl9.3" "/tcl9.4" "/tcl9.5" "/tcl9.6" "/tcl9.7" "/tcl9.8" "/tcl9.9" "/tcl9.10" "/tcl9.11" "/tcl9.12" "/tcl9.13" "/tcl9.14" "/tcl9.15" "/tcl9.16" "/tcl9.17" "/tcl9.19"
			do
				FW_CHECK_FILE($i$j/tcl.h,[TCLINCLUDE=\"-I$i/$j\"])
			done
		done
		dnl first look for a dynamic libtcl
		if ( test -n "$TCLINCLUDE" )
		then
			for i in "/sw/lib" "/opt/csw/lib" "/usr/sfw/lib" "/opt/sfw/lib" "/usr/pkg/lib" "/usr/local/lib" "/usr/local/lib64" "$prefix/lib" "$prefix/lib64" "/usr/lib" "/usr/lib64" "$TCLLIBSPATH"
			do
				for j in "" "8.0" "8.1" "8.2" "8.3" "8.4" "8.5" "8.6" "8.7" "8.8" "8.9" "8.10" "8.11" "8.12" "8.13" "8.14" "8.15" "8.16" "8.17" "8.18" "8.19" "9.0" "9.1" "9.2" "9.3" "9.4" "9.5" "9.6" "9.7" "9.8" "9.9" "9.10" "9.11" "9.12" "9.13" "9.14" "9.15" "9.16" "9.17" "9.18" "9.19" "80" "81" "82" "83" "84" "85" "86" "87" "88" "89" "810" "811" "812" "813" "814" "815" "816" "817" "818" "819" "90" "91" "92" "93" "94" "95" "96" "97" "98" "99" "910" "911" "912" "913" "914" "915" "916" "917" "918" "919"
				do
					FW_CHECK_FILE($i/libtcl$j.$SOSUFFIX,[TCLLIB=\"$i/libtcl$j.$SOSUFFIX\"; TCLLIBSPATH=\"$i\"])
					if ( test -z "$TCLLIB" )
					then
						TESTLIB=`ls $i/libtcl$j.$SOSUFFIX.? 2>/dev/null`
						if ( test -n "$TESTLIB" )
						then
							TCLLIB="$TESTLIB"
							TCLLIBSPATH="$i"
						fi
					fi
					if ( test -z "$TCLLIB" )
					then
						TESTLIB=`ls $i/libtcl$j.$SOSUFFIX.?.? 2>/dev/null`
						if ( test -n "$TESTLIB" )
						then
							TCLLIB="$TESTLIB"
							TCLLIBSPATH="$i"
						fi
					fi
					if ( test -z "$TCLLIB" )
					then
						TESTLIB=`ls $i/libtcl$j.$SOSUFFIX.?.?.? 2>/dev/null`
						if ( test -n "$TESTLIB" )
						then
							TCLLIB="$TESTLIB"
							TCLLIBSPATH="$i"
						fi
					fi
				done
			done
		fi

		dnl if we didn't find it, look for a dynamic libtclstub
		if ( test -n "$TCLINCLUDE " -a -z "$TCLLIB" )
		then
			for i in "/sw/lib" "/opt/csw/lib" "/usr/sfw/lib" "/opt/sfw/lib" "/usr/pkg/lib" "/usr/local/lib" "/usr/local/lib64" "$prefix/lib" "/$prefix/lib64" "/usr/lib" "/usr/lib64" "$TCLLIBSPATH"
			do
				for j in "" "8.0" "8.1" "8.2" "8.3" "8.4" "8.5" "80" "81" "82" "83" "84" "85"
				do
					FW_CHECK_FILE($i/libtclstub$j.$SOSUFFIX,[TCLLIB=\"$i/libtclstub$j.$SOSUFFIX\"; TCLLIBSPATH=\"$i\"; TCLINCLUDE=\"-DUSE_TCL_STUBS $TCLINCLUDE\"])
				done
			done
		fi

		dnl translate /path/libtcl.*.$SOSUFFIX to -Lpath -ltcl.*
		if ( test -n "$TCLLIB" )
		then
			LIBPATH=`dirname $TCLLIB`
			LIBITSELF=`basename $TCLLIB | sed -e "s/^lib//" -e "s/\.$SOSUFFIX.*//"`
			TCLLIB="-L$LIBPATH -l$LIBITSELF"
		fi

		if ( test -z "$TCLLIB" )
		then
			AC_MSG_WARN("The TCL API will not be installed.")
		else
			HAVE_TCL="yes"
			AC_MSG_CHECKING(for Tcl_GetString)
			FW_TRY_LINK([#include <tcl.h>],[Tcl_GetString(NULL);],[$TCLINCLUDE $PTHREADINCLUDES],[$TCLLIB $PTHREADLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes) AC_DEFINE_UNQUOTED(HAVE_TCL_GETSTRING,1,Some versions of TCL don't have Tcl_GetString)],[AC_MSG_RESULT(no)])
			AC_MSG_CHECKING(for Tcl_WideInt)
			FW_TRY_LINK([#include <tcl.h>],[Tcl_WideInt row;],[$TCLINCLUDE $PTHREADINCLUDES],[$TCLLIB $PTHREADLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes) AC_DEFINE_UNQUOTED(HAVE_TCL_WIDEINT,1,Some versions of TCL don't have Tcl_WideInt)],[AC_MSG_RESULT(no)])
			AC_MSG_CHECKING(for const char ** support)
			FW_TRY_LINK([#include <tcl.h>],[static const char *options[]={""}; Tcl_GetIndexFromObj(NULL,NULL,(const char **)options,NULL,0,NULL);],[$TCLINCLUDE $PTHREADINCLUDES],[$TCLLIB $PTHREADLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes) AC_DEFINE_UNQUOTED(HAVE_TCL_CONSTCHAR,1,Some versions of TCL don't use const char ** arguments)],[AC_MSG_RESULT(no)])
			AC_MSG_CHECKING(for Tcl_NewStringObj with const char *)
			FW_TRY_LINK([#include <tcl.h>],[const char *a=""; Tcl_NewStringObj(a,0);],[$TCLINCLUDE $PTHREADINCLUDES],[$TCLLIB $PTHREADLIB],[$LD_LIBRARY_PATH],[AC_MSG_RESULT(yes) AC_DEFINE_UNQUOTED(HAVE_TCL_NEWSTRINGOBJ_CONST_CHAR,1,Some versions of TCL don't use const char ** arguments)],[AC_MSG_RESULT(no)])
		fi
	fi

	FW_INCLUDES(tcl,[$TCLINCLUDE])
	FW_LIBS(tcl,[$TCLLIB])

	AC_SUBST(HAVE_TCL)
	AC_SUBST(TCLINCLUDE)
	AC_SUBST(TCLLIB)
	AC_SUBST(TCLLIBSPATH)
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

	AC_MSG_CHECKING(for socket libraries)

	AC_LANG_SAVE
	AC_LANG(C)
	SOCKETLIBS=""
	DONE=""
	for i in "" "-lnsl" "-lsocket" "-lsocket -lnsl" "-lxnet" "-lwsock32 -lnetapi32" "-lnetwork"
	do
		FW_TRY_LINK([#ifdef HAVE_STDLIB_H
	#include <stdlib.h>
#endif
#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
	#include <sys/socket.h>
#endif
#ifdef __MINGW32__
	#include <winsock2.h>
#endif],[connect(0,NULL,0);
listen(0,0);
bind(0,NULL,0);
accept(0,NULL,0);
send(0,NULL,0,0);
sendto(0,NULL,0,0,NULL,0);
#ifndef __MINGW32__
	sendmsg(0,NULL,0);
#endif
gethostbyname(NULL);],[$CPPFLAGS],[$i],[],[SOCKETLIBS="$i"; DONE="yes"],[])
		if ( test -n "$DONE" )
		then
			break
		fi
	done
	AC_LANG_RESTORE

	if ( test -z "$DONE" )
	then
		AC_MSG_ERROR(no combination of networking libraries was found.)
	fi

	if ( test -z "$SOCKETLIBS" )
	then
		AC_MSG_RESULT(none)
	else
		AC_MSG_RESULT($SOCKETLIBS)
	fi

	AC_SUBST(SOCKETLIBS)
])
