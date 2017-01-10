Name: sqlrelay
Version: 1.0.1
Release: 1%{?dist}
Summary: Database proxy

License: GPLv2 with exceptions
URL: http://sqlrelay.sourceforge.net
Source0: http://downloads.sourceforge.net/%{name}/%{name}-%{version}.tar.gz

# FIXME: I think pcre/openssl/libcurl/krb/readline-devel all need to be moved to the rudiments-devel package
BuildRequires: rudiments-devel >= 1.0.2,mysql-devel,postgresql-devel,sqlite-devel,freetds-devel,unixODBC-devel,firebird-devel,mdbtools-devel,perl-devel,python3-devel,ruby-devel,php-devel,java-1.8.0-openjdk-devel,tcl-devel,erlang,mono-devel,mono-data,mono-data-oracle,nodejs-devel,readline-devel,pcre-devel,openssl-devel,libcurl-devel,krb5-devel

%description
SQL Relay is a persistent database connection pooling, proxying, throttling,
load balancing and query routing/filtering system for Unix and Linux supporting 
ODBC, Oracle, MySQL, PostgreSQL, SAP/Sybase, MS SQL Server, IBM DB2, Informix, 
Firebird, SQLite and MS Access (minimally) with APIs for C, C++, .NET, Perl, 
Perl-DBI, Python, Python-DB, PHP, PHP PDO, Ruby, Java, TCL, Erlang, and node.js,
ODBC and ADO.NET drivers, drop-in replacement libraries for MySQL and
PostgreSQL, command line clients and extensive documentation.  The APIs support
advanced database operations such as bind variables, multi-row fetches,
client-side result set caching and suspended transactions.  It is ideal for
speeding up database-driven web-based applications, accessing databases from
unsupported platforms, migrating between databases, distributing access to
replicated or clustered databases and throttling database access.


%package server-devel
License: GPLv2 with exceptions
Summary: Development files for SQL Relay server modules
Requires: %{name}%{?_isa} = %{version}-%{release}

%description server-devel
Development files for SQL Relay server modules.


%package clients
License: GPLv2
Summary: Command line clients for accessing databases via SQL Relay

%description clients
Command line clients for accessing databases via SQL Relay


%package c++
License: LGPLv2
Summary: SQL Relay C++ client libraries

%description c++
SQL Relay C++ client libraries.


%package c
License: LGPLv2
Summary: SQL Relay C client libraries

%description c
SQL Relay C client libraries.


%package c++-devel
License: LGPLv2
Summary: Development files for the SQL Relay C++ client libraries
Requires: %{name}-c++%{?_isa} = %{version}-%{release},rudiments-devel >= 1.0.2

%description c++-devel
Development files for the SQL Relay C++ client libraries.


%package c-devel
License: LGPLv2
Summary: Development files for the SQL Relay C client libraries
Requires: %{name}-c%{?_isa} = %{version}-%{release}

%description c-devel
Development files for the SQL Relay C client libraries.


%package dropin-postgresql
License: PostgreSQL
Summary: Drop in replacement library that redirects PostgreSQL clients to SQL Relay

%description dropin-postgresql
Drop in replacement library that redirects PostgreSQL clients to SQL Relay.


%package dropin-mysql
License: GPLv2
Summary: Drop in replacement library that redirects MySQL clients to SQL Relay

%description dropin-mysql
Drop in replacement library that redirects MySQL clients to SQL Relay.


%package connector-odbc
License: LGPLv2
Summary: ODBC connector for SQL Relay

%description connector-odbc
ODBC connector for SQL Relay.


%package db2
License: GPLv2 with exceptions
Summary: IBM DB2 back-end module for SQL Relay

%description db2
IBM DB2 back-end module for SQL Relay


%package freetds
License: GPLv2 with exceptions
Summary: FreeTDS back-end module for SQL Relay

%description freetds
FreeTDS back-end module for SQL Relay.  Enables access to SAP/Sybase and MS SQL Server databases.


%package firebird
License: GPLv2 with exceptions
Summary: Firebird back-end module for SQL Relay

%description firebird
Firebird back-end module for SQL Relay.


%package mdbtools
License: GPLv2 with exceptions
Summary: MDB Tools back-end module for SQL Relay

%description mdbtools
MDB Tools back-end module for SQL Relay.  Enables read-only access to Microsoft Access databases.


