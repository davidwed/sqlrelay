top_builddir = ../../..

include ../../../config.mk

ifeq ($(DESTDIR),)
	INSTPREFIX = 
else
	INSTPREFIX = -inst-prefix-dir $(DESTDIR)
endif

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)
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

all: libpqsqlrelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

rebuild: clean all

libpqsqlrelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(INSTALL) libpqsqlrelay.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libpqsqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libpqsqlrelay_p.la
