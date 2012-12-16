top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(SQLITEINCLUDES)
LIBS = $(SQLITELIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)
ifneq ($(strip $(SQLITELIBSPATH)),)
ifeq ($(SQLITEUSERPATH),yes)
	RPATH = -R $(SQLITELIBSPATH)
endif
endif

SRCS= sqliteconnection.cpp
LOBJS= sqliteconnection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlrconnection_sqlite.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrconnection_sqlite.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrconnection_sqlite.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_sqlite.a
	$(RM) $(libexecdir)/sqlrconnection_sqlite.la
	$(MODULERENAME) $(libexecdir)/sqlrconnection_sqlite.$(SOSUFFIX) $(SOSUFFIX) $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_sqlite.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_sqlite*
	$(RM) $(bindir)/sqlr-connection-sqlite$(EXE)
