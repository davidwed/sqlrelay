on error resume next

' run with cscript
If InStr(LCase(WScript.FullName),"cscript")=0 Then
	WScript.Echo("Please run:  cscript /nologo configure.vbs")
	WScript.Quit
End If

' command line arguments
OPTCPPFLAGS="/O2"
DEBUGCPPFLAGS="/MD"
DEBUGLDFLAGS=""
hexversion=""

disableutil=false
disableserver=false
disableoracle=false
disablemysql=false
disablepostgresql=false
disablesap=false
disableodbc=false
disabledb2=false
disablefirebird=false
disableinformix=false
disablerouter=false
disableodbcdriver=false
disablecpp=false
disableperl=false
disablepython=false
disableruby=false
disablejava=false
disablephp=false
disabletcl=false
disablenodejs=false
disablecs=false
disablecmdline=false
disabledoc=false

ORACLEPREFIX=""
MYSQLPREFIX=""
POSTGRESQLPREFIX=""
SYBASEPREFIX=""
DB2PREFIX=""
FIREBIRDPREFIX=""
INFORMIXPREFIX=""
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
SQLR="sqlr"
SQLRELAY="sqlrelay"
SQL_RELAY="SQL Relay"
ABS_MAXCONNECTIONS="4096"

if WScript.Arguments.Count>0 then
	if Wscript.Arguments.Item(0)="--help" then
		WScript.Echo("Usage: cscript /nologo configure.vbs [OPTION]...")
		WScript.Echo("")
		WScript.Echo("Optional Features:")
		WScript.Echo("  --enable-small-code    optimize for small code size")
		WScript.Echo("  --enable-debug         compile with debug option")
		WScript.Echo("  --disable-oracle       Don't build Oracle connection module")
		WScript.Echo("  --disable-mysql        Don't build MySQL connection module")
		WScript.Echo("  --disable-postgresql   Don't build PostgreSQL connection module")
		WScript.Echo("  --disable-sap          Don't build SAP/Sybase connection module")
		WScript.Echo("  --disable-odbc         Don't build ODBC connection module")
		WScript.Echo("  --disable-db2          Don't build DB2 connection module")
		WScript.Echo("  --disable-firebird     Don't build Firebird connection module")
		WScript.Echo("  --disable-informix     Don't build Informix connection module")
		WScript.Echo("  --disable-router       Don't build router connection module")
		WScript.Echo("  --disable-odbc-driver  Don't build ODBC driver")
		WScript.Echo("  --disable-perl         Don't build Perl api")
		WScript.Echo("  --disable-python       Don't build Python api")
		WScript.Echo("  --disable-ruby         Don't build Ruby api")
		WScript.Echo("  --disable-java         Don't build Java api")
		WScript.Echo("  --disable-php          Don't build PHP api")
		WScript.Echo("  --disable-tcl          Don't build TCL api")
		WScript.Echo("  --disable-nodejs       Don't build node.js api")
		WScript.Echo("  --disable-cs           Don't build C# api")
		WScript.Echo("")
		WScript.Echo("Optional Packages:")
		WScript.Echo("  --with-oracle-prefix      Location of Oracle")
		WScript.Echo("  --with-mysql-prefix       Location of MySQL")
		WScript.Echo("  --with-postgresql-prefix  Location of PostgreSQL")
		WScript.Echo("  --with-sap-prefix         Location of SAP/Sybase")
		WScript.Echo("  --with-db2-prefix         Location of DB2")
		WScript.Echo("  --with-firebird-prefix    Location of Firebird")
		WScript.Echo("  --with-informix-prefix    Location of Informix")
		WScript.Echo("  --with-perl-prefix        Location of Perl")
		WScript.Echo("  --with-perl-version       Perl version")
		WScript.Echo("  --with-python-prefix      Location of Perl")
		WScript.Echo("  --with-python-version     Python version")
		WScript.Echo("  --with-ruby-prefix        Location of Perl")
		WScript.Echo("  --with-ruby-version       Ruby version")
		WScript.Echo("  --with-java-prefix        Location of Perl")
		WScript.Echo("  --with-php-prefix         Location of Perl")
		WScript.Echo("  --with-tcl-prefix         Location of Perl")
		WScript.Echo("  --with-nodejs-prefix      Location of Perl")
		WScript.Echo("  --with-sqlr               replacement for ""sqlr""")
		WScript.Echo("  --with-sqlrelay           replacement for ""sqlrelay""")
		WScript.Echo("  --with-sql-relay          replacement for ""SQL Relay""")
		WScript.Quit
	end if
end if

