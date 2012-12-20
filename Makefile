top_builddir = .

include config.mk

.PHONY: all clean install uninstall distclean

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean
	$(MAKE) -C bench clean
	$(RMTREE) .pics */.pics */*/.pics */*/*/.pics */*/*/*/.pics */*/*/*/*/.pics
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
	$(RMTREE) $(libexecdir)

distclean: clean
	$(RM) config.cache \
		config.h \
		config.h~ \
		config.h.in~ \
		config.log \
		config.status \
		config.mk \
		src/api/perl/Connection/Connection.pm \
		src/api/perl/Cursor/Cursor.pm \
		src/api/perl/DBD/Makefile.PL \
		src/api/perl/DBD/SQLRelay.pm \
		bin/sqlr-stop \
		bin/sqlrclient-config \
		bin/sqlrclientwrapper-config \
		bin/sqlrserver-config \
		init/openserver/init.d/sqlrelay \
		init/redhat/init.d/sqlrelay \
		init/slackware/rc.d/rc.sqlrelay \
		init/debian/init.d/sqlrelay \
		init/netbsd/rc.d/sqlrelay \
		init/freebsd/rc.d/sqlrelay \
		test/test \
		libtool \
		libtool.gcc \
		sqlrelay-c.pc \
		sqlrelay-c++.pc \
		src/api/c++/msvc/libsqlrclient.opensdf \
		src/api/c++/msvc/libsqlrclient.sdf \
		src/api/c++/msvc/libsqlrclient.suo \
		src/api/c/msvc/libsqlrclientwrapper.opensdf \
		src/api/c/msvc/libsqlrclientwrapper.sdf \
		src/api/c/msvc/libsqlrclientwrapper.suo \
		src/cmdline/msvc/sqlrsh.opensdf \
		src/cmdline/msvc/sqlrsh.sdf \
		src/cmdline/msvc/sqlrsh.suo \
		src/util/msvc/libsqlrutil.opensdf \
		src/util/msvc/libsqlrutil.sdf \
		src/util/msvc/libsqlrutil.suo \
		src/api/cs/SQLRClient/SQLRClient.suo
	$(RMTREE) autom4te.cache \
		autom4te-*.cache \
		src/api/c++/msvc/Debug \
		src/api/c++/msvc/Release \
		src/api/c++/msvc/Debug \
		src/api/c++/msvc/Release \
		src/cmdline/msvc/Debug \
		src/cmdline/msvc/Release \
		src/util/msvc/Debug \
		src/util/msvc/Release \
		src/api/cs/SQLRClient/bin \
		src/api/cs/SQLRClient/obj
