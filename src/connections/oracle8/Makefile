include ../../../config.mk

top_builddir = ../../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../authenticator -I../../connection $(RUDIMENTSINCLUDES) $(ORACLEINCLUDES)
LIBS = $(ORACLELIBS) -L../../connection -lsqlrconnection -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(ORACLELIBS) -L../../connection -lsqlrconnection_p -L../../util -lsqlrutil_p $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE) -lefence

SRCS= oracle8connection.C main.C
NLOBJS= oracle8connection.n.lo main.n.lo
PLOBJS= oracle8connection.p.lo main.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-oracle8 sqlr-connection-oracle8-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-oracle8
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-oracle8-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-oracle8: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ORACLESTATIC) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS)

sqlr-connection-oracle8-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ORACLESTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-oracle8 $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-oracle8-debug $(bindir)
	$(top_builddir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_builddir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-oracle8
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-oracle8-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
