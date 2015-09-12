' command line arguments
OPTCPPFLAGS="/O2"
DEBUGCPPFLAGS="/MD"
DEBUGLDFLAGS=""

disableoracle=false
disablemysql=false
disablepostgresql=false
disablesap=false
disableodbc=false
disabledb2=false
disablefirebird=false
disableperl=false
disablepython=false
disableruby=false
disablejava=false
disablephp=false
disabletcl=false
disablenodejs=false
disablecs=false

ORACLEPREFIX=""
MYSQLPREFIX=""
POSTGRESQLPREFIX=""
SYBASEPREFIX=""
DB2PREFIX=""
FIREBIRDPREFIX=""
PERLPREFIX=""
PERLVERSION=""
PYTHONPREFIX=""
PYTHONVERSION=""
RUBYPREFIX=""
RUBYVERSION=""
JAVAPREFIX=""
PHPPREFIX=""
TCLPREFIX=""
NODEJSPREFIX=""

for i=0 to WScript.Arguments.Count-1

	arg=Wscript.Arguments.Item(i)

	if arg="--enable-small-code" then
		OPTCPPFLAGS="/O1"
	elseif arg="--enable-debug" then
		DEBUGCPPFLAGS="/Zi /MDd /D _DEBUG"
		DEBUGLDFLAGS="/debug"
	elseif arg="--disable-oracle" then
		disableoracle=true
	elseif mid(arg,1,16)="--oracle-prefix=" then
		ORACLEPREFIX=mid(arg,17)
	elseif arg="--disable-mysql" then
		disablemysql=true
	elseif mid(arg,1,15)="--mysql-prefix=" then
		MYSQLPREFIX=mid(arg,16)
	elseif arg="--disable-postgresql" then
		disablepostgresql=true
	elseif mid(arg,1,20)="--postgresql-prefix=" then
		POSTGRESQLPREFIX=mid(arg,21)
	elseif arg="--disable-sap" then
		disablesap=true
	elseif mid(arg,1,16)="--sap-prefix=" then
		SYBASEPREFIX=mid(arg,17)
	elseif arg="--disable-odbc" then
		disableodbc=true
	elseif arg="--disable-db2" then
		disabledb2=true
	elseif mid(arg,1,13)="--db2-prefix=" then
		DB2PREFIX=mid(arg,14)
	elseif arg="--disable-firebird" then
		disablefirebird=true
	elseif mid(arg,1,18)="--firebird-prefix=" then
		FIREBIRDPREFIX=mid(arg,19)
	elseif arg="--disable-perl" then
		disableperl=true
	elseif mid(arg,1,14)="--perl-prefix=" then
		PERLPREFIX=mid(arg,15)
	elseif mid(arg,1,15)="--perl-version=" then
		PERLVERSION=mid(arg,16)
	elseif arg="--disable-python" then
		disablepython=true
	elseif mid(arg,1,16)="--python-prefix=" then
		PYTHONPREFIX=mid(arg,17)
	elseif mid(arg,1,17)="--python-version=" then
		PYTHONPREFIX=mid(arg,18)
	elseif arg="--disable-ruby" then
		disableruby=true
	elseif mid(arg,1,14)="--ruby-prefix=" then
		RUBYPREFIX=mid(arg,15)
	elseif mid(arg,1,15)="--ruby-version=" then
		RUBYVERSION=mid(arg,16)
	elseif arg="--disable-java" then
		disablejava=true
	elseif mid(arg,1,14)="--java-prefix=" then
		JAVAPREFIX=mid(arg,15)
	elseif arg="--disable-php" then
		disablephp=true
	elseif mid(arg,1,13)="--php-prefix=" then
		PHPPREFIX=mid(arg,14)
	elseif arg="--disable-tcl" then
		disabletcl=true
	elseif mid(arg,1,13)="--tcl-prefix=" then
		TCLPREFIX=mid(arg,14)
	elseif arg="--disable-nodejs" then
		disablenodejs=true
	elseif mid(arg,1,16)="--nodejs-prefix=" then
		NODEJSPREFIX=mid(arg,17)
	elseif arg="--disable-cs" then
		disablecs=true
	end if
next


' version
SQLR_VERSION="0.64"

