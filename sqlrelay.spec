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
# --without mdbtools
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
Version: 0.36
Release: 3
License: GPL/LGPL and Others
Group: System Environment/Daemons
Source0: %{name}-%{version}.tar.gz
URL: http://sqlrelay.sourceforge.net
Buildroot: %{_tmppath}/%{name}-root

%if %([[ %{_vendor} == "suse" ]] && echo 1 || echo 0)
	%define phpdevel %(echo "mod_php4-devel")
	%define gtkdevel %(echo "gtk-devel")
	%define rubydevel %(echo "ruby")
	%define tcldevel %(echo "tcl-devel")
	%define initscript /etc/init.d/sqlrelay
	%define inittab /etc/sqlrelay
	%define docdir %{_docdir}/%{name}
%else
	%define phpdevel %(echo "php-devel")
	%define gtkdevel %(echo "gtk+-devel")
	%define rubydevel %(echo "ruby-devel")
	%define tcldevel %(echo "tcl")
	%define initscript /etc/rc.d/init.d/sqlrelay
	%define inittab /etc/sysconfig/sqlrelay
	%define docdir %{_docdir}/%{name}-%{version}
%endif

BuildRequires: rudiments-devel >= 0.28.1
%{!?_without_gtk:BuildRequires: ,%{gtkdevel}}
%{!?_without_mysql:BuildRequires: ,mysql-devel}
%{!?_without_odbc:BuildRequires: ,unixODBC-devel}
%{!?_without_postgresql:BuildRequires: ,postgresql-devel}
%{!?_without_perl:BuildRequires: ,perl}
%{!?_without_php:BuildRequires: ,%{phpdevel}}
%{!?_without_python:BuildRequires: ,python-devel}
%{!?_without_ruby:BuildRequires: ,%{rubydevel}}
%{!?_without_zope:BuildRequires: ,python-devel}
%{!?_without_tcl:BuildRequires: ,%{tcldevel}}

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
Summary: Development files for developing programs C++ that use SQL Relay.
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


%package mdbtools
Summary: SQL Relay connection daemon for MDB Tools (Microsoft Access).
Group: Applications/Databases

%description mdbtools
SQL Relay connection daemon for MDB Tools (Microsoft Access).


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


%package man
Summary: Man pages for SQL Relay.
Group: Applications/Database

%description man
Man pages for SQL Relay.


