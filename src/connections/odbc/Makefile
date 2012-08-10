top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(ODBCINCLUDES) $(ICONVINCLUDES) $(SIDINCLUDES)
LIBS = $(ODBCLIBS) $(ICONVLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(SIDLIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(ODBCLIBSPATH)),)
ifeq ($(ODBCUSERPATH),yes)
	RPATH = -R $(ODBCLIBSPATH)
endif
endif
ifneq ($(strip $(ICONVLIBSPATH)),)
ifeq ($(ICONVUSERPATH),yes)
	RPATH += -R $(ICONVLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= odbcconnection.cpp main.cpp
LOBJS= odbcconnection.lo main.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-odbc$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-odbc$(EXE)
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

sqlr-connection-odbc$(EXE): $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ODBCSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-connection-odbc$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-odbc$(EXE)
