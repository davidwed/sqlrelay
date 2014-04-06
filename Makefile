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
		bin/sqlr-stop \
		bin/sqlrclient-config \
		bin/sqlrclientwrapper-config \
		bin/sqlrserver-config \
		init/rc.sqlrelay \
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
		src/api/c++/msvc/DebugCLR \
		src/api/c++/msvc/ReleaseCLR \
		src/api/c++/msvc/x64 \
		src/api/c/msvc/Debug \
		src/api/c/msvc/Release \
		src/api/c/msvc/DebugCLR \
		src/api/c/msvc/ReleaseCLR \
		src/api/c/msvc/x64 \
		src/api/cs/SQLRClient/bin \
		src/api/cs/SQLRClient/obj \
		src/api/odbc/msvc/Debug \
		src/api/odbc/msvc/Release \
		src/api/odbc/msvc/DebugCLR \
		src/api/odbc/msvc/ReleaseCLR \
		src/api/odbc/msvc/x64 \
		src/api/perl/msvc/Debug \
		src/api/perl/msvc/Release \
		src/api/perl/msvc/DebugCLR \
		src/api/perl/msvc/ReleaseCLR \
		src/api/perl/msvc/x64 \
		src/api/python/msvc/Debug \
		src/api/python/msvc/Release \
		src/api/python/msvc/DebugCLR \
		src/api/python/msvc/ReleaseCLR \
		src/api/python/msvc/x64 \
		src/cmdline/msvc/Debug \
		src/cmdline/msvc/Release \
		src/cmdline/msvc/DebugCLR \
		src/cmdline/msvc/ReleaseCLR \
		src/cmdline/msvc/x64 \
		src/util/msvc/Debug \
		src/util/msvc/Release \
		src/util/msvc/DebugCLR \
		src/util/msvc/ReleaseCLR \
		src/util/msvc/x64
