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
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPILE_DL=1 -c $< -o $@

all: sql_relay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o *.so
	$(RMTREE) .libs

sql_relay.la: sql_relay.cpp sql_relay.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sql_relay.lo $(LDFLAGS) $(LIBS) -rpath $(PHPEXTDIR) $(RPATH) -avoid-version -module

install: sql_relay.la
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPEXTDIR)
	$(LTINSTALL) sql_relay.la $(DESTDIR)$(PHPEXTDIR)
	$(RM) $(DESTDIR)$(PHPEXTDIR)/sql_relay.la
	$(RM) $(DESTDIR)$(PHPEXTDIR)/sql_relay.a
	if ( test "$(PHPCONFSTYLE)" = "fedora" -o "$(PHPCONFSTYLE)" = "suse" -o "$(PHPCONFSTYLE)" = "solaris" ); \
	then \
		$(MKINSTALLDIRS) $(DESTDIR)$(PHPCONFDIR); \
		$(LTINSTALL) -m 644 sql_relay.ini $(DESTDIR)$(PHPCONFDIR); \
	fi
	if ( test "$(PHPCONFSTYLE)" = "debian" -o "$(PHPCONFSTYLE)" = "ubuntu" ); \
	then \
		$(MKINSTALLDIRS) $(DESTDIR)$(PHPCONFDIR); \
		$(LTINSTALL) -m 644 sql_relay.ini $(DESTDIR)$(PHPCONFDIR); \
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

uninstall:
	$(RM) $(DESTDIR)$(PHPEXTDIR)/sql_relay.so
	$(RM) $(DESTDIR)$(PHPCONFDIR)/sql_relay.ini
	$(RM) $(DESTDIR)$(PHPCONFDIR)/../conf.d/10-sql_relay.ini
	$(RM) $(DESTDIR)$(PHPCONFDIR)/../*/conf.d/10-sql_relay.ini