%package mysql
License: GPLv2 with exceptions
Summary: MySQL back-end module for SQL Relay

%description mysql
MySQL back-end module for SQL Relay.


%package odbc
License: GPLv2 with exceptions
Summary: ODBC back-end module for SQL Relay

%description odbc
ODBC back-end module for SQL Relay.


%package oracle
License: GPLv2 with exceptions
Summary: Oracle back-end module for SQL Relay

%description oracle
Oracle back-end module for SQL Relay.


%package postgresql
License: GPLv2 with exceptions
Summary: PostgreSQL back-end module for SQL Relay

%description postgresql
PostgreSQL back-end module for SQL Relay.


%package sqlite
License: GPLv2 with exceptions
Summary: SQLite back-end module for SQL Relay

%description sqlite
SQLite back-end module for SQL Relay.


%package sap
License: GPLv2 with exceptions
Summary: SAP/Sybase back-end module for SQL Relay

%description sap
SAP/Sybase back-end module for SQL Relay.


%package informix
License: GPLv2 with exceptions
Summary: Informix back-end module for SQL Relay

%description informix
Informix back-end module for SQL Relay.


%package router
License: GPLv2 with exceptions
Summary: Session/query router back-end module for SQL Relay

%description router
Session/query router back-end module for SQL Relay.


%package java
License: LGPLv2
Summary: Java bindings for the SQL Relay client API

%description java
Java bindings for the SQL Relay client API.


%package perl
License: Artistic
Summary: Perl bindings for the SQL Relay client API

%description perl
Perl bindings for the SQL Relay client API.


%package connector-perl-dbi
License: Artistic
Summary: Perl DBI driver for SQL Relay
Requires: %{name}-perl%{?_isa} = %{version}-%{release}

%description connector-perl-dbi
Perl DBI driver for SQL Relay.


%package php
License: LGPLv2
Summary: PHP bindings for the SQL Relay client API

%description php
PHP bindings for the SQL Relay client API.


%package connector-php-pdo
License: LGPLv2
Summary: PHP PDO driver for SQL Relay.

%description connector-php-pdo
PHP PDO driver for SQL Relay.


%package python3
License: PointOne
Summary: Python 3 bindings for the SQL Relay client API

%description python3
Python 3 bindings for the SQL Relay client API.


%package connector-python3-db
License: PointOne
Summary: Python 3 DB bindings for SQL Relay
Requires: %{name}-python3%{?_isa} = %{version}-%{release}

%description connector-python3-db
Python 3 DB bindings for SQL Relay.


%package ruby
License: LGPLv2
Summary: Ruby bindings for the SQL Relay client API.

%description ruby
Ruby bindings for the SQL Relay client API.


%package tcl
License: LGPLv2
Summary: TCL bindings for the SQL Relay client API

%description tcl
TCL bindings for the SQL Relay client API.


%package erlang
License: CC-BY
Summary: Erlang bindings for the SQL Relay client API

%description erlang
Erlang bindings for the SQL Relay client API.


%package mono
License: LGPLv2
Summary: Mono bindings for the SQL Relay client API

%description mono
Mono bindings for the SQL Relay client API.


%package nodejs
License: LGPLv2
Summary: Nodejs bindings for the SQL Relay client API

%description nodejs
Nodejs bindings for the SQL Relay client API.


%package doc
# Documentation is GPLv2 except for example code in the documentation.
# Example code is FSFUL.
License: GPLv2 and FSFUL
Summary: Documentation for SQL Relay
BuildArch: noarch

%description doc
Documentation for SQL Relay.


%package man
License: GPLv2
Summary: Man pages for SQL Relay
BuildArch: noarch

%description man
Man pages for SQL Relay.


# /usr/lib/perl5/site_perl/5.20.1 on suse
%define perl_sitelib %{_datadir}/perl5
# /usr/lib/perl5/site_perl/5.20.1/x86_64-linux-thread-multi on suse
%define perl_sitearch %{_libdir}/perl5


%prep
%autosetup -p1

