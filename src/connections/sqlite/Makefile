include ../../../config.mk

top_builddir = ../../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../authenticator -I../../connection $(RUDIMENTSINCLUDES) $(SQLITEINCLUDES)
LIBS = $(SQLITELIBS) -L../../connection -lsqlrconnection -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(SQLITELIBS) -L../../connection -lsqlrconnection_p -L../../util -lsqlrutil_p $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

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
	$(LIBTOOL) --mode=link $(CXX) $(SQLITESTATIC) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS)

sqlr-connection-sqlite-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(SQLITESTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-sqlite $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-sqlite-debug $(bindir)
	$(top_builddir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_builddir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-sqlite
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-sqlite-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
