include ../../config.mk

.PHONY: all clean

all: mysqltest

clean:
	$(RM) mysqltest

mysqltest: mysqltest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(MYSQLINCLUDES) $? -s -o $@ $(MYSQLLIBS) $(EXTRALIBS) $(SOCKETLIBS)
