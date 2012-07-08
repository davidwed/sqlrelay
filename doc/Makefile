top_builddir = ..

include ../config.mk

.PHONY: install uninstall

install:
	$(MKINSTALLDIRS) $(docdir)
	$(MKINSTALLDIRS) $(docdir)/admin
	$(MKINSTALLDIRS) $(docdir)/api
	$(MKINSTALLDIRS) $(docdir)/api/c/html
	$(MKINSTALLDIRS) $(docdir)/api/c++/html
	$(MKINSTALLDIRS) $(docdir)/api/perl
	$(MKINSTALLDIRS) $(docdir)/api/php/media
	$(MKINSTALLDIRS) $(docdir)/api/php/sql_relay
	$(MKINSTALLDIRS) $(docdir)/api/python
	$(MKINSTALLDIRS) $(docdir)/api/ruby/doc/classes/SQLRConnection.src
	$(MKINSTALLDIRS) $(docdir)/api/ruby/doc/classes/SQLRCursor.src
	$(MKINSTALLDIRS) $(docdir)/api/ruby/doc/files
	$(MKINSTALLDIRS) $(docdir)/api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/Database.src
	$(MKINSTALLDIRS) $(docdir)/api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/Driver.src
	$(MKINSTALLDIRS) $(docdir)/api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/Statement.src
	$(MKINSTALLDIRS) $(docdir)/api/ruby/dbidoc/files
	$(MKINSTALLDIRS) $(docdir)/api/java
	$(MKINSTALLDIRS) $(docdir)/api/tcl
	$(MKINSTALLDIRS) $(docdir)/dropin
	$(MKINSTALLDIRS) $(docdir)/features
	$(MKINSTALLDIRS) $(docdir)/css
	$(MKINSTALLDIRS) $(docdir)/programming
	$(MKINSTALLDIRS) $(docdir)/programming/zope
	$(MKINSTALLDIRS) $(docdir)/gettingstarted
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/redhat
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/redhat/5
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/redhat/6
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/redhat/7
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/redhat/8
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/redhat/9
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/fedoracore/1
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/fedoracore/2
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/fedoracore/3
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/fedoracore/5
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/slackware
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/slackware/7
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/slackware/8
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/oracle/linux/suse/9.0
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/sybase
	$(MKINSTALLDIRS) $(docdir)/gettingstarted/db2
	$(INSTALL) -m 644 index.html $(docdir)
	$(INSTALL) -m 644 faq.html $(docdir)
	$(INSTALL) -m 644 oraclentier.html $(docdir)
	$(INSTALL) -m 644 admin/configuring.html $(docdir)/admin
	$(INSTALL) -m 644 admin/installing.html $(docdir)/admin
	$(INSTALL) -m 644 admin/loadbalfailover.html $(docdir)/admin
	$(INSTALL) -m 644 admin/router.html $(docdir)/admin
	$(INSTALL) -m 644 admin/router.png $(docdir)/admin
	$(INSTALL) -m 644 admin/running.html $(docdir)/admin
	$(INSTALL) -m 644 admin/tuning.html $(docdir)/admin
	$(INSTALL) -m 644 css/styles.css $(docdir)/css
	for file in `ls api/c/html/*.html api/c/html/*.css api/c/html/*.png api/c/html/*.js`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/c/html; \
	done
	for file in `ls api/c++/html/*.html api/c++/html/*.css api/c++/html/*.png api/c++/html/*.js`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/c++/html; \
	done
	$(INSTALL) -m 644 api/perl/perl.html $(docdir)/api/perl
	$(INSTALL) -m 644 api/perl/Connection.html $(docdir)/api/perl
	$(INSTALL) -m 644 api/perl/Cursor.html $(docdir)/api/perl
	$(INSTALL) -m 644 api/perl/SQLRelay.html $(docdir)/api/perl
	for file in `ls api/php/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/php; \
	done
	for file in `ls api/php/media/*.css`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/php/media; \
	done
	for file in `ls api/php/sql_relay/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/php/sql_relay; \
	done
	$(INSTALL) -m 644 api/python/PySQLRClient.py.html $(docdir)/api/python
	$(INSTALL) -m 644 api/python/PySQLRDB.py.html $(docdir)/api/python
	for file in `ls api/ruby/doc/*.html api/ruby/doc/*.css`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/doc; \
	done
	for file in `ls api/ruby/doc/classes/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/doc/classes; \
	done
	for file in `ls api/ruby/doc/classes/SQLRConnection.src/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/doc/classes/SQLRConnection.src; \
	done
	for file in `ls api/ruby/doc/classes/SQLRCursor.src/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/doc/classes/SQLRCursor.src; \
	done
	for file in `ls api/ruby/doc/files/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/doc/files; \
	done
	for file in `ls api/ruby/dbidoc/*.html api/ruby/dbidoc/*.css`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/dbidoc; \
	done
	for file in `ls api/ruby/dbidoc/classes/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/dbidoc/classes; \
	done
	for file in `ls api/ruby/dbidoc/classes/DBI/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/dbidoc/classes/DBI; \
	done
	for file in `ls api/ruby/dbidoc/classes/DBI/DBD/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/dbidoc/classes/DBI/DBD; \
	done
	for file in `ls api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/dbidoc/classes/DBI/DBD/SQLRelay; \
	done
	for file in `ls api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/Database.src/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/Database.src; \
	done
	for file in `ls api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/Driver.src/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/Driver.src; \
	done
	for file in `ls api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/Statement.src/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/dbidoc/classes/DBI/DBD/SQLRelay/Statement.src; \
	done
	for file in `ls api/ruby/dbidoc/files/*.html`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/ruby/dbidoc/files; \
	done
	$(INSTALL) -m 644 api/java/allclasses-frame.html $(docdir)/api/java
	$(INSTALL) -m 644 api/java/allclasses-noframe.html $(docdir)/api/java
	$(INSTALL) -m 644 api/java/constant-values.html $(docdir)/api/java
	$(INSTALL) -m 644 api/java/deprecated-list.html $(docdir)/api/java
	$(INSTALL) -m 644 api/java/help-doc.html $(docdir)/api/java
	$(INSTALL) -m 644 api/java/index-all.html $(docdir)/api/java
	$(INSTALL) -m 644 api/java/index.html $(docdir)/api/java
	$(INSTALL) -m 644 api/java/overview-tree.html $(docdir)/api/java
	$(INSTALL) -m 644 api/java/package-list $(docdir)/api/java
	$(INSTALL) -m 644 api/java/stylesheet.css $(docdir)/api/java
	$(MKINSTALLDIRS) $(docdir)/api/java/com
	$(MKINSTALLDIRS) $(docdir)/api/java/com/firstworks
	$(MKINSTALLDIRS) $(docdir)/api/java/com/firstworks/sqlrelay
	$(INSTALL) -m 644 api/java/com/firstworks/sqlrelay/package-frame.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(INSTALL) -m 644 api/java/com/firstworks/sqlrelay/package-summary.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(INSTALL) -m 644 api/java/com/firstworks/sqlrelay/package-tree.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(INSTALL) -m 644 api/java/com/firstworks/sqlrelay/SQLRConnection.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(INSTALL) -m 644 api/java/com/firstworks/sqlrelay/SQLRCursor.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(MKINSTALLDIRS) $(docdir)/api/java/resources
	$(INSTALL) -m 644 api/java/resources/inherit.gif $(docdir)/api/java/resources
	for file in `ls api/tcl/*.html api/tcl/*.txt`; \
	do \
		$(INSTALL) -m 644 $$file $(docdir)/api/tcl; \
	done
	$(INSTALL) -m 644 dropin/mysql.html $(docdir)/dropin
	$(INSTALL) -m 644 dropin/postgresql.html $(docdir)/dropin
	$(INSTALL) -m 644 features/connectionpooling.html $(docdir)/features
	$(INSTALL) -m 644 features/databases.html $(docdir)/features
	$(INSTALL) -m 644 features/dbabstractionlayers.html $(docdir)/features
	$(INSTALL) -m 644 features/languages.html $(docdir)/features
	$(INSTALL) -m 644 features/loadbalancing.html $(docdir)/features
	$(INSTALL) -m 644 features/multirowfetches.html $(docdir)/features
	$(INSTALL) -m 644 features/platforms.html $(docdir)/features
	$(INSTALL) -m 644 features/proxying.html $(docdir)/features
	$(INSTALL) -m 644 features/queryrouting.html $(docdir)/features
	$(INSTALL) -m 644 features/resultsetcaching.html $(docdir)/features
	$(INSTALL) -m 644 features/substitutionandbindvars.html $(docdir)/features
	$(INSTALL) -m 644 features/suspendedtx.html $(docdir)/features
	$(INSTALL) -m 644 features/throttling.html $(docdir)/features
	$(INSTALL) -m 644 programming/binds.html $(docdir)/programming
	$(INSTALL) -m 644 programming/odbc.html $(docdir)/programming
	$(INSTALL) -m 644 programming/jdbc.html $(docdir)/programming
	$(INSTALL) -m 644 programming/c++.html $(docdir)/programming
	$(INSTALL) -m 644 programming/c.html $(docdir)/programming
	$(INSTALL) -m 644 programming/perl.html $(docdir)/programming
	$(INSTALL) -m 644 programming/perldbi.html $(docdir)/programming
	$(INSTALL) -m 644 programming/php.html $(docdir)/programming
	$(INSTALL) -m 644 programming/python.html $(docdir)/programming
	$(INSTALL) -m 644 programming/pythondb.html $(docdir)/programming
	$(INSTALL) -m 644 programming/ruby.html $(docdir)/programming
	$(INSTALL) -m 644 programming/rubydbi.html $(docdir)/programming
	$(INSTALL) -m 644 programming/java.html $(docdir)/programming
	$(INSTALL) -m 644 programming/tcl.html $(docdir)/programming
	$(INSTALL) -m 644 programming/zope.html $(docdir)/programming
	$(INSTALL) -m 644 programming/zope/instance.gif $(docdir)/programming/zope
	$(INSTALL) -m 644 programming/zope/management.gif $(docdir)/programming/zope
	$(INSTALL) -m 644 programming/zope/resultset.gif $(docdir)/programming/zope
	$(INSTALL) -m 644 programming/zope/sqlrelay.gif $(docdir)/programming/zope
	$(INSTALL) -m 644 programming/zope/test.gif $(docdir)/programming/zope
	$(INSTALL) -m 644 programming/zope/testzsqlmethod.gif $(docdir)/programming/zope
	$(INSTALL) -m 644 programming/zope/zsqlmethod.gif $(docdir)/programming/zope
	$(INSTALL) -m 644 programming/zope/zsqlmethodresultset.gif $(docdir)/programming/zope
	$(INSTALL) -m 644 gettingstarted/db2.html $(docdir)/gettingstarted
	$(INSTALL) -m 644 gettingstarted/firebird.html $(docdir)/gettingstarted
	$(INSTALL) -m 644 gettingstarted/mssqlserver.html $(docdir)/gettingstarted
	$(INSTALL) -m 644 gettingstarted/mysql.html $(docdir)/gettingstarted
	$(INSTALL) -m 644 gettingstarted/oracle.html $(docdir)/gettingstarted
	$(INSTALL) -m 644 gettingstarted/postgresql.html $(docdir)/gettingstarted
	$(INSTALL) -m 644 gettingstarted/sqlite.html $(docdir)/gettingstarted
	$(INSTALL) -m 644 gettingstarted/sybase.html $(docdir)/gettingstarted
	$(INSTALL) -m 644 gettingstarted/blackray.html $(docdir)/gettingstarted
	$(INSTALL) -m 644 gettingstarted/oracle/linux/redhat/5/805.html $(docdir)/gettingstarted/oracle/linux/redhat/5
	$(INSTALL) -m 644 gettingstarted/oracle/linux/redhat/6/8i.html $(docdir)/gettingstarted/oracle/linux/redhat/6
	$(INSTALL) -m 644 gettingstarted/oracle/linux/redhat/7/8i.html $(docdir)/gettingstarted/oracle/linux/redhat/7
	$(INSTALL) -m 644 gettingstarted/oracle/linux/redhat/8/9iR2.html $(docdir)/gettingstarted/oracle/linux/redhat/8
	$(INSTALL) -m 644 gettingstarted/oracle/linux/redhat/9/9iR2.html $(docdir)/gettingstarted/oracle/linux/redhat/9
	$(INSTALL) -m 644 gettingstarted/oracle/linux/fedoracore/1/9iR2.html $(docdir)/gettingstarted/oracle/linux/fedoracore/1
	$(INSTALL) -m 644 gettingstarted/oracle/linux/fedoracore/1/10g.html $(docdir)/gettingstarted/oracle/linux/fedoracore/1
	$(INSTALL) -m 644 gettingstarted/oracle/linux/fedoracore/2/10g.html $(docdir)/gettingstarted/oracle/linux/fedoracore/2
	$(INSTALL) -m 644 gettingstarted/oracle/linux/fedoracore/3/10g.html $(docdir)/gettingstarted/oracle/linux/fedoracore/3
	$(INSTALL) -m 644 gettingstarted/oracle/linux/fedoracore/5/10g.html $(docdir)/gettingstarted/oracle/linux/fedoracore/5
	$(INSTALL) -m 644 gettingstarted/oracle/linux/slackware/8/8i.html $(docdir)/gettingstarted/oracle/linux/slackware/8
	$(INSTALL) -m 644 gettingstarted/oracle/linux/slackware/8/9i.html $(docdir)/gettingstarted/oracle/linux/slackware/8
	$(INSTALL) -m 644 gettingstarted/oracle/linux/suse/9.0/9iR2.html $(docdir)/gettingstarted/oracle/linux/suse/9.0
	$(INSTALL) -m 644 gettingstarted/sybase/11.0.3.3.html $(docdir)/gettingstarted/sybase/11.0.3.3.html
	$(INSTALL) -m 644 gettingstarted/sybase/11.9.2.html $(docdir)/gettingstarted/sybase/11.9.2.html
	$(INSTALL) -m 644 gettingstarted/sybase/12.5.1.html $(docdir)/gettingstarted/sybase/12.5.1.html
	$(INSTALL) -m 644 gettingstarted/sybase/12.5.2.html $(docdir)/gettingstarted/sybase/12.5.2.html
	$(INSTALL) -m 644 gettingstarted/sybase/15.0.2.html $(docdir)/gettingstarted/sybase/15.0.2.html
	$(INSTALL) -m 644 gettingstarted/db2/72.html $(docdir)/gettingstarted/db2/72.html
	$(INSTALL) -m 644 gettingstarted/db2/81.html $(docdir)/gettingstarted/db2/81.html
	$(INSTALL) -m 644 gettingstarted/db2/824.html $(docdir)/gettingstarted/db2/824.html
	$(INSTALL) -m 644 gettingstarted/db2/82.html $(docdir)/gettingstarted/db2/82.html
	$(INSTALL) -m 644 gettingstarted/db2/91.html $(docdir)/gettingstarted/db2/91.html

uninstall:
	$(RMTREE) $(docdir)
