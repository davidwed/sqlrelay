include ../../../config.mk

top_builddir = ../../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../authenticator -I../../connection $(RUDIMENTSINCLUDES) $(MYSQLINCLUDES)
LIBS = $(MYSQLLIBS) -L../../connection -lsqlrconnection -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(MYSQLLIBS) -L../../connection -lsqlrconnection_p -L../../util -lsqlrutil_p $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS= mysqlconnection.C main.C
LOBJS= mysqlconnection.lo main.lo
PLOBJS= mysqlconnection.p.lo main.p.lo

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-mysql sqlr-connection-mysql-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-mysql
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-mysql-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-mysql: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MYSQLSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS)

sqlr-connection-mysql-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MYSQLSTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-mysql $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-mysql-debug $(bindir)
	$(top_builddir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_builddir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-mysql
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-mysql-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
