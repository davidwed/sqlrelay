top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(SSLINCLUDES) $(ORACLEINCLUDES)
LIBS = $(ORACLELIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(PTHREADLIBS) $(SSLLIB) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(ORACLELIBS) -L$(top_builddir)/src/connection -lsqlrconnection_debug -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(PTHREADLIBS) $(SSLLIB) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(ORACLELIBSPATH)),)
ifeq ($(ORACLEUSERPATH),yes)
	RPATH = -R $(ORACLELIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= oracle7connection.C main.C
LOBJS= oracle7connection.lo main.lo
DLOBJS= oracle7connection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

all: sqlr-connection-oracle7 sqlr-connection-oracle7-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-oracle7
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-oracle7-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-oracle7: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ORACLESTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-oracle7-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ORACLESTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(INSTALL) $(INSTALLSTRIP) sqlr-connection-oracle7 $(bindir)
	$(INSTALL) $(INSTALLSTRIP) sqlr-connection-oracle7-debug $(bindir)
	$(MKINSTALLDIRS) $(tmpdir)
	chmod 777 $(tmpdir)
	$(MKINSTALLDIRS) $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-oracle7
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-oracle7-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
