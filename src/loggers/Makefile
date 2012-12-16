top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(LOGDIRCPPFLAG) -c $< -o $@

all: sqlrlogger_slowqueries.la \
	sqlrlogger_custom_nw.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrlogger_slowqueries.la: slowqueries.cpp slowqueries.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ slowqueries.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrlogger_custom_nw.la: custom_nw.cpp custom_nw.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ custom_nw.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

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
