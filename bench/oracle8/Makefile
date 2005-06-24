include ../../config.mk

.PHONY: all clean rebuild

all: ora8test ora8clobtest

clean:
	$(RM) ora8test
	$(RM) ora8clobtest

rebuild: clean all

ora8test: ora8test.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(ORACLELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)

ora8clobtest: ora8clobtest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(ORACLELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)
