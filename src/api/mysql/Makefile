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

all: libmysql3sqlrelay.$(LIBEXT) libmysql40sqlrelay.$(LIBEXT) libmysql41sqlrelay.$(LIBEXT) libmysql50sqlrelay.$(LIBEXT) libmysql51sqlrelay.$(LIBEXT)

.SUFFIXES: .lo

mysql3.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_3=1 $(COMPILE) $< $(OUT)$@

mysql3.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /D COMPAT_MYSQL_3=1 $(COMPILE) $<

mysql40.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_4_0=1 $(COMPILE) $< $(OUT)$@

mysql40.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /D COMPAT_MYSQL_4_0=1 $(COMPILE) $<

mysql41.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_4_1=1 $(COMPILE) $< $(OUT)$@

mysql41.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /D COMPAT_MYSQL_4_1=1 $(COMPILE) $<

mysql50.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_5_0=1 $(COMPILE) $< $(OUT)$@

mysql50.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /D COMPAT_MYSQL_5_0=1 $(COMPILE) $<

mysql51.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPAT_MYSQL_5_1=1 $(COMPILE) $< $(OUT)$@

mysql51.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) /D COMPAT_MYSQL_5_1=1 $(COMPILE) $<

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest

libmysql3sqlrelay.$(LIBEXT): mysql.cpp mysql3.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql3.$(OBJ) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

libmysql40sqlrelay.$(LIBEXT): mysql.cpp mysql40.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql40.$(OBJ) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

libmysql41sqlrelay.$(LIBEXT): mysql.cpp mysql41.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql41.$(OBJ) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

libmysql50sqlrelay.$(LIBEXT): mysql.cpp mysql50.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql50.$(OBJ) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

libmysql51sqlrelay.$(LIBEXT): mysql.cpp mysql51.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql51.$(OBJ) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(EXAMPLEDIR)
	$(INSTALL) -m 644 sample-oracle-mysql-errors.map $(EXAMPLEDIR)
	$(INSTALL) -m 644 sample-mssqlserver-mysql-errors.map $(EXAMPLEDIR)

installdll:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libmysql3sqlrelay.$(LIBEXT) $(bindir)
	$(LTINSTALL) $(CP) libmysql40sqlrelay.$(LIBEXT) $(bindir)
	$(LTINSTALL) $(CP) libmysql41sqlrelay.$(LIBEXT) $(bindir)
	$(LTINSTALL) $(CP) libmysql50sqlrelay.$(LIBEXT) $(bindir)
	$(LTINSTALL) $(CP) libmysql51sqlrelay.$(LIBEXT) $(bindir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libmysql3sqlrelay.$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql3sqlrelay.a
	$(RM) $(libdir)/libmysql3sqlrelay.$(LIBEXT)
	$(LTINSTALL) $(CP) libmysql40sqlrelay.$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql40sqlrelay.a
	$(RM) $(libdir)/libmysql40sqlrelay.$(LIBEXT)
	$(LTINSTALL) $(CP) libmysql41sqlrelay.$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql41sqlrelay.a
	$(RM) $(libdir)/libmysql41sqlrelay.$(LIBEXT)
	$(LTINSTALL) $(CP) libmysql50sqlrelay.$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql50sqlrelay.a
	$(RM) $(libdir)/libmysql50sqlrelay.$(LIBEXT)
	$(LTINSTALL) $(CP) libmysql51sqlrelay.$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql51sqlrelay.a
	$(RM) $(libdir)/libmysql51sqlrelay.$(LIBEXT)

uninstall: $(UNINSTALLLIB)
	$(RM) $(EXAMPLEDIR)/sample-oracle-mysql-errors.map
	$(RM) $(EXAMPLEDIR)/sample-mssqlserver-mysql-errors.map

uninstalldll:
	$(RM) $(bindir)/libmysql*sqlrelay*

uninstalllib:
	$(RM) $(libdir)/libmysql*sqlrelay*
