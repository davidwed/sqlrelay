include ../../config.mk

top_srcdir = ../..

all: ora8test

clean:
	$(RM) ora8test

rebuild: clean all

ora8test: ora8test.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ORACLEINCLUDES) $? -s -o $@ $(ORACLELIBS) $(EXTRALIBS) $(SOCKETLIB)