%define	tcldir		%(dirname `rpm -q -l %{tcldevel} | grep tclConfig.sh`)
%ifarch x86_64
%define	pythondir	%(PYTHONINCLUDES=""; PYTHONDIR=""; for j in "2.3" "2.2" "2.1" "2.0" "1.6" "1.5"; do for i in "/usr/include/python$j" "/usr/local/include/python$j" "/usr/pkg/include/python$j" "/usr/local/python$j/include/python$j" "/opt/sfw/include/python$j"; do if ( test -d "$i" ); then PYTHONINCLUDES="$i"; fi; if ( test -n "$PYTHONINCLUDES" ); then break; fi; done; for i in "/usr/lib64/python$j" "/usr/local/lib64/python$j" "/usr/pkg/lib64/python$j" "/usr/local/python$j/lib64/python$j" "/opt/sfw/lib64/python$j"; do if ( test -d "$i" ); then PYTHONDIR="$i"; fi; if ( test -n "$PYTHONDIR" ); then break; fi; done; if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" ); then echo $PYTHONDIR; break; fi; done)
%else
%define	pythondir	%(PYTHONINCLUDES=""; PYTHONDIR=""; for j in "2.3" "2.2" "2.1" "2.0" "1.6" "1.5"; do for i in "/usr/include/python$j" "/usr/local/include/python$j" "/usr/pkg/include/python$j" "/usr/local/python$j/include/python$j" "/opt/sfw/include/python$j"; do if ( test -d "$i" ); then PYTHONINCLUDES="$i"; fi; if ( test -n "$PYTHONINCLUDES" ); then break; fi; done; for i in "/usr/lib/python$j" "/usr/local/lib/python$j" "/usr/pkg/lib/python$j" "/usr/local/python$j/lib/python$j" "/opt/sfw/lib/python$j"; do if ( test -d "$i" ); then PYTHONDIR="$i"; fi; if ( test -n "$PYTHONDIR" ); then break; fi; done; if ( test -n "$PYTHONINCLUDES" -a -n "$PYTHONDIR" ); then echo $PYTHONDIR; break; fi; done)
%endif
%define	zopedir		%(ZOPEPATH="/opt/Zope/lib/python/Proucts"; for i in "/usr/local/www" "/usr/share" "/usr/local" "/usr" "/usr/lib" "/opt"; do for j in "zope" "Zope"; do if ( test -d "$i/$j" ); then ZOPEPATH="$i/$j/lib/python/Products"; fi; done; done; echo $ZOPEPATH)
%define	phpextdir	%(php-config --extension-dir)
%define	phppeardbdir	%(echo "`php-config --prefix`/share/pear/DB")
%define	perl_prefix	%(eval "export `perl -V:prefix`"; echo $prefix)
%define	perl_sitelib	%(eval "export `perl -V:sitelib`"; echo $sitelib)
%define	perl_installarchlib	%(eval "export `perl -V:installarchlib`"; echo $installarchlib)
%define	perl_installsitearch	%(eval "export `perl -V:installsitearch`"; echo $installsitearch)
%define	perl_sitearch	%(eval "export `perl -V:sitearch`"; echo $sitearch)
%define	perl_installman3dir	%(eval "export `perl -V:installman3dir`"; echo $installman3dir)
%define	perl_man3ext	%(eval "export `perl -V:man3ext`"; echo $man3ext)

%define	ruby_sitelibdir	%(ruby -e 'require "mkmf"' -e 'drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/' -e 'print "arch = "' -e 'print CONFIG["arch"]' -e 'print "\\n"' -e 'print "ruby_version = "' -e 'print Config::CONFIG["ruby_version"]' -e 'print "\\n"' -e 'print "prefix = "' -e 'print with_destdir(CONFIG["prefix"].sub(drive, ""))' -e 'print "\\n"' -e 'print "exec_prefix = "' -e 'print with_destdir(CONFIG["exec_prefix"].sub(drive, ""))' -e 'print "\\n"' -e 'print "libdir = "' -e 'print with_destdir($libdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "rubylibdir = "' -e 'print with_destdir($rubylibdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "archdir = "' -e 'print with_destdir($archdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "sitedir = "' -e 'print with_destdir($sitedir.sub(drive, ""))' -e 'print "\\n"' -e 'print "sitelibdir = "' -e 'print with_destdir($sitelibdir.sub(drive, ""))' -e 'print "\\n\\n"' -e 'print "all:\\n"' -e 'print "	echo $(sitelibdir)\\n"' | make -s -f - )

%define	ruby_sitearchdir	%(ruby -e 'require "mkmf"' -e 'drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/' -e 'print "arch = "' -e 'print CONFIG["arch"]' -e 'print "\\n"' -e 'print "sitearch = "' -e 'print CONFIG["sitearch"]' -e 'print "\\n"' -e 'print "ruby_version = "' -e 'print Config::CONFIG["ruby_version"]' -e 'print "\\n"' -e 'print "prefix = "' -e 'print with_destdir(CONFIG["prefix"].sub(drive, ""))' -e 'print "\\n"' -e 'print "exec_prefix = "' -e 'print with_destdir(CONFIG["exec_prefix"].sub(drive, ""))' -e 'print "\\n"' -e 'print "libdir = "' -e 'print with_destdir($libdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "rubylibdir = "' -e 'print with_destdir($rubylibdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "archdir = "' -e 'print with_destdir($archdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "sitedir = "' -e 'print with_destdir($sitedir.sub(drive, ""))' -e 'print "\\n"' -e 'print "sitelibdir = "' -e 'print with_destdir($sitelibdir.sub(drive, ""))' -e 'print "\\n"' -e 'print "sitearchdir = "' -e 'print with_destdir($sitearchdir.sub(drive, ""))' -e 'print "\\n\\n"' -e 'print "all:\\n"' -e 'print "	echo $(sitearchdir)\\n"' | make -s -f - )

%prep
%setup -q

%build
%configure \
	%{?_without_gtk:	--disable-gtk} \
	%{?_without_db2:	--disable-db2} \
	%{?_without_freetds:	--disable-freetds} \
	%{?_without_interbase:	--disable-interbase} \
	%{?_without_lago:	--disable-lago} \
	%{?_without_mdbtools:	--disable-mdbtools} \
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
make DESTDIR=%{buildroot} docdir=%{buildroot}%{docdir} install
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
%config %attr(600, root, root) %{_sysconfdir}/sqlrelay.conf.example
%config %attr(600, root, root) %{_sysconfdir}/sqlrelay.dtd
%config(noreplace) %attr(600, root, root) %{inittab}
%{initscript}
%{_bindir}/sqlr-cachemanager*
%{_bindir}/sqlr-listener*
%{_bindir}/sqlr-scaler*
%{_bindir}/sqlr-start*
%{_bindir}/sqlr-stop
%{_libdir}/libsqlrconnection*
%{_libdir}/libsqlrutil*
%{_localstatedir}/sqlrelay/tmp
%{_localstatedir}/sqlrelay/debug

%files clients
%defattr(-, root, root)
%{_bindir}/backupschema
%{_bindir}/fields
%{_bindir}/query
%{_bindir}/sqlrsh

%files client-runtime-c++
%defattr(-, root, root)
%{_libdir}/libsqlrclient-*.so.*
%{_localstatedir}/sqlrelay/cache

%files client-runtime-c
%defattr(-, root, root)
%{_libdir}/libsqlrclientwrapper-*.so.*

%files client-devel-c++
%defattr(-, root, root)
%{_bindir}/sqlrclient-config
%{_includedir}/sqlrelay/sqlrclient.h
%{_includedir}/sqlrelay/private
%{_libdir}/libsqlrclient.a
%{_libdir}/libsqlrclient.la
%{_libdir}/libsqlrclient.so
%{_libdir}/pkgconfig/sqlrelay-c++.pc

%files client-devel-c
%defattr(-, root, root)
%{_bindir}/sqlrclientwrapper-config
%{_includedir}/sqlrelay/sqlrclientwrapper.h
%{_libdir}/libsqlrclientwrapper.a
%{_libdir}/libsqlrclientwrapper.la
%{_libdir}/libsqlrclientwrapper.so
%{_libdir}/pkgconfig/sqlrelay-c.pc

%files client-postgresql
%defattr(-, root, root)
%{_libdir}/libpqsqlrelay-*.so.*
%{_libdir}/libpqsqlrelay.a
%{_libdir}/libpqsqlrelay.la

%files client-mysql
%defattr(-, root, root)
%{_libdir}/libmysql*sqlrelay-*.so.*
%{_libdir}/libmysql*sqlrelay.a
%{_libdir}/libmysql*sqlrelay.la

%{!?_without_gtk:%files config-gtk}
%{!?_without_gtk:%defattr(-, root, root)}
%{!?_without_gtk:%{_bindir}/sqlr-config-gtk*}

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

%{!?_without_mdbtools:%files mdbtools}
%{!?_without_mdbtools:%defattr(-, root, root)}
%{!?_without_mdbtools:%{_bindir}/sqlr-connection-mdbtools*}

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

%{!?_without_oracle:%files oracle8}
%{!?_without_oracle:%defattr(-, root, root)}
%{!?_without_oracle:%{_bindir}/sqlr-connection-oracle8*}

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
%{!?_without_perl:%{perl_sitearch}/auto/DBD/SQLRelay}
%{!?_without_perl:%{perl_sitearch}/SQLRelay/Connection.pm}
%{!?_without_perl:%{perl_sitearch}/SQLRelay/Cursor.pm}
%{!?_without_perl:%{perl_sitearch}/auto/SQLRelay/Connection}
%{!?_without_perl:%{perl_sitearch}/auto/SQLRelay/Cursor}
%{!?_without_perl:%{perl_installman3dir}/*.%{perl_man3ext}*}

%{!?_without_php:%files php}
%{!?_without_php:%defattr(-, root, root)}
%{!?_without_php:%{phpextdir}/sql_relay.so}
%{!?_without_php:%{phppeardbdir}/sqlrelay.php}

%{!?_without_python:%files python}
%{!?_without_python:%defattr(-, root, root)}
%{!?_without_python:%{pythondir}/site-packages/SQLRelay}

%{!?_without_ruby:%files ruby}
%{!?_without_ruby:%defattr(-, root, root)}
%{!?_without_ruby:%{ruby_sitearchdir}/sqlrelay.so}
%{!?_without_ruby:%{ruby_sitelibdir}/DBD/SQLRelay}

%{!?_without_zope:%files zope}
%{!?_without_zope:%defattr(-, root, root)}
%{!?_without_zope:%{zopedir}/ZSQLRelayDA}

%{!?_without_tcl:%files tcl}
%{!?_without_tcl:%defattr(-, root, root)}
%{!?_without_tcl:%{tcldir}/sqlrelay/*}

%files doc
%{docdir}

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

