SQLR_VERSION = @SQLR_VERSION@
SONAME_VERSION_INFO =

# installation directories
prefix = @prefix@
exec_prefix= @exec_prefix@
includedir = @includedir@
libdir = @libdir@
javadir = @javadir@
libexecdir = @libexecdir@
bindir = @bindir@
localstatedir = @localstatedir@
sysconfdir = @sysconfdir@
mandir = @mandir@
datadir = @datadir@
docdir = $(datadir)\doc\sqlrelay
EXAMPLEDIR = $(datadir)\examples
tmpdir = $(localstatedir)\sqlrelay\tmp
cachedir = $(localstatedir)\sqlrelay\cache
debugdir = $(localstatedir)\sqlrelay\debug
logdir = $(localstatedir)\sqlrelay\log
initscript_prefix = @initscript_prefix@

# command separator
AND = &

# slash
SLASH = \\

# shell
SHELL =

# libtool command
LIBTOOL =

# compile commands
LTCOMPILE = 
CC = cl
CXX = cl
COMPILE = /c
OUT = -out:
BASECPPFLAGS = /nologo @OPTCPPFLAGS@ @DEBUGCPPFLAGS@ @WINVER@ @WIN32WINDOWS@ @WIN32WINNT@ @_USE_32BIT_TIME_T@ @SDKINCLUDES@
EXTRACPPFLAGS =
CXXFLAGS =
CFLAGS =
WERROR =
WNOUNKNOWNPRAGMAS =
WNOMISMATCHEDTAGS =
INC = /I
OBJ = obj
TMPDIRCPPFLAG = /D TMP_DIR=\"$(tmpdir)\"
DEBUGDIRCPPFLAG = /D DEBUG_DIR=\"$(debugdir)\"
LOGDIRCPPFLAG = /D LOG_DIR=\"$(logdir)\"
CONFIGFILECPPFLAG = /D DEFAULT_CONFIG_FILE=\"$(sysconfdir)\\sqlrelay.conf\" /D DEFAULT_CONFIG_DIR=\"$(sysconfdir)\\sqlrelay.conf.d\"
CACHEDIRCPPFLAG = /D CACHE_DIR=\"$(cachedir)\"

# linker flags
LTLINK =
LINK = link
CCLINK = link
AR =
LDFLAGS = /nologo @DEBUGLDFLAGS@ @SDKLIBS@
LINKFLAGS = /dll
MODLINKFLAGS = /dll
INSTALLLIB = installdll
UNINSTALLLIB = uninstalldll
LIBEXT = dll

# install commands
LTINSTALL =
MV = move
CP = cscript /nologo @top_builddir@\cp.vbs
CHMOD = echo
MKINSTALLDIRS = cscript /nologo @top_builddir@\mkinstalldirs.vbs
LTFINISH =

#uninstall/clean commands
LTUNINSTALL =
LTCLEAN =
RM = cscript /nologo @top_builddir@\rm.vbs
RMTREE = cscript /nologo @top_builddir@\rmtree.vbs

# math library
MATHLIB =

# extra libs
EXTRALIBS =

# windows environment
MINGW32 =
CYGWIN =
UWIN =
MICROSOFT =

# libpthread
PTHREADLIB =

# rudiments library
RUDIMENTSPATH =
RUDIMENTSINCLUDES = /I"C:\Program Files\Firstworks\include"
RUDIMENTSLIBS = /LIBPATH:"C:\Program Files\Firstworks\lib" librudiments.lib
RUDIMENTSLIBSPATH =

#iconv
HAVE_ICONV =
ICONVINCLUDES =
ICONVLIBS =

# c++
CPPCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENT_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\api\c++\include /I$(top_builddir)\src\common $(RUDIMENTSINCLUDES)
CPPLIBS = $(RUDIMENTSLIBS)

# c
CUSERPATH =
CCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENTWRAPPER_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\api\c\include /I$(top_builddir)\src\api\c++\include $(RUDIMENTSINCLUDES)
CLIBS = /LIBPATH:$(top_builddir)\src\api\c++\src libsqlrclient.lib $(RUDIMENTSLIBS)
CRPATH =

