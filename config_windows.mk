SQLR_VERSION = @SQLR_VERSION@

SQLRELAY_ENABLE_SHARED = yes

SQLR = @SQLR@
SQLRELAY = @SQLRELAY@

# installation directories
prefix = "@prefix@"
exec_prefix = "@exec_prefix@"
includedir = "@includedir@"
libdir = "@libdir@"
javadir = "@javadir@"
dotnetdir = "@bindir@"
libexecdir = "@libexecdir@"
bindir = "@bindir@"
localstatedir = "@localstatedir@"
sysconfdir = "@sysconfdir@"
mandir = "@mandir@"
datadir = "@datadir@"
docdir = $(datadir)\\doc\\$(SQLRELAY)
licensedir = $(datadir)\\licenses\\$(SQLRELAY)
EXAMPLEDIR = $(datadir)\\$(SQLRELAY)\\examples
tmpdir = $(localstatedir)\\run\\$(SQLRELAY)
cachedir = $(localstatedir)\\cache\\$(SQLRELAY)
logdir = $(localstatedir)\\log\\$(SQLRELAY)
debugdir = $(logdir)\\debug
initscript_prefix = @initscript_prefix@


# command separator
AND = &


# "this directory" prefix
THISDIR =


# script extension
SCRIPTINT = cscript /nologo
SCRIPTEXT = .vbs


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


# linker flags
LTLINK =
LINK = link
CCLINK = link
AR =
LDFLAGS = /nologo @DEBUGLDFLAGS@ @SDKLIBS@
SRVLINKFLAGS = /dll
CLTLINKFLAGS = /dll
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
RMDIR = cscript /nologo @top_builddir@\rmdir.vbs


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
CPPCPPFLAGS = $(BASECPPFLAGS) /D SQLRCLIENT_EXPORTS /I$(top_builddir) /I./ /I$(top_builddir)\src\common $(RUDIMENTSINCLUDES)
CPPLIBS = $(RUDIMENTSLIBS)


# c
CCPPFLAGS = $(BASECPPFLAGS) /D SQLRCLIENTWRAPPER_EXPORTS /I$(top_builddir) /I./ /I$(top_builddir)\src\api\c++ $(RUDIMENTSINCLUDES)
CLIBS = /LIBPATH:$(top_builddir)\src\api\c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)


# c#
CSC = csc
CSCFLAGS =
SN = sn
ILDASM = ildasm /text
ILDASMOUT = /out=
ILASM = ilasm
GACUTIL = gacutil


# perl
PERLPREFIX = @PERLPREFIX@
PERLVERSION = @PERLVERSION@
PERL = $(PERLPREFIX)\bin\perl
PERLLIB = $(PERLPREFIX)\lib
PERLINC = /I $(PERLLIB)\CORE
PERLCCFLAGS = /DPERL_IMPLICIT_SYS
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
PERLCPPFLAGS = $(BASECPPFLAGS) $(PERLOPTIMIZE_LOCAL) $(PERLCCFLAGS_LOCAL) /I$(top_builddir) /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES) $(PERLINC_LOCAL)
PERLCONLIBS = /LIBPATH:$(PERLLIB)\CORE perl$(PERLVERSION).lib /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)
PERLCURLIBS = /LIBPATH:$(PERLLIB)\CORE perl$(PERLVERSION).lib /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)
PERLINSTALLMAN = installman


# python
PYTHONPREFIX = @PYTHONPREFIX@
PYTHONVERSION = @PYTHONVERSION@
PYTHONINCLUDES = /I$(PYTHONPREFIX)\include
PYTHONDIR = $(PYTHONPREFIX)\Lib
PYTHONSITEDIR = site-packages
PYTHONLIB = /LIBPATH:$(PYTHONPREFIX)\libs python$(PYTHONVERSION).lib
PYTHONCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
PYTHONLIBS = $(PYTHONLIB) /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)
IMPORTEXCEPTIONS = @IMPORTEXCEPTIONS@
EXCEPTIONSSTANDARDERROR = @EXCEPTIONSSTANDARDERROR@


# ruby
RUBYPREFIX = @RUBYPREFIX@
RUBYVERSION= @RUBYVERSION@
RUBYLIBVERSION= @RUBYLIBVERSION@
RUBYVCVERSION = @RUBYVCVERSION@
RUBYTARGET = @RUBYTARGET@
RUBYLIBPREFIX = @RUBYLIBPREFIX@
RUBYSITEARCHDIRSUFFIX = @RUBYSITEARCHDIRSUFFIX@
RUBY = $(RUBYPREFIX)\bin\ruby
RUBYLIB = /LIBPATH:$(RUBYPREFIX)\lib $(RUBYLIBPREFIX)-ruby$(RUBYLIBVERSION).lib
RUBYCFLAGS = /I $(RUBYPREFIX)\include\ruby-$(RUBYVERSION) /I $(RUBYPREFIX)\include\ruby-$(RUBYVERSION)/$(RUBYTARGET)_$(RUBYVCVERSION)
RUBYSITEARCHDIR = $(RUBYPREFIX)\lib\ruby\site_ruby\$(RUBYVERSION)\$(RUBYSITEARCHDIRSUFFIX)
RUBYCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(RUBYCFLAGS) /I./ /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
RUBYLIBS = $(RUBYLIB) /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)


