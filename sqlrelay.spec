# Available build options, you need rpm-build >= 4.0.3 for this to work.
# Example : rpmbuild -ba --without mysql --without php sqlrelay.spec
#
# General options :
# ===============
# --without gtk
#
# Database options :
# ================
# --without db2
# --without freetds
# --without interbase
# --without lago
# --without msql
# --without mysql
# --without odbc
# --without oracle
# --without postgresql
# --without sqlite
# --without sybase
#
# Language options :
# ================
# --without java
# --without perl
# --without php
# --without python
# --without ruby
# --without zope
# --without tcl

Summary: Persistent database connection system.
Name: sqlrelay
Version: 0.33.1
Release: 1
License: GPL/LGPL and Others
Group: System Environment/Daemons
Source0: %{name}-%{version}.tar.gz
URL: http://sqlrelay.sourceforge.net
Buildroot: %{_tmppath}/%{name}-root
BuildRequires: rudiments-devel >= 0.25
%{!?_without_gtk:BuildRequires: ,gtk+-devel}
%{!?_without_mysql:BuildRequires: ,mysql-devel}
%{!?_without_odbc:BuildRequires: ,unixODBC-devel}
%{!?_without_postgresql:BuildRequires: ,postgresql-devel}
%{!?_without_perl:BuildRequires: ,perl}
%{!?_without_php:BuildRequires: ,php-devel}
%{!?_without_python:BuildRequires: ,python-devel}
%{!?_without_ruby:BuildRequires: ,ruby-devel}
%{!?_without_zope:BuildRequires: ,python-devel}
%{!?_without_tcl:BuildRequires: ,tcl}

%description
SQL Relay is a persistent database connection pooling, proxying and load 
balancing system for Unix and Linux supporting ODBC, Oracle, MySQL, mSQL, 
PostgreSQL, Sybase, MS SQL Server, IBM DB2, Interbase, Lago and SQLite with C, 
C++, Perl, Perl-DBD, Python, Python-DB, Zope, PHP, Ruby, Java and TCL APIs,
command line clients, a GUI configuration tool and extensive documentation.
The APIs support advanced database operations such as bind variables, multi-row
fetches, client side result set caching and suspended transactions.  It is
ideal for speeding up database-driven web-based applications, accessing
databases from unsupported platforms, migrating between databases, distributing
access to replicated databases and throttling database access.


%package clients
Summary: Command line applications for accessing databases through SQL Relay.
Group: Applications/Database

%description clients
Command line applications for accessing databases through SQL Relay.


%package clients-debug
Summary: Command line applications for accessing databases through SQL Relay compiled with debugging support.
Group: Applications/Database

%description clients-debug
Command line applications for accessing databases through SQL Relay compiled
with debugging support.


%package client-runtime-c++
Summary: Runtime libraries for SQL Relay clients written in C++.
Group: Applications/Libraries

%description client-runtime-c++
Runtime libraries for SQL Relay clients written in C++.


%package client-runtime-c++-debug
Summary: Runtime libraries for SQL Relay clients written in C++ compiled with debugging support.
Group: Applications/Libraries

%description client-runtime-c++-debug
Runtime libraries for SQL Relay clients written in C++ compield with debugging
support.


%package client-runtime-c
Summary: Runtime libraries for SQL Relay clients written in C.
Group: Applications/Libraries

%description client-runtime-c
Runtime libraries for SQL Relay clients written in C.


%package client-runtime-c-debug
Summary: Runtime libraries for SQL Relay clients written in C compiled with debugging support.
Group: Applications/Libraries

%description client-runtime-c-debug
Runtime libraries for SQL Relay clients written in C compiled with debugging
support.


%package client-devel-c++
Summary: Development files for developing programs C++ that use SQL Relay.
Group: Development/Libraries

%description client-devel-c++
Header files and static libraries to use for developing programs in C++ that
use SQL Relay.


%package client-devel-c++-debug
Summary: Development files for developing programs C++ that use SQL Relay compiled with debugging support.
Group: Development/Libraries

