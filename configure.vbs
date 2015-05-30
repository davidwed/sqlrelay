' command line arguments
OPTCPPFLAGS="/O2"
DEBUGCPPFLAGS="/MD"
DEBUGLDFLAGS=""
for i=0 to WScript.Arguments.Count-1

	arg=Wscript.Arguments.Item(i)

	if arg="--enable-small-code" then
		OPTCPPFLAGS="/O1"
	elseif arg="--enable-debug" then
		DEBUGCPPFLAGS="/Zi /MDd /D _DEBUG"
		DEBUGLDFLAGS="/debug"
	end if
next


' version
SQLR_VERSION="0.60"

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
ORACLEVERSION="12c"
ORACLEINCLUDES="/I ""C:\Program Files\Oracle\instantclient_12_1\sdk\include"""
ORACLELIBS="/LIBPATH:""C:\Program Files\Oracle\instantclient_12_1\sdk\lib\msvc"" oci.lib"
ALLORACLE8="all-oracle8"
INSTALLORACLE8="installdll-oracle8"

' mysql
MYSQLINCLUDES="/I ""C:\Program Files\MySQL\MySQL Connector.C 6.1\include"""
MYSQLLIBS="/LIBPATH:""C:\Program Files\MySQL\MySQL Connector.C 6.1\lib"" libmysql.lib"
ALLMYSQL="all-mysql"
INSTALLMYSQL="installdll-mysql"

' postgresql
POSTGRESQLINCLUDES="/I ""C:\Program Files\PostgreSQL\9.4\include"""
POSTGRESQLLIBS="/LIBPATH:""C:\Program Files\PostgreSQL\9.4\lib"" libpq.lib"
ALLPOSTGRESQL="all-postgresql"
INSTALLPOSTGRESQL="installdll-postgresql"

' sqlite
SQLITEINCLUDES=""
SQLITELIBS=""
ALLSQLITE=""
INSTALLSQLITE=""

' sybase
SYBASEINCLUDES="/I ""C:\SAP\OCS-16_0\include"""
SYBASELIBS="/LIBPATH:""C:\SAP\OCS-16_0\lib"" libsybblk64.lib libsybct64.lib libsybcs64.lib"
ALLSYBASE="all-sybase"
INSTALLSYBASE="installdll-sybase"
if arch="80x86" then
	ALLSYBASE=""
	INSTALLSYBASE=""
end if

' odbc
ODBCINCLUDES=""
ODBCLIBS="user32.lib gdi32.lib odbc32.lib odbccp32.lib"
ALLODBC="all-odbc"
INSTALLODBC="installdll-odbc"

' db2
DB2INCLUDES="/I""C:\Program Files\IBM\SQLLIB\include"""
DB2LIBS="/LIBPATH:""C:\Program Files\IBM\SQLLIB\lib"" db2api.lib"
ALLDB2="all-db2"
INSTALLDB2="installdll-db2"

' firebid
FIREBIRDINCLUDES="/I""C:\Program Files\Firebird\Firebird_2_5\include"""
FIREBIRDLIBS="/LIBPATH:""C:\Program Files\Firebird\Firebird_2_5\lib"" fbclient_ms.lib"
ALLFIREBIRD="all-firebird"
INSTALLFIREBIRD="installdll-firebird"


' perl
PERLPREFIX="C:\Perl64"
PERLVERSION="520"
if arch="80x86" then
	PERLPREFIX="C:\Perl"
end if

' python
PYTHONPREFIX="C:\Python27"
PYTHONVERSION="27"

' ruby
RUBYPREFIX="C:\Ruby"
RUBYVERSION="2.2.0"
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

' php
PHPPREFIX="C:\PHP"

' java
JAVAPREFIX="C:\Program Files\Java\jdk1.8.0_25"

' tcl
TCLPREFIX="C:\Tcl"


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

	' connections
	content=replace(content,"@ORACLEVERSION@",ORACLEVERSION,1,-1,0)
	content=replace(content,"@ORACLEINCLUDES@",ORACLEINCLUDES,1,-1,0)
	content=replace(content,"@ORACLELIBS@",ORACLELIBS,1,-1,0)
	content=replace(content,"@ALLORACLE8@",ALLORACLE8,1,-1,0)
	content=replace(content,"@INSTALLORACLE8@",INSTALLORACLE8,1,-1,0)

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


	' write output file
	set outfile=fso.OpenTextFile(outfiles(i),2,true)
	call outfile.Write(content)
	call outfile.Close()
next
