%{!?tcl_version: %global tcl_version %(echo 'puts $tcl_version' | tclsh)}
%{!?tcl_sitearch: %global tcl_sitearch %{_libdir}/tcl%{tcl_version}}

Name: sqlrelay
Version: 1.6.0
Release: 1%{?dist}
Summary: Database proxy

License: GPLv2 with exceptions
URL: http://sqlrelay.sourceforge.net
Source0: http://downloads.sourceforge.net/%{name}/%{name}-%{version}.tar.gz

%{?systemd_requires}
BuildRequires: gcc-c++, rudiments-devel >= 1.2.0, systemd

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


%package cachemanager
License: GPLv2 with exceptions
Summary: SQL Relay client-side result-set cache manager

%description cachemanager
SQL Relay client-side result-set cache manager


%package common
License: GPLv2 with exceptions
Summary: Components used by SQL Relay client-side and server-side programs

%description common
Components used by SQL Relay client-side and server-side programs.


%package common-devel
License: GPLv2 with exceptions
Summary: Development files for SQL Relay common components.

%description common-devel
Development files for SQL Relay common components.


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
Requires: %{name}-c++%{?_isa} = %{version}-%{release}, rudiments-devel >= 1.2.0

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
%if 0%{?fedora}
BuildRequires: perl-generators
%endif
BuildRequires: perl, perl-devel

%description -n perl-%{name}
Perl bindings for the SQL Relay client API.


%package -n perl-DBD-%{name}
License: Artistic
Summary: Perl DBI driver for SQL Relay
%if 0%{?fedora}
BuildRequires: perl-generators
%endif
BuildRequires: perl, perl-devel
Requires: perl-%{name}%{?_isa} = %{version}-%{release}

%description -n perl-DBD-%{name}
Perl DBI driver for SQL Relay.


%if 0%{?fedora} || 0%{?rhel} > 7

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

%else

%package -n python2-%{name}
License: ZPL 1.0 or MIT
Summary: Python bindings for the SQL Relay client API
BuildRequires: python-devel

%description -n python2-%{name}
Python bindings for the SQL Relay client API.


%package -n python2-db-%{name}
License: ZPL 1.0 or MIT
Summary: Python DB bindings for SQL Relay
BuildRequires: python-devel
Requires: python-%{name}%{?_isa} = %{version}-%{release}

%description -n python2-db-%{name}
Python DB bindings for SQL Relay.

%endif


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
%if 0%{?fedora}
Requires: tcl(abi) = 8.6
%else
Requires: tcl(abi) = 8.5
%endif

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
BuildRequires: mariadb-devel

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
FreeTDS back-end module for SQL Relay.


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
MDB Tools back-end module for SQL Relay.


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
Requires: %{name}-javadoc

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
		--with-perl-site-lib=%{perl_vendorlib} \
		--with-perl-site-arch=%{perl_vendorarch} \
		--with-ruby-site-arch-dir=%{ruby_vendorarchdir} \
		--with-default-runasuser=sqlrelay \
		--with-default-runasgroup=sqlrelay
make

%install
make install DESTDIR=%{buildroot}

