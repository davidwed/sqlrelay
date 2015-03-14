top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrprotocol_sqlrclient.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrprotocol_sqlrclient.la: sqlrclient.cpp sqlrclient.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrclient.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrprotocol_sqlrclient.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrprotocol_sqlrclient.a
	$(RM) $(libexecdir)/sqlrprotocol_sqlrclient.la
	$(MODULERENAME) $(libexecdir)/sqlrprotocol_sqlrclient.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrprotocol_sqlrclient.*
