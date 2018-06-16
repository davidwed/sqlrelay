top_builddir = .

include config.mk

all:
	cd src $(AND) $(MAKE) all

clean:
	cd src $(AND) $(MAKE) clean
	cd test $(AND) $(MAKE) clean
	$(RMTREE) msvc/setupx86/Debug msvc/setupx86/Release msvc/setupx64/Debug msvc/setupx64/Release

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

install-license:
	$(MKINSTALLDIRS) $(licensedir)
	$(CP) COPYING $(licensedir)
	$(CHMOD) 0644 $(licensedir)/COPYING

install-pkgconfig:
	$(MKINSTALLDIRS) $(libdir)/pkgconfig
	$(CP) sqlrelay-c.pc $(libdir)/pkgconfig/$(SQLRELAY)-c.pc
	$(CHMOD) 0644 $(libdir)/pkgconfig/$(SQLRELAY)-c.pc
	$(CP) sqlrelay-c++.pc $(libdir)/pkgconfig/$(SQLRELAY)-c++.pc
	$(CHMOD) 0644 $(libdir)/pkgconfig/$(SQLRELAY)-c++.pc

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

uninstall-license:
	$(RMTREE) $(licensedir)

uninstall-pkgconfig:
	$(RM) $(libdir)/pkgconfig/$(SQLRELAY)-c.pc \
		$(libdir)/pkgconfig/$(SQLRELAY)-c++.pc \
		$(libdir)/pkgconfig/sqlrelay-c.pc \
		$(libdir)/pkgconfig/sqlrelay-c++.pc

distclean: clean
	$(RM) config.cache \
		config.h \
		config.h~ \
		config.h.in~ \
		config.log \
		config.status \
		config.mk \
		src/common/defines.h \
		src/server/sqlrelay/private/sqlrshm.h \
		bin/sqlrclient-config \
		bin/sqlrclientwrapper-config \
		bin/sqlrserver-config \
		init/rc.sqlrelay \
		init/rc.sqlrcachemanager \
		init/com.firstworks.sqlrelay.plist \
		init/com.firstworks.sqlrcachemanager.plist \
		init/sqlrelay.service \
		init/sqlrelay.service.in.in \
		init/sqlrcachemanager.service \
		init/sqlrcachemanager.service.in.in \
		libtool \
		libtool.gcc \
		sqlrelay-c.pc \
		sqlrelay-c++.pc \
		src/api/cs/SQLRClient/SQLRClient.suo \
		test/testall.sh \
		test/testall.vbs \
		test/test.sh \
		test/test.bat \
		msvc/setupx64/setupx64.vdproj \
		msvc/setupx86/setupx86.vdproj \
		doc/admin/installingpkg.wt \
		configure.lineno
	$(RMTREE) autom4te.cache \
		autom4te-*.cache \
		src/api/cs/SQLRClient/bin \
		src/api/cs/SQLRClient/obj \
		.pics \
		*/.pics \
		*/*/.pics \
		*/*/*/.pics \
		*/*/*/*/.pics \
		*/*/*/*/*/.pics

cppcheck:
	cppcheck -j4 --enable=warning,performance,portability src > /dev/null

tests:
	cd test $(AND) $(MAKE) tests

tests-clean:
	cd test $(AND) $(MAKE) clean
