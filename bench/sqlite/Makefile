include ../../config.mk

top_srcdir = ../..

all: sqlitetest

clean:
	$(RM) sqlitetest

rebuild: clean all

sqlitetest: sqlitetest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(SQLITEINCLUDES) $? -s -o $@ $(SQLITELIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB)
