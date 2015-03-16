SQLR_VERSION = @SQLR_VERSION@
SONAME_VERSION_INFO = @SONAME_VERSION_INFO@

# installation directories
prefix = @prefix@
exec_prefix= @exec_prefix@
includedir = @includedir@
libdir = @libdir@
javadir = ${exec_prefix}\java
libexecdir = @libexecdir@\sqlrelay
bindir = @bindir@
localstatedir = @localstatedir@
sysconfdir = @sysconfdir@
mandir = @mandir@
datadir = @datadir@
docdir = ${datadir}\doc\sqlrelay
EXAMPLEDIR = ${datadir}\examples
tmpdir = ${localstatedir}\sqlrelay\tmp
cachedir = ${localstatedir}\sqlrelay\cache
debugdir = ${localstatedir}\sqlrelay\debug
logdir = ${localstatedir}\sqlrelay\log
initscript_prefix = @initscript_prefix@

# command separator
AND = &

# shell
SHELL =

# libtool command
LIBTOOL =
LIBTOOLGCC =

# compile commands
LTCOMPILE = 
CC = cl
CXX = cl
COMPILE = /c
OUT = -out:
BASECPPFLAGS = /nologo @OPTCPPFLAGS@ @DEBUGCPPFLAGS@ @WINVER@ @WIN32WINDOWS@ @WIN32WINNT@ @_USE_32_BIT_TIME@ @SDKINCLUDES@
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
LDFLAGS = /nologo @DEBUGLDFLAGS@ @SDKLIBs@
LINKFLAGS = /dll
MODLINKFLAGS = /dll
LIBPATH = /LIBPATH:
LIB =
INSTALLLIB = installdll
UNINSTALLLIB = uninstalldll
LIBEXT = dll

# install commands
LTINSTALL =
MV = rename 
CP = cscript /nologo @top_builddir@\cp.vbs
CHMOD = echo
MKINSTALLDIRS = cscript /nolog @top_builddir@\mkinstalldirs.vbs
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
RUDIMENTSINCLUDES = /I C:\Program Files\Firstworks\include
RUDIMENTSLIBS = /LIBPATH:C:\Program Files\Firstworks\lib librudiments.lib
RUDIMENTSLIBSPATH =

#iconv
HAVE_ICONV =
ICONVINCLUDES =
ICONVLIBS =

# c++
CPPCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENT_EXPORTS /I $(top_builddir) /I $(top_builddir)\src\api\c++\include /I $(top_builddir)\src\common $(RUDIMENTSINCLUDES)
CPPLIBS = $(RUDIMENTSLIBS)

# c
CUSERPATH =
CCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENTWRAPPER_EXPORTS /I $(top_builddir) /I $(top_builddir)\src\api\c\include /I $(top_builddir)\src\api\c++\include $(RUDIMENTSINCLUDES)
CLIBS = /LIBDIR:$(top_builddir)\src\api\c++\src libsqlrclient.lib $(RUDIMENTSLIBS)
CRPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(CUSERPATH),yes)
	CRPATH = -R $(libdir)
endif
endif

# perl
HAVE_PERL = @HAVE_PERL@
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
PERLCCFLAGS_LOCAL = $(shell echo "$(PERLCCFLAGS)" | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e 's|-x.*$$||g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g" -e "s|-arch .* ||g" -e "s|-Kalloca ||g")
PERLOPTIMIZE_LOCAL = $(shell echo "$(PERLOPTIMIZE)" | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e 's|-x.*$$||g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g")
ifeq ($(OVERRIDEPERLSITEARCH),)
PERLSITEARCH_LOCAL = $(DESTDIR)$(shell echo "$(PERLSITEARCH)" | sed -e "s|UNKNOWN||g" )
else
PERLSITEARCH_LOCAL = $(DESTDIR)$(OVERRIDEPERLSITEARCH)
endif
ifeq ($(OVERRIDEPERLSITELIB),)
PERLSITELIB_LOCAL = $(DESTDIR)$(shell echo "$(PERLSITELIB)" | sed -e "s|UNKNOWN||g" )
else
PERLSITELIB_LOCAL = $(DESTDIR)$(OVERRIDEPERLSITELIB)
endif
PERLINC_LOCAL = $(DESTDIR)$(shell echo "$(PERLINC)" | sed -e "s|UNKNOWN||g" )
ifeq ($(OVERRIDEPERLINSTALLMAN3DIR),)
PERLINSTALLMAN3DIR_LOCAL = $(DESTDIR)$(shell echo "$(PERLINSTALLMAN3DIR)" | sed -e "s|UNKNOWN||g" )
else
PERLINSTALLMAN3DIR_LOCAL = $(DESTDIR)$(OVERRIDEPERLINSTALLMAN3DIR)
endif
ifeq ($(OVERRIDEPERLMAN3EXT),)
PERLMAN3EXT_LOCAL = $(shell echo "$(PERLMAN3EXT)" | sed -e "s|UNKNOWN||g" )
else
PERLMAN3EXT_LOCAL = $(OVERRIDEPERLMAN3EXT)
endif
PERLCPPFLAGS = $(BASECPPFLAGS) $(PERLOPTIMIZE_LOCAL) $(PERLCCFLAGS_LOCAL) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PERLINC_LOCAL)
PERLCONLIBS = $(PERLLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE) -rpath $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection
PERLCURLIBS = $(PERLLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE) -rpath $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor
PERLRPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PERLUSERPATH),yes)
	PERLRPATH = -R $(libdir)