' paths
pfix="C:\\Program Files\\Firstworks"
prefix=pfix
exec_prefix=prefix
bindir=pfix+"\\bin"
includedir=pfix+"\\include"
libdir=pfix+"\\lib"
javadir=pfix+"\\java"
libexecdir=pfix+"\\libexec\\sqlrelay"
localstatedir=pfix+"\\var"
sysconfdir=pfix+"\\etc"
mandir=pfix+"\\share\\man"
datadir=pfix+"\\share"
docdir=pfix+"\\doc\\sqlrelay"
EXAMPLEDIR=pfix+"\\doc\\sqlrelay\\examples"
tmpdir=pfix+"\\var\\sqlrelay\\tmp"
cachedir=pfix+"\\var\\sqlrelay\\cache"
debugdir=pfix+"\\var\\sqlrelay\\debug"
logdir=pfix+"\\var\\sqlrelay\\log"
initscript_prefix=""

' extension
EXE=".exe"

' create file system object
set fso=CreateObject("Scripting.FileSystemObject")


' get top_builddir
top_builddir=fso.GetAbsolutePathName(".")


' determine VC++ version and architecture
set WshShell=WScript.CreateObject("WScript.Shell")
set cmd=WshShell.exec("cl")
stdout=cmd.StdOut.ReadAll()
stderr=cmd.StdErr.ReadLine()
parts=split(stderr)
arch=parts(ubound(parts))
version=""
for i=lbound(parts) to ubound(parts)
	if parts(i)="Version" then
		version=parts(i+1)
	end if
next
parts=split(version,".")
version=parts(0)

' set some architecture-based flags
USE_32BIT_TIME_T=""
if arch="80x86" then
	USE_32BIT_TIME_T="/D _USE_32BIT_TIME_T"
end if



' determine OS Version number
set cmd=WshShell.exec("%comspec% /c ver")
stdout=cmd.StdOut.ReadAll()
stderr=cmd.StdErr.ReadLine()
hexversion=""
if instr(stdout,"Windows NT Version 4.0")>0 then
	hexversion="0x0400"
else
	parts0=split(stdout,"[")
	parts1=split(parts0(1)," ")
	parts2=split(parts1(1),"]")
	parts3=split(parts2(0),".")
	if parts3(1)="00" then
		parts3(1)="0"
	end if
	hexversion="0x0"&parts3(0)&"0"&parts3(1)
end if

' in general, we need to set WIN32WINNT to the hexversion
WINVER=""
WIN32WINDOWS=""
WIN32WINNT=hexversion

' but, for OS'es older than WinXP we have to do some special things...

' for Win2k and WinNT4, set WINVER also
if hexversion="0x0500" or hexversion="0x0400" then
	WINVER=hexversion

' for WinME, set WIN32WINDOWS and unset WIN32WINNT
elseif hexversion="0x0490" then
	WIN32WINDOWS=hexversion
	WIN32WINNT=""

' for Win98, set WIN32WINDOWS and WINVER and unset WIN32WINNT
elseif hexversion="0x0410" then
	WIN32WINDOWS=hexversion
	WINVER=hexversion
	WIN32WINNT=""

' for Win95, set WINVER and unset WIN32WINNT
elseif hexversion="0x0400" then
	WINVER=hexversion
	WIN32WINNT=""

' FIXME: not sure about WinNT3X, Win3X or below
end if

' add /D and macro name
if WINVER<>"" then
	WINVER="/D WINVER="&WINVER
end if
if WIN32WINDOWS<>"" then
	WIN32WINDOWS="/D _WIN32_WINDOWS="&WIN32WINDOWS
end if
if WIN32WINNT<>"" then
	WIN32WINNT="/D _WIN32_WINNT="&WIN32WINNT
end if



' determine config.h template...
configwindowsh="config.windows.h"



' determine SDK headers and libs... (FIXME: make this configurable)

' VS2002, VS2003 and VS2008 and up come with a platform SDK
SDKINCLUDES=""
SDKLIBS=""