%description client-devel-c++-debug
Header files and static libraries to use for developing programs in C++ that
use SQL Relay compiled with debugging support.


%package client-devel-c
Summary: Development files for developing programs C that use SQL Relay.
Group: Development/Libraries

%description client-devel-c
Header files and static libraries to use for developing programs in C that
use SQL Relay.


%package client-devel-c-debug
Summary: Development files for developing programs C that use SQL Relay compiled with debugging support.
Group: Development/Libraries

%description client-devel-c-debug
Header files and static libraries to use for developing programs in C that
use SQL Relay compiled with debugging support.


%package config-gtk
Summary: SQL Relay GUI configuration tool.
Group: Applications/Databases

%description config-gtk
GTK-based configuration tool for SQL Relay.


%package db2
Summary: SQL Relay connection daemon for IBM DB2.
Group: Applications/Databases

%description db2
SQL Relay connection daemon for IBM DB2.


%package freetds
Summary: SQL Relay connection daemon for FreeTDS (Sybase and MS SQL Server).
Group: Applications/Databases

%description freetds
SQL Relay connection daemon for FreeTDS (Sybase and MS SQL Server).


%package interbase
Summary: SQL Relay connection daemon for Interbase.
Group: Applications/Databases

%description interbase
SQL Relay connection daemon for Interbase.


%package lago
Summary: SQL Relay connection daemon for Lago.
Group: Applications/Databases

%description lago
SQL Relay connection daemon for Lago.


%package msql
Summary: SQL Relay connection daemon for mSQL.
Group: Applications/Databases

%description msql
SQL Relay connection daemon for mSQL.


%package mysql
Summary: SQL Relay connection daemon for MySQL.
Group: Applications/Databases

%description mysql
SQL Relay connection daemon for MySQL.


%package odbc
Summary: SQL Relay connection daemon for ODBC.
Group: Applications/Databases

%description odbc
SQL Relay connection daemon for ODBC.


%package oracle7
Summary: SQL Relay connection daemon for Oracle 7.
Group: Applications/Databases

%description oracle7
SQL Relay connection daemon for Oracle 7.


%package oracle8
Summary: SQL Relay connection daemon for Oracle 8.
Group: Applications/Databases

%description oracle8
SQL Relay connection daemon for Oracle 8.


%package postgresql
Summary: SQL Relay connection daemon for PostgreSQL.
Group: Applications/Databases

%description postgresql
SQL Relay connection daemon for PostgreSQL.


%package sqlite
Summary: SQL Relay connection daemon for SQLite.
Group: Applications/Databases

%description sqlite
SQL Relay connection daemon for SQLite.


%package sybase
Summary: SQL Relay connection daemon for Sybase.
Group: Applications/Databases

%description sybase
SQL Relay connection daemon for Sybase.


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


%package zope
Summary: SQL Relay modules for Zope.
Group: Development/Languages

%description zope
SQL Relay modules for Zope.


%package tcl
Summary: SQL Relay modules for TCL.
Group: Development/Languages

%description tcl
SQL Relay modules for TCL.


%package doc
Summary: Documentation for SQL Relay.
Group: Applications/Database

%description doc
Documentation for SQL Relay.


%define tcldir		/usr/lib/tcl
%define pythondir	%(echo -e "import sys\\nimport string\\nout=''\\nfor i in sys.path:\\n if len(i)>0:\\n  for j in range(0,len(i)):\\n   if j<len(i)-1:\\n    out=out+i[j]\\n   else:\\n    if i[j]!='/':\\n     out=out+i[j]\\n  break\\nprint out" | python)
%define	zopedir		/opt/Zope/lib/python/Products
%define	phpextdir	%(php-config --extension-dir)
%define	perl_sitelib	%(eval "export `perl -V:sitelib`"; echo $sitelib)
%define	perl_installman3dir	%(eval "export `perl -V:installman3dir`"; echo $installman3dir)
%define	perl_man3ext	%(eval "export `perl -V:man3ext`"; echo $man3ext)

