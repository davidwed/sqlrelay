top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(COMPILE) $<

all: sqlrbench_db2.$(LIBEXT) \
	sqlrbench_firebird.$(LIBEXT) \
	sqlrbench_freetds.$(LIBEXT) \
	sqlrbench_informix.$(LIBEXT) \
	sqlrbench_mysql.$(LIBEXT) \
	sqlrbench_oracle.$(LIBEXT) \
	sqlrbench_postgresql.$(LIBEXT) \
	sqlrbench_sap.$(LIBEXT) \
	sqlrbench_sqlite.$(LIBEXT) \
	sqlrbench_odbc.$(LIBEXT) \
	sqlrbench_sqlrelay.$(LIBEXT) \
	sqlr-bench

clean:
	$(LTCLEAN) $(RM) sqlr-bench$(EXE) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.png *.csv
	$(RMTREE) .libs

db2bench.lo: db2bench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(DB2INCLUDES) $(COMPILE) $< $(OUT)$@

db2bench.obj: db2bench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(DB2INCLUDES) $(COMPILE) db2bench.cpp

sqlrbench_db2.$(LIBEXT): db2bench.$(OBJ) db2bench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ db2bench.$(OBJ) $(LDFLAGS) $(DB2LIBS) $(MODLINKFLAGS)

firebirdbench.lo: firebirdbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(FIREBIRDINCLUDES) $(COMPILE) $< $(OUT)$@

firebirdbench.obj: firebirdbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(FIREBIRDINCLUDES) $(COMPILE) firebirdbench.cpp

sqlrbench_firebird.$(LIBEXT): firebirdbench.$(OBJ) firebirdbench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ firebirdbench.$(OBJ) $(LDFLAGS) $(FIREBIRDLIBS) $(MODLINKFLAGS)

freetdsbench.lo: freetdsbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(FREETDSINCLUDES) $(COMPILE) $< $(OUT)$@

freetdsbench.obj: freetdsbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(FREETDSINCLUDES) $(COMPILE) freetdsbench.cpp

sqlrbench_freetds.$(LIBEXT): freetdsbench.$(OBJ) freetdsbench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ freetdsbench.$(OBJ) $(LDFLAGS) $(FREETDSLIBS) $(MODLINKFLAGS)

informixbench.lo: informixbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(INFORMIXINCLUDES) $(COMPILE) $< $(OUT)$@

informixbench.obj: informixbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(INFORMIXINCLUDES) $(COMPILE) informixbench.cpp

sqlrbench_informix.$(LIBEXT): informixbench.$(OBJ) informixbench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ informixbench.$(OBJ) $(LDFLAGS) $(INFORMIXLIBS) $(MODLINKFLAGS)

mysqlbench.lo: mysqlbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) $< $(OUT)$@

mysqlbench.obj: mysqlbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) mysqlbench.cpp

sqlrbench_mysql.$(LIBEXT): mysqlbench.$(OBJ) mysqlbench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ mysqlbench.$(OBJ) $(LDFLAGS) $(MYSQLLIBS) $(MODLINKFLAGS)

oraclebench.lo: oraclebench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(ORACLEINCLUDES) $(COMPILE) $< $(OUT)$@

oraclebench.obj: oraclebench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(ORACLEINCLUDES) $(COMPILE) oraclebench.cpp

sqlrbench_oracle.$(LIBEXT): oraclebench.$(OBJ) oraclebench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ oraclebench.$(OBJ) $(LDFLAGS) $(ORACLELIBS) $(MODLINKFLAGS)

postgresqlbench.lo: postgresqlbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) $< $(OUT)$@

postgresqlbench.obj: postgresqlbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) postgresqlbench.cpp

sqlrbench_postgresql.$(LIBEXT): postgresqlbench.$(OBJ) postgresqlbench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ postgresqlbench.$(OBJ) $(LDFLAGS) $(POSTGRESQLLIBS) $(MODLINKFLAGS)

sqlitebench.lo: sqlitebench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(SQLITEINCLUDES) $(COMPILE) $< $(OUT)$@

sqlitebench.obj: sqlitebench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(SQLITEINCLUDES) $(COMPILE) sqlitebench.cpp

sqlrbench_sqlite.$(LIBEXT): sqlitebench.$(OBJ) sqlitebench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ sqlitebench.$(OBJ) $(LDFLAGS) $(SQLITELIBS) $(MODLINKFLAGS)

odbcbench.lo: odbcbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(ODBCINCLUDES) $(COMPILE) $< $(OUT)$@

odbcbench.obj: odbcbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(ODBCINCLUDES) $(COMPILE) odbcbench.cpp

sqlrbench_odbc.$(LIBEXT): odbcbench.$(OBJ) odbcbench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ odbcbench.$(OBJ) $(LDFLAGS) $(ODBCLIBS) $(MODLINKFLAGS)

sqlrelaybench.lo: sqlrelaybench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(COMPILE) $< $(OUT)$@

sqlrelaybench.obj: sqlrelaybench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(COMPILE) sqlrelaybench.cpp

sqlrbench_sqlrelay.$(LIBEXT): sqlrelaybench.$(OBJ) sqlrelaybench.cpp
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelaybench.$(OBJ) $(LDFLAGS) -L$(top_builddir)/src/api/c++ -l$(SQLR)client $(RUDIMENTSLIBS) $(EXTRALIBS) $(MODLINKFLAGS)

sapbench.lo: sapbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(SYBASEINCLUDES) $(COMPILE) $< $(OUT)$@

sapbench.obj: sapbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(SYBASEINCLUDES) $(COMPILE) sapbench.cpp

sqlrbench_sap.$(LIBEXT): sapbench.$(OBJ) sapbench.cpp
			 $(LTLINK) $(LINK) $(OUT)$@ sapbench.$(OBJ) $(LDFLAGS) $(SYBASELIBS) $(MODLINKFLAGS)

sqlr-bench: sqlrbench.cpp sqlrbench.$(OBJ) sqlr-bench.cpp sqlr-bench.$(OBJ) 
	$(LTLINK) $(LINK) $(OUT)$@$(EXE) sqlrbench.$(OBJ) sqlr-bench.$(OBJ) $(LDFLAGS) -export-dynamic $(BENCHLIBS)
