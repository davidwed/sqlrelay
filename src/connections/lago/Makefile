include ../../../config.mk

top_builddir = ../../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../authenticator -I../../connection $(RUDIMENTSINCLUDES) $(LAGOINCLUDES)
LIBS = $(LAGOLIBS) -L../../connection -lsqlrconnection -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(NSLLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(LAGOLIBS) -L../../connection -lsqlrconnection_p -L../../util -lsqlrutil_p $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(NSLLIB) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS= lagoconnection.C main.C
LOBJS= lagoconnection.lo main.lo
PLOBJS= lagoconnection.p.lo main.p.lo

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-lago sqlr-connection-lago-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-lago
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-lago-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-lago: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(LAGOSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS)

sqlr-connection-lago-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(LAGOSTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-lago $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-lago-debug $(bindir)
	$(top_builddir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_builddir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-lago
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-lago-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