# move systemd files to (_unitdir)
mkdir -p %{buildroot}%{_unitdir}
mv %{buildroot}/lib/systemd/system/* %{buildroot}%{_unitdir}

# create tmpfiles.d directories and config file
mkdir -p %{buildroot}/run/%{name}
mkdir -p %{buildroot}%{_tmpfilesdir}
echo "d /run/%{name} 0775 root root -" > %{buildroot}%{_tmpfilesdir}/%{name}.conf

# move tcl modules to (tcl_sitearch)/(name)
mkdir -p %{buildroot}%{tcl_sitearch}
mv %{buildroot}%{_libdir}/%{name} %{buildroot}%{tcl_sitearch}/%{name}


# move mono assembly to (libdir)/(name)
mkdir -p %{buildroot}%{_libdir}/%{name}
mv %{buildroot}%{_libdir}/SQLRClient.dll %{buildroot}%{_libdir}/%{name}
mv %{buildroot}%{_libdir}/SQLRClient.dll.config %{buildroot}%{_libdir}/%{name}


# .move jar files to (_javadir)
mkdir -p %{buildroot}%{_javadir}
mv %{buildroot}%{_prefix}/java/*.jar %{buildroot}%{_javadir}

# move jni shared object files to (_libdir)/(name)
mkdir -p %{buildroot}%{_libdir}/%{name}
mv %{buildroot}%{_prefix}/java/com/firstworks/%{name}/*.so %{buildroot}%{_libdir}/%{name}
rm -rf %{buildroot}%{_prefix}/java

# copy java documentation to (_javadocdir)/(name)
mkdir -p %{buildroot}%{_javadocdir}
cp -r %{buildroot}%{_docdir}/%{name}/api/java %{buildroot}%{_javadocdir}/%{name}

%pre
# Add the "sqlrelay" user
/usr/sbin/useradd -c "SQL Relay" -s /bin/false \
	-r -m -d %{_localstatedir}/%{name} %{name} 2> /dev/null || :


%post
%systemd_post %{name}.service

%preun
%systemd_preun %{name}.service

%postun
%systemd_postun_with_restart %{name}.service


%files
%{_sysconfdir}/%{name}.conf.d
%{_sysconfdir}/%{name}.xsd
%{_unitdir}/%{name}.service
%{_bindir}/sqlr-listener
%{_bindir}/sqlr-connection
%{_bindir}/sqlr-scaler
%{_bindir}/sqlr-start
%{_bindir}/sqlr-stop
%{_bindir}/sqlr-pwdenc
%{_libdir}/libsqlrserver.so.8
%{_libdir}/libsqlrserver.so.8.*
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrauth_*
%{_libexecdir}/%{name}/sqlrbindvariabletranslation_*
%{_libexecdir}/%{name}/sqlrconfig_*
%{_libexecdir}/%{name}/sqlrfilter_*
%{_libexecdir}/%{name}/sqlrtrigger_*
%{_libexecdir}/%{name}/sqlrnotification_*
%{_libexecdir}/%{name}/sqlrparser_*
%{_libexecdir}/%{name}/sqlrprotocol_*
%{_libexecdir}/%{name}/sqlrpwdenc_*
%{_libexecdir}/%{name}/sqlrlogger_*
%{_libexecdir}/%{name}/sqlrmoduledata_*
%{_libexecdir}/%{name}/sqlrquery_*
%{_libexecdir}/%{name}/sqlrresultsettranslation_*
%{_libexecdir}/%{name}/sqlrschedule_*
%{_libexecdir}/%{name}/sqlrtranslation_*
%{_libexecdir}/%{name}/sqlrdirective_*
%{_mandir}/*/sqlr-listener.*
%{_mandir}/*/sqlr-connection.*
%{_mandir}/*/sqlr-scaler.*
%{_mandir}/*/sqlr-start.*
%{_mandir}/*/sqlr-stop.*
%{_mandir}/*/sqlr-pwdenc.*
%doc AUTHORS ChangeLog
%attr(755, sqlrelay, sqlrelay) %dir %{_localstatedir}/log/%{name}
%attr(755, sqlrelay, sqlrelay) %dir /run/%{name}
%{_tmpfilesdir}/%{name}.conf
%exclude %{_libdir}/lib*.la
%if 0%{?fedora}
%license COPYING
%exclude %{_datadir}/licenses/%{name}
%else
%{_datadir}/licenses/%{name}
%endif
%exclude %{_localstatedir}/run

%files server-devel
%{_bindir}/sqlrserver-config
%dir %{_includedir}/%{name}
%dir %{_includedir}/%{name}/private
%{_includedir}/%{name}/sqlrserver.h
%{_includedir}/%{name}/private/sqlrauth.h
%{_includedir}/%{name}/private/sqlrauths.h
%{_includedir}/%{name}/private/sqlrbindvariabletranslation.h
%{_includedir}/%{name}/private/sqlrbindvariabletranslations.h
%{_includedir}/%{name}/private/sqlrfilter.h
%{_includedir}/%{name}/private/sqlrfilters.h
%{_includedir}/%{name}/private/sqlrgsscredentials.h
%{_includedir}/%{name}/private/sqlrlistener.h
%{_includedir}/%{name}/private/sqlrlogger.h
%{_includedir}/%{name}/private/sqlrloggers.h
%{_includedir}/%{name}/private/sqlrmysqlcredentials.h
%{_includedir}/%{name}/private/sqlrmoduledata.h
%{_includedir}/%{name}/private/sqlrmoduledatas.h
%{_includedir}/%{name}/private/sqlrnotification.h
%{_includedir}/%{name}/private/sqlrnotifications.h
%{_includedir}/%{name}/private/sqlrparser.h
%{_includedir}/%{name}/private/sqlrprotocol.h
%{_includedir}/%{name}/private/sqlrprotocols.h
%{_includedir}/%{name}/private/sqlrpwdenc.h
%{_includedir}/%{name}/private/sqlrpwdencs.h
%{_includedir}/%{name}/private/sqlrqueries.h
%{_includedir}/%{name}/private/sqlrquerycursor.h
%{_includedir}/%{name}/private/sqlrquery.h
%{_includedir}/%{name}/private/sqlrresultsetrowblocktranslation.h
%{_includedir}/%{name}/private/sqlrresultsetrowblocktranslations.h
%{_includedir}/%{name}/private/sqlrresultsetrowtranslation.h
%{_includedir}/%{name}/private/sqlrresultsetrowtranslations.h
%{_includedir}/%{name}/private/sqlrresultsettranslation.h
%{_includedir}/%{name}/private/sqlrresultsettranslations.h
%{_includedir}/%{name}/private/sqlrrouter.h
%{_includedir}/%{name}/private/sqlrrouters.h
%{_includedir}/%{name}/private/sqlrschedule.h
%{_includedir}/%{name}/private/sqlrschedulerule.h
%{_includedir}/%{name}/private/sqlrschedules.h
%{_includedir}/%{name}/private/sqlrserverconnection.h
%{_includedir}/%{name}/private/sqlrservercontroller.h
%{_includedir}/%{name}/private/sqlrservercursor.h
%{_includedir}/%{name}/private/sqlrserverincludes.h
%{_includedir}/%{name}/private/sqlrshm.h
%{_includedir}/%{name}/private/sqlrtlscredentials.h
%{_includedir}/%{name}/private/sqlrtranslation.h
%{_includedir}/%{name}/private/sqlrtranslations.h
%{_includedir}/%{name}/private/sqlrtrigger.h
%{_includedir}/%{name}/private/sqlrtriggers.h
%{_includedir}/%{name}/private/sqlruserpasswordcredentials.h
%{_includedir}/%{name}/private/sqlrdirective.h
%{_includedir}/%{name}/private/sqlrdirectives.h
%{_includedir}/%{name}/private/sqlrresultsetheadertranslation.h
%{_includedir}/%{name}/private/sqlrresultsetheadertranslations.h
%{_libdir}/libsqlrserver.so
%exclude %{_libdir}/lib*.la

%files clients
%{_bindir}/sqlrsh
%{_bindir}/sqlr-export
%{_bindir}/sqlr-import
%{_bindir}/sqlr-status
%{_mandir}/*/sqlrsh.*
%{_mandir}/*/sqlr-export.*
%{_mandir}/*/sqlr-import.*
%{_mandir}/*/sqlr-status.*

%files cachemanager
%{_unitdir}/sqlrcachemanager.service
%{_bindir}/sqlr-cachemanager
%{_mandir}/*/sqlr-cachemanager.*
%attr(755, sqlrelay, sqlrelay) %dir %{_localstatedir}/cache/%{name}

