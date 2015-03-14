top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES)
LIBS = $(PHPLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PHPUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPILE_DL=1 $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /D COMPILE_DL=1 $(COMPILE) $<

all: sql_relay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sql_relay.$(LIBEXT): sql_relay.cpp sql_relay.lo
	$(LTLINK) $(LINK) $(OUT)$@ sql_relay.$(OBJ) $(LDFLAGS) $(LIBS) -rpath $(PHPEXTDIR) $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)
	if ( test "$(PHPCONFSTYLE)" = "fedora" -o "$(PHPCONFSTYLE)" = "suse" -o "$(PHPCONFSTYLE)" = "solaris" ); \
	then \
		$(MKINSTALLDIRS) $(DESTDIR)$(PHPCONFDIR); \
		$(LTINSTALL) $(CP) sql_relay.ini $(DESTDIR)$(PHPCONFDIR); \
		$(CHMOD) 644 $(DESTDIR)$(PHPCONFDIR)/sql_relay.ini; \
	fi
	if ( test "$(PHPCONFSTYLE)" = "debian" -o "$(PHPCONFSTYLE)" = "ubuntu" ); \
	then \
		$(MKINSTALLDIRS) $(DESTDIR)$(PHPCONFDIR); \
		$(LTINSTALL) $(CP) sql_relay.ini $(DESTDIR)$(PHPCONFDIR); \
		$(CHMOD) 644 $(DESTDIR)$(PHPCONFDIR)/sql_relay.ini; \
		if ( test "$(PHPCONFSTYLE)" = "debian" ); \
		then \
			if ( test -r "$(DESTDIR)$(PHPCONFDIR)/../conf.d" -a ! -h "$(DESTDIR)$(PHPCONFDIR)/../conf.d/10-sql_relay.ini" -a ! -r "$(DESTDIR)$(PHPCONFDIR)/../conf.d/10-sql_relay.ini" ); \
			then \
				cd $(DESTDIR)$(PHPCONFDIR)/../conf.d; \
				ln -s ../mods-available/sql_relay.ini 10-sql_relay.ini; \
			fi; \
		else \
			for dir in `ls $(DESTDIR)$(PHPCONFDIR)/.. 2> /dev/null`; \
			do \
				if ( test -r "$(DESTDIR)$(PHPCONFDIR)/../$$dir/conf.d" -a ! -h "$(DESTDIR)$(PHPCONFDIR)/../$$dir/conf.d/10-sql_relay.ini" -a ! -r "$(DESTDIR)$(PHPCONFDIR)/../$$dir/conf.d/10-sql_relay.ini" ); \
				then \
					cd $(DESTDIR)$(PHPCONFDIR)/../$$dir/conf.d; \
					ln -s ../../mods-available/sql_relay.ini 10-sql_relay.ini; \
				fi; \
			done; \
		fi; \
	fi

installdll:
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPEXTDIR)
	$(LTINSTALL) $(CP) sql_relay.$(LIBEXT) $(DESTDIR)$(PHPEXTDIR)

installlib:
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPEXTDIR)
	$(LTINSTALL) $(CP) sql_relay.$(LIBEXT) $(DESTDIR)$(PHPEXTDIR)
	$(RM) $(DESTDIR)$(PHPEXTDIR)/sql_relay.$(LIBEXT)
	$(RM) $(DESTDIR)$(PHPEXTDIR)/sql_relay.a

uninstall:
	$(RM) $(DESTDIR)$(PHPEXTDIR)/sql_relay.so
	$(RM) $(DESTDIR)$(PHPCONFDIR)/sql_relay.ini
	$(RM) $(DESTDIR)$(PHPCONFDIR)/../conf.d/10-sql_relay.ini
	$(RM) $(DESTDIR)$(PHPCONFDIR)/../*/conf.d/10-sql_relay.ini
