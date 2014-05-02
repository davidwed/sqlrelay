top_builddir = ../..

include ../../config.mk

.PHONY: all clean

CPPFLAGS = \
	$(BASECPPFLAGS) \
	$(RUDIMENTSINCLUDES) \
	-I$(top_builddir)/src/api/c++/include

LIBS = \
	-L$(top_builddir)/src/api/c++/src \
	-lsqlrclient \
	$(RUDIMENTSLIBS) \
	$(EXTRALIBS)

SRCS = bench.cpp main.cpp
LOBJS = bench.lo main.lo

SRCS += sqlrelaybench.cpp
LOBJS += sqlrelaybench.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: bench

clean:
	$(LIBTOOL) --mode=clean $(RM) bench *.lo *.o
	$(RM) bench

bench: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS)
