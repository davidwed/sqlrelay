include ../../config.mk

all: ora7test

clean:
	$(RM) ora7test

rebuild: clean all

ora7test: ora7test.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $? -s -o $@ $(ORACLELIBS) $(EXTRALIBS) $(SOCKETLIBS)