' VS2005 doesn't come with an SDK and there are several that are compatible
if version=14 then

	' older SDK's have various issues

	' 5.2.3700.0 - Microsoft Platform SDK February 2003
	'SDKINCLUDES="/I""C:\Program Files\Microsoft SDK\include"""
	'SDKLIBS="/LIBPATH:""C:\Program Files\Microsoft SDK\Lib"""

	' 5.2.3790.1830.15 - Windows Server 2003 SP1 Platform SDK
	SDKINCLUDES="/I""C:\Program Files\Microsoft Platform SDK\Include"""
	SDKLIBS="/LIBPATH:""C:\Program Files\Microsoft Platform SDK\Lib"""

	' 5.2.3790.2075.51 - Windows Server 2003 R2 Platform SDK
	'SDKINCLUDES="/I""C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Include"""
	'SDKLIBS="/LIBPATH:""C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Lib"""

	' 6.0A (comes with VC2008)
	'SDKINCLUDES="/I""C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include"""
	'SDKLIBS="/LIBPATH:""C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib"""

	' not sure about newer SDK's

' VS6 doesn't come with a platform SDK
elseif version=12 then

	' older SDK's might work too

	' 5.2.3700.0 - Microsoft Platform SDK February 2003
	SDKINCLUDES="/I""C:\Program Files\Microsoft SDK\include"""
	SDKLIBS="/LIBPATH:""C:\Program Files\Microsoft SDK\Lib"""

	' not sure about newer SDK's

' VS5 and lower don't come with a platform SDK
elseif version<=11 then

	' older SDK's might work too

	' 5.1.2600.2180 - Microsoft Platform SDK for Windows XP SP2
	' (this doesn't actually work)
	'SDKINCLUDES="/I""C:\Program Files\Microsoft Platform SDK for Windows XP SP2\Include"""
	'SDKLIBS="/LIBPATH:""C:\Program Files\Microsoft Platform SDK for Windows XP SP2\Lib"""

	' newer SDK's give link errors

end if



' oracle
if ORACLEPREFIX="" then
	ORACLEPREFIX="C:\Program Files\Oracle\instantclient_12_1\sdk"
end if
ORACLEINCLUDES="/I """+ORACLEPREFIX+"\include"""
ORACLELIBS="/LIBPATH:"""+ORACLEPREFIX+"\lib\msvc"" oci.lib"
if disableoracle=false then
	ALLORACLE="all-oracle"
	INSTALLORACLE="installdll-oracle"
end if

' mysql
if MYSQLPREFIX="" then
	MYSQLPREFIX="C:\Program Files\MySQL\MySQL Connector.C 6.1"
end if
MYSQLINCLUDES="/I """+MYSQLPREFIX+"\include"""
MYSQLLIBS="/LIBPATH:"""+MYSQLPREFIX+"\lib"" libmysql.lib"
if disablemysql=false then
	ALLMYSQL="all-mysql"
	INSTALLMYSQL="installdll-mysql"
end if

' postgresql
if POSTGRESQLPREFIX="" then
	POSTGRESQLPREFIX="C:\Program Files\PostgreSQL\9.4"
end if
POSTGRESQLINCLUDES="/I """+POSTGRESQLPREFIX+"\include"""
POSTGRESQLLIBS="/LIBPATH:"""+POSTGRESQLPREFIX+"\lib"" libpq.lib"
if disablepostgresql=false then
	ALLPOSTGRESQL="all-postgresql"
	INSTALLPOSTGRESQL="installdll-postgresql"
end if

' sqlite
SQLITEINCLUDES=""
SQLITELIBS=""
ALLSQLITE=""
INSTALLSQLITE=""

' sap
if SYBASEPREFIX="" then
	SYBASEPREFIX="C:\SAP\OCS-16_0"
end if
SYBASEINCLUDES="/I """+SYBASEPREFIX+"\include"""
SYBASELIBS="/LIBPATH:"""+SYBASEPREFIX+"\lib"" libsybblk64.lib libsybct64.lib libsybcs64.lib"
if disablesap=false then
	ALLSYBASE="all-sap"
	INSTALLSYBASE="installdll-sap"
end if
if arch="80x86" then
	ALLSYBASE=""
	INSTALLSYBASE=""
end if

' odbc
ODBCINCLUDES=""
ODBCLIBS="user32.lib gdi32.lib odbc32.lib odbccp32.lib"
if disableodbc=false then
	ALLODBC="all-odbc"
	INSTALLODBC="installdll-odbc"
end if

' db2
if DB2PREFIX="" then
	DB2PREFIX="C:\Program Files\IBM\SQLLIB"
