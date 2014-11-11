top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrparser_base.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrparser_base.la: base.cpp base.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ base.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(LIBTOOL) --mode=finish $(libdir)
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrparser_base.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrparser_base.a
	$(RM) $(libexecdir)/sqlrparser_base.la
	$(MODULERENAME) $(libexecdir)/sqlrparser_base.so so $(MODULESUFFIX)
	$(MKINSTALLDIRS) $(libexecdir)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrparser_base.*
