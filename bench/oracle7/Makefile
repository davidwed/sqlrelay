include ../../config.mk

.PHONY: all clean rebuild

all: ora7test

clean:
	$(RM) ora7test

rebuild: clean all

ora7test: ora7test.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(ORACLELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)
