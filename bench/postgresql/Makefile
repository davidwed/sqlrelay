include ../../config.mk

.PHONY: all clean

all: postgresqltest

clean:
	$(RM) postgresqltest

postgresqltest: postgresqltest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(POSTGRESQLINCLUDES) $? -s -o $@ $(POSTGRESQLLIBS) $(EXTRALIBS) $(SOCKETLIBS)