%define ruby_sitelibdir	%(ruby -e 'require "mkmf"' -e 'drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/' -e 'print "arch = "' -e 'print CONFIG["arch"]' -e 'print "\\n"' -e 'print "ruby_version = "' -e 'print Config::CONFIG["ruby_version"]' -e 'print "\\n"' -e 'print "prefix = "' -e 'print with_destdir CONFIG["prefix"].sub(drive, "")' -e 'print "\\n"' -e 'print "exec_prefix = "' -e 'print with_destdir CONFIG["exec_prefix"].sub(drive, "")' -e 'print "\\n"' -e 'print "libdir = "' -e 'print with_destdir $libdir.sub(drive, "")' -e 'print "\\n"' -e 'print "rubylibdir = "' -e 'print with_destdir $rubylibdir.sub(drive, "")' -e 'print "\\n"' -e 'print "archdir = "' -e 'print with_destdir $archdir.sub(drive, "")' -e 'print "\\n"' -e 'print "sitedir = "' -e 'print with_destdir $sitedir.sub(drive, "")' -e 'print "\\n"' -e 'print "sitelibdir = "' -e 'print with_destdir $sitelibdir.sub(drive, "")' -e 'print "\\n\\n"' -e 'print "all:\\n"' -e 'print "	echo $(sitelibdir)\\n"' | make -s -f - )

%define ruby_sitearchdir	%(ruby -e 'require "mkmf"' -e 'drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/' -e 'print "arch = "' -e 'print CONFIG["arch"]' -e 'print "\\n"' -e 'print "ruby_version = "' -e 'print Config::CONFIG["ruby_version"]' -e 'print "\\n"' -e 'print "prefix = "' -e 'print with_destdir CONFIG["prefix"].sub(drive, "")' -e 'print "\\n"' -e 'print "exec_prefix = "' -e 'print with_destdir CONFIG["exec_prefix"].sub(drive, "")' -e 'print "\\n"' -e 'print "libdir = "' -e 'print with_destdir $libdir.sub(drive, "")' -e 'print "\\n"' -e 'print "rubylibdir = "' -e 'print with_destdir $rubylibdir.sub(drive, "")' -e 'print "\\n"' -e 'print "archdir = "' -e 'print with_destdir $archdir.sub(drive, "")' -e 'print "\\n"' -e 'print "sitedir = "' -e 'print with_destdir $sitedir.sub(drive, "")' -e 'print "\\n"' -e 'print "sitelibdir = "' -e 'print with_destdir $sitelibdir.sub(drive, "")' -e 'print "\\n"' -e 'print "sitearchdir = "' -e 'print with_destdir $sitearchdir.sub(drive, "")' -e 'print "\\n\\n"' -e 'print "all:\\n"' -e 'print "	echo $(sitearchdir)\\n"' | make -s -f - )

%prep
%setup -q

%build
%configure \
	%{?_without_gtk:	--disable-gtk} \
	%{?_without_db2:	--disable-db2} \
	%{?_without_freetds:	--disable-freetds} \
	%{?_without_interbase:	--disable-interbase} \
	%{?_without_lago:	--disable-lago} \
	%{?_without_msql:	--disable-msql} \
	%{?_without_mysql:	--disable-mysql} \
	%{?_without_odbc:	--disable-odbc} \
	%{?_without_oracle:	--disable-oracle} \
	%{?_without_postgresql:	--disable-postgresql} \
	%{?_without_sqlite:	--disable-sqlite} \
	%{?_without_sybase:	--disable-sybase} \
	%{?_without_java:	--disable-java} \
	%{?_without_tcl:	--disable-tcl} \
	%{?_without_perl:	--disable-perl} \
	%{?_without_php:	--disable-php} \
	%{?_without_python:	--disable-python} \
	%{?_without_ruby:	--disable-ruby} \
	%{?_without_zope:	--disable-zope}
	
