top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

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

uninstall:
	$(RM) $(DESTDIR)$(PHPEXTDIR)/sql_relay.so
