include config.mk
top_srcdir = .

all:
	$(MAKE) -C src all

clients:
	$(MAKE) -C src clients

servers:
	$(MAKE) -C src servers

clean:
	$(MAKE) -C src clean
	$(MAKE) -C bench clean
	$(RMTREE) .pics
	$(RMTREE) */.pics
	$(RMTREE) */*/.pics
	$(RMTREE) */*/*/.pics
	$(RMTREE) */*/*/*/.pics
	$(RMTREE) */*/*/*/*/.pics
	$(RM) packages/RPMS/*.rpm
	$(RM) packages/SRPMS/*.rpm

rebuild:
	$(MAKE) -C src rebuild

install:
	$(MAKE) -C bin install
	$(MAKE) -C doc install
	$(MAKE) -C man install
	$(MAKE) -C etc install
	$(MAKE) -C src install
	$(MAKE) -C init install

uninstall:
	$(MAKE) -C bin uninstall
	$(MAKE) -C doc uninstall
	$(MAKE) -C man uninstall
	$(MAKE) -C etc uninstall
	$(MAKE) -C src uninstall
	$(MAKE) -C init uninstall

unconfig: clean
	$(RM) config.cache
	$(RM) config.h
	$(RM) config.h~
	$(RM) config.h.in~
	$(RM) config.log
	$(RM) config.status
	$(RM) config.mk
	$(RMTREE) autom4te.cache
	$(RM) src/api/perl/SQLRConnection/Makefile.PL
	$(RM) src/api/perl/SQLRConnection/SQLRConnection.pm
	$(RM) src/api/perl/SQLRCursor/Makefile.PL
	$(RM) src/api/perl/SQLRCursor/SQLRCursor.pm
	$(RM) src/api/perl/DBD/Makefile.PL
	$(RM) src/api/perl/DBD/SQLRelay.pm
	$(RM) bin/sqlr-stop
	$(RM) bin/sqlrclient-config
	$(RM) bin/sqlrclientwrapper-config
	$(RM) init/openserver/init.d/sqlrelay
	$(RM) init/redhat/init.d/sqlrelay
	$(RM) init/slackware/rc.d/rc.sqlrelay
	$(RM) init/debian/init.d/sqlrelay
	$(RM) libtool

distclean: unconfig
