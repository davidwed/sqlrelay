include ../../../../config.mk


INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/api/c/include -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES)

SRCS = sqlrclientwrapper.C
NLOBJS = sqlrclientwrapper.n.lo
PLOBJS = sqlrclientwrapper.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrclientwrapper.la libsqlrclientwrapper_p.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

libsqlrclientwrapper.la: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(NLOBJS) -L$(abs_srcdir)/src/api/c++/src -L$(abs_srcdir)/src/api/c++/src/.libs -lsqlrclient $(RUDIMENTSLIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libsqlrclientwrapper_p.la: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(PLOBJS) -L$(abs_srcdir)/src/api/c++/src -L$(abs_srcdir)/src/api/c++/src/.libs -lsqlrclient_p $(RUDIMENTSDEBUGLIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(abs_srcdir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) -s libsqlrclientwrapper.la $(libdir)/libsqlrclientwrapper.la
	$(LIBTOOL) --mode=install $(INSTALL) libsqlrclientwrapper_p.la $(libdir)/libsqlrclientwrapper_p.la
	$(abs_srcdir)/mkinstalldirs $(cachedir)
	chmod 777 $(cachedir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper_p.la
	$(RMTREE) $(cachedir)
	$(RMTREE) $(prefix)/lib/sqlrelay/cache
