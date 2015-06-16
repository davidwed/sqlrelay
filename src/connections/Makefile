top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

all: $(CONNECTIONSALLTARGETS)

all-db2: sqlrconnection_db2.$(LIBEXT)

all-firebird: sqlrconnection_firebird.$(LIBEXT)

all-freetds: sqlrconnection_freetds.$(LIBEXT)

all-mdbtools: sqlrconnection_mdbtools.$(LIBEXT)

all-mysql: sqlrconnection_mysql.$(LIBEXT)

all-odbc: sqlrconnection_odbc.$(LIBEXT)

all-oracle8: sqlrconnection_oracle8.$(LIBEXT)

all-postgresql: sqlrconnection_postgresql.$(LIBEXT)

all-router: sqlrconnection_router.$(LIBEXT)

all-sqlite: sqlrconnection_sqlite.$(LIBEXT)

all-sybase: sqlrconnection_sybase.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs



db2.lo: db2.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DB2CONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

db2.obj: db2.cpp
	$(CXX) $(CXXFLAGS) $(DB2CONNECTIONCPPFLAGS) $(COMPILE) db2.cpp

sqlrconnection_db2.$(LIBEXT): db2.$(OBJ) db2.cpp
	$(LTLINK) $(LINK) $(OUT)$@ db2.$(OBJ) $(LDFLAGS) $(DB2CONNECTIONLIBS) $(MODLINKFLAGS)



