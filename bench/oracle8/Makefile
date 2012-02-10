include ../../config.mk

.PHONY: all clean

all: ora8test ora8clobtest

clean:
	$(RM) ora8test
	$(RM) ora8clobtest

ora8test: ora8test.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(ORACLELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)

ora8clobtest: ora8clobtest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(ORACLELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)
