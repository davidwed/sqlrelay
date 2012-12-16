top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/connection -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(SIDINCLUDES) $(DEBUGDIRCPPFLAG) $(LOGDIRCPPFLAG) -DSERVER_VERSION=\"$(SQLR_VERSION)\" -DLIBDIR=\"$(libdir)\"
LIBS = -L$(top_builddir)/src/util -lsqlrutil $(SIDLIBS) $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlrlogger_slowqueries.la \
	sqlrlogger_custom_nw.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrlogger_slowqueries.la: slowqueries.cpp slowqueries.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ slowqueries.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrlogger_custom_nw.la: custom_nw.cpp custom_nw.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ custom_nw.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrlogger_slowqueries.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrlogger_slowqueries.a
	$(RM) $(libexecdir)/sqlrlogger_slowqueries.la
	$(LTINSTALL) sqlrlogger_custom_nw.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrlogger_custom_nw.a
	$(RM) $(libexecdir)/sqlrlogger_custom_nw.la
	$(LIBTOOL) --mode=finish $(libexecdir)

uninstall:
	$(RM) $(libexecdir)/sqlrlogger_*