%build
%configure --disable-static \
		--enable-oracle-on-demand \
		--enable-sap-on-demand \
		--enable-db2-on-demand \
		--enable-informix-on-demand \
		--disable-python \
		--with-perl-site-lib=%{perl_sitelib} \
		--with-perl-site-arch=%{perl_sitearch} \
		--with-ruby-site-arch=%{ruby_sitearchdir}
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}
# "make install" installs COPYING in: (buildroot)/usr/share/licenses/rudiments
# But, since we prefer to get it directly from the source code, we'll remove it
# from the buildroot so it doesn't trigger an "Installed (but unpackaged)
# file(s) found" error.
make uninstall-license DESTDIR=%{buildroot}

%pre
# Add the "sqlrelay" user
/usr/sbin/useradd -c "SQL Relay" -s /bin/false \
	-r -d %{_localstatedir}/sqlrelay sqlrelay 2> /dev/null || :

%post
/sbin/ldconfig
if [ $1 = 1 ]; then
	/usr/bin/systemctl enable sqlrelay.service
	/usr/bin/systemctl enable sqlrcachemanager.service
fi

%preun
if [ $1 = 0 ]; then
	/usr/bin/systemctl stop sqlrelay.service
	/usr/bin/systemctl stop sqlrcachemanager.service
	/usr/bin/systemctl disable sqlrelay.service
	/usr/bin/systemctl disable sqlrcachemanager.service
fi

%postun
/sbin/ldconfig
if [ "$1" -ge "1" ]; then
	/sbin/service sqlrelay condrestart >/dev/null 2>&1 || :
fi
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :
rm -rf %{_localstatedir}/sqlrelay/tmp
rmdir %{_localstatedir}/sqlrelay 2> /dev/null || :


%files
%{_sysconfdir}/sqlrelay.conf.d
%config %attr(600, root, root) %{_sysconfdir}/sqlrelay.xsd
/lib/systemd/system/sqlrelay.service
/lib/systemd/system/sqlrcachemanager.service
%{_bindir}/sqlr-cachemanager*
%{_bindir}/sqlr-listener*
%{_bindir}/sqlr-connection*
%{_bindir}/sqlr-scaler*
%{_bindir}/sqlr-start*
%{_bindir}/sqlr-stop
%{_bindir}/sqlr-pwdenc
%{_libdir}/libsqlrserver.so.*
%{_libdir}/libsqlrutil.so.*
%{_libexecdir}/sqlrelay/sqlrauth_*
%{_libexecdir}/sqlrelay/sqlrconfig_*
%{_libexecdir}/sqlrelay/sqlrfilter_*
%{_libexecdir}/sqlrelay/sqlrnotification_*
%{_libexecdir}/sqlrelay/sqlrparser_*
%{_libexecdir}/sqlrelay/sqlrprotocol_*
%{_libexecdir}/sqlrelay/sqlrpwdenc_*
%{_libexecdir}/sqlrelay/sqlrlogger_*
%{_libexecdir}/sqlrelay/sqlrquery_*
%{_libexecdir}/sqlrelay/sqlrresultsettranslation_*
%{_libexecdir}/sqlrelay/sqlrschedule_*
%{_libexecdir}/sqlrelay/sqlrtranslation_*
%{_localstatedir}/sqlrelay/tmp
%{_localstatedir}/sqlrelay/debug
%{_localstatedir}/sqlrelay/log
%{_localstatedir}/sqlrelay/cache
%doc AUTHORS ChangeLog
%license COPYING
%exclude %{_libdir}/lib*.la

