%{!?tcl_version: %global tcl_version %(echo 'puts $tcl_version' | tclsh)}
%{!?tcl_sitearch: %global tcl_sitearch %{_libdir}/tcl%{tcl_version}}

Name: sqlrelay
Version: 1.0.1
Release: 1%{?dist}
Summary: Database proxy

License: GPLv2 with exceptions
URL: http://sqlrelay.sourceforge.net
Source0: http://downloads.sourceforge.net/%{name}/%{name}-%{version}.tar.gz

%{?systemd_requires}
BuildRequires: rudiments-devel >= 1.0.3, systemd

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
Command line clients for accessing databases via SQL Relay.


%package c++
License: LGPLv2
Summary: The SQL Relay C++ client library

%description c++
The SQL Relay C++ client library.


%package c
License: LGPLv2
Summary: The SQL Relay C client library

%description c
The SQL Relay C client library.


%package c++-devel
License: LGPLv2
Summary: Development files for the SQL Relay C++ client library
Requires: %{name}-c++%{?_isa} = %{version}-%{release}, rudiments-devel >= 1.0.3

%description c++-devel
Development files for the SQL Relay C++ client library.


%package c-devel
License: LGPLv2
Summary: Development files for the SQL Relay C client library
Requires: %{name}-c%{?_isa} = %{version}-%{release}, %{name}-c++-devel%{?_isa} = %{version}-%{release}

%description c-devel
Development files for the SQL Relay C client library.


%package -n odbc-%{name}
License: LGPLv2
Summary: ODBC driver for SQL Relay
BuildRequires: unixODBC-devel

%description -n odbc-%{name}
ODBC driver for SQL Relay.


%package -n perl-%{name}
License: Artistic
Summary: Perl bindings for the SQL Relay client API
BuildRequires: perl-generators, perl, perl-devel

%description -n perl-%{name}
Perl bindings for the SQL Relay client API.


%package -n perl-DBD-%{name}
License: Artistic
Summary: Perl DBI driver for SQL Relay
BuildRequires: perl-generators, perl, perl-devel
Requires: perl-%{name}%{?_isa} = %{version}-%{release}

%description -n perl-DBD-%{name}
Perl DBI driver for SQL Relay.


%package -n python3-%{name}
License: ZPL 1.0 or MIT
Summary: Python bindings for the SQL Relay client API
BuildRequires: python3-devel

%description -n python3-%{name}
Python bindings for the SQL Relay client API.


%package -n python3-db-%{name}
License: ZPL 1.0 or MIT
Summary: Python DB bindings for SQL Relay
BuildRequires: python3-devel
Requires: python3-%{name}%{?_isa} = %{version}-%{release}

%description -n python3-db-%{name}
Python DB bindings for SQL Relay.


%package -n ruby-%{name}
License: LGPLv2
Summary: Ruby bindings for the SQL Relay client API.
BuildRequires: ruby-devel
Requires: ruby(release)

%description -n ruby-%{name}
Ruby bindings for the SQL Relay client API.


%package -n php-%{name}
License: LGPLv2
Summary: PHP bindings for the SQL Relay client API
BuildRequires: php-devel
Requires: php(zend-abi) = %{php_zend_api}, php(api) = %{php_core_api}

%description -n php-%{name}
PHP bindings for the SQL Relay client API.


%package -n php-pdo-%{name}
License: LGPLv2
Summary: PHP PDO driver for SQL Relay.
BuildRequires: php-devel
Requires: php(zend-abi) = %{php_zend_api}, php(api) = %{php_core_api}

%description -n php-pdo-%{name}
PHP PDO driver for SQL Relay.


%package -n java-%{name}
License: LGPLv2
Summary: Java bindings for the SQL Relay client API
BuildRequires: java-devel
Requires: java-headless, javapackages-tools

%description -n java-%{name}
Java bindings for the SQL Relay client API.


%package -n tcl-%{name}
License: LGPLv2
Summary: TCL bindings for the SQL Relay client API
BuildRequires: tcl-devel
Requires: tcl(abi) = 8.6

%description -n tcl-%{name}
TCL bindings for the SQL Relay client API.


%package -n erlang-%{name}
License: CC-BY
Summary: Erlang bindings for the SQL Relay client API
BuildRequires: erlang
Requires: erlang

