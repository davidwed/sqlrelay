include ../../config.mk

.PHONY: all clean rebuild

all: interbasetest

clean:
	$(RM) interbasetest

rebuild: clean all

interbasetest: interbasetest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(INTERBASEINCLUDES) $? -s -o $@ $(INTERBASELIBS) $(EXTRALIBS) $(SOCKETLIBS)
