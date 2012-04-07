top_builddir = ../..

include ../../config.mk

.PHONY: all clean

all: sqlrtest inserttest

clean:
	$(RM) sqlrtest inserttest

sqlrtest: sqlrtest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $? -s -o $@ -L$(top_builddir)/src/api/c++/lib -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS)

inserttest: inserttest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $? -s -o $@ -L$(top_builddir)/src/api/c++/lib -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS)
