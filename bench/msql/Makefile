include ../../config.mk

.PHONY: all clean rebuild

all: msqltest

clean:
	$(RM) msqltest

rebuild: clean all

msqltest: msqltest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(MSQLINCLUDES) $? -s -o $@ $(MSQLLIBS) $(EXTRALIBS) $(SOCKETLIBS)