# php
PHPPREFIX = @PHPPREFIX@
PHPINCLUDES = /I $(PHPPREFIX)\dev\include\php /I $(PHPPREFIX)\dev\include\php\main /I $(PHPPREFIX)\dev\include\php\TSRM /I $(PHPPREFIX)\dev\include\php\Zend /I $(PHPPREFIX)\dev\include\php\ext /I $(PHPPREFIX)\dev\include\php\ext\date\lib $(RUDIMENTSINCLUDES)
PHPEXTDIR = $(PHPPREFIX)\ext
PHPVERSION =
PHPMAJORVERSION =
PHPLIB = /LIBPATH:$(PHPPREFIX)\dev php5ts.lib
PHPCONFDIR = C:\Windows
PHPCONFSTYLE = windows
PHPCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir) /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES) $(PHPINCLUDES) /D COMPILE_DL=1
PHPLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)
PHPPDOCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES) $(PHPINCLUDES) /D COMPILE_DL=1
PHPPDOLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)


# java
JAVAPREFIX = @JAVAPREFIX@
JAVAC = "$(JAVAPREFIX)\bin\javac"
JAR = "$(JAVAPREFIX)\bin\jar"
JAVAINCLUDES = /I "$(JAVAPREFIX)\include" /I "$(JAVAPREFIX)\include\win32"
JAVACPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c /I$(top_builddir)\src\api\c++ $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
JAVALIBS = /LIBPATH:$(top_builddir)\src\api\c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS) /LIBPATH:"$(JAVAPREFIX)\lib" jvm.lib


# tcl
TCLPREFIX = @TCLPREFIX@
TCLINCLUDE = /I$(TCLPREFIX)\include
TCLLIB = /LIBPATH:$(TCLPREFIX)\lib tcl86.lib
TCLLIBSPATH = $(TCLPREFIX)\lib
TCLCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(TCLINCLUDE) /I$(top_builddir) /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
TCLLIBS = $(TCLLIB) /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)


# erlang
ERLC = @ERLC@
ERLCFLAGS = @ERLCFLAGS@
ERLANGINCLUDES = @ERLANGINCLUDES@
ERLANGLIB = @ERLANGLIBS@
ERLANG_ROOT_DIR = @ERLANG_ROOT_DIR@
ERLANG_LIB_DIR = @ERLANG_LIB_DIR@
ERLANG_INSTALL_LIB_DIR = @ERLANG_INSTALL_LIB_DIR@
ERLANGCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(ERLANGINCLUDES) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
ERLANGLIBS = $(ERLANGLIB) /LIBPATH:$(top_builddir)/src/api/c /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)clientwrapper.lib lib$(SQLR)client.lib $(RUDIMENTSLIBS)


# node.js
NODE = "@NODEJSPREFIX@\node.exe"
NODEGYP = $(NODE) "@NODEJSPREFIX@\node_modules\npm\node_modules\node-gyp\bin\node-gyp.js" --msvs_version=@NODEJSMSVSVERSION@
NODEMODULEDIR = "@NODEJSPREFIX@\node_modules"
NODEJSINCLUDEDIRS = @top_builddir@\src\api\c++;C:\Program Files\Firstworks\include
NODEJSLIBS = @top_builddir@\src\api\c++\lib$(SQLR)client.lib;C:\Program Files\Firstworks\lib\librudiments.lib


# libsocket
SOCKETLIBS =


# oracle
ORACLEINCLUDES = @ORACLEINCLUDES@
ORACLELIBS = @ORACLELIBS@


# mysql
MYSQLINCLUDES = @MYSQLINCLUDES@
MYSQLLIBS = @MYSQLLIBS@
MYSQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
MYSQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)


# postgresql
POSTGRESQLINCLUDES = @POSTGRESQLINCLUDES@
POSTGRESQLLIBS = @POSTGRESQLLIBS@
POSTGRESQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
POSTGRESQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)


# sqlite
SQLITEINCLUDES = @SQLITEINCLUDES@
SQLITELIBS = @SQLITELIBS@


