include ../../config.mk

top_srcdir = ../..

all: sqlrtest

clean:
	$(RM) sqlrtest

rebuild: clean all

sqlrtest: sqlrtest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I../../src/api/c++/include $(RUDIMENTSINCLUDES) $? -s -o $@ -L../../src/api/c++/lib -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB)
