include ../../config.mk

.PHONY: all clean rebuild

all: freetdstest

clean:
	$(RM) freetdstest

rebuild: clean all

freetdstest: freetdstest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -I./ $(FREETDSINCLUDES) $(RUDIMENTSINCLUDES) $? -s -o $@ $(FREETDSLIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIBS)
