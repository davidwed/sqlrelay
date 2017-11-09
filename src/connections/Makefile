top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

all: $(CONNECTIONSALLTARGETS)

all-db2: $(SQLR)connection_db2.$(LIBEXT)

all-firebird: $(SQLR)connection_firebird.$(LIBEXT)

all-freetds: $(SQLR)connection_freetds.$(LIBEXT)

all-informix: $(SQLR)connection_informix.$(LIBEXT)

all-mdbtools: $(SQLR)connection_mdbtools.$(LIBEXT)

all-mysql: $(SQLR)connection_mysql.$(LIBEXT)

all-odbc: $(SQLR)connection_odbc.$(LIBEXT)

all-oracle: $(SQLR)connection_oracle.$(LIBEXT)

all-postgresql: $(SQLR)connection_postgresql.$(LIBEXT)

all-router: $(SQLR)connection_router.$(LIBEXT)

all-sqlite: $(SQLR)connection_sqlite.$(LIBEXT)

all-sap: $(SQLR)connection_sap.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs



db2.lo: db2.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DB2CONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

db2.obj: db2.cpp
	$(CXX) $(CXXFLAGS) $(DB2CONNECTIONCPPFLAGS) $(COMPILE) db2.cpp

$(SQLR)connection_db2.$(LIBEXT): db2.$(OBJ) db2.cpp
	$(LTLINK) $(LINK) $(OUT)$@ db2.$(OBJ) $(LDFLAGS) $(DB2CONNECTIONLIBS) $(MODLINKFLAGS)



