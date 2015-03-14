top_builddir = ../../..

include ../../../config.mk

ifeq ($(DESTDIR),)
	INSTPREFIX = 
else
	INSTPREFIX = -inst-prefix-dir $(DESTDIR)
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) -DSQLR_VERSION=\"$(SQLR_VERSION)\"
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)
ifneq ($(strip $(libdir)),)
ifeq ($(PSQLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

all: libmysql3sqlrelay.la libmysql40sqlrelay.la libmysql41sqlrelay.la libmysql50sqlrelay.la libmysql51sqlrelay.la

.SUFFIXES: .lo

mysql3.lo: mysql.cpp
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_3=1 -c $< -o $@

mysql40.lo: mysql.cpp
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_4_0=1 -c $< -o $@

mysql41.lo: mysql.cpp
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_4_1=1 -c $< -o $@

mysql50.lo: mysql.cpp
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_5_0=1 -c $< -o $@

mysql51.lo: mysql.cpp
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_5_1=1 -c $< -o $@

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

libmysql3sqlrelay.la: mysql.cpp mysql3.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql3.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libmysql40sqlrelay.la: mysql.cpp mysql40.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql40.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libmysql41sqlrelay.la: mysql.cpp mysql41.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql41.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libmysql50sqlrelay.la: mysql.cpp mysql50.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql50.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libmysql51sqlrelay.la: mysql.cpp mysql51.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysql51.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

install:
	$(MKINSTALLDIRS) $(EXAMPLEDIR)
	$(INSTALL) -m 644 sample-oracle-mysql-errors.map $(EXAMPLEDIR)
	$(INSTALL) -m 644 sample-mssqlserver-mysql-errors.map $(EXAMPLEDIR)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libmysql3sqlrelay.la $(libdir)
	$(RM) $(libdir)/libmysql3sqlrelay.a
	$(RM) $(libdir)/libmysql3sqlrelay.la
	$(LTINSTALL) $(CP) libmysql40sqlrelay.la $(libdir)
	$(RM) $(libdir)/libmysql40sqlrelay.a
	$(RM) $(libdir)/libmysql40sqlrelay.la
	$(LTINSTALL) $(CP) libmysql41sqlrelay.la $(libdir)
	$(RM) $(libdir)/libmysql41sqlrelay.a
	$(RM) $(libdir)/libmysql41sqlrelay.la
	$(LTINSTALL) $(CP) libmysql50sqlrelay.la $(libdir)
	$(RM) $(libdir)/libmysql50sqlrelay.a
	$(RM) $(libdir)/libmysql50sqlrelay.la
	$(LTINSTALL) $(CP) libmysql51sqlrelay.la $(libdir)
	$(RM) $(libdir)/libmysql51sqlrelay.a
	$(RM) $(libdir)/libmysql51sqlrelay.la

uninstall:
	$(RM) $(EXAMPLEDIR)/sample-oracle-mysql-errors.map
	$(RM) $(EXAMPLEDIR)/sample-mssqlserver-mysql-errors.map
	$(RM) $(libdir)/libmysql*sqlrelay*
