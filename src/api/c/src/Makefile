include ../../../../config.mk


INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/api/c/include -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES)

SRCS = sqlrclientwrapper.C
LOBJS = sqlrclientwrapper.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrclientwrapper.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

libsqlrclientwrapper.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(abs_srcdir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) libsqlrclientwrapper.la $(libdir)/libsqlrclientwrapper.la
	$(abs_srcdir)/mkinstalldirs $(cachedir)
	chmod 777 $(cachedir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclientwrapper_p.la
	$(RMTREE) $(cachedir)
	$(RMTREE) $(prefix)/lib/sqlrelay/cache
