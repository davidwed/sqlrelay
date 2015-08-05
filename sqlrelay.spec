# Available build options, you need rpm-build >= 4.0.3 for this to work.
# Example: rpmbuild -ba --without mysql --without php sqlrelay.spec
#
# Database options:
# ================
# --without db2
# --without freetds
# --without firebird
# --without mdbtools
# --without mysql
# --without odbc
# --without oracle
# --without postgresql
# --without sqlite
# --without sybase
#
# Language options:
# ================
# --without java
# --without perl
# --without php
# --without python
# --without ruby
# --without tcl
# --without erlang
# --without mono
# --without nodejs

Summary: Persistent database connection system.
Name: sqlrelay
Version: 0.61
Release: 1
License: GPL/LGPL and Others
Group: System Environment/Daemons
Source0: %{name}-%{version}.tar.gz
URL: http://sqlrelay.sourceforge.net
Buildroot: %{_tmppath}/%{name}-root

%define fedoraversion %(for word in `cat /etc/redhat-release 2> /dev/null`; do VAR=`echo $word | grep -x "[0-9]*"`; if ( test -n "$VAR" ); then echo $VAR; fi ; done)

%define docdir %{_docdir}/%{name}
%if %([[ %{_vendor} == "suse" ]] && echo 1 || echo 0)
	%define phpconfdir /etc/php5/conf.d
%else
	%define phpconfdir /etc/php.d
%endif

%if %([[ %{_vendor} == "suse" ]] && echo 1 || echo 0)
%ifarch x86_64
	%define nodejsdir /usr/lib64/node_modules
%else
	%define nodejsdir /usr/lib/node_modules
%endif
%else
	%define nodejsdir /usr/lib/node_modules
%endif

BuildRequires: rudiments-devel
%{!?_without_mysql:BuildRequires: ,mysql-devel}
%{!?_without_odbc:BuildRequires: ,unixODBC-devel}
%{!?_without_postgresql:BuildRequires: ,postgresql-devel}
%{!?_without_perl:BuildRequires: ,perl}
%{!?_without_python:BuildRequires: ,python-devel}
%{!?_without_ruby:BuildRequires: ,ruby-devel}
%{!?_without_tcl:BuildRequires: ,tcl-devel}
%{!?_without_erlang:BuildRequires: ,erlang}
%{!?_without_mono:BuildRequires: ,mono-devel,mono-data}
%{!?_without_nodejs:BuildRequires: ,nodejs-devel}

%description
SQL Relay is a persistent database connection pooling, proxying, throttling, load balancing and query routing/filtering system for Unix and Linux supporting ODBC, Oracle, MySQL, PostgreSQL, Sybase, MS SQL Server, IBM DB2, Firebird, SQLite and MS Access (minimally) with APIs for C, C++, .NET, Perl, Perl-DBI, Python, Python-DB, PHP, PHP PDO, Ruby, Java, TCL, Erlang, and node.js, ODBC and ADO.NET drivers, drop-in replacement libraries for MySQL and PostgreSQL, command line clients and extensive documentation.  The APIs support advanced database operations such as bind variables, multi-row fetches, client-side result set caching and suspended transactions.  It is ideal for speeding up database-driven web-based applications, accessing databases from unsupported platforms, migrating between databases, distributing access to replicated or clustered databases and throttling database access.


%package server-devel
Summary: Development files for developing modules for the SQL Relay server.
Group: Development/Libraries

%description server-devel
Development files for developing modules for the SQL Relay server.


%package clients
Summary: Command line applications for accessing databases through SQL Relay.
Group: Applications/Database

%description clients
Command line applications for accessing databases through SQL Relay.


%package client-runtime-c++
Summary: Runtime libraries for SQL Relay clients written in C++.
Group: Applications/Libraries

%description client-runtime-c++
Runtime libraries for SQL Relay clients written in C++.


%package client-runtime-c
Summary: Runtime libraries for SQL Relay clients written in C.
Group: Applications/Libraries

