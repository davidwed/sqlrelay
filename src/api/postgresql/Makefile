include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/api/c++/include -I$(abs_srcdir)/src/common $(RUDIMENTSINCLUDES)
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(STDCPPLIB)
DEBUGLIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient_p $(RUDIMENTSDEBUGLIBS) $(STDCPPLIB) $(LIBEFENCE)

SRCS = pqasync.C pqconndefaults.C pqconnection.C pqcopy.C \
	pqencoding.C pqescape.C pqfastpath.C pqlob.C \
	pqnotifies.C pqprint.C pqresult.C pqtrace.C
NLOBJS = pqasync.n.lo pqconndefaults.n.lo pqconnection.n.lo pqcopy.n.lo \
	pqencoding.n.lo pqescape.n.lo pqfastpath.n.lo pqlob.n.lo \
	pqnotifies.n.lo pqprint.n.lo pqresult.n.lo pqtrace.n.lo
PLOBJS = pqasync.p.lo pqconndefaults.p.lo pqconnection.p.lo pqcopy.p.lo \
	pqencoding.p.lo pqescape.p.lo pqfastpath.p.lo pqlob.p.lo \
	pqnotifies.p.lo pqprint.p.lo pqresult.p.lo pqtrace.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

#all: libpq-sqlrelay.la libpq-sqlrelay_p.la
all: libpq-sqlrelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

libpq-sqlrelay.la: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(NLOBJS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libpq-sqlrelay_p.la: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(PLOBJS) $(DEBUGLIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(abs_srcdir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) -s libpq-sqlrelay.la $(libdir)/libpq-sqlrelay.la
	#$(LIBTOOL) --mode=install $(INSTALL) libpq-sqlrelay_p.la $(libdir)/libpq-sqlrelay_p.la
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libpq-sqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libpq-sqlrelay_p.la
