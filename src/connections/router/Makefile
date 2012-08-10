top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(SIDINCLUDES)
LIBS = -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(SIDLIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(ROUTERLIBSPATH)),)
ifeq ($(ROUTERUSERPATH),yes)
	RPATH = -R $(ROUTERLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= routerconnection.cpp main.cpp
LOBJS= routerconnection.lo main.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-router$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-router$(EXE)
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

sqlr-connection-router$(EXE): $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ROUTERSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-connection-router$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-router$(EXE)