%post cachemanager
%systemd_post %{name}cachemanager.service

%preun cachemanager
%systemd_preun %{name}cachemanager.service

%files common
%{_libdir}/libsqlrutil.so.8
%{_libdir}/libsqlrutil.so.8.*

%files common-devel
%dir %{_includedir}/%{name}
%dir %{_includedir}/%{name}/private
%{_includedir}/%{name}/sqlrutil.h
%{_includedir}/%{name}/private/sqlrutilincludes.h
%{_libdir}/libsqlrutil.so

%files c++
%{_libdir}/libsqlrclient.so.5
%{_libdir}/libsqlrclient.so.5.*

%files c
%{_libdir}/libsqlrclientwrapper.so.5
%{_libdir}/libsqlrclientwrapper.so.5.*

%files c++-devel
%{_bindir}/sqlrclient-config
%dir %{_includedir}/%{name}
%dir %{_includedir}/%{name}/private
%{_includedir}/%{name}/sqlrclient.h
%{_includedir}/%{name}/private/sqlrclientincludes.h
%{_includedir}/%{name}/private/sqlrconnection.h
%{_includedir}/%{name}/private/sqlrcursor.h
%{_libdir}/libsqlrclient.so
%{_libdir}/pkgconfig/%{name}-c++.pc
%exclude %{_libdir}/lib*.la

%files c-devel
%{_bindir}/sqlrclientwrapper-config
%dir %{_includedir}/%{name}
%dir %{_includedir}/%{name}/private
%{_includedir}/%{name}/sqlrclientwrapper.h
%{_includedir}/%{name}/private/sqlrclientwrapper.h
%{_includedir}/%{name}/private/sqlrclientwrapperincludes.h
%{_libdir}/libsqlrclientwrapper.so
%{_libdir}/pkgconfig/%{name}-c.pc
%exclude %{_libdir}/lib*.la

%files -n odbc-%{name}
%{_libdir}/libsqlrodbc.so.5
%{_libdir}/libsqlrodbc.so.5.*
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


%if 0%{?fedora}

%files -n python3-%{name}
%dir %{python3_sitearch}/SQLRelay/__pycache__
%dir %{python3_sitearch}/SQLRelay
%{python3_sitearch}/SQLRelay/CSQLRelay.so
%{python3_sitearch}/SQLRelay/PySQLRClient.py
%{python3_sitearch}/SQLRelay/__init__.py
%{python3_sitearch}/SQLRelay/__pycache__/PySQLRClient.*
%{python3_sitearch}/SQLRelay/__pycache__/__init__.*

