include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(ORACLEINCLUDES)
LIBS = $(ORACLELIBS) $(abs_srcdir)/src/connection/libsqlrconnection.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(ORACLELIBS) $(abs_srcdir)/src/connection/libsqlrconnection_p.la $(abs_srcdir)/src/util/libsqlrutil_p.la $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifeq ($(strip $(ORACLE7LIBSPATH)),)
	RPATH =
else
	RPATH = -rpath $(ORACLE7LIBSPATH)
endif

SRCS= oracle7connection.C main.C
NLOBJS= oracle7connection.n.lo main.n.lo
PLOBJS= oracle7connection.p.lo main.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-oracle7 sqlr-connection-oracle7-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-oracle7
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-oracle7-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-oracle7: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ORACLESTATIC) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS) $(RPATH)

sqlr-connection-oracle7-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ORACLESTATIC) $(DEBUGLDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-connection-oracle7 $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-connection-oracle7-debug $(bindir)
	$(abs_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(abs_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-oracle7
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-oracle7-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
