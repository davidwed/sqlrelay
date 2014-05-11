top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrquery_sqlrcmdgstat.la \
	sqlrquery_sqlrcmdcstat.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrquery_sqlrcmdgstat.la: sqlrcmdgstat.cpp sqlrcmdgstat.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrcmdgstat.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module

sqlrquery_sqlrcmdcstat.la: sqlrcmdcstat.cpp sqlrcmdcstat.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrcmdcstat.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrquery_sqlrcmdgstat.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.a
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.la
	$(MODULERENAME) $(libexecdir)/sqlrquery_sqlrcmdgstat.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrquery_sqlrcmdcstat.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.a
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.la
	$(MODULERENAME) $(libexecdir)/sqlrquery_sqlrcmdcstat.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrquery_*
