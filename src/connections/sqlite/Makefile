include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(SQLITEINCLUDES)
LIBS = $(SQLITELIBS) $(abs_srcdir)/src/connection/libsqlrconnection.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(SQLITELIBS) $(abs_srcdir)/src/connection/libsqlrconnection_p.la $(abs_srcdir)/src/util/libsqlrutil_p.la $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifeq ($(strip $(SQLITELIBSPATH)),)
	RPATH =
else
	RPATH = -rpath $(SQLITELIBSPATH)
endif

SRCS= sqliteconnection.C main.C
NLOBJS= sqliteconnection.n.lo main.n.lo
PLOBJS= sqliteconnection.p.lo main.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-sqlite sqlr-connection-sqlite-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-sqlite
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-sqlite-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-sqlite: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(SQLITESTATIC) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS) $(RPATH)

sqlr-connection-sqlite-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(SQLITESTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-sqlite $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-sqlite-debug $(bindir)
	$(abs_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(abs_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-sqlite
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-sqlite-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
