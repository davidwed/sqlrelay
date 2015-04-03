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



db2connection.lo: db2connection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DB2CONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

db2connection.obj: db2connection.cpp
	$(CXX) $(CXXFLAGS) $(DB2CONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_db2.$(LIBEXT): db2connection.$(OBJ) db2connection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ db2connection.$(OBJ) $(LDFLAGS) $(DB2CONNECTIONLIBS) $(MODLINKFLAGS)



firebirdconnection.lo: firebirdconnection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(FIREBIRDCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

firebirdconnection.obj: firebirdconnection.cpp
	$(CXX) $(CXXFLAGS) $(FIREBIRDCONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_firebird.$(LIBEXT): firebirdconnection.$(OBJ) firebirdconnection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ firebirdconnection.$(OBJ) $(LDFLAGS) $(FIREBIRDCONNECTIONLIBS) $(MODLINKFLAGS)



freetdsconnection.lo: freetdsconnection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(FREETDSCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

freetdsconnection.obj: freetdsconnection.cpp
	$(CXX) $(CXXFLAGS) $(FREETDSCONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_freetds.$(LIBEXT): freetdsconnection.$(OBJ) freetdsconnection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ freetdsconnection.$(OBJ) $(LDFLAGS) $(FREETDSCONNECTIONLIBS) $(MODLINKFLAGS)



mdbtoolsconnection.lo: mdbtoolsconnection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MDBTOOLSCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

mdbtoolsconnection.obj: mdbtoolsconnection.cpp
	$(CXX) $(CXXFLAGS) $(MDBTOOLSCONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_mdbtools.$(LIBEXT): mdbtoolsconnection.$(OBJ) mdbtoolsconnection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ mdbtoolsconnection.$(OBJ) $(LDFLAGS) $(MDBTOOLSCONNECTIONLIBS) $(MODLINKFLAGS)



mysqlconnection.lo: mysqlconnection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MYSQLCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

mysqlconnection.obj: mysqlconnection.cpp
	$(CXX) $(CXXFLAGS) $(MYSQLCONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_mysql.$(LIBEXT): mysqlconnection.$(OBJ) mysqlconnection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ mysqlconnection.$(OBJ) $(LDFLAGS) $(MYSQLCONNECTIONLIBS) $(MODLINKFLAGS)



odbcconnection.lo: odbcconnection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ODBCCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

odbcconnection.obj: odbcconnection.cpp
	$(CXX) $(CXXFLAGS) $(ODBCCONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_odbc.$(LIBEXT): odbcconnection.$(OBJ) odbcconnection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ odbcconnection.$(OBJ) $(LDFLAGS) $(ODBCCONNECTIONLIBS) $(MODLINKFLAGS)



oracle8connection.lo: oracle8connection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ORACLECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

oracle8connection.obj: oracle8connection.cpp
	$(CXX) $(CXXFLAGS) $(ORACLECONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_oracle8.$(LIBEXT): oracle8connection.$(OBJ) oracle8connection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ oracle8connection.$(OBJ) $(LDFLAGS) $(ORACLECONNECTIONLIBS) $(MODLINKFLAGS)



postgresqlconnection.lo: postgresqlconnection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(POSTGRESQLCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

postgresqlconnection.obj: postgresqlconnection.cpp
	$(CXX) $(CXXFLAGS) $(POSTGRESQLCONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_postgresql.$(LIBEXT): postgresqlconnection.$(OBJ) postgresqlconnection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ postgresqlconnection.$(OBJ) $(LDFLAGS) $(POSTGRESQLCONNECTIONLIBS) $(MODLINKFLAGS)



routerconnection.lo: routerconnection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ROUTERCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

routerconnection.obj: routerconnection.cpp
	$(CXX) $(CXXFLAGS) $(ROUTERCONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_router.$(LIBEXT): routerconnection.$(OBJ) routerconnection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ routerconnection.$(OBJ) $(LDFLAGS) $(ROUTERCONNECTIONLIBS) $(MODLINKFLAGS)



sqliteconnection.lo: sqliteconnection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SQLITECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

sqliteconnection.obj: sqliteconnection.cpp
	$(CXX) $(CXXFLAGS) $(SQLITECONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_sqlite.$(LIBEXT): sqliteconnection.$(OBJ) sqliteconnection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ sqliteconnection.$(OBJ) $(LDFLAGS) $(SQLITECONNECTIONLIBS) $(MODLINKFLAGS)



sybaseconnection.lo: sybaseconnection.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SYBASECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

sybaseconnection.obj: sybaseconnection.cpp
	$(CXX) $(CXXFLAGS) $(SYBASECONNECTIONCPPFLAGS) $(COMPILE) $<

sqlrconnection_sybase.$(LIBEXT): sybaseconnection.$(OBJ) sybaseconnection.cpp
	$(LTLINK) $(LINK) $(OUT)$@ sybaseconnection.$(OBJ) $(LDFLAGS) $(SYBASECONNECTIONLIBS) $(MODLINKFLAGS)



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