for i=0 to WScript.Arguments.Count-1

	arg=Wscript.Arguments.Item(i)

	if arg="--enable-small-code" then
		OPTCPPFLAGS="/O1"
	elseif arg="--enable-debug" then
		DEBUGCPPFLAGS="/Zi /MDd /D _DEBUG"
		DEBUGLDFLAGS="/debug"
	elseif arg="--disable-oracle" then
		disableoracle=true
	elseif mid(arg,1,21)="--with-oracle-prefix=" then
		ORACLEPREFIX=mid(arg,22)
	elseif arg="--disable-mysql" then
		disablemysql=true
	elseif mid(arg,1,20)="--with-mysql-prefix=" then
		MYSQLPREFIX=mid(arg,21)
	elseif arg="--disable-postgresql" then
		disablepostgresql=true
	elseif mid(arg,1,25)="--with-postgresql-prefix=" then
		POSTGRESQLPREFIX=mid(arg,26)
	elseif arg="--disable-sap" then
		disablesap=true
	elseif mid(arg,1,18)="--with-sap-prefix=" then
		SYBASEPREFIX=mid(arg,19)
	elseif arg="--disable-odbc" then
		disableodbc=true
	elseif arg="--disable-db2" then
		disabledb2=true
	elseif mid(arg,1,18)="--with-db2-prefix=" then
		DB2PREFIX=mid(arg,19)
	elseif arg="--disable-firebird" then
		disablefirebird=true
	elseif mid(arg,1,23)="--with-firebird-prefix=" then
		FIREBIRDPREFIX=mid(arg,24)
	elseif arg="--disable-informix" then
		disableinformix=true
	elseif mid(arg,1,23)="--with-informix-prefix=" then
		INFORMIXPREFIX=mid(arg,24)
	elseif arg="--disable-router" then
		disablerouter=true
	elseif arg="--disable-odbc-driver" then
		disableodbcdriver=true
	elseif arg="--disable-perl" then
		disableperl=true
	elseif mid(arg,1,19)="--with-perl-prefix=" then
		PERLPREFIX=mid(arg,20)
	elseif mid(arg,1,20)="--with-perl-version=" then
		PERLVERSION=mid(arg,21)
	elseif arg="--disable-python" then
		disablepython=true
	elseif mid(arg,1,21)="--with-python-prefix=" then
		PYTHONPREFIX=mid(arg,22)
	elseif mid(arg,1,22)="--with-python-version=" then
		PYTHONVERSION=mid(arg,23)
	elseif arg="--disable-ruby" then
		disableruby=true
	elseif mid(arg,1,19)="--with-ruby-prefix=" then
		RUBYPREFIX=mid(arg,20)
	elseif mid(arg,1,20)="--with-ruby-version=" then
		RUBYVERSION=mid(arg,21)
	elseif arg="--disable-java" then
		disablejava=true
	elseif mid(arg,1,19)="--with-java-prefix=" then
		JAVAPREFIX=mid(arg,20)
	elseif arg="--disable-php" then
		disablephp=true
	elseif mid(arg,1,18)="--with-php-prefix=" then
		PHPPREFIX=mid(arg,19)
	elseif arg="--disable-tcl" then
		disabletcl=true
	elseif mid(arg,1,18)="--with-tcl-prefix=" then
		TCLPREFIX=mid(arg,19)
	elseif arg="--disable-nodejs" then
		disablenodejs=true
	elseif mid(arg,1,21)="--with-nodejs-prefix=" then
		NODEJSPREFIX=mid(arg,22)
	elseif arg="--disable-cs" then
		disablecs=true
	elseif mid(arg,1,12)="--with-sqlr=" then
		SQLR=mid(arg,13)
	elseif mid(arg,1,16)="--with-sqlrelay=" then
		SQLRELAY=mid(arg,17)
	elseif mid(arg,1,17)="--with-sql-relay=" then
		SQL_RELAY=mid(arg,18)
	elseif mid(arg,1,16)="--disable-server" then
		disableserver=true
		disableoracle=true
		disablemysql=true
		disablepostgresql=true
		disablesap=true
		disableodbc=true
		disabledb2=true
		disablefirebird=true
		disableinformix=true
		disablerouter=true
	elseif mid(arg,1,16)="--disable-client" then
		disablecmdline=true
		disableodbcdriver=true
		disablecpp=true
		disableperl=true
		disablepython=true
		disableruby=true
		disablejava=true
		disablephp=true
		disabletcl=true
		disablecs=true
		disablenodejs=true
		disablerouter=true
	elseif mid(arg,1,17)="--disable-cmdline" then
		disablecmdline=true
	elseif mid(arg,1,13)="--disable-doc" then
		disabledoc=true
	elseif mid(arg,1,23)="--with-windows-version=" then
		hexversion=mid(arg,24)
	end if
next

if disableserver=true and disablecmdline=true then
	disableutil=true
end if


' version
SQLR_VERSION="1.6.0"

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

' create shell object
set WshShell=WScript.CreateObject("WScript.Shell")


' get top_builddir
top_builddir=chr(34) & fso.GetAbsolutePathName(".") & chr(34)


WScript.Echo("")
WScript.Echo("***** Platform ***************")

' determine VC++ version and architecture
set cmd=WshShell.exec("cl")
stdout=cmd.StdOut.ReadAll()
stderr=cmd.StdErr.ReadLine()
parts=split(stderr)
arch=parts(ubound(parts))
if arch="80x86" then
	arch="x86"
