SQLR_VERSION = @SQLR_VERSION@
SONAME_VERSION_INFO =

SQLRELAY_ENABLE_SHARED = yes

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
CPPCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENT_EXPORTS /I$(top_builddir) /I./ /I$(top_builddir)\src\common $(RUDIMENTSINCLUDES)
CPPLIBS = $(RUDIMENTSLIBS)


# c
CCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRCLIENTWRAPPER_EXPORTS /I$(top_builddir) /I./ /I$(top_builddir)\src\api\c++ $(RUDIMENTSINCLUDES)
CLIBS = /LIBPATH:$(top_builddir)\src\api\c++ libsqlrclient.lib $(RUDIMENTSLIBS)


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
PERLCPPFLAGS = $(BASECPPFLAGS) $(PERLOPTIMIZE_LOCAL) $(PERLCCFLAGS_LOCAL) /I$(top_builddir) /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES) $(PERLINC_LOCAL)
PERLCONLIBS = /LIBPATH:$(PERLLIB)\CORE perl$(PERLVERSION).lib /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)
PERLCURLIBS = /LIBPATH:$(PERLLIB)\CORE perl$(PERLVERSION).lib /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)
PERLINSTALLMAN = installman


# python
PYTHONPREFIX = @PYTHONPREFIX@
PYTHONVERSION = @PYTHONVERSION@
PYTHONINCLUDES = /I$(PYTHONPREFIX)\include
PYTHONDIR = $(PYTHONPREFIX)\Lib
PYTHONLIB = /LIBPATH:$(PYTHONPREFIX)\libs python$(PYTHONVERSION).lib
PYTHONCPPFLAGS = /D HAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
PYTHONLIBS = $(PYTHONLIB) /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


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
RUBYLIBS = $(RUBYLIB) /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


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
PHPLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)
PHPPDOCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir) /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES) $(PHPINCLUDES) /D COMPILE_DL=1
PHPPDOLIBS = $(PHPLIB) /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# java
JAVAPREFIX = @JAVAPREFIX@
JAVAC = "$(JAVAPREFIX)\bin\javac"
JAR = "$(JAVAPREFIX)\bin\jar"
JAVAINCLUDES = /I "$(JAVAPREFIX)\include" /I "$(JAVAPREFIX)\include\win32"
JAVACPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c /I$(top_builddir)\src\api\c++ $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
JAVALIBS = /LIBPATH:$(top_builddir)\src\api\c++ libsqlrclient.lib $(RUDIMENTSLIBS) /LIBPATH:"$(JAVAPREFIX)\lib" jvm.lib


# tcl
TCLPREFIX = @TCLPREFIX@
TCLINCLUDE = /I$(TCLPREFIX)\include
TCLLIB = /LIBPATH:$(TCLPREFIX)\lib tcl86.lib
TCLLIBSPATH = $(TCLPREFIX)\lib
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
ORACLEVERSION = @ORACLEVERSION@
ORACLEINCLUDES = @ORACLEINCLUDES@
ORACLELIBS = @ORACLELIBS@


# mysql
MYSQLINCLUDES = @MYSQLINCLUDES@
MYSQLLIBS = @MYSQLLIBS@
MYSQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
MYSQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# postgresql
POSTGRESQLINCLUDES = @POSTGRESQLINCLUDES@
POSTGRESQLLIBS = @POSTGRESQLLIBS@
POSTGRESQLDRLIBCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
POSTGRESQLDRLIBLIBS = /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# sqlite
SQLITEINCLUDES = @SQLITEINCLUDES@
SQLITELIBS = @SQLITELIBS@


# sybase
SYBASEINCLUDES = @SYBASEINCLUDES@
SYBASELIBS = @SYBASELIBS@


# odbc
ODBCINCLUDES = @ODBCINCLUDES@
ODBCLIBS = @ODBCLIBS@
ODBCUNICODE =
ODBCDRIVERCPPFLAGS = $(BASECPPFLAGS) /D LIBSQLRODBC_EXPORTS /I$(top_builddir) /I$(top_builddir)\src\common /I$(top_builddir)\src\api\c++ $(RUDIMENTSINCLUDES) $(ODBCINCLUDES)
ODBCDRIVERLIBS = /LIBPATH:$(top_builddir)\src\api\c++ libsqlrclient.lib $(RUDIMENTSLIBS) $(ODBCLIBS) /DEF:sqlrodbc.def


# db2
DB2INCLUDES = @DB2INCLUDES@
DB2LIBS = @DB2LIBS@


# firebird
FIREBIRDINCLUDES = @FIREBIRDINCLUDES@
FIREBIRDLIBS = @FIREBIRDLIBS@