# perl
HAVE_PERL =
PERL = @PERL@
PERLPREFIX = @PERLPREFIX@
PERLLIB = @PERLLIB@
PERLINC = @PERLINC@
PERLCCFLAGS = @PERLCCFLAGS@ @PERL500@ @PERLREALLYOLD@
PERLOPTIMIZE = @PERLOPTIMIZE@
PERLSITEARCH = @PERLSITEARCH@
PERLSITELIB = @PERLSITELIB@
PERLARCHLIBEXP = @PERLARCHLIBEXP@
PERLINSTALLMAN3DIR = @PERLINSTALLMAN3DIR@
PERLMAN3EXT = @PERLMAN3EXT@
XSUBPP = @XSUBPP@
POD2MAN = @POD2MAN@
OVERRIDEPERLSITEARCH = @OVERRIDEPERLSITEARCH@
OVERRIDEPERLSITELIB = @OVERRIDEPERLSITELIB@
OVERRIDEPERLINSTALLMAN3DIR = @OVERRIDEPERLINSTALLMAN3DIR@
OVERRIDEPERLMAN3EXT = @OVERRIDEPERLMAN3EXT@
#PERLCCFLAGS_LOCAL = $(shell echo "$(PERLCCFLAGS)" | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e 's|-x.*$$||g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g" -e "s|-arch .* ||g" -e "s|-Kalloca ||g")
#PERLOPTIMIZE_LOCAL = $(shell echo "$(PERLOPTIMIZE)" | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e 's|-x.*$$||g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g")
#ifeq ($(OVERRIDEPERLSITEARCH),)
#PERLSITEARCH_LOCAL = $(DESTDIR)$(shell echo "$(PERLSITEARCH)" | sed -e "s|UNKNOWN||g" )
#else
#PERLSITEARCH_LOCAL = $(DESTDIR)$(OVERRIDEPERLSITEARCH)
#endif
#ifeq ($(OVERRIDEPERLSITELIB),)
#PERLSITELIB_LOCAL = $(DESTDIR)$(shell echo "$(PERLSITELIB)" | sed -e "s|UNKNOWN||g" )
#else
#PERLSITELIB_LOCAL = $(DESTDIR)$(OVERRIDEPERLSITELIB)
#endif
#PERLINC_LOCAL = $(DESTDIR)$(shell echo "$(PERLINC)" | sed -e "s|UNKNOWN||g" )
#ifeq ($(OVERRIDEPERLINSTALLMAN3DIR),)
#PERLINSTALLMAN3DIR_LOCAL = $(DESTDIR)$(shell echo "$(PERLINSTALLMAN3DIR)" | sed -e "s|UNKNOWN||g" )
#else
#PERLINSTALLMAN3DIR_LOCAL = $(DESTDIR)$(OVERRIDEPERLINSTALLMAN3DIR)
#endif
#ifeq ($(OVERRIDEPERLMAN3EXT),)
#PERLMAN3EXT_LOCAL = $(shell echo "$(PERLMAN3EXT)" | sed -e "s|UNKNOWN||g" )
#else
#PERLMAN3EXT_LOCAL = $(OVERRIDEPERLMAN3EXT)
#endif
#PERLCPPFLAGS = $(BASECPPFLAGS) $(PERLOPTIMIZE_LOCAL) $(PERLCCFLAGS_LOCAL) /I./ /I$(top_builddir) /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PERLINC_LOCAL)
#PERLCONLIBS = $(PERLLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
#PERLCURLIBS = $(PERLLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
#PERLRPATH =
#ifneq ($(strip $(libdir)),)
#ifeq ($(PERLUSERPATH),yes)
	#PERLRPATH = -R $(libdir)
#endif
#endif