%description client-runtime-c
Runtime libraries for SQL Relay clients written in C.


%package client-devel-c++
Summary: Development files for developing programs in C++ that use SQL Relay.
Group: Development/Libraries

%description client-devel-c++
Header files and static libraries to use for developing programs in C++ that
use SQL Relay.


%package client-devel-c
Summary: Development files for developing programs C that use SQL Relay.
Group: Development/Libraries

%description client-devel-c
Header files and static libraries to use for developing programs in C that
use SQL Relay.


%package client-postgresql
Summary: Drop in replacement library allowing PostgreSQL clients to use SQL Relay instead.
Group: Applications/Libraries

%description client-postgresql
Drop in replacement library allowing PostgreSQL clients to use SQL Relay instead.


%package client-mysql
Summary: Drop in replacement library allowing MySQL clients to use SQL Relay instead.
Group: Applications/Libraries

%description client-mysql
Drop in replacement library allowing MySQL clients to use SQL Relay instead.


%package client-odbc
Summary: ODBC driver
Group: Applications/Libraries

%description client-odbc
ODBC driver


%package db2
Summary: SQL Relay connection plugin for IBM DB2.
Group: Applications/Databases

%description db2
SQL Relay connection plugin for IBM DB2.


%package freetds
Summary: SQL Relay connection plugin for FreeTDS (Sybase and MS SQL Server).
Group: Applications/Databases

%description freetds
SQL Relay connection plugin for FreeTDS (Sybase and MS SQL Server).


%package firebird
Summary: SQL Relay connection plugin for Firebird.
Group: Applications/Databases

%description firebird
SQL Relay connection plugin for Firebird.


%package mdbtools
Summary: SQL Relay connection plugin for MDB Tools (Microsoft Access).
Group: Applications/Databases

%description mdbtools
SQL Relay connection plugin for MDB Tools (Microsoft Access).


%package mysql
Summary: SQL Relay connection plugin for MySQL.
Group: Applications/Databases

%description mysql
SQL Relay connection plugin for MySQL.


%package odbc
Summary: SQL Relay connection plugin for ODBC.
Group: Applications/Databases

%description odbc
SQL Relay connection plugin for ODBC.


%package oracle8
Summary: SQL Relay connection plugin for Oracle 8.
Group: Applications/Databases

%description oracle8
SQL Relay connection plugin for Oracle 8.


%package postgresql
Summary: SQL Relay connection plugin for PostgreSQL.
Group: Applications/Databases

%description postgresql
SQL Relay connection plugin for PostgreSQL.


%package sqlite
Summary: SQL Relay connection plugin for SQLite.
Group: Applications/Databases

%description sqlite
SQL Relay connection plugin for SQLite.


%package sybase
Summary: SQL Relay connection plugin for Sybase.
Group: Applications/Databases

%description sybase
SQL Relay connection plugin for Sybase.


%package router
Summary: SQL Relay query routing daemon.
Group: Applications/Databases

%description router
SQL Relay query routing daemon.


%package java
Summary: SQL Relay modules for Java.
Group: Development/Languages

%description java
SQL Relay modules for Java.


%package perl
Summary: SQL Relay modules for Perl.
Group: Development/Languages

%description perl
SQL Relay modules for Perl.


%package php
Summary: SQL Relay modules for PHP.
Group: Development/Languages

%description php
SQL Relay modules for PHP.


%package python
Summary: SQL Relay modules for Python.
Group: Development/Languages

%description python
SQL Relay modules for Python.


%package ruby
Summary: SQL Relay modules for Ruby.
Group: Development/Languages

%description ruby
SQL Relay modules for Ruby.


%package tcl
Summary: SQL Relay modules for TCL.
Group: Development/Languages

%description tcl
SQL Relay modules for TCL.


%package erlang
Summary: SQL Relay modules for Erlang.
Group: Development/Languages

%description erlang
SQL Relay modules for Erlang.


%package mono
Summary: SQL Relay modules for Mono.
Group: Development/Languages

