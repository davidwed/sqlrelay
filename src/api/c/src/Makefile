top_builddir = ../../../..

include ../../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/api/c/include -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
ifneq ($(strip $(libdir)),)
ifeq ($(CUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = sqlrclientwrapper.C
LOBJS = sqlrclientwrapper.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

all: libsqlrclientwrapper.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

libsqlrclientwrapper.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(MKINSTALLDIRS) $(libdir)
	$(INSTALL) $(INSTALLSTRIP) libsqlrclientwrapper.la $(libdir)/libsqlrclientwrapper.la
	$(MKINSTALLDIRS) $(cachedir)
	chmod 777 $(cachedir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper_p.la
	$(RMTREE) $(cachedir)
	$(RMTREE) $(prefix)/lib/sqlrelay/cache
