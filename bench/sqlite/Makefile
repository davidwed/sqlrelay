include ../../config.mk

.PHONY: all clean rebuild

all: sqlitetest

clean:
	$(RM) sqlitetest

rebuild: clean all

sqlitetest: sqlitetest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(SQLITEINCLUDES) $? -s -o $@ $(SQLITELIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIBS)
