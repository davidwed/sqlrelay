include ../../config.mk

all: msqltest

clean:
	$(RM) msqltest

rebuild: clean all

msqltest: msqltest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(MSQLINCLUDES) $? -s -o $@ $(MSQLLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB)
