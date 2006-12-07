top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(FIREBIRDINCLUDES) $(SIDINCLUDES)
LIBS = $(FIREBIRDLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SIDLIBS) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(FIREBIRDLIBS) -L$(top_builddir)/src/connection -lsqlrconnection_debug -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SIDLIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(FIREBIRDLIBSPATH)),)
ifeq ($(FIREBIRDUSERPATH),yes)
	RPATH = -R $(FIREBIRDLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= firebirdconnection.C main.C
LOBJS= firebirdconnection.lo main.lo
DLOBJS= firebirdconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-firebird sqlr-connection-firebird-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-firebird$(EXE)
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-firebird-debug$(EXE)
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

rebuild: clean all

sqlr-connection-firebird: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(FIREBIRDSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-firebird-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(FIREBIRDSTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-connection-firebird$(EXE) $(bindir)
	$(LTINSTALL) sqlr-connection-firebird-debug$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-firebird$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-firebird-debug$(EXE)
