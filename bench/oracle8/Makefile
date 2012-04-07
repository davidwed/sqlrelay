include ../../config.mk

.PHONY: all clean

all: ora8test ora8clobtest inserttest

clean:
	$(RM) ora8test
	$(RM) ora8clobtest
	$(RM) inserttest

ora8test: ora8test.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(ORACLELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)

ora8clobtest: ora8clobtest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(ORACLELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)

inserttest: inserttest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(ORACLELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)