end if
version=""
for i=lbound(parts) to ubound(parts)
	if parts(i)="Version" then
		version=parts(i+1)
	end if
next
parts=split(version,".")
version=parts(0)

WScript.Echo("Visual C++ Version: " & version)
WScript.Echo("Visual C++ Architecture: " & arch)


' set some architecture-based flags
USE_32BIT_TIME_T=""
if arch="x86" then
	USE_32BIT_TIME_T="/D _USE_32BIT_TIME_T"
end if


' determine OS Version number
if len(hexversion)=0 then
	set cmd=WshShell.exec("%comspec% /c ver")
	stdout=cmd.StdOut.ReadAll()
	stderr=cmd.StdErr.ReadLine()
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
end if

WScript.Echo("Windows Version: " & hexversion)

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
configwindowsh="config_windows.h"



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

WScript.Echo("******************************")


' util
ALLUTIL=""
INSTALLUTIL=""
if disableutil=false then
	ALLUTIL="all-util"
	INSTALLUTIL="install-util"
end if


' server
ALLSERVER=""
INSTALLSERVER=""
if disableserver=false then
	ALLSERVER="all-server all-configs all-parsers all-queries all-loggers all-notifications all-schedules all-routers all-protocols all-pwdencs all-auths all-directives all-translations all-resultsettranslations all-resultsetrowtranslations all-resultsetrowblocktranslations all-filters all-triggers all-connections"
	INSTALLSERVER="install-server install-configs install-parsers install-queries install-loggers install-notifications install-schedules install-routers install-protocols install-pwdencs install-auths install-directives install-translations install-resultsettranslations install-resultsetrowtranslations install-resultsetrowblocktranslations install-filters install-triggers install-connections"
end if



' oracle
WScript.Echo("")
WScript.Echo("***** Oracle *****************")

configureDatabase "Oracle","oracle",disableoracle,_
			"C:\Program Files\Oracle","instantclient_",_
			"sdk\include","oci.h",_
			"sdk\lib\msvc","oci.lib","",_
			"\include","\lib\msvc","oci.lib",_
			ORACLEPREFIX,ORACLEINCLUDES,ORACLELIBS,_
			ALLORACLE,INSTALLORACLE

ORACLEVERSION

WScript.Echo("******************************")



' mysql
WScript.Echo("")
WScript.Echo("***** MySQL ******************")

'"MySQL Connector.C ",_
configureDatabase "MySQL","mysql",disablemysql,_
			"C:\Program Files\MySQL",_
			"MySQL Connector C ",_
			"include","mysql.h",_
			"lib","libmysql.lib","",_
			"\include","\lib","libmysql.lib",_
			MYSQLPREFIX,MYSQLINCLUDES,MYSQLLIBS,_
			ALLMYSQL,INSTALLMYSQL

WScript.Echo("******************************")


' postgresql
WScript.Echo("")
WScript.Echo("***** PostgreSQL *************")

configureDatabase "PostgreSQL","postgresql",disablepostgresql,_
			"C:\Program Files\PostgreSQL","",_
			"include","libpq-fe.h",_
			"lib","libpq.lib","",_
			"\include","\lib","libpq.lib",_
			POSTGRESQLPREFIX,POSTGRESQLINCLUDES,POSTGRESQLLIBS,_
			ALLPOSTGRESQL,INSTALLPOSTGRESQL

WScript.Echo("******************************")


' sqlite
SQLITEINCLUDES=""
SQLITELIBS=""
ALLSQLITE=""
INSTALLSQLITE=""


' sap
WScript.Echo("")
WScript.Echo("***** SAP/Sybase *************")

configureDatabase "SAP/SYBASE","sap",disablesap,_
			"C:\SAP","OCS-",_
			"include","ctpublic.h",_
			"lib","libsybct64.lib",_
			"libsybblk64.lib libsybcs64.lib",_
			"\include","\lib","libsybct64.lib",_
			SYBASEPREFIX,SYBASEINCLUDES,SYBASELIBS,_
			ALLSYBASE,INSTALLSYBASE

WScript.Echo("******************************")


' odbc
WScript.Echo("")
WScript.Echo("***** ODBC *******************")
ODBCINCLUDES=""
ODBCLIBS="user32.lib gdi32.lib odbc32.lib odbccp32.lib"

' VS2015 requires legacy_stdio_definitions.lib or
' __vsnwprintf_s will be unresolved
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
if version=19 then
	ODBCLIBS=ODBCLIBS+" legacy_stdio_definitions.lib"
end if

if disableodbc=false then
	ALLODBC="all-odbc"
	INSTALLODBC="installdll-odbc"

	WScript.Echo("ODBC includes... " & ODBCINCLUDES)
	WScript.Echo("ODBC libs... " & ODBCLIBS)
else
	WScript.Echo("ODBC support will not be built. ")
end if
WScript.Echo("******************************")


' db2
WScript.Echo("")
WScript.Echo("***** DB2 ********************")

