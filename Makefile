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

install-license:
	$(MKINSTALLDIRS) $(licensedir)
	$(CP) COPYING $(licensedir)
	$(CHMOD) 0644 $(licensedir)/COPYING

install-pkgconfig:
	$(MKINSTALLDIRS) $(libdir)/pkgconfig
	$(CP) sqlrelay-c.pc $(libdir)/pkgconfig
	$(CHMOD) 0644 $(libdir)/pkgconfig/sqlrelay-c.pc
	$(CP) sqlrelay-c++.pc $(libdir)/pkgconfig
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

uninstall-license:
	$(RMTREE) $(licensedir)

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
		libtool \
		libtool.gcc \
		sqlrelay-c.pc \
		sqlrelay-c++.pc \
		src/api/cs/SQLRClient/SQLRClient.suo
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
