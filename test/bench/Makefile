top_builddir = ../..

include ../../config.mk

.PHONY: all clean

CPPFLAGS = \
	$(BASECPPFLAGS) \
	$(RUDIMENTSINCLUDES) \
	-I$(top_builddir)/src/api/c++/include \
	$(DB2INCLUDES) \
	$(FIREBIRDINCLUDES) \
	$(FREETDSINCLUDES) \
	$(MYSQLINCLUDES) \
	$(ORACLEINCLUDES) \
	$(POSTGRESQLINCLUDES) \
	$(SQLITEINCLUDES) \
	$(SYBASEINCLUDES)

LIBS = \
	-L$(top_builddir)/src/api/c++/src \
	-lsqlrclient \
	$(RUDIMENTSLIBS) \
	$(EXTRALIBS)

ifneq ($(strip $(DB2LIBSPATH)),)
	LIBS += -R $(DB2LIBSPATH)
endif
LIBS += $(DB2LIBS)

ifneq ($(strip $(FIREBIRDLIBSPATH)),)
	LIBS += -R $(FIREBIRDLIBSPATH)
endif
LIBS += $(FIREBIRDLIBS)

ifneq ($(strip $(FREETDSLIBSPATH)),)
	LIBS += -R $(FREETDSLIBSPATH)
endif
LIBS += $(FREETDSLIBS)

ifneq ($(strip $(MYSQLLIBSPATH)),)
	LIBS += -R $(MYSQLLIBSPATH)
endif
LIBS += $(MYSQLLIBS)

ifneq ($(strip $(ORACLELIBSPATH)),)
	LIBS += -R $(ORACLELIBSPATH)
endif
LIBS += $(ORACLELIBS)

ifneq ($(strip $(POSTGRESQLLIBSPATH)),)
	LIBS += -R $(POSTGRESQLLIBSPATH)
endif
LIBS += $(POSTGRESQLLIBS)

ifneq ($(strip $(SQLITELIBSPATH)),)
	LIBS += -R $(SQLITELIBSPATH)
endif
LIBS += $(SQLITELIBS)

ifneq ($(strip $(SYBASELIBSPATH)),)
	LIBS += -R $(SYBASELIBSPATH)
endif
LIBS += $(SYBASELIBS)

SRCS = bench.cpp main.cpp \
	sqlrelaybench.cpp \
	db2bench.cpp \
	firebirdbench.cpp \
	freetdsbench.cpp \
	mysqlbench.cpp \
	oraclebench.cpp \
	postgresqlbench.cpp \
	sqlitebench.cpp \
	sybasebench.cpp

LOBJS = bench.lo main.lo \
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
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-bench

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-bench *.lo *.o
	$(RM) sqlr-bench

sqlr-bench: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS)
