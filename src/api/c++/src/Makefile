top_builddir = ../../../..

include ../../../../config.mk

.PHONY: all clean install uninstall

ifeq ($(MINGW32),yes)
DLLEXPORTS = -DLIBSQLRCLIENT_EXPORTS
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(DLLEXPORTS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/api/c++/include -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)


SRCS = sqlrconnection.cpp sqlrcursor.cpp row.cpp
LOBJS = sqlrconnection.lo sqlrcursor.lo row.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrclient.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

libsqlrclient.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined $(LIBS)

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrclient.la $(libdir)/libsqlrclient.la
	$(LIBTOOL) --mode=finish $(libdir)
	$(MKINSTALLDIRS) $(cachedir)
	chmod 777 $(cachedir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclient.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclient_p.la
	$(RMTREE) $(cachedir)
	$(RMTREE) $(prefix)/lib/sqlrelay/cache
