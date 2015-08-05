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

LOBJS = bench.lo \
	main.lo \
	sqlrelaybench.lo \
	db2bench.lo \
	firebirdbench.lo \
	freetdsbench.lo \
	mysqlbench.lo \
	oraclebench.lo \
	postgresqlbench.lo \
	sqlitebench.lo \
	sybasebench.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(BENCHCPPFLAGS) $(COMPILE) $<

all: sqlr-bench

clean:
	$(LTCLEAN) $(RM) sqlr-bench$(EXE) *.lo *.o *.obj *.lib *.exp *.pdb *.manifest

sqlr-bench: $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@$(EXE) $(LOBJS) $(LDFLAGS) $(BENCHLIBS)