%description -n erlang-%{name}
Erlang bindings for the SQL Relay client API.


%package -n mono-data-%{name}
License: LGPLv2
Summary: Mono bindings for the SQL Relay client API and ADO.NET driver
ExclusiveArch: %{mono_arches}
BuildRequires: mono-devel, mono-data, mono-data-oracle
Requires: mono-core, mono-data, mono-data-oracle

%description -n mono-data-%{name}
Mono bindings for the SQL Relay client API and ADO.NET driver.


%package -n nodejs-%{name}
License: LGPLv2
Summary: Nodejs bindings for the SQL Relay client API
ExclusiveArch: %{nodejs_arches}
BuildRequires: nodejs-packaging, node-gyp, nodejs-devel

%description -n nodejs-%{name}
Nodejs bindings for the SQL Relay client API.


%package dropin-mysql
License: GPLv2
Summary: Drop in replacement library that redirects MySQL clients to SQL Relay

%description dropin-mysql
Drop in replacement library that redirects MySQL clients to SQL Relay.


%package dropin-postgresql
License: PostgreSQL
Summary: Drop in replacement library that redirects PostgreSQL clients to SQL Relay
BuildRequires: postgresql-devel

%description dropin-postgresql
Drop in replacement library that redirects PostgreSQL clients to SQL Relay.


%package oracle
License: GPLv2 with exceptions
Summary: Oracle back-end module for SQL Relay

%description oracle
Oracle back-end module for SQL Relay.


%package mysql
License: GPLv2 with exceptions
Summary: MySQL back-end module for SQL Relay
BuildRequires: mysql-devel

%description mysql
MySQL back-end module for SQL Relay.


%package postgresql
License: GPLv2 with exceptions
Summary: PostgreSQL back-end module for SQL Relay

%description postgresql
PostgreSQL back-end module for SQL Relay.


%package sqlite
License: GPLv2 with exceptions
Summary: SQLite back-end module for SQL Relay
BuildRequires: sqlite-devel

%description sqlite
SQLite back-end module for SQL Relay.


%package freetds
License: GPLv2 with exceptions
Summary: FreeTDS back-end module for SQL Relay
BuildRequires: freetds-devel

%description freetds
FreeTDS back-end module for SQL Relay.  Enables access to SAP/Sybase and MS SQL Server databases.


%package sap
License: GPLv2 with exceptions
Summary: SAP/Sybase back-end module for SQL Relay

%description sap
SAP/Sybase back-end module for SQL Relay.


%package odbc
License: GPLv2 with exceptions
Summary: ODBC back-end module for SQL Relay
BuildRequires: unixODBC-devel

%description odbc
ODBC back-end module for SQL Relay.


%package db2
License: GPLv2 with exceptions
Summary: IBM DB2 back-end module for SQL Relay

%description db2
IBM DB2 back-end module for SQL Relay.


%package firebird
License: GPLv2 with exceptions
Summary: Firebird back-end module for SQL Relay
BuildRequires: firebird-devel

%description firebird
Firebird back-end module for SQL Relay.


%package mdbtools
License: GPLv2 with exceptions
Summary: MDB Tools back-end module for SQL Relay
BuildRequires: mdbtools-devel

%description mdbtools
MDB Tools back-end module for SQL Relay.  Enables read-only access to Microsoft Access databases.


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


%package doc
# Documentation is GPLv2 except for example code in the documentation.
# Example code is FSFUL.
License: GPLv2 and FSFUL
Summary: Documentation for SQL Relay
BuildArch: noarch
Requires: sqlrelay-javadoc

%description doc
Documentation for SQL Relay.


%package javadoc
License: GPLv2
Summary: API documentation for SQL Relay
BuildArch: noarch

%description javadoc
API documentation for SQL Relay.


%prep
%autosetup -p1

%build
%configure --disable-static \
		--enable-oracle-at-runtime \
		--enable-sap-at-runtime \
		--enable-db2-at-runtime \
		--enable-informix-at-runtime \
		--disable-python \
		--with-perl-site-lib=%{perl_vendorlib} \
		--with-perl-site-arch=%{perl_vendorarch} \
		--with-ruby-site-arch-dir=%{ruby_vendorarchdir}
# modify libtool to avoid unused-direct-shlib-dependency errors
sed -i -e 's! -shared ! -Wl,--as-needed\0!g' libtool
make

