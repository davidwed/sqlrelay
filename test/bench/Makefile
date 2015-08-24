top_builddir = ../..

include ../../config.mk

SRCS = bench.cpp \
	main.cpp \
	sqlrelaybench.cpp \
	db2bench.cpp \
	firebirdbench.cpp \
	freetdsbench.cpp \
	mysqlbench.cpp \
	oraclebench.cpp \
	postgresqlbench.cpp \
	sqlitebench.cpp \
	sybasebench.cpp

LOBJS = bench.$(OBJ) \
	main.$(OBJ) \
	sqlrelaybench.$(OBJ) \
	db2bench.$(OBJ) \
	firebirdbench.$(OBJ) \
	freetdsbench.$(OBJ) \
	mysqlbench.$(OBJ) \
	oraclebench.$(OBJ) \
	postgresqlbench.$(OBJ) \
	sqlitebench.$(OBJ) \
	sybasebench.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(COMPILE) $<

db2bench.lo: db2bench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(DB2INCLUDES) $(COMPILE) $< $(OUT)$@

db2bench.obj: db2bench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(DB2INCLUDES) $(COMPILE) db2bench.cpp

firebirdbench.lo: firebirdbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(FIREBIRDINCLUDES) $(COMPILE) $< $(OUT)$@

firebirdbench.obj: firebirdbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(FIREBIRDINCLUDES) $(COMPILE) firebirdbench.cpp

freetdsbench.lo: freetdsbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(FREETDSINCLUDES) $(COMPILE) $< $(OUT)$@

freetdsbench.obj: freetdsbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(FREETDSINCLUDES) $(COMPILE) freetdsbench.cpp

mysqlbench.lo: mysqlbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) $< $(OUT)$@

mysqlbench.obj: mysqlbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) mysqlbench.cpp

oraclebench.lo: oraclebench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(ORACLEINCLUDES) $(COMPILE) $< $(OUT)$@

oraclebench.obj: oraclebench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(ORACLEINCLUDES) $(COMPILE) oraclebench.cpp

postgresqlbench.lo: postgresqlbench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) $< $(OUT)$@

postgresqlbench.obj: postgresqlbench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) postgresqlbench.cpp

sqlitebench.lo: sqlitebench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(SQLITEINCLUDES) $(COMPILE) $< $(OUT)$@

sqlitebench.obj: sqlitebench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(SQLITEINCLUDES) $(COMPILE) sqlitebench.cpp

sybasebench.lo: sybasebench.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(SYBASEINCLUDES) $(COMPILE) $< $(OUT)$@

sybasebench.obj: sybasebench.cpp
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(SYBASEINCLUDES) $(COMPILE) sybasebench.cpp

all: sqlr-bench

clean:
	$(LTCLEAN) $(RM) sqlr-bench$(EXE) *.lo *.o *.obj *.lib *.exp *.pdb *.manifest

sqlr-bench: $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@$(EXE) $(LOBJS) $(LDFLAGS) $(BENCHLIBS)