firebird.lo: firebird.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(FIREBIRDCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

firebird.obj: firebird.cpp
	$(CXX) $(CXXFLAGS) $(FIREBIRDCONNECTIONCPPFLAGS) $(COMPILE) firebird.cpp

sqlrconnection_firebird.$(LIBEXT): firebird.$(OBJ) firebird.cpp
	$(LTLINK) $(LINK) $(OUT)$@ firebird.$(OBJ) $(LDFLAGS) $(FIREBIRDCONNECTIONLIBS) $(MODLINKFLAGS)



freetds.lo: freetds.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(FREETDSCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

freetds.obj: freetds.cpp
	$(CXX) $(CXXFLAGS) $(FREETDSCONNECTIONCPPFLAGS) $(COMPILE) freetds.cpp

sqlrconnection_freetds.$(LIBEXT): freetds.$(OBJ) freetds.cpp
	$(LTLINK) $(LINK) $(OUT)$@ freetds.$(OBJ) $(LDFLAGS) $(FREETDSCONNECTIONLIBS) $(MODLINKFLAGS)



mdbtools.lo: mdbtools.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MDBTOOLSCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

mdbtools.obj: mdbtools.cpp
	$(CXX) $(CXXFLAGS) $(MDBTOOLSCONNECTIONCPPFLAGS) $(COMPILE) mdbtools.cpp

sqlrconnection_mdbtools.$(LIBEXT): mdbtools.$(OBJ) mdbtools.cpp
	$(LTLINK) $(LINK) $(OUT)$@ mdbtools.$(OBJ) $(LDFLAGS) $(MDBTOOLSCONNECTIONLIBS) $(MODLINKFLAGS)



mysql.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MYSQLCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

mysql.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(MYSQLCONNECTIONCPPFLAGS) $(COMPILE) mysql.cpp

sqlrconnection_mysql.$(LIBEXT): mysql.$(OBJ) mysql.cpp
	$(LTLINK) $(LINK) $(OUT)$@ mysql.$(OBJ) $(LDFLAGS) $(MYSQLCONNECTIONLIBS) $(MODLINKFLAGS)



odbc.lo: odbc.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ODBCCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

odbc.obj: odbc.cpp
	$(CXX) $(CXXFLAGS) $(ODBCCONNECTIONCPPFLAGS) $(COMPILE) odbc.cpp

sqlrconnection_odbc.$(LIBEXT): odbc.$(OBJ) odbc.cpp
	$(LTLINK) $(LINK) $(OUT)$@ odbc.$(OBJ) $(LDFLAGS) $(ODBCCONNECTIONLIBS) $(MODLINKFLAGS)



oracle8.lo: oracle8.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ORACLECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

oracle8.obj: oracle8.cpp
	$(CXX) $(CXXFLAGS) $(ORACLECONNECTIONCPPFLAGS) $(COMPILE) oracle8.cpp

sqlrconnection_oracle8.$(LIBEXT): oracle8.$(OBJ) oracle8.cpp
	$(LTLINK) $(LINK) $(OUT)$@ oracle8.$(OBJ) $(LDFLAGS) $(ORACLECONNECTIONLIBS) $(MODLINKFLAGS)



postgresql.lo: postgresql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(POSTGRESQLCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

postgresql.obj: postgresql.cpp
	$(CXX) $(CXXFLAGS) $(POSTGRESQLCONNECTIONCPPFLAGS) $(COMPILE) postgresql.cpp

sqlrconnection_postgresql.$(LIBEXT): postgresql.$(OBJ) postgresql.cpp
	$(LTLINK) $(LINK) $(OUT)$@ postgresql.$(OBJ) $(LDFLAGS) $(POSTGRESQLCONNECTIONLIBS) $(MODLINKFLAGS)



router.lo: router.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ROUTERCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

router.obj: router.cpp
	$(CXX) $(CXXFLAGS) $(ROUTERCONNECTIONCPPFLAGS) $(COMPILE) router.cpp

sqlrconnection_router.$(LIBEXT): router.$(OBJ) router.cpp
	$(LTLINK) $(LINK) $(OUT)$@ router.$(OBJ) $(LDFLAGS) $(ROUTERCONNECTIONLIBS) $(MODLINKFLAGS)



sqlite.lo: sqlite.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SQLITECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

sqlite.obj: sqlite.cpp
	$(CXX) $(CXXFLAGS) $(SQLITECONNECTIONCPPFLAGS) $(COMPILE) sqlite.cpp

sqlrconnection_sqlite.$(LIBEXT): sqlite.$(OBJ) sqlite.cpp
	$(LTLINK) $(LINK) $(OUT)$@ sqlite.$(OBJ) $(LDFLAGS) $(SQLITECONNECTIONLIBS) $(MODLINKFLAGS)



sybase.lo: sybase.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SYBASECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

sybase.obj: sybase.cpp
	$(CXX) $(CXXFLAGS) $(SYBASECONNECTIONCPPFLAGS) $(COMPILE) sybase.cpp

sqlrconnection_sybase.$(LIBEXT): sybase.$(OBJ) sybase.cpp
	$(LTLINK) $(LINK) $(OUT)$@ sybase.$(OBJ) $(LDFLAGS) $(SYBASECONNECTIONLIBS) $(MODLINKFLAGS)



install: $(INSTALLLIB)

installdll: $(CONNECTIONSINSTALLTARGETS)

installdll-db2:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_db2.$(LIBEXT) $(libexecdir)

installdll-firebird:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_firebird.$(LIBEXT) $(libexecdir)

installdll-freetds:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_freetds.$(LIBEXT) $(libexecdir)

installdll-mdbtools:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mdbtools.$(LIBEXT) $(libexecdir)

installdll-mysql:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mysql.$(LIBEXT) $(libexecdir)

installdll-odbc:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_odbc.$(LIBEXT) $(libexecdir)

installdll-oracle8:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_oracle8.$(LIBEXT) $(libexecdir)

installdll-postgresql:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_postgresql.$(LIBEXT) $(libexecdir)

installdll-router:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_router.$(LIBEXT) $(libexecdir)

installdll-sqlite:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sqlite.$(LIBEXT) $(libexecdir)

installdll-sybase:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sybase.$(LIBEXT) $(libexecdir)


installlib: $(INSTALLSHAREDLIB)

installsharedlib: $(CONNECTIONSINSTALLTARGETS)

installsharedlib-db2:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_db2.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_db2.a
	$(RM) $(libexecdir)/sqlrconnection_db2.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_db2.so so $(MODULESUFFIX)

installsharedlib-firebird:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_firebird.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_firebird.a
	$(RM) $(libexecdir)/sqlrconnection_firebird.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_firebird.so so $(MODULESUFFIX)

installsharedlib-freetds:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_freetds.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_freetds.a
	$(RM) $(libexecdir)/sqlrconnection_freetds.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_freetds.so so $(MODULESUFFIX)

installsharedlib-mdbtools:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mdbtools.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.a
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_mdbtools.so so $(MODULESUFFIX)

installsharedlib-mysql:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mysql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_mysql.a
	$(RM) $(libexecdir)/sqlrconnection_mysql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_mysql.so so $(MODULESUFFIX)

installsharedlib-odbc:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_odbc.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_odbc.a
	$(RM) $(libexecdir)/sqlrconnection_odbc.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_odbc.so so $(MODULESUFFIX)

installsharedlib-oracle8:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_oracle8.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_oracle8.a
	$(RM) $(libexecdir)/sqlrconnection_oracle8.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_oracle8.so so $(MODULESUFFIX)

installsharedlib-postgresql:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_postgresql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_postgresql.a
	$(RM) $(libexecdir)/sqlrconnection_postgresql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_postgresql.so so $(MODULESUFFIX)

installsharedlib-router:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_router.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_router.a
	$(RM) $(libexecdir)/sqlrconnection_router.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_router.so so $(MODULESUFFIX)

installsharedlib-sqlite:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sqlite.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_sqlite.a
	$(RM) $(libexecdir)/sqlrconnection_sqlite.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_sqlite.so so $(MODULESUFFIX)

installsharedlib-sybase:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sybase.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_sybase.a
	$(RM) $(libexecdir)/sqlrconnection_sybase.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_sybase.so so $(MODULESUFFIX)


uninstall:
	$(RM) $(libexecdir)/sqlrconnection_*
