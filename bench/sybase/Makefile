include ../../config.mk

top_srcdir = ../..

all: sybasetest

clean:
	$(RM) sybasetest

rebuild: clean all

sybasetest: sybasetest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I./ $(SYBASEINCLUDES) $? -s -o $@ $(SYBASELIBS) $(EXTRALIBS) $(SOCKETLIB)