configureDatabase "DB2","db2",disabledb2,_
			"C:\Program Files\IBM","SQLLIB",_
			"include","sqlcli1.h",_
			"lib","db2api.lib",_
			"",_
			"\include","\lib","db2api.lib",_
			DB2PREFIX,DB2INCLUDES,DB2LIBS,_
			ALLDB2,INSTALLDB2

WScript.Echo("******************************")


' firebid
WScript.Echo("")
WScript.Echo("***** Firebird ***************")

configureDatabase "Firebird","firebird",disablefirebird,_
			"C:\Program Files\Firebird","Firebird_",_
			"include","ibase.h",_
			"lib","fbclient_ms.lib",_
			"",_
			"\include","\lib","fbclient_ms.lib",_
			FIREBIRDPREFIX,FIREBIRDINCLUDES,FIREBIRDLIBS,_
			ALLFIREBIRD,INSTALLFIREBIRD

WScript.Echo("******************************")


' informix
WScript.Echo("")
WScript.Echo("***** Informix ***************")

configureDatabase "Informix","informix",disableinformix,_
			"C:\Program Files","IBM Informix Software Bundle",_
			"incl\cli","infxcli.h",_
			"lib","iclit",_
			"",_
			"\incl\cli","\lib","iclit09b.lib",_
			INFORMIXPREFIX,INFORMIXINCLUDES,INFORMIXLIBS,_
			ALLINFORMIX,INSTALLINFORMIX

WScript.Echo("******************************")


' router
ALLROUTER=""
INSTALLROUTER=""
if disablerouter=false then
	ALLROUTER="all-router"
	INSTALLROUTER="installdll-router"
end if


' cmdline programs
ALLCMDLINE=""
INSTALLCMDLINE=""
if disablecmdline=false then
	ALLCMDLINE="all-cmdline"
	INSTALLCMDLINE="install-cmdline"
end if


' api's...
APIALLSUBDIRS=""
APICLEANSUBDIRS=""
APIINSTALLSUBDIRS=""
APIUNINSTALLSUBDIRS=""
if disablecpp=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-cpp all-c"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-cpp clean-c"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-cpp install-c"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+" uninstall-cpp uninstall-c"
end if
if disableodbcdriver=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-odbc"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-odbc"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-odbc"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+" uninstall-odbc"
end if


' c#
WScript.Echo("")
WScript.Echo("***** C# *********************")
if disablecs=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-cs"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-cs"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-cs"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-cs"
else
	WScript.Echo("C# support will not be built. ")
end if
WScript.Echo("******************************")


' perl
WScript.Echo("")
WScript.Echo("***** Perl *******************")

if PERLPREFIX="" then
	findPrefix "C:\","Perl",PERLPREFIX,disableperl
end if

if PERLPREFIX<>"" and PERLVERSION="" then
	findVersion PERLPREFIX & "\lib\CORE","perl",".lib",PERLVERSION
end if

if disableperl=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-perl"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-perl"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-perl"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-perl"

	WScript.Echo("Perl prefix... " & PERLPREFIX)
	WScript.Echo("Perl version... " & PERLVERSION)
else
	WScript.Echo("Perl support will not be built. ")
end if
WScript.Echo("******************************")


' python
WScript.Echo("")
WScript.Echo("***** Python *****************")

if PYTHONPREFIX="" then
	findPrefix "C:\","Python",PYTHONPREFIX,disablepython
end if

if PYTHONPREFIX<>"" and PYTHONVERSION="" then
	findVersion PYTHONPREFIX & "\libs","python",".lib",PYTHONVERSION
end if

if PYTHONVERSION<30 then
	IMPORTEXCEPTIONS="import exceptions"
	EXCEPTIONSSTANDARDERROR="exceptions.StandardError"
else
	IMPORTEXCEPTIONS=""
	EXCEPTIONSSTANDARDERROR="Exception"
end if

if disablepython=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-python"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-python"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-python"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-python"

	WScript.Echo("Python prefix... " & PYTHONPREFIX)
	WScript.Echo("Python version... " & PYTHONVERSION)
else
	WScript.Echo("Python support will not be built. ")
end if
WScript.Echo("******************************")


' ruby
WScript.Echo("")
WScript.Echo("***** Ruby *******************")

if RUBYPREFIX="" then
	findPrefix "C:\","Ruby",RUBYPREFIX,disableruby
end if

if RUBYPREFIX<>"" and RUBYVERSION="" then
	findVersion RUBYPREFIX & "\include","ruby-","",RUBYVERSION

	RUBYLIBVERSION=RUBYVERSION
	while InStr(RUBYLIBVERSION,".")>0
		RUBYLIBVERSION=Replace(RUBYLIBVERSION,".","")
	wend
end if

