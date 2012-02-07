top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

ifeq ($(DESTDIR),)
	INSTPREFIX = 
else
	INSTPREFIX = -inst-prefix-dir $(DESTDIR)
endif

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) -DSQLR_VERSION=\"$(SQLR_VERSION)\"
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)
ifneq ($(strip $(libdir)),)
ifeq ($(PSQLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

all: libmysql3sqlrelay.la libmysql40sqlrelay.la libmysql41sqlrelay.la libmysql50sqlrelay.la libmysql51sqlrelay.la

.SUFFIXES: .lo

mysql3.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_3=1 -c $< -o $@

mysql40.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_4_0=1 -c $< -o $@

mysql41.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_4_1=1 -c $< -o $@

mysql50.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_5_0=1 -c $< -o $@

mysql51.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_5_1=1 -c $< -o $@

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

libmysql3sqlrelay.la: mysql.C mysql3.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql3.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libmysql40sqlrelay.la: mysql.C mysql40.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql40.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libmysql41sqlrelay.la: mysql.C mysql41.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql41.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libmysql50sqlrelay.la: mysql.C mysql50.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql50.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libmysql51sqlrelay.la: mysql.C mysql51.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql51.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

install:
	$(MKINSTALLDIRS) $(EXAMPLEDIR)
	$(INSTALL) -m 644 sample-oracle-mysql-errors.map $(EXAMPLEDIR)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libmysql3sqlrelay.la $(libdir)
	$(LTINSTALL) libmysql40sqlrelay.la $(libdir)
	$(LTINSTALL) libmysql41sqlrelay.la $(libdir)
	$(LTINSTALL) libmysql50sqlrelay.la $(libdir)
	$(LTINSTALL) libmysql51sqlrelay.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(RM) $(EXAMPLEDIR)/sample-oracle-mysql-errors.map
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysql3sqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysql40sqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysql41sqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysql50sqlrelay.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libmysql51sqlrelay.la
