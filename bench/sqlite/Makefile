include ../../config.mk

.PHONY: all clean

all: sqlitetest

clean:
	$(RM) sqlitetest

sqlitetest: sqlitetest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(SQLITEINCLUDES) $? -s -o $@ $(SQLITELIBS) $(PTHREADLIB) $(EXTRALIBS) $(SOCKETLIBS)
