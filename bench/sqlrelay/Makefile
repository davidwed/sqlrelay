top_builddir = ../..

include ../../config.mk

all: sqlrtest

clean:
	$(RM) sqlrtest

rebuild: clean all

sqlrtest: sqlrtest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $? -s -o $@ -L$(top_builddir)/src/api/c++/lib -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS)
