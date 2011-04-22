include ../../config.mk

.PHONY: all clean

all: db2test

clean:
	$(RM) db2test

db2test: db2test.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(DB2INCLUDES) $? -s -o $@ $(DB2LIBS) $(EXTRALIBS) $(SOCKETLIBS)