if RUBYPREFIX<>"" and RUBYVERSION<>"" then
	if arch="x86" then
		RUBYTARGET="i386-mswin32"
		findVersion RUBYPREFIX & "\lib","msvcr","-ruby" & RUBYLIBVERSION & ".lib",RUBYVCVERSION
		RUBYLIBPREFIX="msvcr" & RUBYVCVERSION
		RUBYSITEARCHDIRSUFFIX="i386-msvcr" & RUBYVCVERSION
	else
		RUBYTARGET="x64-mswin64"
		findVersion RUBYPREFIX & "\lib","x64-msvcr","-ruby" & RUBYLIBVERSION & ".lib",RUBYVCVERSION
		RUBYLIBPREFIX="x64-msvcr" & RUBYVCVERSION
		RUBYSITEARCHDIRSUFFIX="x64-msvcr" & RUBYVCVERSION
	end if
end if

if disableruby=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-ruby"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-ruby"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-ruby"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-ruby"

	WScript.Echo("Ruby prefix... " & RUBYPREFIX)
	WScript.Echo("Ruby version... " & RUBYVERSION)
else
	WScript.Echo("Ruby support will not be built. ")
end if
WScript.Echo("******************************")


' php
WScript.Echo("")
WScript.Echo("***** PHP ********************")

if PHPPREFIX="" then
	findPrefix "C:\","PHP",PHPPREFIX,disablephp
end if

if disablephp=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-php all-phppdo"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-php clean-phppdo"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-php install-phppdo"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-php uninstall-phppdo"

	WScript.Echo("PHP prefix... " & PHPPREFIX)
else
	WScript.Echo("PHP support will not be built. ")
end if
WScript.Echo("******************************")


' java
WScript.Echo("")
WScript.Echo("***** Java *******************")

if JAVAPREFIX="" then
	findPrefix "C:\Program Files\Java\","jdk",JAVAPREFIX,disablejava
end if

if disablejava=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-java all-jdbc"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-java clean-jdbc"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-java install-jdbc"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-java uninstall-jdbc"

	WScript.Echo("Java prefix... " & JAVAPREFIX)
else
	WScript.Echo("Java support will not be built. ")
end if
WScript.Echo("******************************")


' tcl
WScript.Echo("")
WScript.Echo("***** TCL ********************")

if TCLPREFIX="" then
	findPrefix "C:\","Tcl",TCLPREFIX,disabletcl
end if

if disabletcl=false then
	APIALLSUBDIRS=APIALLSUBDIRS+" all-tcl"
	APICLEANSUBDIRS=APICLEANSUBDIRS+" clean-tcl"
	APIINSTALLSUBDIRS=APIINSTALLSUBDIRS+" install-tcl"
	APIUNINSTALLSUBDIRS=APIUNINSTALLSUBDIRS+ " uninstall-tcl"

	WScript.Echo("TCL prefix... " & TCLPREFIX)
else
	WScript.Echo("TCL support will not be built. ")
end if
WScript.Echo("******************************")


' node.js
WScript.Echo("")
WScript.Echo("***** node.js ****************")

if NODEJSPREFIX="" then
	findPrefix "C:\Program Files\","nodejs",NODEJSPREFIX,disablenodejs
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

	WScript.Echo("node.js prefix... " & NODEJSPREFIX)
else
	WScript.Echo("node.js support will not be built. ")
end if
WScript.Echo("******************************")


' docs
INSTALLDOC=""
if disabledoc=false then
	INSTALLDOC="install-doc"
end if

TESTDBS=""
TESTAPIS=""

CMDLINEBUILD="no "
CPPBUILD="no "
PERLBUILD="no "
PYTHONBUILD="no "
RUBYBUILD="no "
PHPBUILD="no "
PHPPDOBUILD="no "
ODBCDRIVERBUILD="no "
JAVABUILD="no "
TCLBUILD="no "
CSBUILD="no "
NODEJSBUILD="no "
ODBCBUILD="no "
if disablecmdline=false then
	CMDLINEBUILD="yes"
end if
if disablecpp=false then
	CPPBUILD="yes"
	TESTAPIS=TESTAPIS & """c"",""c++"","
end if
if disableperl=false then
	PERLBUILD="yes"
	TESTAPIS=TESTAPIS & """perl"",""perldbi"","
end if
if disablepython=false then
	PYTHONBUILD="yes"
	TESTAPIS=TESTAPIS & """python"",""pythondb"","
end if
if disableruby=false then
	RUBYBUILD="yes"
	TESTAPIS=TESTAPIS & """ruby"","
end if
if disablephp=false then
	PHPBUILD="yes"
	TESTAPIS=TESTAPIS & """php"","
end if
if disablephp=false then
	PHPPDOBUILD="yes"
	TESTAPIS=TESTAPIS & """phppdo"","
end if
if disableodbcdriver=false then
	ODBCDRIVERBUILD="yes"
end if
if disablejava=false then
	JAVABUILD="yes"
	TESTAPIS=TESTAPIS & """java"","
end if
if disabletcl=false then
	TCLBUILD="yes"
	TESTAPIS=TESTAPIS & """tcl"","
end if
if disablecs=false then
	CSBUILD="yes"
	TESTAPIS=TESTAPIS & """cs"","
end if
if disablenodejs=false then
	NODEJSBUILD="yes"
	TESTAPIS=TESTAPIS & """nodejs"","
