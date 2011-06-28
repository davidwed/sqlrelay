top_builddir = .

include config.mk

.PHONY: all clean install uninstall distclean

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean
	$(MAKE) -C bench clean
	$(RMTREE) .pics
	$(RMTREE) */.pics
	$(RMTREE) */*/.pics
	$(RMTREE) */*/*/.pics
	$(RMTREE) */*/*/*/.pics
	$(RMTREE) */*/*/*/*/.pics
	$(MAKE) -C test clean

install:
	$(MAKE) -C src install
	$(MAKE) -C bin install
	$(MAKE) -C etc install
	$(MAKE) -C init install
	$(MAKE) -C man install
	$(MAKE) -C doc install
	$(MKINSTALLDIRS) $(libdir)/pkgconfig
	$(INSTALL) -m 0644 sqlrelay-c.pc $(libdir)/pkgconfig/sqlrelay-c.pc
	$(INSTALL) -m 0644 sqlrelay-c++.pc $(libdir)/pkgconfig/sqlrelay-c++.pc

uninstall:
	$(MAKE) -C src uninstall
	$(MAKE) -C bin uninstall
	$(MAKE) -C etc uninstall
	$(MAKE) -C init uninstall
	$(MAKE) -C man uninstall
	$(MAKE) -C doc uninstall
	$(RM) $(libdir)/pkgconfig/sqlrelay-c.pc
	$(RM) $(libdir)/pkgconfig/sqlrelay-c++.pc

distclean: clean
	$(RM) config.cache
	$(RM) config.h
	$(RM) config.h~
	$(RM) config.h.in~
	$(RM) config.log
	$(RM) config.status
	$(RM) config.mk
	$(RMTREE) autom4te.cache
	$(RMTREE) autom4te-*.cache
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
	$(RM) init/netbsd/rc.d/sqlrelay
	$(RM) init/freebsd/rc.d/sqlrelay
	$(RM) test/test
	$(RM) libtool
	$(RM) libtool.gcc
	$(RM) sqlrelay-c.pc
	$(RM) sqlrelay-c++.pc
