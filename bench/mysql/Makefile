include ../../config.mk

top_srcdir = ../..

all: mysqltest

clean:
	$(RM) mysqltest

rebuild: clean all

mysqltest: mysqltest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(MYSQLINCLUDES) $? -s -o $@ $(MYSQLLIBS) $(EXTRALIBS) $(SOCKETLIB)
