top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(SSLINCLUDES) $(MSQLINCLUDES)
LIBS = $(MSQLLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(PTHREADLIBS) $(SSLLIB) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(MSQLLIBS) -L$(top_builddir)/src/connection -lsqlrconnection_debug -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(PTHREADLIBS) $(SSLLIB) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(MSQLLIBSPATH)),)
ifeq ($(MSQLUSERPATH),yes)
	RPATH = -R $(MSQLLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= msqlconnection.C main.C
LOBJS= msqlconnection.lo main.lo
DLOBJS= msqlconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

all: sqlr-connection-msql sqlr-connection-msql-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-msql
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-msql-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-msql: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MSQLSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-msql-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MSQLSTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(INSTALL) $(INSTALLSTRIP) sqlr-connection-msql $(bindir)
	$(INSTALL) $(INSTALLSTRIP) sqlr-connection-msql-debug $(bindir)
	$(MKINSTALLDIRS) $(tmpdir)
	chmod 777 $(tmpdir)
	$(MKINSTALLDIRS) $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-msql
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-msql-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