# util
UTILCPPFLAGS = $(BASECPPFLAGS) /D PREFIX="\"@prefix@\"" /D LIBSQLRUTIL_EXPORTS /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common $(RUDIMENTSINCLUDES)
UTILLIBS = $(RUDIMENTSLIBS) advapi32.lib


# cmdline
CMDLINECPPFLAGS = $(BASECPPFLAGS) /D SYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/util /I$(top_builddir)/src/server /I$(top_builddir)/src/api/c++ $(RUDIMENTSINCLUDES)
CMDLINELIBS = /LIBPATH:$(top_builddir)/src/util libsqlrutil.lib /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)
PWDENCLIBS = /LIBPATH:$(top_builddir)/src/server libsqlrserver.lib /LIBPATH:$(top_builddir)/src/util libsqlrutil.lib /LIBPATH:$(top_builddir)/src/api/c++ libsqlrclient.lib $(RUDIMENTSLIBS)


# server
STATICPLUGINSRCS = 
STATICPLUGINOBJS =
STATICPLUGINLIBS =

SERVERCPPFLAGS = $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/common /I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) /D LIBSQLRSERVER_EXPORTS
LIBSQLRSERVERLIBS = /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib
SERVERLIBS = /LIBPATH:./ libsqlrserver.lib $(STATICPLUGINLIBS) /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)


# plugins
PLUGINCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir) /I$(top_builddir)\src\util /I$(top_builddir)\src\server /I$(top_builddir)\src\common $(RUDIMENTSINCLUDES) /D LIBSQLRSERVER_EXPORTS
PLUGINLIBS = /LIBPATH:$(top_builddir)\src\server libsqlrserver.lib /LIBPATH:$(top_builddir)\src\util libsqlrutil.lib $(RUDIMENTSLIBS) $(EXTRALIBS)

AUTHCPPFLAGS = $(PLUGINCPPFLAGS) /I$(top_builddir)\src\api\c++
SQLRAUTH_SQLRELAYLIBS = /LIBPATH:$(top_builddir)\src\api\c++ libsqlrclient.lib

LOGGERCPPFLAGS = $(PLUGINCPPFLAGS)

INSTALLSHAREDLIB =


# connections
CONNECTIONCPPFLAGS = $(BASECPPFLAGS) /I$(top_builddir)\ /I$(top_builddir)\src\common /I$(top_builddir)\src\util /I$(top_builddir)\src\server $(RUDIMENTSINCLUDES) /D LIBSQLRSERVER_EXPORTS
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
CONNECTIONSALLTARGETS = @ALLDB2@ @ALLFIREBIRD@ @ALLMYSQL@ @ALLODBC@ @ALLORACLE8@ @ALLPOSTGRESQL@ @ALLSQLITE@ @ALLSYBASE@ all-router
CONNECTIONSINSTALLTARGETS = @INSTALLDB2@ @INSTALLFIREBIRD@ @INSTALLMYSQL@ @INSTALLODBC@ @INSTALLORACLE8@ @INSTALLPOSTGRESQL@ @INSTALLSQLITE@ @INSTALLSYBASE@ installdll-router


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

STRESSCPPFLAGS = $(BASECPPFLAGS) /I $(includedir) $(RUDIMENTSINCLUDES)
STRESSLIBS = /LIBPATH:$(libdir) libsqlrclient.lib $(RUDIMENTSLIBS)


# microsoft-specific
EXE = .exe


# shared object and module
SOSUFFIX = dll
MODULESUFFIX = dll
JNISUFFIX = dll
PYTHONSUFFIX = pyd


# build directories
INSTALLSUBDIRS = install-reg install-src install-bin install-etc install-doc
UNINSTALLSUBDIRS = uninstall-src uninstall-bin uninstall-etc uninstall-doc uninstall-reg

SRCALLSUBDIRS = all-util all-api all-server all-parsers all-queries all-loggers all-protocols all-pwdencs all-auths all-connections all-cmdline

SRCINSTALLSUBDIRS = install-util install-api install-server install-parsers install-queries install-loggers install-protocols install-pwdencs install-auths install-connections install-cmdline


APIALLSUBDIRS = all-cpp all-c all-cs all-odbc all-python all-perl all-ruby all-php all-phppdo all-java all-tcl

APICLEANSUBDIRS = clean-cpp clean-c clean-cs clean-odbc clean-python clean-perl clean-ruby clean-php clean-phppdo clean-java clean-tcl

APIINSTALLSUBDIRS = install-cpp install-c install-cs install-odbc install-python install-perl install-ruby install-php install-phppdo install-java install-tcl

APIUNINSTALLSUBDIRS = uninstall-cpp uninstall-c uninstall-cs uninstall-odbc uninstall-python uninstall-perl uninstall-ruby uninstall-php uninstall-phppdo uninstall-java uninstall-tcl