%files server-devel
%{_bindir}/sqlrserver-config
%{_includedir}/sqlrelay/sqlrserver.h
%{_includedir}/sqlrelay/private/sqlrauth.h
%{_includedir}/sqlrelay/private/sqlrauths.h
%{_includedir}/sqlrelay/private/sqlrfilter.h
%{_includedir}/sqlrelay/private/sqlrfilters.h
%{_includedir}/sqlrelay/private/sqlrgsscredentials.h
%{_includedir}/sqlrelay/private/sqlrlistener.h
%{_includedir}/sqlrelay/private/sqlrlogger.h
%{_includedir}/sqlrelay/private/sqlrloggers.h
%{_includedir}/sqlrelay/private/sqlrnotification.h
%{_includedir}/sqlrelay/private/sqlrnotifications.h
%{_includedir}/sqlrelay/private/sqlrparser.h
%{_includedir}/sqlrelay/private/sqlrprotocol.h
%{_includedir}/sqlrelay/private/sqlrprotocols.h
%{_includedir}/sqlrelay/private/sqlrpwdenc.h
%{_includedir}/sqlrelay/private/sqlrpwdencs.h
%{_includedir}/sqlrelay/private/sqlrqueries.h
%{_includedir}/sqlrelay/private/sqlrquerycursor.h
%{_includedir}/sqlrelay/private/sqlrquery.h
%{_includedir}/sqlrelay/private/sqlrresultsetrowtranslation.h
%{_includedir}/sqlrelay/private/sqlrresultsetrowtranslations.h
%{_includedir}/sqlrelay/private/sqlrresultsettranslation.h
%{_includedir}/sqlrelay/private/sqlrresultsettranslations.h
%{_includedir}/sqlrelay/private/sqlrrouter.h
%{_includedir}/sqlrelay/private/sqlrrouters.h
%{_includedir}/sqlrelay/private/sqlrschedule.h
%{_includedir}/sqlrelay/private/sqlrschedulerule.h
%{_includedir}/sqlrelay/private/sqlrschedules.h
%{_includedir}/sqlrelay/private/sqlrserverconnection.h
%{_includedir}/sqlrelay/private/sqlrservercontroller.h
%{_includedir}/sqlrelay/private/sqlrservercursor.h
%{_includedir}/sqlrelay/private/sqlrserverincludes.h
%{_includedir}/sqlrelay/private/sqlrshm.h
%{_includedir}/sqlrelay/private/sqlrtlscredentials.h
%{_includedir}/sqlrelay/private/sqlrtranslation.h
%{_includedir}/sqlrelay/private/sqlrtranslations.h
%{_includedir}/sqlrelay/private/sqlrtrigger.h
%{_includedir}/sqlrelay/private/sqlrtriggers.h
%{_includedir}/sqlrelay/private/sqlruserpasswordcredentials.h
%{_includedir}/sqlrelay/sqlrutil.h
%{_includedir}/sqlrelay/private/sqlrutilincludes.h
%{_libdir}/libsqlrserver.so
%{_libdir}/libsqlrutil.so
%exclude %{_libdir}/lib*.la

%postun server-devel
rmdir %{_includedir}/sqlrelay 2> /dev/null || :
rmdir %{_includedir}/sqlrelay/private 2> /dev/null || :

%files clients
%{_bindir}/sqlrsh
%{_bindir}/sqlr-export
%{_bindir}/sqlr-import
%{_bindir}/sqlr-status*

%files c++
%{_libdir}/libsqlrclient.so.*

%post c++
/sbin/ldconfig

%postun c++
/sbin/ldconfig

%files c
%{_libdir}/libsqlrclientwrapper.so.*

%post c
/sbin/ldconfig

%postun c
/sbin/ldconfig

%files c++-devel
%{_bindir}/sqlrclient-config
%{_includedir}/sqlrelay/sqlrclient.h
%{_includedir}/sqlrelay/private/sqlrclientincludes.h
%{_includedir}/sqlrelay/private/sqlrconnection.h
%{_includedir}/sqlrelay/private/sqlrcursor.h
%{_libdir}/libsqlrclient.so
%{_libdir}/pkgconfig/sqlrelay-c++.pc
%exclude %{_libdir}/lib*.la

%postun c++-devel
rmdir %{_includedir}/sqlrelay 2> /dev/null || :
rmdir %{_includedir}/sqlrelay/private 2> /dev/null || :

%files c-devel
%{_bindir}/sqlrclientwrapper-config
%{_includedir}/sqlrelay/sqlrclientwrapper.h
%{_includedir}/sqlrelay/private/sqlrclientwrapper.h
%{_includedir}/sqlrelay/private/sqlrclientwrapperincludes.h
%{_libdir}/libsqlrclientwrapper.so
%{_libdir}/pkgconfig/sqlrelay-c.pc
%exclude %{_libdir}/lib*.la

%postun c-devel
rmdir %{_includedir}/sqlrelay 2> /dev/null || :
rmdir %{_includedir}/sqlrelay/private 2> /dev/null || :

%files dropin-postgresql
%{_libdir}/libpqsqlrelay.so.*
%{_libdir}/libpqsqlrelay.so

%files dropin-mysql
%{_libdir}/libmysql*sqlrelay.so.*
%{_libdir}/libmysql*sqlrelay.so