%files -n python3-db-%{name}
%dir %{python3_sitearch}/SQLRelay/__pycache__
%dir %{python3_sitearch}/SQLRelay
%{python3_sitearch}/SQLRelay/PySQLRDB.py
%{python3_sitearch}/SQLRelay/__pycache__/PySQLRDB.*

%else

%files -n python2-%{name}
%dir %{python_sitearch}/SQLRelay
%{python_sitearch}/SQLRelay/CSQLRelay.so
%{python_sitearch}/SQLRelay/PySQLRClient.py*
%{python_sitearch}/SQLRelay/__init__.py*

%files -n python2-db-%{name}
%dir %{python_sitearch}/SQLRelay
%{python_sitearch}/SQLRelay/PySQLRDB.py*

%endif


%files -n ruby-%{name}
%{ruby_vendorarchdir}/%{name}.so

%files -n php-%{name}
%{php_extdir}/sql_relay.so
%{php_inidir}/sql_relay.ini

%files -n php-pdo-%{name}
%{php_extdir}/pdo_%{name}.so
%{php_inidir}/pdo_%{name}.ini

%files -n java-%{name}
%{_javadir}/*.jar
%dir %{_libdir}/%{name}
%{_libdir}/%{name}/*.so

%files -n tcl-%{name}
%{tcl_sitearch}/%{name}


%files -n erlang-%{name}
%{_libdir}/erlang/lib/%{name}-%{version}

%files -n mono-data-%{name}
%dir %{_libdir}/%{name}
%{_libdir}/%{name}/SQLRClient.dll
%{_libdir}/%{name}/SQLRClient.dll.config

%files -n nodejs-%{name}
%{nodejs_sitearch}/%{name}


%files dropin-mysql
%{_libdir}/libmysql*%{name}.so.*

%files dropin-postgresql
%{_libdir}/libpq%{name}.so.*

%files oracle
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_oracle*

%files mysql
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_mysql*

%files postgresql
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_postgresql*

%files sqlite
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_sqlite*


%files freetds
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_freetds*


%files sap
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_sap*

%files odbc
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_odbc*

%files db2
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_db2*


%files firebird
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_firebird*

%files mdbtools
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_mdbtools*


%files informix
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_informix*

%files router
%dir %{_libexecdir}/%{name}
%{_libexecdir}/%{name}/sqlrconnection_router*
%{_libexecdir}/%{name}/sqlrrouter_*

%files doc
%{_docdir}/%{name}
%{_datadir}/licenses/%{name}
%{_datadir}/%{name}

%files javadoc
%{_javadocdir}/%{name}

%changelog
* Fri Jun 28 2019 David Muse <david.muse@firstworks.com> - 1.6.0-1
- Updated to version 1.6.0.
- Replaced python-* names with python2-*.
- Updated to build python 3 packages for rhel > 7.
- Updated to build Nodejs and Erlang packages on RHEL.
- Updated to build Firebird, FreeTDS, and MDB Tools modules on RHEL.

* Thu Feb 21 2019 David Muse <david.muse@firstworks.com> - 1.5.0-1
- Replaced empty-directory-removing postun's with dir's.
- Updated to require rudiments 1.2.0.
- Removed globbing of library major versions.
- Removed calls to /sbin/ldconfig.
- Updated to version 1.5.0.
- Added bind variable translation and module data components.
- Added mysql-frontend components.
- Added triggers.

* Wed Sep 05 2018 David Muse <david.muse@firstworks.com> - 1.4.0-1
- Updated to version 1.4.0.
- Updated to require rudiments 1.0.8.
- Added resultsetrowblocktranslations headers.

* Wed May 16 2018 David Muse <david.muse@firstworks.com> - 1.3.0-1
- Updated to version 1.3.0.
- Updated to require rudiments 1.0.7.
- Added directive module headers and shared objects.
- Added resultsetheadertranslation headers.

* Fri Sep 22 2017 David Muse <david.muse@firstworks.com> - 1.2.0-1
- Updated required version of rudiments-devel to 1.0.5.

* Fri Feb 17 2017 David Muse <david.muse@firstworks.com> - 1.1.0-1
- Added fedora dist-tag conditionals.
- Created sqlrelay-cachemanager subpackage.
- Created sqlrelay-common and sqlrelay-common-devel subpackages.

* Mon Jan 09 2017 David Muse <david.muse@firstworks.com> - 1.0.1-1
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
- Added postin/postun with calls to /sbin/ldconfig for all library subpackages.
- Excluded .so files from dropin-postgresql and dropin-mysql subpackages.
- Added tmpfiles.d configuration.
- Made log, cache, and run owned by the sqlrelay group and gave them 775 perms.

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

