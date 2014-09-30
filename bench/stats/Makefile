top_builddir = ../..

include ../../config.mk

.PHONY: all clean

CPPFLAGS = \
	$(BASECPPFLAGS) \
	$(RUDIMENTSINCLUDES) \
	-I$(top_builddir)/src/api/c++/include \
	$(DB2INCLUDES) \
	$(MYSQLINCLUDES) \
	$(ORACLEINCLUDES) \
	$(POSTGRESQLINCLUDES) \
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

ifneq ($(strip $(SYBASELIBSPATH)),)
	LIBS += -R $(SYBASELIBSPATH)
endif
LIBS += $(SYBASELIBS)

SRCS = bench.cpp main.cpp
LOBJS = bench.lo main.lo

SRCS += sqlrelaybench.cpp
LOBJS += sqlrelaybench.lo

SRCS += db2bench.cpp
LOBJS += db2bench.lo

SRCS += mysqlbench.cpp
LOBJS += mysqlbench.lo

SRCS += oraclebench.cpp
LOBJS += oraclebench.lo

SRCS += postgresqlbench.cpp
LOBJS += postgresqlbench.lo

SRCS += sybasebench.cpp
LOBJS += sybasebench.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-bench

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-bench *.lo *.o
	$(RM) sqlr-bench

sqlr-bench: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS)
