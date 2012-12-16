top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES)
LIBS = -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)
ifneq ($(strip $(ROUTERLIBSPATH)),)
ifeq ($(ROUTERUSERPATH),yes)
	RPATH = -R $(ROUTERLIBSPATH)
endif
endif

SRCS= routerconnection.cpp
LOBJS= routerconnection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlrconnection_router.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrconnection_router.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrconnection_router.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_router.a
	$(RM) $(libexecdir)/sqlrconnection_router.la

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_router.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_router*
	$(RM) $(bindir)/sqlr-connection-router$(EXE)