end if
if disableodbc=false then
	ODBCBUILD="yes"
end if
ORACLE8BUILD="no     "
MYSQLBUILD="no     "
POSTGRESQLBUILD="no     "
FREETDSBUILD="no     "
SYBASEBUILD="no     "
ODBCBUILD="no     "
DB2BUILD="no     "
FIREBIRDBUILD="no     "
MDBTOOLSBUILD="no     "
INFORMIXBUILD="no     "
ROUTERBUILD="no     "
if disableoracle=false then
	ORACLE8BUILD="yes    "
	TESTDBS=TESTDBs&"""oracle"","
end if
if disablemysql=false then
	MYSQLBUILD="yes    "
	TESTDBS=TESTDBs&"""mysql"","
end if
if disablepostgresql=false then
	POSTGRESQLBUILD="yes    "
	TESTDBS=TESTDBs&"""postgresql"","
end if
if disablesap=false then
	SYBASEBUILD="yes    "
	TESTDBS=TESTDBs&"""sap"","
end if
if disableodbc=false then
	ODBCBUILD="yes    "
end if
if disabledb2=false then
	DB2BUILD="yes    "
	TESTDBS=TESTDBs&"""db2"","
end if
if disablefirebird=false then
	FIREBIRDBUILD="yes    "
	TESTDBS=TESTDBs&"""firebird"","
end if
if disableinformix=false then
	INFORMIXBUILD="yes    "
	TESTDBS=TESTDBs&"""informix"","
end if
if disablerouter=false then
	ROUTERBUILD="yes    "
	TESTDBS=TESTDBs&"""router"","
