include ../../config.mk

all: sqlrtest

clean:
	$(RM) sqlrtest

rebuild: clean all

sqlrtest: sqlrtest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES) $? -s -o $@ -L$(abs_srcdir)/src/api/c++/lib -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB)
