top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(POSTGRESQLINCLUDES)
LIBS = $(POSTGRESQLLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(POSTGRESQLLIBS) -L$(top_builddir)/src/connection -lsqlrconnection_debug -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(POSTGRESQLLIBSPATH)),)
ifeq ($(POSTGRESQLUSERPATH),yes)
	RPATH = -R $(POSTGRESQLLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= postgresqlconnection.C main.C
LOBJS= postgresqlconnection.lo main.lo
DLOBJS= postgresqlconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@

all: sqlr-connection-postgresql sqlr-connection-postgresql-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-postgresql
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-postgresql-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-postgresql: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(POSTGRESQLSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-postgresql-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(POSTGRESQLSTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(INSTALL) sqlr-connection-postgresql$(EXE) $(bindir)
	$(INSTALL) sqlr-connection-postgresql-debug$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-postgresql$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-postgresql-debug$(EXE)