end if
DB2INCLUDES="/I"""+DB2PREFIX+"\include"""
DB2LIBS="/LIBPATH:"""+DB2PREFIX+"\lib"" db2api.lib"
if disabledb2=false then
	ALLDB2="all-db2"
	INSTALLDB2="installdll-db2"
end if

' firebid
if FIREBIRDPREFIX="" then
	FIREBIRDPREFIX="C:\Program Files\Firebird\Firebird_2_5"
end if
FIREBIRDINCLUDES="/I"""+FIREBIRDPREFIX+"\include"""
FIREBIRDLIBS="/LIBPATH:"""+FIREBIRDPREFIX+"\lib"" fbclient_ms.lib"
if disablefirebird=false then
	ALLFIREBIRD="all-firebird"
	INSTALLFIREBIRD="installdll-firebird"
end if


' api's...
APIALLSUBDIRS="all-cpp all-c all-odbc"
APICLEANSUBDIRS="clean-cpp clean-c clean-odbc"
APIINSTALLSUBDIRS="install-cpp install-c install-odbc"
APIUNINSTALLSUBDIRS="uninstall-cpp uninstall-c uninstall-odbc"

' c#
if disablecs=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-cs"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-cs"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-cs"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-cs"
end if

' perl
if PERLPREFIX="" then
	PERLPREFIX="C:\Perl64"
end if
if PERLVERSION="" then
	PERLVERSION="520"
end if
if arch="80x86" then
	PERLPREFIX="C:\Perl"
end if
if disableperl=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-perl"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-perl"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-perl"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-perl"
end if

' python
if PYTHONPREFIX="" then
	PYTHONPREFIX="C:\Python27"
end if
if PYTHONVERSION="" then
	PYTHONVERSION="27"
end if
if disablepython=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-python"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-python"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-python"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-python"
end if

' ruby
if RUBYPREFIX="" then
	RUBYPREFIX="C:\Ruby"
end if
if RUBYVERSION="" then
	RUBYVERSION="2.2.0"
end if
RUBYLIBVERSION="220"
RUBYVCVERSION="100"
RUBYTARGET="x64-mswin64"
RUBYLIBPREFIX="x64-msvcr100"
RUBYSITEARCHDIRSUFFIX="x64-msvcr100"
if arch="80x86" then
	RUBYTARGET="i386-mswin32"
	RUBYLIBPREFIX="msvcr100"
	RUBYSITEARCHDIRSUFFIX="i386-msvcr100"
end if
if disableruby=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-ruby"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-ruby"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-ruby"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-ruby"
end if

' php
if PHPPREFIX="" then
	PHPPREFIX="C:\PHP"
end if
if disablephp=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-php all-phppdo"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-php clean-phppdo"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-php install-phppdo"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-php uninstall-phppdo"
end if

' java
if JAVAPREFIX="" then
	JAVAPREFIX="C:\Program Files\Java\jdk1.8.0_25"
end if
if disablejava=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-java"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-java"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-java"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-java"
end if

' tcl
if TCLPREFIX="" then
	TCLPREFIX="C:\Tcl"
end if
if disabletcl=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-tcl"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-tcl"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-tcl"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-tcl"
end if

' node.js
if NODEJSPREFIX="" then
	NODEJSPREFIX="C:\Program Files\nodejs"
end if
NODEJSMSVSVERSION=2005
if version=15 then
	NODEJSMSVSVERSION=2008
elseif version=16 then
	NODEJSMSVSVERSION=2010
elseif version=17 then
	NODEJSMSVSVERSION=2012
elseif version=18 then
	NODEJSMSVSVERSION=2013
elseif version=19 then
	NODEJSMSVSVERSION=2015
else
	NODEJSMSVSVERSION=2000+version-4
end if
if disablenodejs=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-nodejs"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-nodejs"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-nodejs"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-nodejs"
end if



' input and output files
infiles=Array(_
	"config.windows.mk",_
	configwindowsh,_
	"bin\\sqlrclient-config.in",_
	"bin\\sqlrclientwrapper-config.in",_
	"bin\\sqlrserver-config.in",_
	"sqlrelay-c.pc.in",_
	"sqlrelay-c++.pc.in"_
	)
