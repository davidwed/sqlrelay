include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(POSTGRESQLINCLUDES)
LIBS = $(POSTGRESQLLIBS) $(abs_srcdir)/src/connection/libsqlrconnection.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(POSTGRESQLLIBS) $(abs_srcdir)/src/connection/libsqlrconnection_p.la $(abs_srcdir)/src/util/libsqlrutil_p.la $(RUDIMENTSDEBUGLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifeq ($(strip $(POSTGRESQLLIBSPATH)),)
	RPATH =
else
	RPATH = -rpath $(POSTGRESQLLIBSPATH)
endif

SRCS= postgresqlconnection.C main.C
NLOBJS= postgresqlconnection.n.lo main.n.lo
PLOBJS= postgresqlconnection.p.lo main.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	#$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-postgresql sqlr-connection-postgresql-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-postgresql
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-postgresql-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-postgresql: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(POSTGRESQLSTATIC) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS) $(RPATH)

sqlr-connection-postgresql-debug: $(SRCS) $(PLOBJS)
	#$(LIBTOOL) --mode=link $(CXX) $(POSTGRESQLSTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS) $(RPATH)
	$(LIBTOOL) --mode=link $(CXX) $(POSTGRESQLSTATIC) $(LDFLAGS) -o $@ $(PLOBJS) $(LIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-postgresql $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-postgresql-debug $(bindir)
	$(abs_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(abs_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-postgresql
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-postgresql-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
