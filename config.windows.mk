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
docdir = $(datadir)\\doc\\sqlrelay
EXAMPLEDIR = $(datadir)\\examples
tmpdir = $(localstatedir)\\sqlrelay\\tmp
cachedir = $(localstatedir)\\sqlrelay\\cache
debugdir = $(localstatedir)\\sqlrelay\\debug
logdir = $(localstatedir)\\sqlrelay\\log
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

# dmalloc
LIBDMALLOC =

# ElectricFence
LIBEFENCE =

# c++
CPPCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENT_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\api\c++\include /I$(top_builddir)\src\common $(RUDIMENTSINCLUDES)
CPPLIBS = $(RUDIMENTSLIBS)

# c
CCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENTWRAPPER_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\api\c\include /I$(top_builddir)\src\api\c++\include $(RUDIMENTSINCLUDES)
CLIBS = /LIBPATH:$(top_builddir)\src\api\c++\src libsqlrclient.lib $(RUDIMENTSLIBS)

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


# python
HAVE_PYTHON =
PYTHONINCLUDES = /I@PYTHONPREFIX@\include
PYTHONDIR = @PYTHONPREFIX@\Lib
PYTHONLIB = /LIBPATH:@PYTHONPREFIX@\libs python27.lib
PYTHONCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
PYTHONLIBS = $(PYTHONLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)


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


# php
HAVE_PHP =
PHPINCLUDES = /I C:\\PHP\\dev\\include\\php /I C:\\PHP\\dev\\include\\php\\main /I C:\\PHP\\dev\\include\\php\\TSRM /I C:\\PHP\\dev\\include\\php\\Zend /I C:\\PHP\\dev\\include\\php\\ext /I C:\\PHP\\dev\\include\\php\\ext\date\lib $(RUDIMENTSINCLUDES)
PHPEXTDIR = C:\\PHP\\ext
PHPVERSION =
PHPMAJORVERSION =
PHPLIB = /LIBPATH:C:\\PHP\\dev php5ts.lib
PHPCONFDIR = C:\\Windows
PHPCONFSTYLE = windows
HAVE_PHP_PDO =
PHPCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES) /D COMPILE_DL=1
PHPLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
PHPPDOCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES) /D COMPILE_DL=1
PHPPDOLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)


# java
HAVE_JAVA =
JAVAC = "@JAVAPREFIX@\bin\javac"
JAR = "@JAVAPREFIX@\bin\jar"
JAVAINCLUDES = /I "@JAVAPREFIX@\include" /I "@JAVAPREFIX@\include\win32"
JAVACPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c\include /I$(top_builddir)\src\api\c++\include $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
JAVALIBS = /LIBPATH:$(top_builddir)\src\api\c++\src libsqlrclient.lib $(RUDIMENTSLIBS) /LIBPATH:"@JAVAPREFIX@\lib" jvm.lib


# tcl
HAVE_TCL =
TCLINCLUDE = /IC:\Tcl\include
TCLLIB = /LIBPATH:C:\Tcl\lib tcl86.lib
TCLLIBSPATH = C:\Tcl\lib
TCLCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(TCLINCLUDE) /I$(top_builddir) /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
TCLLIBS = $(TCLLIB) /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)


# erlang
HAVE_ERLANG =
ERLC = @ERLC@
ERLCFLAGS = @ERLCFLAGS@
ERLANGINCLUDES = @ERLANGINCLUDES@
ERLANGLIB = @ERLANGLIBS@
ERLANG_ROOT_DIR = @ERLANG_ROOT_DIR@
ERLANG_LIB_DIR = @ERLANG_LIB_DIR@
ERLANG_INSTALL_LIB_DIR = @ERLANG_INSTALL_LIB_DIR@
ERLANGCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(ERLANGINCLUDES) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c/include /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
ERLANGLIBS = $(ERLANGLIB) /LIBPATH:$(top_builddir)/src/api/c/src /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclientwrapper.lib libsqlrclient.lib $(RUDIMENTSLIBS)


# readline
READLINEINCLUDES =
READLINELIBS =

# libsocket
SOCKETLIBS =


