include ../../config.mk

top_srcdir = ../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -DSYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" -I./ -I../../ -I../common -I../util -I../api/c++/include $(RUDIMENTSINCLUDES)
CONFIGFILELIBS = -L../util -lsqlrutil
LIBS = -L../api/c++/src/.libs -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB)

all: query fields sqlrsh

clean:
	$(RM) *.o
	$(RM) fields
	$(RM) query
	$(RM) sqlrsh

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

query: query.C query.o
	$(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ query.o $(CONFIGFILELIBS) $(LIBS)

fields: fields.C fields.o
	$(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ fields.o $(CONFIGFILELIBS) $(LIBS)

sqlrsh: sqlrsh.C sqlrsh.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.o $(CONFIGFILELIBS) $(LIBS) $(READLINELIB)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	$(INSTALL) query $(bindir)
	$(INSTALL) fields $(bindir)
	$(INSTALL) sqlrsh $(bindir)

uninstall:
	$(RM) $(bindir)/query
	$(RM) $(bindir)/fields
	$(RM) $(bindir)/sqlrsh
