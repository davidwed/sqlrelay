include ../../config.mk

all: interbasetest

clean:
	$(RM) interbasetest

rebuild: clean all

interbasetest: interbasetest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(INTERBASEINCLUDES) $? -s -o $@ $(INTERBASELIBS) $(EXTRALIBS) $(SOCKETLIB)
