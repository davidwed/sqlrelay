top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/authenticator -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(MDBTOOLSINCLUDES)
LIBS = $(MDBTOOLSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(MDBTOOLSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection_debug -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(MDBTOOLSLIBSPATH)),)
ifeq ($(MDBTOOLSUSERPATH),yes)
	RPATH = -R $(MDBTOOLSLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= mdbtoolsconnection.C main.C
LOBJS= mdbtoolsconnection.lo main.lo
DLOBJS= mdbtoolsconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@

all: sqlr-connection-mdbtools sqlr-connection-mdbtools-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-mdbtools
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-mdbtools-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-mdbtools: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MDBTOOLSSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-mdbtools-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MDBTOOLSSTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(INSTALL) sqlr-connection-mdbtools$(EXE) $(bindir)
	$(INSTALL) sqlr-connection-mdbtools-debug$(EXE) $(bindir)
	$(MKINSTALLDIRS) $(tmpdir)
	chmod 777 $(tmpdir)
	$(MKINSTALLDIRS) $(tmpdir)/sockets
	chmod 777 $(tmpdir)/sockets
	$(MKINSTALLDIRS) $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-mdbtools$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-mdbtools-debug$(EXE)
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
