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
REPLACE = cscript /nologo @top_builddir@\replace.vbs


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
ICONVINCLUDES =
ICONVLIBS =


# dmalloc
LIBDMALLOC =


# ElectricFence
LIBEFENCE =


# c++
CPPCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENT_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\common $(RUDIMENTSINCLUDES)
CPPLIBS = $(RUDIMENTSLIBS)


# c
CCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENTWRAPPER_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\api\c++ $(RUDIMENTSINCLUDES)
CLIBS = /LIBPATH:$(top_builddir)\src\api\c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# perl
PERLPREFIX = @PERLPREFIX@
PERL = $(PERLPREFIX)\bin\perl
PERLLIB = $(PERLPREFIX)\lib
PERLINC = /I $(PERLLIB)\CORE
PERLCCFLAGS =
PERLOPTIMIZE =
PERLSITEARCH = $(PERLPREFIX)\site\lib
PERLSITELIB = $(PERLPREFIX)\site\lib
PERLARCHLIBEXP =
PERLINSTALLMAN3DIR = $(PERLPREFIX)\man\man3
PERLMAN3EXT = 3
PERLMANCLASSSEPARATOR = _
XSUBPP = $(PERLPREFIX)\bin\xsubpp.bat
POD2MAN = $(PERLPREFIX)\bin\pod2man.bat
OVERRIDEPERLSITEARCH =
OVERRIDEPERLSITELIB =
OVERRIDEPERLINSTALLMAN3DIR =
OVERRIDEPERLMAN3EXT =
PERLCCFLAGS_LOCAL = $(PERLCCFLAGS)
PERLOPTIMIZE_LOCAL = $(PERLOPTIMIZE)
PERLSITEARCH_LOCAL = $(PERLSITEARCH)
PERLSITELIB_LOCAL = $(PERLSITELIB)
PERLINC_LOCAL = $(PERLINC)
PERLINSTALLMAN3DIR_LOCAL = $(PERLINSTALLMAN3DIR)
PERLMAN3EXT_LOCAL = $(PERLMAN3EXT)
PERLCPPFLAGS = $(BASECPPFLAGS) $(PERLOPTIMIZE_LOCAL) $(PERLCCFLAGS_LOCAL) /I./ /I$(top_builddir) /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES) $(PERLINC_LOCAL)
PERLCONLIBS = /LIBPATH:$(PERLLIB)\CORE perl520.lib /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)
PERLCURLIBS = /LIBPATH:$(PERLLIB)\CORE perl520.lib /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)
PERLINSTALLMAN = installman


# python
PYTHONINCLUDES = /IC:\Python27\include
PYTHONDIR = C:\Python27\Lib
PYTHONLIB = /LIBPATH:C:\Python27\libs python27.lib
PYTHONCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
PYTHONLIBS = $(PYTHONLIB) /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# ruby
RUBYPREFIX = C:\Ruby
RUBYARCH = @RUBYARCH@
RUBYVCVERSION = @RUBYVCVERSION@
RUBYTARGET = @RUBYTARGET@
RUBY = $(RUBYPREFIX)\bin\ruby
RUBYLIB = /LIBPATH:$(RUBYPREFIX)\lib $(RUBYARCH)$(RUBYVCVERSION)-ruby220.lib
RUBYCFLAGS = /I $(RUBYPREFIX)\include\ruby-2.2.0 /I $(RUBYPREFIX)\include\ruby-2.2.0/$(RUBYTARGET)_$(RUBYVCVERSION)
RUBYSITEARCHDIR = $(RUBYPREFIX)\lib\ruby\site_ruby\2.2.0\$(RUBYARCH)$(RUBYVCVERSION)
RUBYCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(RUBYCFLAGS) /I./ /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
RUBYLIBS = $(RUBYLIB) /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# php
PHPINCLUDES = /I C:\\PHP\\dev\\include\\php /I C:\\PHP\\dev\\include\\php\\main /I C:\\PHP\\dev\\include\\php\\TSRM /I C:\\PHP\\dev\\include\\php\\Zend /I C:\\PHP\\dev\\include\\php\\ext /I C:\\PHP\\dev\\include\\php\\ext\date\lib $(RUDIMENTSINCLUDES)
PHPEXTDIR = C:\\PHP\\ext
PHPVERSION =
PHPMAJORVERSION =
PHPLIB = /LIBPATH:C:\\PHP\\dev php5ts.lib
PHPCONFDIR = C:\\Windows
PHPCONFSTYLE = windows
PHPCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES) $(PHPINCLUDES) /D COMPILE_DL=1
PHPLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)
PHPPDOCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES) $(PHPINCLUDES) /D COMPILE_DL=1
PHPPDOLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# java
JAVAC = "C:\Program Files\Java\jdk1.8.0_25\bin\javac"
JAR = "C:\Program Files\Java\jdk1.8.0_25\bin\jar"
JAVAINCLUDES = /I "C:\Program Files\Java\jdk1.8.0_25\include" /I "C:\Program Files\Java\jdk1.8.0_25\include\win32"
JAVACPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c /I$(top_builddir)\src\api\c++ $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
JAVALIBS = /LIBPATH:$(top_builddir)\src\api\c++ libsqlrclient.lib $(RUDIMENTSLIBS) /LIBPATH:"C:\Program Files\Java\jdk1.8.0_25\lib" jvm.lib