firebird.lo: firebird.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(FIREBIRDCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

firebird.obj: firebird.cpp
	$(CXX) $(CXXFLAGS) $(FIREBIRDCONNECTIONCPPFLAGS) $(COMPILE) firebird.cpp

$(SQLR)connection_firebird.$(LIBEXT): firebird.$(OBJ) firebird.cpp
	$(LTLINK) $(LINK) $(OUT)$@ firebird.$(OBJ) $(LDFLAGS) $(FIREBIRDCONNECTIONLIBS) $(MODLINKFLAGS)



freetds.lo: freetds.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(FREETDSCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

freetds.obj: freetds.cpp
	$(CXX) $(CXXFLAGS) $(FREETDSCONNECTIONCPPFLAGS) $(COMPILE) freetds.cpp

$(SQLR)connection_freetds.$(LIBEXT): freetds.$(OBJ) freetds.cpp
	$(LTLINK) $(LINK) $(OUT)$@ freetds.$(OBJ) $(LDFLAGS) $(FREETDSCONNECTIONLIBS) $(MODLINKFLAGS)



informix.lo: informix.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(INFORMIXCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

informix.obj: informix.cpp
	$(CXX) $(CXXFLAGS) $(INFORMIXCONNECTIONCPPFLAGS) $(COMPILE) informix.cpp

$(SQLR)connection_informix.$(LIBEXT): informix.$(OBJ) informix.cpp
	$(LTLINK) $(LINK) $(OUT)$@ informix.$(OBJ) $(LDFLAGS) $(INFORMIXCONNECTIONLIBS) $(MODLINKFLAGS)



mdbtools.lo: mdbtools.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MDBTOOLSCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

mdbtools.obj: mdbtools.cpp
	$(CXX) $(CXXFLAGS) $(MDBTOOLSCONNECTIONCPPFLAGS) $(COMPILE) mdbtools.cpp

$(SQLR)connection_mdbtools.$(LIBEXT): mdbtools.$(OBJ) mdbtools.cpp
	$(LTLINK) $(LINK) $(OUT)$@ mdbtools.$(OBJ) $(LDFLAGS) $(MDBTOOLSCONNECTIONLIBS) $(MODLINKFLAGS)



mysql.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MYSQLCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

mysql.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(MYSQLCONNECTIONCPPFLAGS) $(COMPILE) mysql.cpp

$(SQLR)connection_mysql.$(LIBEXT): mysql.$(OBJ) mysql.cpp
	$(LTLINK) $(LINK) $(OUT)$@ mysql.$(OBJ) $(LDFLAGS) $(MYSQLCONNECTIONLIBS) $(MODLINKFLAGS)



odbc.lo: odbc.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ODBCCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

odbc.obj: odbc.cpp
	$(CXX) $(CXXFLAGS) $(ODBCCONNECTIONCPPFLAGS) $(COMPILE) odbc.cpp

$(SQLR)connection_odbc.$(LIBEXT): odbc.$(OBJ) odbc.cpp
	$(LTLINK) $(LINK) $(OUT)$@ odbc.$(OBJ) $(LDFLAGS) $(ODBCCONNECTIONLIBS) $(MODLINKFLAGS)



oracle.lo: oracle.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ORACLECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

oracle.obj: oracle.cpp
	$(CXX) $(CXXFLAGS) $(ORACLECONNECTIONCPPFLAGS) $(COMPILE) oracle.cpp

$(SQLR)connection_oracle.$(LIBEXT): oracle.$(OBJ) oracle.cpp
	$(LTLINK) $(LINK) $(OUT)$@ oracle.$(OBJ) $(LDFLAGS) $(ORACLECONNECTIONLIBS) $(MODLINKFLAGS)



postgresql.lo: postgresql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(POSTGRESQLCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

postgresql.obj: postgresql.cpp
	$(CXX) $(CXXFLAGS) $(POSTGRESQLCONNECTIONCPPFLAGS) $(COMPILE) postgresql.cpp

$(SQLR)connection_postgresql.$(LIBEXT): postgresql.$(OBJ) postgresql.cpp
	$(LTLINK) $(LINK) $(OUT)$@ postgresql.$(OBJ) $(LDFLAGS) $(POSTGRESQLCONNECTIONLIBS) $(MODLINKFLAGS)



router.lo: router.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ROUTERCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

router.obj: router.cpp
	$(CXX) $(CXXFLAGS) $(ROUTERCONNECTIONCPPFLAGS) $(COMPILE) router.cpp

$(SQLR)connection_router.$(LIBEXT): router.$(OBJ) router.cpp
	$(LTLINK) $(LINK) $(OUT)$@ router.$(OBJ) $(LDFLAGS) $(ROUTERCONNECTIONLIBS) $(MODLINKFLAGS)



sqlite.lo: sqlite.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SQLITECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

sqlite.obj: sqlite.cpp
	$(CXX) $(CXXFLAGS) $(SQLITECONNECTIONCPPFLAGS) $(COMPILE) sqlite.cpp

$(SQLR)connection_sqlite.$(LIBEXT): sqlite.$(OBJ) sqlite.cpp
	$(LTLINK) $(LINK) $(OUT)$@ sqlite.$(OBJ) $(LDFLAGS) $(SQLITECONNECTIONLIBS) $(MODLINKFLAGS)



sap.lo: sap.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SYBASECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

sap.obj: sap.cpp
	$(CXX) $(CXXFLAGS) $(SYBASECONNECTIONCPPFLAGS) $(COMPILE) sap.cpp

$(SQLR)connection_sap.$(LIBEXT): sap.$(OBJ) sap.cpp
	$(LTLINK) $(LINK) $(OUT)$@ sap.$(OBJ) $(LDFLAGS) $(SYBASECONNECTIONLIBS) $(MODLINKFLAGS)



install: $(INSTALLLIB)

installdll: $(CONNECTIONSINSTALLTARGETS)

installdll-db2:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_db2.$(LIBEXT) $(libexecdir)

installdll-firebird:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_firebird.$(LIBEXT) $(libexecdir)

installdll-freetds:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_freetds.$(LIBEXT) $(libexecdir)

installdll-informix:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_informix.$(LIBEXT) $(libexecdir)

installdll-mdbtools:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_mdbtools.$(LIBEXT) $(libexecdir)

installdll-mysql:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_mysql.$(LIBEXT) $(libexecdir)

installdll-odbc:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_odbc.$(LIBEXT) $(libexecdir)

installdll-oracle:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_oracle.$(LIBEXT) $(libexecdir)

installdll-postgresql:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_postgresql.$(LIBEXT) $(libexecdir)

installdll-router:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_router.$(LIBEXT) $(libexecdir)

installdll-sqlite:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_sqlite.$(LIBEXT) $(libexecdir)

installdll-sap:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_sap.$(LIBEXT) $(libexecdir)


installlib: $(INSTALLSHAREDLIB)

installsharedlib: $(CONNECTIONSINSTALLTARGETS)

installsharedlib-db2:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_db2.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_db2.a
	$(RM) $(libexecdir)/$(SQLR)connection_db2.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_db2.so so $(MODULESUFFIX)

installsharedlib-firebird:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_firebird.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_firebird.a
	$(RM) $(libexecdir)/$(SQLR)connection_firebird.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_firebird.so so $(MODULESUFFIX)

installsharedlib-freetds:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_freetds.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_freetds.a
	$(RM) $(libexecdir)/$(SQLR)connection_freetds.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_freetds.so so $(MODULESUFFIX)

installsharedlib-informix:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_informix.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_informix.a
	$(RM) $(libexecdir)/$(SQLR)connection_informix.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_informix.so so $(MODULESUFFIX)

installsharedlib-mdbtools:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_mdbtools.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_mdbtools.a
	$(RM) $(libexecdir)/$(SQLR)connection_mdbtools.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_mdbtools.so so $(MODULESUFFIX)

installsharedlib-mysql:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_mysql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_mysql.a
	$(RM) $(libexecdir)/$(SQLR)connection_mysql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_mysql.so so $(MODULESUFFIX)

installsharedlib-odbc:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_odbc.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_odbc.a
	$(RM) $(libexecdir)/$(SQLR)connection_odbc.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_odbc.so so $(MODULESUFFIX)

installsharedlib-oracle:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_oracle.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_oracle.a
	$(RM) $(libexecdir)/$(SQLR)connection_oracle.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_oracle.so so $(MODULESUFFIX)

installsharedlib-postgresql:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_postgresql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_postgresql.a
	$(RM) $(libexecdir)/$(SQLR)connection_postgresql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_postgresql.so so $(MODULESUFFIX)

installsharedlib-router:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_router.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_router.a
	$(RM) $(libexecdir)/$(SQLR)connection_router.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_router.so so $(MODULESUFFIX)

installsharedlib-sqlite:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_sqlite.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_sqlite.a
	$(RM) $(libexecdir)/$(SQLR)connection_sqlite.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_sqlite.so so $(MODULESUFFIX)

installsharedlib-sap:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)connection_sap.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)connection_sap.a
	$(RM) $(libexecdir)/$(SQLR)connection_sap.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)connection_sap.so so $(MODULESUFFIX)


uninstall:
	$(RM) $(libexecdir)/$(SQLR)connection_* $(libexecdir)/sqlrconnection_*