# oracle
ORACLEVERSION = @ORACLEVERSION@
ORACLEINCLUDES = @ORACLEINCLUDES@
ORACLELIBS = @ORACLELIBS@
ORACLELIBSPATH = @ORACLELIBSPATH@


# mysql
MYSQLINCLUDES = /I "C:\Program Files\MySQL\MySQL Connector.C 6.1\include"
MYSQLLIBS = /LIBPATH:"C:\Program Files\MySQL\MySQL Connector.C 6.1\lib" libmysql.lib
MYSQLLIBSPATH =
MYSQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
MYSQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)


# postgresql
POSTGRESQLINCLUDES = @POSTGRESQLINCLUDES@
POSTGRESQLLIBS = @POSTGRESQLLIBS@
POSTGRESQLLIBSPATH = @POSTGRESQLLIBSPATH@
POSTGRESQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
POSTGRESQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)


# sqlite
SQLITEINCLUDES = @SQLITEINCLUDES@
SQLITELIBS = @SQLITELIBS@
SQLITELIBSPATH = @SQLITELIBSPATH@


# freetds
FREETDSINCLUDES = @FREETDSINCLUDES@
FREETDSLIBS = @FREETDSLIBS@
FREETDSLIBSPATH = @FREETDSLIBSPATH@


# sybase
SYBASEINCLUDES = @SYBASEINCLUDES@
SYBASELIBS = @SYBASELIBS@
SYBASELIBSPATH = @SYBASELIBSPATH@


# odbc
ODBCINCLUDES =
ODBCLIBS =
ODBCLIBSPATH =
ODBCUNICODE =
ODBCDRIVERCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRODBC_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c\include /I$(top_builddir)\src\api\c++\include $(RUDIMENTSINCLUDES)
ODBCDRIVERLIBS = /LIBPATH:$(top_builddir)\src\api\c++\src libsqlrclient.lib $(RUDIMENTSLIBS) user32.lib gdi32.lib odbc32.lib odbccp32.lib /DEF:sqlrodbc.def


# mdbtools
MDBTOOLSINCLUDES = @MDBTOOLSINCLUDES@
MDBTOOLSLIBS = @MDBTOOLSLIBS@
MDBTOOLSLIBSPATH = @MDBTOOLSLIBSPATH@


# db2
DB2INCLUDES = @DB2INCLUDES@
DB2LIBS = @DB2LIBS@
DB2LIBSPATH = @DB2LIBSPATH@


# firebird
FIREBIRDINCLUDES = @FIREBIRDINCLUDES@
FIREBIRDLIBS = @FIREBIRDLIBS@
FIREBIRDLIBSPATH = @FIREBIRDLIBSPATH@


# router
ROUTERLIBSPATH =


# util
UTILCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRUTIL_EXPORTS $(TMPDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(CONFIGFILECPPFLAG) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) /DLIBEXECDIR=\"$(libexecdir)\"
UTILLIBS = $(RUDIMENTSLIBS)


# cmdline
CMDLINECPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) /D SYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" $(CACHEDIRCPPFLAG) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/util /I$(top_builddir)/src/server/include /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
CMDLINELIBS = /LIBPATH:$(top_builddir)/src/util libsqlrutil.lib /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
PWDENCLIBS = /LIBPATH:$(top_builddir)/src/server libsqlrserver.lib /LIBPATH:$(top_builddir)/src/util libsqlrutil.lib /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)


# tests
CPPTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
CPPTESTLIBS = /LIBPATH:$(libdir) libsqlrclient.lib $(RUDIMENTSLIBS)

CTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
CTESTLIBS = /LIBPATH:$(libdir) libsqlrclient.lib libsqlrclientwrapper.lib $(RUDIMENTSLIBS)

ODBCTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(ODBCINCLUDES)
ODBCTESTLIBS = $(RUDIMENTSLIBS) $(ODBCLIBS)

DROPINTESTCPPFLAGS = $(BASECPPFLAGS) /I $(top_builddir) /I $(includedir) $(RUDIMENTSINCLUDES)
DROPINTESTLIBS = $(RUDIMENTSLIBS)


# Microsoft-specific
EXE = .exe


# Shared object and module
SOSUFFIX = dll
MODULESUFFIX = dll
JNISUFFIX = dll
PYTHONSUFFIX = pyd