endif
endif


# python
HAVE_PYTHON = @HAVE_PYTHON@
PYTHONINCLUDES = @PYTHONFRAMEWORK@ @PYTHONINCLUDES@
PYTHONDIR = @PYTHONDIR@
PYTHONUSERPATH = @PYTHONUSERPATH@
PYTHONLIB = @PYTHONLIB@
PYTHONCPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
PYTHONLIBS = $(PYTHONLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE) -rpath $(PYTHONDIR)/site-packages/SQLRelay
PYTHONRPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PYTHONUSERPATH),yes)
	PYTHONRPATH = -R $(libdir)
endif
endif


# ruby
HAVE_RUBY = @HAVE_RUBY@
RUBY = @RUBY@
RUBYLIB = @RUBYLIB@
OVERRIDERUBYSITEARCHDIR = @OVERRIDERUBYSITEARCHDIR@

RUBYCFLAGS = $(shell LANG=POSIX $(RUBY) getcflags.rb | sed -e "s|-x.* | |g" -e "s|-belf||g" -e "s|-mtune=.* | |g" | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )

ifeq ($(OVERRIDERUBYSITEARCHDIR),)
RUBYSITEARCHDIR = $(shell LANG=POSIX $(RUBY) getsitearchdir.rb | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )
else
RUBYSITEARCHDIR = $(OVERRIDERUBYSITEARCHDIR)
endif

RUBYCPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(RUBYCFLAGS) -I./ -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
RUBYLIBS = $(RUBYLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE) -rpath $(RUBYSITEARCHDIR)
RUBYRPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(RUBYUSERPATH),yes)
	RUBYRPATH = -R $(libdir)
endif
endif


# php
HAVE_PHP = @HAVE_PHP@
PHPINCLUDES = @PHPINCLUDES@
PHPEXTDIR = @PHPEXTDIR@
PHPUSERPATH = @PHPUSERPATH@
PHPVERSION = @PHPVERSION@
PHPMAJORVERSION = @PHPMAJORVERSION@
PHPLIB = @PHPLIB@
PHPCONFDIR = @PHPCONFDIR@
PHPCONFSTYLE = @PHPCONFSTYLE@
HAVE_PHP_PDO = @HAVE_PHP_PDO@
PHPCPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES) -DCOMPILE_DL=1
PHPLIBS = $(PHPLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(PHPEXTDIR)
PHPRPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PHPUSERPATH),yes)
	PHPRPATH = -R $(libdir)
endif
endif
PHPPDOCPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES) -DCOMPILE_DL=1
PHPPDOLIBS = $(PHPLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(PHPEXTDIR)
PHPPDORPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PHPUSERPATH),yes)
	PHPPDORPATH = -R $(libdir)
endif
endif


# java
HAVE_JAVA = @HAVE_JAVA@
JAVAC = @JAVAC@
JAR = @JAR@
JAVAINCLUDES = @JAVAINCLUDES@
JAVAUSERPATH = @JAVAUSERPATH@
JAVACPPFLAGS = $(WERROR) $(WNOUNKNOWNPRAGMAS) $(BASECPPFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
JAVALIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(javadir)/com/firstworks/sqlrelay
JAVARPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(JAVAUSERPATH),yes)
	JAVARPATH = -R $(libdir)
endif
endif


