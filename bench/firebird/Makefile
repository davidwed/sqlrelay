include ../../config.mk

.PHONY: all clean rebuild

all: firebirdtest

clean:
	$(RM) firebirdtest

rebuild: clean all

firebirdtest: firebirdtest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(FIREBIRDINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(RUDIMENTSLIBS) $(FIREBIRDLIBS) $(EXTRALIBS) $(SOCKETLIBS)
