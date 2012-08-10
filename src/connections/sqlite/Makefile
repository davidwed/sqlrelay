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
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= sqliteconnection.cpp main.cpp
LOBJS= sqliteconnection.lo main.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-sqlite$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-sqlite$(EXE)
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

sqlr-connection-sqlite$(EXE): $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(SQLITESTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-connection-sqlite$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-sqlite$(EXE)
