top_builddir = ../..

include ../../config.mk

.PHONY: all clean

CPPFLAGS = \
	$(BASECPPFLAGS) \
	$(RUDIMENTSINCLUDES) \
	-I$(top_builddir)/src/api/c++/include \
	$(ORACLEINCLUDES) \
	$(MYSQLINCLUDES)

LIBS = \
	-L$(top_builddir)/src/api/c++/src \
	-lsqlrclient \
	$(RUDIMENTSLIBS) \
	$(EXTRALIBS)

ifneq ($(strip $(ORACLELIBSPATH)),)
	LIBS += -R $(ORACLELIBSPATH)
endif
LIBS += $(ORACLELIBS)

ifneq ($(strip $(MYSQLLIBSPATH)),)
	LIBS += -R $(MYSQLLIBSPATH)
endif
LIBS += $(MYSQLLIBS)

SRCS = bench.cpp main.cpp
LOBJS = bench.lo main.lo

SRCS += sqlrelaybench.cpp
LOBJS += sqlrelaybench.lo

SRCS += oraclebench.cpp
LOBJS += oraclebench.lo

SRCS += sybasebench.cpp
LOBJS += sybasebench.lo

SRCS += mysqlbench.cpp
LOBJS += mysqlbench.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-bench

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-bench *.lo *.o
	$(RM) sqlr-bench

sqlr-bench: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS)