%description mono
SQL Relay modules for Mono.


%package nodejs
Summary: SQL Relay modules for node.js.
Group: Development/Languages

%description nodejs
SQL Relay modules for node.js.


%package doc
Summary: Documentation for SQL Relay.
Group: Applications/Database

%description doc
Documentation for SQL Relay.


%package man
Summary: Man pages for SQL Relay.
Group: Applications/Database

%description man
Man pages for SQL Relay.


%define	tclconfig	%(TCLCONFIG=`rpm -q -l tcl-devel | grep -m1 "/tclConfig.sh"`; RTCLCONFIG=`readlink $TCLCONFIG`; if ( test -n "$RTCLCONFIG" ) then echo $RTCLCONFIG; else echo $TCLCONFIG; fi)
%define	tcldir		%(dirname %{tclconfig})
%ifarch x86_64
%define	erlangdir	%(ERLPATH=""; for i in "/usr/local/lib64/erlang/lib" "/usr/lib64/erlang/lib"; do if ( test -d "$i" ); then ERLPATH="$i"; fi; done; echo $ERLPATH)
%else
%define	erlangdir	%(ERLPATH=""; for i in "/usr/local/lib/erlang/lib" "/usr/lib/erlang/lib"; do if ( test -d "$i" ); then ERLPATH="$i"; fi; done; echo $ERLPATH)
%endif
%ifarch x86_64
%define	pythondir	%(PYTHONINCLUDES=""; PYTHONDIR=""; for j in "2.9" "2.8" "2.7" "2.6" "2.5" "2.4" "2.3" "2.2" "2.1" "2.0" "1.6" "1.5"; do for i in "/usr/include/python$j" "/usr/local/include/python$j" "/usr/pkg/include/python$j" "/usr/local/python$j/include/python$j" "/opt/sfw/include/python$j"; do if ( test -d "$i" ); then PYTHONINCLUDES="$i"; fi; if ( test -n "$PYTHONINCLUDES" ); then break; fi; done; for i in "/usr/lib64/python$j" "/usr/local/lib64/python$j" "/usr/pkg/lib64/python$j" "/usr/local/python$j/lib64/python$j" "/opt/sfw/lib64/python$j"; do if ( test -d "$i" ); then PYTHONDIR="$i"; fi; if ( test -n "$PYTHONDIR" ); then break; fi; done; if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" ); then echo $PYTHONDIR; break; fi; done)
%else
%define	pythondir	%(PYTHONINCLUDES=""; PYTHONDIR=""; for j in "2.9" "2.8" "2.7" "2.6" "2.5" "2.4" "2.3" "2.2" "2.1" "2.0" "1.6" "1.5"; do for i in "/usr/include/python$j" "/usr/local/include/python$j" "/usr/pkg/include/python$j" "/usr/local/python$j/include/python$j" "/opt/sfw/include/python$j"; do if ( test -d "$i" ); then PYTHONINCLUDES="$i"; fi; if ( test -n "$PYTHONINCLUDES" ); then break; fi; done; for i in "/usr/lib/python$j" "/usr/local/lib/python$j" "/usr/pkg/lib/python$j" "/usr/local/python$j/lib/python$j" "/opt/sfw/lib/python$j"; do if ( test -d "$i" ); then PYTHONDIR="$i"; fi; if ( test -n "$PYTHONDIR" ); then break; fi; done; if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" ); then echo $PYTHONDIR; break; fi; done)
%endif
%define	phpextdir	%(php-config --extension-dir)
%define	perl_prefix	%(eval "export `perl -V:prefix`"; echo $prefix)
%define	perl_sitelib	%(eval "export `perl -V:sitelib`"; echo $sitelib)
%define	perl_installarchlib	%(eval "export `perl -V:installarchlib`"; echo $installarchlib)
%define	perl_installsitearch	%(eval "export `perl -V:installsitearch`"; echo $installsitearch)
%define	perl_sitearch	%(eval "export `perl -V:sitearch`"; echo $sitearch)
%define	perl_installman3dir	%(eval "export `perl -V:installman3dir`"; echo $installman3dir)
%define	perl_man3ext	%(eval "export `perl -V:man3ext`"; echo $man3ext)

# On opensuse 13.2, for some reason, rubylibprefix uses RUBY_BSE_NAME rather
# than RUBY_BASE_NAME when building RPM's, but only when building rpms, not
# normally.  It's weird.
%define	ruby_sitearchdir	%(ruby -e 'require "mkmf"' -e 'drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/' -e 'print "arch = "' -e 'print CONFIG["arch"]' -e 'print "\\n"' -e 'print "sitearch = "' -e 'print CONFIG["sitearch"]' -e 'print "\\n"' -e 'print "ruby_version = "' -e 'print Config::CONFIG["ruby_version"]' -e 'print "\\n"' -e 'print "prefix = "' -e 'print with_destdir(CONFIG["prefix"].sub(drive, ""))' -e 'print "\\n"' -e 'print "exec_prefix = "' -e 'print with_destdir(CONFIG["exec_prefix"].sub(drive, ""))' -e 'print "\\n"' -e 'print "libdir = "' -e 'print with_destdir($libdir.sub(drive, ""))' -e 'print "\\n"' -e 'if CONFIG["RUBY_BASE_NAME"]!=nil then' -e 'print "RUBY_BASE_NAME = "' -e 'print CONFIG["RUBY_BASE_NAME"]' -e 'print "\\n"' -e 'print "RUBY_BSE_NAME = "' -e 'print CONFIG["RUBY_BASE_NAME"]' -e 'print "\\n"' -e 'end' -e 'if CONFIG["rubylibprefix"]!=nil then' -e 'print "rubylibprefix = "' -e 'print with_destdir(CONFIG["rubylibprefix"].sub(drive, ""))' -e 'print "\\n"' -e 'end' -e 'print "rubylibdir = "' -e 'print with_destdir($rubylibdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "archdir = "' -e 'print with_destdir($archdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "sitedir = "' -e 'print with_destdir($sitedir.sub(drive, ""))' -e 'print "\\n"' -e 'print "_fc_sitedir = "' -e 'print with_destdir($sitedir.sub(drive, ""))' -e 'print "\\n"' -e 'print "sitelibdir = "' -e 'print with_destdir($sitelibdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "sitearchdir = "' -e 'print with_destdir($sitearchdir.sub(drive, ""))' -e 'print "\\n\\n"' -e 'print "all:\\n"' -e 'print "	echo $(sitearchdir)\\n"' | make -s -f - )

%prep
%setup -q

%build
%configure \
	%{?_without_db2:	--disable-db2} \
	%{?_without_freetds:	--disable-freetds} \
	%{?_without_firebird:	--disable-firebird} \
	%{?_without_mdbtools:	--disable-mdbtools} \
	%{?_without_mysql:	--disable-mysql} \
	%{?_without_odbc:	--disable-odbc} \
	%{?_without_oracle:	--disable-oracle} \
	%{?_without_postgresql:	--disable-postgresql} \
	%{?_without_sqlite:	--disable-sqlite} \
	%{?_without_sybase:	--disable-sybase} \
	%{?_without_java:	--disable-java} \
	%{?_without_tcl:	--disable-tcl} \
	%{?_without_erlang:	--disable-erlang} \
	%{?_without_nodejs:	--disable-nodejs} \
	%{?_without_mono:	--disable-mono} \
	%{?_without_perl:	--disable-perl} \
	%{?_without_php:	--disable-php} \
	%{?_without_python:	--disable-python} \
	%{?_without_ruby:	--disable-ruby}
	
make

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} docdir=%{buildroot}%{_docdir}/%{name} EXAMPLEDIR=%{buildroot}%{_datadir}/examples/%{name} install
# get rid of some garbage
rm -f %{buildroot}%{perl_installsitearch}/perllocal.pod

%pre
# Add the "sqlrelay" user
/usr/sbin/useradd -c "SQL Relay" -s /bin/false \
	-r -d %{_localstatedir}/sqlrelay sqlrelay 2> /dev/null || :

%post
/sbin/ldconfig
if [ $1 = 1 ]; then
	/sbin/chkconfig --add sqlrelay
fi

%preun
if [ $1 = 0 ]; then
	/sbin/service sqlrelay stop> /dev/null 2>&1 || :
	/sbin/chkconfig --del sqlrelay
fi

%postun
/sbin/ldconfig
if [ "$1" -ge "1" ]; then
	/sbin/service sqlrelay condrestart >/dev/null 2>&1 || :
fi


%clean
rm -rf %{buildroot}


%files
%defattr(-, root, root)
%{_sysconfdir}/sqlrelay.conf.d
%config %attr(600, root, root) %{_sysconfdir}/sqlrelay.conf.example
%config %attr(600, root, root) %{_sysconfdir}/sqlrelay.dtd
%config %attr(600, root, root) %{_sysconfdir}/sqlrelay.xsd
/etc/init.d/sqlrelay
/etc/init.d/sqlrcachemanager
%{_bindir}/sqlr-cachemanager*
%{_bindir}/sqlr-listener*
%{_bindir}/sqlr-connection*
%{_bindir}/sqlr-scaler*
%{_bindir}/sqlr-start*
%{_bindir}/sqlr-stop
%{_bindir}/sqlr-pwdenc
%{_libdir}/libsqlrserver-*.so.*
%{_libdir}/libsqlrutil-*.so.*
%{_libexecdir}/sqlrelay
%{_libexecdir}/sqlrelay/sqlrlogger_*
%{_libexecdir}/sqlrelay/sqlrquery_*
%{_libexecdir}/sqlrelay/sqlrpwdenc_*
%{_libexecdir}/sqlrelay/sqlrauth_*
%{_libexecdir}/sqlrelay/sqlrparser_*
%{_libexecdir}/sqlrelay/sqlrprotocol_*
%{_localstatedir}/sqlrelay
%{_localstatedir}/sqlrelay/tmp
%{_localstatedir}/sqlrelay/debug
%{_localstatedir}/sqlrelay/log
%{_localstatedir}/sqlrelay/cache

%files server-devel
%defattr(-, root, root)
%{_bindir}/sqlrserver-config
%{_includedir}/sqlrelay
%{_includedir}/sqlrelay/private
%{_includedir}/sqlrelay/sqlrserver.h
%{_includedir}/sqlrelay/private/sqlrconnection.h
%{_includedir}/sqlrelay/private/sqlrcursor.h
%{_includedir}/sqlrelay/private/sqlrlistener.h
%{_includedir}/sqlrelay/private/sqlrserverconnection.h
%{_includedir}/sqlrelay/private/sqlrservercontroller.h
%{_includedir}/sqlrelay/private/sqlrservercursor.h
%{_includedir}/sqlrelay/private/sqlrserverdll.h
%{_includedir}/sqlrelay/private/sqlrserverincludes.h
%{_includedir}/sqlrelay/private/sqlrshmdata.h
%{_includedir}/sqlrelay/sqlrutil.h
%{_includedir}/sqlrelay/private/sqlrutildll.h
%{_libdir}/libsqlrserver.a
%{_libdir}/libsqlrserver.la
%{_libdir}/libsqlrserver.so
%{_libdir}/libsqlrutil.a
%{_libdir}/libsqlrutil.la
%{_libdir}/libsqlrutil.so

%files clients
%defattr(-, root, root)
%{_bindir}/sqlr-fields
%{_bindir}/sqlr-query
%{_bindir}/sqlrsh
%{_bindir}/sqlr-export
%{_bindir}/sqlr-import
%{_bindir}/sqlr-status*

%files client-runtime-c++
%defattr(-, root, root)
%{_libdir}/libsqlrclient-*.so.*

%files client-runtime-c
%defattr(-, root, root)
%{_libdir}/libsqlrclientwrapper-*.so.*

%files client-devel-c++
%defattr(-, root, root)
%{_bindir}/sqlrclient-config
%{_includedir}/sqlrelay/sqlrclient.h
%{_includedir}/sqlrelay/private/bindvar.h
%{_includedir}/sqlrelay/private/column.h
%{_includedir}/sqlrelay/private/dll.h
%{_includedir}/sqlrelay/private/row.h
%{_includedir}/sqlrelay/private/sqlrdefines.h
%{_includedir}/sqlrelay/private/sqlrincludes.h
%{_libdir}/libsqlrclient.a
%{_libdir}/libsqlrclient.la
%{_libdir}/libsqlrclient.so
%{_libdir}/pkgconfig/sqlrelay-c++.pc

%files client-devel-c
%defattr(-, root, root)
%{_bindir}/sqlrclientwrapper-config
%{_includedir}/sqlrelay/sqlrclientwrapper.h
%{_includedir}/sqlrelay/private/sqlrclientwrapper.h
%{_includedir}/sqlrelay/private/wrapperdll.h
%{_libdir}/libsqlrclientwrapper.a
%{_libdir}/libsqlrclientwrapper.la
%{_libdir}/libsqlrclientwrapper.so
%{_libdir}/pkgconfig/sqlrelay-c.pc

%files client-postgresql
%defattr(-, root, root)
%{_libdir}/libpqsqlrelay-*.so.*
%{_libdir}/libpqsqlrelay.so

%files client-mysql
%defattr(-, root, root)
%{_libdir}/libmysql*sqlrelay-*.so.*
%{_libdir}/libmysql*sqlrelay.so

%{!?_without_odbc:%files client-odbc}
%{!?_without_odbc:%defattr(-, root, root)}
%{!?_without_odbc:%{_libdir}/libsqlrodbc-*.so.*}
%{!?_without_odbc:%{_libdir}/libsqlrodbc.so}

%{!?_without_db2:%files db2}
%{!?_without_db2:%defattr(-, root, root)}
%{!?_without_db2:%{_libexecdir}/sqlrelay/sqlrconnection_db2*}

%{!?_without_freetds:%files freetds}
%{!?_without_freetds:%defattr(-, root, root)}
%{!?_without_freetds:%{_libexecdir}/sqlrelay/sqlrconnection_freetds*}

%{!?_without_firebird:%files firebird}
%{!?_without_firebird:%defattr(-, root, root)}
%{!?_without_firebird:%{_libexecdir}/sqlrelay/sqlrconnection_firebird*}

%{!?_without_mdbtools:%files mdbtools}
%{!?_without_mdbtools:%defattr(-, root, root)}
%{!?_without_mdbtools:%{_libexecdir}/sqlrelay/sqlrconnection_mdbtools*}

%{!?_without_mysql:%files mysql}
%{!?_without_mysql:%defattr(-, root, root)}
%{!?_without_mysql:%{_libexecdir}/sqlrelay/sqlrconnection_mysql*}

%{!?_without_odbc:%files odbc}
%{!?_without_odbc:%defattr(-, root, root)}
%{!?_without_odbc:%{_libexecdir}/sqlrelay/sqlrconnection_odbc*}

%{!?_without_oracle:%files oracle8}
%{!?_without_oracle:%defattr(-, root, root)}
%{!?_without_oracle:%{_libexecdir}/sqlrelay/sqlrconnection_oracle8*}

%{!?_without_postgresql:%files postgresql}
%{!?_without_postgresql:%defattr(-, root, root)}
%{!?_without_postgresql:%{_libexecdir}/sqlrelay/sqlrconnection_postgresql*}

%{!?_without_sqlite:%files sqlite}
%{!?_without_sqlite:%defattr(-, root, root)}
%{!?_without_sqlite:%{_libexecdir}/sqlrelay/sqlrconnection_sqlite*}

%{!?_without_sybase:%files sybase}
%{!?_without_sybase:%defattr(-, root, root)}
%{!?_without_sybase:%{_libexecdir}/sqlrelay/sqlrconnection_sybase*}

%{!?_without_router:%files router}
%{!?_without_router:%defattr(-, root, root)}
%{!?_without_router:%{_libexecdir}/sqlrelay/sqlrconnection_router*}

%{!?_without_java:%files java}
%{!?_without_java:%defattr(-, root, root)}
%{!?_without_java:%{_prefix}/java/*}

%{!?_without_perl:%files perl}
%{!?_without_perl:%defattr(-, root, root)}
%{!?_without_perl:%{perl_sitelib}/DBD/SQLRelay.pm}
%{!?_without_perl:%{perl_sitearch}/auto/DBD/SQLRelay}
%{!?_without_perl:%{perl_sitearch}/SQLRelay/Connection.pm}
%{!?_without_perl:%{perl_sitearch}/SQLRelay/Cursor.pm}
%{!?_without_perl:%{perl_sitearch}/auto/SQLRelay/Connection}
%{!?_without_perl:%{perl_sitearch}/auto/SQLRelay/Cursor}
%{!?_without_perl:%{perl_installman3dir}/*.%{perl_man3ext}*}

%{!?_without_php:%files php}
%{!?_without_php:%defattr(-, root, root)}
%{!?_without_php:%{phpextdir}/sql_relay.so}
%{!?_without_php:%{phpextdir}/pdo_sqlrelay.so}
%{!?_without_php:%{phpconfdir}/sql_relay.ini}
%{!?_without_php:%{phpconfdir}/pdo_sqlrelay.ini}

%{!?_without_python:%files python}
%{!?_without_python:%defattr(-, root, root)}
%{!?_without_python:%{pythondir}/site-packages/SQLRelay}

%{!?_without_ruby:%files ruby}
%{!?_without_ruby:%defattr(-, root, root)}
%{!?_without_ruby:%{ruby_sitearchdir}/sqlrelay.so}

%{!?_without_tcl:%files tcl}
%{!?_without_tcl:%defattr(-, root, root)}
%{!?_without_tcl:%{tcldir}/sqlrelay/*}

%{!?_without_erlang:%files erlang}
%{!?_without_erlang:%defattr(-, root, root)}
%{!?_without_erlang:%{erlangdir}/sqlrelay-%{version}}

%{!?_without_mono:%files mono}
%{!?_without_mono:%defattr(-, root, root)}
%{!?_without_mono:%{_libdir}/SQLRClient.dll}
%{!?_without_mono:%{_libdir}/SQLRClient.dll.config}

%{!?_without_nodejs:%files nodejs}
%{!?_without_nodejs:%defattr(-, root, root)}
%{!?_without_nodejs:%{nodejsdir}/sqlrelay}

%files doc
%{_docdir}/%{name}
%{_datadir}/licenses/%{name}
%{_datadir}/examples/%{name}

%files man
%{_mandir}

%changelog
* Mon Feb 17 2003 David Muse <david.muse@firstworks.com>
- removed the -u from useradd
- uses init script and /etc/sysconfig/sqlrelay from the distribution
- updated url's
- put .so's in devel package and .so.*'s in main package

* Fri Apr 26 2002 Matthias Saou <matthias.saou@est.une.marmotte.net>
- Polished the init script, too bad sqlr-start and sqlr-stop don't seem
  to have exit values different than 0 :-( It's "OK" whatever happens.

* Thu Apr 25 2002 Matthias Saou <matthias.saou@est.une.marmotte.net>
- Added the sysconfig entry to start connections automatically.
- Big updates to the init script.

* Fri Apr 19 2002 Matthias Saou <matthias.saou@est.une.marmotte.net>
- Finished the perl package rewrite.
- Added an init script.

* Mon Apr 15 2002 Matthias Saou <matthias.saou@est.une.marmotte.net>
- Spec file rewrite start.

