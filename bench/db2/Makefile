include ../../config.mk

.PHONY: all clean rebuild

all: db2test

clean:
	$(RM) db2test

rebuild: clean all

db2test: db2test.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(DB2INCLUDES) $? -s -o $@ $(DB2LIBS) $(EXTRALIBS) $(SOCKETLIBS)
