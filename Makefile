include config.mk
top_srcdir = .

all:
	cd src; $(MAKE) all

clients:
	cd src; $(MAKE) clients

servers:
	cd src; $(MAKE) servers

clean:
	cd src; $(MAKE) clean
	cd bench; $(MAKE) clean
	$(RMTREE) .pics */.pics */*/.pics */*/*/.pics */*/*/*/.pics */*/*/*/*/.pics
	$(RM) packages/RPMS/*.rpm
	$(RM) packages/SRPMS/*.rpm

rebuild:
	cd src; $(MAKE) rebuild

install:
	cd bin; $(MAKE) install
	cd doc; $(MAKE) install
	cd man; $(MAKE) install
	cd etc; $(MAKE) install
	cd src; $(MAKE) install
	cd init; $(MAKE) install

uninstall:
	cd bin; $(MAKE) uninstall
	cd doc; $(MAKE) uninstall
	#cd man; $(MAKE) uninstall
	cd etc; $(MAKE) uninstall
	cd src; $(MAKE) uninstall
	cd init; $(MAKE) uninstall

unconfig: clean
	$(RM) config.cache config.h config.h~ config.h.in~ config.log config.status config.mk
	$(RMTREE) autom4te.cache
	$(RM) src/api/perl/Makefile.PL
	$(RM) src/api/perl/SQLRConnection/SQLRConnection.pm
	$(RM) src/api/perl/SQLRCursor/SQLRCursor.pm
	$(RM) src/api/perl/DBD/SQLRelay.pm
	$(RM) sqlrelay.spec.in.in
	$(RM) sqlrelay.spec.in
	$(RM) sqlrelay.spec
	$(RM) bin/sqlr-stop
	$(RM) bin/sqlrclient-config
	$(RM) bin/sqlrclientwrapper-config
	$(RM) init/openserver/init.d/sqlrelay
	$(RM) init/redhat/init.d/sqlrelay
	$(RM) init/slackware/rc.d/rc.sqlrelay
	$(RM) init/debian/init.d/sqlrelay
	$(RM) libtool

distclean: unconfig

rpm:
	if ( test -n "$(HAVE_PERL)" ); then \
		cd src/api/perl/SQLRConnection; $(MAKE) -f Makefile.master rpminfo; \
	fi
	if ( test -n "$(HAVE_RUBY)" ); then \
		cd src/api/ruby; $(MAKE) -f Makefile.master rpminfo; \
	fi
	./buildspec
	rm rpminfo
	chmod 666 sqlrelay.spec
	cd ..; \
	$(RM) $(RPM_BUILD_DIR)/SOURCES/sqlrelay-$(SQLR_VERSION).tar.gz; \
	tar cf $(RPM_BUILD_DIR)/SOURCES/sqlrelay-$(SQLR_VERSION).tar sqlrelay-$(SQLR_VERSION); \
	gzip $(RPM_BUILD_DIR)/SOURCES/sqlrelay-$(SQLR_VERSION).tar
	rpm -ba sqlrelay.spec
	mv $(RPM_BUILD_DIR)/SRPMS/sqlrelay-$(SQLR_VERSION)*.src.rpm packages/SRPMS
	mv $(RPM_BUILD_DIR)/RPMS/*/sqlrelay*-$(SQLR_VERSION)*.rpm packages/RPMS
	chmod 666 packages/RPMS/*
	$(RMTREE) $(RPM_BUILD_DIR)/BUILD/sqlrelay-$(SQLR_VERSION)
	$(RM) $(RPM_BUILD_DIR)/SOURCES/sqlrelay-$(SQLR_VERSION).tar.gz
