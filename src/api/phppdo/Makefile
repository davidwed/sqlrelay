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

uninstall:
	$(RM) $(DESTDIR)$(PHPEXTDIR)/pdo_sqlrelay.so
