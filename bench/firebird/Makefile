include ../../config.mk

.PHONY: all clean

all: firebirdtest

clean:
	$(RM) firebirdtest

firebirdtest: firebirdtest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(FIREBIRDINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(RUDIMENTSLIBS) $(FIREBIRDLIBS) $(EXTRALIBS) $(SOCKETLIBS)