%install
make install DESTDIR=%{buildroot}
# move systemd files to (_unitdir)
mkdir -p %{buildroot}%{_unitdir}
mv %{buildroot}/lib/systemd/system/* %{buildroot}%{_unitdir}
# move tcl modules to (tcl_sitearch)/(name)
mkdir -p %{buildroot}%{tcl_sitearch}
mv %{buildroot}%{_libdir}/sqlrelay %{buildroot}%{tcl_sitearch}/%{name}
# move mono assembly to (libdir)/(name)
mkdir -p %{buildroot}%{_libdir}/%{name}
mv %{buildroot}%{_libdir}/SQLRClient.dll %{buildroot}%{_libdir}/%{name}
mv %{buildroot}%{_libdir}/SQLRClient.dll.config %{buildroot}%{_libdir}/%{name}
# .move jar files to (_javadir)
mkdir -p %{buildroot}%{_javadir}
mv %{buildroot}%{_prefix}/java/*.jar %{buildroot}%{_javadir}
# move jni shared object files to (_libdir)/(name)
mkdir -p %{buildroot}%{_libdir}/%{name}
mv %{buildroot}%{_prefix}/java/com/firstworks/sqlrelay/*.so %{buildroot}%{_libdir}/%{name}
rm -rf %{buildroot}%{_prefix}/java
# copy java documentation to (_javadocdir)/(name)
mkdir -p %{buildroot}%{_javadocdir}
cp -r %{buildroot}%{_docdir}/%{name}/api/java %{buildroot}%{_javadocdir}/%{name}

%pre
# Add the "sqlrelay" user
/usr/sbin/useradd -c "SQL Relay" -s /bin/false \
	-r -d %{_localstatedir}/sqlrelay sqlrelay 2> /dev/null || :

%post
/sbin/ldconfig
%systemd_post %{name}.service
%systemd_post %{name}cachemanager.service

%preun
%systemd_preun %{name}.service
%systemd_preun %{name}cachemanager.service

%postun
/sbin/ldconfig
%systemd_postun_with_restart %{name}.service
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :
rm -rf %{_localstatedir}/sqlrelay/tmp
rmdir %{_localstatedir}/sqlrelay 2> /dev/null || :


%files
%{_sysconfdir}/sqlrelay.conf.d
%config %attr(600, root, root) %{_sysconfdir}/sqlrelay.xsd
%{_unitdir}/sqlrelay.service
%{_unitdir}/sqlrcachemanager.service
%{_bindir}/sqlr-cachemanager
%{_bindir}/sqlr-listener
%{_bindir}/sqlr-connection
%{_bindir}/sqlr-scaler
%{_bindir}/sqlr-start
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
%{_mandir}/*/sqlr-cachemanager.*
%{_mandir}/*/sqlr-listener.*
%{_mandir}/*/sqlr-connection.*
%{_mandir}/*/sqlr-scaler.*
%{_mandir}/*/sqlr-start.*
%{_mandir}/*/sqlr-stop.*
%{_mandir}/*/sqlr-pwdenc.*
%doc AUTHORS ChangeLog
%license COPYING
%exclude %{_libdir}/lib*.la
%exclude %{_datadir}/licenses/sqlrelay

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
%{_bindir}/sqlr-status
%{_mandir}/*/sqlrsh.*
%{_mandir}/*/sqlr-export.*
%{_mandir}/*/sqlr-import.*
%{_mandir}/*/sqlr-status.*

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

%files -n odbc-%{name}
%{_libdir}/libsqlrodbc.so.*
%{_libdir}/libsqlrodbc.so

%files -n perl-%{name}
%{perl_vendorarch}/*
%{_mandir}/*/SQLRelay::Connection.*
%{_mandir}/*/SQLRelay::Cursor.*
%exclude %{perl_vendorarch}/auto/DBD/
%exclude %{perl_vendorarch}/auto/SQLRelay/*/.packlist
%exclude %{perl_vendorarch}/auto/SQLRelay/*/*.bs
%exclude %dir %{perl_vendorarch}/auto/

%files -n perl-DBD-%{name}
%{perl_vendorlib}/*
%{_mandir}/*/DBD::SQLRelay.*

