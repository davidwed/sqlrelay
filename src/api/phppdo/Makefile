top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(PHPPDOCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(PHPPDOCPPFLAGS) $(COMPILE) $<

all: pdo_sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

pdo_sqlrelay.$(LIBEXT): pdo_sqlrelay.cpp pdo_sqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ pdo_sqlrelay.$(OBJ) $(LDFLAGS) $(PHPPDOLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPEXTDIR)
	$(LTINSTALL) $(CP) pdo_sqlrelay.$(LIBEXT) $(DESTDIR)$(PHPEXTDIR)/php_pdo_sqlrelay.$(LIBEXT)

installlib:
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPEXTDIR)
	$(LTINSTALL) $(CP) pdo_sqlrelay.$(LIBEXT) $(DESTDIR)$(PHPEXTDIR)
	$(RM) $(DESTDIR)$(PHPEXTDIR)/pdo_sqlrelay.$(LIBEXT)
	$(RM) $(DESTDIR)$(PHPEXTDIR)/pdo_sqlrelay.a
	if ( test "$(PHPCONFSTYLE)" = "fedora" -o "$(PHPCONFSTYLE)" = "suse" -o "$(PHPCONFSTYLE)" = "solaris" -o "$(PHPCONFSTYLE)" = "freebsd" -o "$(PHPCONFSTYLE)" = "netbsd" ); \
	then \
		$(MKINSTALLDIRS) $(DESTDIR)$(PHPCONFDIR); \
		$(LTINSTALL) $(CP) pdo_sqlrelay.ini $(DESTDIR)$(PHPCONFDIR); \
		$(CHMOD) 644 $(DESTDIR)$(PHPCONFDIR)/pdo_sqlrelay.ini; \
	fi
	if ( test "$(PHPCONFSTYLE)" = "debian" -o "$(PHPCONFSTYLE)" = "ubuntu" ); \
	then \
		$(MKINSTALLDIRS) $(DESTDIR)$(PHPCONFDIR); \
		$(LTINSTALL) $(CP) pdo_sqlrelay.ini $(DESTDIR)$(PHPCONFDIR); \
		$(CHMOD) 644 $(DESTDIR)$(PHPCONFDIR)/pdo_sqlrelay.ini; \
		if ( test "$(PHPCONFSTYLE)" = "debian" ); \
		then \
			if ( test -r "$(DESTDIR)$(PHPCONFDIR)/../conf.d" -a ! -h "$(DESTDIR)$(PHPCONFDIR)/../conf.d/10-pdo_sqlrelay.ini" -a ! -r "$(DESTDIR)$(PHPCONFDIR)/../conf.d/10-pdo_sqlrelay.ini" ); \
			then \
				cd $(DESTDIR)$(PHPCONFDIR)/../conf.d; \
				ln -s ../mods-available/pdo_sqlrelay.ini 10-pdo_sqlrelay.ini; \
			fi; \
		else \
			for dir in `ls $(DESTDIR)$(PHPCONFDIR)/.. 2> /dev/null`; \
			do \
				if ( test -r "$(DESTDIR)$(PHPCONFDIR)/../$$dir/conf.d" -a ! -h "$(DESTDIR)$(PHPCONFDIR)/../$$dir/conf.d/10-pdo_sqlrelay.ini" -a ! -r "$(DESTDIR)$(PHPCONFDIR)/../$$dir/conf.d/10-pdo_sqlrelay.ini" ); \
				then \
					cd $(DESTDIR)$(PHPCONFDIR)/../$$dir/conf.d; \
					ln -s ../../mods-available/pdo_sqlrelay.ini 10-pdo_sqlrelay.ini; \
				fi; \
			done; \
		fi; \
	fi

uninstall:
	$(RM) $(DESTDIR)$(PHPEXTDIR)/pdo_sqlrelay.$(LIBEXT)
	$(RM) $(DESTDIR)$(PHPCONFDIR)/pdo_sqlrelay.ini
	$(RM) $(DESTDIR)$(PHPCONFDIR)/../conf.d/10-pdo_sqlrelay.ini
	$(RM) $(DESTDIR)$(PHPCONFDIR)/../*/conf.d/10-pdo_sqlrelay.ini
