top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server -lsqlrserver $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/src/server -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrauth_default.la sqlrauth_sqlrelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrauth_default.la: default.cpp default.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ default.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrauth_sqlrelay.la: sqlrelay.cpp sqlrelay.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrelay.lo $(LDFLAGS) -L$(topdir)/src/api/c++/src -lsqlrclient $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrauth_default.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_default.a
	$(RM) $(libexecdir)/sqlrauth_default.la
	$(MODULERENAME) $(libexecdir)/sqlrauth_default.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrauth_sqlrelay.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.a
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.la
	$(MODULERENAME) $(libexecdir)/sqlrauth_sqlrelay.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrauth_*
