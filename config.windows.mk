SQLR_VERSION = @SQLR_VERSION@
SONAME_VERSION_INFO =

SQLRELAY_ENABLE_SHARED = yes

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
TCLLIBSPATH = C:\Tclib
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
ORACLEVERSION = 12c
ORACLEINCLUDES = /I "C:\Program Files\Oracle\instantclient_12_1\sdk\include"
ORACLELIBS = /LIBPATH:"C:\Program Files\Oracle\instantclient_12_1\sdk\lib\msvc" oci.lib


# mysql
MYSQLINCLUDES = /I "C:\Program Files\MySQL\MySQL Connector.C 6.1\include"
MYSQLLIBS = /LIBPATH:"C:\Program Files\MySQL\MySQL Connector.C 6.1\lib" libmysql.lib
MYSQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
MYSQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)


# postgresql
POSTGRESQLINCLUDES = /I "C:\Program Files\PostgreSQL\9.4\include"
POSTGRESQLLIBS = /LIBPATH:"C:\Program Files\PostgreSQL\9.4\lib" libpq.lib
POSTGRESQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
POSTGRESQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)


# sqlite
SQLITEINCLUDES = @SQLITEINCLUDES@
SQLITELIBS = @SQLITELIBS@


# sybase
SYBASEINCLUDES = /I "C:\SAP\OCS-16_0\include"
SYBASELIBS = /LIBPATH:"C:\SAP\OCS-16_0\lib" libsybblk64.lib libsybct64.lib libsybcs64.lib


# odbc
ODBCINCLUDES =
ODBCLIBS = user32.lib gdi32.lib odbc32.lib odbccp32.lib
ODBCUNICODE =
ODBCDRIVERCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRODBC_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c\include /I$(top_builddir)\src\api\c++\include $(RUDIMENTSINCLUDES) $(ODBCINCLUDES)
ODBCDRIVERLIBS = /LIBPATH:$(top_builddir)\src\api\c++\src libsqlrclient.lib $(RUDIMENTSLIBS) $(ODBCLIBS) /DEF:sqlrodbc.def


# db2
DB2INCLUDES = /I"C:\Program Files\IBM\SQLLIB\include"
DB2LIBS = /LIBPATH:"C:\Program Files\IBM\SQLLIB\lib" db2api.lib


# firebird
FIREBIRDINCLUDES = /I"C:\Program Files\Firebird\Firebird_2_5\include"
FIREBIRDLIBS = /LIBPATH:"C:\Program Files\Firebird\Firebird_2_5\lib" fbclient_ms.lib


# util
UTILCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRUTIL_EXPORTS $(TMPDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(CONFIGFILECPPFLAG) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) /DLIBEXECDIR=\"$(libexecdir)\"
UTILLIBS = $(RUDIMENTSLIBS)


# cmdline
CMDLINECPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) /D SYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" $(CACHEDIRCPPFLAG) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/util /I$(top_builddir)/src/server/include /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
CMDLINELIBS = /LIBPATH:$(top_builddir)/src/util libsqlrutil.lib /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)
PWDENCLIBS = /LIBPATH:$(top_builddir)/src/server/src libsqlrserver.lib /LIBPATH:$(top_builddir)/src/util libsqlrutil.lib /LIBPATH:$(top_builddir)/src/api/c++/src libsqlrclient.lib $(RUDIMENTSLIBS)


# server
STATICPLUGINSRCS = 
STATICPLUGINOBJS =
STATICPLUGINLIBS =

SERVERCPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) $(CACHEDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(LOGDIRCPPFLAG) /I../include /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) /D LIBEXECDIR=\"$(libexecdir)\" /D LIBSQLRSERVER_EXPORTS
LIBSQLRSERVERLIBS = /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib
SERVERLIBS = /LIBPATH:./ libsqlrserver.lib $(STATICPLUGINLIBS) /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)


# plugins
PLUGINCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir) /I$(top_builddir)\src\util /I$(top_builddir)\src\server\include /I$(top_builddir)\src\common $(RUDIMENTSINCLUDES) /D LIBSQLRSERVER_EXPORTS
PLUGINLIBS = /LIBPATH:$(top_builddir)\src\server\src libsqlrserver.lib /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib $(RUDIMENTSLIBS) $(EXTRALIBS)

AUTHCPPFLAGS = $(PLUGINCPPFLAGS) /I$(top_builddir)\src\api\c++\include
SQLRAUTH_SQLRELAYLIBS = /LIBPATH:$(top_builddir)\src\api\c++\src libsqlrclient.lib

LOGGERCPPFLAGS = $(PLUGINCPPFLAGS) $(DEBUGDIRCPPFLAG) $(LOGDIRCPPLAG)

INSTALLSHAREDLIB =


# connections
CONNECTIONCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)\ /I$(top_builddir)\src\common /I$(top_builddir)\src\util /I$(top_builddir)\src\server\include $(RUDIMENTSINCLUDES) /D LIBSQLRSERVER_EXPORTS
CONNECTIONLIBS = /LIBPATH:$(top_builddir)\src\server\src libsqlrserver.lib /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)

