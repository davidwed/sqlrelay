include ../../config.mk

.PHONY: all clean rebuild

all: lagotest

clean:
	$(RM) lagotest

rebuild: clean all

lagotest: lagotest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LAGOINCLUDES) $? -s -o $@ $(LAGOLIBS) $(EXTRALIBS) $(SOCKETLIBS)