# tcl
TCLINCLUDE = /IC:\Tcl\include
TCLLIB = /LIBPATH:C:\Tcl\lib tcl86.lib
TCLLIBSPATH = C:\\Tcl\\lib
TCLCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(TCLINCLUDE) /I$(top_builddir) /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
TCLLIBS = $(TCLLIB) /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# erlang
ERLC = @ERLC@
ERLCFLAGS = @ERLCFLAGS@
ERLANGINCLUDES = @ERLANGINCLUDES@
ERLANGLIB = @ERLANGLIBS@
ERLANG_ROOT_DIR = @ERLANG_ROOT_DIR@
ERLANG_LIB_DIR = @ERLANG_LIB_DIR@
ERLANG_INSTALL_LIB_DIR = @ERLANG_INSTALL_LIB_DIR@
ERLANGCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(ERLANGINCLUDES) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
ERLANGLIBS = $(ERLANGLIB) /LIBPATH:$(top_builddir)/src/api/c /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclientwrapper.lib libsqlrclient.lib $(RUDIMENTSLIBS)


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
MYSQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
MYSQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# postgresql
POSTGRESQLINCLUDES = /I "C:\Program Files\PostgreSQL\9.4\include"
POSTGRESQLLIBS = /LIBPATH:"C:\Program Files\PostgreSQL\9.4\lib" libpq.lib
POSTGRESQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
POSTGRESQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


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
ODBCDRIVERCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRODBC_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c /I$(top_builddir)\src\api\c++ $(RUDIMENTSINCLUDES) $(ODBCINCLUDES)
ODBCDRIVERLIBS = /LIBPATH:$(top_builddir)\src\api\c++ libsqlrclient.lib $(RUDIMENTSLIBS) $(ODBCLIBS) /DEF:sqlrodbc.def


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
CMDLINECPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) /D SYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" $(CACHEDIRCPPFLAG) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/util /I$(top_builddir)/src/server /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
CMDLINELIBS = /LIBPATH:$(top_builddir)/src/util libsqlrutil.lib /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)
PWDENCLIBS = /LIBPATH:$(top_builddir)/src/server libsqlrserver.lib /LIBPATH:$(top_builddir)/src/util libsqlrutil.lib /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# server
STATICPLUGINSRCS = 
STATICPLUGINOBJS =
STATICPLUGINLIBS =

SERVERCPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) $(CACHEDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(LOGDIRCPPFLAG) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) /D LIBEXECDIR=\"$(libexecdir)\" /D LIBSQLRSERVER_EXPORTS
LIBSQLRSERVERLIBS = /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib
SERVERLIBS = /LIBPATH:./ libsqlrserver.lib $(STATICPLUGINLIBS) /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)


# plugins
PLUGINCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir) /I$(top_builddir)\src\util /I$(top_builddir)\src\server /I$(top_builddir)\src\common $(RUDIMENTSINCLUDES) /D LIBSQLRSERVER_EXPORTS
PLUGINLIBS = /LIBPATH:$(top_builddir)\src\server libsqlrserver.lib /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib $(RUDIMENTSLIBS) $(EXTRALIBS)

AUTHCPPFLAGS = $(PLUGINCPPFLAGS) /I$(top_builddir)\src\api\c++
SQLRAUTH_SQLRELAYLIBS = /LIBPATH:$(top_builddir)\src\api\c++ libsqlrclient.lib

LOGGERCPPFLAGS = $(PLUGINCPPFLAGS) $(DEBUGDIRCPPFLAG) $(LOGDIRCPPLAG)

INSTALLSHAREDLIB =


# connections
CONNECTIONCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)\ /I$(top_builddir)\src\common /I$(top_builddir)\src\util /I$(top_builddir)\src\server $(RUDIMENTSINCLUDES) /D LIBSQLRSERVER_EXPORTS
CONNECTIONLIBS = /LIBPATH:$(top_builddir)\src\server libsqlrserver.lib /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)

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

ROUTERCONNECTIONCPPFLAGS = /I$(top_builddir)\src\api\c++ $(CONNECTIONCPPFLAGS)
ROUTERCONNECTIONLIBS = /LIBPATH:$(top_builddir)\src\api\c++ libsqlrclient.lib $(CONNECTIONLIBS)

SQLITECONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(SQLITEINCLUDES)
SQLITECONNECTIONLIBS = $(SQLITELIBS) $(CONNECTIONLIBS)

SYBASECONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(SYBASEINCLUDES)
SYBASECONNECTIONLIBS = $(SYBASELIBS) $(CONNECTIONLIBS)


# connections
CONNECTIONSALLTARGETS = all-db2 all-firebird all-mysql all-odbc all-oracle8 all-postgresql all-sybase all-router
CONNECTIONSINSTALLTARGETS = installdll-db2 installdll-firebird installdll-mysql installdll-odbc installdll-oracle8 installdll-postgresql installdll-sybase installdll-router


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


APIALLSUBDIRS = all-cpp all-c all-cs all-odbc all-python all-php all-phppdo all-java all-tcl

APICLEANSUBDIRS = clean-cpp clean-c clean-cs clean-odbc clean-python clean-php clean-phppdo clean-java clean-tcl

APIINSTALLSUBDIRS = install-cpp install-c install-cs install-odbc install-python install-php install-phppdo install-java install-tcl

APIUNINSTALLSUBDIRS = uninstall-cpp uninstall-c uninstall-cs uninstall-odbc uninstall-python uninstall-php uninstall-phppdo uninstall-java uninstall-tcl
