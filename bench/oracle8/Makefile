include ../../config.mk

.PHONY: all clean rebuild

all: ora8test

clean:
	$(RM) ora8test

rebuild: clean all

ora8test: ora8test.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(ORACLELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)