# sap/sybase
SYBASEINCLUDES = @SYBASEINCLUDES@
SYBASELIBS = @SYBASELIBS@


# odbc
ODBCINCLUDES = @ODBCINCLUDES@
ODBCLIBS = @ODBCLIBS@
ODBCUNICODE =
ODBCDRIVERCPPFLAGS = $(BASECPPFLAGS) /D SQLRODBC_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c++ $(RUDIMENTSINCLUDES) $(ODBCINCLUDES)
ODBCDRIVERLIBS = /LIBPATH:$(top_builddir)\src\api\c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS) $(ODBCLIBS) /DEF:sqlrodbc.def


# db2
DB2INCLUDES = @DB2INCLUDES@
DB2LIBS = @DB2LIBS@


# firebird
FIREBIRDINCLUDES = @FIREBIRDINCLUDES@
FIREBIRDLIBS = @FIREBIRDLIBS@


# informix
INFORMIXINCLUDES = @INFORMIXINCLUDES@
INFORMIXLIBS = @INFORMIXLIBS@


# auth modules
AUTHALLOPTIONALTARGETS = all-sqlrelay


# util
UTILCPPFLAGS = $(BASECPPFLAGS) /D PREFIX="\"@prefix@\"" /D SQLRUTIL_EXPORTS /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common $(RUDIMENTSINCLUDES)
UTILLIBS = $(RUDIMENTSLIBS) advapi32.lib


# cmdline
CMDLINECPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/util /I$(top_builddir)/src/server /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
CMDLINELIBS = /LIBPATH:$(top_builddir)/src/util lib$(SQLR)util.lib /LIBPATH:$(top_builddir)/src/api/c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS)


# server
STATICPLUGINSRCS = 
STATICPLUGINOBJS =
STATICPLUGINLIBS =

SERVERCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) /D SQLRSERVER_EXPORTS
LIBSQLRSERVERLIBS = /LIBPATH:$(top_builddir)\src\util lib$(SQLR)util.lib
SERVERLIBS = /LIBPATH:./ lib$(SQLR)server.lib $(STATICPLUGINLIBS) /LIBPATH:$(top_builddir)\src\util lib$(SQLR)util.lib $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)
CACHEMANAGERLIBS = /LIBPATH:$(top_builddir)\src\util lib$(SQLR)util.lib $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)


# plugins
PLUGINCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir) /I$(top_builddir)\src\util /I$(top_builddir)\src\server /I$(top_builddir)\src\api\c++ /I$(top_builddir)\src\common $(RUDIMENTSINCLUDES) /D SQLRSERVER_EXPORTS /D SQLRUTIL_EXPORTS
PLUGINLIBS = /LIBPATH:$(top_builddir)\src\server lib$(SQLR)server.lib /LIBPATH:$(top_builddir)\src\util lib$(SQLR)util.lib $(RUDIMENTSLIBS) $(EXTRALIBS)
ROUTERPLUGINLIBS = /LIBPATH:$(top_builddir)\src\api\c++ lib$(SQLR)client.lib /LIBPATH:$(top_builddir)\src\server lib$(SQLR)server.lib /LIBPATH:$(top_builddir)\src\util lib$(SQLR)util.lib $(RUDIMENTSLIBS) $(EXTRALIBS)
CONFIGPLUGINLIBS = /LIBPATH:$(top_builddir)\src\util lib$(SQLR)util.lib $(RUDIMENTSLIBS) $(EXTRALIBS)

AUTHCPPFLAGS = $(PLUGINCPPFLAGS) /I$(top_builddir)\src\api\c++
SQLRAUTH_SQLRELAYLIBS = /LIBPATH:$(top_builddir)\src\api\c++ lib$(SQLR)client.lib
SQLRLOGGER_STALECURSORSLIBS = /LIBPATH:$(top_builddir)\src\api\c++ lib$(SQLR)client.lib

INSTALLSHAREDLIB =


# connections
CONNECTIONCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir)\ /I$(top_builddir)\src\common /I$(top_builddir)\src\util /I$(top_builddir)\src\server $(RUDIMENTSINCLUDES) /D SQLRSERVER_EXPORTS
CONNECTIONLIBS = /LIBPATH:$(top_builddir)\src\server lib$(SQLR)server.lib /LIBPATH:$(top_builddir)\src\util lib$(SQLR)util.lib $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)

DB2CONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(DB2INCLUDES)
DB2CONNECTIONLIBS = $(DB2LIBS) $(CONNECTIONLIBS)

FIREBIRDCONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(FIREBIRDINCLUDES)
FIREBIRDCONNECTIONLIBS = $(FIREBIRDLIBS) $(CONNECTIONLIBS)

INFORMIXCONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(INFORMIXINCLUDES)
INFORMIXCONNECTIONLIBS = $(INFORMIXLIBS) $(CONNECTIONLIBS)

MYSQLCONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(MYSQLINCLUDES)
MYSQLCONNECTIONLIBS = $(MYSQLLIBS) $(CONNECTIONLIBS)

ODBCCONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(ODBCINCLUDES) $(ICONVINCLUDES)
ODBCCONNECTIONLIBS = $(ODBCLIBS) $(ICONVLIBS) $(CONNECTIONLIBS)

ORACLECONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(ORACLEINCLUDES)
ORACLECONNECTIONLIBS = $(ORACLELIBS) $(CONNECTIONLIBS)

POSTGRESQLCONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(POSTGRESQLINCLUDES)
POSTGRESQLCONNECTIONLIBS = $(POSTGRESQLLIBS) $(CONNECTIONLIBS)

ROUTERCONNECTIONCPPFLAGS = /I$(top_builddir)\src\api\c++ $(CONNECTIONCPPFLAGS)
ROUTERCONNECTIONLIBS = /LIBPATH:$(top_builddir)\src\api\c++ lib$(SQLR)client.lib $(CONNECTIONLIBS)

SQLITECONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(SQLITEINCLUDES)
SQLITECONNECTIONLIBS = $(SQLITELIBS) $(CONNECTIONLIBS)

SYBASECONNECTIONCPPFLAGS = $(CONNECTIONCPPFLAGS) $(SYBASEINCLUDES)
SYBASECONNECTIONLIBS = $(SYBASELIBS) $(CONNECTIONLIBS)


# connections
CONNECTIONSALLTARGETS = @ALLDB2@ @ALLFIREBIRD@ @ALLMYSQL@ @ALLODBC@ @ALLORACLE@ @ALLPOSTGRESQL@ @ALLSQLITE@ @ALLSYBASE@ @ALLINFORMIX@ @ALLROUTER@
CONNECTIONSINSTALLTARGETS = @INSTALLDB2@ @INSTALLFIREBIRD@ @INSTALLMYSQL@ @INSTALLODBC@ @INSTALLORACLE@ @INSTALLPOSTGRESQL@ @INSTALLSQLITE@ @INSTALLSYBASE@ @INSTALLINFORMIX@ @INSTALLROUTER@


# tests
TESTALLSUBDIRS = all-dropin all-c all-cpp all-cs all-java all-stress all-tcl

CPPTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
CPPTESTLIBS = /LIBPATH:$(libdir) lib$(SQLR)client.lib $(RUDIMENTSLIBS)

CTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
CTESTLIBS = /LIBPATH:$(libdir) lib$(SQLR)client.lib lib$(SQLR)clientwrapper.lib $(RUDIMENTSLIBS)

ODBCTESTCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(ODBCINCLUDES)
ODBCTESTLIBS = $(RUDIMENTSLIBS) $(ODBCLIBS)

DROPINTESTTARGETS = mysql postgresql
DROPINTESTCPPFLAGS = $(BASECPPFLAGS) /I $(top_builddir) /I $(includedir) $(RUDIMENTSINCLUDES)
DROPINTESTLIBS = $(RUDIMENTSLIBS)


# bench
BENCHCPPFLAGS = $(BASECPPFLAGS) $(RUDIMENTSINCLUDES) /I$(top_builddir)\src\api\c++
BENCHLIBS = /LIBPATH:$(top_builddir)\src\api\c++ lib$(SQLR)client.lib $(RUDIMENTSLIBS) $(EXTRALIBS)


# stress tests
STRESSCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
STRESSLIBS = /LIBPATH:$(libdir) lib$(SQLR)client.lib $(RUDIMENTSLIBS)


# microsoft-specific
EXE = .exe


# shared object and module
SOSUFFIX = dll
MODULESUFFIX = dll
JNISUFFIX = dll
PYTHONSUFFIX = pyd


# build directories
INSTALLSUBDIRS = install-src install-etc @INSTALLDOC@ install-license
UNINSTALLSUBDIRS = uninstall-src uninstall-etc uninstall-doc uninstall-license

SRCALLSUBDIRS = @ALLUTIL@ all-api @ALLSERVER@ @ALLCMDLINE@

SRCINSTALLSUBDIRS = @INSTALLUTIL@ install-api @INSTALLSERVER@ @INSTALLCMDLINE@


APIALLSUBDIRS = @APIALLSUBDIRS@

APICLEANSUBDIRS = @APICLEANSUBDIRS@

APIINSTALLSUBDIRS = @APIINSTALLSUBDIRS@

APIUNINSTALLSUBDIRS = @APIUNINSTALLSUBDIRS@
