include ../../../config.mk

top_builddir = ../../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../authenticator -I../../connection $(RUDIMENTSINCLUDES) $(MSQLINCLUDES)
LIBS = $(MSQLLIBS) -L../../connection -lsqlrconnection -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(MSQLLIBS) -L../../connection -lsqlrconnection_p -L../../util -lsqlrutil_p $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS= msqlconnection.C main.C
LOBJS= msqlconnection.lo main.lo
PLOBJS= msqlconnection.p.lo main.p.lo

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-msql sqlr-connection-msql-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-msql
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-msql-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-msql: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MSQLSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS)

sqlr-connection-msql-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MSQLSTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-msql $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-msql-debug $(bindir)
	$(top_builddir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_builddir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-msql
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-msql-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
