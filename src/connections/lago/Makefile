include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(LAGOINCLUDES)
LIBS = $(LAGOLIBS) $(abs_srcdir)/src/connection/libsqlrconnection.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(NSLLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(LAGOLIBS) $(abs_srcdir)/src/connection/libsqlrconnection_p.la $(abs_srcdir)/src/util/libsqlrutil_p.la $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(NSLLIB) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifeq ($(strip $(LAGOLIBSPATH)),)
	RPATH =
else
	RPATH = -rpath $(LAGOLIBSPATH)
endif

SRCS= lagoconnection.C main.C
NLOBJS= lagoconnection.n.lo main.n.lo
PLOBJS= lagoconnection.p.lo main.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
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

sqlr-connection-lago: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(LAGOSTATIC) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS) $(RPATH)

sqlr-connection-lago-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(LAGOSTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-lago $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-lago-debug $(bindir)
	$(abs_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(abs_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-lago
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-lago-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
