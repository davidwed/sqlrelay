include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(ODBCINCLUDES)
LIBS = $(ODBCLIBS) $(abs_srcdir)/src/connection/libsqlrconnection.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(ODBCLIBS) $(abs_srcdir)/src/connection/libsqlrconnection_p.la $(abs_srcdir)/src/util/libsqlrutil_p.la $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifeq ($(strip $(ODBCLIBSPATH)),)
	RPATH =
else
	RPATH = -rpath $(ODBCLIBSPATH)
endif

SRCS= odbcconnection.C main.C
NLOBJS= odbcconnection.n.lo main.n.lo
PLOBJS= odbcconnection.p.lo main.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-odbc sqlr-connection-odbc-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-odbc
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-odbc-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-odbc: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ODBCSTATIC) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS) $(RPATH)

sqlr-connection-odbc-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ODBCSTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-odbc $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-odbc-debug $(bindir)
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
