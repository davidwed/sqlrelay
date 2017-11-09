top_builddir = ../../..

include ../../../config.mk

all: libmysql3$(SQLRELAY).$(LIBEXT) libmysql40$(SQLRELAY).$(LIBEXT) libmysql41$(SQLRELAY).$(LIBEXT) libmysql50$(SQLRELAY).$(LIBEXT) libmysql51$(SQLRELAY).$(LIBEXT)

.SUFFIXES: .lo

mysql3.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) -DCOMPAT_MYSQL_3=1 $(COMPILE) $< $(OUT)$@

mysql3.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) /D COMPAT_MYSQL_3=1 $(COMPILE) mysql.cpp
	$(MV) mysql.obj mysql3.obj

mysql40.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) -DCOMPAT_MYSQL_4_0=1 $(COMPILE) $< $(OUT)$@

mysql40.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) /D COMPAT_MYSQL_4_0=1 $(COMPILE) mysql.cpp
	$(MV) mysql.obj mysql40.obj

mysql41.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) -DCOMPAT_MYSQL_4_1=1 $(COMPILE) $< $(OUT)$@

mysql41.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) /D COMPAT_MYSQL_4_1=1 $(COMPILE) mysql.cpp
	$(MV) mysql.obj mysql41.obj

mysql50.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) -DCOMPAT_MYSQL_5_0=1 $(COMPILE) $< $(OUT)$@

mysql50.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) /D COMPAT_MYSQL_5_0=1 $(COMPILE) mysql.cpp
	$(MV) mysql.obj mysql50.obj

mysql51.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) -DCOMPAT_MYSQL_5_1=1 $(COMPILE) $< $(OUT)$@

mysql51.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(MYSQLDRLIBCPPFLAGS) /D COMPAT_MYSQL_5_1=1 $(COMPILE) mysql.cpp
	$(MV) mysql.obj mysql51.obj

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

libmysql3$(SQLRELAY).$(LIBEXT): mysql.cpp mysql3.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql3.$(OBJ) $(LDFLAGS) $(MYSQLDRLIBLIBS) $(CLTLINKFLAGS)

libmysql40$(SQLRELAY).$(LIBEXT): mysql.cpp mysql40.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql40.$(OBJ) $(LDFLAGS) $(MYSQLDRLIBLIBS) $(CLTLINKFLAGS)

libmysql41$(SQLRELAY).$(LIBEXT): mysql.cpp mysql41.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql41.$(OBJ) $(LDFLAGS) $(MYSQLDRLIBLIBS) $(CLTLINKFLAGS)

libmysql50$(SQLRELAY).$(LIBEXT): mysql.cpp mysql50.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql50.$(OBJ) $(LDFLAGS) $(MYSQLDRLIBLIBS) $(CLTLINKFLAGS)

libmysql51$(SQLRELAY).$(LIBEXT): mysql.cpp mysql51.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql51.$(OBJ) $(LDFLAGS) $(MYSQLDRLIBLIBS) $(CLTLINKFLAGS)

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(EXAMPLEDIR)
	$(CP) sample-oracle-mysql-errors.map $(EXAMPLEDIR)
	$(CHMOD) 644 $(EXAMPLEDIR)/sample-oracle-mysql-errors.map
	$(CP) sample-mssqlserver-mysql-errors.map $(EXAMPLEDIR)
	$(CHMOD) 644 $(EXAMPLEDIR)/sample-mssqlserver-mysql-errors.map

installdll:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libmysql3$(SQLRELAY).$(LIBEXT) $(bindir)
	$(LTINSTALL) $(CP) libmysql40$(SQLRELAY).$(LIBEXT) $(bindir)
	$(LTINSTALL) $(CP) libmysql41$(SQLRELAY).$(LIBEXT) $(bindir)
	$(LTINSTALL) $(CP) libmysql50$(SQLRELAY).$(LIBEXT) $(bindir)
	$(LTINSTALL) $(CP) libmysql51$(SQLRELAY).$(LIBEXT) $(bindir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libmysql3$(SQLRELAY).$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql3$(SQLRELAY).a \
		$(libdir)/libmysql3$(SQLRELAY).so \
		$(libdir)/libmysql3$(SQLRELAY).$(LIBEXT)
	$(LTINSTALL) $(CP) libmysql40$(SQLRELAY).$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql40$(SQLRELAY).a \
		$(libdir)/libmysql40$(SQLRELAY).so \
		$(libdir)/libmysql40$(SQLRELAY).$(LIBEXT)
	$(LTINSTALL) $(CP) libmysql41$(SQLRELAY).$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql41$(SQLRELAY).a \
		$(libdir)/libmysql41$(SQLRELAY).so \
		$(libdir)/libmysql41$(SQLRELAY).$(LIBEXT)
	$(LTINSTALL) $(CP) libmysql50$(SQLRELAY).$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql50$(SQLRELAY).a \
		$(libdir)/libmysql50$(SQLRELAY).so \
		$(libdir)/libmysql50$(SQLRELAY).$(LIBEXT)
	$(LTINSTALL) $(CP) libmysql51$(SQLRELAY).$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libmysql51$(SQLRELAY).a \
		$(libdir)/libmysql51$(SQLRELAY).so \
		$(libdir)/libmysql51$(SQLRELAY).$(LIBEXT)

uninstall: $(UNINSTALLLIB)
	$(RM) $(EXAMPLEDIR)/sample-oracle-mysql-errors.map \
		$(EXAMPLEDIR)/sample-mssqlserver-mysql-errors.map \
		$(datadir)/examples/sample-oracle-mysql-errors.map \
		$(datadir)/examples/sample-mssqlserver-mysql-errors.map

uninstalldll:
	$(RM) $(bindir)/libmysql*$(SQLRELAY)* $(bindir)/libmysql*sqlrelay*

uninstalllib:
	$(RM) $(libdir)/libmysql*$(SQLRELAY)* $(libdir)/libmysql*sqlrelay*
