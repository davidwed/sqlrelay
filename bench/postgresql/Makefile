include ../../config.mk

.PHONY: all clean rebuild

all: postgresqltest

clean:
	$(RM) postgresqltest

rebuild: clean all

postgresqltest: postgresqltest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(POSTGRESQLINCLUDES) $? -s -o $@ $(POSTGRESQLLIBS) $(EXTRALIBS) $(SOCKETLIBS)
