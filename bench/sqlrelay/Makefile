top_builddir = ../..

include ../../config.mk

.PHONY: all clean

all: sqlrtest

clean:
	$(RM) sqlrtest

sqlrtest: sqlrtest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $? -s -o $@ -L$(top_builddir)/src/api/c++/lib -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS)
