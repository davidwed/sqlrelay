include ../../config.mk

all: freetdstest

clean:
	$(RM) freetdstest

rebuild: clean all

freetdstest: freetdstest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I./ $(FREETDSINCLUDES) $? -s -o $@ $(FREETDSLIBS) $(EXTRALIBS) $(SOCKETLIB)
