include ../../../../config.mk

top_builddir = ../../../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../../ -I../include -I../../c++/include $(RUDIMENTSINCLUDES)

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
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(NLOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libsqlrclientwrapper_p.la: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(PLOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(top_builddir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) -s libsqlrclientwrapper.la $(libdir)/libsqlrclientwrapper.la
	$(LIBTOOL) --mode=install $(INSTALL) libsqlrclientwrapper_p.la $(libdir)/libsqlrclientwrapper_p.la
	$(top_builddir)/mkinstalldirs $(cachedir)
	chmod 777 $(cachedir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper_p.la
	$(RMTREE) $(cachedir)
	$(RMTREE) $(prefix)/lib/sqlrelay/cache
