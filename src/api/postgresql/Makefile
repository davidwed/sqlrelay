include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(PTHREADLIBS) $(STDCPPLIB)
ifneq ($(strip $(libdir)),)
ifeq ($(PSQLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = pqasync.C pqconndefaults.C pqconnection.C pqcopy.C \
	pqencoding.C pqescape.C pqfastpath.C pqlob.C \
	pqnotifies.C pqprint.C pqresult.C pqtrace.C
LOBJS = pqasync.lo pqconndefaults.lo pqconnection.lo pqcopy.lo \
	pqencoding.lo pqescape.lo pqfastpath.lo pqlob.lo \
	pqnotifies.lo pqprint.lo pqresult.lo pqtrace.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	if ( test "$(STRIP)" != "touch" ); then \
		$(STRIP) -x -R .note -R .comment $@; \
	fi

all: libpqsqlrelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

libpqsqlrelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -O -o $@ $(LOBJS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(abs_srcdir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) libpqsqlrelay.la $(libdir)/libpqsqlrelay.la
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libpqsqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libpqsqlrelay_p.la