# tcl
HAVE_TCL = @HAVE_TCL@
TCLINCLUDE = @TCLINCLUDE@
TCLLIB = @TCLLIB@
TCLLIBSPATH = @TCLLIBSPATH@
TCLUSERPATH = @TCLUSERPATH@
TCLCPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(TCLINCLUDE) -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
TCLLIBS = $(TCLLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE) -rpath $(TCLLIBSPATH)/sqlrelay
TCLRPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(TCLUSERPATH),yes)
	TCLRPATH = -R $(libdir)
endif
endif


# erlang
HAVE_ERLANG = @HAVE_ERLANG@
ERLC = @ERLC@
ERLCFLAGS = @ERLCFLAGS@
ERLANGINCLUDES = @ERLANGINCLUDES@
ERLANGLIB = @ERLANGLIBS@
ERLANG_ROOT_DIR = @ERLANG_ROOT_DIR@
ERLANG_LIB_DIR = @ERLANG_LIB_DIR@
ERLANG_INSTALL_LIB_DIR = @ERLANG_INSTALL_LIB_DIR@
ERLANGUSERPATH = @ERLANGUSERPATH@
ERLANGCPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(ERLANGINCLUDES) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c/include -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
ERLANGLIBS = $(ERLANGLIB) -L$(top_builddir)/src/api/c/src -L$(top_builddir)/src/api/c++/src -lsqlrclientwrapper -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
ERLANGRPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(ERLANGUSERPATH),yes)
	ERLANGRPATH = -R $(libdir)
endif
endif


# psql
PSQLUSERPATH = @PSQLUSERPATH@


# readline
READLINEINCLUDES = @READLINEINCLUDES@
READLINELIBS = @READLINELIBS@

# libsocket
SOCKETLIBS = @SOCKETLIBS@


# oracle
ORACLEVERSION = @ORACLEVERSION@
ORACLEINCLUDES = @ORACLEINCLUDES@
ORACLELIBS = @ORACLELIBS@
ORACLELIBSPATH = @ORACLELIBSPATH@
ORACLEUSERPATH = @ORACLEUSERPATH@


# mysql
MYSQLINCLUDES = @MYSQLINCLUDES@
MYSQLLIBS = @MYSQLLIBS@
MYSQLLIBSPATH = @MYSQLLIBSPATH@
MYSQLUSERPATH = @MYSQLUSERPATH@
MYSQLDRLIBCPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) -DSQLR_VERSION=\"$(SQLR_VERSION)\"
MYSQLDRLIBLIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)
MYSQLDRLIBRPATH = 
ifneq ($(strip $(libdir)),)
ifeq ($(PSQLUSERPATH),yes)
	MYSQLDRLIBRPATH = -R $(libdir)
endif
endif


# postgresql
POSTGRESQLINCLUDES = @POSTGRESQLINCLUDES@
POSTGRESQLLIBS = @POSTGRESQLLIBS@
POSTGRESQLLIBSPATH = @POSTGRESQLLIBSPATH@
POSTGRESQLUSERPATH = @POSTGRESQLUSERPATH@
POSTGRESQLDRLIBCPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
POSTGRESQLDRLIBLIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)
POSTGRESQLDRLIBRPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PSQLUSERPATH),yes)
	POSTGRESQLDRLIBRPATH = -R $(libdir)
endif
endif


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
ODBCINCLUDES = @ODBCINCLUDES@
ODBCLIBS = @ODBCLIBS@
ODBCLIBSPATH = @ODBCLIBSPATH@
ODBCUSERPATH = @ODBCUSERPATH@
ODBCUNICODE = @ODBCUNICODE@
ODBCDRIVERCPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(ODBCINCLUDES) $(ICONVINCLUDES) -DSQLR_VERSION=\"$(SQLR_VERSION)\"
ODBCDRIVERLIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(ODBCLIBS)
ODBCDRIVERRPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PSQLUSERPATH),yes)
	ODBCRPATH = -R $(libdir)
endif
endif


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


# client/server rpath flags
CLIENTUSERPATH = @CLIENTUSERPATH@
SERVERUSERPATH = @SERVERUSERPATH@


# dmalloc
LIBDMALLOC = @LIBDMALLOC@


# ElectricFence
LIBEFENCE = @LIBEFENCE@


# Microsoft-specific
EXE = @EXE@


# Shared object and module
SQLRELAY_ENABLE_SHARED = @enable_shared@
SOSUFFIX = @SOSUFFIX@
MODULESUFFIX = @MODULESUFFIX@
JNISUFFIX = @JNISUFFIX@
MODULERENAME = $(top_builddir)/modulerename.sh
