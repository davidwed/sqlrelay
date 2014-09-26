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

SRCS += mysqlbench.cpp
LOBJS += mysqlbench.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: bench

clean:
	$(LIBTOOL) --mode=clean $(RM) bench *.lo *.o
	$(RM) bench

bench: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS)
