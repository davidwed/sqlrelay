include ../../config.mk

.PHONY: all clean

all: odbctest

clean:
	$(RM) odbctest

odbctest: odbctest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ODBCINCLUDES) $? -s -o $@ $(ODBCLIBS) $(EXTRALIBS) $(SOCKETLIBS)
