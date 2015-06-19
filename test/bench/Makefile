top_builddir = ../..

include ../../config.mk

CPPFLAGS = \
	$(BASECPPFLAGS) \
	$(RUDIMENTSINCLUDES) \
	-I$(top_builddir)/src/api/c++ \
	$(DB2INCLUDES) \
	$(FIREBIRDINCLUDES) \
	$(FREETDSINCLUDES) \
	$(MYSQLINCLUDES) \
	$(ORACLEINCLUDES) \
	$(POSTGRESQLINCLUDES) \
	$(SQLITEINCLUDES) \
	$(SYBASEINCLUDES)

LIBS = \
	-L$(top_builddir)/src/api/c++ \
	-lsqlrclient \
	$(RUDIMENTSLIBS) \
	$(EXTRALIBS) \
	$(DB2LIBS) \
	$(FIREBIRDLIBS) \
	$(FREETDSLIBS) \
	$(MYSQLLIBS) \
	$(ORACLELIBS) \
	$(POSTGRESQLLIBS) \
	$(SQLITELIBS) \
	$(SYBASELIBS)

#ifneq ($(strip $(DB2LIBSPATH)),)
	#LIBS += -R $(DB2LIBSPATH)
#endif

#ifneq ($(strip $(FIREBIRDLIBSPATH)),)
	#LIBS += -R $(FIREBIRDLIBSPATH)
#endif

#ifneq ($(strip $(FREETDSLIBSPATH)),)
	#LIBS += -R $(FREETDSLIBSPATH)
#endif

#ifneq ($(strip $(MYSQLLIBSPATH)),)
	#LIBS += -R $(MYSQLLIBSPATH)
#endif

#ifneq ($(strip $(ORACLELIBSPATH)),)
	#LIBS += -R $(ORACLELIBSPATH)
#endif

#ifneq ($(strip $(POSTGRESQLLIBSPATH)),)
	#LIBS += -R $(POSTGRESQLLIBSPATH)
#endif

#ifneq ($(strip $(SQLITELIBSPATH)),)
	#LIBS += -R $(SQLITELIBSPATH)
#endif

#ifneq ($(strip $(SYBASELIBSPATH)),)
	#LIBS += -R $(SYBASELIBSPATH)
#endif

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
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: sqlr-bench

clean:
	$(LTCLEAN) $(RM) sqlr-bench$(EXE) *.lo *.o *.obj *.lib *.exp *.pdb *.manifest

sqlr-bench: $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@$(EXE) $(LOBJS) $(LDFLAGS) $(LIBS)
