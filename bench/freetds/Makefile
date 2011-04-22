include ../../config.mk

.PHONY: all clean

all: freetdstest

clean:
	$(RM) freetdstest

freetdstest: freetdstest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I./ $(FREETDSINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(FREETDSLIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)