make

%install
rm -rf %{buildroot}
# must install everything except ruby first because the "prefix" environment
# variable screws up the ruby install
%makeinstall \
	incdir=%{buildroot}%{_includedir} \
	PYTHONDIR=%{buildroot}%{pythondir} \
	ZOPEDIR=%{buildroot}%{zopedir} \
	PHPEXTDIR=%{buildroot}%{phpextdir} \
	TCLLIBSPATH=%{buildroot}%{tcldir} \
	PREFIX=%{buildroot}%{_prefix} \
	HAVE_RUBY="" \
	initroot=%{buildroot}
# now install ruby
%{!?_without_ruby: %makeinstall DESTDIR=%{buildroot}}
# remove crap from doc dir
rm -f doc/Makefile
rm -rf doc/*/CVS
rm -rf doc/*/*/CVS
rm -rf doc/*/*/*/CVS
rm -rf doc/*/*/*/*/CVS
rm -rf doc/*/*/*/*/*/CVS

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
%config %attr(600, root, root) %{_sysconfdir}/sqlrelay.conf.example
%config %attr(600, root, root) %{_sysconfdir}/sqlrelay.dtd
%config(noreplace) %attr(600, root, root) /etc/sysconfig/sqlrelay
/etc/init.d/sqlrelay
%{_bindir}/sqlr-cachemanager
%{_bindir}/sqlr-listener
%{_bindir}/sqlr-listener-debug
%{_bindir}/sqlr-scaler
%{_bindir}/sqlr-start
%{_bindir}/sqlr-stop
%{_libdir}/libsqlrconnection-*.so.*
%{_libdir}/libsqlrconnection_p-*.so.*
%{_libdir}/libsqlrutil-*.so.*
%{_libdir}/libsqlrutil_p-*.so.*
%{_localstatedir}/sqlrelay/tmp
%{_localstatedir}/sqlrelay/debug

%files clients
%defattr(-, root, root)
%{_bindir}/backupschema
%{_bindir}/fields
%{_bindir}/query
%{_bindir}/sqlrsh

%files clients-debug
%defattr(-, root, root)
%{_bindir}/fields-debug
%{_bindir}/query-debug
%{_bindir}/sqlrsh-debug

%files client-runtime-c++
%defattr(-, root, root)
%{_libdir}/libsqlrclient-*.so.*
%{_localstatedir}/sqlrelay/cache

%files client-runtime-c++-debug
%defattr(-, root, root)
%{_libdir}/libsqlrclient_p-*.so.*

%files client-runtime-c
%defattr(-, root, root)
%{_libdir}/libsqlrclientwrapper-*.so.*

%files client-runtime-c-debug
%defattr(-, root, root)
%{_libdir}/libsqlrclientwrapper_p-*.so.*

%files client-devel-c++
%defattr(-, root, root)
%{_includedir}/sqlrelay/sqlrclient.h
%{_includedir}/sqlrelay/private
%{_libdir}/libsqlrclient.a
%{_libdir}/libsqlrclient.la
%{_libdir}/libsqlrclient.so

%files client-devel-c++-debug
%defattr(-, root, root)
%{_libdir}/libsqlrclient_p.a
%{_libdir}/libsqlrclient_p.la
%{_libdir}/libsqlrclient_p.so

%files client-devel-c
%defattr(-, root, root)
%{_includedir}/sqlrelay/sqlrclientwrapper.h
%{_libdir}/libsqlrclientwrapper.a
%{_libdir}/libsqlrclientwrapper.la
%{_libdir}/libsqlrclientwrapper.so

%files client-devel-c-debug
%defattr(-, root, root)
%{_libdir}/libsqlrclientwrapper_p.a
%{_libdir}/libsqlrclientwrapper_p.la
%{_libdir}/libsqlrclientwrapper_p.so

%{!?_without_gtk:%files config-gtk}
%{!?_without_gtk:%defattr(-, root, root)}
%{!?_without_gtk:%{_bindir}/sqlr-config-gtk}