%files connector-odbc
%{_libdir}/libsqlrodbc.so.*
%{_libdir}/libsqlrodbc.so

%files db2
%{_libexecdir}/sqlrelay/sqlrconnection_db2*

%postun db2
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files freetds
%{_libexecdir}/sqlrelay/sqlrconnection_freetds*

%postun freetds
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files firebird
%{_libexecdir}/sqlrelay/sqlrconnection_firebird*

%postun firebird
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files mdbtools
%{_libexecdir}/sqlrelay/sqlrconnection_mdbtools*

%postun mdbtools
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files mysql
%{_libexecdir}/sqlrelay/sqlrconnection_mysql*

%postun mysql
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files odbc
%{_libexecdir}/sqlrelay/sqlrconnection_odbc*

%postun odbc
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files oracle
%{_libexecdir}/sqlrelay/sqlrconnection_oracle*

%postun oracle
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files postgresql
%{_libexecdir}/sqlrelay/sqlrconnection_postgresql*

%postun postgresql
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files sqlite
%{_libexecdir}/sqlrelay/sqlrconnection_sqlite*

%postun sqlite
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files sap
%{_libexecdir}/sqlrelay/sqlrconnection_sap*

%postun sap
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files informix
%{_libexecdir}/sqlrelay/sqlrconnection_informix*

%postun informix
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files router
%{_libexecdir}/sqlrelay/sqlrconnection_router*
%{_libexecdir}/sqlrelay/sqlrrouter_*

%postun router
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files java
%{_prefix}/java/*

%files perl
%{perl_sitearch}/SQLRelay/Connection.pm
%{perl_sitearch}/SQLRelay/Cursor.pm
%{perl_sitearch}/auto/SQLRelay/Connection
%{perl_sitearch}/auto/SQLRelay/Cursor
%{_datadir}/man/man3/SQLRelay::Connection.3pm*
%{_datadir}/man/man3/SQLRelay::Cursor.3pm*

%files connector-perl-dbi
%{perl_sitelib}/DBD/SQLRelay.pm
%{perl_sitearch}/auto/DBD/SQLRelay
%{_datadir}/man/man3/DBD::SQLRelay.3pm*

%files php
%{php_extdir}/sql_relay.so
%{php_inidir}/sql_relay.ini

%files connector-php-pdo
%{php_extdir}/pdo_sqlrelay.so
%{php_inidir}/pdo_sqlrelay.ini

%files python3
%{python3_sitearch}/SQLRelay/CSQLRelay.so
%{python3_sitearch}/SQLRelay/PySQLRClient.py
%{python3_sitearch}/SQLRelay/__init__.py
%{python3_sitearch}/SQLRelay/__pycache__/PySQLRClient.*
%{python3_sitearch}/SQLRelay/__pycache__/__init__.*

%files connector-python3-db
%{python3_sitearch}/SQLRelay/PySQLRDB.py
%{python3_sitearch}/SQLRelay/__pycache__/PySQLRDB.*

%files ruby
%{ruby_sitearchdir}/sqlrelay.so

%files tcl
%{_libdir}/sqlrelay/*

%files erlang
%{_libdir}/erlang/lib/sqlrelay-%{version}

%files mono
%{_libdir}/SQLRClient.dll
%{_libdir}/SQLRClient.dll.config

%files nodejs
%{nodejs_sitearch}/sqlrelay

%files doc
%{_docdir}/%{name}
%{_datadir}/licenses/%{name}
%{_datadir}/%{name}/examples

%files man
%{_mandir}

%changelog
* Mon Jan 09 2012 David Muse <david.muse@firstworks.com> - 1.0.1-1
- Removed --without options.
- Removed dynamic generation of language-related directory names.
- Added dependency on a specific verion of rudiments.
- Replaced setup with autosetup.
- Added AUTHORS, ChangeLog, and COPYING.
- Updated package names and descriptions to match established conventions.
- Added --disable-static option to configure.
- Added Requires to devel packages.
- Added License to all packages.
- Added --enable-*-on-demand flags to the configure command.
- Added --with-perl/ruby-* flags to the configure command.
- Excluded lib*.la.
- Updated to use perl, python, php, ruby, and nodejs macros.
- Updated to build/package for python3.
- Split out perl-dbi, php-pdo, and python-db drivers into separate packages.

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

