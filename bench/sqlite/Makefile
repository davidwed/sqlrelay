include ../../config.mk

.PHONY: all clean

all: sqlitetest

clean:
	$(RM) sqlitetest

sqlitetest: sqlitetest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(SQLITEINCLUDES) $? -s -o $@ $(SQLITELIBS) $(PTHREADLIB) $(EXTRALIBS) $(SOCKETLIBS)
