top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES)
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

all: pdo_sqlrelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o *.so phpincludes.h errors1 errors2 errors3
	$(RMTREE) .libs

pdo_sqlrelay.la: pdo_sqlrelay.cpp pdo_sqlrelay.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ pdo_sqlrelay.lo $(LDFLAGS) $(LIBS) -rpath $(PHPEXTDIR) $(RPATH) -avoid-version -module

install: pdo_sqlrelay.la
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPEXTDIR)
	$(LTINSTALL) pdo_sqlrelay.la $(DESTDIR)$(PHPEXTDIR)
	$(RM) $(DESTDIR)$(PHPEXTDIR)/pdo_sqlrelay.la
	$(RM) $(DESTDIR)$(PHPEXTDIR)/pdo_sqlrelay.a
	if ( test "$(PHPCONFSTYLE)" = "fedora" -o "$(PHPCONFSTYLE)" = "suse" -o "$(PHPCONFSTYLE)" = "solaris" ); \
	then \
		$(MKINSTALLDIRS) $(DESTDIR)$(PHPCONFDIR); \
		$(LTINSTALL) -m 644 pdo_sqlrelay.ini $(DESTDIR)$(PHPCONFDIR); \
	fi
	if ( test "$(PHPCONFSTYLE)" = "debian" -o "$(PHPCONFSTYLE)" = "ubuntu" ); \
	then \
		$(MKINSTALLDIRS) $(DESTDIR)$(PHPCONFDIR); \
		$(LTINSTALL) -m 644 pdo_sqlrelay.ini $(DESTDIR)$(PHPCONFDIR); \
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
	$(RM) $(DESTDIR)$(PHPEXTDIR)/pdo_sqlrelay.so
	$(RM) $(DESTDIR)$(PHPCONFDIR)/pdo_sqlrelay.ini
	$(RM) $(DESTDIR)$(PHPCONFDIR)/../conf.d/10-pdo_sqlrelay.ini
	$(RM) $(DESTDIR)$(PHPCONFDIR)/../*/conf.d/10-pdo_sqlrelay.ini
