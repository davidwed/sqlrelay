include ../../config.mk

top_srcdir = ../..

all: postgresqltest

clean:
	$(RM) postgresqltest

rebuild: clean all

postgresqltest: postgresqltest.C
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(POSTGRESQLINCLUDES) $? -s -o $@ $(POSTGRESQLLIBS) $(EXTRALIBS) $(SOCKETLIB)
