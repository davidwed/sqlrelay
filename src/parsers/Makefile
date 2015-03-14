top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrparser_default.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrparser_default.la: default.cpp default.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ default.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(LIBTOOL) --mode=finish $(libdir)
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrparser_default.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrparser_default.a
	$(RM) $(libexecdir)/sqlrparser_default.la
	$(MODULERENAME) $(libexecdir)/sqlrparser_default.so so $(MODULESUFFIX)
	$(MKINSTALLDIRS) $(libexecdir)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrparser_default.*