%files -n python3-%{name}
%{python3_sitearch}/SQLRelay/CSQLRelay.so
%{python3_sitearch}/SQLRelay/PySQLRClient.py
%{python3_sitearch}/SQLRelay/__init__.py
%{python3_sitearch}/SQLRelay/__pycache__/PySQLRClient.*
%{python3_sitearch}/SQLRelay/__pycache__/__init__.*

%postun -n python3-%{name}
rmdir %{python3_sitearch}/SQLRelay/__pycache__ 2> /dev/null || :
rmdir %{python3_sitearch}/SQLRelay 2> /dev/null || :

%files -n python3-db-%{name}
%{python3_sitearch}/SQLRelay/PySQLRDB.py
%{python3_sitearch}/SQLRelay/__pycache__/PySQLRDB.*

%files -n ruby-%{name}
%{ruby_vendorarchdir}/sqlrelay.so

%files -n php-%{name}
%{php_extdir}/sql_relay.so
%{php_inidir}/sql_relay.ini

%files -n php-pdo-%{name}
%{php_extdir}/pdo_sqlrelay.so
%{php_inidir}/pdo_sqlrelay.ini

%files -n java-%{name}
%{_javadir}/*.jar
%{_libdir}/%{name}/*.so

%files -n tcl-%{name}
%{tcl_sitearch}/sqlrelay

%files -n erlang-%{name}
%{_libdir}/erlang/lib/%{name}-%{version}

%files -n mono-data-%{name}
%{_libdir}/%{name}/SQLRClient.dll
%{_libdir}/%{name}/SQLRClient.dll.config

%files -n nodejs-%{name}
%{nodejs_sitearch}/sqlrelay

%files dropin-mysql
%{_libdir}/libmysql*sqlrelay.so.*
%{_libdir}/libmysql*sqlrelay.so

%files dropin-postgresql
%{_libdir}/libpqsqlrelay.so.*
%{_libdir}/libpqsqlrelay.so

%files oracle
%{_libexecdir}/sqlrelay/sqlrconnection_oracle*

%postun oracle
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files mysql
%{_libexecdir}/sqlrelay/sqlrconnection_mysql*

%postun mysql
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files postgresql
%{_libexecdir}/sqlrelay/sqlrconnection_postgresql*

%postun postgresql
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files sqlite
%{_libexecdir}/sqlrelay/sqlrconnection_sqlite*

%postun sqlite
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files freetds
%{_libexecdir}/sqlrelay/sqlrconnection_freetds*

%postun freetds
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files sap
%{_libexecdir}/sqlrelay/sqlrconnection_sap*

%postun sap
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files odbc
%{_libexecdir}/sqlrelay/sqlrconnection_odbc*

%postun odbc
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files db2
%{_libexecdir}/sqlrelay/sqlrconnection_db2*

%postun db2
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files firebird
%{_libexecdir}/sqlrelay/sqlrconnection_firebird*

%postun firebird
rmdir %{_libexecdir}/sqlrelay 2> /dev/null || :

%files mdbtools
%{_libexecdir}/sqlrelay/sqlrconnection_mdbtools*

%postun mdbtools
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

%files doc
%{_docdir}/%{name}
%{_datadir}/licenses/%{name}
%{_datadir}/%{name}/examples

%files javadoc
%{_javadocdir}/%{name}

%changelog
* Mon Jan 09 2012 David Muse <david.muse@firstworks.com> - 1.0.1-1
- Removed --without options.
- Removed dynamic generation of language-related directory names.
- Added dependency on a specific verion of rudiments.
- Replaced setup with autosetup.
- Added AUTHORS, ChangeLog, and COPYING.
- Updated package names and descriptions to match guidelines.
- Added --disable-static option to configure.
- Added Requires to devel packages.
- Added License to all packages.
- Updated license names to match guidelines.
- Added --enable-*-on-demand flags to the configure command.
- Added --with-perl/ruby-* flags to the configure command.
- Excluded lib*.la.
- Updated to use perl, python, php, ruby, and nodejs macros.
- Updated to build/package for python3.
- Split out perl-dbi, php-pdo, and python-db drivers into separate packages.
- Reordered package definitions.
- Updated package definitions to use macros.
- Updated install target to move various files to the required places.
- Separated javadocs into their own subpackage.
- Combined the man pages with the subpackages containing their programs.

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

