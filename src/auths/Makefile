top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server -lsqlrserver $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/src/server $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrauth_default.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrauth_default.la: default.cpp default.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ default.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrauth_default.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_default.a
	$(RM) $(libexecdir)/sqlrauth_default.la
	$(MODULERENAME) $(libexecdir)/sqlrauth_default.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrauth_*
