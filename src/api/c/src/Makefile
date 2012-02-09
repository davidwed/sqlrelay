top_builddir = ../../../..

include ../../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/api/c/include -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(libdir)),)
ifeq ($(CUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = sqlrclientwrapper.cpp
LOBJS = sqlrclientwrapper.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrclientwrapper.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

libsqlrclientwrapper.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined $(LIBS)

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrclientwrapper.la $(libdir)/libsqlrclientwrapper.la
	$(MKINSTALLDIRS) $(cachedir)
	chmod 777 $(cachedir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper_p.la
	$(RMTREE) $(cachedir)
	$(RMTREE) $(prefix)/lib/sqlrelay/cache
