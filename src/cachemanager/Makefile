top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) -DCACHE_DIR=\"$(cachedir)\" -I./ -I$(top_builddir) -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES)
LIBS = -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-cachemanager$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-cachemanager$(EXE)
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o
	$(RMTREE) .libs

sqlr-cachemanager$(EXE): sqlr-cachemanager.cpp sqlr-cachemanager.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-cachemanager.lo $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-cachemanager$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-cachemanager$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-cachemanager-debug$(EXE)
