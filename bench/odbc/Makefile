include ../../config.mk

top_srcdir = ../..

all: odbctest

clean:
	$(RM) odbctest

rebuild: clean all

odbctest: odbctest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(ODBCINCLUDES) $? -s -o $@ $(ODBCLIBS) $(EXTRALIBS) $(SOCKETLIB)
