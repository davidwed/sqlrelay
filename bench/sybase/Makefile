include ../../config.mk

all: sybasetest

clean:
	$(RM) sybasetest

rebuild: clean all

sybasetest: sybasetest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I./ $(SYBASEINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(SYBASELIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)
