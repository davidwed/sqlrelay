include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(ODBCINCLUDES)
LIBS = $(ODBCLIBS) $(abs_srcdir)/src/connection/libsqlrconnection.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(ODBCLIBS) $(abs_srcdir)/src/connection/libsqlrconnection_debug.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifeq ($(strip $(ODBCLIBSPATH)),)
	RPATH =
else
ifeq ($(ODBCUSERPATH),yes)
	RPATH = -rpath $(ODBCLIBSPATH)
endif
endif

SRCS= odbcconnection.C main.C
LOBJS= odbcconnection.lo main.lo
DLOBJS= odbcconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@

all: sqlr-connection-odbc sqlr-connection-odbc-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-odbc
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-odbc-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-odbc: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ODBCSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-odbc-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ODBCSTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-odbc $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-odbc-debug $(bindir)
	$(abs_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(abs_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-odbc
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-odbc-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