# python
HAVE_PYTHON =
PYTHONINCLUDES = /I@PYTHONPREFIX@\include
PYTHONDIR = @PYTHONPREFIX@\Lib
PYTHONUSERPATH =
PYTHONLIB = /LIBPATH:@PYTHONPREFIX@\libs python27.lib
PYTHONCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
PYTHONLIBS = $(PYTHONLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
PYTHONRPATH =


# ruby
HAVE_RUBY =
RUBY = @RUBY@
RUBYLIB = @RUBYLIB@
OVERRIDERUBYSITEARCHDIR = @OVERRIDERUBYSITEARCHDIR@

#RUBYCFLAGS = $(shell LANG=POSIX $(RUBY) getcflags.rb | sed -e "s|-x.* | |g" -e "s|-belf||g" -e "s|-mtune=.* | |g" | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )

#ifeq ($(OVERRIDERUBYSITEARCHDIR),)
#RUBYSITEARCHDIR = $(shell LANG=POSIX $(RUBY) getsitearchdir.rb | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )
#else
#RUBYSITEARCHDIR = $(OVERRIDERUBYSITEARCHDIR)
#endif

RUBYCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(RUBYCFLAGS) /I./ /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
RUBYLIBS = $(RUBYLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
RUBYRPATH =


# php
HAVE_PHP =
PHPINCLUDES = /I C:\\PHP\\dev\\include\\php /I C:\\PHP\\dev\\include\\php\\main /I C:\\PHP\\dev\\include\\php\\TSRM /I C:\\PHP\\dev\\include\\php\\Zend /I C:\\PHP\\dev\\include\\php\\ext /I C:\\PHP\\dev\\include\\php\\ext\date\lib $(RUDIMENTSINCLUDES)
PHPEXTDIR = C:\\PHP\\ext
PHPUSERPATH =
PHPVERSION =
PHPMAJORVERSION =
PHPLIB = /LIBPATH:C:\\PHP\\dev php5ts.lib
PHPCONFDIR = C:\\Windows
PHPCONFSTYLE = windows
HAVE_PHP_PDO =
PHPCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES) /D COMPILE_DL=1
PHPLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
PHPRPATH =
PHPPDOCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES) /D COMPILE_DL=1
PHPPDOLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
PHPPDORPATH =


# java
HAVE_JAVA =
JAVAC = "@JAVAPREFIX@\bin\javac"
JAR = "@JAVAPREFIX@\bin\jar"
JAVAINCLUDES = /I "@JAVAPREFIX@\include" /I "@JAVAPREFIX@\include\win32"
JAVAUSERPATH =
JAVACPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c\include /I$(top_builddir)\src\api\c++\include $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
JAVALIBS = /LIBPATH:$(top_builddir)\src\api\c++\src libsqlrclient.lib $(RUDIMENTSLIBS) /LIBPATH:"@JAVAPREFIX@\lib" jvm.lib
JAVARPATH =


