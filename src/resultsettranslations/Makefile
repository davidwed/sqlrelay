top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrresultsettranslation_reformatdatetime.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrresultsettranslation_reformatdatetime.la: reformatdatetime.cpp reformatdatetime.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ reformatdatetime.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrresultsettranslation_reformatdatetime.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrresultsettranslation_reformatdatetime.a
	$(RM) $(libexecdir)/sqlrresultsettranslation_reformatdatetime.la
	$(MODULERENAME) $(libexecdir)/sqlrresultsettranslation_reformatdatetime.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrresultsettranslation_*