%{!?_without_db2:%files db2}
%{!?_without_db2:%defattr(-, root, root)}
%{!?_without_db2:%{_bindir}/sqlr-connection-db2*}

%{!?_without_freetds:%files freetds}
%{!?_without_freetds:%defattr(-, root, root)}
%{!?_without_freetds:%{_bindir}/sqlr-connection-freetds*}

%{!?_without_interbase:%files interbase}
%{!?_without_interbase:%defattr(-, root, root)}
%{!?_without_interbase:%{_bindir}/sqlr-connection-interbase*}

%{!?_without_lago:%files lago}
%{!?_without_lago:%defattr(-, root, root)}
%{!?_without_lago:%{_bindir}/sqlr-connection-lago*}

%{!?_without_msql:%files msql}
%{!?_without_msql:%defattr(-, root, root)}
%{!?_without_msql:%{_bindir}/sqlr-connection-msql*}

%{!?_without_mysql:%files mysql}
%{!?_without_mysql:%defattr(-, root, root)}
%{!?_without_mysql:%{_bindir}/sqlr-connection-mysql*}

%{!?_without_odbc:%files odbc}
%{!?_without_odbc:%defattr(-, root, root)}
%{!?_without_odbc:%{_bindir}/sqlr-connection-odbc*}

%{!?_without_oracle:%files oracle7}
%{!?_without_oracle:%defattr(-, root, root)}
%{!?_without_oracle:%{_bindir}/sqlr-connection-oracle7*}

%{!?_without_oracle8:%files oracle8}
%{!?_without_oracle8:%defattr(-, root, root)}
%{!?_without_oracle8:%{_bindir}/sqlr-connection-oracle8*}

%{!?_without_postgresql:%files postgresql}
%{!?_without_postgresql:%defattr(-, root, root)}
%{!?_without_postgresql:%{_bindir}/sqlr-connection-postgresql*}

%{!?_without_sqlite:%files sqlite}
%{!?_without_sqlite:%defattr(-, root, root)}
%{!?_without_sqlite:%{_bindir}/sqlr-connection-sqlite*}

%{!?_without_sybase:%files sybase}
%{!?_without_sybase:%defattr(-, root, root)}
%{!?_without_sybase:%{_bindir}/sqlr-connection-sybase*}

%{!?_without_java:%files java}
%{!?_without_java:%defattr(-, root, root)}
%{!?_without_java:%{_prefix}/java/*}

%{!?_without_perl:%files perl}
%{!?_without_perl:%defattr(-, root, root)}
%{!?_without_perl:%{perl_sitelib}/DBD/SQLRelay.pm}
%{!?_without_perl:%{perl_sitearch}/Firstworks}
%{!?_without_perl:%{perl_sitearch}/auto/Firstworks}
%{!?_without_perl:%{perl_installman3dir}/*.%{perl_man3ext}*}

%{!?_without_php:%files php}
%{!?_without_php:%defattr(-, root, root)}
%{!?_without_php:%{phpextdir}/sql_relay.so}

%{!?_without_python:%files python}
%{!?_without_python:%defattr(-, root, root)}
%{!?_without_python:%{pythondir}/site-packages/SQLRelay}

%{!?_without_ruby:%files ruby}
%{!?_without_ruby:%defattr(-, root, root)}
%{!?_without_ruby:%{ruby_sitearchdir}/*.so}
%{!?_without_ruby:%{ruby_sitelibdir}/DBD/SQLRelay}

%{!?_without_zope:%files zope}
%{!?_without_zope:%defattr(-, root, root)}
%{!?_without_zope:%{zopedir}/ZSQLRelayDA}

%{!?_without_tcl:%files tcl}
%{!?_without_tcl:%defattr(-, root, root)}
%{!?_without_tcl:%{tcldir}/sqlrelay/*}

%files doc
%doc doc

%changelog
* Mon Feb 17 2003 David Muse <mused@firstworks.com>
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