# tcl
HAVE_TCL =
TCLINCLUDE = /IC:\Tcl\include
TCLLIB = /LIBPATH:C:\Tcl\lib tcl86.lib
TCLLIBSPATH = C:\Tcl\lib
TCLUSERPATH =
TCLCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(TCLINCLUDE) /I$(top_builddir) /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
TCLLIBS = $(TCLLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
TCLRPATH =


# erlang
HAVE_ERLANG =
ERLC = @ERLC@
ERLCFLAGS = @ERLCFLAGS@
ERLANGINCLUDES = @ERLANGINCLUDES@
ERLANGLIB = @ERLANGLIBS@
ERLANG_ROOT_DIR = @ERLANG_ROOT_DIR@
ERLANG_LIB_DIR = @ERLANG_LIB_DIR@
ERLANG_INSTALL_LIB_DIR = @ERLANG_INSTALL_LIB_DIR@
ERLANGUSERPATH = @ERLANGUSERPATH@
ERLANGCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(ERLANGINCLUDES) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c/include /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
ERLANGLIBS = $(ERLANGLIB) /LIBPATH:$(top_builddir)/src/api/c/src /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclientwrapper.lib libsqlrclient.lib $(RUDIMENTSLIBS)
ERLANGRPATH =


# psql
PSQLUSERPATH = @PSQLUSERPATH@


# readline
READLINEINCLUDES = @READLINEINCLUDES@
READLINELIBS = @READLINELIBS@

# libsocket
SOCKETLIBS =


# oracle
ORACLEVERSION = @ORACLEVERSION@
ORACLEINCLUDES = @ORACLEINCLUDES@
ORACLELIBS = @ORACLELIBS@
ORACLELIBSPATH = @ORACLELIBSPATH@
ORACLEUSERPATH = @ORACLEUSERPATH@


# mysql
MYSQLINCLUDES = /I "C:\Program Files\MySQL\MySQL Connector.C 6.1\include"
MYSQLLIBS = /LIBPATH:"C:\Program Files\MySQL\MySQL Connector.C 6.1\lib" libmysql.lib
MYSQLLIBSPATH =
MYSQLUSERPATH =
MYSQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
MYSQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
MYSQLDRLIBRPATH = 


# postgresql
POSTGRESQLINCLUDES = @POSTGRESQLINCLUDES@
POSTGRESQLLIBS = @POSTGRESQLLIBS@
POSTGRESQLLIBSPATH = @POSTGRESQLLIBSPATH@
POSTGRESQLUSERPATH = @POSTGRESQLUSERPATH@
POSTGRESQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
POSTGRESQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
POSTGRESQLDRLIBRPATH =


# sqlite
SQLITEINCLUDES = @SQLITEINCLUDES@
SQLITELIBS = @SQLITELIBS@
SQLITELIBSPATH = @SQLITELIBSPATH@
SQLITEUSERPATH = @SQLITEUSERPATH@


# freetds
FREETDSINCLUDES = @FREETDSINCLUDES@
FREETDSLIBS = @FREETDSLIBS@
FREETDSLIBSPATH = @FREETDSLIBSPATH@
FREETDSUSERPATH = @FREETDSUSERPATH@


# sybase
SYBASEINCLUDES = @SYBASEINCLUDES@
SYBASELIBS = @SYBASELIBS@
SYBASELIBSPATH = @SYBASELIBSPATH@
SYBASEUSERPATH = @SYBASEUSERPATH@


# odbc
ODBCINCLUDES =
ODBCLIBS =
ODBCLIBSPATH =
ODBCUSERPATH =
ODBCUNICODE =
ODBCDRIVERCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRODBC_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c\include /I$(top_builddir)\src\api\c++\include $(RUDIMENTSINCLUDES)
ODBCDRIVERLIBS = /LIBPATH:$(top_builddir)\src\api\c++\src libsqlrclient.lib $(RUDIMENTSLIBS) user32.lib gdi32.lib odbc32.lib odbccp32.lib /DEF:sqlrodbc.def
ODBCDRIVERRPATH =


# mdbtools
MDBTOOLSINCLUDES = @MDBTOOLSINCLUDES@
MDBTOOLSLIBS = @MDBTOOLSLIBS@
MDBTOOLSLIBSPATH = @MDBTOOLSLIBSPATH@
MDBTOOLSUSERPATH = @MDBTOOLSUSERPATH@


# db2
DB2INCLUDES = @DB2INCLUDES@
DB2LIBS = @DB2LIBS@
DB2LIBSPATH = @DB2LIBSPATH@
DB2USERPATH = @DB2USERPATH@


# firebird
FIREBIRDINCLUDES = @FIREBIRDINCLUDES@
FIREBIRDLIBS = @FIREBIRDLIBS@
FIREBIRDLIBSPATH = @FIREBIRDLIBSPATH@
FIREBIRDUSERPATH = @FIREBIRDUSERPATH@


# router
ROUTERLIBSPATH =
ROUTERUSERPATH =


# tests
CPPTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
CPPTESTLIBS = /LIBPATH:$(libdir) libsqlrclient.lib $(RUDIMENTSLIBS)
CTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
CTESTLIBS = /LIBPATH:$(libdir) libsqlrclient.lib libsqlrclientwrapper.lib $(RUDIMENTSLIBS)
DROPINTESTCPPFLAGS = $(BASECPPFLAGS) /I $(top_builddir) /I $(includedir) $(RUDIMENTSINCLUDES)
DROPINTESTLIBS = $(RUDIMENTSLIBS)


# client/server rpath flags
CLIENTUSERPATH = @CLIENTUSERPATH@
SERVERUSERPATH = @SERVERUSERPATH@


# Microsoft-specific
EXE = .exe


# Shared object and module
SOSUFFIX = dll
MODULESUFFIX = dll
JNISUFFIX = dll
PYTHONSUFFIX = pyd
