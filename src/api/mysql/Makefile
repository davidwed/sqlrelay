top_builddir = ../../..

include ../../../config.mk

ifeq ($(INSTALLPREFIX),)
	INSTPREFIX = 
else
	INSTPREFIX = -inst-prefix-dir $(INSTALLPREFIX)
endif

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) -DSQLR_VERSION=\"$(SQLR_VERSION)\"
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)
ifneq ($(strip $(libdir)),)
ifeq ($(PSQLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

all: libmysql3sqlrelay.la libmysql40sqlrelay.la libmysql41sqlrelay.la libmysql50sqlrelay.la

.SUFFIXES: .lo

mysql3.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_3=1 -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

mysql40.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_4_0=1 -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

mysql41.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_4_1=1 -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

mysql50.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_5_0=1 -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

libmysql3sqlrelay.la: mysql.C mysql3.lo
	$(LIBTOOL) --mode=link $(CXX) $(INSTALLPREFIX) -O -o $@ mysql3.lo $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libmysql40sqlrelay.la: mysql.C mysql40.lo
	$(LIBTOOL) --mode=link $(CXX) $(INSTALLPREFIX) -O -o $@ mysql40.lo $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libmysql41sqlrelay.la: mysql.C mysql41.lo
	$(LIBTOOL) --mode=link $(CXX) $(INSTALLPREFIX) -O -o $@ mysql41.lo $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libmysql50sqlrelay.la: mysql.C mysql50.lo
	$(LIBTOOL) --mode=link $(CXX) $(INSTALLPREFIX) -O -o $@ mysql50.lo $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(MKINSTALLDIRS) $(libdir)
	$(INSTALL) $(INSTALLSTRIP) libmysql3sqlrelay.la $(libdir)
	$(INSTALL) $(INSTALLSTRIP) libmysql40sqlrelay.la $(libdir)
	$(INSTALL) $(INSTALLSTRIP) libmysql41sqlrelay.la $(libdir)
	$(INSTALL) $(INSTALLSTRIP) libmysql50sqlrelay.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysql3sqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysql40sqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysql41sqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysql50sqlrelay.la