end if
if disablemysql=false then
	TESTDBS=TESTDBs&"""mysqlprotocol"","
end if

' truncate the trailing comma
TESTDBS=left(TESTDBS,len(TESTDBS)-1)
TESTAPIS=left(TESTAPIS,len(TESTAPIS)-1)

' input and output files
infiles=Array(_
	"config_windows.mk",_
	configwindowsh,_
	"src\\common\\defines.h.in",_
	"src\\server\\sqlrelay\\private\\sqlrshm.h.in",_
	"bin\\sqlrclient-config.in",_
	"bin\\sqlrclientwrapper-config.in",_
	"bin\\sqlrserver-config.in",_
	"test\\testall.vbs.in",_
	"sqlrelay-c.pc.in",_
	"sqlrelay-c++.pc.in",_
	"msvc\\setupx64\\setupx64.vdproj.in",_
	"msvc\\setupx86\\setupx86.vdproj.in"_
	)
outfiles=Array(_
	"config.mk",_
	"config.h",_
	"src\\common\\defines.h",_
	"src\\server\\sqlrelay\\private\\sqlrshm.h",_
	"bin\\sqlrclient-config",_
	"bin\\sqlrclientwrapper-config",_
	"bin\\sqlrserver-config",_
	"test\\testall.vbs",_
	"sqlrelay-c.pc",_
	"sqlrelay-c++.pc",_
	"msvc\\setupx64\\setupx64.vdproj",_
	"msvc\\setupx86\\setupx86.vdproj"_
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
	content=replace(content,"@IMPORTEXCEPTIONS@",IMPORTEXCEPTIONS,1,-1,0)
	content=replace(content,"@EXCEPTIONSSTANDARDERROR@",EXCEPTIONSSTANDARDERROR,1,-1,0)

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

	' util library
	content=replace(content,"@ALLUTIL@",ALLUTIL,1,-1,0)
	content=replace(content,"@INSTALLUTIL@",INSTALLUTIL,1,-1,0)

	' server programs
	content=replace(content,"@ALLSERVER@",ALLSERVER,1,-1,0)
	content=replace(content,"@INSTALLSERVER@",INSTALLSERVER,1,-1,0)

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

	content=replace(content,"@INFORMIXINCLUDES@",INFORMIXINCLUDES,1,-1,0)
	content=replace(content,"@INFORMIXLIBS@",INFORMIXLIBS,1,-1,0)
	content=replace(content,"@ALLINFORMIX@",ALLINFORMIX,1,-1,0)
	content=replace(content,"@INSTALLINFORMIX@",INSTALLINFORMIX,1,-1,0)
	content=replace(content,"@ALLROUTER@",ALLROUTER,1,-1,0)
	content=replace(content,"@INSTALLROUTER@",INSTALLROUTER,1,-1,0)

	' cmdline programs
	content=replace(content,"@ALLCMDLINE@",ALLCMDLINE,1,-1,0)
	content=replace(content,"@INSTALLCMDLINE@",INSTALLCMDLINE,1,-1,0)

	' docs
	content=replace(content,"@INSTALLDOC@",INSTALLDOC,1,-1,0)

	' enabled apis
	content=replace(content,"@APIALLSUBDIRS@",APIALLSUBDIRS,1,-1,0)
	content=replace(content,"@APICLEANSUBDIRS@",APICLEANSUBDIRS,1,-1,0)
	content=replace(content,"@APIINSTALLSUBDIRS@",APIINSTALLSUBDIRS,1,-1,0)
	content=replace(content,"@APIUNINSTALLSUBDIRS@",APIUNINSTALLSUBDIRS,1,-1,0)

	' sqlr
	content=replace(content,"@SQLR@",SQLR,1,-1,0)
	content=replace(content,"@SQLRELAY@",SQLRELAY,1,-1,0)
	content=replace(content,"@SQL_RELAY@",SQL_RELAY,1,-1,0)

	' max connections
	content=replace(content,"@ABS_MAXCONNECTIONS@",ABS_MAXCONNECTIONS,1,-1,0)

	' tests
	content=replace(content,"@TESTDBS@",TESTDBS,1,-1,0)
	content=replace(content,"@TESTAPIS@",TESTAPIS,1,-1,0)

	' write output file
	set outfile=fso.OpenTextFile(outfiles(i),2,true)
	call outfile.Write(content)
	call outfile.Close()
next


' summary
WScript.Echo("")
WScript.Echo("***** Summary ***********************************************")
WScript.Echo(" Version      : " & SQLR_VERSION)
WSCript.Echo("")
WScript.Echo(" Branding     : " & SQL_RELAY & " / " & SQLRELAY & " / " & SQLR)
WSCript.Echo("")
WScript.Echo(" Command Line : Clients     " & CMDLINEBUILD)
WSCript.Echo("")
WSCript.Echo(" APIs         : C/C++       " & CPPBUILD & "           Perl       " & PERLBUILD)
WSCript.Echo("                Python      " & PYTHONBUILD & "           Ruby       " & RUBYBUILD)
WSCript.Echo("                PHP         " & PHPBUILD & "           Java       " & JAVABUILD)
WSCript.Echo("                PHP PDO     " & PHPPDOBUILD & "           ODBC       " & ODBCDRIVERBUILD)
WSCript.Echo("                TCL         " & TCLBUILD & "           C#         " & CSBUILD)
WScript.Echo("                node.js     " & NODEJSBUILD)
WSCript.Echo("")
WScript.Echo(" Databases    : Oracle8     " & ORACLE8BUILD & "       MySQL      " & MYSQLBUILD)
WScript.Echo("                PostgreSQL  " & POSTGRESQLBUILD & "       SAP/Sybase " & SYBASEBUILD)
WScript.Echo("                ODBC        " & ODBCBUILD & "       DB2        " & DB2BUILD)
WScript.Echo("                Firebird    " & FIREBIRDBUILD & "       Informix   " & INFORMIXBUILD)
WScript.Echo("                Router      " & ROUTERBUILD)
WScript.Echo("*************************************************************")
WScript.Echo("")
WScript.Echo("If you expected a Database or API that doesn't show up in the Summary")
WScript.Echo("then the configure script probably couldn't find a package it needed to")
WScript.Echo("build it.  You can manually specify package locations using command line")
WScript.Echo("options.")
WScript.Echo("")
WScript.Echo("Type:  cscript /nologo configure.vbs --help   for a list of options.")
WScript.Echo("")



Sub configureDatabase(dbname, dblowername, disabledb,_
			basefolder, subfolderpattern,_
			includessubfolder, includespattern,_
			libssubfolder, libpattern, extralibs,_
			defaultincludes, defaultlibssubfolder, defaultlibs,_
			DBPREFIX, DBINCLUDES, DBLIBS, ALLDB, INSTALLDB)
	on error resume next

	if disabledb=false then

		if DBPREFIX="" then

			' if no db prefix was supplied, then look for the db
			includesfolder=""
			libsfolder=""
			libfile=""
			if findHeadersAndLibs(basefolder,subfolderpattern,_
						includessubfolder,_
						includespattern,_
						includesfolder,_
						libssubfolder,_
						libpattern,_
						libsfolder,_
						libfile)=true then

				' if we found it then set the
				' DBINCLUDES and DBLIBS 
				DBINCLUDES="/I """ & includesfolder & """"
				DBLIBS="/LIBPATH:""" & libsfolder & """ " &_
						libfile & " " & extralibs
			else
				' if we didn't find it then disable the db
				disabledb=true
			end if

		else

			' if a db prefix was supplied, then just use it
			DBINCLUDES="/I """ & DBPREFIX & defaultincludes & """"
			DBLIBS="/LIBPATH:""" & DBPREFIX &_
						 defaultlibssubfolder &_
						 " " & defaultlibs & """"
		end if
	end if
	
	' display success or failure
	if disabledb=false then
		ALLDB="all-" & dblowername
		INSTALLDB="installdll-" & dblowername

		WScript.Echo(dbname & " includes... " & DBINCLUDES)
		WScript.Echo(dbname & " libs... " & DBLIBS)
	else
		WScript.Echo(dbname & " support will not be built. ")
	end if
End Sub


Function findHeadersAndLibs(basefolder, subfolderpattern,_
			includessubfolder, includespattern, includesfolder,_
			libssubfolder, libpattern, libsfolder, libfile)

	on error resume next
	Err.Number=0

	findHeadersAndLibs=false

	' open the base folder
	Set bf=fso.GetFolder(basefolder)
	if Err.Number<>0 then
		return
	end if

	' get and sort its subfolders (descending)
	' (this makes more newly versioned folders be found first)
	Dim subfolders(100)
	i=0
	for each sf in bf.SubFolders
		if i < UBound(subfolders) then
			subfolders(i) = sf.Name
			i=i+1
		end if
	next
	Sort(subfolders)
	Reverse(subfolders)

	' run through the subfolders...
	for i=0 to UBound(subfolders)

		sfname=subfolders(i)

		' reset output variables
		includesfolder=""
		libsfolder=""
		libfile=""

		' if we find the specified subfolder pattern...
		if InStr(sfname,subfolderpattern)>0 then

			' look for the includes subfolder
			isfname=basefolder & "\" & sfname &_
						"\" & includessubfolder
			Set isf=fso.GetFolder(isfname)
			if Err.Number=0 then
				for each fname in isf.Files
					if InStr(fname,includespattern)>0 then
						includesfolder=isfname
						exit for
					end if
				next
			end if

			' look for the libs subfolder
			lsfname=basefolder & "\" & sfname &_
						"\" & libssubfolder
			Set lsf=fso.GetFolder(lsfname)
			if Err.Number=0 then
				for each fname in lsf.Files
					if InStr(fname,libpattern)>0 then
						libsfolder=lsfname
						if InStrRev(fname,"\")>0 then
							libfile=_
							Mid(fname,_
							InStrRev(fname,"\")+1)
						else
							libfile=fname
						end if
						exit for
					end if
				next
			end if

			' exit if we found everything we were looking for
			if Len(includesfolder)>0 and _
				Len(libsfolder)>0 and Len(libfile)>0 then
				findHeadersAndLibs=true
				exit for
			end if
		end if
	next
End Function


Sub findPrefix(basefolder, subfolderpattern, apiprefix, disableapi)

	on error resume next
	Err.Number=0

	findPrefix=false
	apiprefix=""

	if disableapi=false then

		disableapi=true

		' open the base folder
		Set bf=fso.GetFolder(basefolder)
		if Err.Number<>0 then
			return
		end if

		' get and sort its subfolders (descending)
		' (this makes more newly versioned folders be found first)
		Dim subfolders(100)
		i=0
		for each sf in bf.SubFolders
			if i < UBound(subfolders) then
				if InStr(sf.Name,subfolderpattern)>0 then
					subfolders(i) = sf.Name
					i=i+1
				end if
			end if
		next
		Sort(subfolders)
		Reverse(subfolders)

		' return the first matching subfolder (after the sort)
		if UBound(subfolders)>-1 then
			apiprefix=basefolder & subfolders(0)
			disableapi=false
		end if

	end if
End Sub


Sub findVersion(basefolder, fileprefix, filesuffix, apiversion)

	on error resume next
	Err.Number=0

	apiversion=""

	' open the base folder
	Set bf=fso.GetFolder(basefolder)
	if Err.Number<>0 then
		return
	end if

	baselen=Len(basefolder)+1
	prefixlen=Len(fileprefix)
	suffixlen=Len(filesuffix)

	' run through its files
	for each fname in bf.Files

		fname=Mid(fname,baselen+1)
		
		' ignore the file if it's too short
		if Len(fname)>prefixlen+suffixlen then

			' get the beginning and end of the file
			fprefix=Left(fname,prefixlen)
			fsuffix=Right(fname,suffixlen)

			if fprefix=fileprefix and fsuffix=filesuffix then
				apiversion=Mid(fname,prefixlen+1,_
						Len(fname)-prefixlen-suffixlen)
				exit for
			end if
		end if
	next

	' if necessary, run through its folders
	if apiversion="" then
		for each sfname in bf.SubFolders

			sfname=Mid(sfname,baselen+1)
		
			' ignore the folder if it's too short
			if Len(sfname)>prefixlen+suffixlen then

				' get the beginning and end of the folder
				sfprefix=Left(sfname,prefixlen)
				sfsuffix=Right(sfname,suffixlen)

				if sfprefix=fileprefix and _
					sfsuffix=filesuffix then
					apiversion=Mid(sfname,prefixlen+1,_
						Len(sfname)-prefixlen-suffixlen)
					exit for
				end if
			end if
		next
	end if

End Sub

Function Sort(arr)
	for i=UBound(arr)-1 to 0 step -1
		for j=0 to i
			if arr(j)>arr(j+1) then
				temp=arr(j+1)
				arr(j+1)=arr(j)
				arr(j)=temp
			end if
		next
	next
	Sort=arr
End Function

Function Reverse(arr)
	for i=0 to UBound(arr)/2
		temp=arr(i)
		arr(i)=arr(UBound(arr)-i)
		arr(UBound(arr)-i)=temp
	next
	Reverse=arr
End Function
