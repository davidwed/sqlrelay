top_builddir = ../../..

include ../../../config.mk

ifeq ($(INSTALLPREFIX),)
	INSTPREFIX = 
else
	INSTPREFIX = -inst-prefix-dir $(INSTALLPREFIX)
endif

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) -DSQLR_VERSION=\"$(SQLR_VERSION)\"
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(PTHREADLIBS)
ifneq ($(strip $(libdir)),)
ifeq ($(PSQLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = mysql.C
LOBJS = mysql.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

all: libmysqlsqlrelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

libmysqlsqlrelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(INSTALLPREFIX) -O -o $@ $(LOBJS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(MKINSTALLDIRS) $(libdir)
	$(INSTALL) $(INSTALLSTRIP) libmysqlsqlrelay.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysqlsqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysqlsqlrelay_p.la
