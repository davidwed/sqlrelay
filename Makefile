top_builddir = .

include config.mk

all:
	cd src $(AND) $(MAKE) all

clean:
	cd src $(AND) $(MAKE) clean
	cd test $(AND) $(MAKE) clean

install: $(INSTALLSUBDIRS)

install-src:
	cd src $(AND) $(MAKE) install

install-bin:
	cd bin $(AND) $(MAKE) install

install-etc:
	cd etc $(AND) $(MAKE) install

install-init:
	cd init $(AND) $(MAKE) install

install-man:
	cd man $(AND) $(MAKE) install

install-doc:
	cd doc $(AND) $(MAKE) install

install-pkgconfig:
	$(MKINSTALLDIRS) $(libdir)/pkgconfig
	$(CP) sqlrelay-c.pc $(libdir)/pkgconfig/sqlrelay-c.pc
	$(CHMOD) 0644 $(libdir)/pkgconfig/sqlrelay-c.pc
	$(CP) sqlrelay-c++.pc $(libdir)/pkgconfig/sqlrelay-c++.pc
	$(CHMOD) 0644 $(libdir)/pkgconfig/sqlrelay-c++.pc

uninstall: $(UNINSTALLSUBDIRS)
	$(RMTREE) $(libexecdir)

uninstall-src:
	cd src $(AND) $(MAKE) uninstall

uninstall-bin:
	cd bin $(AND) $(MAKE) uninstall

uninstall-etc:
	cd etc $(AND) $(MAKE) uninstall

uninstall-init:
	cd init $(AND) $(MAKE) uninstall

uninstall-man:
	cd man $(AND) $(MAKE) uninstall

uninstall-doc:
	cd doc $(AND) $(MAKE) uninstall

uninstall-pkgconfig:
	$(RM) $(libdir)/pkgconfig/sqlrelay-c.pc \
		$(libdir)/pkgconfig/sqlrelay-c++.pc

distclean: clean
	$(RM) config.cache \
		config.h \
		config.h~ \
		config.h.in~ \
		config.log \
		config.status \
		config.mk \
		bin/sqlrclient-config \
		bin/sqlrclientwrapper-config \
		bin/sqlrserver-config \
		init/rc.sqlrelay \
		init/rc.sqlrcachemanager \
		init/com.firstworks.sqlrelay.plist \
		init/com.firstworks.sqlrcachemanager.plist \
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
		src/api/java/msvc/Debug \
		src/api/java/msvc/Release \
		src/api/java/msvc/DebugCLR \
		src/api/java/msvc/ReleaseCLR \
		src/api/java/msvc/x64 \
		src/cmdline/msvc/Debug \
		src/cmdline/msvc/Release \
		src/cmdline/msvc/DebugCLR \
		src/cmdline/msvc/ReleaseCLR \
		src/cmdline/msvc/x64 \
		src/util/msvc/Debug \
		src/util/msvc/Release \
		src/util/msvc/DebugCLR \
		src/util/msvc/ReleaseCLR \
		src/util/msvc/x64 \
		.pics \
		*/.pics \
		*/*/.pics \
		*/*/*/.pics \
		*/*/*/*/.pics \
		*/*/*/*/*/.pics

cppcheck:
	cppcheck -j4 --enable=warning,performance,portability src > /dev/null
