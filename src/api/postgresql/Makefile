include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/api/c++/include -I$(abs_srcdir)/src/common $(RUDIMENTSINCLUDES)
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)

SRCS = pqasync.C pqconndefaults.C pqconnection.C pqcopy.C \
	pqencoding.C pqescape.C pqfastpath.C pqlob.C pqnoticeprocessor.C \
	pqnotifies.C pqprint.C pqresult.C pqtrace.C
NLOBJS = pqasync.n.lo pqconndefaults.n.lo pqconnection.n.lo pqcopy.n.lo \
	pqencoding.n.lo pqescape.n.lo pqfastpath.n.lo pqlob.n.lo \
	pqnoticeprocessor.n.lo pqnotifies.n.lo pqprint.n.lo pqresult.n.lo \
	pqtrace.n.lo

.SUFFIXES: .n.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libpq.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

libpq.la: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(NLOBJS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(abs_srcdir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) -s libqp.la $(libdir)/libpq.la
	$(LIBTOOL) --mode=finish $(libdir)
	$(abs_srcdir)/mkinstalldirs $(cachedir)
	chmod 777 $(cachedir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libpq.la
	$(RMTREE) $(cachedir)
	$(RMTREE) $(prefix)/lib/sqlrelay/cache
