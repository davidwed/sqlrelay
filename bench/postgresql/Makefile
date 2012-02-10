include ../../config.mk

.PHONY: all clean

all: postgresqltest

clean:
	$(RM) postgresqltest

postgresqltest: postgresqltest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(POSTGRESQLINCLUDES) $? -s -o $@ $(POSTGRESQLLIBS) $(EXTRALIBS) $(SOCKETLIBS)
