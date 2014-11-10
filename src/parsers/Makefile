top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: libsqlreparser.la sqlrparser_base.la sqlrparser_enterprise.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

libsqlreparser.la: sqlreparser.cpp sqlreparser.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlreparser.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

sqlrparser_base.la: base.cpp base.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ base.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrparser_enterprise.la: enterprise.cpp enterprise.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ enterprise.lo $(LDFLAGS) -L./ -lsqlreparser $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlreparser.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrparser_base.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrparser_base.a
	$(RM) $(libexecdir)/sqlrparser_base.la
	$(MODULERENAME) $(libexecdir)/sqlrparser_base.so so $(MODULESUFFIX)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrparser_enterprise.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrparser_enterprise.a
	$(RM) $(libexecdir)/sqlrparser_enterprise.la
	$(MODULERENAME) $(libexecdir)/sqlrparser_enterprise.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libdir)/libsqlreparser*
	$(RM) $(libexecdir)/sqlrparser_*
