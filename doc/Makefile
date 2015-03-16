top_builddir = ..

include ../config.mk

all:

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
	$(MKINSTALLDIRS) $(docdir)/api/java
	$(MKINSTALLDIRS) $(docdir)/api/tcl
	$(MKINSTALLDIRS) $(docdir)/dropin
	$(MKINSTALLDIRS) $(docdir)/features
	$(MKINSTALLDIRS) $(docdir)/images
	$(MKINSTALLDIRS) $(docdir)/js
	$(MKINSTALLDIRS) $(docdir)/css
	$(MKINSTALLDIRS) $(docdir)/modules
	$(MKINSTALLDIRS) $(docdir)/programming
	$(CP) index.html $(docdir)
	$(CHMOD) 644 $(docdir)/index.html
	$(CP) faq.html $(docdir)
	$(CHMOD) 644 $(docdir)/faq.html
	$(CP) admin/configguide.html $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/configguide.html
	$(CP) admin/configreference.html $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/configreference.html
	$(CP) admin/installing.html $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/installing.html
	$(CP) admin/loadbalfailover.html $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/loadbalfailover.html
	$(CP) admin/oraclentier.html $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/oraclentier.html
	$(CP) admin/router.html $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/router.html
	$(CP) admin/router.png $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/router.png
	$(CP) admin/running.html $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/running.html
	$(CP) admin/tuning.html $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/tuning.html
	$(CP) css/styles.css $(docdir)/css
	$(CHMOD) 644 $(docdir)/css/styles.css
	for file in `ls api/c/html/*.html api/c/html/*.css api/c/html/*.png api/c/html/*.js`; \
	do \
		$(CP) $$file $(docdir)/api/c/html; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	for file in `ls api/c++/html/*.html api/c++/html/*.css api/c++/html/*.png api/c++/html/*.js`; \
	do \
		$(CP) $$file $(docdir)/api/c++/html; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	$(CP) api/perl/perl.html $(docdir)/api/perl
	$(CHMOD) 644 $(docdir)/api/perl/perl.html
	$(CP) api/perl/Connection.html $(docdir)/api/perl
	$(CHMOD) 644 $(docdir)/api/perl/Connection.html
	$(CP) api/perl/Cursor.html $(docdir)/api/perl
	$(CHMOD) 644 $(docdir)/api/perl/Cursor.html
	$(CP) api/perl/SQLRelay.html $(docdir)/api/perl
	$(CHMOD) 644 $(docdir)/api/perl/SQLRelay.html
	for file in `ls api/php/*.html`; \
	do \
		$(CP) $$file $(docdir)/api/php; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	for file in `ls api/php/media/*.css`; \
	do \
		$(CP) $$file $(docdir)/api/php/media; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	for file in `ls api/php/sql_relay/*.html`; \
	do \
		$(CP) $$file $(docdir)/api/php/sql_relay; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	$(CP) api/python/PySQLRClient.py.html $(docdir)/api/python
	$(CHMOD) 644 $(docdir)/api/python/PySQLRClient.py.html
	$(CP) api/python/PySQLRDB.py.html $(docdir)/api/python
	$(CHMOD) 644 $(docdir)/api/python/PySQLRDB.py.html
	for file in `ls api/ruby/doc/*.html api/ruby/doc/*.css`; \
	do \
		$(CP) $$file $(docdir)/api/ruby/doc; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	for file in `ls api/ruby/doc/classes/*.html`; \
	do \
		$(CP) $$file $(docdir)/api/ruby/doc/classes; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	for file in `ls api/ruby/doc/classes/SQLRConnection.src/*.html`; \
	do \
		$(CP) $$file $(docdir)/api/ruby/doc/classes/SQLRConnection.src; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	for file in `ls api/ruby/doc/classes/SQLRCursor.src/*.html`; \
	do \
		$(CP) $$file $(docdir)/api/ruby/doc/classes/SQLRCursor.src; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	for file in `ls api/ruby/doc/files/*.html`; \
	do \
		$(CP) $$file $(docdir)/api/ruby/doc/files; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	$(CP) api/java/allclasses-frame.html $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/allclasses-frame.html
	$(CP) api/java/allclasses-noframe.html $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/allclasses-noframe.html
	$(CP) api/java/constant-values.html $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/constant-values.html
	$(CP) api/java/deprecated-list.html $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/deprecated-list.html
	$(CP) api/java/help-doc.html $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/help-doc.html
	$(CP) api/java/index-all.html $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/index-all.html
	$(CP) api/java/index.html $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/index.html
	$(CP) api/java/overview-tree.html $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/overview-tree.html
	$(CP) api/java/package-list $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/package-list
	$(CP) api/java/stylesheet.css $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/stylesheet.css
	$(MKINSTALLDIRS) $(docdir)/api/java/com
	$(MKINSTALLDIRS) $(docdir)/api/java/com/firstworks
	$(MKINSTALLDIRS) $(docdir)/api/java/com/firstworks/sqlrelay
	$(CP) api/java/com/firstworks/sqlrelay/package-frame.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(CHMOD) 644 $(docdir)/api/java/com/firstworks/sqlrelay/package-frame.html
	$(CP) api/java/com/firstworks/sqlrelay/package-summary.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(CHMOD) 644 $(docdir)/api/java/com/firstworks/sqlrelay/package-summary.html
	$(CP) api/java/com/firstworks/sqlrelay/package-tree.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(CHMOD) 644 $(docdir)/api/java/com/firstworks/sqlrelay/package-tree.html
	$(CP) api/java/com/firstworks/sqlrelay/SQLRConnection.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(CHMOD) 644 $(docdir)/api/java/com/firstworks/sqlrelay/SQLRConnection.html
	$(CP) api/java/com/firstworks/sqlrelay/SQLRCursor.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(CHMOD) 644 $(docdir)/api/java/com/firstworks/sqlrelay/SQLRCursor.html
	$(MKINSTALLDIRS) $(docdir)/api/java/resources
	$(CP) api/java/resources/inherit.gif $(docdir)/api/java/resources
	$(CHMOD) 644 $(docdir)/api/java/resources/inherit.gif
	for file in `ls api/tcl/*.html api/tcl/*.txt`; \
	do \
		$(CP) $$file $(docdir)/api/tcl; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	$(CP) dropin/mysql.html $(docdir)/dropin
	$(CHMOD) 644 $(docdir)/dropin/mysql.html
	$(CP) dropin/postgresql.html $(docdir)/dropin
	$(CHMOD) 644 $(docdir)/dropin/postgresql.html
	$(CP) features/connectionpooling.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/connectionpooling.html
	$(CP) features/databases.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/databases.html
	$(CP) features/dbabstractionlayers.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/dbabstractionlayers.html
	$(CP) features/dropinreplacementlibs.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/dropinreplacementlibs.html
	$(CP) features/languages.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/languages.html
	$(CP) features/loadbalancing.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/loadbalancing.html
	$(CP) features/multirowfetches.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/multirowfetches.html
	$(CP) features/proxying.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/proxying.html
	$(CP) features/queryrouting.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/queryrouting.html
	$(CP) features/resultsetcaching.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/resultsetcaching.html
	$(CP) features/substitutionandbindvars.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/substitutionandbindvars.html
	$(CP) features/suspendedtx.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/suspendedtx.html
	$(CP) features/throttling.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/throttling.html
	for file in `ls images/*.png images/*.PNG`; \
	do \
		$(CP) $$file $(docdir)/images; \
		$(CHMOD) 644 $(docdir)/$$file; \
	done
	$(CP) js/rotate.js $(docdir)/js
	$(CHMOD) 644 $(docdir)/js/rotate.js
	$(CP) modules/passwordencryption.html $(docdir)/modules
	$(CHMOD) 644 $(docdir)/modules/passwordencryption.html
	$(CP) modules/logger.html $(docdir)/modules
	$(CHMOD) 644 $(docdir)/modules/logger.html
	$(CP) modules/sqlr-connection.1871.html $(docdir)/modules
	$(CHMOD) 644 $(docdir)/modules/sqlr-connection.1871.html
	$(CP) modules/sqlr-listener.1869.html $(docdir)/modules
	$(CHMOD) 644 $(docdir)/modules/sqlr-listener.1869.html
	$(CP) modules/sqlr-listener.1886.html $(docdir)/modules
	$(CHMOD) 644 $(docdir)/modules/sqlr-listener.1886.html
	$(CP) modules/sqlr-connection-oracletest-querylog.2899.html $(docdir)/modules
	$(CHMOD) 644 $(docdir)/modules/sqlr-connection-oracletest-querylog.2899.html
	$(CP) programming/binds.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/binds.html
	$(CP) programming/odbc.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/odbc.html
	$(CP) programming/jdbc.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/jdbc.html
	$(CP) programming/c++.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/c++.html
	$(CP) programming/c.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/c.html
	$(CP) programming/perl.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/perl.html
	$(CP) programming/perldbi.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/perldbi.html
	$(CP) programming/php.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/php.html
	$(CP) programming/phppdo.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/phppdo.html
	$(CP) programming/python.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/python.html
	$(CP) programming/pythondb.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/pythondb.html
	$(CP) programming/ruby.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/ruby.html
	$(CP) programming/java.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/java.html
	$(CP) programming/tcl.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/tcl.html

uninstall:
	$(RMTREE) $(docdir)
