include ../../../config.mk

top_builddir = ../../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../authenticator -I../../connection $(RUDIMENTSINCLUDES) $(POSTGRESQLINCLUDES)
LIBS = $(POSTGRESQLLIBS) -L../../connection -lsqlrconnection -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(POSTGRESQLLIBS) -L../../connection -lsqlrconnection_p -L../../util -lsqlrutil_p $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS= postgresqlconnection.C main.C
NLOBJS= postgresqlconnection.n.lo main.n.lo
PLOBJS= postgresqlconnection.p.lo main.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-postgresql sqlr-connection-postgresql-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-postgresql
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-postgresql-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-postgresql: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(POSTGRESQLSTATIC) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS)

sqlr-connection-postgresql-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(POSTGRESQLSTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-postgresql $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-postgresql-debug $(bindir)
	$(top_builddir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_builddir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-postgresql
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-postgresql-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
