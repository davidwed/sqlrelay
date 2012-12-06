top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(SQLITEINCLUDES) $(SIDINCLUDES)
LIBS = $(SQLITELIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(SIDLIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
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

all: libsqlrelay_sqlrconnection_sqlite.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

libsqlrelay_sqlrconnection_sqlite.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrelay_sqlrconnection_sqlite.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_sqlite.a

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqlrconnection_sqlite.la
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-sqlite$(EXE)
