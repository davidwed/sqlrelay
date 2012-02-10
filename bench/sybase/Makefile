include ../../config.mk

.PHONY: all clean

all: sybasetest

clean:
	$(RM) sybasetest

sybasetest: sybasetest.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I./ $(SYBASEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(SYBASELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)