DB2CONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(DB2INCLUDES)
DB2CONNECTIONLIBS = $(DB2LIBS) $(CONNECTIONLIBS)

FIREBIRDCONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(FIREBIRDINCLUDES)
FIREBIRDCONNECTIONLIBS = $(FIREBIRDLIBS) $(CONNECTIONLIBS)

MYSQLCONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(MYSQLINCLUDES)
MYSQLCONNECTIONLIBS = $(MYSQLLIBS) $(CONNECTIONLIBS)

ODBCCONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(ODBCINCLUDES) $(ICONVINCLUDES)
ODBCCONNECTIONLIBS = $(ODBCLIBS) $(ICONVLIBS) $(CONNECTIONLIBS)

ORACLECONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(ORACLEINCLUDES)
ORACLECONNECTIONLIBS = $(ORACLELIBS) $(CONNECTIONLIBS)

POSTGRESQLCONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(POSTGRESQLINCLUDES)
POSTGRESQLCONNECTIONLIBS = $(POSTGRESQLLIBS) $(CONNECTIONLIBS)

ROUTERCONNECTIONCPPFLAGS = /I$(top_builddir)\src\api\c++\include $(CONNECTIONCPPFLAGS)
ROUTERCONNECTIONLIBS = /LIBPATH:$(top_builddir)\src\api\c++\src libsqlrclient.lib $(CONNECTIONLIBS)

SQLITECONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(SQLITEINCLUDES)
SQLITECONNECTIONLIBS = $(SQLITELIBS) $(CONNECTIONLIBS)

SYBASECONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(SYBASEINCLUDES)
SYBASECONNECTIONLIBS = $(SYBASELIBS) $(CONNECTIONLIBS)


# connections
CONNECTIONSALLSUBDIRS = all-db2 all-firebird all-mysql all-odbc all-oracle8 all-postgresql all-sybase all-router
CONNECTIONSINSTALLSUBDIRS = install-db2 install-firebird install-mysql install-odbc install-oracle8 install-postgresql install-sybase install-router


# tests
CPPTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
CPPTESTLIBS = /LIBPATH:$(libdir) libsqlrclient.lib $(RUDIMENTSLIBS)

CTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
CTESTLIBS = /LIBPATH:$(libdir) libsqlrclient.lib libsqlrclientwrapper.lib $(RUDIMENTSLIBS)

ODBCTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(ODBCINCLUDES)
ODBCTESTLIBS = $(RUDIMENTSLIBS) $(ODBCLIBS)

DROPINTESTTARGETS = mysql postgresql
DROPINTESTCPPFLAGS = $(BASECPPFLAGS) /I $(top_builddir) /I $(includedir) $(RUDIMENTSINCLUDES)
DROPINTESTLIBS = $(RUDIMENTSLIBS)


# microsoft-specific
EXE = .exe


# shared object and module
SOSUFFIX = dll
MODULESUFFIX = dll
JNISUFFIX = dll
PYTHONSUFFIX = pyd


# build directories
INSTALLSUBDIRS = install-src install-bin install-etc install-doc
UNINSTALLSUBDIRS = uninstall-src uninstall-bin uninstall-etc uninstall-doc

SRCALLSUBDIRS = all-util all-api all-server all-parsers all-queries all-loggers all-protocols all-pwdencs all-auths all-connections all-cmdline

SRCINSTALLSUBDIRS = install-util install-api install-server install-parsers install-queries install-loggers install-protocols install-pwdencs install-auths install-connections install-cmdline


#APIALLSUBDIRS = all-cpp all-c all-postgresql all-mysql all-odbc all-python all-perl all-ruby all-php all-phppdo all-java all-tcl all-erlang
APIALLSUBDIRS = all-cpp all-c all-postgresql all-mysql all-odbc all-python all-php all-phppdo all-java all-tcl

#APICLEANSUBDIRS = clean-cpp clean-c clean-postgresql clean-mysql clean-odbc clean-python clean-perl clean-ruby clean-php clean-phppdo clean-java clean-tcl clean-erlang
APICLEANSUBDIRS = clean-cpp clean-c clean-postgresql clean-mysql clean-odbc clean-python clean-php clean-phppdo clean-java clean-tcl

#APIINSTALLSUBDIRS = install-cpp install-c install-postgresql install-mysql install-odbc install-python install-perl install-ruby install-php install-phppdo install-java install-tcl install-erlang
APIINSTALLSUBDIRS = install-cpp install-c install-postgresql install-mysql install-odbc install-python install-php install-phppdo install-java install-tcl

#APIUNINSTALLSUBDIRS = uninstall-cpp uninstall-c uninstall-postgresql uninstall-mysql uninstall-odbc uninstall-python uninstall-perl uninstall-ruby uninstall-php uninstall-phppdo uninstall-java uninstall-tcl uninstall-erlang
APIUNINSTALLSUBDIRS = uninstall-cpp uninstall-c uninstall-postgresql uninstall-mysql uninstall-odbc uninstall-python uninstall-php uninstall-phppdo uninstall-java uninstall-tcl