outfiles=Array(_
	"config.mk",_
	"config.h",_
	"bin\\sqlrclient-config",_
	"bin\\sqlrclientwrapper-config",_
	"bin\\sqlrserver-config",_
	"sqlrelay-c.pc",_
	"sqlrelay-c++.pc"_
	)


' create output files
for i=lbound(infiles) to ubound(infiles)

	' read input file
	set infile=fso.OpenTextFile(infiles(i))
	content=infile.ReadAll()

	' version
	content=replace(content,"@SQLR_VERSION@",SQLR_VERSION,1,-1,0)

	' paths
	content=replace(content,"@prefix@",prefix,1,-1,0)
	content=replace(content,"@exec_prefix@",exec_prefix,1,-1,0)
	content=replace(content,"@bindir@",bindir,1,-1,0)
	content=replace(content,"@includedir@",includedir,1,-1,0)
	content=replace(content,"@libdir@",libdir,1,-1,0)
	content=replace(content,"@javadir@",javadir,1,-1,0)
	content=replace(content,"@mandir@",mandir,1,-1,0)
	content=replace(content,"@datadir@",datadir,1,-1,0)
	content=replace(content,"@libexecdir@",libexecdir,1,-1,0)
	content=replace(content,"@localstatedir@",localstatedir,1,-1,0)
	content=replace(content,"@sysconfdir@",sysconfdir,1,-1,0)
	content=replace(content,"@docdir@",docdir,1,-1,0)
	content=replace(content,"@EXAMPLEDIR@",EXAMPLEDIR,1,-1,0)
	content=replace(content,"@tmpdir@",tmpdir,1,-1,0)
	content=replace(content,"@cachedir@",cachedir,1,-1,0)
	content=replace(content,"@debugdir@",debugdir,1,-1,0)
	content=replace(content,"@logdir@",logdir,1,-1,0)
	content=replace(content,"@initscript_prefix@",initscript_prefix,1,-1,0)

	' flags
	content=replace(content,"@OPTCPPFLAGS@",OPTCPPFLAGS,1,-1,0)
	content=replace(content,"@DEBUGCPPFLAGS@",DEBUGCPPFLAGS,1,-1,0)
	content=replace(content,"@DEBUGLDFLAGS@",DEBUGLDFLAGS,1,-1,0)
	content=replace(content,"@_USE_32BIT_TIME_T@",USE_32BIT_TIME_T,1,-1,0)
	content=replace(content,"@SDKINCLUDES@",SDKINCLUDES,1,-1,0)
	content=replace(content,"@WINVER@",WINVER,1,-1,0)
	content=replace(content,"@WIN32WINDOWS@",WIN32WINDOWS,1,-1,0)
	content=replace(content,"@WIN32WINNT@",WIN32WINNT,1,-1,0)

	' libraries
	content=replace(content,"@SDKLIBS@",SDKLIBS,1,-1,0)

	' extension
	content=replace(content,"@EXE@",EXE,1,-1,0)

	' top_builddir
	content=replace(content,"@top_builddir@",top_builddir,1,-1,0)

	' perl
	content=replace(content,"@PERLPREFIX@",PERLPREFIX,1,-1,0)
	content=replace(content,"@PERLVERSION@",PERLVERSION,1,-1,0)

	' python
	content=replace(content,"@PYTHONPREFIX@",PYTHONPREFIX,1,-1,0)
	content=replace(content,"@PYTHONVERSION@",PYTHONVERSION,1,-1,0)

	' ruby
	content=replace(content,"@RUBYPREFIX@",RUBYPREFIX,1,-1,0)
	content=replace(content,"@RUBYVERSION@",RUBYVERSION,1,-1,0)
	content=replace(content,"@RUBYLIBVERSION@",RUBYLIBVERSION,1,-1,0)
	content=replace(content,"@RUBYVCVERSION@",RUBYVCVERSION,1,-1,0)
	content=replace(content,"@RUBYTARGET@",RUBYTARGET,1,-1,0)
	content=replace(content,"@RUBYLIBPREFIX@",RUBYLIBPREFIX,1,-1,0)
	content=replace(content,"@RUBYSITEARCHDIRSUFFIX@",RUBYSITEARCHDIRSUFFIX,1,-1,0)

	' php
	content=replace(content,"@PHPPREFIX@",PHPPREFIX,1,-1,0)

	' java
	content=replace(content,"@JAVAPREFIX@",JAVAPREFIX,1,-1,0)

	' tcl
	content=replace(content,"@TCLPREFIX@",TCLPREFIX,1,-1,0)

	' nodejs
	content=replace(content,"@NODEJSPREFIX@",NODEJSPREFIX,1,-1,0)
	content=replace(content,"@NODEJSMSVSVERSION@",NODEJSMSVSVERSION,1,-1,0)

	' connections
	content=replace(content,"@ORACLEINCLUDES@",ORACLEINCLUDES,1,-1,0)
	content=replace(content,"@ORACLELIBS@",ORACLELIBS,1,-1,0)
	content=replace(content,"@ALLORACLE@",ALLORACLE,1,-1,0)
	content=replace(content,"@INSTALLORACLE@",INSTALLORACLE,1,-1,0)

	content=replace(content,"@MYSQLINCLUDES@",MYSQLINCLUDES,1,-1,0)
	content=replace(content,"@MYSQLLIBS@",MYSQLLIBS,1,-1,0)
	content=replace(content,"@ALLMYSQL@",ALLMYSQL,1,-1,0)
	content=replace(content,"@INSTALLMYSQL@",INSTALLMYSQL,1,-1,0)

	content=replace(content,"@POSTGRESQLINCLUDES@",POSTGRESQLINCLUDES,1,-1,0)
	content=replace(content,"@POSTGRESQLLIBS@",POSTGRESQLLIBS,1,-1,0)
	content=replace(content,"@ALLPOSTGRESQL@",ALLPOSTGRESQL,1,-1,0)
	content=replace(content,"@INSTALLPOSTGRESQL@",INSTALLPOSTGRESQL,1,-1,0)

	content=replace(content,"@SQLITEINCLUDES@",SQLITEINCLUDES,1,-1,0)
	content=replace(content,"@SQLITELIBS@",SQLITELIBS,1,-1,0)
	content=replace(content,"@ALLSQLITE@",ALLSQLITE,1,-1,0)
	content=replace(content,"@INSTALLSQLITE@",INSTALLSQLITE,1,-1,0)

	content=replace(content,"@SYBASEINCLUDES@",SYBASEINCLUDES,1,-1,0)
	content=replace(content,"@SYBASELIBS@",SYBASELIBS,1,-1,0)
	content=replace(content,"@ALLSYBASE@",ALLSYBASE,1,-1,0)
	content=replace(content,"@INSTALLSYBASE@",INSTALLSYBASE,1,-1,0)

	content=replace(content,"@ODBCINCLUDES@",ODBCINCLUDES,1,-1,0)
	content=replace(content,"@ODBCLIBS@",ODBCLIBS,1,-1,0)
	content=replace(content,"@ALLODBC@",ALLODBC,1,-1,0)
	content=replace(content,"@INSTALLODBC@",INSTALLODBC,1,-1,0)

	content=replace(content,"@DB2INCLUDES@",DB2INCLUDES,1,-1,0)
	content=replace(content,"@DB2LIBS@",DB2LIBS,1,-1,0)
	content=replace(content,"@ALLDB2@",ALLDB2,1,-1,0)
	content=replace(content,"@INSTALLDB2@",INSTALLDB2,1,-1,0)

	content=replace(content,"@FIREBIRDINCLUDES@",FIREBIRDINCLUDES,1,-1,0)
	content=replace(content,"@FIREBIRDLIBS@",FIREBIRDLIBS,1,-1,0)
	content=replace(content,"@ALLFIREBIRD@",ALLFIREBIRD,1,-1,0)
	content=replace(content,"@INSTALLFIREBIRD@",INSTALLFIREBIRD,1,-1,0)

	' enabled apis
	content=replace(content,"@APIALLSUBDIRS@",APIALLSUBDIRS,1,-1,0)
	content=replace(content,"@APICLEANSUBDIRS@",APICLEANSUBDIRS,1,-1,0)
	content=replace(content,"@APIINSTALLSUBDIRS@",APIINSTALLSUBDIRS,1,-1,0)
	content=replace(content,"@APIUNINSTALLSUBDIRS@",APIUNINSTALLSUBDIRS,1,-1,0)

	' write output file
	set outfile=fso.OpenTextFile(outfiles(i),2,true)
	call outfile.Write(content)
	call outfile.Close()
next
