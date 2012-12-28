top_builddir = ../..

include ../../config.mk

.PHONY: all clean

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(ODBCINCLUDES)
LIBS = $(RUDIMENTSLIBS) $(ODBCLIBS) $(LIBEFENCE)

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all:
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) oracle
endif

oracle: oracle.cpp oracle.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle.lo $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o oracle
	$(RM) cachefile* sqlnet.log
	$(RMTREE) .libs
